---
title: Integer Multiplier
---

# Integer Multiplier Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Added flush_i port and active pipeline clear, and renamed headers to exe_headers_t |
| 2026-05-23 | v0.1 | First draft |

## 2. Overview

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

- **RV64M:** `MUL`, `MULH`, `MULHU`, `MULHSU`, `MULW` instructions with *XLEN=64*.
- **Data width:** Support for 64-bit operands.
- **Pipeline:** Operations should be executed in a 3-stage pipelined execution unit, allowing the dispatcher to issue a new multiplication instruction every cycle.

## 4. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `flush_i` | `logic` | 1 | IN | Pipeline flush signal (clears pipeline stages EX1, EX2, EX3) |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid multiplication request |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating multiplier is ready to accept a new request |
| `disp_headers_i`| `t__exe_headers`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | First operand (typically RS1) |
| `operand_b_i` | `logic` | 64 | IN | Second operand (typically RS2 or IMM) |
| `operator_i` | `logic` | 3 | IN | Operation code provided by Decoder |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Calculated multiplication result |
| `wb_headers_o` | `t__exe_headers`| - | OUT | Output execution stage header metadata to Writeback Buffer |

## 5. Functional Description

### 5.1 Multiplication Architecture
To meet timing requirements, the multiplier is split into three pipeline stages (EX1, EX2, EX3). It calculates the full 128-bit product of the two 64-bit inputs to support high-part multiplication:
* **`MUL` / `MULW`**: Returns the lower 64 bits of the product. `MULW` truncates inputs to 32 bits and sign-extends the 32-bit result.
* **`MULH`**: Returns the upper 64 bits of the signed $\times$ signed product.
* **`MULHU`**: Returns the upper 64 bits of the unsigned $\times$ unsigned product.
* **`MULHSU`**: Returns the upper 64 bits of the signed $\times$ unsigned product.

### 5.2 Speculative Flush (Active Pipeline Reset)
To prevent the **Late Writeback Hazard** (where a multiplication completes after its ROB tag is reassigned), the multiplier actively monitors `flush_i`:
* When `flush_i` is asserted, all internal pipeline registers (EX1, EX2, EX3) are immediately cleared.
* Valid bits associated with each pipeline stage are reset to `0`, and `wb_valid_o` is deasserted.
* This actively kills any in-flight multiplication operations, ensuring they never write back stale results to the ROB after a flush.

## 6. Timing and Performance

The module implements multiplication in 3 pipeline stages. The pipeline registers are placed to balance delays and minimize the critical path.

### 6.1 Pipeline Stall Propagation & Bypassing
* **Stalling:** If the Writeback Arbiter deasserts `wb_ready_i` while a valid result is in the EX3 output stage (`wb_valid_o` is high), the EX3 stage registers stall. This stall propagates backward: EX3 stalls EX2, and EX2 stalls EX1. If EX1 is stalled, `disp_ready_o` is deasserted to prevent the dispatcher from issuing new instructions.
* **Bypassing:** While a completed multiplication result sits in the EX3 output register (`wb_result_o`) due to a writeback stall, the register bypass logic can forward this value directly to the dispatcher's execution operands.

### 6.2 Future Superscalar Scalability
In a superscalar upgrade (dual/multi-issue), the Writeback Arbiter must scale to support multiple concurrent writeback channels. It is recommended that the multiplier's writeback port is mapped to a dedicated writeback channel or a prioritized channel that is decoupled from simple single-cycle ALU lanes to avoid stalling the multiplier when it completes.

When no operation is being processed or stalled, the corresponding stage registers should be gated (dynamic clock gating) to save power.

## 7. Verification

Multiply Module verification is based on constrained random tests:

1. **Unit Tests:** Verification of all valid ISA instructions with edge cases.
2. **Reference Model:** RTL results must be identical to the programmatic reference model (C, C++, or Python).
3. **Coverage:** Full 100% coverage of `operator_i` codes and all operand bits.
4. **Pipeline:** DUT must return input headers at the end of the pipeline with correct fields and changes applied.
