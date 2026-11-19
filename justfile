# ==============================================================================
# Pulsar-V Crab Core - Project Justfile
# ==============================================================================

set shell := ["bash", "-uc"]

default:
    @just --list

# ------------------------------------------------------------------------------
# VERIFICATION & TESTING (Cocotb + Pytest)
# ------------------------------------------------------------------------------

# Run verification test suite (e.g. 'just test' or 'just test -m sanity' or 'just test -k addsub')
test *args="":
    pytest verif/ {{args}}

# Run test with simulation waveforms enabled for Surfer (VCD/FST)
test-waves test_path="verif/":
    WAVES=1 pytest -s {{test_path}}

# Run test suite in parallel across available CPU cores (pytest-xdist)
test-parallel *args="":
    pytest -n auto verif/ {{args}}

# Clean simulation outputs, build directories and waveform dumps
clean:
    rm -rf output/ regression/ sim_build/ .pytest_cache .ruff_cache results.xml *.vcd *.fst

# ------------------------------------------------------------------------------
# SOURCE & FILELIST MANAGEMENT
# ------------------------------------------------------------------------------

# Generate filelist for Verible Language Server and simulators
filelist:
    @python3 scripts/gen_filelist.py -o verible.filelist

# ------------------------------------------------------------------------------
# CODE QUALITY & FORMATTING
# ------------------------------------------------------------------------------

# Lint SystemVerilog and Python sources (Slang, Verible, Ruff, License)
lint:
    @echo "[LINT] Checking license headers..."
    @just check-license
    @echo "[LINT] Checking SystemVerilog sources..."
    @if [ -d rtl ]; then \
        files=$(find rtl prim -type f \( -name '*.sv' -o -name '*.v' \) 2>/dev/null); \
        if [ -n "$files" ]; then \
            slang --lint-only $files; \
            verible-verilog-lint --rules_config=.rules.verible_lint $files; \
        fi; \
    fi
    @echo "[LINT] Checking Python scripts..."
    @if [ -d verif ]; then \
        ruff check scripts/ verif/; \
    else \
        ruff check scripts/; \
    fi

# Format SystemVerilog and Python sources in place
fmt:
    @echo "[FMT] Formatting SystemVerilog sources..."
    @if [ -d rtl ]; then \
        files=$(find rtl prim -type f \( -name '*.sv' -o -name '*.v' \) 2>/dev/null); \
        if [ -n "$files" ]; then \
            verible-verilog-format --flagfile=.rules.verible_format --inplace $files; \
        fi; \
    fi
    @echo "[FMT] Formatting Python scripts..."
    @if [ -d verif ]; then \
        ruff format scripts/ verif/; \
        ruff check --fix scripts/ verif/; \
    else \
        ruff format scripts/; \
        ruff check --fix scripts/; \
    fi

# Check license headers across codebase
check-license:
    @files=$(find rtl prim verif scripts -type f \( -name '*.sv' -o -name '*.v' -o -name '*.svh' -o -name '*.vh' -o -name '*.py' \) 2>/dev/null); \
    if [ -n "$files" ]; then \
        python3 scripts/check_license.py $files; \
    fi

# Validate branch commits against Conventional Commits
check-commits base="origin/main":
    @npx --yes @commitlint/cli --from {{base}} --to HEAD

# ------------------------------------------------------------------------------
# DOCUMENTATION (Zensical)
# ------------------------------------------------------------------------------

# Start local live-reloading documentation server
docs:
    zensical serve

# Build static documentation site
docs-build:
    zensical build
