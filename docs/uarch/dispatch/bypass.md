---
title: Forwarding & Bypass Network
---

# Forwarding & Bypass Network

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `rtl/dispatch/bypass_network.sv`
    * :material-progress-wrench: **Status:** `Ratified Architecture`
    * :material-scale-balance: **Target:** Dual-Target (ASIC IHP SG13G2 / FPGA)

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-09-06 | v0.3 | Refactored forwarding to direct register address matching (`rd == rs`), eliminating redundant tag matching. Clarified pipeline register boundaries (`R_DISP_EX` and `R_EX_WB`) providing 1-cycle execution with 0-stall EX-to-EX forwarding. |
| 2026-08-28 | v0.2 | Corrected Register File commit semantics: clarified that Register Files are written strictly in-order at Stage 7 Commit. |
| 2026-08-28 | v0.1 | Initial ratified 2-tap bypass specification. |

## 2. Overview

The **Forwarding & Bypass Network** is the combinational operand routing subsystem of the **Dispatch Stage (Stage 5)**. It allows in-flight dependent instructions to receive newly computed data directly from execution and writeback stages without stalling the pipeline for in-order Register File writeback.

### Crucial Architectural Invariants
* **In-Order Commit Point:** The architectural Register Files (GPR and FPR) are written **strictly in-order at Stage 7 (Retire / Commit)** when an instruction reaches the head of the ROB FIFO and is confirmed exception-free.
* **Pipeline Framing of Execution:** The ALU execution lane in Stage 6 is bounded by two physical pipeline registers:
  * **`R_DISP_EX`** (Stage 5/6 boundary): latches dispatched operands, operation code, and destination register address.
  * **`R_EX_WB`** (Stage 6/7 boundary): latches execution results and destination register metadata before entering Writeback.
* **Address-Based Matching:** Forwarding matches the 5-bit register destination address (`rd_addr`) against source register addresses (`rs1`, `rs2`, `rs3`), eliminating the area and power overhead of tag comparison in the bypass network.

```
                           [ STAGE 5: DISPATCH ]
                                     │
         ┌───────────────────────────┼───────────────────────────┐
         ▼                           ▼                           ▼
    Operand A Mux               Operand B Mux               Operand C Mux
    (RS1 / 64-bit)              (RS2 / 64-bit)              (RS3 / 64-bit)
         ▲                           ▲                           ▲
         │                           │                           │
  ┌──────┴───────────────────────────┴───────────────────────────┴──────┐
  │                      COMBINATIONAL BYPASS NETWORK                   │
  │                                                                     │
  │   [Tap 1: EX-to-EX Forwarding]      [Tap 2: WB-to-EX Forwarding]    │
  └──────────────▲──────────────────────────────────▲───────────────────┘
                 │                                  │
                 │ (Combinational ALU output)       │ (Latched in R_EX_WB)
          [ Stage 6: ALU ]                 [ Stage 7: Writeback ]
          (from R_DISP_EX)                 (from R_EX_WB)
```

## 3. Physical Bypass Taps

The bypass network consists of exactly two physical forwarding taps:

### 3.1 Tap 1: `alu_bypass` (EX-to-EX Forwarding)
* **Connected Source:** Direct combinational output of `alu.sv` in Stage 6 (computed from inputs latched in `R_DISP_EX`).
* **Execution Latency:** **1 clock cycle** (latched in `R_DISP_EX` at clock edge $T$, calculated during cycle $T+1$).
* **Forwarding Penalty:** **0 stall cycles** ($IPC = 1.0$). Back-to-back dependent ALU instructions execute continuously because the result of cycle $T+1$ is forwarded combinatorially into `R_DISP_EX` for cycle $T+2$.

