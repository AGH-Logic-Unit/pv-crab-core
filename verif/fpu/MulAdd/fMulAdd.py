# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

import ctypes
from pathlib import Path
import random
import logging
import queue

import cocotb
from cocotb.clock import Clock
from cocotb.triggers import ReadWrite, RisingEdge
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db


ROOT = Path(__file__).resolve().parents[3]
COMPAT_PATH = ROOT / "output" / "testfloat_compat.so"
LIB_PATH = ROOT / "vendor" / "TestFloat-1" / "softfloat.so"

ctypes.CDLL(str(COMPAT_PATH), mode=ctypes.RTLD_GLOBAL)
softfloat = ctypes.CDLL(str(LIB_PATH))

##############################################
##### float-64 expected function configs #####
##############################################

softfloat.f64_add.argtypes = [
    ctypes.c_uint64,
    ctypes.c_uint64,
]
softfloat.f64_add.restype = ctypes.c_uint64

softfloat.f64_sub.argtypes = [
    ctypes.c_uint64,
    ctypes.c_uint64,
]
softfloat.f64_sub.restype = ctypes.c_uint64

softfloat.f64_mul.argtypes = [
    ctypes.c_uint64,
    ctypes.c_uint64,
]
softfloat.f64_mul.restype = ctypes.c_uint64

softfloat.f64_mulAdd.argtypes = [
    ctypes.c_uint64,
    ctypes.c_uint64,
    ctypes.c_uint64,
]
softfloat.f64_mulAdd.restype = ctypes.c_uint64

softfloat_rounding_mode = ctypes.c_ubyte.in_dll(
    softfloat, "softfloat_roundingMode"
)

###################################
##### float-64 expected funcs #####
###################################

def softfloat_add(a, b):
    return softfloat.f64_add(a, b)
def softfloat_sub(a, b):
    return softfloat.f64_sub(a, b)
def softfloat_mul(a, b):
    return softfloat.f64_mul(a, b)
def softfloat_mulAdd(a, b, c):
    return softfloat.f64_mulAdd(a, b, c)


def negate_float(value):
    return value ^ 0x8000000000000000


def softfloat_expected(a, b, c, op, rm):
    softfloat_rounding_mode.value = rm

    match op:
        case 0:
            expected = softfloat_add(a, b)
        case 1:
            expected = softfloat_sub(a, b)
        case 2:
            expected = softfloat_mul(a, b)
        case 4:
            expected = softfloat_mulAdd(a, b, c)
        case 5:
            c = negate_float(c)
            expected = softfloat_mulAdd(a, b, c)
        case 6:
            c = negate_float(c)
            expected = softfloat_mulAdd(a, b, c)
            expected = negate_float(expected)
        case 7:
            expected = softfloat_mulAdd(a, b, c)
            expected = negate_float(expected)
        case _:
            raise ValueError(f"Unsupported fMulAdd opcode: {op}")
    return expected

##########################
##### Coverage model #####
##########################

SPECIAL_VALUES = [
    0x0000000000000000,  # +0.0
    0x8000000000000000,  # -0.0
    0x3FF0000000000000,  # +1.0
    0xBFF0000000000000,  # -1.0
    0x7FF0000000000000,  # +infinity
    0xFFF0000000000000,  # -infinity
    0x7FF8000000000000,  # quiet NaN
]

range_relation = lambda val_, bin_: bin_[0] <= val_ <= bin_[1]
OPCODES = [0, 1, 2, 4, 5, 6, 7]
OPCODE_LABELS = ["FADD", "FSUB", "FMUL", "FMADD", "FMSUB", "FNMSUB", "FNMADD"]
ROUNDING_MODES = [0,1,2,3,4,6]
ROUNDING_MODE_LABELS = ["ROUND_NEAR_EVEN", "ROUND_NEAR_MAXMAG", "ROUND_MINMAG", "ROUND_MIN", "ROUND_MAX", "ROUND_ODD"]
OPERAND_BINS = [
    (0x0000000000000000, 0x7FFFFFFFFFFFFFFF),
    (0x8000000000000000, 0xFFFFFFFFFFFFFFFF),
]
@CoverPoint("top.rm", xf=lambda a, b, c, op, rm: rm,
            bins=ROUNDING_MODES, bins_labels=ROUNDING_MODE_LABELS)
@CoverPoint("top.op", xf=lambda a, b, c, op, rm: op,
            bins=OPCODES, bins_labels=OPCODE_LABELS)
