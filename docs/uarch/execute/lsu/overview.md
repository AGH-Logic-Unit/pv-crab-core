---
title: Overview
---

# Load Store Unit (LSU) Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Changelog

| Date | Version | Description |
| :--- | :-----: | :--- |
| 2026-08-08 | v0.1 | Initial microarchitectural specification defining speculative store lifecycle, interfaces, and Store-to-Load Forwarding with partial-overlap stall. |

## 2. Overview

The **Load-Store Unit (LSU)** is the execution engine for all memory operations in Stage 6 (Execute) of the Crab Core. It coordinates address generation, physical security verification (PMP & PMA), Store Buffer queuing, and port arbitration to the L1 Data Cache / MMU subsystem.

## 3. Architectural Requirements

The module is required to implement the following RISC-V Unprivileged ISA specifications:

* **RV64I Load/Store Instructions:**
    * Loads: `LB`, `LBU` (byte), `LH`, `LHU` (halfword), `LW`, `LWU` (word), `LD` (doubleword).
    * Stores: `SB` (byte), `SH` (halfword), `SW` (word), `SD` (doubleword).
* **RV64A Atomic Instructions (XLEN=64):**
    * Load-Reserved: `LR.W`, `LR.D`.
    * Store-Conditional: `SC.W`, `SC.D`.
    * Atomic Memory Operations (AMO): `AMOSWAP`, `AMOADD`, `AMOAND`, `AMOOR`, `AMOXOR`, `AMOMIN`, `AMOMAX`, `AMOMINU`, `AMOMAXU` in word (`.W`) and doubleword (`.D`) variants.
* **RV64FD Floating-Point Load/Store Instructions:**
    * Floating Loads: `FLW` (single-precision), `FLD` (double-precision).
    * Floating Stores: `FSW` (single-precision), `FSD` (double-precision).
* **Alignment:**
    * The module requires natural alignment for all memory accesses. If an address is not aligned to the size of the access, a misaligned address exception is raised.

## 4. Parameters

| Parameter Name | Type | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `STB_DEPTH` | `int` | `4` | Depth of the Store Buffer. Must be a power of 2 ($\le 4$). |
| `TAG_WIDTH` | `int` | `4` | ROB instruction tracking tag width. |

## 5. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `clk_i` | `logic` | 1 | IN | Clock signal |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset signal |
| `flush_i` | `logic` | 1 | IN | Pipeline flush signal |

### 5.1 Dispatch (Stage 5)

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `disp_valid_i` | `logic` | 1 | IN | Dispatcher handshake indicating valid memory/AMO request |
| `disp_ready_o` | `logic` | 1 | OUT | Handshake indicating unit is ready to accept a new instruction |
| `disp_headers_i`| `exe_headers_t`| - | IN | Input execution stage header metadata from Dispatcher |
| `operand_a_i` | `logic` | 64 | IN | Base address register (typically `RS1`) |
| `operand_b_i` | `logic` | 64 | IN | Store data or AMO operand (typically `RS2` or `FRS2`) |
| `imm_i` | `logic` | 64 | IN | Immediate offset value for address generation |
| `operator_i` | `logic` | 6 | IN | Operation code |

### 5.2 Writeback (Stage 7) & Commit Interface

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `wb_valid_o` | `logic` | 1 | OUT | Writeback handshake indicating valid result/exception is ready |
| `wb_ready_i` | `logic` | 1 | IN | Handshake indicating Writeback Arbiter can accept result |
| `wb_result_o` | `logic` | 64 | OUT | Operation result (read data for loads/AMOs, SC status) OR faulting virtual address (`badaddr`) if `wb_trap_o` is active |
| `wb_trap_o` | `logic` | 1 | OUT | Exception indicator (asserted on address misaligned, PMP fault, or page fault) |
| `wb_cause_o` | `logic` | 6 | OUT | Exception cause code (valid when `wb_trap_o` is 1) |
| `wb_headers_o` | `exe_headers_t`| - | OUT | Output execution stage header metadata to Writeback Buffer |

