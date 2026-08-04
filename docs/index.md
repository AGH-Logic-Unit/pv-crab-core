---
title: Crab Core
---

# Pulsar-V in-order core - Crab

Welcome to the official **Crab Core** documentation, an in-order core implementing RV64GC.

## About Project

Crab is an in-order RISC-V core (hart) designed to implement the RV64GC ISA for general-purpose CPU usage. It is performance optimized for both FPGA and ASIC implementations.

## Structure of Documentation

Documentation is structured as follows:

*   **Guides & Guidelines:**
    *   [Repository Standards](rules.md) — Guidelines for code quality, formatting, verification, and contribution workflows.
*   **Microarchitecture (uArch):** Detailed specifications of the core stages and components:
    *   **Common Types:**
        *   [uArch Types](uarch/types.md) — Base data types and headers.
        *   [uArch Parameters](uarch/parameters.md) - Global parameters and config.
    *   **Memory Subsystem:**
        *   [L1 Data Cache Design](uarch/cache.md) — Write policy, local reservations, and core-side AMOs.
    *   **Execute Stage (`execute/`):**
        *   [Integer ALU](uarch/execute/alu.md) — Combinational logic and W-variants.
        *   [Integer Multiplier](uarch/execute/mul.md) — 3-stage pipelined multiplication.
        *   [Integer Divider](uarch/execute/div.md) — 16-cycle division and W-variants.
        *   [Load-Store & AMOs (LSO-AMO)](uarch/execute/lso_amo.md) — Load, store, LR/SC, and atomic memory operations.
        *   [FP Multiplier-Adder (fMulAdd)](uarch/execute/fMulAdd.md) — 3-stage FMA unit for addition, subtraction, and multiplication.
        *   [FP Divider/Sqrt (fDiv)](uarch/execute/fDiv.md) — Non-pipelined division and square root.
        *   [FP Miscellaneous (fMisc)](uarch/execute/fMisc.md) — Moves, conversions, comparisons, and classification.
    *   **Writeback Stage (`writeback/`):**
        *   [Writeback & Register Bypass](uarch/writeback/writeback_bypass.md) — Scoreboarding, priority writeback, and data forwarding.

---
!!! note "Project Status"
    Documentation is currently under continuous development and its current status is `Draft`.
