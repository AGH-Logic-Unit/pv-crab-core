---
title: Integer Divider
---

# Integer Divider Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.3 | Added flush_i port and active reset behavior, and renamed headers to exe_headers_t |
| 2026-05-24 | v0.2 | Integrated DIVW, DIVUW, REMW, and REMUW instructions, updated operator_i width |
| 2026-05-23 | v0.1 | First draft |

## 2. Overview

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

- **RV64M:** `DIV`, `DIVU`, `REM`, `REMU`, `DIVW`, `DIVUW`, `REMW`, `REMUW` instructions with *XLEN=64*.
- **Data width:** Support for 64-bit operands.
- **Staged:** Operations should be executed in 16 cycles without internal pipelining, blocking dispatch of the next division instruction until the previous one is finished and committed.

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `flush_i` | `logic` | 1 | IN | Pipeline flush signal (resets execution state to IDLE) |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid division request |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating divider is ready to accept a new request |
| `disp_headers_i`| `exe_headers_t`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | First operand (typically RS1) |
| `operand_b_i` | `logic` | 64 | IN | Second operand (typically RS2 or IMM) |
| `operator_i` | `logic` | 3 | IN | Operation code provided by Decoder (encoding DIV, DIVU, REM, REMU, and W-variants) |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Calculated quotient or remainder result |
| `wb_headers_o` | `exe_headers_t`| - | OUT | Output execution stage header metadata to Writeback Buffer |

## 5. Functional Description

### 5.1 Division Algorithm
The division module implements a serial division algorithm (e.g., Radix-2 or Radix-4) to compute the quotient and remainder.

* **Division by Zero:** According to RISC-V specification, division by zero must return all ones (`-1` or `0xFFFFFFFFFFFFFFFF` for 64-bit, `0xFFFFFFFF` for 32-bit) for the quotient, and the dividend for the remainder (`REM`/`REMU`/`REMW`/`REMUW`). This case should be detected and handled as a 1-cycle bypass.
* **Signed Overflow:** The overflow condition occurs only for signed division when dividing the most negative integer ($-2^{63}$ for 64-bit, $-2^{31}$ for 32-bit) by $-1$. The result should be the most negative integer for the quotient and `0` for the remainder. This should also be bypassed in 1 cycle.
* **32-bit W-variants:** For `DIVW`, `DIVUW`, `REMW`, and `REMUW` instructions:
  - Inputs `operand_a_i` and `operand_b_i` are truncated to 32 bits.
  - The division operation is performed on these 32-bit values.
  - The 32-bit quotient or remainder result is **sign-extended** to 64 bits before being driven on `wb_result_o`.

### 5.2 Speculative Flush (Active Reset)
To prevent the **Late Writeback Hazard** (where a flushed multicycle division completes after the ROB tag has been reassigned to a new instruction), the divider actively monitors the `flush_i` signal:
* When `flush_i` is asserted, any ongoing division calculation is immediately aborted.
* The internal serial control logic, counters, and registers are reset.
* The module deasserts `wb_valid_o` and transitions back to the `IDLE` state in the same cycle.
* This ensures that no invalid results are written back to the ROB and that the unit is immediately ready to receive new instructions, preventing structural resource blocks.

## 6. Timing and Performance

The module implements operations in 16 cycles with width optimized for performance.

### 6.1 Arbiter Stalls & Output Register Bypassing
* **Stalling:** When the 16-cycle calculation completes, the divider registers the result in its output registers, asserts `wb_valid_o`, and deasserts `disp_ready_o`. If the Writeback Arbiter deasserts `wb_ready_i` (due to priority conflicts with other execution modules), the divider stalls, holding `wb_valid_o` and `wb_result_o` stable.
* **Bypassing:** While the result is waiting in the output register (`wb_result_o`) due to a deasserted `wb_ready_i`, the register file bypass network can forward the data directly to dispatcher execution operands, resolving dependencies for subsequent instructions without stalling them.

### 6.2 Future Superscalar Scalability
In a superscalar upgrade (dual/multi-issue), the Writeback Arbiter must scale to support multiple concurrent writeback channels. It is recommended that the divider's writeback port is mapped to a dedicated writeback channel or a prioritized channel that is decoupled from simple single-cycle ALU lanes to avoid stalling the divider when it completes.

When no operation is being processed or stalled, the module should be clock-gated to save power.

## 7. Verification

Division Module verification is based on constrained random tests:

1. **Unit Tests:** Verification of all valid ISA instructions with edge cases.
2. **Reference Model:** RTL results must be identical to the programmatic reference model (C, C++, or Python).
3. **Coverage:** Full 100% coverage of `operator_i` codes and all operand bits.
4. **Latency:** DUT must return correct headers after specified latency with adequate modifications applied.
