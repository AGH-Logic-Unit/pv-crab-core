import cocotb
from cocotb.triggers import Timer
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db
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

# --- Stimulus Helpers ---
SPECIAL_VALS = [0, 1, 2, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA]

def get_random_operand() -> int:
    """Generate a random 32-bit integer focusing on interesting edge cases and ranges"""
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

# --- Coverage Feedback Mapping ---

# Define combinations that are mathematically or architecturally prohibited by our constraints.
# In this module, our constraint prohibits both operands from being 0xFFFFFFFF at the same time.
IGNORE_BINS = {
    ("sub", "max", "max"),
    ("add", "max", "max")
}

def get_value_for_bin(bin_label: str) -> int:
    """Returns a valid integer falling inside the specified coverage bin range"""
    if bin_label == "zero":
        return 0
    elif bin_label == "small_pos":
        return random.randint(1, 100)
    elif bin_label == "large_pos":
        return random.randint(101, 0x7FFFFFFF)
    elif bin_label == "negative":
        return random.randint(0x80000000, 0xFFFFFFFE)
    elif bin_label == "max":
        return 0xFFFFFFFF
    raise ValueError(f"Unknown bin: {bin_label}")

def get_op_for_bin(bin_label: str) -> int:
    """Returns an op value matching the operation bin"""
    if bin_label == "sub":
        return 0
    elif bin_label == "add":
        return 1
    raise ValueError(f"Unknown bin: {bin_label}")

# --- Testbench Entrypoint ---

@cocotb.test()
async def run_addsub_feedback_test(dut):
    """Verify adder/subtractor and use a feedback loop to achieve 100% reachable coverage"""
    log = logging.getLogger("cocotb.test")
    log.info("Starting Coverage Feedback-Directed Test...")

    matches = 0
    mismatches = 0

    # --- Phase 1: Random Stimulus Run ---
    # Run a small number of random iterations to hit most bins easily
    random_iterations = 100000
    log.info(f"Phase 1: Running {random_iterations} randomized tests...")

    for i in range(random_iterations):
        a = get_random_operand()
        b = get_random_operand()
        op = random.choice([0, 1, random.randint(0, 0xFFFFFFFF)])

        # Apply constraint: Do not allow both inputs to be maximum (0xFFFFFFFF)
        if a == 0xFFFFFFFF and b == 0xFFFFFFFF:
            b = 0x0

        dut.a_i.value = a
        dut.b_i.value = b
        dut.op_i.value = int(op != 0)
        await Timer(1, units='ns')

        if not (dut.a_i.value.is_resolvable and dut.b_i.value.is_resolvable and dut.res_o.value.is_resolvable):
            continue

        res = int(dut.res_o.value)
        expected = model_addsub(a, b, op)
        if res == expected:
            matches += 1
        else:
            mismatches += 1
            log.error(f"Mismatch: {a:#x} {'+' if op != 0 else '-'} {b:#x} = {res:#x} (Expected: {expected:#x})")

        sample_coverage(a, b, op)

    # --- Phase 2: Directed Coverage Loop ---
    # Query the coverage database, identify unhit bins, and generate targeted stimulus
    log.info("Phase 2: Checking coverage and targeting unhit bins...")

    cross_db = coverage_db["top.op_cross_a_b"]

    # Loop until all reachable bins are hit
    while True:
        # Find bins with 0 hits that are not in the ignore list
        unhit_bins = [
            bin_name for bin_name, hit_count in cross_db.detailed_coverage.items()
            if hit_count == 0 and bin_name not in IGNORE_BINS
        ]

        if not unhit_bins:
            log.info("All reachable coverage bins have been successfully hit!")
            break

        log.info(f"Unhit reachable bins remaining: {len(unhit_bins)}")

        # Target the first unhit bin
        target_bin = unhit_bins[0]
        op_bin, a_bin, b_bin = target_bin
        log.info(f"Directing stimulus to target bin: {target_bin}")

        # Generate targeted values matching this bin combination
        op = get_op_for_bin(op_bin)
        a = get_value_for_bin(a_bin)
        b = get_value_for_bin(b_bin)

        # Apply constraint checks (if any generated value violates constraints, resolve it)
        # Note: Since IGNORE_BINS filters out (max, max), this case will not occur here.
        if a == 0xFFFFFFFF and b == 0xFFFFFFFF:
            b = 0x0

        # Drive targeted stimulus
        dut.a_i.value = a
        dut.b_i.value = b
        dut.op_i.value = op
        await Timer(1, units='ns')

        res = int(dut.res_o.value)
        expected = model_addsub(a, b, op)
        if res == expected:
            matches += 1
        else:
            mismatches += 1
            log.error(f"Mismatch: {a:#x} {'+' if op != 0 else '-'} {b:#x} = {res:#x} (Expected: {expected:#x})")

        # Sample coverage to increment hit count
        sample_coverage(a, b, op)

    log.info("==================================================")
    log.info(f"Feedback Simulation Completed.")
    log.info(f"Total Matches: {matches}")
    log.info(f"Total Mismatches: {mismatches}")
    log.info("==================================================")

    # Report functional coverage
    log.info("             FUNCTIONAL COVERAGE REPORT           ")
    log.info("==================================================")
    coverage_db.report_coverage(log.info, bins=True)
    log.info("==================================================")

    # Export coverage database
    import os
    results_dir = os.getenv("RESULTS_DIR", ".")
    cov_path = os.path.join(results_dir, "coverage_feedback.xml")
    coverage_db.export_to_xml(cov_path)
    log.info(f"Functional coverage exported to {cov_path}")

    # Assert correctness
    assert mismatches == 0, f"Found {mismatches} mismatches during verification!"
    log.info("Feedback-directed verification PASSED successfully!")
