---
title: Index
---

# Pulsar-V in-order core - Crab

Welcome to the official **Crab Core** documentation, an in-order core implementing RV64GC.

## Documentation Map

*   **Project Overview:**
    *   [About & Goals](about.md) — High-level description, goals, and key hardware requirements of Crab Core.
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
        *   [Register Bypass Notes](uarch/writeback/writeback_bypass.md) — Scoreboarding, priority writeback, and data forwarding.
        *   [Writeback Stage](uarch/writeback/writeback.md) — Retirement Buffer (ROB) layout, Split-Array ROB, dynamic stalls, and retirement commits.
        *   [Control & Status Registers (CSR)](uarch/writeback/csr.md) — System state, privilege levels, trap handling, and Physical Memory Protection (PMP).

---
!!! note "Project Status"
    Documentation is currently under continuous development and its current status is `Draft`.
