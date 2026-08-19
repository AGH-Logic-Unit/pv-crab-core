---
title: MMU & Address Translation Overview
---

# MMU & Address Translation Overview

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Overview

The **Memory Management Unit (MMU)** provides hardware-enforced virtual memory management and physical security mechanisms for the 64-bit Crab Core. It implements the standard **RISC-V Sv39** multi-level page translation scheme, Physical Memory Protection (PMP), Physical Memory Attributes (PMA), and hardware page-table walk acceleration.

The MMU is partitioned into modular execution blocks located in **Stage 6 (Memory / Translation)**:

* **Instruction TLB (I-TLB):** 16-entry fully associative CAM for zero-cycle/single-cycle instruction fetch translation.
* **Data TLB (D-TLB):** 32-entry fully associative CAM for single-cycle data address translation and PMP/PMA access checks.
* **Hardware Page Table Walker (PTW):** Centralized 3-level Sv39 state machine serving translation refill requests from both I-TLB and D-TLB with starvation prevention.
* **PMP / PMA Checker:** Evaluates 16 PMP regions and hardwired PMA memory attributes at refill time.

## 2. Architectural Requirements

The MMU complies with the following RISC-V Privileged Architecture specifications (v1.12):

* **Sv39 Virtual Memory:** 39-bit Virtual Address space ($VA[38:0]$) mapped to 56-bit Physical Address space ($PA[55:0]$).
* **Page Granularities:** 4 KiB Base Pages, 2 MiB Megapages, and 1 GiB Gigapages.
* **Privilege Levels:** User (`U-mode`), Supervisor (`S-mode`), and Machine (`M-mode`).
* **Hardware Protection:**
    * 16-Entry Physical Memory Protection (PMP) supporting `OFF`, `TOR`, `NA4`, `NAPOT` addressing modes.
    * Physical Memory Attributes (PMA) defining Cacheability and 4-level Atomic (AMO) support.
    * `Svpbmts` extension support for page-based memory type attributes (`PTE[62:61]`).
* **Address Space ID (ASID):** 16-bit ASID filtering in TLB CAM tags with Global mapping (`G`) support.

## 3. Subsystem Architecture

```
                       +---------------------------------------+
                       |              MMU Subsystem            |
                       |                                       |
                       |  +---------------+ +---------------+  |
   Fetch VA ---------->|  |  I-TLB (16)   | |  D-TLB (32)   |  |<---------- LSU VA (Stage 6)
                       |  +-------+-------+ +-------+-------+  |
                       |          |                 |          |
                       |          v                 v          |
                       |  +---------------------------------+  |
                       |  |    Hardware Page Table Walker   |  |
                       |  |            (Sv39 PTW)           |  |
                       |  +----------------+----------------+  |
                       |                   |                   |
                       |                   v                   |
                       |  +---------------------------------+  |
                       |  |      PMP & PMA Pre-Evaluator    |  |
                       |  +---------------------------------+  |
                       +-------------------|-------------------+
                                           |
                                           v (ptw_* ports)
                                  [ L1 Data Cache ]
```

## 4. Subsystem Interfaces

### 4.1 Clock, Reset and CSR Control Interface

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Main core clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `flush_i` | `logic` | 1 | IN | Global pipeline flush (aborts active PTW walk in 1 cycle) |
| `curr_priv_i` | `priv_mode_t` | 2 | IN | Current processor privilege level (`M`, `S`, `U`) |
| `satp_i` | `satp_reg_t` | 64 | IN | Supervisor Address Translation and Protection register (`MODE`, `ASID`, `PPN`) |
| `mstatus_i` | `mstatus_reg_t` | 64 | IN | Machine Status register (`MPRV`, `MPP`, `SUM`, `MXR`) |
| `pmp_write_valid_i` | `logic` | 1 | IN | 70-bit broadcast bus: valid write pulse on PMP CSR commit |
| `pmp_write_idx_i` | `logic` | 5 | IN | 70-bit broadcast bus: target register index (`0..3` cfg, `0..15` addr) |
| `pmp_write_data_i` | `logic` | 64 | IN | 70-bit broadcast bus: write payload data |

