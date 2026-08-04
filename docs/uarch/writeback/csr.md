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

To maximize core performance and simplify the Dispatch unit, the PV-Crab core uses an optimized **Execute-at-Retire with No-Stall Dispatch** strategy. CSR instructions flow through the pipeline without stalling the Dispatch stage. Both the read of the old value and the write of the new value are deferred to **Stage 9 (Retire & Writeback)**.

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

### 3.1 Writeback <-> CSR Unit Port List

The ports defined below reflect the optimized interface where CSR metadata and operands are retrieved from the Retirement Buffer slots at the Commit point (Stage 9).

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| **Global Signals** | | | | |
| `clk_i` | `logic` | 1 | IN | System clock |
| `rst_ni` | `logic` | 1 | IN | Asynchronous active-low reset |
| **Global State Outputs** | | | | |
| `priv_mode_o` | `priv_mode_t` | 2 | OUT | Active CPU privilege level (sent to Decode for security checks) |
| **Stage 9 Commit Interface** | | | | |
| `commit_valid_i` | `logic` | 1 | IN | High when the instruction at the head of the Retirement FIFO is committing |
| `commit_csr_valid_i` | `logic` | 1 | IN | Asserted if the committing instruction is a CSR instruction (`op_class == OP_CLASS_CSR`) |
| `commit_csr_addr_i` | `logic` | 12 | IN | Target CSR address (from `meta.csr.addr`) |
| `commit_csr_op_i` | `csr_op_t` | 2 | IN | CSR operation type (from `meta.csr.op`) |
| `commit_wdata_i` | `logic` | 64 | IN | CSR write data operand (from `result` field of the slot) |
| `commit_rdata_o` | `logic` | 64 | OUT | Old CSR value to be written back to GPR file (`rd`) |
| `commit_illegal_o` | `logic` | 1 | OUT | Asserted if privilege/RO checks on the CSR fail at retirement |
| `commit_flush_req_o` | `logic` | 1 | OUT | Request pipeline flush (asserted for system-critical writes e.g. `satp`) |
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
| `satp_o` | `logic` | 64 | OUT | Address translation register (sent to TLB/MMU) |
| `mstatus_o` | `logic` | 64 | OUT | Machine status register (sent to Interrupt Controller/Control logic) |
| `mie_o` | `logic` | 64 | OUT | Machine interrupt enables (sent to Interrupt Controller) |
| `mip_o` | `logic` | 64 | OUT | Machine interrupt pending flags (sent to Interrupt Controller) |
| `frm_o` | `logic` | 3 | OUT | Floating-point dynamic rounding mode (sent directly to FPU in Stage 8) |
| `pmpcfg_o` | `logic [15][7:0]` | 128 | OUT | Exported PMP entry configurations (16 entries * 8 bits, sent to I/D Memory Access Filters) |
| `pmpaddr_o` | `logic [15][53:0]` | 864 | OUT | Exported PMP physical address bounds (16 entries * 54 bits, holding address bits [55:2], sent to Filters) |
| **Hardware Counter Interface** | | | | |
| `inst_retired_i` | `logic` | 1 | IN | Pulse indicating an instruction has successfully retired (for `minstret`) |

---

## 6. Functional Description

### 6.1 Local ALU

The CSR ALU performs atomic read-modify-write bit operations on the active register state. In the equations below, `csr_val` is the old value read from the register file, and `op_val` is the operand provided by `commit_wdata_i` (representing the GPR value `rs1` or zero-extended immediate `zimm`):

* **`CSRRW(I)` (Read/Write):**
  $$\text{new\_val} = \text{op\_val}$$
  $$\text{rd} = \text{csr\_val}$$
* **`CSRRS(I)` (Bitwise Set):**
  $$\text{new\_val} = \text{csr\_val} \mid \text{op\_val}$$
  $$\text{rd} = \text{csr\_val}$$
* **`CSRRC(I)` (Bitwise Clear):**
  $$\text{new\_val} = \text{csr\_val} \ \& \ \sim\text{op\_val}$$
  $$\text{rd} = \text{csr\_val}$$

