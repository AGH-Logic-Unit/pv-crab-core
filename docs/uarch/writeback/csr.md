---
title: Control and Status Registers (CSR)
---

# Control and Status Registers (CSR) Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`
    * :material-cpu: **Architecture:** RV64GC S/U

## 1. Changelog

| Date | Version | Description |
| :-- | :--: | :-- |
| 2026-08-05 | v0.2 | Added parameterization section, detailed PMP entry registers/configs, and aligned trap vector calculations |
| 2026-07-31 | v0.1 | Initial uarch draft specifications |

---

## 2. Overview

The Control and Status Registers (CSR) module is responsible for managing the processor's internal state, configuration of privilege levels, trap handling, virtual memory configurations (`satp` register), physical memory protection (PMP), performance counters, and floating-point control/status (`fcsr`).

To maximize core performance and simplify the Dispatch unit, the PV-Crab core uses an optimized **Execute-at-Retire with No-Stall Dispatch** strategy. CSR instructions flow through the pipeline without stalling the Dispatch stage. Both the read of the old value and the write of the new value are deferred to **Stage 7 (Retire & Writeback)**.

---

## 3. Architectural Requirements

Under the **RISC-V RV64GC** specifications, the system must support:

1. **CSR Instructions:**
    * `CSRRW` / `CSRRWI` (Atomic Read/Write CSR)
    * `CSRRS` / `CSRRSI` (Atomic Read and Set Bits)
    * `CSRRC` / `CSRRCI` (Atomic Read and Clear Bits)
2. **Three Privilege Levels:**
    * **Machine Mode (M-mode):** Highest privilege (required).
    * **Supervisor Mode (S-mode):** OS level privilege (required for virtual memory Sv39 and Linux support).
    * **User Mode (U-mode):** Application level privilege.
3. **Required CSR Register Set:**
    * **M-mode:** `mstatus`, `misa`, `medeleg`, `mideleg`, `mie`, `mtvec`, `mscratch`, `mepc`, `mcause`, `mtval`, `mip`, `mcycle`, `minstret`, `mhartid`, `mvendorid`, `marchid`, `mimpid`.
    * **S-mode:** `sstatus`, `sie`, `stvec`, `sscratch`, `sepc`, `scause`, `stval`, `sip`, `satp`.
    * **U-mode (FP extension):** `fcsr` (consisting of `frm` and `fflags`).
    * **Counters:** `cycle`, `time`, `instret`.
4. **Trap Handling:** Atomic privilege escalation/de-escalation, vector addressing (Vectored/Direct modes), exception delegation to S-mode via `medeleg`/`mideleg`.

---

## 4. Parameterization

To support multi-core scaling and allow hardware customization, the machine information CSRs (`mvendorid`, `marchid`, `mimpid`, `mhartid`) are parameterized at instantiation. These parameters are hardwired as read-only values in M-mode:

| Parameter | Type | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `HART_ID` | `logic [63:0]` | `64'h0` | Hardware Thread ID (`mhartid` value) |
| `VENDOR_ID` | `logic [63:0]` | `64'h0` | JEDEC Manufacturer ID (`mvendorid` value) |
| `ARCH_ID` | `logic [63:0]` | `64'h0` | Open-source Architecture ID (`marchid` value) |
| `IMP_ID` | `logic [63:0]` | `64'h01` | Implementation version code (`mimpid` value) |

---

## 5. Interfaces

### 5.1 Writeback <-> CSR Unit Port List

