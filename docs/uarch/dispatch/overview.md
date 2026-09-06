---
title: Dispatch Stage Overview
---

# Dispatch Stage (Stage 5)

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `rtl/dispatch/dispatch_unit.sv`
    * :material-progress-wrench: **Status:** `Ratified Architecture`
    * :material-scale-balance: **Target:** Dual-Target (ASIC IHP SG13G2 / FPGA Distributed LUT-RAM)

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-09-06 | v0.3 | Refactored Scoreboard to pure 1-bit pending tracking (64 DFFs), adopted address-based forwarding, framed Stage 6 with physical registers `R_DISP_EX` and `R_EX_WB`, added `issue_imm_o [63:0]`, widened `issue_op_o` to 6 bits, added `issue_rm_o [2:0]`, and added barrier serialization locking. |
| 2026-08-28 | v0.2 | Standardized architectural documentation style, clarified In-Order Commit boundaries, and updated signal tables. |
| 2026-08-28 | v0.1 | Initial redesign from scratch: unified 7-stage pipeline, 7-lane execution routing with dynamic back-pressure, and SVA tables. |

## 2. Overview

The **Dispatch Stage (Stage 5)** is the central hazard resolution, dependency tracking, and execution routing hub of the 7-stage in-order Crab Core pipeline. Positioned directly between **Decode (Stage 4)** and **Execute (Stage 6)**, it coordinates the flow of decoded micro-operations (`uOps`) to their specific execution lanes while enforcing strict in-order program semantics.

```
                  [ STAGE 4: DECODE ]
                            │
                            │ Decoded uOp {pc, op_class, rd, rs1, rs2, rs3, imm, payload}
                            ▼
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                                STAGE 5: DISPATCH UNIT                                  │
│                                                                                        │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────────────────────┐  │
│  │ Register File Read      │  │ Bypass / Forwarding Multiplexing (bypass.md)        │  │
│  │ (Async / Combinational: │  │ - Tap 1: EX-to-EX Forwarding (from R_DISP_EX ALU)   │  │
│  │  32 GPRs + 32 FPRs)     │  │ - Tap 2: WB-to-EX Forwarding (from R_EX_WB Stage 7) │  │
│  └────────────┬────────────┘  └──────────────────────────┬──────────────────────────┘  │
│               │                                          │                             │
│               └────────────────────┬─────────────────────┘                             │
│                                    ▼                                                   │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐  │
│  │ RAW & WAW Hazard Interlock & Scoreboard Verification (scoreboard.md)             │  │
│  │ - Pure 1-bit pending check for rs1, rs2, rs3 and destination rd (WAW interlock)  │  │
│  │ - Deasserts disp_ready_o = 0 to stall Decode & Fetch FIFO if unresolved          │  │
│  └─────────────────────────────────┬────────────────────────────────────────────────┘  │
│                                    ▼                                                   │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐  │
│  │ Structural Hazard & Barrier Check (Dynamic Target FU Ready & Serializing Lock)   │  │
│  │ - Evaluates fu_ready_i for target execution lane (ALU, MUL, DIV, LSU, FPU)       │  │
│  │ - Locks dispatch on SFENCE.VMA, FENCE.I, or critical CSRs until retirement       │  │
│  └─────────────────────────────────┬────────────────────────────────────────────────┘  │
│                                    ▼                                                   │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐  │
│  │ ROB Slot Allocation (Metadata Array Write)                                       │  │
│  │ - Checks ROB occupancy (Stalls if rob_full == 1)                                 │  │
│  │ - Allocates Tag = tail_ptr [3:0]                                                 │  │
│  │ - Writes {pc, rd, rd_type, rd_we, op_class, is_rvc} into Metadata Array          │  │
│  └─────────────────────────────────┬────────────────────────────────────────────────┘  │
│                                    ▼                                                   │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐  │
│  │ Issue Demultiplexer & Operand Assembly                                           │  │
│  │ ├── Routes operands {A, B, C, imm, rm, op, payload} to target execution lane     │  │
│  │ └── Latches into Stage 5/6 Pipeline Register R_DISP_EX                           │  │
│  └─────────────────────────────────┬────────────────────────────────────────────────┘  │
└────────────────────────────────────┼───────────────────────────────────────────────────┘
                                     │
                                     ▼
                      ══════════════════════════════
                      ║    R_DISP_EX (Stage 5/6)   ║
                      ══════════════════════════════
                                     │
                                     ▼
                      [ STAGE 6: EXECUTE PIPELINES ]
```

## 3. Architectural Design Decisions

