---
title: Arithmetic Logic Unit
---

# Arithmetic Logic Unit Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-09-06 | v0.2 | Clarified pipeline framing: combinational ALU datapath operates in Stage 6 framed by physical registers R_DISP_EX (Stage 5/6) and R_EX_WB (Stage 6/7), yielding 1-cycle execution latency with 0-stall EX-to-EX forwarding. |
| 2026-04-26 | v0.1 | First draft |

## 2. Overview

The ALU module is a stateless, **combinational** arithmetic-logical unit for the RISC-V 64-bit architecture located in the execution (EX / Stage 6) stage of the Crab Core pipeline. It performs *arithmetic*, *logical*, and *shift* operations on the provided inputs.

!!! info "Pipeline Framing & Characteristics"
    While the ALU core datapath is **purely combinational**, in the 7-stage Crab Core it is framed by two physical pipeline registers: **`R_DISP_EX`** (Stage 5/6 boundary) on the input, and **`R_EX_WB`** (Stage 6/7 boundary) on the output. This provides clean clock isolation, a **1-cycle execution latency**, and supports **0-stall EX-to-EX forwarding (Tap 1 bypass)** directly back to the Dispatch stage.

## 3. Architectural Requirements

The module is required to fully implement the following RISC-V Unprivileged ISA specifications:

- **RV64I:** `ADD`, `SUB`, `SLL`, `SLT`, `SLTU`, `XOR`, `SRL`, `SRA`, `OR`, `AND` along with their immediate and 32-bit (`W`) variants (`ADDW`, `SUBW`, `SLLW`, `SRLW`, `SRAW`).
- **Data width compliance:** Support for 64-bit wide operands.
- **Jump resolution:** Compute branch comparison conditions.

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `operand_a_i` | `logic` | 64 | IN | First operand (typically RS1 or PC) |
| `operand_b_i` | `logic` | 64 | IN | Second operand (typically RS2 or IMM) |
| `operator_i` | `logic` | 5 | IN | Operation code provided by Decoder |
| `result_o` | `logic` | 64 | OUT | Result of selected operation |
| `zero_o` | `logic` | 1 | OUT | Flag indicating result being zero |

## 5. Functional Description

### 5.1 Adder
For area optimization, both `ADD` and `SUB` (and their `W` variants) utilize a single 64-bit adder.

* For `SUB` operations, `operand_b_i` is inverted and the input carry is driven **high**.

### 5.2 Barrel Shifter
Implemented as a 64-bit barrel shifter, it supports arithmetic right (`SRA`/`SRAW`) and both logical right (`SRL`/`SRLW`) and logical left (`SLL`/`SLLW`) shifts.

### 5.3 Comparator
The ALU generates logical comparison outputs for `SLT`/`SLTU` operations by reusing the subtraction datapath. Branch instruction conditions (`BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU`) map directly onto these comparison mechanisms:

* `BEQ` / `BNE`: Evaluated via equality comparison / `zero_o` from subtraction.
* `BLT` / `BGE`: Evaluated via signed comparison (`SLT` result bit `result_o[0]`).
* `BLTU` / `BGEU`: Evaluated via unsigned comparison (`SLTU` result bit `result_o[0]`).

### 5.4 Operations

| Operation | Lambda | Definition|
| :--- | :---: | :--- |
| ADD | $\lambda A.(\lambda B.(A + B))$ | $Result = A + B$ |
| SUB | $\lambda A.(\lambda B.(A - B))$ | $Result = A - B$ |
| AND | $\lambda A.(\lambda B.(A\ \&\ B))$ | $Result = A \ \& \ B$ |
| OR | $\lambda A.(\lambda B.(A\ \|\ B))$ | $Result = A \ \ \| \ B$ |
| XOR | $\lambda A.(\lambda B.(A \ \oplus \ B))$ | $Result = A \ \oplus \ B$ |
| SLL | $\lambda A.(\lambda B.(A \ll B[5:0]))$ | $Result = A \ll B[5:0]$ |
| SRL | $\lambda A.(\lambda B.(A \gg_L B[5:0]))$ | $Result = A \gg_L B[5:0]$ |
| SRA | $\lambda A.(\lambda B.(A \gg_A B[5:0]))$ | $Result = A \gg_A B[5:0]$ |
| SLT | $\lambda A.(\lambda B.(A <_s B ? 1 : 0))$ | $Result = (A <_s B) ? 1 : 0$ |
| SLTU | $\lambda A.(\lambda B.(A <_u B ? 1 : 0))$ | $Result = (A <_u B) ? 1 : 0$ |

### 5.5 32-bit Operations (W-variants)
For instructions with the `W` suffix (`ADDW`, `SUBW`, `SLLW`, `SRLW`, `SRAW`):

* The operations are performed on the lower 32 bits of the inputs.
* The 32-bit result is sign-extended to 64 bits before being driven on `result_o`.
* For shift operations (`SLLW`, `SRLW`, `SRAW`), the shift amount is truncated to 5 bits (`B[4:0]`).

## 6. Timing and Performance

The ALU datapath executes within **1 clock cycle** (Stage 6), reading from the `R_DISP_EX` pipeline register and driving the `R_EX_WB` pipeline register.

!!! note "0-Stall EX-to-EX Forwarding (Tap 1 Bypass)"
    To avoid pipeline stalls on back-to-back dependent ALU instructions, the raw combinational output `result_o` is routed directly back to the Dispatcher's operand multiplexers via **Tap 1 EX-to-EX forwarding** (prior to the `R_DISP_EX` input register). This ensures continuous execution with $IPC = 1.0$ and 0 stall penalty for ALU-to-ALU dependencies.

!!! info "Stalling & Buffer Isolation"
    Because the ALU core datapath is combinational, it does not implement internal state machines. Pipeline stalling and holding of ALU results are managed externally by the pipeline registers (`R_DISP_EX`, `R_EX_WB`), the Dispatch controller, and the Stage 7 Writeback Arbiter.

!!! warning "Critical Path"
    Probable critical paths: <br/>
    `operand_b_i` -> `Barrel Shifter` -> `Result Mux` -> `result_o` <br/>
    or <br/>
    `operand_b_i` -> `Adder` -> `Result Mux` -> `result_o` <br/>
    It is advised to analyze delays on both paths during synthesis.

## 7. Verification

ALU verification is based on constrained random tests:

1. **Unit Tests:** Verification of all ISA instructions with edge cases.
2. **Reference Model:** RTL results must be identical to the programmatic reference model (C, C++, or Python).
3. **Coverage:** Full 100% coverage of `operator_i` codes and all operand bits.
