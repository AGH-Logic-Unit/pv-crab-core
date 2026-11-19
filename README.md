# Pulsar-V: Crab Core - Verification Examples

Demonstration of digital verification testbenches running in the AGH Logic Unit Unified Development Environment (UDE).

---

## 1. Development Environment
Open this repository in VS Code using Dev Containers or DevPod:
```bash
devpod up .
```
The container uses `ghcr.io/agh-logic-unit/alu-rtl:latest`, which includes Slang, Verible, Verilator, Cocotb, Z3 solver, and Just preinstalled.

---

## 2. Running Verification Tests
Tests are executed using Pytest and Cocotb:

```bash
just test                  # Run all verification testbenches
just test -m sanity        # Run quick sanity check (random stimulus)
just test -m coverage      # Run functional coverage feedback testbench
just test -m z3            # Run formal constraint generation testbench with Z3
just test-waves            # Run with waveform dumping enabled (view with Surfer)
just test-parallel         # Run testbenches in parallel across CPU cores
```

---

## 3. Code Quality & Standards
```bash
just lint                  # Check license, SystemVerilog (Slang & Verible) and Python (Ruff)
just fmt                   # Format SystemVerilog and Python in place
just check-license         # Verify Apache-2.0 license headers
just check-commits         # Validate local branch commits against Conventional Commits
```

---

## 4. Documentation
```bash
just docs                  # Launch live-reloading documentation server (Zensical)
```