### 4.2 I-TLB Subsystem Interface

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `itlb_lookup_priv_mode_i`| `priv_mode_t` | 2 | IN | Current processor privilege level for fetch permission validation |
| `itlb_miss_valid_i` | `logic` | 1 | IN | Asserted when I-TLB misses during instruction fetch |
| `itlb_miss_va_i` | `logic` | 64 | IN | Faulting virtual address for instruction fetch |
| `itlb_refill_valid_o` | `logic` | 1 | OUT | 1-cycle strobe indicating valid translation refill payload |
| `itlb_refill_vpn_o` | `logic` | 27 | OUT | Virtual page number tag (`VA[38:12]`) |
| `itlb_refill_page_size_o`| `page_size_t` | 2 | OUT | Refilled page size (`PAGE_4K`, `PAGE_2M`, `PAGE_1G`) |
| `itlb_refill_ppn_o` | `logic` | 44 | OUT | Physical page number translation payload (`PA[55:12]`) |
| `itlb_refill_flags_o` | `pte_flags_t` | 8 | OUT | Page permission flags (`R`, `W`, `X`, `U`, `G`, `A`, `D`, `PBMT`) |
| `itlb_refill_pmp_fault_o`| `logic` | 1 | OUT | Pre-evaluated PMP permission fault flag cached in TLB entry |
| `itlb_refill_pma_cacheable_o`| `logic` | 1 | OUT | Pre-evaluated PMA cacheability attribute |
| `itlb_page_fault_o` | `logic` | 1 | OUT | Strobe asserting instruction page fault exception |
| `itlb_page_fault_cause_o` | `logic` | 6 | OUT | Exception cause code (`cause = 12`) |

### 4.3 D-TLB Subsystem Interface (and LSU `tlbpm_*` Mapping)

#### D-TLB Runtime Lookup & PTW Refill Ports:

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `dtlb_lookup_valid_i` | `logic` | 1 | IN | Lookup request valid strobe from Stage 6 LSU |
| `dtlb_lookup_va_i` | `logic` | 64 | IN | Target virtual address (`VA[63:0]`) to translate |
| `dtlb_req_r_i` | `logic` | 1 | IN | Access requires Read permission (Load / AMO) |
| `dtlb_req_w_i` | `logic` | 1 | IN | Access requires Write permission (Store / AMO) |
| `dtlb_lookup_ready_o` | `logic` | 1 | OUT | Response valid (`1` = TLB Hit / Ready, `0` = TLB Miss / Stalled) |
| `dtlb_lookup_pa_o` | `logic` | 56 | OUT | Translated 56-bit Physical Address (`PA[55:0]`) |
| `dtlb_err_r_o` | `logic` | 1 | OUT | Denied read permission (`cause = 5` Access Fault or `13` Page Fault) |
| `dtlb_err_w_o` | `logic` | 1 | OUT | Denied write permission (`cause = 7` Access Fault or `15` Page Fault) |
| `dtlb_cause_r_o` | `logic` | 6 | OUT | Exact Read fault cause code (`5` = Load Access Fault, `13` = Load Page Fault) |
| `dtlb_cause_w_o` | `logic` | 6 | OUT | Exact Write fault cause code (`7` = Store/AMO Access Fault, `15` = Store/AMO Page Fault) |
| `dtlb_pma_cacheable_o` | `logic` | 1 | OUT | Pre-evaluated cacheability flag (`1` = DRAM, `0` = MMIO) |
| `dtlb_pma_amo_level_o` | `pma_amo_level_t` | 2 | OUT | Pre-evaluated atomic capability level (`None`, `Swap`, `Logical`, `Arith`) |

#### Cross-Module Port Mapping: LSU (`docs/uarch/execute/lsu/overview.md` §5.4) $\leftrightarrow$ D-TLB:

| LSU Port Name | D-TLB Port Name | Direction (re. LSU) | Width | Description |
| :--- | :--- | :---: | :---: | :--- |
| `tlbpm_valid_o` | `dtlb_lookup_valid_i` | OUT | 1 | Translation lookup strobe |
| `tlbpm_addr_o` | `dtlb_lookup_va_i` | OUT | 64 | Virtual Address ($VA[63:0]$) |
| `tlbpm_req_r_o` | `dtlb_req_r_i` | OUT | 1 | Read intent flag |
| `tlbpm_req_w_o` | `dtlb_req_w_i` | OUT | 1 | Write intent flag |
| `tlbpm_rvalid_i` | `dtlb_lookup_ready_o` | IN | 1 | TLB Hit / Ready acknowledgment |
| `tlbpm_addr_i` | `dtlb_lookup_pa_o` | IN | 56 | Physical Address ($PA[55:0]$) |
| `tlbpm_err_r_i` | `dtlb_err_r_o` | IN | 1 | Read permission fault flag |
| `tlbpm_err_w_i` | `dtlb_err_w_o` | IN | 1 | Write permission fault flag |
| `tlbpm_cause_r_i` | `dtlb_cause_r_o` | IN | 6 | Exact trap cause for read access (`5` vs `13`) |
| `tlbpm_cause_w_i` | `dtlb_cause_w_o` | IN | 6 | Exact trap cause for write access (`7` vs `15`) |
| `tlbpm_cacheable_i` | `dtlb_pma_cacheable_o` | IN | 1 | Cacheable DRAM indicator |
| `tlbpm_pma_amo_level_i`| `dtlb_pma_amo_level_o` | IN | 2 | Supported AMO capability level |

