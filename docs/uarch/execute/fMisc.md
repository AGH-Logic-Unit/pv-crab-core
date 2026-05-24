---
title: Floating-Point Miscellaneous
---

# Floating-Point Miscellaneous Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-05-24 | v0.2 | Integrated FMV move instructions (FPR <-> GPR) |
| 2026-05-24 | v0.1 | Initial draft for conversions, comparisons, and auxiliary FP operations |

## 2. Overview

The **fMisc** module is an execution unit in the Pulsar-V core dedicated to floating-point auxiliary operations that do not belong in the FMA pipeline (`fMulAdd`) or the division pipeline (`fDiv`). These operations include floating-point to integer conversions, integer to floating-point conversions, precision conversions, sign-injection, floating-point comparisons, and class detection.

Many of these instructions read from or write to the General Purpose Registers (GPRs), serving as the bridge between the floating-point register file (FPR) and the integer register file.

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

- **RV32F / RV64F (Single-Precision):**
  - **Float to Integer:** `FCVT.W.S`, `FCVT.WU.S`, `FCVT.L.S` (RV64), `FCVT.LU.S` (RV64).
  - **Integer to Float:** `FCVT.S.W`, `FCVT.S.WU`, `FCVT.S.L` (RV64), `FCVT.S.LU` (RV64).
  - **Sign Injection:** `FSGNJ.S`, `FSGNJN.S`, `FSGNJX.S`.
  - **Comparisons:** `FEQ.S`, `FLT.S`, `FLE.S`.
  - **Min/Max:** `FMIN.S`, `FMAX.S`.
  - **Classification:** `FCLASS.S`.
  - **Data Movement (FPR <-> GPR):** `FMV.X.W`, `FMV.W.X`.
- **RV32D / RV64D (Double-Precision):**
  - **Float to Integer:** `FCVT.W.D`, `FCVT.WU.D`, `FCVT.L.D` (RV64), `FCVT.LU.D` (RV64).
  - **Integer to Float:** `FCVT.D.W`, `FCVT.D.WU`, `FCVT.D.L` (RV64), `FCVT.D.LU` (RV64).
  - **Float to Float:** `FCVT.S.D`, `FCVT.D.S`.
  - **Sign Injection:** `FSGNJ.D`, `FSGNJN.D`, `FSGNJX.D`.
  - **Comparisons:** `FEQ.D`, `FLT.D`, `FLE.D`.
  - **Min/Max:** `FMIN.D`, `FMAX.D`.
  - **Classification:** `FCLASS.D`.
  - **Data Movement (FPR <-> GPR):** `FMV.X.D`, `FMV.D.X`.
- **Rounding Modes:** Support for all RISC-V rounding modes (`RNE`, `RTZ`, `RDN`, `RUP`, `RMM`, dynamic rounding via `frm`) during conversion operations.

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid instruction |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating unit can accept new inputs |
| `disp_headers_i`| `t__exe_headers`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | First operand (FPR value, or GPR value for integer-to-float / FMV.W.X / FMV.D.X) |
| `operand_b_i` | `logic` | 64 | IN | Second operand (FPR value for comparisons/min/max, unused for conversions/moves) |
| `operator_i` | `logic` | 5 | IN | Operation encoding (FCVT, FSGNJ, FEQ, FCLASS, FMV, etc.) |
| `rm_i` | `logic` | 3 | IN | Rounding mode from floating-point control register `frm` |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Calculated result (NaN-boxed to 64 bits for FPR-bound outputs, sign-extended for GPR-bound outputs) |
| `wb_fflags_o` | `logic` | 5 | OUT | Floating-point exception flags: `{NV, DZ, OF, UF, NX}` (only valid for FCVT/FMIN/FMAX/comparisons) |
| `wb_headers_o` | `t__exe_headers`| - | OUT | Output execution stage header metadata to Writeback Buffer |

## 5. Functional Description

### 5.1 Berkeley HardFloat Integration
To accelerate development and ensure correctness, the module relies on the following Berkeley HardFloat IP blocks:
- **`recFNToIN` (Float-to-Integer):** Performs conversion from recoded floats to signed/unsigned 32-bit and 64-bit integers.
- **`iNToRecFN` (Integer-to-Float):** Performs conversion from signed/unsigned integers to recoded floats.
- **`recFNToRecFN` (Float-to-Float):** Converts between double-precision and single-precision recoded formats.
- **`compareRecFN` (Comparisons):** Performs IEEE 754-2008 compliant comparisons, outputting equality, less-than, and signalling exceptions for invalid comparisons (e.g. signaling NaNs).

### 5.2 Single-Cycle Combinatorial Operations
For non-conversion operations, the module operates combinatorially and bypasses pipeline stages to complete in **1 cycle**:
- **Sign Injection (`FSGNJ`/`FSGNJN`/`FSGNJX`):** Computes results by combining the sign bit of `operand_b_i` with the exponent and significand of `operand_a_i`.
- **Classification (`FCLASS`):** Checks the exponent, fraction, and sign of the input to output a 10-bit mask identifying the category of the floating-point value (negative infinity, negative normal, negative subnormal, etc.).
- **Min/Max (`FMIN`/`FMAX`):** Employs the `compareRecFN` block to determine the minimum/maximum of two inputs. Handles special IEEE 754 NaN propagation rules (e.g., if one input is NaN, returns the other).
- **Data Movement (`FMV`):** Copies raw bits between General Purpose Registers (GPRs) and Floating-Point Registers (FPRs) without any numerical conversion:
  - `FMV.X.W` / `FMV.X.D`: Moves the bit pattern from FPR `operand_a_i` to GPR output `wb_result_o`. For `FMV.X.W`, the 32-bit pattern is sign-extended to 64 bits.
  - `FMV.W.X` / `FMV.D.X`: Moves the bit pattern from GPR `operand_a_i` to FPR output `wb_result_o`. For `FMV.W.X`, the 32-bit pattern is NaN-boxed (upper 32 bits set to `0xFFFFFFFF`).

### 5.3 NaN-boxing
- **Inputs:** Float inputs are checked for 32-bit NaN-boxing. If a single-precision input is not properly NaN-boxed (upper 32 bits are not all 1s), it is treated as a default NaN (`0x7FC00000`).
- **Outputs (FPR bound):** Single-precision results written back to the FPR are NaN-boxed (upper 32 bits set to `0xFFFFFFFF`).
- **Outputs (GPR bound):** 32-bit integer results written back to GPRs are sign-extended to 64 bits.

## 6. Timing and Performance

To balance design complexity and pipeline scheduling, the unit is split into two paths:

1. **Fast-path (1 cycle):** FSGNJ, FCLASS, comparisons (`FEQ`/`FLT`/`FLE`), `FMIN`/`FMAX`, and `FMV` move operations bypass the pipeline registers and write back immediately.
2. **Slow-path (2 cycles):** Conversion operations (`FCVT`) are registered to meet core clock timing, completing with a 2-cycle latency.

### 6.1 Stall Propagation
If the Writeback Arbiter deasserts `wb_ready_i`, the pipeline output registers are stalled, holding `wb_valid_o` high. Stall signals propagate back to the dispatcher.

## 7. Verification

Verification is based on comparison against reference models:
1. **Berkeley TestFloat:** Exhaustive verification of conversion rounding modes, integer bounds check, and comparison exception behaviors.
2. **Corner-Case Analysis:** Verifying NaN-boxing logic on inputs and outputs.
