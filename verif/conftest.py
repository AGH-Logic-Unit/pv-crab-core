# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

import os
from pathlib import Path

import pytest

PROJ_ROOT = Path(__file__).resolve().parent.parent


def get_rtl_sources(*subdirs: str, root: Path = PROJ_ROOT) -> tuple[list[Path], list[Path]]:
    """Scan given RTL directories and return (verilog_sources, include_dirs).

    Ensures SystemVerilog packages (*pkg*.sv) are compiled first so that type definitions
    and packages are available for dependent modules. Automatically discovers include directories
    containing header files (*.svh, *.vh).
    """
    sources: list[Path] = []
    incdirs: set[Path] = set()

    targets = subdirs if subdirs else ("rtl", "prim")

    for target in targets:
        folder = (root / target).resolve()
        if not folder.exists():
            continue
        if folder.is_file():
            if folder.suffix in (".sv", ".v"):
                sources.append(folder)
            continue

        for f in folder.rglob("*"):
            if f.name.startswith((".", "#", "~")) or f.name.endswith(".bak"):
                continue
            if f.suffix in (".sv", ".v"):
                sources.append(f)
            elif f.suffix in (".svh", ".vh"):
                incdirs.add(f.parent)

    pkgs = sorted([s for s in sources if "pkg" in s.name.lower()])
    modules = sorted([s for s in sources if "pkg" not in s.name.lower()])

    return pkgs + modules, sorted(incdirs)


def pytest_addoption(parser):
    parser.addoption(
        "--sim",
        action="store",
        default=None,
        help="HDL simulator to use (e.g. verilator, icarus). Overrides $SIM env var.",
    )
    parser.addoption(
        "--waves",
        action="store_true",
        default=False,
        help="Enable waveform dumping (VCD/FST). Overrides $WAVES env var.",
    )


def pytest_configure(config):
    """Register custom markers to avoid PytestUnknownMarkWarning."""
    config.addinivalue_line("markers", "sanity: Quick sanity check tests")
    config.addinivalue_line("markers", "coverage: Functional coverage-directed tests")
    config.addinivalue_line("markers", "z3: Formal constraint tests using Z3 solver")
    config.addinivalue_line("markers", "slow: Long-running simulation tests")


@pytest.fixture
def sim_options(request):
    """Fixture providing simulator and waveform settings from CLI or environment."""
    cli_sim = request.config.getoption("--sim")
    cli_waves = request.config.getoption("--waves")

    sim = cli_sim or os.getenv("SIM", "verilator")
    waves = cli_waves or bool(int(os.getenv("WAVES", "0")))

    return {
        "sim": sim,
        "waves": waves,
    }


@pytest.fixture
def rtl_sources():
    """Fixture providing automatic source discovery helper function."""
    return get_rtl_sources


def pytest_sessionfinish(session, exitstatus):
    """Treat empty test suites (exit code 5) as success so branches without tests pass CI."""
    if exitstatus == 5:
        session.exitstatus = 0
