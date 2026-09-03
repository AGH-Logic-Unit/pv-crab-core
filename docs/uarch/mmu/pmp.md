---
title: Physical Memory Protection (PMP)
---

# Physical Memory Protection (PMP) Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Overview

The **Physical Memory Protection (PMP)** unit provides per-hart physical address filtering to isolate memory regions between execution privilege levels. It checks physical addresses (`PA`) against up to 16 configured region entries and enforces Read (`R`), Write (`W`), and Execute (`X`) permission rules.

## 2. Shadow PMP Registers & Broadcast Interface

To avoid routing 992 parallel output bits across the chip floorplan from Stage 7 Commit to the memory subsystem, the MMU maintains local **Shadow PMP Registers**. Updates from Stage 7 Commit are received via a narrow 70-bit broadcast write bus:

| Signal Name | Width | Direction | Description |
| :--- | :---: | :---: | :--- |
| `pmp_write_valid_i` | 1 | IN | Broadcast write pulse triggered when a PMP CSR is written |
| `pmp_write_idx_i` | 5 | IN | Target PMP register index (`0..3` for `pmpcfg`, `0..15` for `pmpaddr`) |
| `pmp_write_data_i` | 64 | IN | Write payload value |

!!! note "Physical Timing Budget (130nm ASIC)"
    On the IHP SG13G2 130nm process ($T_{clk} = 10\text{ ns}$), cross-chip propagation from Stage 7 Commit to Stage 6 MMU across ~2–3 mm metal tracks is estimated at $\sim 0.8\text{–}1.5\text{ ns}$, with register setup taking $\sim 1.1\text{ ns}$. The total path delay ($\sim 2.4\text{–}3.1\text{ ns}$) easily meets timing with $>6.5\text{ ns}$ of positive slack.

## 3. PMP Configuration Entry Layout

Each PMP entry consists of an 8-bit configuration field (`pmpcfgI`) and a 54-bit address bound field (`pmpaddrI` representing `PA[55:2]`):

| Bits | Field | Description |
| :--- | :---: | :--- |
| `7` | `L` | Lock bit. When `1`, rules apply to M-mode and configuration is locked until reset. |
| `6:5` | - | Reserved (must be `0`). |
| `4:3` | `A` | Address Matching Mode (`00`=OFF, `01`=TOR, `10`=NA4, `11`=NAPOT). |
| `2` | `X` | Executable permission bit. |
| `1` | `W` | Writable permission bit. |
| `0` | `R` | Readable permission bit. |

### Address Matching Modes (`A`):
1. **`OFF` (`00`):** Entry disabled (bypassed).
2. **`TOR` (`01` - Top of Range):** Entry forms range $[ \text{pmpaddr}_{I-1} \le \text{PA} < \text{pmpaddr}_I )$.
3. **`NA4` (`10` - Naturally Aligned 4-byte):** Matches exact 4-byte address $\text{PA}[55:2] == \text{pmpaddr}_I$.
4. **`NAPOT` (`11` - Naturally Aligned Power-of-Two):** Matches power-of-two region sizes $\ge 8$ bytes, encoded via trailing 1s in `pmpaddrI`.

## 4. Arbitration & Priority Rules

* **Lowest Index Priority:** Entries are evaluated in parallel. The **matching entry with the lowest index (0 to 15)** determines access permission.
* **M-mode Default Access:** If no PMP entry matches:
    * In **M-mode** (`L=0`): Access is **granted** by default.
    * In **S-mode / U-mode**: Access is **denied** by default (raises Access Fault).
* **Locked Entries (`L=1`):** Enforces PMP permission checks on M-mode accesses as well.

## 5. Pre-Evaluation and TLB Integration

PMP verification is performed **at Page Table Walk refill time** (or during BARE mode allocation) and cached inside the TLB entry (`pmp_fault` flag). The entry coverage is constrained to $\min(\text{Page Size}, \text{PMP Region})$. Runtime TLB lookups do not invoke PMP comparators, eliminating PMP evaluation from the critical timing path.

## 6. Exception Cause Mapping

* **Fetch Access Violation:** Raises `Instruction Access Fault` (`cause = 1`).
* **Load Access Violation:** Raises `Load Access Fault` (`cause = 5`).
* **Store / SC / AMO Access Violation:** Raises `Store/AMO Access Fault` (`cause = 7`).