### 3.2 Tap 2: `wb_bypass` (WB-to-EX Forwarding)
* **Connected Source:** Output of the `R_EX_WB` pipeline register and Writeback Arbiter in Stage 7.
* **Mechanism:** Delivers results of instructions that completed execution in the previous cycle (including 1-cycle ALU results parked before retirement, or multi-cycle units like Multiplier, Divider, FPU, or LSU hits).
* **Forwarding Penalty:** **0 stall cycles** for instructions dependent on the immediately retiring operation.

---

## 4. Hardware Interfaces (`bypass_network.sv`)

| Signal | Type / Width | Direction | Connected Source | Description |
| :--- | :---: | :---: | :--- | :--- |
| **Tap 1: Stage 6 ALU Forwarding** | | | | |
| `ex_alu_valid_i` | `logic` | IN | Stage 6 `R_DISP_EX` | ALU lane has a valid active operation |
| `ex_alu_rd_we_i` | `logic` | IN | Stage 6 `R_DISP_EX` | Destination register write enable |
| `ex_alu_rd_type_i` | `logic` | IN | Stage 6 `R_DISP_EX` | Destination type (`0` = GPR, `1` = FPR) |
| `ex_alu_rd_addr_i` | `logic [4:0]` | IN | Stage 6 `R_DISP_EX` | Destination register address |
| `ex_alu_data_i` | `logic [63:0]` | IN | Stage 6 ALU Output | 64-bit calculation result from combinational ALU |
| **Tap 2: Stage 7 Writeback Forwarding** | | | | |
| `wb_valid_i` | `logic` | IN | Stage 7 `R_EX_WB` | Writeback stage holds a valid completing operation |
| `wb_rd_we_i` | `logic` | IN | Stage 7 `R_EX_WB` | Destination register write enable |
| `wb_rd_type_i` | `logic` | IN | Stage 7 `R_EX_WB` | Destination type (`0` = GPR, `1` = FPR) |
| `wb_rd_addr_i` | `logic [4:0]` | IN | Stage 7 `R_EX_WB` | Destination register address |
| `wb_data_i` | `logic [63:0]` | IN | Stage 7 `R_EX_WB` | 64-bit result entering Writeback / ROB Result Array |
| **Scoreboard Tracking Inputs** | | | | |
| `scb_rs1_pending_i` | `logic` | IN | Scoreboard | RS1 is currently pending in execution pipeline |
| `scb_rs2_pending_i` | `logic` | IN | Scoreboard | RS2 is currently pending in execution pipeline |
| `scb_rs3_pending_i` | `logic` | IN | Scoreboard | RS3 is currently pending in execution pipeline |
| **Register File Inputs** | | | | |
| `rf_rs1_data_i` | `logic [63:0]` | IN | Register File | Asynchronous read data for RS1 |
| `rf_rs2_data_i` | `logic [63:0]` | IN | Register File | Asynchronous read data for RS2 |
| `rf_rs3_data_i` | `logic [63:0]` | IN | Register File | Asynchronous read data for RS3 |
| **Decoded Instruction Inputs** | | | | |
| `dec_rs1_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 1 address |
| `dec_rs2_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 2 address |
| `dec_rs3_addr_i` | `logic [4:0]` | IN | Stage 4 Decode | Source register 3 address |
| `dec_rs1_type_i` | `logic` | IN | Stage 4 Decode | RS1 register file selector (`0` = GPR, `1` = FPR) |
| `dec_rs2_type_i` | `logic` | IN | Stage 4 Decode | RS2 register file selector (`0` = GPR, `1` = FPR) |
| `dec_rs3_type_i` | `logic` | IN | Stage 4 Decode | RS3 register file selector (`0` = GPR, `1` = FPR) |
| `dec_rs1_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS1 |
| `dec_rs2_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS2 |
| `dec_rs3_req_i` | `logic` | IN | Stage 4 Decode | Instruction reads RS3 |
| **Resolved Outputs to Stage 6** | | | | |
| `resolved_operand_a_o`| `logic [63:0]` | OUT | Stage 6 `R_DISP_EX` | Final multiplexed 64-bit operand A |
| `resolved_operand_b_o`| `logic [63:0]` | OUT | Stage 6 `R_DISP_EX` | Final multiplexed 64-bit operand B |
| `resolved_operand_c_o`| `logic [63:0]` | OUT | Stage 6 `R_DISP_EX` | Final multiplexed 64-bit operand C |
| `raw_stall_o` | `logic` | OUT | Dispatch Controller | Asserted when an operand is pending and missing from both bypass taps |

