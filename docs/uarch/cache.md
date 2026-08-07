---
title: L1 Data Cache Design Specs
---

# L1 Data Cache Design Specification

## 1. Overview
This document specifies the microarchitectural design requirements and policies for the Crab Core's Level 1 Data Cache (L1 D-Cache). The L1 D-Cache is tightly coupled to the CPU core's Load-Store and Atomic Memory Operations (LSO-AMO) module to provide low-latency memory accesses, atomic execution, and hardware compatibility hooks for future multi-hart scalability.

---

## 2. Cache Policies

### 2.1 Write-Back Policy
To prevent DRAM or system bus write latency from stalling the execution pipelines:
* The cache operates on a **Write-Back** policy. Standard stores and the writeback phase of atomic memory operations are recorded locally in the L1 D-Cache line.
* Cache lines are written to main memory only during dirty line eviction.
* A **Write-Allocate** fetch policy is used on store misses, bringing the target line into the cache before modifying it.

### 2.2 Cache Line Locking
* The cache line is locked during local multi-cycle atomic sequences (like AMOs) to prevent external evictions or read/write snoop actions from breaking RMW atomicity.

---

## 3. Atomic Operations (AMO)

The system utilizes a **Cache-Side RMW** execution model to simplify the core execution stages and optimize bus transactions:
* The L1 D-Cache controller performs the atomic calculations and RMW sequence locally.
* The LSO-AMO module in the core sends the target physical address, operand data, and operation code (`dmem_amo_op_o` carrying the 5-bit RISC-V `funct5` value) to the L1 D-Cache.
* The L1 D-Cache controller reads the target word, sends the original (unmodified) data back to the core on `dmem_rdata_i` (to write back to the destination GPR/FPR), applies the arithmetic/logical operation, and writes the modified result back to the cache line in a single atomic sequence.
* If a cache miss occurs, the L1 Cache controller loads the targeted line first (using standard Write-Allocate fetch) before performing the atomic RMW sequence.

---

## 4. Load-Reserved / Store-Conditional (LR/SC) Reservation

To manage memory atomicity and prepare the core for multi-hart coherence:
* **Local Reservation Controller:** In single-hart operation, reservations are registered and tracked inside the core's local logic.
* **Cache-Line Granularity:** Reservations are tracked at the **cache-line boundary** (comparing only the upper address bits `[63:6]` for a 64-byte line) instead of exact byte matches.
* **Coherence Snoop Invalidation:** When the core receives an external snoop invalidation indicating another controller has written to the cached address, the D-Cache controller asserts `disp_snoop_invalidate_valid_i` along with the address to invalidate the reservation.
* **Context Clear:** The reservation is automatically cleared on interrupts, context switches (specifically any write to the `satp` address translation CSR), or any standard store instructions executed by the local hart.

---

## 5. Line Locking Protocol (Deprecated)

With the transition to **Cache-Side RMW**, the physical `dmem_lock_o` signal at the core-to-cache interface is **deprecated**:
* Atomicity is managed internally by the L1 D-Cache controller's state machine.
* The L1 D-Cache controller locks the targeted cache line from external evictions or snoop invalidations during its internal hit/miss/modify cycles, guaranteeing that no other agent can modify the line until the local RMW is completed.

---

## 6. MMIO Bypass Routing (Non-Cacheable Accesses)

To support peripherals (UART, USB, SPI) and memory-mapped configuration registers, the L1 D-Cache controller implements a **PMA-based Bypass Route**:

* **PMA Check:** Any access where the Physical Address is in the MMIO range (typically `PA < 0x8000_0000`) is classified as non-cacheable.
* **Cache Array Isolation:** For non-cacheable accesses, the cache controller does not read the tag arrays or allocate lines in the data SRAM.
* **Direct Bus Routing:** The transaction is routed directly to the external system bus (Wishbone B4 Master) as a single-beat transfer with the size matching the original request (byte, halfword, word, or doubleword).
* **Synchronization:** The cache controller waits for the external Wishbone bus acknowledgment (`ACK`) and forwards it directly to the core on `dmem_rvalid_i` along with the read data.

---

## 7. Cache Maintenance Operations (DMA Coherence)

Because the L1 D-Cache does not implement hardware bus snooping for DMA transfers in LiteX, the cache controller must provide a hardware interface for **Software-Managed Coherence**:

### 7.1 Interface CSR Registers
The cache controller exposes three memory-mapped registers (or custom CSRs) to the CPU:
1. **`DCACHE_CLEAN_ADDR`:** Specifies the starting physical address of the range to be cleaned.
2. **`DCACHE_INVALIDATE_ADDR`:** Specifies the starting physical address of the range to be invalidated.
3. **`DCACHE_MAINT_LEN`:** Specifies the length of the range (in bytes) and triggers the operation when written.

### 7.2 Operation Flow
* **Cache Clean (Flush):**
  * When `DCACHE_MAINT_LEN` is written after setting `DCACHE_CLEAN_ADDR`, the cache controller steps through the target cache lines.
  * If a line is **hit** and marked **dirty**, the controller schedules an eviction, writing the line back to main memory (DDR/HyperRAM) and clearing the dirty bit.
* **Cache Invalidate:**
  * When `DCACHE_MAINT_LEN` is written after setting `DCACHE_INVALIDATE_ADDR`, the cache controller steps through the target cache lines.
  * The controller clears the valid bit (`valid = 0`) of all matching cache lines in the specified address range, discarding their cached contents.
* **Stalling:** During a Cache Maintenance operation, the cache controller deasserts `dmem_gnt_i` to the CPU core, stalling new memory requests until the operation completes.