#### Special Case `rs1 == x0` / `zimm == 0`
If the source register index `rs1` is `0` (or `zimm` immediate is `0`) for `CSRRS` and `CSRRC` instructions, no write side-effects are triggered. The register file is not updated, allowing read-only access to read-only CSR registers (such as performance counters) without triggering privilege violations on writes.

---

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
| `0x340` | `mscratch` | M | RW | Machine Scratch |
| `0x341` | `mepc` | M | RW | Machine Exception PC |
| `0x342` | `mcause` | M | RW | Machine Exception Cause |
| `0x343` | `mtval` | M | RW | Machine Trap Value |
| `0x344` | `mip` | M | RW | Machine Interrupt Pending |
| **S-Mode** | | | | |
| `0x100` | `sstatus` | S | RW | Supervisor Status (shadow of `mstatus`) |
| `0x104` | `sie` | S | RW | Supervisor Interrupt Enable |
| `0x105` | `stvec` | S | RW | Supervisor Trap Vector |
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
| **Counters** | | | | |
| `0xC00` | `cycle` | U | RO | Cycle Counter |
| `0xC01` | `time` | U | RO | Wall-clock Timer |
| `0xC02` | `instret` | U | RO | Instructions Retired Counter |

Any access to an unimplemented or unsupported CSR address triggers an **Illegal Instruction Exception** (`commit_illegal_o` = 1).

---

## 6.3 Privilege Modes and Control

Security checks are performed in **Stage 6 (Decode)** based on `priv_mode_o` output by the CSR unit:
1. **Privilege Violations:** The current privilege level of the core (`curr_priv`) must be greater than or equal to the privilege level required by the CSR (encoded in bits `[9:8]` of the CSR address).
2. **Write Violations:** An instruction must not attempt a write to a read-only CSR (bits `[11:10] == 2'b11`).

If a violation is detected during Decode, the instruction is tagged with a trap flag (`trap = 1`, `meta.trap.cause = 2` - Illegal Instruction) and flows down the pipeline as a bubble. If the violation occurs at retirement, the CSR unit asserts `commit_illegal_o` which triggers the trap logic.

#### FPU State Management (`mstatus.FS`)
The floating-point state is monitored using the `FS` field in `mstatus` (bits `[14:13]`):
* `2'b00` (Off): Any attempt to execute an FPU instruction or access `fcsr`/`frm`/`fflags` triggers an **Illegal Instruction Exception**.
* `2'b01` (Initial) / `2'b10` (Clean): Permission granted.
* `2'b11` (Dirty): Must be set automatically by the CSR unit whenever FPU registers or FPU CSRs are updated.

#### Physical Memory Protection (PMP)
The core implements a RISC-V Physical Memory Protection (PMP) unit supporting 16 entries to enforce access permissions on physical addresses:
* **Architectural Registers:**
  * `pmpcfg0` and `pmpcfg2` (in RV64, `pmpcfg0` configures entries 0–7, and `pmpcfg2` configures entries 8–15).
  * `pmpaddr0` to `pmpaddr15` (16 address registers).
* **PMP Entry Configuration Format (8 bits per entry):**
  * `R` (bit 0): Read permission.
  * `W` (bit 1): Write permission.
  * `X` (bit 2): Execute (instruction fetch) permission.
  * `A` (bits 4:3): Address matching mode (00 = OFF, 01 = TOR (Top of Range), 10 = NA4 (Naturally Aligned 4-byte), 11 = NAPOT (Naturally Aligned Power-of-Two, $\ge$ 8 bytes)).
  * `L` (bit 7): Lock bit (enforces restrictions on M-mode and locks configuration until reset).
* **Hardware Signal Export:**
  * The CSR unit continuously exports all 16 configuration fields as a packed array on `pmpcfg_o` (`16 * 8 = 128` bits).
  * It exports all 16 address registers (holding physical address bits `[55:2]`) on `pmpaddr_o` (`16 * 54 = 864` bits).
  * These exported signals are routed directly to the Stage 1 Fetch security filter and the Stage 8 LSO-AMO memory access filter.
