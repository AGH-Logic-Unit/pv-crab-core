---
title: Floating-Point FMA, Multiplier & Adder
---

# Floating-Point FMA, Multiplier & Adder Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Added flush_i port and active pipeline reset, and renamed headers to exe_headers_t |
| 2026-05-23 | v0.1 | Initial draft template |

## 2. Overview

The **fMulAdd** module is a pipelined floating-point execution unit in the Pulsar-V core. It performs floating-point addition, subtraction, multiplication, and Fused Multiply-Accumulate (FMA) operations for single and double-precision operands. By leveraging a Fused Multiply-Accumulate (FMA) architecture, it computes $A \times B + C$ with a single rounding step, avoiding intermediate precision loss. Standard addition and subtraction operations are mapped directly onto the FMA datapath.

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

- **RV32F / RV64F (Single-Precision):** `FADD.S`, `FSUB.S`, `FMUL.S`, `FMADD.S`, `FMSUB.S`, `FNMADD.S`, `FNMSUB.S`.
- **RV32D / RV64D (Double-Precision):** `FADD.D`, `FSUB.D`, `FMUL.D`, `FMADD.D`, `FMSUB.D`, `FNMADD.D`, `FNMSUB.D`.
- **Rounding Modes:** Support for all RISC-V rounding modes (`RNE`, `RTZ`, `RDN`, `RUP`, `RMM`, dynamic rounding via `frm`).
- **IEEE 754-2008 Compliance:** Pipelined exception detection, rounding, and generation of floating-point status flags (NV, DZ, OF, UF, NX).

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `flush_i` | `logic` | 1 | IN | Pipeline flush signal (clears all FMA pipeline registers) |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid FMA instruction |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating unit can accept new inputs |
| `disp_headers_i`| `exe_headers_t`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | First floating-point operand (multiplicand, or first term for FADD/FSUB) |
| `operand_b_i` | `logic` | 64 | IN | Second floating-point operand (multiplier, or set to 1.0 for FADD/FSUB) |
| `operand_c_i` | `logic` | 64 | IN | Third floating-point operand (addend for FMA, second term for FADD/FSUB, tied to 0 for FMUL) |
| `operator_i` | `logic` | 4 | IN | Operation encoding (FADD, FSUB, FMUL, FMADD, FMSUB, etc. in SP and DP) |
| `rm_i` | `logic` | 3 | IN | Rounding mode from floating-point control register `frm` |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Calculated floating-point result (NaN-boxed to 64 bits for single-precision) |
| `wb_fflags_o` | `logic` | 5 | OUT | Floating-point exception flags: `{NV, DZ, OF, UF, NX}` |
| `wb_headers_o` | `exe_headers_t`| - | OUT | Output execution stage header metadata to Writeback Buffer |

## 5. Functional Description

### 5.1 FMA Operations
All operations are mapped to the core Fused Multiply-Accumulate logic:

- `FADD`: Computes $A \times 1.0 + C$ (where $A$ is the first input operand and $C$ is the second input operand, with the multiplier operand internally forced or provided as $1.0$).
- `FSUB`: Computes $A \times 1.0 - C$ (where $A$ is the first input operand and $C$ is the second input operand, with the multiplier operand internally forced or provided as $1.0$).
- `FMUL`: Computes $A \times B + 0$.
- `FMADD`: Computes $A \times B + C$.
- `FMSUB`: Computes $A \times B - C$.
- `FNMADD`: Computes $-(A \times B + C)$.
- `FNMSUB`: Computes $-(A \times B - C)$.

### 5.2 Speculative Flush (Active Pipeline Reset)
To prevent the **Late Writeback Hazard** (where an FMA instruction completes after its ROB tag is reassigned), the FMA unit actively monitors `flush_i`:

* When `flush_i` is asserted, all internal pipeline registers of the FMA execution unit are immediately cleared.
* Valid bits associated with each pipeline stage are reset to `0`, and `wb_valid_o` is deasserted.
* This actively kills any in-flight floating-point multiply-add operations, ensuring they never write back stale results or flags to the ROB.

### 5.3 NaN-boxing
Inputs are checked for 32-bit NaN-boxing compliance. Single-precision operations will produce 64-bit outputs with the upper 32 bits set to `0xFFFFFFFF`.

## 6. Timing and Performance

The module is fully pipelined, allowing a new operation to start every cycle:

- **Pipeline Latency:** 3 clock cycles.
- **Clock Gating:** Active clock gating on registers when the pipeline contains no valid calculations to reduce dynamic power.

### 6.1 Pipeline Stall Propagation & Bypassing
* **Stalling:** If `wb_ready_i` is deasserted while a result is ready in the output stage (`wb_valid_o` is high), the final stage stalls. This stall propagates backward through the 3 pipeline stages, halting the entire pipeline. When stalled, `disp_ready_o` is deasserted.
* **Bypassing:** While a result sits in the output register `wb_result_o` due to a writeback stall, the register file bypass network can read and forward this value directly to subsequent dependent instructions.


## 7. Verification

Verification is based on comparison against reference models:

1. **Berkeley TestFloat:** Extensively testing FMA operations across all rounding modes and edge cases (denormals, infinity, signaling NaN).
2. **Pipeline Hazard Tests:** Validating correctness under pipeline stalls (`ready_i` asserted/deasserted).