### 4.4 PTW Memory Access Interface (to L1 D-Cache)

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `ptw_req_o` | `logic` | 1 | OUT | Memory read request for PTE fetch issued to L1 D-Cache |
| `ptw_gnt_i` | `logic` | 1 | IN | Grant from L1 D-Cache port arbiter accepting address phase |
| `ptw_addr_o` | `logic` | 56 | OUT | Physical address of Page Table Entry (`PA[55:0]`) |
| `ptw_rvalid_i` | `logic` | 1 | IN | Data validation strobe indicating PTE data available |
| `ptw_rdata_i` | `logic` | 64 | IN | 64-bit raw Page Table Entry content from memory |
| `ptw_err_i` | `logic` | 1 | IN | Bus error or PMP violation response on PTE fetch |

### 4.5 Invalidation and Synchronization Interface

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `sfence_vma_valid_i` | `logic` | 1 | IN | Broadcast strobe from Commit executing `SFENCE.VMA` |
| `sfence_vma_asid_i` | `logic` | 16 | IN | Target ASID filter for TLB invalidation (`0` = all ASIDs) |
| `sfence_vma_addr_i` | `logic` | 64 | IN | Target virtual address filter for TLB invalidation (`0` = all VAs) |
| `itlb_flush_o` | `logic` | 1 | OUT | Invalidation pulse routed to I-TLB array |
| `dtlb_flush_o` | `logic` | 1 | OUT | Invalidation pulse routed to D-TLB array |

### 4.6 Internal MMU Signals (D-TLB $\leftrightarrow$ Hardware PTW)

| Signal | Type | Width | Source $\rightarrow$ Dest | Description |
| :--- | :---: | :---: | :---: | :--- |
| `dtlb_ptw_miss_valid` | `logic` | 1 | D-TLB $\rightarrow$ PTW | Request strobe initiating Page Table Walk upon D-TLB lookup miss |
| `dtlb_ptw_miss_va` | `logic` | 64 | D-TLB $\rightarrow$ PTW | Faulting Virtual Address ($VA[63:0]$) |
| `dtlb_ptw_miss_op` | `logic` | 2 | D-TLB $\rightarrow$ PTW | Memory intent: `00` = Read (Load), `01` = Write (Store), `10` = Atomic (AMO) |
| `ptw_dtlb_refill_valid`| `logic` | 1 | PTW $\rightarrow$ D-TLB | 1-cycle write strobe pushing translated entry into D-TLB CAM |
| `ptw_dtlb_refill_vpn` | `logic` | 27 | PTW $\rightarrow$ D-TLB | Virtual Page Number ($VA[38:12]$) |
| `ptw_dtlb_refill_pa` | `logic` | 54 | PTW $\rightarrow$ D-TLB | Physical Page Tag ($PA[55:2]$) |
| `ptw_dtlb_refill_mask` | `logic` | 37 | PTW $\rightarrow$ D-TLB | Effective comparison mask ($M_{\text{eff}}[36:0]$) |
| `ptw_dtlb_refill_flags`| `pte_flags_t` | 8 | PTW $\rightarrow$ D-TLB | Leaf PTE permission flags (`R, W, X, U, G, A, D, PBMT`) |
| `ptw_dtlb_pmp_fault` | `logic` | 1 | PTW $\rightarrow$ D-TLB | Pre-evaluated PMP permission fault bit |
| `ptw_dtlb_cacheable` | `logic` | 1 | PTW $\rightarrow$ D-TLB | Pre-evaluated PMA cacheability attribute |
| `ptw_dtlb_amo_level` | `pma_amo_level_t`| 2 | PTW $\rightarrow$ D-TLB | Pre-evaluated PMA atomic operation capability level |
| `ptw_dtlb_page_fault` | `logic` | 1 | PTW $\rightarrow$ D-TLB | Translation abort due to invalid PTE, misaligned superpage, or permission fault |
| `ptw_dtlb_fault_cause`| `logic` | 6 | PTW $\rightarrow$ D-TLB | Exact RISC-V cause code (`13` = Load, `15` = Store/AMO Page Fault) |

## 5. Functional Description