The ports defined below reflect the optimized interface where CSR metadata and operands are retrieved from the Retirement Buffer slots at the Commit point (Stage 7).

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| **Global Signals** | | | | |
| `clk_i` | `logic` | 1 | IN | System clock |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset |
| **Global State Outputs** | | | | |
| `priv_mode_o` | `priv_mode_t` | 2 | OUT | Active CPU privilege level (sent to Decode for security checks) |
| **Stage 7 Commit Interface** | | | | |
| `commit_valid_i` | `logic` | 1 | IN | High when the instruction at the head of the Retirement FIFO is committing |
| `commit_csr_valid_i` | `logic` | 1 | IN | Asserted if the committing instruction is a CSR instruction (`op_class == OP_CLASS_CSR`) |
| `commit_csr_addr_i` | `logic` | 12 | IN | Target CSR address (from `meta.csr.addr`) |
| `commit_csr_op_i` | `csr_op_t` | 2 | IN | CSR operation type (from `meta.csr.op`) |
| `commit_wdata_i` | `logic` | 64 | IN | CSR write data operand (from `result` field of the slot) |
| `commit_rdata_o` | `logic` | 64 | OUT | Old CSR value to be written back to GPR file (`rd`) |
| `commit_illegal_o` | `logic` | 1 | OUT | Asserted if privilege/RO checks on the CSR fail at retirement |
| `commit_flush_req_o` | `logic` | 1 | OUT | Request pipeline flush (asserted for system-critical writes e.g. `satp`, PMP) |
| **FPU Commit Interface** | | | | |
| `commit_fpu_valid_i` | `logic` | 1 | IN | Asserted when committing an FPU instruction (`op_class == OP_CLASS_FPU`) |
| `commit_fflags_i` | `logic` | 5 | IN | Accrued exception flags to be merged into `fflags` (from `meta.fp.fflags`) |
| **Trap Interface (at Commit)** | | | | |
| `trap_valid_i` | `logic` | 1 | IN | Asserted when the committing instruction has a trap flag (`trap == 1`) |
| `trap_cause_i` | `logic` | 6 | IN | Trap exception cause code (from `meta.trap.cause`) |
| `trap_is_interrupt_i` | `logic` | 1 | IN | Asserted if the trap is an asynchronous interrupt |
| `trap_badaddr_i` | `logic` | 64 | IN | Memory address or instruction value causing the exception (from `result`) |
| `trap_pc_i` | `logic` | 64 | IN | PC of the faulting instruction (from `pc`) |
| `trap_vector_o` | `logic` | 64 | OUT | Target PC address of the trap handler (calculated via `mtvec`/`stvec`) |
| **System Status Outputs** | | | | |
| `satp_o` | `satp_reg_t` | 64 | OUT | Address translation register (sent to MMU / TLBs) |
| `mstatus_o` | `mstatus_reg_t` | 64 | OUT | Machine status register (sent to MMU and Interrupt Controller) |
| `mie_o` | `logic` | 64 | OUT | Machine interrupt enables (sent to Interrupt Controller) |
| `mip_o` | `logic` | 64 | OUT | Machine interrupt pending flags (sent to Interrupt Controller) |
| `frm_o` | `logic` | 3 | OUT | Floating-point dynamic rounding mode (sent directly to FPU in Stage 6) |
| **70-bit PMP Broadcast Bus** | | | | |
| `pmp_write_valid_o` | `logic` | 1 | OUT | Broadcast write pulse triggered when a PMP CSR is written |
| `pmp_write_idx_o` | `logic` | 5 | OUT | Target PMP register index (`0..3` for `pmpcfg`, `0..15` for `pmpaddr`) |
| `pmp_write_data_o` | `logic` | 64 | OUT | 64-bit write payload data broadcast to Stage 6 MMU |
| **External Interrupt & Timer Inputs** | | | | |
| `timer_irq_i` | `logic` | 1 | IN | Timer interrupt pending input from CLINT (`mip.MTIP`) |
| `sw_irq_i` | `logic` | 1 | IN | Software interrupt pending input from CLINT (`mip.MSIP`) |
| `ext_irq_i` | `logic` | 1 | IN | External interrupt pending input from PLIC (`mip.MEIP`) |
| `mtime_i` | `logic` | 64 | IN | Continuous 64-bit real-time counter from CLINT (shadowed by `time` CSR) |
| **Hardware Counter Interface** | | | | |
| `inst_retired_i` | `logic` | 1 | IN | Pulse indicating an instruction has successfully retired (for `minstret`) |

