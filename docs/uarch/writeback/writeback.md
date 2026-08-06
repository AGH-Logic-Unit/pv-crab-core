---
title: Writeback Module
---

# Writeback

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Integrated Split-Array ROB, dynamic stall handshakes, RVC PC[0] redirection, and FPU exception flags accumulation |
| 2026-07-31 | v0.1 | Initial draft |

## 2. Overview

Writeback Stage is the last stage of Crab pipeline and it's **only one** commit point.

## 3. Architectural Requirements

Writeback module must implement the following functional requirements:

- **Retirement Buffer**: FIFO register addressable by *Operation Tag* assigned by *Dispatch*, storing PC and output of execution stage. Buffer is responsible for ensuring in-order retirement of instructions (similar to ROB in Out-of-Order execution).
- **Priority executed operation handling**: Module is responsible for passing finished instruction passed by Execution Stage to the correct address in Buffer. It handles output of each execution module by following priority rules.
- **Instruction commit**: Writeback is the primary and single commit point in Crab pipeline. Each completed instruction, ready and valid, should be committed overriding corresponding register in Register File. *Store* instructions should await commit from writeback to be written into memory, as well as AMO instructions.
- **Trap handling**: When trap instruction or exception was encountered while committing, and have not yet been resolved, should be handled according to RISC-V trap specification.
- **CSR**: UArch follows Execute-at-Retire philosophy for CSR instructions. That said, writeback must strictly cooperate with CSR module.


## 4. Parameterization

| Parameter | Type | Default | Description | Architectural Impact |
| :--- | :---: | :---: | :--- | :--- |
| `RB_SIZE` | `int unsigned` | `8` | Total slots in the Retirment Buffer. | Determines tag width: `TAG_WIDTH = $clog2(RB_SIZE)`. Affects occupancy tracking. Must be a power of 2. |
| `TAG_WIDTH` | `int unsigned` | `$clog2(RB_SIZE)` | Bit width of tag assigned to operation. | Affects size of registers in execute modules. |

## 5. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| **Global Signals** | | | | |
| `clk_i` | `logic` | 1 | IN | System clock |
| `rst_ni` | `logic` | 1 | IN | Asynchronous reset (active-low) |
| **Pipeline Control** | | | | |
| `flush_req_o` | `logic` | 1 | OUT | Pipeline flush request |
| **CSR interface** | | | | |
| `commit_csr_valid_o` | `logic` | 1 | OUT | Committing instruction is CSR |
| `commit_csr_addr_o` | `logic` | 12 | OUT | Target CSR address |
| `commit_csr_op_o` | `csr_op_t` | 2 | OUT | CSR operation type |
| `commit_wdata_o` | `logic` | 64 | OUT | CSR write data (rs1 or zimm operand) |
| `commit_rdata_i` | `logic` | 64 | IN | Old CSR register value read from CSR Unit |
| `commit_illegal_i` | `logic` | 1 | IN | Illegal instruction exception flag |
| `commit_flush_req_i` | `logic` | 1 | IN | System-critical CSR write flush request |
| **FPU interface** | | | | |
| `commit_fpu_valid_o` | `logic` | 1 | OUT | Committing instruction is FPU |
| `commit_fflags_o` | `logic` | 5 | OUT | speculative FPU exception flags |
| **Commit Interface** | | | | |
| `commit_valid_o` | `logic` | 1 | OUT | Instruction commit valid |
| `commit_tag_o` | `logic` | `TAG_WIDTH` | OUT | Tag of the committing instruction |
| `commit_rd_addr_o` | `logic` | 5 | OUT | Committing destination register address |
| `commit_rd_type_o` | `logic` | 1 | OUT | Committing destination type (`0` = GPR, `1` = FPR) |
| `commit_rd_we_o` | `logic` | 1 | OUT | Committing write enable |
| **STB Interface** | | | | |
| `stb_commit_valid_o` | `logic` | 1 | OUT | Commit valid for store/AMO |
| `stb_commit_tag_o` | `logic` | `TAG_WIDTH` | OUT | Tag of the store to commit |
| **Trap Interface** | | | | |
| `trap_valid_o` | `logic` | 1 | OUT | Trap condition active at retirement head |
| `trap_cause_o` | `logic` | 6 | OUT | Trap exception cause code |
| `trap_is_interrupt_o` | `logic` | 1 | OUT | Asynchronous interrupt indicator |
| `trap_badaddr_o` | `logic` | 64 | OUT | Bad virtual address / instruction word |
| `trap_pc_o` | `logic` | 64 | OUT | PC of the faulting instruction |
| `trap_vector_i` | `logic` | 64 | IN | Target trap vector address from CSR unit |

### 5.1 Execution module interfaces

