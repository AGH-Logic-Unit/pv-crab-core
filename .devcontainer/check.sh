#!/usr/bin/env bash
# Devcontainer health check

set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

ok()   { echo -e "  ${GREEN}✓${NC} $1"; }
fail() { echo -e "  ${RED}✗${NC} $1"; ERRORS=$((ERRORS + 1)); }
warn() { echo -e "  ${YELLOW}!${NC} $1"; }

ERRORS=0

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "   Crab Core — environment check"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# --- Tools ---
echo ""
echo "▸ Tools"

check_tool() {
    local name="$1" cmd="$2"
    local bin
    bin=$(echo "$cmd" | awk '{print $1}')
    if ! command -v "$bin" &>/dev/null; then
        fail "$name — not in PATH"
        return
    fi
    local ver
    ver=$(eval "$cmd" 2>/dev/null | head -1) || ver="(version unknown)"
    ok "$name — $ver"
}

check_tool "Verilator" "verilator --version"
check_tool "Verible fmt" "verible-verilog-format --version"
check_tool "Verible lint" "verible-verilog-lint --version"
check_tool "Verible ls" "verible-verilog-ls --version"
check_tool "svlangserver" "svlangserver --version"
check_tool "Yosys" "yosys --version"
check_tool "SymbiYosys (sby)" "sby --version"
check_tool "Spike" "spike --version"
check_tool "lcov" "lcov --version"
check_tool "rg" "rg --version"
check_tool "riscv64-gcc" "riscv64-unknown-elf-gcc --version"
check_tool "z3" "z3 --version"
check_tool "node" "node --version"

# --- riscv-tests ---
echo ""
echo "▸ RISC-V Tests"
if [[ -d "${RISCV_TESTS:-/opt/riscv-tests}/isa" ]]; then
    count=$(find "${RISCV_TESTS:-/opt/riscv-tests}/isa" -name 'rv64*' -not -name '*.dump' | wc -l)
    ok "riscv-tests ISA — $count ELF binaries at ${RISCV_TESTS:-/opt/riscv-tests}/isa/"
else
    fail "riscv-tests — not found at ${RISCV_TESTS:-/opt/riscv-tests}/isa/"
fi

# --- Python packages ---
echo ""
echo "▸ Python (venv)"

VENV_PIP="/home/vscode/venv/bin/pip"
REQ_FILE="$(dirname "$(realpath "$0")")/requirements.txt"

if [[ ! -f "$VENV_PIP" ]]; then
    fail "venv not found at /home/vscode/venv"
else
    if [[ -f "$REQ_FILE" ]]; then
        # Check for missing packages
        missing=$("$VENV_PIP" install --dry-run -r "$REQ_FILE" 2>&1 \
            | grep "^Would install" | sed 's/Would install //' | tr ' ' '\n' || true)
        if [[ -z "$missing" ]]; then
            ok "All packages from requirements.txt installed"
        else
            warn "Packages out of date / missing — running pip install..."
            "$VENV_PIP" install -q -r "$REQ_FILE"
            ok "Packages updated"
        fi
    else
        warn "requirements.txt not found, skipping package check"
    fi

    # Spot-check key packages
    for pkg in cocotb pytest ruff pre-commit z3-solver rich loguru bitwuzla; do
        if "$VENV_PIP" show "$pkg" &>/dev/null; then
            ver=$("$VENV_PIP" show "$pkg" | grep "^Version:" | awk '{print $2}')
            ok "$pkg $ver"
        else
            fail "$pkg — not installed"
        fi
    done
fi

# --- pre-commit hooks ---
echo ""
echo "▸ pre-commit"

if [[ -f ".git/hooks/pre-commit" ]]; then
    ok "pre-commit hook installed"
else
    warn "pre-commit hook missing — installing..."
    pre-commit install && pre-commit install --hook-type commit-msg
    ok "pre-commit installed"
fi

# --- Summary ---
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
if [[ $ERRORS -eq 0 ]]; then
    echo -e " ${GREEN}✓ Environment OK${NC}"
else
    echo -e " ${RED}✗ $ERRORS issue(s) found — check output above${NC}"
fi
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

exit $ERRORS
