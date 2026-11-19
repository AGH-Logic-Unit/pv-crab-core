# AGH Logic Unit - HDL & Verification Guidelines

Conventions and rules for AI assistants working in AGH Logic Unit repositories.

---

## 1. SystemVerilog & RTL Standards
- Language standard: IEEE 1800-2017. Do not use legacy Verilog-1995/2001 constructs (`reg`, `wire`, `always @(*)`).
- Sequential logic: use `always_ff @(posedge clk or negedge rst_n)` with asynchronous active-low reset.
- Combinational logic: use `always_comb` with blocking assignments (`=`).
- Interfaces and structs: use packed structs for grouped signals and SystemVerilog interfaces for bus protocols.
- Naming rules:
  - Inputs end in `_i`, outputs end in `_o`.
  - Active-low signals end in `_n` (e.g. `rst_n`).
  - Constants and parameters use `UPPER_SNAKE_CASE`.

---

## 2. Verification & Testing (Cocotb 2.x)
- Test framework: Cocotb with pytest runner (`pytest scripts/test.py` triggered via `just test <module>`).
- Clocks and timing: drive clocks using `cocotb.clock.Clock` and await `RisingEdge(dut.clk)`. Do not use busy-wait polling loops.
- Reproducibility: testbenches must respect the `RANDOM_SEED` environment variable.

---

## 3. Workflow via Just
- Run repository tasks through `just` rather than invoking raw commands:
  - `just check` - run Slang syntax check and Ruff linter.
  - `just fmt` - format code with Verible (`verible-verilog-format`) and Ruff.
  - `just test <module>` - run module testbench under Verilator.
  - `just waves <module>` - run testbench and dump FST waveforms for Surfer.
- Syntax checking: use `slang --lint-only <file>.sv` to check syntax without full simulation.

---

## 4. Formatting
- Formatting rules are defined in `.rules.verible_format` and `ruff.toml`.
- Let the automated formatters handle alignment rather than doing manual column alignment.

---

## 5. Git & Commit Message Standards
- Strictly follow the [Conventional Commits](https://www.conventionalcommits.org/) specification: `<type>(<scope>): <summary>`
- Allowed types: standard SemVer-aligned types (`feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`).
- Hardware modules, subsystems, and tooling belong in the **scope**, not the type (e.g. `feat(alu): ...`, `fix(decoder): ...`, `test(dv): ...`, `feat(pcb): ...`, `chore(tapeout): ...`).
- Configuration is standardized via `.commitlintrc.json`.
- Enforced on PR branches in CI. Keep commits atomic so microarchitectural regressions can be isolated with `git bisect`.