The interface between the Writeback Arbiter and each execution module (ALU, Divider, FPU, LSO-AMO) carries the dynamic results and execution flags to be written into the ROB Result/Status Array:

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `ex_valid_i` | `logic` | 1 | IN | Writeback handshake indicating valid result is ready |
| `ex_ready_o` | `logic` | 1 | OUT | Handshake indicating Writeback Arbiter can accept result |
| `ex_result_i` | `logic` | 64 | IN | Operation result / CSR `wdata` / Exception `badaddr` |
| `ex_fflags_i` | `logic` | 5 | IN | Dynamic Floating-Point exception flags (from FPU modules, otherwise 0) |
| `ex_trap_i` | `logic` | 1 | IN | Asserted if the instruction encountered a synchronous exception (e.g. page fault, misaligned) |
| `ex_cause_i` | `logic` | 6 | IN | Exception cause code (valid when `ex_trap_i` is 1) |
| `ex_headers_i` | `exe_headers_t`| - | IN | Carry-over tracking header (contains only the `tag`) |

---

## 6. Functional Description

### 6.1 Retirement buffer

The Retirement Buffer is designed using a **Split-Array Architecture** to avoid the hardware overhead and timing bottlenecks of dual-write-port (2W) RAM structures. It is partitioned into two physically distinct, single-write-port arrays:

1. **Metadata Array (Static Array):**
      * **Ports:** 1 Write Port (used by Decode/Dispatch at Stage 5), 1 Read Port (used by Retire at Stage 7).
      * **Contents:** `pc` (64-bit program counter, see Section 6.4 for `PC[0]` optimization), `rd_addr` (5-bit), `rd_type` (1-bit), `rd_we` (1-bit), `op_class` (3-bit `op_class_t`), and CSR static metadata (`meta.csr.addr`, `meta.csr.op`, `meta.trap.is_interrupt`).
      * **Behavior:** Written once at Dispatch. It is never modified by execution stages.

2. **Result/Status Array (Dynamic Array):**
      * **Ports:** 1 Write Port (used by the Writeback Arbiter / Flush controller), 1 Read Port (used by Retire).
      * **Contents:** `valid` (1-bit completion flag), `trap` (1-bit trap flag), `result` (64-bit data), and execution-dynamic status (`meta.fp.fflags`, `meta.trap.cause`).
      * **Behavior:** Written by execution units via the Writeback Arbiter upon completion.

#### Register contents

| Name | Type | Width | Description |
| :--- | :---: | :---: | :--- |
| `valid` | `logic` | 1 | Operation completed, result/metadata valid (in Result Array) |
| `trap` | `logic` | 1 | Instruction encountered a trap (in Result Array) |
| `op_class` | `op_class_t` | 3 | Class of operation (in Metadata Array) |
| `rd_we` | `logic` | 1 | Destination register write enable (in Metadata Array) |
| `rd_type` | `logic` | 1 | Destination register type (`0` = GPR, `1` = FPR) (in Metadata Array) |
| `rd_addr` | `logic` | 5 | Destination register address (in Metadata Array) |
| `pc` | `logic` | 64 | Program Counter (in Metadata Array) |
| `is_rvc` | `logic` | 1 | RVC compressed instruction indicator (in Metadata Array) |
| `result` | `logic` | 64 | Execution result / CSR `wdata` / Trap `badaddr` (in Result Array) |
| `meta` | `rb_meta_t` | 14 | Packed union: FPU flag `fflags` / trap `cause`/`is_interrupt` / CSR `addr`/`op` |

#### Invalidation upon Flush:
When a pipeline flush is triggered:

* **Tail Pointer Rollback:** The ROB tail pointer is rolled back to `head_ptr + 1` (or `head_ptr`). Any slot outside the active pointer window is treated as empty. This rollback method invalidates speculative instructions in a single cycle with **zero register-write energy overhead**.
* **Register Clearing (Optional):** If registers are explicitly cleared, the `valid` bit in the **Result/Status Array** is written to `0` for the flushed slots. Since all preceding pipeline stages are frozen during a flush, there are no write conflicts with the Writeback Arbiter.

---

### 6.2 Readout Arbiter & Dynamic Stall (Back-pressure)

The Writeback Arbiter prioritizes and routes results from execution units (ALU, Divider, FPU, Memory) to the Result/Status Array of the Retirement Buffer.

#### Dynamic Stall Handshake
PV-Crab implements a distributed **ready-valid handshake (back-pressure)** rather than a complex centralized hazard controller:

* If the Writeback stage cannot accept a result (e.g. because a structural hazard is present or the ROB is full), the Writeback Arbiter deasserts `ex_ready_o` to the respective execution module.
* The execution module stalls internally and deasserts its ready signal to the Dispatch stage.
* This back-pressure automatically propagates backward to Decode and Fetch, stalling the pipeline dynamically.

#### Readout Arbitration
Priority rules for execution units completing in the same cycle:
`fDiv > div > fMulAdd > mul > fMisc > alu > lsamo`

---

### 6.3 Committing & FPU Flag Accumulation

For standard instructions (`op_class == OP_CLASS_NORMAL` or `OP_CLASS_FPU` when `trap == 0`):