#### 5.2.1 Stage 7 Commit Queue Interface (ROB Head Control)

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `commit_head_valid_i` | `logic` | 1 | IN | Asserted when instruction at ROB head is ready to retire |
| `commit_head_tag_i` | `logic` | `TAG_WIDTH` | IN | ROB tag of instruction currently at the commit head |
| `is_rob_head_i` | `logic` | 1 | IN | Asserted when LSU's active serialized instruction matches ROB head |

### 5.3 D-Cache

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `dmem_req_o` | `logic` | 1 | OUT | Memory request validation flag |
| `dmem_gnt_i` | `logic` | 1 | IN | Memory subsystem grant (address phase accepted) |
| `dmem_addr_o` | `logic` | 56 | OUT | Memory access physical address (`PA[55:0]`) |
| `dmem_we_o` | `logic` | 1 | OUT | Write enable (1 = Write, 0 = Read) |
| `dmem_wdata_o` | `logic` | 64 | OUT | Memory write data payload (for stores and AMO operands) |
| `dmem_be_o` | `logic` | 8 | OUT | Byte enable strobes for write masking |
| `dmem_size_o` | `logic` | 2 | OUT | Size of memory operation (`00` = Byte, `01` = Halfword, `10` = Word, `11` = Doubleword) |
| `dmem_amo_req_o` | `logic` | 1 | OUT | Active high flag indicating transaction is an Atomic Memory Operation |
| `dmem_amo_op_o` | `logic` | 5 | OUT | AMO operation code (`funct5`) |
| `dmem_rvalid_i` | `logic` | 1 | IN | Data validation flag (read data available or write acknowledged) |
| `dmem_rdata_i` | `logic` | 64 | IN | Memory read data input (contains original pre-AMO value for atomic operations) |
| `dmem_err_i` | `logic` | 1 | IN | Memory access error response (e.g., bus fault or PMP violation) |

### 5.4 D-TLB / PMP / PMA MMU Interface

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `tlbpm_valid_o` | `logic` | 1 | OUT | Request valid strobe sent to D-TLB |
| `tlbpm_addr_o` | `logic` | 64 | OUT | Virtual address (`VA[63:0]`) calculated by LSU |
| `tlbpm_req_r_o` | `logic` | 1 | OUT | Read permission check request |
| `tlbpm_req_w_o` | `logic` | 1 | OUT | Write permission check request |
| `tlbpm_err_r_i` | `logic` | 1 | IN | Asserted if Read permission is denied (`cause = 5` or `13`) |
| `tlbpm_err_w_i` | `logic` | 1 | IN | Asserted if Write permission is denied (`cause = 7` or `15`) |
| `tlbpm_cause_r_i` | `logic` | 6 | IN | Exact Read fault cause code (`5` = Load Access Fault, `13` = Load Page Fault) |
| `tlbpm_cause_w_i` | `logic` | 6 | IN | Exact Write fault cause code (`7` = Store/AMO Access Fault, `15` = Store/AMO Page Fault) |
| `tlbpm_rvalid_i` | `logic` | 1 | IN | Data response valid (`1` = TLB Hit / Ready, `0` = TLB Miss / Stalled) |
| `tlbpm_addr_i` | `logic` | 56 | IN | Translated physical address (`PA[55:0]`) from D-TLB |
| `tlbpm_cacheable_i` | `logic` | 1 | IN | Active high if physical address points to cacheable DRAM |
| `tlbpm_pma_amo_level_i`| `pma_amo_level_t` | 2 | IN | Physical memory attribute atomic capability level (`None`, `Swap`, `Logical`, `Arithmetic`) |

## 6. Functional Description

### 6.1 Structure & Address Flow

