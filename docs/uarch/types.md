---
title: Types
---

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Standardized type names to use the _t suffix, cleaned up and moved private Writeback ROB types to their respective files, and added priv_mode_t. |
| 2026-04-26 | v0.1 | Initial draft |


## 2. Global Pipeline Types

This section defines the standardized, global SystemVerilog types used to interface between the different stages of the Pulsar-V / Crab core. In accordance with project coding standards, all user-defined types use the `_t` suffix to differentiate them from variables and signals.

### 2.1 `exe_headers_t` (Execution Pipeline Tracking Header)

Following the Split-Array ROB optimization, static instruction metadata (such as destination register addresses, write enables, and operation classes) is written directly to the ROB's Metadata Array at Dispatch. Consequently, the execution unit pipeline registers do not carry this static metadata. The `exe_headers_t` struct is used globally to carry only the tracking tag:

| Field Name | Type / Width | Description |
| :--- | :---: | :--- |
| `tag` | `logic [TAG_WIDTH-1:0]` | Instruction ID tag for tracking and ROB addressing. |

### 2.2 `priv_mode_t` (Privilege Levels)

Used to track and enforce the current privilege level of the core:

| Element Name | Encoding | Privilege Mode | Description |
| :--- | :---: | :---: | :--- |
| `PRIV_USER` | `2'b00` | User (U-mode) | Lowest privilege level for user applications. |
| `PRIV_SUPERVISOR` | `2'b01` | Supervisor (S-mode) | Operating system level privilege (required for virtual memory Sv39). |
| `PRIV_MACHINE` | `2'b11` | Machine (M-mode) | Highest privilege level (required, raw hardware access). |

### 2.3 `csr_op_t` (CSR Operation Type)

Used by the Decode, Dispatch, and Retire stages to specify the atomic operation to perform on the CSR register:

| Element Name | Encoding | Description | RISC-V Instructions |
| :--- | :---: | :--- | :--- |
| `CSR_OP_NONE` | `2'b00` | Read only (no write side-effects) | Performance counter reads (`cycle`, `time`, etc.) |
| `CSR_OP_WRITE` | `2'b01` | Atomic Read/Write | `CSRRW`, `CSRRWI` |
| `CSR_OP_SET` | `2'b10` | Atomic Read and Set Bits | `CSRRS`, `CSRRSI` |
| `CSR_OP_CLEAR` | `2'b11` | Atomic Read and Clear Bits | `CSRRC`, `CSRRCI` |

### 2.4 `op_class_t` (Instruction Execution Class)

Set during Decode and used at Dispatch/Retire to route instructions and execute their commit-phase side effects:

| Element Name | Encoding | Description / Action at Commit |
| :--- | :---: | :--- |
| `OP_CLASS_NORMAL` | `3'b000` | Standard ALU/Load (GPR/FPR register writeback, no other side-effects). |
| `OP_CLASS_CSR` | `3'b001` | CSR instruction (halts normal commits, executes RMW in CSR unit, GPR writeback). |
| `OP_CLASS_FPU` | `3'b010` | FPU instruction (updates dynamic `fflags` in CSR unit). |
| `OP_CLASS_STORE` | `3'b011` | Store / AMO instruction (requires Store Buffer / STB commit). |
| `OP_CLASS_SYSTEM` | `3'b100` | System instruction (privilege returns like `MRET`/`SRET`, wait-states like `WFI`, or TLB flushes like `SFENCE.VMA`). |

### 2.5 `rb_meta_t` (Packed Metadata Union)

A 14-bit packed union constructed at Decode/Dispatch and stored in the ROB. It overlays mutually exclusive instruction-specific metadata:

| Field Group | Sub-Field Name | Width | Bit Range | Description / Conditions |
| :--- | :--- | :---: | :---: | :--- |
| **FPU (fp)** | `fflags` | 5 | `[4:0]` | Speculative FPU exception flags generated in Stage 6. |
| | *unused* | 9 | `[13:5]` | Zero padding. |
| **Trap (trap)** | `cause` | 6 | `[5:0]` | Trap exception cause code (valid when `trap == 1` at retirement). |
| | `is_interrupt` | 1 | `[6]` | Asynchronous interrupt indicator. |
| | *unused* | 7 | `[13:7]` | Zero padding. |
| **CSR (csr)** | `addr` | 12 | `[11:0]` | Target CSR address. |
| | `op` | 2 | `[13:12]` | CSR operation type (`csr_op_t`). |

### 2.6 `pma_amo_level_t` (PMA Atomic Capability Levels)

Used by the MMU, D-TLB, and LSU to enforce physical memory attribute atomic operation capabilities:

| Element Name | Encoding | Capability Level | Description |
| :--- | :---: | :--- | :--- |
| `PMA_AMO_NONE` | `2'b00` | None | No atomic operations supported (standard MMIO peripherals, ROM). |
| `PMA_AMO_SWAP` | `2'b01` | Swap Only | Supports `AMOSWAP` only (simple synchronization registers). |
| `PMA_AMO_LOGICAL` | `2'b10` | Logical | Supports `AMOSWAP`, `AMOAND`, `AMOOR`, `AMOXOR` (bitmask controllers). |
| `PMA_AMO_ARITHMETIC` | `2'b11` | Arithmetic & Full | Supports all AMOs (`AMOADD`, `AMOMIN`, `AMOMAX`, etc.) and `LR/SC`. |


## 3. Future Scalability (Superscalar Upgrades)

To facilitate future migration to a superscalar (dual-issue or multi-issue) hart, the following architectural guidelines should be followed:

### Register File & Scoreboard
* **Port Parameterization:** Avoid hardcoding the number of read/write ports on the Register File and the Scoreboard. The Scoreboard must support checking dependencies and allocating tags for multiple instructions concurrently.
* **Tag Routing:** For $N$-issue superscalar execution, the dispatcher must allocate up to $N$ unique tags per cycle and route them to their respective execution lanes.