### 3.1 Pipeline Register Boundaries (`R_DISP_EX` & `R_EX_WB`)
To guarantee timing closure at 100 MHz in IHP SG13G2 130nm, execution lanes are strictly isolated by physical pipeline registers:
* **`R_DISP_EX` (Stage 5/6 boundary):** Latches resolved source operands, immediate offsets, operation opcodes, and target lane control signals at the end of Dispatch.
* **`R_EX_WB` (Stage 6/7 boundary):** Latches execution results and writeback metadata from execution lanes before arbitration into the ROB Result Array.
* **Register File (GPR/FPR):** Combinational (asynchronous) read in Stage 5; written **strictly in-order at Stage 7 Commit** from the ROB Head.

### 3.2 Dynamic Structural Hazard & Back-Pressure Network
Every functional execution lane asserts a dynamic `ready` signal back to Dispatch:

| Target Functional Unit | Unit Classification | Readiness Condition (`target_fu_ready`) |
| :--- | :--- | :--- |
| **`FU_ALU`** | 1-cycle latency / Control | `alu_ready_i` (Deasserted when Writeback Arbiter blocks ALU output) |
| **`FU_MUL`** | 3-Cycle Pipelined Multiplier | `mul_ready_i` (Stage EX1 input register is free) |
| **`FU_DIV`** | 16-Cycle Non-Pipelined Divider | `div_ready_i` (Divider FSM is in `IDLE` state) |
| **`FU_LSU`** | Load-Store / AMO Unit | `lsu_ready_i` (STB not full and no active AMO holding lock) |
| **`FU_FMA`** | 3-Cycle Pipelined FMA | `fma_ready_i` (Stage EX1 input register is free) |
| **`FU_FDIV`** | 15/30-Cycle Non-Pipelined Div/Sqrt | `fdiv_ready_i` (fDiv FSM is in `IDLE` state) |
| **`FU_FMISC`** | 1-2 Cycle Conversions / Moves | `fmisc_ready_i` (Unit input register is ready) |

### 3.3 RAW & WAW Hazard Resolution (Pure 1-bit Scoreboard)
* **Scoreboard Structure:** Pure 1-bit pending flags (`32'h0` GPR + `32'h0` FPR = 64 DFFs total). No tag storage is duplicated in the Scoreboard.
* **RAW Hazards:** If an operand's pending bit is set, Dispatch inspects Tap 1 (`R_DISP_EX` ALU output) and Tap 2 (`R_EX_WB` writeback output) using direct 5-bit register address comparison (`rd_addr == rs_addr`). If an address matches with write enable, the value is forwarded with 0 stall cycles. Otherwise, Dispatch asserts `raw_stall_o = 1`.
* **WAW Hazards:** If a new instruction has `rd_we == 1` and `scb_rd_pending == 1` (destination register is currently targeted by an older in-flight multi-cycle instruction), Dispatch stalls until the older instruction commits (`waw_stall = 1`).
* **Hardwired Zero (`x0`):** Register `x0` is hardwired to 0, never pending, and never stalls.

### 3.4 ROB Allocation (Split-Array Architecture)
* 16-entry Retirement Buffer (`RB_SIZE = 16`, `TAG_WIDTH = 4`).
* If `rob_full_i == 1`, Dispatch stalls (`disp_ready_o = 0`).
* Otherwise, Dispatch allocates `tag = tail_ptr [3:0]` and writes static metadata to `ROB_Metadata_Array[tag]`: `{pc, rd_addr, rd_type, rd_we, op_class, is_rvc}` (`rb_static_meta_t`).

### 3.5 Serialization of Barrier & System Instructions
* **Serializing Classes:** `SFENCE.VMA`, `FENCE.I`, and system-critical CSR writes (`satp`, PMP configuration) require execution ordering guarantees.
* **Serializing Lock:** When an instruction of class `OP_CLASS_SYS` or `OP_CLASS_FENCE` is issued, Dispatch asserts an internal `serializing_lock_q` register and deasserts `disp_ready_o = 0`.
* **Release:** Dispatch resumes issuing only after the serializing instruction commits at Stage 7 (indicated by `commit_barrier_ack_i` or pipeline `flush_i`).

---

## 4. Interfaces and Signal Definitions