```
                  +-----------------------------------+
                  |      LSU (Load-Store Unit)        |
                  |                                   |
                  |  +-------------+  +------------+  |
  Pipeline ──────>|  |   LSU-AMO   |  |   Store    |  |<──────> D-TLB (tlbpm_*)
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

#### Address Flow:
$$\text{RS1} + \text{imm} \rightarrow \text{VA}[63:0] \xrightarrow{\text{tlbpm}} \text{D-TLB} \xrightarrow{\text{PA}[55:0] + \text{Flags}} \text{LSU} \xrightarrow{\text{dmem\_addr\_o}} \text{D-Cache}$$

In BARE mode (or M-mode without translation), the D-TLB performs a 1:1 lookup on cached BARE entries (`is_bare == 1`) to retrieve pre-evaluated PMP/PMA permissions in a single cycle without runtime PMP comparators.

### 6.2 Load Lifecycle

```mermaid
sequenceDiagram
    autonumber
    actor PIPE as Pipeline / Dispatch
    participant LSU as LSU FSM
    participant STB as Store Buffer (STB)
    participant DTLB as D-TLB
    participant CACHE as L1 D-Cache

    Note over PIPE,LSU: STAGE 5 (Dispatch)
        PIPE->>LSU: uOp Dispatched
    Note over PIPE,LSU: STAGE 6 (Execute)
        LSU->>DTLB: Lookup VA (tlbpm_addr_o, req_r = 1)
        DTLB-->>LSU: Return PA[55:0] + Flags (tlbpm_rvalid_i == 1)
        LSU->>LSU: 1. Alignment Check (VA[2:0])
        LSU->>LSU: 2. PMP & Page Permission Check

        alt Exception (Misaligned / Access Fault / Page Fault)
            LSU-->>PIPE: Trap Request (wb_trap_o = 1, wb_cause_o = cause, badaddr = VA)
        else Access Permitted
            LSU<<->>STB: Store-to-Load Forwarding Lookup
            alt STB Hit (full byte match)
                STB-->>LSU: Bypassed Data (Youngest Store)
            else STB Hit (partial overlap)
                STB-->>LSU: Bypass Stall (Hold Stage 6)
            else STB Miss (no overlap)
                LSU->>CACHE: Read Request (dmem_addr_o = PA[55:0])
                CACHE-->>LSU: Read Data
            end
            LSU-->>PIPE: Forward Result
        end
    Note over PIPE,LSU: STAGE 7 (Retire)
        PIPE->>PIPE: Commit Instruction
```

### 6.3 Store Lifecycle

```mermaid
sequenceDiagram
    autonumber
    actor PIPE as Pipeline / Dispatch
    participant LSU as LSU FSM
    participant DTLB as D-TLB
    participant STB as Store Buffer (STB)
    participant CACHE as L1 D-Cache

    Note over PIPE,LSU: STAGE 5 (Dispatch)
        PIPE->>LSU: uOp Dispatched (Store / SC)
    Note over PIPE,LSU: STAGE 6 (Execute)
        LSU->>DTLB: Lookup VA (tlbpm_addr_o, req_w = 1)
        DTLB-->>LSU: Return PA[55:0] + Flags (tlbpm_rvalid_i == 1)
        LSU->>LSU: 1. Alignment Check (VA[2:0])
        LSU->>LSU: 2. PMP & Page Permission Check

        alt Exception (Misaligned / Access Fault / Page Fault)
            LSU-->>PIPE: Trap Request (wb_trap_o = 1, wb_cause_o, badaddr)
            Note over LSU,STB: No STB Entry Allocated
        else SC Failed (reservation_valid == 0 || mismatch)
            LSU-->>PIPE: SC Result = 1 (Failure, no memory write)
        else Valid Store / SC Success
            LSU->>STB: Push Speculative Entry (PA, wdata, be, size, tag)
            LSU-->>PIPE: Execution Done (SC Result = 0 / Store ACK)
        end

    Note over PIPE,CACHE: STAGE 7 (Retire / Commit)
        alt Pipeline Flush (Mispredict / Trap)
            PIPE->>STB: flush_i == 1
            STB->>STB: Rollback write_ptr (Discard Speculative)
        else Commit
            PIPE->>STB: Commit Strobe (stb_commit_tag_i)
            STB->>STB: Mark Entry as Committed (speculative = 0)
            STB->>CACHE: Drain to D-Cache (dmem_we_o = 1, dmem_addr_o = PA)
            PIPE->>PIPE: Retire Instruction
        end
