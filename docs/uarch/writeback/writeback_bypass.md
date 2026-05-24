---
title: Writeback & Register Bypass Specs
---

# Writeback & Register Bypass Specification

## 1. Overview
The Crab Core execution stage implements a decoupled, out-of-order execution completion pipeline with in-order dispatch. Because execution units have varying latencies (combinational, pipelined, or multi-cycle FSM), the core utilizes a Scoreboard to track register dependencies and a central Writeback Buffer. A dynamic register bypass network is implemented to prevent dependency stalls from degrading performance.

---

## 2. Register Scoreboard & Tracking

To track outstanding writes to the Register File (GPR/FPR):
* **Override Flag:** Each register is associated with an `override` (or pending) status bit.
* **Instruction Tag:** Each register tracks a `tag` (4 bits) matching the instruction allocated to write to it.
* **Allocation:** When an instruction is dispatched, its destination register `rd` is marked as pending, and its `tag` is registered.
* **Commit:** When the instruction completes writeback and commits to the register file, the pending bit is cleared.

---

## 3. Decoupled Writeback & Stalls

* **Prioritized Arbitration:** Completed results from execution units compete for writeback through a prioritized Writeback Arbiter.
* **Stalling:** If multiple units complete in the same cycle and the arbiter denies writeback to a unit, that unit's output interface stalls (`wb_ready_i` is driven low). The unit must hold its output valid (`wb_valid_o`) and result data (`wb_result_o`) stable.
* **Stall Propagation:** Stalls propagate backward to freeze pipeline registers or FSM states, eventually deasserting `disp_ready_o` to the dispatcher.

---

## 4. Register Bypassing (Forwarding)

To maximize Instruction Per Cycle (IPC) performance, operands are bypassed directly to the dispatcher's execution stage:

### 4.1 0-Cycle Combinational ALU Bypass
* Since the ALU is purely combinational, its output is available in the dispatch cycle.
* The combinational output is routed directly back to the dispatcher's operand multiplexers. This allows back-to-back dependent ALU instructions to execute without incurring any stall cycles.

### 4.2 Output Register Bypassing (1-Cycle Latency)
* If an execution unit completes but is stalled by the arbiter, its result sits in its output register (`wb_result_o`).
* The bypass network monitors these unread output registers. If a dispatched instruction depends on a pending register and its tag matches the tag of an active output register, the operand is bypassed directly from `wb_result_o`.

### 4.3 Writeback Buffer Bypassing
* Completed instructions waiting in the Writeback Buffer to write to the Register File also participate in the bypass network. Operands are forwarded from matching tag entries in the Writeback Buffer.

---

## 5. Physical Layout & Area Efficiency

By restricting bypass forwarding inputs to only:
1. The Writeback Buffer entries (typically 2–4 slots).
2. The active execution unit output registers (`wb_result_o`).
3. The combinational ALU output.

The core avoids routing bypass lines from the middle of deep internal pipelines (like the 3-stage multiplier or FPU). This centralized bypass multiplexing structure minimizes silicon area and simplifies timing closure.