| Signal | Type / Width | Direction | Source / Destination | Description |
| :--- | :---: | :---: | :---: | :--- |
| **Global Control** | | | | |
| `clk_i` | `logic` | IN | System | Master system clock |
| `rst_ni` | `logic` | IN | System | Asynchronous active-low reset |
| `flush_i` | `logic` | IN | Global Pipeline Flush | Synchronous pipeline flush (Branch mispredict or Trap) |
| **Decode Interface (Input)** | | | | |
| `dec_valid_i` | `logic` | IN | Stage 4 Decode | Valid decoded instruction |
| `dec_pc_i` | `logic [63:0]` | IN | Stage 4 Decode | Program Counter |
| `dec_is_rvc_i` | `logic` | IN | Stage 4 Decode | 16-bit compressed instruction indicator |
| `dec_op_class_i` | `op_class_t` | IN | Stage 4 Decode | Instruction class |
| `dec_target_fu_i`| `fu_sel_t` | IN | Stage 4 Decode | Target functional unit selector |
| `dec_op_i` | `logic [5:0]` | IN | Stage 4 Decode | 6-bit operation code (accommodates all ALU/LSU/AMO ops) |
| `dec_rs1_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 1 address |
| `dec_rs2_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 2 address |
| `dec_rs3_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 3 address (FMA) |
| `dec_rs1_type_i` | `logic` | IN | Stage 4 Decode | `0` = GPR, `1` = FPR |
| `dec_rs2_type_i` | `logic` | IN | Stage 4 Decode | `0` = GPR, `1` = FPR |
| `dec_rs3_type_i` | `logic` | IN | Stage 4 Decode | `0` = GPR, `1` = FPR |
| `dec_rs1_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS1 |
| `dec_rs2_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS2 |
| `dec_rs3_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS3 |
| `dec_rd_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Destination register address |
| `dec_rd_type_i` | `logic` | IN | Stage 4 Decode | `0` = GPR, `1` = FPR |
| `dec_rd_we_i` | `logic` | IN | Stage 4 Decode | Destination register write enable |
| `dec_imm_i` | `logic [63:0]` | IN | Stage 4 Decode | Decoded sign-extended immediate value |
| `dec_rm_i` | `logic [2:0]` | IN | Stage 4 Decode | Static floating-point rounding mode from instruction |
| `dec_payload_i` | `logic [13:0]` | IN | Stage 4 Decode | Auxiliary metadata payload (CSR/SYS) |
| `disp_ready_o` | `logic` | OUT | Stage 4 Decode | Dispatcher ready (back-pressure to Decode/Fetch) |
| **CSR Dynamic State** | | | | |
| `csr_frm_i` | `logic [2:0]` | IN | Stage 7 CSR Unit | Dynamic floating-point rounding mode (`fcsr.frm`) |
| `commit_barrier_ack_i`| `logic`| IN | Stage 7 Commit | Pulse clearing barrier serializing lock |
| **Scoreboard Interface** | | | | |
| `scb_rs1_pending_i`| `logic` | IN | Scoreboard | RS1 busy flag |
| `scb_rs2_pending_i`| `logic` | IN | Scoreboard | RS2 busy flag |
| `scb_rs3_pending_i`| `logic` | IN | Scoreboard | RS3 busy flag |
| `scb_rd_pending_i` | `logic` | IN | Scoreboard | RD busy flag (WAW check) |
| `scb_alloc_valid_o`| `logic` | OUT | Scoreboard | Mark `rd` as busy |
| `scb_alloc_rd_o` | `logic [4:0]` | OUT | Scoreboard | Destination register address |
| `scb_alloc_type_o`| `logic` | OUT | Scoreboard | Destination register type |
| **Bypass Network Interface** | | | | |
| `ex_alu_valid_i` | `logic` | IN | Stage 6 ALU | Tap 1: Stage 6 ALU operation valid |
| `ex_alu_rd_we_i` | `logic` | IN | Stage 6 ALU | Tap 1: ALU write enable |
| `ex_alu_rd_type_i`| `logic` | IN | Stage 6 ALU | Tap 1: ALU destination type |
| `ex_alu_rd_addr_i`| `logic [4:0]` | IN | Stage 6 ALU | Tap 1: ALU destination register address |
| `ex_alu_data_i` | `logic [63:0]`| IN | Stage 6 ALU | Tap 1: 64-bit combinational calculation result |
| `wb_valid_i` | `logic` | IN | Stage 7 Writeback | Tap 2: Stage 7 writeback valid |
| `wb_rd_we_i` | `logic` | IN | Stage 7 Writeback | Tap 2: Writeback write enable |
| `wb_rd_type_i` | `logic` | IN | Stage 7 Writeback | Tap 2: Writeback destination type |
| `wb_rd_addr_i` | `logic [4:0]` | IN | Stage 7 Writeback | Tap 2: Writeback destination register address |
| `wb_data_i` | `logic [63:0]`| IN | Stage 7 Writeback | Tap 2: 64-bit result entering Writeback / ROB |
| **ROB Allocation Interface** | | | | |
| `rob_full_i` | `logic` | IN | ROB Controller | ROB FIFO occupancy is full |
| `rob_alloc_valid_o`| `logic` | OUT | ROB Controller | Allocate new slot in ROB |
| `rob_alloc_tag_i` | `logic [3:0]` | IN | ROB Controller | Assigned tag (`tail_ptr`) |
| `rob_meta_we_o` | `logic` | OUT | ROB Metadata | Write strobe to Metadata Array |
| `rob_meta_tag_o` | `logic [3:0]` | OUT | ROB Metadata | Address/Tag for Metadata Array |
| `rob_meta_data_o` | `rb_static_meta_t`| OUT | ROB Metadata | Packet `{pc, rd_addr, rd_type, rd_we, op_class, is_rvc}` |
| **Execution Handshake (Stage 6 Inputs)** | | | | |
| `alu_ready_i` | `logic` | IN | ALU / WB Arbiter | ALU lane ready |
| `mul_ready_i` | `logic` | IN | Multiplier | Multiplier EX1 ready |
| `div_ready_i` | `logic` | IN | Divider | Divider FSM ready (IDLE) |
| `lsu_ready_i` | `logic` | IN | LSU / STB | LSU ready (STB not full, no lock) |
| `fma_ready_i` | `logic` | IN | fMulAdd | FMA pipeline ready |
| `fdiv_ready_i` | `logic` | IN | fDiv | fDiv FSM ready (IDLE) |
| `fmisc_ready_i`| `logic` | IN | fMisc | fMisc unit ready |
| **Issue Interface (Stage 6 `R_DISP_EX` Inputs)** | | | | |
| `issue_valid_o` | `logic` | OUT | Stage 6 `R_DISP_EX`| Instruction issued to execution lane |
| `issue_target_fu_o`| `fu_sel_t` | OUT | Stage 6 `R_DISP_EX`| Target execution unit selector |
| `issue_tag_o` | `logic [3:0]` | OUT | Stage 6 `R_DISP_EX`| Tracking tag assigned to operation |
| `issue_op_class_o` | `op_class_t` | OUT | Stage 6 `R_DISP_EX`| Instruction execution class |
| `issue_op_o` | `logic [5:0]` | OUT | Stage 6 `R_DISP_EX`| 6-bit operation opcode |
| `issue_operand_a_o`| `logic [63:0]`| OUT | Stage 6 `R_DISP_EX`| Resolved operand A (RS1 or PC) |
| `issue_operand_b_o`| `logic [63:0]`| OUT | Stage 6 `R_DISP_EX`| Resolved operand B (RS2 or Immediate) |
| `issue_operand_c_o`| `logic [63:0]`| OUT | Stage 6 `R_DISP_EX`| Resolved operand C (RS3 for FMA) |
| `issue_imm_o` | `logic [63:0]`| OUT | Stage 6 `R_DISP_EX`| 64-bit sign-extended immediate (for LSU AGU / Stores) |
| `issue_rm_o` | `logic [2:0]` | OUT | Stage 6 `R_DISP_EX`| Effective rounding mode for FPU (`dyn ? frm : static`) |
| `issue_payload_o` | `logic [13:0]` | OUT | Stage 6 `R_DISP_EX`| Pass-through payload (CSR/SYS) |

---

## 5. Formal Verification Suite (SVA)

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_DISP_01` | `(dec_valid_i && dec_rs1_addr_i == 5'd0 && dec_rs1_type_i == 1'b0) |-> !scb_rs1_pending_i` | `FATAL` | **CHK-PIPE-01** | Register x0 must never be marked pending and always resolve to 0 |
| `SVA_DISP_02` | `(rob_full_i) |-> (disp_ready_o == 1'b0 && issue_valid_o == 1'b0)` | `FATAL` | **CHK-PIPE-01** | Dispatch must immediately stall when ROB FIFO is full |
| `SVA_DISP_03` | `(!target_fu_ready) |-> (disp_ready_o == 1'b0 && issue_valid_o == 1'b0)` | `FATAL` | **CHK-PIPE-01** | Dispatch must stall when target functional execution lane is busy |
| `SVA_DISP_04` | `(dec_valid_i && dec_rd_we_i && scb_rd_pending_i && dec_rd_addr_i != 5'd0) |-> (disp_ready_o == 1'b0 && issue_valid_o == 1'b0)` | `FATAL` | **CHK-PIPE-01** | WAW hazard interlock must stall dispatch when destination register is busy |
| `SVA_DISP_05` | `(flush_i) |=> (issue_valid_o == 1'b0)` | `ERROR` | **CHK-PIPE-03** | Pipeline flush must immediately suppress instruction issue in next cycle |
| `SVA_DISP_06` | `(dec_valid_i && disp_ready_o) |-> (issue_tag_o == rob_alloc_tag_i)` | `ERROR` | **CHK-PIPE-01** | Issue tracking tag must strictly match the newly allocated ROB tag |
| `SVA_DISP_07` | `(serializing_lock_q) |-> (disp_ready_o == 1'b0 && issue_valid_o == 1'b0)` | `FATAL` | **CHK-CSR-10** | Barrier serializing lock must halt dispatch until retirement |
