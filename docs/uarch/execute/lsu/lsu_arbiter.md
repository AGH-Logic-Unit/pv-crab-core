---
title: Port Arbiter & Subsystem Interface
---

# LSU Port Arbiter & Subsystem Interface

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Overview

The **LSU Port Arbiter** is the top-level interface block inside the Load-Store Unit. It multiplexes access to the single L1 Data Cache / MMU memory port between two competing internal sources:

1. **LSU FSM:** Executing speculative loads and retirement-stage AMOs.
2. **Store Buffer (STB):** Draining committed, non-speculative store entries to memory in the background.

## 2. Subsystem Interface Definition (D-Cache)

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `dmem_req_o` | `logic` | 1 | OUT | Memory access request validation flag |
| `dmem_gnt_i` | `logic` | 1 | IN | Memory grant signal (address phase accepted by D-Cache/MMU) |
| `dmem_addr_o` | `logic` | 56 | OUT | Physical target address (`PA[55:0]`) |
| `dmem_we_o` | `logic` | 1 | OUT | Write enable (`1` = Write, `0` = Read) |
| `dmem_wdata_o` | `logic` | 64 | OUT | Write payload data (Store data or AMO operand B) |
| `dmem_be_o` | `logic` | 8 | OUT | Byte enable strobes for write masking |
| `dmem_size_o` | `logic` | 2 | OUT | Access size (`00`=Byte, `01`=Halfword, `10`=Word, `11`=Doubleword) |
| `dmem_amo_req_o` | `logic` | 1 | OUT | Active high flag indicating Atomic Memory Operation (Cache-Side RMW) |
| `dmem_amo_op_o` | `logic` | 5 | OUT | Encoded AMO operation (`funct5` code) |
| `dmem_rvalid_i` | `logic` | 1 | IN | Data response valid (read data returned or write acknowledged) |
| `dmem_rdata_i` | `logic` | 64 | IN | Read data payload (contains original value for AMOs) |
| `dmem_err_i` | `logic` | 1 | IN | Access error response (bus fault or access violation) |

## 3. Port Arbitration Rules & STB Starvation Protection

```
                  +-----------------------------------+
                  |      LSU (Load-Store Unit)        |
                  |                                   |
                  |  +-------------+  +------------+  |
  Pipeline ──────>|  |   LSU-AMO   |  |   Store    |  |
  Inputs          |  |     FSM     |  |Buffer (STB)|  |
                  |  +------+------+  +-----+------+  |
                  |         |               |         |
                  |         v               v         |
                  |      +---------------------+      |
                  |      |  LSU Port Arbiter   |      |
                  |      +----------+----------+      |
                  +-----------------|-----------------+
                                    |
                                    v (dmem_* ports)
                            [ L1 Data Cache ]
```

### 3.1 Priority Levels

Under normal operation, the LSU Port Arbiter enforces a dynamic two-tier priority scheme:

1. **Priority 1 (Default High):** `LSU FSM` requests (Speculative Loads and retirement-stage AMOs).
2. **Priority 2 (Default Low):** `Store Buffer (STB)` drain requests (Committed non-speculative stores).

### 3.2 STB Threshold Drain (Starvation Prevention & Deadlock Avoidance)

* **Problem:** Under continuous load-stream workloads, granting Priority 1 continuously to `LSU FSM` would starve STB from draining to memory. This would cause STB to fill up (`stb_full == 1`), stalling Stage 5 Dispatch.
* **Solution (Threshold Drain):** The Arbiter monitors STB occupancy:
    * **Elevate Priority to HIGH:** When total valid STB entries $\ge 3$ (for a 4-entry STB, $\ge 75\%$) AND at least one committed entry is pending drain (`committed_entries >= 1`), the Arbiter elevates STB drain to **Priority 1** over speculative loads.
    * **Drop Priority back to NORMAL:** Once total occupancy drops to $\le 1$ entry ($\le 25\%$) OR no committed entries remain, Priority 1 is returned to the LSU FSM.
    * **Hard Stall Safeguard:** When `stb_full_o == 1`, Dispatch is stalled, and committed entries are drained with top priority.

## 4. Verification

The LSU Port Arbiter is verified using formal property checking and constrained random simulation against the following assertions:

### Formal SVA Assertion Table

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_ARB_01` | `$bits(dmem_addr_o) == 56` | `FATAL` | **CHK-MMU-02** | Physical memory request address width must strictly equal 56 bits |
| `SVA_ARB_02` | `(lsu_req_i && !stb_elevate_prio) |-> (arb_grant_o == GRANT_FSM)` | `ERROR` | **CHK-LSU-04** | Default arbitration must grant Priority 1 to LSU FSM when STB is below threshold |
| `SVA_ARB_03` | `(stb_drain_req_i && stb_elevate_prio) |-> (arb_grant_o == GRANT_STB)` | `FATAL` | **CHK-LSU-04** | STB drain must take priority when STB occupancy $\ge 75\%$ with committed entries |
| `SVA_ARB_04` | `!(fsm_gnt_o && stb_gnt_o)` | `FATAL` | **CHK-MMU-02** | LSU FSM and STB must never be granted memory bus access simultaneously |

### Testbench Verification Matrix
1. **Load/Store Contention Tests:** Alternating back-to-back load bursts and store bursts to verify threshold escalation and de-escalation without packet loss.
2. **AMO Priority Tests:** Ensure committing AMO instructions obtain exclusive cache port access without interleaving with pending stores.
3. **Full Buffer Deadlock Prevention:** Verify that continuous store generation stalls dispatch cleanly and drains completely without dropping data.
