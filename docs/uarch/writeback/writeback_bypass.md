---
title: Writeback & Register Bypass Specs
---

!!! abstract "Module Card"
    * :material-progress-wrench: **Status:** `Not ratified - notes only`

# Writeback & Register Bypass Specification

## 1. Overview
The Crab Core execution stage implements a decoupled execution completion pipeline with in-order dispatch and in-order retirement. To maximize performance while minimizing area and timing complexity, the core utilizes a Scoreboard to track register dependencies and a simplified, low-overhead register forwarding (bypass) network.

Instead of complex multi-entry multiplexing from the writeback/retirement buffers, the forwarding network directly connects the outputs of the execution modules to the Dispatch stage.

---

## 2. Register Scoreboard & Tracking

To track outstanding writes to the Register File (GPR/FPR):

* **Pending Status (Busy Bit):** Each register is associated with a single pending (busy) bit in the Scoreboard.
* **Allocation:** When an instruction is dispatched, its destination register `rd` is marked as pending (busy).
* **Commit:** When the instruction completes writeback and commits to the architectural Register File at the Retirement stage (Stage 9), the pending bit is cleared.

---

## 3. Decoupled Writeback & Stalls

* **Prioritized Arbitration:** Completed results from execution units compete for writeback through a prioritized Writeback Arbiter.
* **Stalling:** If multiple units complete in the same cycle and the arbiter denies writeback to a unit, that unit's output interface stalls (`wb_ready_i` is driven low). The unit must hold its output valid (`wb_valid_o`) and result data (`wb_result_o`) stable.
* **Stall Propagation:** Stalls propagate backward to freeze pipeline registers or FSM states, eventually deasserting `disp_ready_o` to the dispatcher.

---

## 4. Register Bypassing (Forwarding)

To maximize Instruction Per Cycle (IPC) performance, a simple, direct forwarding network is implemented from the execution module outputs back to the Dispatcher's operand selection multiplexers.

Unlike complex designs, operands are **not** forwarded from the Retirement FIFO (Stage 9) or the Writeback Buffer. Consequently, bypassed data is only available for a limited window.

### 4.1 0-Cycle Combinational ALU Bypass
* Since the ALU is purely combinational, its output is available in the same cycle as dispatch.
* The combinational output is routed directly back to the dispatcher's operand multiplexers, allowing back-to-back dependent ALU instructions to execute without stalls.

### 4.2 1-Cycle Execute Output Forwarding
* The outputs of all stateful execution modules (Multiplier, Divider, FPU, LSAMO) are connected directly back to the Dispatch stage operand selection multiplexers.
* When an execution unit completes its operation and drives its result on `wb_result_o` (valid when `wb_valid_o` is asserted), a dependent instruction in the Dispatch stage (Stage 7) can read this value directly.
* **1-Cycle Window Constraint:** The bypassed data is **only available for exactly one cycle**—the cycle in which the execution unit completes and exposes its result.
* **Valid Flag Clearing:** This 1-cycle availability window is a direct consequence of the writeback handshake. When the Writeback Arbiter reads/accepts the result from the execution unit to buffer it into the Retirement FIFO (Stage 9), the execution unit immediately clears its `wb_valid_o` flag in the next clock cycle. This prevents the forwarding network from pulling duplicate or stale data from completed units.
* If the dependent instruction in the Dispatch stage cannot be dispatched in that exact cycle (due to structural stalls, branch mispredict flushes, or other hazards), the result is buffered into the Retirement FIFO (Stage 9) and is **no longer available** on the forwarding network. In this case, the dependent instruction must stall until the writing instruction commits (retires) and writes its result to the Register File, after which it is read normally from the Register File.

---

## 5. Physical Layout & Area Efficiency

By restricting bypass forwarding inputs strictly to:

1. The combinational ALU output (0-cycle).
2. The active execution module outputs (`wb_result_o` from active units).

The core eliminates multiplexing logic from internal pipeline stages and the Writeback Buffer/Retirement FIFO. This direct forwarding network significantly reduces silicon area, cuts down wire routing complexity, and simplifies timing closure in the Dispatch-to-Execute path.
