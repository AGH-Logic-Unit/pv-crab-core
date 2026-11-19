# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0
"""
Pytest integration for addsub cocotb testbenches.
Supports sanity random testing, coverage feedback, and formal Z3 solver verification.
"""

from pathlib import Path

import pytest

from cocotb_test.simulator import run

TEST_DIR = Path(__file__).resolve().parent


@pytest.mark.sanity
def test_addsub_simple(sim_options, rtl_sources):
    """Run baseline random tests for adder/subtractor."""
    sources, incdirs = rtl_sources("rtl/addsub")
    run(
        verilog_sources=sources,
        includes=incdirs,
        toplevel="addsub",
        module="tb_addsub",
        python_search=[str(TEST_DIR)],
        simulator=sim_options["sim"],
        waves=sim_options["waves"],
    )


@pytest.mark.coverage
def test_addsub_feedback(sim_options, rtl_sources):
    """Run coverage-directed feedback testbench."""
    sources, incdirs = rtl_sources("rtl/addsub")
    run(
        verilog_sources=sources,
        includes=incdirs,
        toplevel="addsub",
        module="tb_addsub_feedback",
        python_search=[str(TEST_DIR)],
        simulator=sim_options["sim"],
        waves=sim_options["waves"],
    )


@pytest.mark.z3
def test_addsub_z3(sim_options, rtl_sources):
    """Run formal constraint generation testbench using Z3 solver."""
    sources, incdirs = rtl_sources("rtl/addsub")
    run(
        verilog_sources=sources,
        includes=incdirs,
        toplevel="addsub",
        module="tb_addsub_z3",
        python_search=[str(TEST_DIR)],
        simulator=sim_options["sim"],
        waves=sim_options["waves"],
    )
