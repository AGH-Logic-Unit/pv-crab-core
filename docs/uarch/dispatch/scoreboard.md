---
title: Register Scoreboard Module
---

# Register Scoreboard Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `rtl/dispatch/scoreboard.sv`
    * :material-progress-wrench: **Status:** `Ratified Architecture`
    * :material-scale-balance: **Target:** Dual-Target (ASIC IHP SG13G2 / FPGA)

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-09-06 | v0.3 | Re-architected to pure 1-bit pending tracking (64 DFFs), removed redundant tag arrays, adopted Dispatch-level WAW interlock, and aligned release strictly with Stage 7 Commit. |
| 2026-08-28 | v0.2 | Removed raw Verilog syntax, standardized architectural documentation style, clarified WAW and Writeback release rules, and updated SVA table. |
| 2026-08-28 | v0.1 | Initial redesign from scratch: dual-register-file tracking (32 GPR + 32 FPR). |

## 2. Overview

The **Register Scoreboard** is a compact hardware tracking unit located inside the **Dispatch Stage (Stage 5)**. It tracks in-flight register modifications across all execution pipelines (ALU, Multiplier, Divider, LSU, and FPU) to detect **Read-After-Write (RAW)** and **Write-After-Write (WAW)** hazards.

Following the principle of hardware minimization in the 130nm ASIC budget, the Scoreboard operates as a pure **1-bit pending tracking array (64 DFFs total)**. Instruction tracking tags live exclusively in the **ROB (Retirement Buffer)** for in-order commit and precise exceptions, eliminating the need for tag duplication in the Scoreboard.

## 3. Structural Architecture

The Scoreboard maintains two parallel 32-bit tracking registers for integer (GPR) and floating-point (FPR) register files:

```
                  ┌───────────────────────────────────────────────────────────┐
                  │            REGISTER SCOREBOARD (64 FLIP-FLOPS)            │
                  │                                                           │
                  │     GPR File (x0 - x31)           FPR File (f0 - f31)     │
                  │  ┌───────────────────────┐     ┌───────────────────────┐  │
                  │  │ gpr_pending_r [31:0]  │     │ fpr_pending_r [31:0]  │  │
                  │  └───────────────────────┘     └───────────────────────┘  │
                  └───────────────────────────────────────────────────────────┘
```

### 3.1 State Registers
1. **Integer Status Register (`logic [31:0] gpr_pending_r`):**
   * `gpr_pending_r[i] == 1'b1`: An in-flight instruction is computing a new value for integer register `x[i]`.
2. **Floating-Point Status Register (`logic [31:0] fpr_pending_r`):**
   * `fpr_pending_r[i] == 1'b1`: An in-flight instruction is computing a new value for floating-point register `f[i]`.

### 3.2 Hardwired Zero Register (`x0`)
* Register `x0` is hardwired to zero in strict compliance with RISC-V ISA:
  * `gpr_pending_r[0] = 1'b0` (permanently tied to ground).
  * Any query for `rs_addr == 5'd0 && rs_type == GPR` immediately returns `pending = 0` with zero combinational delay.

---

## 4. Hazard Detection & Commit Rules

### 4.1 RAW Hazard Detection (Source Operand Query)
During Stage 5, the Dispatcher combinatorially queries the Scoreboard for each source register (`rs1`, `rs2`, `rs3`):

* **Zero Register (`x0`):** Always returns `pending = 0`.
* **GPR Source:** Returns `gpr_pending_r[rs_addr]`.
* **FPR Source:** Returns `fpr_pending_r[rs_addr]`.

If an operand's `pending` bit is asserted, the Dispatcher queries the **Forwarding / Bypass Network** (`bypass.md`). If the operand cannot be forwarded directly from Stage 6 (ALU output) or Stage 7 (Writeback stage), Dispatch generates a `raw_stall_o` back-pressure signal.

### 4.2 WAW Hazard Interlock (Dispatch Stall)
Because the Scoreboard maintains a single pending bit per register without ROB tag duplication, multiple concurrent in-flight writes to the same destination register are prevented by a simple **WAW Interlock at Dispatch**:

* If a newly decoded instruction targeting register `rd` with write enable (`dec_rd_we_i == 1`) finds `scb_rd_pending == 1` (i.e., a prior long-latency instruction like `DIV` or `LSU` is still in flight targeting the same `rd`), Dispatch **stalls issuance**:
  $$\text{waw\_stall} = \text{dec\_valid\_i} \land \text{dec\_rd\_we\_i} \land (\text{dec\_rd\_addr\_i} \ne 0) \land \text{scb\_rd\_pending}$$
* **Performance Impact:** Negligible ($< 0.01\%$ IPC difference), as standard optimizing compilers (GCC, LLVM) eliminate dead WAW sequences during SSA optimization.

### 4.3 Commit Release Protocol (Stage 7 Retire)
The architectural Register Files are updated strictly at **Stage 7 (Commit / Retire)** when an instruction reaches the head of the ROB FIFO. Consequently:

