# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

import cocotb
from cocotb.triggers import Timer
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db
from z3 import Solver, BitVec, ZeroExt, sat, Extract
import random
import logging
import grm_alu
import coverage_alu

# --- Testbench Entrypoint ---

OP_MAP = {
    "ADD":  (0, 0, 0),
    "SUB":  (0, 0, 1),
    "AND":  (7, 0, 0),
    "OR":   (6, 0, 0),
    "XOR":  (4, 0, 0),
    "SLL":  (1, 0, 0),
    "SRL":  (1, 0, 1),
    "SRA":  (5, 0, 1),
    "SLT":  (2, 0, 0),
    "SLTU": (3, 0, 0),
    "ADDW": (0, 1, 0),
    "SUBW": (0, 1, 1),
    "SLLW": (1, 1, 0),
    "SRLW": (1, 1, 1),
    "SRAW": (5, 1, 1)
}



@cocotb.test()
async def run_alu_z3_test(dut):
    log = logging.getLogger("cocotb.test")
    log.info("Starting Z3-constrained simulation...")

    matches = 0
    mismatches = 0

    for i in range(10000):
        # 1. Generate stimulus using Z3 solver
        a, b, op_str = coverage_alu.get_z3_stimulus()


        op_clean = str(op_str)
        op_type, w_mode, comp = OP_MAP[op_clean]

        # 2. Drive DUT
        dut.operand_a_i.value = int(a)
        dut.operand_b_i.value = int(b)
        dut.operator_i.operator_type.value = op_type
        dut.operator_i.word_mode.value     = w_mode
        dut.operator_i.complement.value    = comp

        # 3. Wait for combinational settling
        await Timer(1, 'ns')

        # 4. Check for unresolvable values
        if not (dut.operand_a_i.value.is_resolvable and
                dut.operand_b_i.value.is_resolvable and
                dut.result_o.value.is_resolvable):
            log.warning(f"Iteration {i}: skipped comparison due to unresolved values")
            continue

        # 5. Read output from DUT
        res_dut = int(dut.result_o.value)
        operator_raw = (w_mode << 4) | (op_type << 1) | comp
        expected_res, expected_zero = grm_alu.model_alu(a, b, operator_raw)

        if res_dut == expected_res:
            matches += 1
            log.debug(f"Match: {a:#x} {op_clean} {b:#x} = {res_dut:#x}")
        else:
            mismatches += 1
            log.error(f"Mismatch at iteration {i}: {a:#x} {op_clean} {b:#x} = DUT: {res_dut:#x} (Expected: {expected_res:#x})")

        zero_dut = int(dut.zero_o.value)
        if zero_dut != expected_zero:
            mismatches += 1
            log.error(f"ZERO FLAG Mismatch at iter {i}: DUT: {zero_dut} Expected: {expected_zero}")

        # 6. Sample coverage
        coverage_alu.sample_coverage(a, b, op_clean)

    log.info("==================================================")
    log.info(f"Z3 Test Completed. Matches: {matches}, Mismatches: {mismatches}")
    log.info("==================================================")

    # Report functional coverage
    log.info("             FUNCTIONAL COVERAGE REPORT           ")
    log.info("==================================================")
    coverage_db.report_coverage(log.info, bins=True)
    log.info("==================================================")

    # Export coverage database
    import os
    results_dir = os.getenv("RESULTS_DIR", ".")
    cov_path = os.path.join(results_dir, "coverage_z3.xml")
    coverage_db.export_to_xml(cov_path)
    log.info(f"Functional coverage exported to {cov_path}")

    # Assert no mismatches
    assert mismatches == 0, f"Found {mismatches} mismatches during verification!"
    log.info("Z3 Verification PASSED successfully!")