* **PMP Writes and Security Pipeline Flushes:**
  * Any write to a `pmpcfg*` or `pmpaddr*` register changes the system's memory security policy.
  * To prevent instructions fetched or executing under stale permissions from executing, a write to any PMP register causes the CSR unit to assert `commit_flush_req_o = 1`.
  * The Writeback stage registers this request, invalidates all in-flight speculative instructions, and redirects the Fetch stage to the sequential next instruction (`PC + 4`) to execute it under the newly established security permissions.

---

## 6.4 Trap Handling

When the Writeback stage detects `trap == 1` at the head of the Retirement Buffer, it asserts `trap_valid_i = 1`. The CSR module resolves the trap as follows:

1. **PC Preservation:** The faulting instruction's PC (`trap_pc_i`) is written to `mepc` (or `sepc` if delegated). The LSB is masked to `0` during write: `mepc <= {trap_pc_i[63:1], 1'b0}`.
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
     assign trap_vector_o = {tvec_val[63:8], (tvec_val[0] && trap_is_interrupt_i) ? trap_cause_i[5:0] : 6'b00, 2'b00};
     ```
   * The calculated address is output on `trap_vector_o` to redirect the Fetch stage.

### 6.5 Instruction Length & Redirection PC Optimization (LSB PC / RVC)

Upon a pipeline redirect (e.g. system-critical write flush or exception/branch flush), the target PC must be calculated. To avoid passing an extra `is_rvc` bit through all pipeline stages and Retirement Buffer arrays, the PV-Crab core utilizes a **PC LSB Optimization**:
* Since RISC-V instructions are always aligned to at least a 16-bit (2-byte) boundary, the actual address bit `PC[0]` is always `0`.
* The `PC[0]` bit is repurposed throughout the pipeline and Retirement FIFO to carry the **`is_rvc` flag** (where `1` indicates a compressed 16-bit instruction and `0` indicates a standard 32-bit instruction).
* At the Decode stage, this bit is injected: `dec_pc[0] = is_rvc`.
* Any stage requiring redirect PC calculation (e.g. Writeback for system flush, Execute for branches) extracts the base address and length, and calculates the next PC locally using a small localized incrementer:
  ```systemverilog
  logic [63:0] base_pc;
  logic [2:0]  instr_len;
  logic [63:0] next_pc;

  assign base_pc   = {stage_pc[63:1], 1'b0};
  assign instr_len = stage_pc[0] ? 3'd2 : 3'd4;
  assign next_pc   = base_pc + instr_len;
  ```
* When writing the PC to CSRs (such as `mepc`/`sepc`), the LSB is masked back to `0`.

---

## 7. Timing and Performance

### 7.1 Critical Paths
Due to the Execute-at-Retire design, the CSR module operates entirely within a single cycle. The critical path involves:
1. Reading the target CSR register value.
2. Routing this value to `commit_rdata_o` for writeback to the GPR file.
3. Performing the ALU operation and updating the register file.

To maintain a target minimum frequency of **100MHz**, the logic must be highly optimized. If timing closure becomes critical, the Retirement/Writeback stage can be split into two sub-stages:
* **Stage 9a (Read & Verify):** Fetch the CSR register value, perform privilege checks, and route `commit_rdata_o` to the GPR file write ports.
* **Stage 9b (Execute & Commit):** Perform the CSR ALU modify operation and write the new value back to the CSR register.

---

## 8. Verification Plan

The CSR unit must undergo rigorous block-level and system-level verification:
1. **Access Control Tests:** Attempt to write to read-only CSRs, and access M-mode registers from S-mode/U-mode to verify illegal instruction traps.
2. **FPU State Checks:** Verify that FPU instructions trap when `mstatus.FS = 0`, and that the dirty bit is set correctly (`mstatus.FS = 2'b11`) upon register updates.
3. **Trap Routing Tests:** Verify correct exception delegation to `sepc`/`scause` using different configurations of `medeleg`/`mideleg`.
4. **Counter Accuracy:** Verify that the `cycle` and `instret` registers increment accurately under heavy pipeline stall scenarios.
