---
title: ALU
---

# Arithmetic Logical Unit (ALU)

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-04-26 | v0.1 | First draft |

## 2. Overview

ALU module is a stateless, purely **combinational** arithmetic-logical unit for RISC-V 64b architecture located in execution (EX) stage of Pulsar-V pipeline. It performs *arithmetical*, *logical* and *shift* operations on provided inputs.

!!! info "Characteristic"
    This module is **purely combinational** and has no state or buffer registers, therefore it does not perform any decisions on its own.

## 3. Architectural Requirements

Module is required to fully implement following RISC-V Unprivileged ISA specification:

- **RV64I:** `ADD`, `SUB`, `SLL`, `SLT`, `SLTU`, `XOR`, `SRL`, `SRA`, `OR`, `AND` with their immediate and 32bit (`W`) variants
- **Data width compliance:** Support for 64 bits wide operands
- **Jump resolution:** Compute branch comparison conditions

## 4. Interfaces

| Signal | Width | Direction | Description |
| :--- | :---: | :---: | :--- |
| `operand_a_i` | 64 | IN | First operand (typically RS1 or PC) |
| `operand_b_i` | 64 | IN | Second operand (typically RS2 or IMM) |
| `operator_i` | 5 | IN | Operation code provided by Decoder |
| `result_o` | 64 | OUT | Result of selected operation |
| `branch_valid_o` | 1 | OUT | Flag indicating that branch comparison condition was met `1` or not `0` |

## 5. Functional Description

### 5.1 Adder
For area optimization, both `ADD` and `SUB` utilize a single 64-bit adder.

* For `SUB` operation, `operand_b_i` is inverted and input carry driven **high**

### 5.2 Barrel Shifter
Implemented as a 64-bit barrel shifter, it supports arithmetic right (`SRA`) and both logical right (`SRL`) and logical left (`SLL`) shifts.

### 5.3 Comparator
ALU generates logical output for `SLT`/`SLTU` operations. Additionally it produces binary flag for branch instructions (`BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU`) by comparing operands and checking flags: zero, negative and carry-out. This unit can be implemented by reusing other units.

### 5.4 Operations

| Operation | Lambda | Definition|
| :--- | :---: | :--- |
| ADD | $\lambda A.(\lambda B.(A + B))$ | $Result = A + B$ |
| SUB | $\lambda A.(\lambda B.(A - B))$ | $Result = A - B$ |
| AND | $\lambda A.(\lambda B.(A\ \&\ B))$ | $Result = A \ \& \ B$ |
| OR | $\lambda A.(\lambda B.(A\ \|\ B))$ | $Result = A \ \| \ B$ |
| XOR | $\lambda A.(\lambda B.(A \ \oplus \ B))$ | $Result = A \ \oplus \ B$ |
| SLL | $\lambda A.(\lambda B.(A \ll B[5:0]))$ | $Result = A \ll B[5:0]$ |
| SRL | $\lambda A.(\lambda B.(A \gg_L B[5:0]))$ | $Result = A \gg_L B[5:0]$ |
| SRA | $\lambda A.(\lambda B.(A \gg_A B[5:0]))$ | $Result = A \gg_A B[5:0]$ |
| SLT | $\lambda A.(\lambda B.(A <_s B ? 1 : 0))$ | $Result = (A <_s B) ? 1 : 0$ |
| SLTU | $\lambda A.(\lambda B.(A <_u B ? 1 : 0))$ | $Result = (A <_u B) ? 1 : 0$ |

## 6. Timing and Performance

Since module is combinational, it has 0-cycle latency.

!!! warning "Critical Path"
    Probable critical path: <br/>
    `operand_b_i` -> `Barrel Shifter` -> `Result Mux` -> `result_o` <br/>
    or <br/>
    `operand_b_i` -> `Adder` -> `Result Mux` -> `result_o` <br/>
    It is advised to analyze delays on both during synthesis.

## 7. Verification

ALU verification is based on constrained random tests:

1. **Unit Tests:** Verification of all ISA instructions with edge cases
2. **Reference Model:** RTL results must be identical to the programmatic reference model (C, C++ or Python)
3. **Coverage:** Full 100% coverage of `operator_i` codes and preferably all operand bits