## 6. Functional Description

### 6.1 Local ALU

The CSR ALU performs atomic read-modify-write bit operations on the active register state. In the equations below, `csr_val` is the old value read from the register file, and `op_val` is the operand provided by `commit_wdata_i` (representing the GPR value `rs1` or zero-extended immediate `zimm`):

* **`CSRRW(I)` (Read/Write):**
    * `new_val = op_val`
    * `rd = csr_val`
* **`CSRRS(I)` (Bitwise Set):**
    * `new_val = csr_val | op_val`
    * `rd = csr_val`
* **`CSRRC(I)` (Bitwise Clear):**
    * `new_val = csr_val & ~op_val`
    * `rd = csr_val`

#### Special Case `rs1 == x0` / `zimm == 0`
If the source register index `rs1` is `0` (or `zimm` immediate is `0`) for `CSRRS` and `CSRRC` instructions, no write side-effects are triggered. The register file is not updated, allowing read-only access to read-only CSR registers (such as performance counters) without triggering privilege violations on writes.

### 6.2 Registers and Addressing

The CSR unit implements a 12-bit address space. The table below lists the essential registers and their specifications:

| CSR Address | Name | Privilege | Access Type | Description |
| :--- | :--- | :---: | :---: | :--- |
| **M-Mode (Info)** | | | | |
| `0xF11` | `mvendorid` | M | RO | Machine Vendor ID |
| `0xF12` | `marchid` | M | RO | Machine Architecture ID |
| `0xF13` | `mimpid` | M | RO | Machine Implementation ID |
| `0xF14` | `mhartid` | M | RO | Hardware Thread ID |
| **M-Mode (Control)** | | | | |
| `0x300` | `mstatus` | M | RW | Machine Status |
| `0x301` | `misa` | M | RW | Machine ISA Extensions |
| `0x302` | `medeleg` | M | RW | Machine Exception Delegation |
| `0x303` | `mideleg` | M | RW | Machine Interrupt Delegation |
| `0x304` | `mie` | M | RW | Machine Interrupt Enable |
| `0x305` | `mtvec` | M | RW | Machine Trap Vector |
| `0x306` | `mcounteren` | M | RW | Machine Counter Enable |
| `0x320` | `mcountinhibit` | M | RW | Machine Counter-Inhibit |
| `0x340` | `mscratch` | M | RW | Machine Scratch |
| `0x341` | `mepc` | M | RW | Machine Exception PC |
| `0x342` | `mcause` | M | RW | Machine Exception Cause |
| `0x343` | `mtval` | M | RW | Machine Trap Value |
| `0x344` | `mip` | M | RW | Machine Interrupt Pending |
| **PMP Registers** | | | | |
| `0x3A0` | `pmpcfg0` | M | RW | PMP Configuration entries 0–7 |
| `0x3A2` | `pmpcfg2` | M | RW | PMP Configuration entries 8–15 |
| `0x3B0`–`0x3BF` | `pmpaddr0`–`pmpaddr15` | M | RW | PMP Physical Address bound registers (16 entries) |
| **M-Mode Counters** | | | | |
| `0xB00` | `mcycle` | M | RW | Machine Cycle Counter |
| `0xB02` | `minstret` | M | RW | Machine Instructions-Retired Counter |
| **S-Mode** | | | | |
| `0x100` | `sstatus` | S | RW | Supervisor Status (shadow of `mstatus`) |
| `0x104` | `sie` | S | RW | Supervisor Interrupt Enable |
| `0x105` | `stvec` | S | RW | Supervisor Trap Vector |
| `0x106` | `scounteren` | S | RW | Supervisor Counter Enable |
| `0x140` | `sscratch` | S | RW | Supervisor Scratch |
| `0x141` | `sepc` | S | RW | Supervisor Exception PC |
| `0x142` | `scause` | S | RW | Supervisor Exception Cause |
| `0x143` | `stval` | S | RW | Supervisor Trap Value |
| `0x144` | `sip` | S | RW | Supervisor Interrupt Pending |
| `0x180` | `satp` | S | RW | Supervisor Address Translation & Protection (Sv39) |
| **U-Mode (FP)**| | | | |
| `0x001` | `fflags` | U | RW | Floating-Point Accrued Exceptions |
| `0x002` | `frm` | U | RW | Floating-Point Rounding Mode |
| `0x003` | `fcsr` | U | RW | Floating-Point Control & Status (`frm` + `fflags`) |
| **Counters (Shadow / User View)** | | | | |
| `0xC00` | `cycle` | U | RO | Cycle Counter (shadow of `mcycle`) |
| `0xC01` | `time` | U | RO | Wall-clock Timer (shadow of CLINT `mtime`) |
| `0xC02` | `instret` | U | RO | Instructions Retired Counter (shadow of `minstret`) |

