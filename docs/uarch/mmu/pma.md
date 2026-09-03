---
title: Physical Memory Attributes (PMA)
---

# Physical Memory Attributes (PMA) Module

!!! abstract "Module Card"
    * :material-file-code: **RTL file:** `N/A`
    * :material-progress-wrench: **Status:** `Draft`

## 1. Overview

The **Physical Memory Attributes (PMA)** module decodes the physical hardware characteristics of target memory addresses (`PA`). It determines whether an access targets cacheable DRAM, non-cacheable MMIO peripherals, or idempotent memory, and classifies the level of Atomic Memory Operation (AMO) support available for the region.

## 2. Parameters

| Parameter Name | Type | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `PMA_NUM_REGIONS` | `int unsigned` | `4` | Number of hardware-decoded physical address regions |
| `PMA_ADDR_BASE` | `logic [PMA_NUM_REGIONS-1:0][63:0]` | `{64'h0000_0000_8000_0000, 64'h0000_0000_1000_0000, 64'h0000_0000_0200_0000, 64'h0000_0000_0001_0000}` | Base physical addresses for decoded memory regions |
| `PMA_ADDR_MASK` | `logic [PMA_NUM_REGIONS-1:0][63:0]` | `{64'hFFFF_FFFF_8000_0000, 64'hFFFF_FFFF_F000_0000, 64'hFFFF_FFFF_FE00_0000, 64'hFFFF_FFFF_FFFF_0000}` | Address match bitmasks for region bounds evaluation |
| `PMA_CACHEABLE` | `logic [PMA_NUM_REGIONS-1:0]` | `{1'b1, 1'b0, 1'b0, 1'b1}` | Cacheability attribute per region (`1` = Cacheable DRAM/ROM, `0` = Uncached MMIO) |
| `PMA_AMO_LEVEL` | `pma_amo_level_t [PMA_NUM_REGIONS-1:0]` | `{PMA_AMO_ARITHMETIC, PMA_AMO_NONE, PMA_AMO_SWAP, PMA_AMO_NONE}` | Atomic memory operation capability per region |

### Default SoC Memory Map Regions

| Region Index | Region Name | Address Range | Size | Cacheable | Default AMO Level | Description |
| :---: | :--- | :--- | :---: | :---: | :---: | :--- |
| `0` | **DRAM (Main Memory)** | `0x8000_0000`–`0xFFFF_FFFF` | 2 GiB | Yes (`1`) | `PMA_AMO_ARITHMETIC` | Main memory (DRAM / HyperRAM), full AMO and LR/SC support. |
| `1` | **MMIO Peripherals** | `0x1000_0000`–`0x1FFF_FFFF` | 256 MiB | No (`0`) | `PMA_AMO_NONE` | Standard MMIO peripherals (UART, SPI, Timers). AMOs forbidden. |
| `2` | **CLINT & PLIC** | `0x0200_0000`–`0x03FF_FFFF` | 32 MiB | No (`0`) | `PMA_AMO_SWAP` | Interrupt controllers; supports `AMOSWAP` synchronization. |
| `3` | **Boot ROM** | `0x0001_0000`–`0x0001_FFFF` | 64 KiB | Yes (`1`) | `PMA_AMO_NONE` | On-chip Boot ROM (Read-Only), AMOs forbidden. |


## 3. Interfaces

| Signal | Type | Width | Direction | Description |
| :--- | :---: | :---: | :---: | :--- |
| `pa_i` | `logic` | 56 | IN | Target physical address to evaluate (`PA[55:0]`) |
| `pte_pbmt_i` | `logic` | 2 | IN | Page-Based Memory Type bits (`PTE[62:61]`) from Sv39 page table |
| `pma_cacheable_o` | `logic` | 1 | OUT | Region cacheability flag (`1` = Cacheable DRAM, `0` = Bypass Cache) |
| `pma_amo_level_o` | `pma_amo_level_t` | 2 | OUT | Effective atomic capability level for target address |
| `pma_idempotent_o` | `logic` | 1 | OUT | Idempotency attribute (`1` = Idempotent, `0` = Strict Non-Idempotent) |