### 5.1 Memory Access & Pre-Evaluated Protection Flow

```mermaid
sequenceDiagram
    autonumber
    participant LSU as LSU (Stage 6)
    participant DTLB as D-TLB (32 CAM)
    participant MMU as MMU / PTW
    participant DCACHE as L1 D-Cache

    LSU->>DTLB: Lookup VA[38:0]
    alt TLB Hit (Single Cycle)
        DTLB-->>LSU: Return PA[55:0] + Precomputed PMP Fault + PMA AMO Level
        alt PMP Fault || Permission Error
            LSU->>LSU: Trigger Exception (Access Fault / Page Fault)
        else Access Allowed
            LSU->>DCACHE: Send Memory Request (dmem_addr = PA[55:0])
        end
    else TLB Miss
        DTLB->>MMU: Assert dtlb_miss_valid_i (Faulting VA)
        Note over MMU: PTW 3-Level Sv39 Walk + PMP/PMA Check on PTEs
        MMU-->>DTLB: dtlb_refill_valid_o (PA + Pre-evaluated PMP/PMA Flags)
        DTLB->>DTLB: Allocate Entry via Tree-PLRU
        DTLB-->>LSU: Replay Lookup -> Return PA + Flags
    end
```

!!! note "Elimination of PMP from Critical Path"
    Because Physical Memory Protection (PMP) rules are pre-evaluated at Page Table Walk refill time and cached inside the TLB entry (`pmp_fault` flag), runtime TLB lookups do not invoke the 16-entry PMP comparator bank. This design meets the 10 ns clock budget on the 130nm ASIC technology.

### 5.2 Translation Mode & Enable Logic

Address translation and TLB lookup enablement is governed by CPU privilege and `satp` configuration:

```systemverilog
// Effective Privilege Level (accounting for mstatus.MPRV for data accesses)
assign eff_priv_d = (mstatus_i.mprv && (curr_priv_i == PRIV_MACHINE)) ? mstatus_i.mpp : curr_priv_i;

// Translation Enable Flags
assign itlb_trans_en_o = (curr_priv_i != PRIV_MACHINE) && (satp_i.mode == 4'd8);
assign dtlb_trans_en_o = (eff_priv_d != PRIV_MACHINE) && (satp_i.mode == 4'd8);
```

!!! note "Translation Bypass (BARE Mode)"
    When `itlb_trans_en_o == 0` or `dtlb_trans_en_o == 0`, address translation is bypassed (`PA == VA`). To maintain single-cycle PMP checks without multi-stage comparator logic, BARE mode accesses are cached in the TLB with `is_bare = 1`.

### 5.3 Page Table Walk Sequence (Sv39 3-Level Walk)

When an I-TLB or D-TLB miss occurs, the central Hardware PTW FSM initiates a page table walk:

```mermaid
sequenceDiagram
    autonumber
    participant TLB as TLB (I/D)
    participant PTW as Central PTW FSM
    participant RAM as L1 D-Cache / Memory

    TLB->>PTW: TLB Miss (Faulting VA)
    Note over PTW: Level 2 Walk (Gigapage)
    PTW->>RAM: Read PTE at Root (satp.PPN + VPN[2]*8)
    RAM-->>PTW: Return PTE2
    alt PTE2 is Leaf (R=1 || X=1)
        PTW->>TLB: Refill 1GiB Gigapage Entry (PMP/PMA Pre-evaluated)
    else PTE2 is Pointer (V=1, R=0, W=0, X=0)
        Note over PTW: Level 1 Walk (Megapage)
        PTW->>RAM: Read PTE at (PTE2.PPN + VPN[1]*8)
        RAM-->>PTW: Return PTE1
        alt PTE1 is Leaf (R=1 || X=1)
            PTW->>TLB: Refill 2MiB Megapage Entry (PMP/PMA Pre-evaluated)
        else PTE1 is Pointer
            Note over PTW: Level 0 Walk (4KiB Base Page)
            PTW->>RAM: Read PTE at (PTE1.PPN + VPN[0]*8)
            RAM-->>PTW: Return PTE0
            PTW->>TLB: Refill 4KiB Base Page Entry (PMP/PMA Pre-evaluated)
        end
    end
```

### 5.4 Page Fault Exception Handling

The MMU triggers a Page Fault exception to the committing stage when any of the following conditions occur during a page table walk:

