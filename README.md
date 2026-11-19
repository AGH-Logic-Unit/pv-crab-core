# Pulsar-V: Crab Core

RISC-V processor core developed by AGH Logic Unit at AGH University of Krakow.

---

## 1. Development Environment

This repository uses the AGH Logic Unit Unified Development Environment (Digital UDE) based on Docker, containing pre-configured tools (Slang, Verible, Verilator, Cocotb, Z3 solver, RISC-V toolchains, and Just).

### Getting Started
Open this repository in VS Code using the Dev Containers extension or launch with DevPod:
```bash
devpod up .
```

---

## 2. Common Tasks via Just

Project commands are automated using `just`. Run `just --list` to view all available recipes.

### Verification & Testing
```bash
just test                  # Run verification testbenches (Pytest + Cocotb)
just test-waves            # Run testbenches with waveform dumping (FST/VCD for Surfer)
just test-parallel         # Run test suite in parallel across all CPU cores
```

### Filelists & Simulators
```bash
just filelist              # Generate verible.filelist from rtl/ and prim/
```

### Code Quality & Standards
```bash
just lint                  # Run Slang linter, Verible linter, Ruff, and check license headers
just fmt                   # Format SystemVerilog (Verible) and Python (Ruff) in place
just check-license         # Verify SPDX Apache-2.0 license headers
just check-commits         # Validate local branch commits against Conventional Commits
```

### Documentation
```bash
just docs                  # Launch live-reloading documentation server (Zensical)
just docs-build            # Build static documentation site
```