---

## 5. Operand Selection & Priority Logic

For each operand lane $X \in \{\text{rs1}, \text{rs2}, \text{rs3}\}$, operand resolution follows strict architectural priority:

### Hit Generation:
```systemverilog
wire hit_alu_X = dec_rsX_req_i && ex_alu_valid_i && ex_alu_rd_we_i &&
                 (ex_alu_rd_type_i == dec_rsX_type_i) &&
                 (ex_alu_rd_addr_i != 5'd0) &&
                 (ex_alu_rd_addr_i == dec_rsX_addr_i);

wire hit_wb_X  = dec_rsX_req_i && wb_valid_i && wb_rd_we_i &&
                 (wb_rd_type_i == dec_rsX_type_i) &&
                 (wb_rd_addr_i != 5'd0) &&
                 (wb_rd_addr_i == dec_rsX_addr_i);
```

### Multiplexing Priority:
1. **Hardwired Zero ($x0$):** If source address is `0` and type is `GPR`, output is unconditionally tied to `64'h0`.
2. **Priority 1 (Tap 1 ALU EX-to-EX):** If `hit_alu_X == 1`, operand is selected from `ex_alu_data_i`.
3. **Priority 2 (Tap 2 WB-to-EX):** If `hit_wb_X == 1` and `!hit_alu_X`, operand is selected from `wb_data_i`.
4. **Default (Register File):** If `!scb_rsX_pending_i`, operand is read directly from the architectural Register File (`rf_rsX_data_i`).

### RAW Hazard Stall Condition
If an operand is marked pending by the Scoreboard but is not currently present on either active forwarding tap, Dispatch asserts `raw_stall_o = 1`:

$$\text{raw\_stall\_o} = \bigvee_{X \in \{1, 2, 3\}} \Big( \text{dec\_rs}_X\text{\_req} \land \text{scb\_rs}_X\text{\_pending} \land !(\text{hit\_alu}_X \lor \text{hit\_wb}_X) \Big)$$

---

## 6. Formal Verification Suite (SVA)

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_BYP_01` | `(dec_rs1_addr_i == 5'd0 && dec_rs1_type_i == 1'b0) |-> (resolved_operand_a_o == 64'h0)` | `FATAL` | **CHK-PIPE-01** | Register x0 must unconditionally evaluate to 0 regardless of bypass tap assertions |
| `SVA_BYP_02` | `(hit_alu_1) |-> (resolved_operand_a_o == ex_alu_data_i)` | `FATAL` | **CHK-PIPE-01** | When Tap 1 matches RS1 address, operand A must strictly select ex_alu_data_i |
| `SVA_BYP_03` | `(hit_wb_1 && !hit_alu_1) |-> (resolved_operand_a_o == wb_data_i)` | `FATAL` | **CHK-PIPE-01** | When Tap 2 matches RS1 address and no ALU hit, operand A must strictly select wb_data_i |
| `SVA_BYP_04` | `(dec_rs1_req_i && scb_rs1_pending_i && !hit_alu_1 && !hit_wb_1) |-> (raw_stall_o == 1'b1)` | `FATAL` | **CHK-PIPE-01** | Unresolved RAW hazard on required operand RS1 must assert raw_stall_o immediately |
| `SVA_BYP_05` | `(hit_alu_1 && hit_wb_1) |-> (resolved_operand_a_o == ex_alu_data_i)` | `ERROR` | **CHK-PIPE-01** | In case of dual tap hit on same address, Tap 1 (younger EX output) must take strict priority |