```

### 6.4 AMO Lifecycle

```mermaid
sequenceDiagram
    autonumber
    actor PIPE as Pipeline / Dispatch
    participant LSU as LSU FSM
    participant STB as Store Buffer (STB)
    participant CACHE as L1 D-Cache / MMU

    Note over PIPE,LSU: STAGE 5 (Dispatch)
        PIPE->>LSU: uOp Dispatched (AMO)
    Note over PIPE,LSU: STAGE 6 (Execute)
        LSU->>LSU: PMP (R+W) & PMA & Alignment Check
        LSU-->>PIPE: FSM Stall (Execute-at-Retire)

    Note over PIPE,LSU: STAGE 7 (Retire)
        PIPE->>LSU: Commit Signal (is_rob_head_i == 1)
        LSU-->>LSU: Await STB Empty (stb_empty_i == 1)
        STB->>LSU: STB Empty
        LSU->>CACHE: Send amo_req (Cache-Side RMW)
        CACHE-->>LSU: Return pre-AMO Data
        LSU-->>PIPE: Forward Data
```

## 7. Policies and Rulesets

### 7.1 Privilege Levels and Address Translation Mode

1. **M-mode (Machine Mode) Bare Addressing:**
    * When executing in M-mode (`curr_priv == PRIV_MACHINE`) or when `satp.MODE == BARE`, Sv39 page table translation is **disabled** (`PA == VA`).
    * Memory accesses still perform a 1-cycle lookup in the D-TLB against cached BARE entries (`is_bare == 1`) to retrieve pre-evaluated PMP (for locked `L=1` rules) and PMA region attributes without activating runtime PMP comparators.
    * If a BARE miss occurs, the MMU installs a new 1:1 BARE TLB entry via the combined PMP/PMA evaluation block.

2. **S-mode & U-mode Sv39 Translation:**
    * When `satp.MODE == Sv39` and executing in S-mode or U-mode, virtual addresses (`VA`) undergo TLB lookup and Page Table Walk translation to Physical Addresses (`PA`).
    * Cacheability and memory types are determined via `Svpbmts` Page-Based Memory Types (`PTE[62:61]`) or PMA region attributes.

### 7.2 Memory Security and Alignment Rules

1. Verification of alignment rules:
    * **Doubleword (64b):** `addr[2:0] = 3'b000`
    * **Word (32b):** `addr[1:0] = 2'b00`
    * **Halfword (16b):** `addr[0] = 1'b0`
    * Wrong alignment causes exceptions:
        - *Load Address Misaligned:* `cause = 4`
        - *Store/AMO Address Misaligned:* `cause = 6`

2. Physical Memory Protection (PMP) checks:
    * **Load / LR:** Require Read (`R`) permission. On violation, raise Load Access Fault (`cause = 5`).
    * **Store / SC:** Require Write (`W`) permission. On violation, raise Store/AMO Access Fault (`cause = 7`).
    * **AMOs:** Because AMOs perform a read-modify-write, they **must require both Read (R) and Write (W) permissions**. If either permission is missing, raise Store/AMO Access Fault (`cause = 7`).

!!! Warning
    PMP checks are evaluated for `SC` regardless of whether the reservation is valid or not.

3. Physical Memory Attributes (PMA) Abstraction:
    * **AMOs:** Require AMO support (`pma_supports_amo`). If not supported, raise Store/AMO Access Fault (`cause = 7`).

### 7.3 Execution Policies

1. **Load and LR** Speculative Execution:
    * Address calculation -> PMP -> send read to Cache/STB
    * `LR` saves cache-line address in `reservation_address` register and sets `reservation_valid = 1`

