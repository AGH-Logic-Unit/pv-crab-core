---
title: Parameters
---

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Cleaned up file to contain only global core parameters, and integrated PMP_ENTRIES as a global parameter. |
| 2026-08-04 | v0.1 | Initial draft registry |

---

## 2. Global Microarchitectural Parameters Registry

This document serves as the central registry for all **Global Parameters** within the Pulsar-V / Crab core. These parameters are defined at the top-level of the core design (typically in a global package like `config_pkg`) and are shared across multiple pipeline stages, interfaces, and security modules.

Module-specific or local parameters (such as pipeline latencies, buffer sizes, or specific execution lane implementations) are documented directly within their respective module files to maintain documentation locality.

---

## 3. Global Core Parameters

| Parameter Name | Type | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `XLEN` | `int` | `64` | General-purpose register and address width (64 bits for RV64). |
| `FLEN` | `int` | `64` | Floating-point register width (64 bits for single and double precision support). |
| `RB_SIZE` | `int` | `16` | Size (number of slots) of the Retirement Buffer (ROB) FIFO. |
| `TAG_WIDTH` | `int` | `$clog2(RB_SIZE)` | Width of the instruction tag used for tracking (4 bits for `RB_SIZE = 16`). |
| `PA_LEN` | `int` | `56` | Physical address width (56 bits for RISC-V Sv39 physical addressing). |
| `VA_LEN` | `int` | `39` | Virtual address width (39 bits for Sv39 virtual translation). |
| `CACHE_LINE_BYTES`| `int` | `64` | Coherence line size in bytes (used for L1 caches, and PMP NAPOT address matching). |
| `PMP_ENTRIES` | `int` | `16` | Number of implemented Physical Memory Protection entries (affects CSR configuration registers, Stage 1 Fetch filter, and Stage 6 LSO-AMO filter). |
