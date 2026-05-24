---
title: Types
---

## Execute stage

### `t__exe_headers`

| Signal | Type | Width | Description |
| :--- | :--- | :---: | :--- |
| `tag` | `logic` | 4 | Instruction ID tag for scoreboard tracking |
| `rd_addr` | `logic` | 5 | Destination register address (RD) |
| `rd_we` | `logic` | 1 | Register write enable (1 = Write to RD, 0 = No write) |
| `exc_valid` | `logic` | 1 | Exception validity flag (1 = Exception occurred, 0 = No exception) |
| `exc_code` | `logic` | 4 | RISC-V exception cause code (valid when `exc_valid` is 1) |

## Future Scalability (Superscalar Upgrades)

To facilitate future migration to a superscalar (dual-issue or multi-issue) hart, the following architectural guidelines should be followed:

### Register File & Scoreboard
* **Port Parameterization:** Avoid hardcoding the number of read/write ports on the Register File and the Scoreboard. The Scoreboard must support checking dependencies and allocating tags for multiple instructions concurrently.
* **Tag Routing:** For $N$-issue superscalar execution, the dispatcher must allocate up to $N$ unique tags per cycle and route them to their respective execution lanes.