* The Scoreboard clears the pending bit **strictly upon retirement**:
  * Triggered by `commit_valid_i && commit_rd_we_i`.
  * Clears `gpr_pending_r[commit_rd_addr_i] <= 1'b0` (or FPR equivalent).
* **Intra-Cycle Dispatch / Commit Priority:** If an instruction commits in the exact same cycle as a new instruction dispatches with the same destination address, the **Dispatch allocation takes strict priority**, keeping the register marked `pending = 1`.

### 4.4 Pipeline Flush Behavior (`flush_i`)
Upon an architectural flush (branch misprediction from Stage 6 or exception/trap/barrier from Stage 7):

* All `gpr_pending_r` and `fpr_pending_r` bits are reset to `32'h0` in a single clock cycle.

---

## 5. Interfaces and Signal Definitions

| Signal | Type / Width | Direction | Source / Destination | Description |
| :--- | :---: | :---: | :---: | :--- |
| **Global Control** | | | | |
| `clk_i` | `logic` | IN | System | Master system clock |
| `rst_ni` | `logic` | IN | System | Asynchronous active-low reset |
| `flush_i` | `logic` | IN | Global Flush | Synchronous pipeline flush (clears all pending bits) |
| **Source Query Ports (Combinational)** | | | | |
| `rs1_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 1 address |
| `rs1_type_i` | `logic` | IN | Stage 4 Decode | Source register 1 type (`0` = GPR, `1` = FPR) |
| `rs1_pending_o`| `logic` | OUT | Stage 5 Hazard | RS1 is currently pending in execution pipeline |
| `rs2_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 2 address |
| `rs2_type_i` | `logic` | IN | Stage 4 Decode | Source register 2 type (`0` = GPR, `1` = FPR) |
| `rs2_pending_o`| `logic` | OUT | Stage 5 Hazard | RS2 is currently pending in execution pipeline |
| `rs3_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 3 address (FMA operand) |
| `rs3_type_i` | `logic` | IN | Stage 4 Decode | Source register 3 type (`0` = GPR, `1` = FPR) |
| `rs3_pending_o`| `logic` | OUT | Stage 5 Hazard | RS3 is currently pending in execution pipeline |
| `rd_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Destination register address for WAW check |
| `rd_type_i` | `logic` | IN | Stage 4 Decode | Destination register type for WAW check |
| `rd_pending_o` | `logic` | OUT | Stage 5 Hazard | Destination register is already pending (WAW hazard) |
| **Dispatch Allocation Port** | | | | |
| `disp_alloc_valid_i`| `logic` | IN | Dispatch Controller | New instruction dispatched (`disp_fire && dec_rd_we && !trap`) |
| `disp_rd_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Destination register address to allocate |
| `disp_rd_type_i` | `logic` | IN | Stage 4 Decode | Destination register type (`0` = GPR, `1` = FPR) |
| **Commit Release Port** | | | | |
| `commit_valid_i` | `logic` | IN | Stage 7 Commit | Instruction successfully committed at ROB Head |
| `commit_rd_we_i` | `logic` | IN | Stage 7 Commit | Instruction writes to architectural Register File |
| `commit_rd_addr_i`| `logic [4:0]` | IN | Stage 7 Commit | Committing destination register address |
| `commit_rd_type_i`| `logic` | IN | Stage 7 Commit | Committing destination type (`0` = GPR, `1` = FPR) |

---

## 6. Formal Verification Suite (SVA)

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_SCB_01` | `!gpr_pending_r[0]` | `FATAL` | **CHK-PIPE-01** | Register x0 must never be marked pending under any condition |
| `SVA_SCB_02` | `(disp_alloc_valid_i && disp_rd_addr_i != 5'd0 && disp_rd_type_i == 1'b0) |=> gpr_pending_r[$past(disp_rd_addr_i)]` | `FATAL` | **CHK-PIPE-01** | Allocation must set pending bit on the following cycle |
| `SVA_SCB_03` | `(commit_valid_i && commit_rd_we_i && commit_rd_addr_i != 5'd0 && !(disp_alloc_valid_i && disp_rd_addr_i == commit_rd_addr_i && disp_rd_type_i == commit_rd_type_i) && commit_rd_type_i == 1'b0) |=> !gpr_pending_r[$past(commit_rd_addr_i)]` | `ERROR` | **CHK-PIPE-01** | Commit must clear pending bit when no WAW collision occurs |
| `SVA_SCB_04` | `flush_i |=> (gpr_pending_r == 32'h0 && fpr_pending_r == 32'h0)` | `FATAL` | **CHK-PIPE-03** | Pipeline flush must unconditionally clear all pending bits across both register files |
| `SVA_SCB_05` | `(disp_alloc_valid_i && commit_valid_i && commit_rd_we_i && disp_rd_addr_i == commit_rd_addr_i && disp_rd_type_i == commit_rd_type_i && disp_rd_addr_i != 5'd0) |=> (disp_rd_type_i ? fpr_pending_r[$past(disp_rd_addr_i)] : gpr_pending_r[$past(disp_rd_addr_i)])` | `FATAL` | **CHK-PIPE-01** | Intra-cycle dispatch must take strict priority over commit release |