## 4. Types: `pma_amo_level_t` (PMA Atomic Capability Levels)

Used to classify the supported Atomic Memory Operations for physical memory regions:

| Element Name | Encoding | Capability Level | Description |
| :--- | :---: | :--- | :--- |
| `PMA_AMO_NONE` | `2'b00` | None | No atomic operations supported (standard MMIO peripherals, ROM). |
| `PMA_AMO_SWAP` | `2'b01` | Swap Only | Supports `AMOSWAP` only (simple synchronization registers). |
| `PMA_AMO_LOGICAL` | `2'b10` | Logical | Supports `AMOSWAP`, `AMOAND`, `AMOOR`, `AMOXOR` (bitmask controllers). |
| `PMA_AMO_ARITHMETIC` | `2'b11` | Arithmetic & Full | Supports all AMOs (`AMOADD`, `AMOMIN`, `AMOMAX`, etc.) and `LR/SC`. |

## 5. RISC-V `Svpbmts` Page-Based Memory Types (`PTE[62:61]`)

In Virtual Memory mode (Sv39), the PMA evaluation logic integrates with the `Svpbmts` extension bits extracted from the Page Table Entry (`PTE[62:61]`):

| `PTE[62:61]` (PBMT) | Memory Type Name | Effective Cache & Bus Behavior |
| :--- | :--- | :--- |
| `2'b00` | **PMA (Default)** | Attributes derived directly from physical PMA region map decoders. |
| `2'b01` | **NC (Non-Cacheable)** | Bypasses L1 Cache; treats region as idempotent main memory. |
| `2'b10` | **IO (Uncached I/O)** | Bypasses L1 Cache; enforces non-speculative, strict RVWMO order for MMIO. AMOs disabled (`PMA_AMO_NONE`). |
| `2'b11` | - | Reserved (triggers Page Fault `cause = 13/15`). |

## 6. Evaluation and Override Rules

### 6.1 PBMT Override Matrix
The resulting attributes emitted by the PMA Checker are resolved according to the following evaluation priority:

| `PBMT` | `pma_cacheable_o` | `pma_idempotent_o` | `pma_amo_level_o` | Exception |
| :---: | :---: | :---: | :---: | :--- |
| `2'b00` (Default) | Region Decoder | Region Decoder | Region Decoder | None |
| `2'b01` (NC) | `0` (Override) | `1` (Idempotent) | Region Decoder | None |
| `2'b10` (IO) | `0` (Override) | `0` (Non-idempotent) | `PMA_AMO_NONE` (Override) | None |
| `2'b11` (Reserved) | - | - | - | Page Fault (`cause = 13/15`) |

### 6.2 AMO Execution Verification Rules
During memory execution in Stage 6 (LSU) or PTW Refill, the instruction's atomic intent is validated against `pma_amo_level_o`:

| Instruction Class | Specific Instructions | Required Minimal PMA Level |
| :--- | :--- | :---: |
| **Reservation** | `LR.W`, `LR.D`, `SC.W`, `SC.D` | `PMA_AMO_ARITHMETIC` |
| **Swap Atomic** | `AMOSWAP.W`, `AMOSWAP.D` | `PMA_AMO_SWAP` |
| **Logical Atomics** | `AMOAND`, `AMOOR`, `AMOXOR` (`.W` / `.D`) | `PMA_AMO_LOGICAL` |
| **Arithmetic Atomics** | `AMOADD`, `AMOMIN`, `AMOMAX`, `AMOMINU`, `AMOMAXU` (`.W` / `.D`) | `PMA_AMO_ARITHMETIC` |

!!! warning "Access Fault on Unsupported AMO"
    If an atomic operation is attempted on a physical address region with insufficient PMA capability (or `PBMT == IO`), the core immediately raises a **Store/AMO Access Fault (`cause = 7`)**.
