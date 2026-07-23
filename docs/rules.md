---
title: Repository standards
---

# Repository Standards and Guidelines

This document describes the coding standards, developer tools, verification processes, and CI/CD workflows established in the **Pulsar-V Crab Core** repository. All contributors must follow these guidelines to ensure codebase consistency, high quality, and smooth collaboration.

---

## 1. Development Environment (Workspace Setup)

To guarantee an identical environment for every developer and eliminate "works on my machine" issues, the project fully supports and recommends using the predefined **Devcontainer**.

### Starting the Environment
You can launch the development container using [DevPod](https://devpod.sh/) or the VS Code [Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers) extension:
```bash
devpod up .
```

### Container Specifications (`.devcontainer/Dockerfile`)
The development container image is built on **Debian 12 Slim** and includes all critical dependencies for HDL design and verification:

*   **Compilation & Build Tools:** `git`, `make`, `autoconf`, `g++`, `ccache`, `yosys`.
*   **RISC-V Toolchain:** `gcc-riscv64-unknown-elf`, `picolibc-riscv64-unknown-elf` (required to compile software targeted for the core).
*   **Simulator:** [Verilator](https://verilator.org/) version `5.046` (compiled from source).
*   **HDL Linter & Formatter:** [Verible](https://github.com/chipsalliance/verible) (version `v0.0-4080-ga0a8d8eb`).
*   **Python Toolchain:** A dedicated virtual environment at `/home/vscode/venv` pre-installed with [cocotb](https://www.cocotb.org/), [pytest](https://docs.pytest.org/), [cocotb-test](https://github.com/the-cocotb-foundation/cocotb-test), [cocotb-coverage](https://github.com/cocotb/cocotb-coverage), [z3-solver](https://github.com/Z3Prover/z3), [Ruff](https://astral.sh/ruff), [Zensical](https://zensical.org/), and helper libraries.

### VS Code Integration (`.devcontainer/devcontainer.json`)
When using VS Code inside the container, the editor is pre-configured with:

*   **Extensions:** `mshr-h.veriloghdl` (HDL support), `surfer-project.surfer` (fast waveform viewer), `ms-python.python` (Python test running), `tamasfe.even-better-toml` (TOML formatting), and `eamodio.gitlens` (Git utility).
*   **Auto-formatting:** Enabled on save (`editor.formatOnSave: true`) utilizing `verible-verilog-format` (2-space indent, 120-column limit).
*   **Linting:** Background linting using `verible-verilog-lint` guided by `.rules.verible_lint`.

---

## 2. Code Quality and Pre-commit Hooks

The repository uses [pre-commit](https://pre-commit.com/) hooks to automatically validate and format files before changes are committed to Git. The hooks are set up automatically when the devcontainer is created:
```bash
# To manually install or update hooks:
pre-commit install && pre-commit install --hook-type commit-msg
```

All hook behaviors are defined in `.pre-commit-config.yaml`:

### General Quality Hooks:
*   `trailing-whitespace` – Trims trailing whitespace from all lines.
*   `end-of-file-fixer` – Ensures files end with a newline.
*   `check-yaml` – Validates YAML syntax.
*   `check-added-large-files` – Blocks accidental staging of large binary files.

### Commit Guidelines (Conventional Commits):
The [conventional-pre-commit](https://github.com/compilerla/conventional-pre-commit) hook validates commit messages during the `commit-msg` stage. Commit messages must follow the [Conventional Commits](https://www.conventionalcommits.org/) format:
`<type>(<scope>): <description>` (e.g., `feat(alu): add 64-bit addition support`).

*   **Allowed Scopes:** `alu`, `lsu`, `fpu`, `mul`, `div`, `core`.
*   **Strict Mode:** Enforces compliance strictly.

### SystemVerilog Tooling (Verible):
Whenever `.v` or `.sv` files are modified, pre-commit runs the following local tools:

1.  **Formatter:** `verible-verilog-format` using settings from `.rules.verible_format` (overwrites the files inline via `--inline`).
2.  **Linter:** `verible-verilog-lint` matching rules in `.rules.verible_lint`.

---

## 3. Style and Linting Rules (SystemVerilog)

### Formatting (`.rules.verible_format`)
*   **Indentation:** 2 spaces (no tabs allowed).
*   **Line Limit:** 120 characters (`--column_limit=120`).
*   **Alignment:** Auto-aligns assignment statements, module port lists, net/variable declarations, struct/union members, case items, and parameters.
*   **Port & Parameter Indentation:** Indented relative to the module declaration (`indent`).

### Linting Rules (`.rules.verible_lint`)
*   **Critical RTL Quality Rules:**
    *   `+always-ff-non-blocking` – Requires non-blocking assignments (`<=`) inside sequential (`always_ff`) blocks.
    *   `+always-comb` – Mandates modern `always_comb` blocks over legacy `always @(*)`.
    *   `+always-comb-blocking` – Enforces blocking assignments (`=`) inside combinational `always_comb` blocks.
    *   `+no-tabs` – Prohibits tabs in source files.
*   **Style and Naming Rules:**
    *   `+typedef-enums` – Requires defining enum types using `typedef`.
    *   `+enum-name-style` – Enforces a consistent style for enum names.
    *   `+module-filename` / `+package-filename` – Requires the module/package name to match its physical filename.
    *   `+port-name-suffix` – Standardizes port suffixes (e.g., `_i` for inputs, `_o` for outputs).
    *   `+parameter-name-style` / `+signal-name-style` / `+interface-name-style` – Enforces uniform naming styles across signals, parameters, and interfaces.
    *   `+explicit-parameter-storage-type` – Parameters must have an explicitly specified storage type.
    *   `+generate-label` / `+mismatched-labels` – Requires generate blocks to be labeled and verifies label matching.
*   **Disabled Rules:**
    *   `-line-length` – No hard limit enforced by the linter parser (formatting rules still target 120 characters).

---

## 4. Repository Structure & Contribution Workflow

To maintain a clean and modular design, the codebase is strictly organized by concerns. Contributors should locate their changes and construct additions within the corresponding directories.

### Repository Map
*   `rtl/` – Core hardware design files. Each design unit (such as `addsub` or execution stages) occupies its own subfolder.
*   `verif/` – Verification testbenches, reference models, and testlist definitions matching the structure of `rtl/`.
*   `docs/` – Markdown documentation files. Detailed design specifications of microarchitecture stages reside in subfolders (e.g., `docs/uarch/execute/`).
*   `scripts/` – Repository helper and toolchain scripts (e.g., test runners, filelist generators).
*   `regression/` – Holds output results, waveforms, and coverage data generated by test runner executions.
*   `site/` – Compiled production documentation HTML generated by Zensical.

### Contribution Flow

When adding or modifying components, please adhere to the following sequence:

#### A. Adding or Modifying RTL
1. Place new SystemVerilog files inside a dedicated subdirectory in `rtl/` (e.g., `rtl/my_module/my_module.sv`).
2. Module names must match their corresponding filename (e.g., `module my_module` inside `my_module.sv`).
3. Re-generate the global filelist to register the new files:
   ```bash
   python scripts/gen_filelist.py -o verible.filelist
   ```

#### B. Implementing Tests
1. Test directories should mirror the RTL structure under `verif/` (e.g., `verif/my_module/`).
2. Create Python cocotb test benches (`tb_my_module.py`) and write validation scenarios.
3. Define or append a YAML testlist (e.g., `verif/my_module/my_module_l0.yaml`) listing the top-level modules, Python test files, and target settings.
4. Execute testlists locally to confirm validation:
   ```bash
   python -m pytest scripts/test.py --testlist verif/my_module/my_module_l0.yaml --waves
   ```

#### C. Documenting Specifications
1. Create or update design specification Markdown files inside `docs/uarch/` matching the subcomponent's category.
2. Link the new specifications within the documentation index in `docs/index.md`.
3. Validate document compilation locally using Zensical:
   ```bash
   zensical build --clean
   ```

#### D. Submitting Changes
1. Run pre-commit checks locally before staging to resolve styling/lint errors automatically:
   ```bash
   pre-commit run --all-files
   ```
2. Commit your changes using a Conventional Commit style message scoped to the component (e.g., `feat(alu): add double-precision float support`).

---

## 5. Verification Framework & Regression

Hardware verification is powered by [pytest](https://docs.pytest.org/) combined with [cocotb](https://www.cocotb.org/) (via the [cocotb-test](https://github.com/the-cocotb-foundation/cocotb-test) package).

### Generating the Filelist
Before running simulations or starting the Verible Language Server, you must generate a filelist of the SystemVerilog sources:
```bash
python scripts/gen_filelist.py -o verible.filelist
```
This script recursively scans `rtl/` and `verif/` for source files (`.v`, `.sv`) and header files (`.vh`, `.svh`). It places packages (files with `pkg` in their name) at the top of the compilation order to avoid compilation errors.

### YAML Testlists
Test configurations are structured in YAML files within the verification folders (e.g., `verif/addsub/addsub_l0.yaml` in the `examples` branch). A typical testlist structure:
```yaml
config:
  name: "addsub_sanity"
  simulator: "verilator"  # Default simulator
  coverage: false

tests:
  - name: "simple"
    module: tb_addsub     # Name of the Python test bench module (without .py)
    toplevel: addsub      # Name of the SystemVerilog top-level module
    filelist: verible.filelist
```

### Running Tests
All tests are executed via `pytest` using the runner entry script `scripts/test.py`:
```bash
# Run a specific testlist
python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml

# Run tests with a specified filelist
python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --filelist verible.filelist

# Force a specific randomization seed
python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --seed 12345

# Enable waveform dumping (VCD/FST dumps)
python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --waves

# Enable functional/code coverage collection
python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --coverage
```

### Optimization & Results
*   **Shared Build Directory (`output/sim_build`)**: Simulations share a build directory to cache and reuse Verilator compile objects, drastically reducing incremental compile times.
*   **Results Archiving (`regression/results`)**: Upon simulation completion, results (JUnit reports `results.xml`, waveforms `.vcd`/`.fst`, and coverage databases `coverage.dat`/`coverage.xml`) are copied to a designated folder:
    `regression/results/<testlist_name>/<test_name>/`
    Temporary waveforms and build residues in `sim_build` are cleaned up automatically to conserve disk space.

---

## 6. CI/CD Workflows (GitHub Actions)

The repository runs 3 automated GitHub workflows:

1.  **Verible Verification (`verible-check.yaml`)**
    *   **Trigger:** On every Pull Request targeted at the `main` branch.
    *   **Actions:**
        *   Runs `chipsalliance/verible-linter-action` using `.rules.verible_lint` config on modified `.v`, `.sv`, `.vh`, and `.svh` files.
        *   Verifies style formatting using `verible-verilog-format --verify --flagfile=.rules.verible_format` against the PR base branch.
2.  **Devcontainer Deployment (`build-devcontainer.yaml`)**
    *   **Trigger:** On pushes to `main` involving `.devcontainer/Dockerfile` or the workflow itself.
    *   **Actions:** Builds the Docker image and publishes it to GitHub Container Registry at `ghcr.io/agh-logic-unit/pv-crab-core:main`. Employs GHA caches to speed up docker layer generation.
3.  **Documentation Publishing (`docs.yaml`)**
    *   **Trigger:** On pushes to `main` modifying files under `docs/**`, the configuration `zensical.toml`, or the workflow file.
    *   **Actions:** Installs **Zensical**, compiles the markdown document source using `zensical build --clean`, and deploys the contents of `./site` to the `gh-pages` branch.

---

## 7. Project Documentation (Zensical)

Static documentation is generated using [Zensical](https://zensical.org/) (configured in `zensical.toml`).

### Markdown Extensions & Layout
The setup supports rich Markdown extensions suitable for hardware specification:

*   **Mermaid** – Embeds sequence, state, and block diagrams directly in documents using markdown fences.
*   **MathJax** – Evaluates LaTeX formatting for equations (via the `arithmatex` extension).
*   **Admonitions** – Provides notice blocks (e.g., `!!! note "Title"` or `!!! warning`).
*   **Task lists** – Interoperable todo lists.
*   **Code Highlighting** – Styled syntax formatting with line numbering anchors (`anchor_linenums: true`).

The documentation sources reside in `docs/` (as Markdown), with the landing page at `docs/index.md`. The production HTML output goes to the `site/` folder.