Any access to an unimplemented or unsupported CSR address triggers an **Illegal Instruction Exception** (`commit_illegal_o` = 1).

#### ISA Register `misa` (`0x301`) WARL Behavior
The `misa` register reports the standard extensions supported by the PV-Crab Core. In accordance with the RISC-V specification for hardwired implementations, `misa` is **Read-Only (RO WARL)**; writes are ignored without generating exceptions.

$$
\text{misa} = \text{64'h8000_0000_0014_112D} \quad (\text{RV64IMAFDCSU})
$$

* `MXL[63:62] = 2'b10`: 64-bit native architecture ($XLEN=64$).
* `Extensions[25:0]`: Encodes supported extensions:
  * `Bit 0` (`A`): Atomic Extension
  * `Bit 2` (`C`): Compressed 16-bit Instructions
  * `Bit 3` (`D`): Double-Precision Floating-Point
  * `Bit 5` (`F`): Single-Precision Floating-Point
  * `Bit 8` (`I`): Base Integer ISA
  * `Bit 12` (`M`): Integer Multiplication & Division
  * `Bit 18` (`S`): Supervisor Mode
  * `Bit 20` (`U`): User Mode

#### Machine Status Register `mstatus` (`0x300`) Bit Field Layout

| Bits | Field Name | Access | Reset Value | Description / Architectural Behavior |
| :---: | :--- | :---: | :---: | :--- |
| `63` | `SD` | RO | `1'b0` | State Dirty: Set dynamically if $(FS == 2\text{'b}11) \mid (XS == 2\text{'b}11)$ |
| `37:36` | `SBE`/`MBE` | RO | `2'b00` | Endianness: Hardwired Little-Endian (`00`) |
| `22` | `TSR` | RW | `1'b0` | Trap SRET: When `1`, executing `SRET` in S-mode raises Illegal Instruction (`cause = 2`) |
| `21` | `TW` | RW | `1'b0` | Timeout Wait: When `1`, executing `WFI` in S/U-mode raises Illegal Instruction after timeout |
| `20` | `TVM` | RW | `1'b0` | Trap Virtual Memory: When `1`, access to `satp` or `SFENCE.VMA` in S-mode raises Illegal Instruction |
| `19` | `MXR` | RW | `1'b0` | Make Executable Readable: When `1`, loads succeed on executable-only pages ($X=1$) |
| `18` | `SUM` | RW | `1'b0` | Permit Supervisor User Memory Access: When `1`, S-mode accesses to User pages ($U=1$) are permitted |
| `17` | `MPRV` | RW | `1'b0` | Modify Privilege: When `1`, memory data accesses evaluate translation & protection using `MPP` |
| `16:15` | `XS` | RO | `2'b00` | User Extension Status: Hardwired `00` (no custom accelerator state) |
| `14:13` | `FS` | RW | `2'b00` | Floating-Point Status: `00`=Off, `01`=Initial, `10`=Clean, `11`=Dirty |
| `12:11` | `MPP` | RW | `2'b11` | Previous Privilege Mode for M-mode (Reset = Machine Mode `11`) |
| `8` | `SPP` | RW | `1'b0` | Previous Privilege Mode for S-mode (`0`=User, `1`=Supervisor) |
| `7` | `MPIE` | RW | `1'b0` | Machine Previous Interrupt Enable |
| `5` | `SPIE` | RW | `1'b0` | Supervisor Previous Interrupt Enable |
| `3` | `MIE` | RW | `1'b0` | Machine Global Interrupt Enable |
| `1` | `SIE` | RW | `1'b0` | Supervisor Global Interrupt Enable |

### 6.3 Privilege Modes and Control

Security checks are performed in **Stage 4 (Decode)** based on `priv_mode_o` output by the CSR unit:

1. **Privilege Violations:** The current privilege level of the core (`curr_priv`) must be greater than or equal to the privilege level required by the CSR (encoded in bits `[9:8]` of the CSR address).
2. **Write Violations:** An instruction must not attempt a write to a read-only CSR (bits `[11:10] == 2'b11`).

If a violation is detected during Decode, the instruction is tagged with a trap flag (`trap = 1`, `meta.trap.cause = 2` - Illegal Instruction) and flows down the pipeline as a bubble. If the violation occurs at retirement, the CSR unit asserts `commit_illegal_o` which triggers the trap logic.

#### FPU State Management (`mstatus.FS`)
The floating-point state is monitored using the `FS` field in `mstatus` (bits `[14:13]`):

* `2'b00` (Off): Any attempt to execute an FPU instruction or access `fcsr`/`frm`/`fflags` triggers an **Illegal Instruction Exception**.
* `2'b01` (Initial) / `2'b10` (Clean): Permission granted.
* `2'b11` (Dirty): Must be set automatically by the CSR unit whenever FPU registers or FPU CSRs are updated.

#### Physical Memory Protection (PMP) Broadcast & TLB Invalidation
The core implements a RISC-V Physical Memory Protection (PMP) unit supporting 16 entries to enforce access permissions on physical addresses:

* **Architectural Registers:**
    * `pmpcfg0` and `pmpcfg2` (in RV64, `pmpcfg0` configures entries 0–7, and `pmpcfg2` configures entries 8–15).
    * `pmpaddr0` to `pmpaddr15` (16 address registers holding physical address bits `[55:2]`).
* **70-bit Broadcast Bus:**
    * To avoid routing 992 parallel output lines across the chip floorplan, PMP register updates at commit-time are broadcast via `pmp_write_valid_o`, `pmp_write_idx_o`, and `pmp_write_data_o` to the local Shadow PMP Registers in Stage 6 MMU.
* **PMP Writes and Security Pipeline Flushes:**
    * Any write to a `pmpcfg*` or `pmpaddr*` register asserts `pmp_write_valid_o = 1` and `commit_flush_req_o = 1`.
    * **TLB Invalidation Action:** The Stage 6 MMU receives `pmp_write_valid_o` and unloads/invalidates all cached **BARE mode TLB entries (`is_bare == 1`)** across both I-TLB and D-TLB (and flushes Sv39 entries with `pmp_fault == 1`), ensuring stale security rules are eliminated.
    * The Writeback stage clears in-flight pipeline instructions and redirects Fetch to `PC + 4` under the newly programmed permissions.


### 6.4 Trap Handling

When the Writeback stage detects `trap == 1` at the head of the Retirement Buffer, it asserts `trap_valid_i = 1`. The CSR module resolves the trap as follows:

1. **PC Preservation:** The faulting instruction's PC (`trap_pc_i`) is written to `mepc` (or `sepc` if delegated). The LSB is masked to `0` during write (`mepc <= {trap_pc_i[63:1], 1'b0}`) as a defense-in-depth measure.
2. **Trap Context:** The exception code (`trap_cause_i`) is written to `mcause` (`scause`), and any bad address (`trap_badaddr_i`) is written to `mtval` (`stval`).
3. **Privilege Delegation:** The CSR unit checks exception delegation registers (`medeleg` for exceptions, `mideleg` for interrupts). If S-mode is active and the bit corresponding to the trap is set in the delegation register, the trap is routed to S-mode:
    * Privilege level transitions to Supervisor (`curr_priv = PRIV_SUPERVISOR`).
    * `sstatus.SPP` is set to `curr_priv`.
    * `sstatus.SPIE` is set to `sstatus.SIE`, and `sstatus.SIE` is cleared to `0`.
    * Target PC is read from `stvec`.
    Otherwise, the trap goes to Machine Mode:
    * Privilege level transitions to Machine (`curr_priv = PRIV_MACHINE`).
    * `mstatus.MPP` is set to `curr_priv`.
    * `mstatus.MPIE` is set to `mstatus.MIE`, and `mstatus.MIE` is cleared to `0`.
    * Target PC is read from `mtvec`.
4. **Zero-Adder Vector Address Calculation:**
    * Under the RISC-V Privileged Specification, when Vectored Mode is enabled, the base address of the trap vector (`BASE` in `mtvec`/`stvec`) **must be aligned to a 256-byte boundary**. Therefore, the lower 8 bits of `BASE` are always `8'b00000000`.
    * Because the interrupt cause code is at most 6 bits, the offset `4 * cause` fits entirely within 8 bits (maximum offset of `252` or `8'b11111100`).
    * Rather than using a full 64-bit adder, the CSR unit calculates the target trap vector address using **bitwise concatenation** (zero hardware adders):
      ```systemverilog
      logic [63:0] base_address;
      assign base_address = {tvec_val[63:2], 2'b00};
      assign trap_vector_o = (tvec_val[1:0] == 2'b01 && trap_is_interrupt_i) ?
                             {tvec_val[63:8], trap_cause_i[5:0], 2'b00} :
                             base_address;
      ```
    * The calculated address is output on `trap_vector_o` to redirect the Fetch stage.

