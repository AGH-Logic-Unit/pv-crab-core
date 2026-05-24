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

The core utilizes a **Core-Side RMW** execution model to maintain low AMO latencies:
* The LSO-AMO module performs the atomic calculations locally.
* On an AMO hit, the L1 D-Cache controller ensures the line is in Exclusive or Modified state. The local FSM performs the read, logic modification, and writeback within the L1 cache.
* If a miss occurs, the cache line is loaded into L1 first, and then the RMW sequence is completed.

---

## 4. Load-Reserved / Store-Conditional (LR/SC) Reservation

To manage memory atomicity and prepare the core for multi-hart coherence:

* **Local Reservation Controller:** In single-hart operation, reservations are registered and tracked inside the core's local logic.
* **Cache-Line Granularity:** Reservations are tracked at the **cache-line boundary** (comparing only the upper address bits `[63:6]` for a 64-byte line) instead of exact byte matches.
* **Coherence Snoop Invalidation:** When the core receives an external snoop invalidation indicating another controller has written to the cached address, the D-Cache controller asserts `disp_snoop_invalidate_valid_i` along with the address to invalidate the reservation.
* **Context Clear:** The reservation is automatically cleared on interrupts, context switches, or any standard store instructions.

---

## 5. Line Locking Protocol (`dmem_lock_o`)

During active AMO FSM sequences, the LSO-AMO module asserts the `dmem_lock_o` signal.
* When `dmem_lock_o` is asserted, the L1 D-Cache controller must lock the targeted cache line.
* The cache controller blocks external snoop requests and prevents cache-line evictions for the locked line.
* The lock is held through the read request, modify, and writeback phases, and is released only when `dmem_lock_o` is deasserted.