@CoverPoint("top.a_val", xf=lambda a, b, c, op, rm: a, bins=OPERAND_BINS,
            bins_labels=["positive", "negative"], rel=range_relation)
@CoverPoint("top.b_val", xf=lambda a, b, c, op, rm: b, bins=OPERAND_BINS,
            bins_labels=["positive", "negative"], rel=range_relation)
@CoverPoint("top.c_val", xf=lambda a, b, c, op, rm: c, bins=OPERAND_BINS,
            bins_labels=["positive", "negative"], rel=range_relation)
@CoverCross("top.op_cross_a_b_c", items=["top.op", "top.a_val", "top.b_val", "top.c_val", "top.rm"])
def sample_coverage(a, b, c, op, rm):
    pass

# --- Stimulus Generation Helper ---

def get_random_operand() -> int:
    category = random.choice(["special", "positive", "negative"])
    if category == "special":
        return random.choice(SPECIAL_VALUES)

    sign = 0 if category == "positive" else 1
    exponent = random.randint(1, 0x7FE)
    fraction = random.getrandbits(52)
    return (sign << 63) | (exponent << 52) | fraction


def is_nan(value):
    exponent = (value >> 52) & 0x7FF
    fraction = value & ((1 << 52) - 1)
    return exponent == 0x7FF and fraction != 0


def results_match(actual, expected):
    return actual == expected or (is_nan(actual) and is_nan(expected))

#################
##### TESTS #####
#################
def op_string(op):
    return {
        0: ("+", ""),
        1: ("-", ""),
        2: ("x", ""),
        4: ("x", "+"),
        5: ("x", "-"),
        6: ("x", "-"),
        7: ("x", "+"),
    }[op]

LATENCY = 3
@cocotb.test()
async def mulAdd_functional_verif(dut):
    clock = Clock(dut.clk_i, 10, units="ns")
    cocotb.start_soon(clock.start())

    matches = 0
    mismatches = 0

    log = logging.getLogger("cocotb.test")
    log.info("Starting simulation...")

    expected_q = queue.Queue()
    op_q = queue.Queue()
    rm_q = queue.Queue()

    dut.rst_ni.value = 0
    await RisingEdge(dut.clk_i)
    dut.rst_ni.value = 1

    for i in range(100000):
        # Generate stimulus
        a = get_random_operand()
        b = get_random_operand()
        c = get_random_operand()
        op = random.choice(OPCODES)
        rm = random.choice(ROUNDING_MODES)
        sample_coverage(a, b, c, op, rm)

        # Drive the design
        dut.operand_a_i.value = a
        dut.operand_b_i.value = b
        dut.operand_c_i.value = c
        dut.opcode_i.value = op
        dut.rm_i.value = rm

        # Wait for Rising Edge
        await RisingEdge(dut.clk_i)
        await ReadWrite()

        res = int(dut.result_o.value)
        result_valid = int(dut.result_valid_o.value)
        expected_q.put(softfloat_expected(a, b, c, op, rm))
        op_q.put(op_string(op))
        rm_q.put(rm)

        if result_valid:
            expected = expected_q.get()
            op_str = op_q.get()
            result_rm = rm_q.get()
            rm_name = ROUNDING_MODE_LABELS[ROUNDING_MODES.index(result_rm)]

            if results_match(res, expected):
                matches += 1
                # Log matches at debug level to keep logs clean, or info for small tests
                log.debug(f"Match: rm={rm_name} {a:#x} {op_str[0]} {b:#x} {op_str[1]} {c:#x} = {res:#x}")
            else:
                mismatches += 1
                log.error(f"Mismatch at iteration {i}: rm={rm_name} {a:#x} {op_str[0]} {b:#x} {op_str[1]} {c:#x} (Result: {res:#x} Expected: {expected:#x})")

# Print results summary
    log.info("==================================================")
    log.info(f"Test Execution Completed.")
    log.info(f"Matches: {matches}")
    log.info(f"Mismatches: {mismatches}")
    log.info("==================================================")

    # Report functional coverage
    log.info("             FUNCTIONAL COVERAGE REPORT           ")
    log.info("==================================================")
    coverage_db.report_coverage(log.info, bins=True)
    log.info("==================================================")

    # Export coverage database
    import os
    results_dir = os.getenv("RESULTS_DIR", ".")
    cov_path = os.path.join(results_dir, "coverage.xml")
    coverage_db.export_to_xml(cov_path)
    log.info(f"Functional coverage exported to {cov_path}")

    # Assert correctness
    assert mismatches == 0, f"Found {mismatches} mismatches during verification!"
    log.info("Verification PASSED successfully!")
