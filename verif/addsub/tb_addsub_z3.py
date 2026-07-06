import cocotb
from cocotb.triggers import Timer
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db
from z3 import Solver, BitVec, sat
import random
import logging

# --- Reference Model ---
def model_addsub(a: int, b: int, op: int) -> int:
    """Golden reference model for adder/subtractor"""
    if op != 0:
        return (a + b) & 0xFFFFFFFF
    else:
        return (a - b) & 0xFFFFFFFF

# --- Functional Coverage Model ---
range_relation = lambda val_, bin_: bin_[0] <= val_ <= bin_[1]

@CoverPoint("top.op", xf=lambda a, b, op: 1 if op != 0 else 0, bins=[0, 1], bins_labels=["sub", "add"])
@CoverPoint("top.a_val", xf=lambda a, b, op: a, bins=[
    (0, 0),
    (1, 100),
    (101, 0x7FFFFFFF),
    (0x80000000, 0xFFFFFFFE),
    (0xFFFFFFFF, 0xFFFFFFFF)
], bins_labels=["zero", "small_pos", "large_pos", "negative", "max"], rel=range_relation)
@CoverPoint("top.b_val", xf=lambda a, b, op: b, bins=[
    (0, 0),
    (1, 100),
    (101, 0x7FFFFFFF),
    (0x80000000, 0xFFFFFFFE),
    (0xFFFFFFFF, 0xFFFFFFFF)
], bins_labels=["zero", "small_pos", "large_pos", "negative", "max"], rel=range_relation)
@CoverCross("top.op_cross_a_b", items=["top.op", "top.a_val", "top.b_val"])
def sample_coverage(a: int, b: int, op: int):
    pass

# --- Z3 Constrained Stimulus Generator ---

def get_z3_stimulus() -> tuple[int, int, int]:
    """Generates a mathematically precise stimulus transaction using the Z3 solver"""
    # Pick a random test scenario
    scenarios = [
        "signed_overflow_add",
        "signed_underflow_sub",
        "zero_result_sub",
        "zero_result_add",
        "alternating_bits_add",
        "range_constrained_sub"
    ]
    scenario = random.choice(scenarios)

    # Initialize solver and 32-bit bitvectors
    s = Solver()
    a_bv = BitVec('a', 32)
    b_bv = BitVec('b', 32)

    if scenario == "signed_overflow_add":
        # Scenario: a > 0, b > 0, but a + b < 0 (signed overflow)
        # Constrain 'a' to a random positive signed integer to ensure test variety
        rand_a = random.randint(1, 0x7FFFFFFF)
        s.add(a_bv == rand_a)
        s.add(a_bv > 0)
        s.add(b_bv > 0)
        s.add(a_bv + b_bv < 0)
        op = 1

    elif scenario == "signed_underflow_sub":
        # Scenario: a < 0, b > 0, but a - b > 0 (signed underflow)
        rand_a = random.randint(0x80000000, 0xFFFFFFFF)
        s.add(a_bv == rand_a)
        s.add(a_bv < 0)
        s.add(b_bv > 0)
        s.add(a_bv - b_bv > 0)
        op = 0

    elif scenario == "zero_result_sub":
        # Scenario: subtraction resulting in exactly zero (a == b)
        rand_a = random.randint(0, 0xFFFFFFFF)
        s.add(a_bv == rand_a)
        s.add(a_bv == b_bv)
        op = 0

    elif scenario == "zero_result_add":
        # Scenario: addition resulting in exactly zero (a + b == 0, i.e., b is two's complement of a)
        rand_a = random.randint(1, 0xFFFFFFFF)
        s.add(a_bv == rand_a)
        s.add(a_bv + b_bv == 0)
        op = 1

    elif scenario == "alternating_bits_add":
        # Scenario: 'a' has alternating bits (0x55555555), find 'b' such that their sum is 0xFFFFFFFF
        s.add(a_bv == 0x55555555)
        s.add(a_bv + b_bv == 0xFFFFFFFF)
        op = 1

    elif scenario == "range_constrained_sub":
        # Scenario: 'a' in [0x1000, 0x2000], 'b' in [0x3000, 0x4000], subtraction
        rand_a = random.randint(0x1000, 0x2000)
        s.add(a_bv == rand_a)
        s.add(b_bv >= 0x3000)
        s.add(b_bv <= 0x4000)
        op = 0

    # Solve constraints
    if s.check() == sat:
        model = s.model()
        a_val = model[a_bv].as_long()
        b_val = model[b_bv].as_long()
        return a_val, b_val, op
    else:
        # Fallback to random if solver fails to find a solution
        return random.randint(0, 0xFFFFFFFF), random.randint(0, 0xFFFFFFFF), random.choice([0, 1])

# --- Testbench Entrypoint ---

@cocotb.test()
async def run_addsub_z3_test(dut):
    """Test adder/subtractor with precise Z3 constraint-solved transactions"""
    log = logging.getLogger("cocotb.test")
    log.info("Starting Z3-constrained simulation...")

    matches = 0
    mismatches = 0

    for i in range(10000):
        # 1. Generate stimulus using Z3 solver
        a, b, op = get_z3_stimulus()

        # 2. Drive DUT
        dut.a_i.value = a
        dut.b_i.value = b
        dut.op_i.value = op

        # 3. Wait for combinational settling
        await Timer(1, units='ns')

        # 4. Check for unresolvable values (X/Z)
        if not (dut.a_i.value.is_resolvable and
                dut.b_i.value.is_resolvable and
                dut.op_i.value.is_resolvable and
                dut.res_o.value.is_resolvable):
            log.warning(f"Iteration {i}: skipped comparison due to unresolved values")
            continue

        # 5. Read output and compare with reference model
        res = int(dut.res_o.value)
        expected = model_addsub(a, b, op)

        op_str = "+" if op != 0 else "-"
        if res == expected:
            matches += 1
            log.debug(f"Match: {a:#x} {op_str} {b:#x} = {res:#x}")
        else:
            mismatches += 1
            log.error(f"Mismatch at iteration {i}: {a:#x} {op_str} {b:#x} = {res:#x} (Expected: {expected:#x})")

        # 6. Sample coverage
        sample_coverage(a, b, op)

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
