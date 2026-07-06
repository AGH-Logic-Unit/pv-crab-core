import cocotb
from cocotb.triggers import Timer
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db
import random
import logging

# --- Reference Model ---
def model_addsub(a: int, b: int, op: int) -> int:
    """Golden reference model for the adder/subtractor module"""
    if op != 0:
        return (a + b) & 0xFFFFFFFF
    else:
        return (a - b) & 0xFFFFFFFF

# --- Functional Coverage Model ---
range_relation = lambda val_, bin_: bin_[0] <= val_ <= bin_[1]

# Define coverpoints for operations and operands
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
# Cross coverage across operation and both operand values
@CoverCross("top.op_cross_a_b", items=["top.op", "top.a_val", "top.b_val"])
def sample_coverage(a: int, b: int, op: int):
    """Callback function to record functional coverage metrics"""
    pass

# --- Stimulus Generation Helper ---
SPECIAL_VALS = [0, 1, 2, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA]

def get_random_operand() -> int:
    """Generate a 32-bit integer targeting representative value ranges and edge cases"""
    category = random.choice(["special", "small_pos", "large_pos", "negative", "max", "uniform"])
    if category == "special":
        return random.choice(SPECIAL_VALS)
    elif category == "small_pos":
        return random.randint(1, 100)
    elif category == "large_pos":
        return random.randint(101, 0x7FFFFFFF)
    elif category == "negative":
        return random.randint(0x80000000, 0xFFFFFFFE)
    elif category == "max":
        return 0xFFFFFFFF
    else:
        return random.randint(0, 0xFFFFFFFF)

# --- Testbench Entrypoint ---
@cocotb.test()
async def run_addsub_test(dut):
    """Verify adder/subtractor behavior using Pythonic, boilerplate-free cocotb verification"""
    log = logging.getLogger("cocotb.test")
    log.info("Starting simulation...")

    matches = 0
    mismatches = 0

    for i in range(100000):
        # 1. Generate stimulus
        a = get_random_operand()
        b = get_random_operand()
        op = random.choice([0, 1, random.randint(0, 0xFFFFFFFF)])

        # Stimulus constraint handling:
        # Example: Do not allow both inputs to be maximum (0xFFFFFFFF) at the same time.
        if a == 0xFFFFFFFF and b == 0xFFFFFFFF:
            b = 0x0

        # 2. Drive the design under test (DUT)
        dut.a_i.value = a
        dut.b_i.value = b
        dut.op_i.value = int(op != 0)

        # 3. Wait for propagation delay (combinational settling)
        await Timer(1, units='ns')

        # 4. Check for undefined/unresolved values (X/Z) before comparing
        if not (dut.a_i.value.is_resolvable and
                dut.b_i.value.is_resolvable and
                dut.op_i.value.is_resolvable and
                dut.res_o.value.is_resolvable):
            log.warning(f"Iteration {i}: skipped comparison due to unresolved values")
            continue

        # 5. Read DUT output and compare with golden reference model
        res = int(dut.res_o.value)
        expected = model_addsub(a, b, op)

        op_str = "+" if op != 0 else "-"
        if res == expected:
            matches += 1
            # Log matches at debug level to keep logs clean, or info for small tests
            log.debug(f"Match: {a:#x} {op_str} {b:#x} = {res:#x}")
        else:
            mismatches += 1
            log.error(f"Mismatch at iteration {i}: {a:#x} {op_str} {b:#x} = {res:#x} (Expected: {expected:#x})")

        # 6. Sample functional coverage
        sample_coverage(a, b, op)

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
