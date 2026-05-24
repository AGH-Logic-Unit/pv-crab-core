---
title: Floating-Point Divider & Square Root
---

# Floating-Point Divider & Square Root Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-05-23 | v0.1 | Initial draft template |

## 2. Overview

The **fDiv** module is a non-pipelined, multi-cycle floating-point execution unit in the Pulsar-V core. It performs single and double-precision floating-point division and square root operations in compliance with the IEEE 754-2008 standard and the RISC-V RV64FD extensions.

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

- **RV32F / RV64F (Single-Precision):** `FDIV.S`, `FSQRT.S`.
- **RV32D / RV64D (Double-Precision):** `FDIV.D`, `FSQRT.D`.
- **Rounding Modes:** Support for all RISC-V rounding modes (`RNE`, `RTZ`, `RDN`, `RUP`, `RMM`, dynamic rounding via `frm`).
- **IEEE 754-2008 Compliance:** Correct handling of subnormal numbers, NaNs, infinities, and generation of floating-point status flags (NV, DZ, OF, UF, NX).

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid division request |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating unit is ready to accept a new request |
| `disp_headers_i`| `t__exe_headers`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | First floating-point operand (dividend/radicand) |
| `operand_b_i` | `logic` | 64 | IN | Second floating-point operand (divisor, unused for FSQRT) |
| `operator_i` | `logic` | 2 | IN | Operation selector: `FDIV.S`, `FDIV.D`, `FSQRT.S`, `FSQRT.D` |
| `rm_i` | `logic` | 3 | IN | Rounding mode from floating-point control register `frm` |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Calculated floating-point result (NaN-boxed to 64 bits for single-precision) |
| `wb_fflags_o` | `logic` | 5 | OUT | Floating-point exception flags: `{NV, DZ, OF, UF, NX}` |
| `wb_headers_o` | `t__exe_headers`| - | OUT | Output execution stage header metadata to Writeback Buffer |

## 5. Functional Description

### 5.1 Algorithm
To minimize area, the module implements a digit-recurrence division and square root algorithm (e.g., Radix-2 or Radix-4 SRT algorithm).

* **NaN-boxing:** For single-precision operations on a 64-bit FPU, inputs are checked for proper NaN-boxing (upper 32 bits must be all ones). The output of single-precision operations is NaN-boxed (upper 32 bits set to `0xFFFFFFFF`).

## 6. Timing and Performance

The module is non-pipelined and blocks dispatch of subsequent floating-point division/square root operations while executing.
- **Single-Precision Division (`FDIV.S`):** ~15 clock cycles.
- **Double-Precision Division (`FDIV.D`):** ~30 clock cycles.
- **Single-Precision Square Root (`FSQRT.S`):** ~15 clock cycles.
- **Double-Precision Square Root (`FSQRT.D`):** ~30 clock cycles.

### 6.1 Arbiter Stalls & Output Register Bypassing
* **Stalling:** When the floating-point calculation completes, the unit registers the result, asserts `wb_valid_o`, and deasserts `disp_ready_o`. If `wb_ready_i` is deasserted by the Writeback Arbiter (due to a priority stall), the module stalls and holds its outputs (`wb_valid_o`, `wb_result_o`, `wb_fflags_o`, `wb_headers_o`) stable.
* **Bypassing:** While the result is waiting in the output register due to a writeback stall, the register file bypass network can forward the floating-point result from `wb_result_o` to subsequent instructions depending on it.


## 7. Verification

Verification is based on comparison against reference models:
1. **Test Vectors:** Verification using Berkeley TestFloat vectors to cover boundary conditions, extreme rounding scenarios, and exception flag generation.
2. **FSM State Coverage:** Verify FSM stalls, backpressure from pipeline `ready_i`, and clock-gating during idle cycles.