1. **Invalid Entry (`V == 0`):** Any level PTE has valid bit cleared.
2. **Write Without Read (`W == 1 && R == 0`):** Reserved bit combination in RISC-V specification.
3. **Misaligned Superpage:** A leaf PTE at Level 2 has `PPN[1:0] != 0` or at Level 1 has `PPN[0] != 0`.
4. **Privilege Fault:**
    * User mode (`U`) accessing a Supervisor page (`U == 0`).
    * Supervisor mode (`S`) accessing a User page (`U == 1`) when `mstatus.SUM == 0`.
5. **Access Permission Fault:**
    * Fetching an instruction from a non-executable page (`X == 0`).
    * Loading data from a non-readable page (`R == 0` and `mstatus.MXR == 0`).
    * Storing data to a non-writable page (`W == 0`).
6. **Accessed / Dirty Bit Fault:**
    * Reading a page with `A == 0` (hardware A/D update not enabled).
    * Writing a page with `D == 0` (hardware A/D update not enabled).

### 5.5 Atomic Memory Operation (AMO) Verification Rules in MMU

Because Atomic Memory Operations perform a Read-Modify-Write (RMW) cycle, the MMU enforces strict combined permission and PMA capability rules:

1. **PTE Page Permissions:** The leaf Page Table Entry **must have BOTH Read (`R == 1`) AND Write (`W == 1`) permissions**. If either bit is `0`, the MMU raises a **Store/AMO Page Fault (`cause = 15`)**.
2. **PTE A/D Bits:** The leaf PTE **must have BOTH Accessed (`A == 1`) AND Dirty (`D == 1`) bits set**. If `A == 0` or `D == 0`, the MMU raises a **Store/AMO Page Fault (`cause = 15`)**.
3. **PMP Permission Bounds:** PMP evaluation for AMOs **requires BOTH Read (`R`) AND Write (`W`) PMP permissions**. If either permission is missing, the MMU raises a **Store/AMO Access Fault (`cause = 7`)**.
4. **PMA Capability Verification:** The physical address target must satisfy the required `pma_amo_level_t`:
    * `LR/SC` and arithmetic AMOs (`AMOADD`, `AMOMIN`, etc.) require `PMA_AMO_ARITHMETIC`.
    * Logical AMOs (`AMOAND`, `AMOOR`, `AMOXOR`) require `PMA_AMO_LOGICAL` or higher.
    * `AMOSWAP` requires `PMA_AMO_SWAP` or higher.
    * If the region has `PMA_AMO_NONE` (or insufficient capability), the core raises a **Store/AMO Access Fault (`cause = 7`)**.

## 6. Verification

The MMU subsystem is verified through formal property verification (SVA) and random SV39 test environments:

### Formal SVA Assertion Table

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_MMU_01` | `(flush_i && ptw_busy) |=> (!ptw_busy && !dtlb_refill_valid_o)` | `FATAL` | **CHK-MMU-07** | Pipeline flush aborts active PTW walk in 1 cycle and suppresses refill strobe |
| `SVA_MMU_02` | `pmp_write_valid_i |=> (tlb_bare_valid_count == '0)` | `FATAL` | **CHK-MMU-05** | PMP CSR write must immediately invalidate all cached BARE TLB entries |
| `SVA_MMU_03` | `(sfence_vma_valid_i && (is_bare || pte_flags.G)) |=> (entry_valid)` | `ERROR` | **CHK-MMU-05** | SFENCE.VMA must never invalidate Global or BARE mode TLB entries |
| `SVA_MMU_04` | `(itlb_miss && dtlb_consecutive >= 4) |-> ##[1:2] (ptw_grant == TARGET_ITLB)` | `ERROR` | **CHK-MMU-08** | I-TLB starvation counter must enforce PTW grant after 4 consecutive D-TLB walks |
| `SVA_MMU_05` | `(is_amo && (pma_amo_level < req_level)) |-> (dtlb_err_w_o && cause == 7)` | `FATAL` | **CHK-MMU-09** | AMO to unsupported PMA region must trigger Store/AMO Access Fault (`cause = 7`) |
| `SVA_MMU_06` | `(ptw_pte_pmp_err) |-> (ptw_state == PTW_ACCESS_FAULT)` | `FATAL` | **CHK-MMU-10** | PTE read violating PMP must trigger an Access Fault during walk |

### Coverage Strategy
1. **Translation Coverage:** 100% functional coverage of 4 KiB Base Pages, 2 MiB Megapages, and 1 GiB Gigapages.
2. **Permission Matrices:** Cross-coverage of `U`, `S`, `M` modes against `R/W/X`, `SUM`, `MXR`, and `MPRV` bits.
3. **PMP Boundary Invariant:** Exhaustive testing of NAPOT, NA4, and TOR PMP rules against 4 KiB page boundaries.