1. **Register Writeback:** Writeback asserts the GPR/FPR write enable `commit_rd_we_o` using the slot's `rd_we` and `rd_type`. The destination register address `commit_rd_addr_o` receives `rd_addr`, and the data is read from the slot's `result`.
2. **FPU Flag Accumulation:** If `op_class == OP_CLASS_FPU`:
      * Writeback asserts `commit_fpu_valid_o = 1` and routes `commit_fflags_o = meta.fp.fflags` (which was generated speculatively in Stage 6 by FMA/FDiv/FMisc units and stored in the ROB Result Array) to the CSR unit.
      * The CSR unit merges these accrued exceptions into the architectural `fflags` CSR using a bitwise OR operation: `fflags <= fflags | commit_fflags_i`.
      * The CSR unit also updates the floating-point state status `mstatus.FS` to `2'b11` (Dirty) to indicate FPR/FPU CSR changes.
      * *Security Check:* If `mstatus.FS == 2'b00` (Off) when the FPU instruction was decoded, the instruction is tagged with an Illegal Instruction exception at Stage 4 (Decode) and traps at retirement (Stage 7), skipping normal commit.
3. **Scoreboard Release:** The scoreboard clears the busy bit corresponding to the retired tag, making it available for subsequent instructions.

For Store/AMO instructions (`op_class == OP_CLASS_STORE`):

1. Assert `stb_commit_valid_o = 1` and `stb_commit_tag_o = tag`.
2. The Store Buffer (STB) receives the signal and commits the speculative store to the memory system.

---

### 6.4 CSR/PMP Execution-at-Retirement & PC Redirection

When a CSR instruction (`op_class == OP_CLASS_CSR` and `trap == 0`) reaches the head of the Retirement FIFO:

1. The Writeback stage halts GPR writes and outputs:
      * `commit_csr_valid_o = 1`
      * `commit_csr_addr_o = meta.csr.addr`
      * `commit_csr_op_o = meta.csr.op`
      * `commit_wdata_o = result` (containing the operand from `rs1` or `zimm`)
2. The CSR unit processes the read-modify-write operation and returns:
      * `commit_rdata_i` (the old value of the CSR register)
      * `commit_illegal_i` (if a security violation is caught at retirement)
      * `commit_flush_req_i` (if a system-critical register like `satp` or PMP `pmpcfg*`/`pmpaddr*` was updated)
3. If `commit_illegal_i == 1`, the instruction traps (see Section 6.5).
4. Otherwise, the old value from `commit_rdata_i` is written to GPR under `rd_addr` (if `rd_we == 1`).
5. **System-Critical / PMP Security Redirect:** If `commit_flush_req_i == 1`, a global pipeline flush is requested (`flush_req_o = 1`). This is critical when changing page tables (`satp`) or security configuration registers (`pmpcfg*`, `pmpaddr*`) to clear all speculative instructions from the pipeline.
      * The next PC target is calculated locally:
      Redirection target = `pc + (is_rvc ? 2 : 4)`.

      * This target PC is sent back to Stage 1. Since CSR and PMP instructions are strictly 32-bit (4-byte), `is_rvc` is `0` and the offset is `+4`.

---

### 6.5 Trap handling

When an instruction reaches the head of the Retirement FIFO with the `trap` bit set to `1`:

1. Normal commit processing for this and all subsequent instructions is halted.
2. The Writeback stage routes the trap parameters to the CSR unit:
      * `trap_valid_o = 1`
      * `trap_cause_o = meta.trap.cause`
      * `trap_is_interrupt_o = meta.trap.is_interrupt`
      * `trap_badaddr_o = result` (the bad memory address/instruction code causing the exception)
      * `trap_pc_o = pc` (LSB masked to `0` before writing to `mepc`/`sepc`)
3. The CSR unit updates the architectural exception status registers and outputs `trap_vector_i`.
4. Writeback asserts `flush_req_o = 1` to clear stages 1-6 and the Retirement Buffer, and updates Stage 1 to fetch from `trap_vector_i`.

---

### 6.6 Global Flush, Clock-gating & Late Writeback Hazard Prevention

#### Pipeline Flush & Clock-gating
A flush signal is global but only invalidates instructions in stages *prior* to the issuing stage. It clears the `valid` bits of the pipeline registers. This invalidation state (`valid == 0`) is directly used to perform **clock-gating** on the pipeline registers of the invalidated stages, disabling clocking to save dynamic switching power.

#### Late Writeback Hazard Prevention
When the tail pointer of the ROB is rolled back during a flush, tags are freed and can be reassigned to new instructions at Dispatch. To prevent a long-latency execution unit (e.g., Divider or FPU) from writing back a late result to a tag that has already been reassigned:

* All multi-cycle execution units (Divider, FPU, D-Cache controller) receive the global `flush` signal.
* Upon detecting a flush, they immediately abort their current operations and reset their pipelines.
* Consequently, they will never assert `valid` or attempt to write back for the flushed tags, making tag reassignment 100% hazard-free.

---

## 7. Timing and Performance

The stage should have its critical path ready for 100MHz minimum. If needed, it can be divided into two sub-stages.

---

## 8. Verification
