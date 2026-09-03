---
title: Repository Standards
---

# Repository Standards and Guidelines

This document describes the development environment setup, coding standards, contribution workflows, and verification processes for the **Pulsar-V Crab Core** repository.

---

## 1. Quick Start & Setup

To ensure consistent development environments, the project uses a predefined **Devcontainer** (built on Debian 12 Slim with Verilator, RISC-V toolchains, Verible, Python, and cocotb).

### Getting Started

1.  **Launch the Environment:**

    Open the repository using the VS Code **Dev Containers** extension or run:
    ```bash
    devpod up .
    ```

2.  **Generate the Filelist:**

    Generate a list of SystemVerilog source files for simulation and IDE language servers:
    ```bash
    python scripts/gen_filelist.py -o verible.filelist
    ```

3.  **Install Git Hooks:**

    Enforce commit rules and style formatting locally:
    ```bash
    pre-commit install && pre-commit install --hook-type commit-msg
    ```

---

## 2. Coding & Commit Standards

### SystemVerilog Style and Linting

Formatting and coding checks are automated via **Verible**. Custom configurations are located in `.rules.verible_format` and `.rules.verible_lint`. Key rules include:

*   **Indentation & Limits:** 2 spaces (no tabs), 120-character line limit.

*   **RTL Quality:** Modern SystemVerilog constructs only (e.g., `always_ff` with `<=`, `always_comb` with `=`).

*   **Naming Conventions:** Standardized port suffixes (`_i` for inputs, `_o` for outputs) and type suffixes (`_t` for typedefs).

*   **Auto-Formatting:** Apply the formatter to your changes automatically before committing:
    ```bash
    pre-commit run --all-files
    ```

### Commit Guidelines (Conventional Commits)

Commit messages are linted during the `commit-msg` hook and must follow the Conventional Commits specification:
```
<type>(<scope>): <description>
```

*   **Example:** `feat(alu): add double-precision FMA unit`

---

## 3. Contribution Workflow

When implementing new hardware or modifying existing units, follow this standard sequence:

```
[Write RTL Code] -> [Run Filelist Generator] -> [Write cocotb Tests & YAML] -> [Run Verification] -> [Update Docs] -> [Pre-commit & Commit]
```

1.  **Hardware RTL:**

    Place SystemVerilog source files under a dedicated subdirectory in `rtl/` (e.g., `rtl/my_module/`).

    Regenerate the filelist:
    ```bash
    python scripts/gen_filelist.py -o verible.filelist
    ```

2.  **Verification:**

    Add Python cocotb testbenches under `verif/my_module/` and define them in a YAML testlist (e.g., `my_module_l0.yaml`).

    Run the tests locally:
    ```bash
    python -m pytest scripts/test.py --testlist verif/my_module/my_module_l0.yaml --waves
    ```

3.  **Documentation:**

    Add/update specifications in Markdown under `docs/uarch/` and link them in `docs/index.md`.

    Verify documentation builds locally:
    ```bash
    zensical build
    ```

4.  **Submission:**

    Run style/lint checks:
    ```bash
    pre-commit run --all-files
    ```

    Commit changes using a scoped message:
    ```bash
    git commit -m "feat(alu): add double-precision float support"
    ```

---

## 4. Verification Framework

Verification is powered by **cocotb** and **pytest**, orchestrated by `scripts/test.py`.

### Common Test Commands

*   **Run a specific testlist:**
    ```bash
    python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml
    ```

*   **Run with a custom filelist:**
    ```bash
    python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --filelist verible.filelist
    ```

*   **Dump waveforms (VCD/FST):**
    ```bash
    python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --waves
    ```

*   **Collect coverage:**
    ```bash
    python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --coverage
    ```

*   **Force a specific randomization seed:**
    ```bash
    python -m pytest scripts/test.py --testlist verif/addsub/addsub_l0.yaml --seed 12345
    ```

### Build Caching & Results

*   **Shared Compile Cache:** Verilator build outputs are cached in `output/sim_build/` to reduce compilation time.

*   **Simulation Artifacts:** Output files (waveforms, coverage databases, and JUnit reports) are moved to `regression/results/<testlist>/<test>/` upon completion.

---

## 5. Repository & Tooling Map

### Directory Structure

*   `rtl/` – Core hardware design files.

*   `verif/` – Verification testbenches, reference models, and testlist configurations.

*   `docs/` – Markdown documentation and microarchitectural specifications.

*   `scripts/` – Tooling scripts (e.g., test runners, filelist generators).

*   `regression/` – Test results, waveforms, and coverage outputs.

*   `site/` – Generated HTML documentation site (output of Zensical).

### Automated CI/CD (GitHub Actions)

*   `pull-req-checks.yaml` – Validates Verible syntax, formatting, and linting on every PR.

*   `build-devcontainer.yaml` – Rebuilds and publishes the development Docker container image to GHCR when changes are made.

*   `docs.yaml` – Compiles documentation and deploys the HTML site to GitHub Pages on every push to `main`.