### 6.5 Instruction Length & Redirection PC Calculation

Upon a pipeline redirect (e.g. system-critical write flush or exception/branch flush), the target PC is calculated locally using the `is_rvc` instruction length flag:

```systemverilog
logic [2:0]  instr_len;
logic [63:0] next_pc;

assign instr_len = is_rvc ? 3'd2 : 3'd4;
assign next_pc   = stage_pc + instr_len;
```

## 7. Timing and Performance

### 7.1 Critical Paths
Due to the Execute-at-Retire design, the CSR module operates entirely within a single cycle. The critical path involves:

1. Reading the target CSR register value.
2. Routing this value to `commit_rdata_o` for writeback to the GPR file.
3. Performing the ALU operation and updating the register file.

To maintain a target minimum frequency of **100MHz**, the logic is structured for single-cycle retirement with positive slack on the 130nm process.

## 8. Verification Plan

The CSR unit must undergo rigorous block-level and system-level verification:

1. **Access Control Tests:** Attempt to write to read-only CSRs, and access M-mode registers from S-mode/U-mode to verify illegal instruction traps.
2. **FPU State Checks:** Verify that FPU instructions trap when `mstatus.FS = 0`, and that the dirty bit is set correctly (`mstatus.FS = 2'b11`) upon register updates.
3. **Trap Routing Tests:** Verify correct exception delegation to `sepc`/`scause` using different configurations of `medeleg`/`mideleg`.
4. **Counter Accuracy:** Verify that the `cycle` and `instret` registers increment accurately under heavy pipeline stall scenarios and are inhibited by `mcountinhibit`.