2. **Store and SC** Speculative Execution:
    * Address calculation -> PMP -> send write to STB
    * For `SC` `reservation_valid == 1` and valid address is required -> send to STB; result is `0` (success). If `reservation_valid == 0` there is no save action in STB nor Cache, result is `1` (failure).

3. **AMO and MMIO** Execute-at-Retire:
    * Address calculation -> PMP, PMA checks
    * Execution is stalled and instruction awaits in `ROB`
    * Await STB drain (`stb_empty_i == 1`)
    * Send packet `dmem_amo_req_o` to Cache/MMU (Cache-Side RMW)

### 7.4 Reservation Invalidation

Reservation is established when an `LR` instruction is executed. The reservation status and physical cache-line address are stored locally: `reservation_valid <= 1` and `reservation_address <= pa[55:6]`.

The reservation is invalidated strictly under the following conditions:

1. On `Store` or `SC` instruction commit in Stage 7 (Commit / Retire)
2. On `FENCE.I` or `SFENCE.VMA` commit
3. On `satp` modification (address space switch)
4. On trap / interrupt entry
5. On external signal from coherence bus (`dmem_snoop_invalidate_valid_i` with `dmem_snoop_invalidate_addr_i[55:6] == reservation_address`) [*multi-hart only]

### 7.5 Port Arbiter and STB Drain Policy

Port Arbiter priority:

1. **(High)**: Speculative loads and AMO while committing
2. **(Low)**: STB drain of validated store operations

STB must be protected from starvation. If occupancy reaches $75\%$, STB drain priority raises to `High` to prevent deadlock.

## 8. Verification

The LSU is verified through formal property verification (SVA), directed testing, and constrained random simulation against the following assertions:

### Formal SVA Assertion Table

| Assertion ID | Property / Condition | Severity | Checkpoint | Description |
| :--- | :--- | :---: | :---: | :--- |
| `SVA_LSU_01` | `(dmem_amo_req_o == 1) |-> (is_rob_head_i && !flush_i)` | `FATAL` | **CHK-LSU-04** | AMO requests must be issued ONLY when at ROB commit head and non-flushed |
| `SVA_LSU_02` | `(ex_store_valid && flush_i) |=> (reservation_valid == $past(reservation_valid))` | `ERROR` | **CHK-LSU-01** | Speculative store flushed in Stage 6 must NOT destroy LR reservation |
| `SVA_LSU_03` | `(disp_valid_i && is_sc_i && pmp_err) |-> ##1 (wb_trap_o && (wb_cause_o == 7))` | `FATAL` | **CHK-LSU-02** | SC PMP violation triggers Access Fault even if SC reservation is invalid |
| `SVA_LSU_04` | `(disp_valid_i && misaligned) |-> ##1 (wb_trap_o && (wb_cause_o == 4 || 6))` | `ERROR` | **CHK-LSU-08** | Address misalignment on VA[2:0] immediately raises Alignment Exception |
| `SVA_LSU_05` | `$bits(dmem_addr_o) == 56` | `FATAL` | **CHK-MMU-02** | Physical memory request address port width must strictly equal 56 bits |
| `SVA_LSU_06` | `(load_bypass_hit_o && (valid_match_count > 1)) |-> (forwarded_tag == youngest_stb_tag)` | `ERROR` | **CHK-LSU-06** | Store-to-load forwarding must always select the youngest matching entry |

### Verification Methodology
1. **Constrained Random Tests:** Generates random mixtures of loads, stores, LR/SC, and AMOs across 4 KiB page boundaries and PMP sub-regions.
2. **Directed Corner Cases:** Partial store overlaps (1B/2B/4B/8B permutations), misaligned traps, multiple matching STB entries, and pipeline flushes during wait states.
3. **Reference Comparison:** Output data and exceptions are matched cycle-by-cycle against a Golden C++ Architectural Model.
