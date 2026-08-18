---
title: Port Arbiter & Subsystem Interface
---

# LSU Port Arbiter & Subsystem Interface

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Overview

The **LSU Port Arbiter** is the top-level interface block inside the Load-Store Unit. It multiplexes access to the single L1 Data Cache / MMU memory port between two competing internal sources:
1. **LSO FSM:** Executing speculative loads and retirement-stage AMOs.
2. **Store Buffer (STB):** Draining committed, non-speculative store entries to memory in the background.

## 2. Subsystem Interface Definition (D-Cache)

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `dmem_req_o` | `logic` | 1 | OUT | Memory access request validation flag |
| `dmem_gnt_i` | `logic` | 1 | IN | Memory grant signal (address phase accepted by D-Cache/MMU) |
| `dmem_addr_o` | `logic` | 64 | OUT | Physical target address |
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
  Pipeline ──────>|  |   LSO-AMO   |  |   Store    |  |
  Inputs          |  |     FSM     |  |Buffer (STB)|  |
                  |  +------+------+  +-----+------+  |
                  |         |               |         |
                  |         v               v         |
                  |      +---------------------+      |
                  |      | LSU Port Arbiter    |      |
                  |      +----------+----------+      |
                  +-----------------|-----------------+
                                    |
                                    v (dmem_* ports)
                            [ L1 Data Cache ]
```

### 3.1 Priority Levels

Under normal operation, the LSU Port Arbiter enforces a fixed-priority scheme:

1. **Priority 1 (High):** `LSO FSM` requests (Speculative Loads and retirement-stage AMOs).
2. **Priority 2 (Low):** `Store Buffer (STB)` drain requests (Committed non-speculative stores).

### 3.2 STB Threshold Drain (Starvation Prevention)

* **Problem:** Under heavy load-stream workloads, granting Priority 1 continuously to `LSO FSM` would prevent STB from draining to memory. This causes STB to fill up (`stb_full == 1`), stalling Stage 5 Dispatch.
* **Solution (Threshold Drain):** The Arbiter monitors STB occupancy. When $\ge 75\%$ of STB entries are committed and pending drain (e.g. 3 of 4 slots occupied in a 4-entry STB), the Arbiter dynamically elevates STB drain priority to **High** until occupancy drops below $25\%$. This guarantees memory throughput and prevents pipeline stalls.
