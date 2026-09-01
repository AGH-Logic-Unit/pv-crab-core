# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

import cocotb
from cocotb.triggers import Timer
from cocotb_coverage.coverage import CoverPoint, CoverCross, coverage_db
from z3 import Solver, BitVec, ZeroExt, sat, Extract
import random
import logging


# --- Functional Coverage Model ---
range_relation = lambda val_, bin_: bin_[0] <= val_ <= bin_[1]

@CoverPoint("operator_i", xf=lambda a, b, op: op,
  bins = [
    "ADD","SUB","AND","OR","XOR",
    "SLL","SRL","SRA","SLT","SLTU",
    "ADDW","SUBW","SLLW","SRLW","SRAW"
  ],
  bin_labels = [
    "ADD","SUB","AND","OR","XOR",
    "SLL","SRL","SRA","SLT","SLTU",
    "ADDW","SUBW","SLLW","SRLW","SRAW"
  ])

@CoverPoint("operand_a",xf=lambda a, b, op: a, bins=[
    (0x0000000000000000, 0x0000000000000000),
    (0x0000000000000001, 0x7FFFFFFFFFFFFFFF),
    (0x8000000000000000, 0xFFFFFFFFFFFFFFFE),
    (0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF)
],bin_labels = ["zero","positive", "negative", "max"],rel = range_relation)

@CoverPoint("operand_b",xf=lambda a, b, op: b, bins=[
    (0x0000000000000000, 0x0000000000000000),
    (0x0000000000000001, 0x7FFFFFFFFFFFFFFF),
    (0x8000000000000000, 0xFFFFFFFFFFFFFFFE),
    (0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF)
],bin_labels = ["zero","positive", "negative", "max"],rel = range_relation)

@CoverPoint("operand_a_word",xf=lambda a, b, op: a & 0xFFFFFFFF, bins=[
    (0x00000000, 0x00000000),
    (0x00000001, 0x7FFFFFFF),
    (0x80000000, 0xFFFFFFFE),
    (0xFFFFFFFF, 0xFFFFFFFF)
],bin_labels = ["zero","positive", "negative", "max"],rel = range_relation)

@CoverPoint("operand_b_word",xf=lambda a, b, op: b & 0xFFFFFFFF, bins=[
    (0x00000000, 0x00000000),
    (0x00000001, 0x7FFFFFFF),
    (0x80000000, 0xFFFFFFFE),
    (0xFFFFFFFF, 0xFFFFFFFF)
],bin_labels = ["zero","positive", "negative", "max"],rel = range_relation)

@CoverPoint("shift_amount_64", xf=lambda a, b, op: (b & 0x3F) if op in ["SLL", "SRL", "SRA"] else -1,
    bins=[
        (0,0), (1,1), (2,30),
        (31,31), (32,62), (63,63)
], bin_labels = ["zero", "one", "low", "bit31", "high", "max"],rel = range_relation)

@CoverPoint("shift_amount_32", xf=lambda a, b, op: (b & 0x1F) if op in ["SLLW", "SRLW", "SRAW"] else -1,
    bins=[
        (0,0), (1,1), (2,14),
        (15,15), (16,30), (31,31)
], bin_labels = ["zero", "one", "low", "bit15", "high", "max"],rel = range_relation)


@CoverCross("top.cross_64bit", items=["top.operator_i", "top.operand_a", "top.operand_b"],
    ign_bins={"operator_i": ["ADDW", "SUBW", "SLLW", "SRLW", "SRAW"]})
def sample_64bit(a, b, op):
    pass

@CoverCross("top.cross_word", items=["top.operator_i", "top.operand_a_word", "top.operand_b_word"],
    ign_bins={"operator_i": ["ADD", "SUB", "AND", "OR", "XOR", "SLL", "SRL", "SRA", "SLT", "SLTU"]})
def sample_word(a, b, op):
    pass

@CoverCross("top.cross_shift_64", items=["top.operator_i", "top.shift_amount_64"],
    ign_bins={"operator_i": [
        "ADD", "SUB", "AND", "OR", "XOR", "SLT", "SLTU",
        "ADDW", "SUBW", "SLLW", "SRLW", "SRAW"
    ]})
def sample_shift_cross_64(a, b, op):
    pass

@CoverCross("top.cross_shift_32", items=["top.operator_i", "top.shift_amount_32"],
    ign_bins={"operator_i": [
        "ADD", "SUB", "AND", "OR", "XOR", "SLT", "SLTU",
        "ADDW", "SUBW", "SLL", "SRL", "SRA"
    ]})
def sample_shift_cross_32(a, b, op):
    pass

# --- Scenario Definitions ---

def get_z3_stimulus(scenario_name: str | None = None) -> tuple[int, int, str]:

    scenarios = [
        # ADD / SUB 64-bit
        "ADD_POSITIVE_POSITIVE", "ADD_POSITIVE_NEGATIVE", "ADD_NEGATIVE_NEGATIVE",
        "ADD_SIGNED_OVERFLOW", "ADD_MIN_PLUS_NEGATIVE", "ADD_COMPLEMENT",
        "SUB_POSITIVE_NEGATIVE", "SUB_NEGATIVE_POSITIVE", "SUB_SIGNED_BOUNDARY", "SUB_SIGNED_OVERFLOW",

        # ADDW / SUBW 32-bit
        "ADDW_SIGNED_OVERFLOW", "ADDW_LOW32_OVERFLOW", "ADDW_SIGN_EXTENSION", "ADDW_NEGATIVE_RESULT",
        "SUBW_SIGNED_BOUNDARY", "SUBW_LOW32_WRAP", "SUBW_SIGN_EXTENSION",

        #LOGIC, SHIFTS, COMPARISONS TBD TO BE KEPT OR REMOVED

        # LOGICAL
        "AND_COMPLEMENT", "AND_MASK_RANDOM", "OR_COMPLEMENT", "OR_TOGGLE_MSB_LSB",
        "XOR_TOGGLE_MSB", "XOR_COMPLEMENT",

        # SHIFTS 64-bit & 32-bit
        "SLL_SHIFT_VARIABLE", "SLL_SIGN_BIT_SHIFT", "SRL_SHIFT_VARIABLE",
        "SRA_NEGATIVE_SHIFT", "SRA_POSITIVE_SHIFT",
        "SLLW_SIGN_BIT_OUT", "SRLW_SIGN_BIT_LOGICAL", "SRAW_SIGN_EXTENSION",

        # (SLT / SLTU)
        "SLT_NEGATIVE_POSITIVE", "SLT_POSITIVE_NEGATIVE", "SLT_MIN_MAX",
        "SLTU_NEGATIVE_AS_LARGE_UNSIGNED", "SLTU_SIGNED_VS_UNSIGNED_MISMATCH"
    ]

    if scenario_name is None:
        scenario_name = random.choice(scenarios)

    s = Solver()
    a_bv = BitVec('a', 64)
    b_bv = BitVec('b', 64)

    # Extract op from scenario name
    op = scenario_name.split("_")[0]

    # ========================================================
    # 1. ADD / SUB (64-bit)
    # ========================================================
    if scenario_name == "ADD_POSITIVE_POSITIVE":
        # a > 0, b > 0 without overflow
        rand_a = random.randint(1, 0x3FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv > 0, a_bv + b_bv > 0)

    elif scenario_name == "ADD_POSITIVE_NEGATIVE":
        # a > 0, b < 0, result = 0
        rand_a = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, a_bv + b_bv == 0)

    elif scenario_name == "ADD_NEGATIVE_NEGATIVE":
        # a < 0, b < 0
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv < 0, a_bv + b_bv < 0)

    elif scenario_name == "ADD_SIGNED_OVERFLOW":
        # Positive + Positive = Negative (Overflow in two's complement)
        rand_a = random.randint(0x4000000000000000, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv > 0, a_bv + b_bv < 0)

    elif scenario_name == "ADD_MIN_PLUS_NEGATIVE":
        # INT64_MIN + negative (underflow)
        s.add(a_bv == 0x8000000000000000, b_bv < 0)

    elif scenario_name == "ADD_COMPLEMENT":
        # x + (~x + 1) = 0 (random x)
        rand_a = random.randint(1, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, a_bv + b_bv == 0)

    elif scenario_name == "SUB_POSITIVE_NEGATIVE":
        # a > 0, b < 0 -> positive result
        rand_a = random.randint(1, 0x3FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv < 0, a_bv - b_bv > 0)

    elif scenario_name == "SUB_NEGATIVE_POSITIVE":
        # a < 0, b > 0 -> negative result
        rand_a = random.randint(0xC000000000000000, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv > 0, a_bv - b_bv < 0)

    elif scenario_name == "SUB_SIGNED_BOUNDARY":
        # INT64_MIN - positive
        rand_b = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == 0x8000000000000000, b_bv == rand_b)

    elif scenario_name == "SUB_SIGNED_OVERFLOW":
        # INT64_MAX - negative -> result < 0
        rand_a = random.randint(0x4000000000000000, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv < 0, a_bv - b_bv < 0)

    # ========================================================
    # 2. ADDW / SUBW (32-bit with sign-extension to 64-bit)
    # ========================================================
    elif scenario_name == "ADDW_SIGNED_OVERFLOW":
        # 32-bit signed overflow: a[31] + b[31] = 1 (MSB of result is 1)
        rand_a = random.randint(0x40000000, 0x7FFFFFFF)
        s.add(Extract(31, 0, a_bv) == rand_a)
        s.add(Extract(31, 0, b_bv) > 0)
        s.add(Extract(31, 0, b_bv) <= 0x7FFFFFFF)
        sum_32 = s.add(Extract(31, 0, a_bv + b_bv) >= 0x80000000)
        s.add(Extract(31, 31, sum_32) == 1)  # MSB of the result is 1, indicating signed overflow

    elif scenario_name == "ADDW_LOW32_OVERFLOW":
        # Lower 32 bits exceed 0xFFFFFFFF (wrap around to zero)
        rand_a = random.randint(0x80000000, 0xFFFFFFFF)
        s.add(Extract(31, 0, a_bv) == rand_a)
        a33 = ZeroExt(1, Extract(31, 0, a_bv))
        b33 = ZeroExt(1, Extract(31, 0, b_bv))
        s.add(a33 + b33 == 0x100000000)

    elif scenario_name == "ADDW_SIGN_EXTENSION":
        # Test if MSB (bit 31) correctly extends to bits 63..32
        rand_a = random.randint(0x40000000, 0x7FFFFFFF)
        s.add(Extract(31, 0, a_bv) == rand_a)
        s.add(Extract(31, 0, a_bv + b_bv) == 0x80000000)

    elif scenario_name == "ADDW_NEGATIVE_RESULT":
        # Random values producing a negative 32-bit result
        rand_a = random.randint(0x80000000, 0xFFFFFFFF)
        s.add(Extract(31, 0, a_bv) == rand_a)
        s.add(Extract(31, 0, b_bv) > 0)

    elif scenario_name == "SUBW_SIGNED_BOUNDARY":
        # INT32_MIN - positive
        rand_b = random.randint(1, 0x7FFFFFFF)
        s.add(Extract(31, 0, a_bv) == 0x80000000, Extract(31, 0, b_bv) == rand_b)

    elif scenario_name == "SUBW_LOW32_WRAP":
        # 0 - negative/positive wrapped in 32 bits
        rand_b = random.randint(1, 0x7FFFFFFF)
        s.add(Extract(31, 0, a_bv) == 0, Extract(31, 0, b_bv) == rand_b)

    elif scenario_name == "SUBW_SIGN_EXTENSION":
        # Subtraction result with bit 31 set
        rand_a = random.randint(0, 0x3FFFFFFF)
        s.add(Extract(31, 0, a_bv) == rand_a)
        s.add(Extract(31, 0, a_bv - b_bv) == 0x80000000)

    # ========================================================
    # 3. LOGICAL (AND, OR, XOR)
    # ========================================================
    elif scenario_name == "AND_COMPLEMENT":
        # Complementary bitmasks (e.g., 0x555... vs 0xAAA...)
        s.add(a_bv & b_bv == 0, a_bv | b_bv == 0xFFFFFFFFFFFFFFFF)

    elif scenario_name == "AND_MASK_RANDOM":
        # Extracting a random slice of bits
        rand_mask = random.choice([0x00000000FFFFFFFF, 0xFFFF0000FFFF0000, 0xFF00FF00FF00FF00])
        s.add(b_bv == rand_mask, a_bv != 0)

    elif scenario_name == "OR_COMPLEMENT":
        # Result must produce all ones (0xFFFFFFFFFFFFFFFF)
        s.add(a_bv | b_bv == 0xFFFFFFFFFFFFFFFF, a_bv != 0, b_bv != 0)

    elif scenario_name == "OR_TOGGLE_MSB_LSB":
        # Most significant bit in 'a', least significant in 'b'
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv & 1 == 1)

    elif scenario_name == "XOR_TOGGLE_MSB":
        # Inverting the sign bit via XOR
        rand_a = random.randint(0, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv == 0x8000000000000000)

    elif scenario_name == "XOR_COMPLEMENT":
        # a ^ b = 0xFFFFFFFFFFFFFFFF
        rand_a = random.randint(0, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, a_bv ^ b_bv == 0xFFFFFFFFFFFFFFFF)

    # ========================================================
    # 4. SHIFTS (SLL, SRL, SRA, SLLW, SRLW, SRAW)
    # ========================================================
    elif scenario_name == "SLL_SHIFT_VARIABLE":
        # Shift by a random number of bits in the range 1..63
        rand_shift = random.randint(1, 63)
        s.add(b_bv & 0x3F == rand_shift, a_bv > 0)

    elif scenario_name == "SLL_SIGN_BIT_SHIFT":
        # Shift a bit into the MSB position (bit 63)
        rand_shift = random.randint(1, 63)
        s.add(b_bv & 0x3F == rand_shift, (a_bv << rand_shift) & 0x8000000000000000 != 0)

    elif scenario_name == "SRL_SHIFT_VARIABLE":
        # Logical right shift of a value with set MSB
        rand_shift = random.randint(1, 63)
        s.add(Extract(63, 63, a_bv) == 1)

    elif scenario_name == "SRA_NEGATIVE_SHIFT":
        # Arithmetic shift of a negative number (sign bit propagation)
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        rand_shift = random.randint(1, 63)
        s.add(a_bv == rand_a, b_bv & 0x3F == rand_shift)

    elif scenario_name == "SRA_POSITIVE_SHIFT":
        # Arithmetic shift of a positive number (zero propagation)
        rand_a = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        rand_shift = random.randint(1, 63)
        s.add(a_bv == rand_a, b_bv & 0x3F == rand_shift)

    elif scenario_name == "SLLW_SIGN_BIT_OUT":
        # 32-bit shift (bits 4..0 of shift), where MSB (bit 31) is shifted out
        rand_shift = random.randint(1, 31)
        s.add(b_bv & 0x1F == rand_shift, Extract(31, 0, a_bv) >= 0x80000000)

    elif scenario_name == "SRLW_SIGN_BIT_LOGICAL":
        # 32-bit logical shift zero-pads bits 31..0, then sign-extends to 64-bit
        rand_shift = random.randint(1, 31)
        s.add(Extract(31, 0, a_bv) >= 0x80000000, b_bv & 0x1F == rand_shift)

    elif scenario_name == "SRAW_SIGN_EXTENSION":
        # 32-bit arithmetic shift of a negative value preserving sign-extension to 64 bits
        rand_shift = random.randint(1, 31)
        s.add(Extract(31, 0, a_bv) >= 0x80000000, b_bv & 0x1F == rand_shift)

    # ========================================================
    # 5. COMPARISONS (SLT / SLTU)
    # ========================================================
    elif scenario_name == "SLT_NEGATIVE_POSITIVE":
        # Negative < Positive (Signed: True)
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        rand_b = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv == rand_b)

    elif scenario_name == "SLT_POSITIVE_NEGATIVE":
        # Positive < Negative (Signed: False)
        rand_a = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        rand_b = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv == rand_b)

    elif scenario_name == "SLT_MIN_MAX":
        # INT64_MIN vs INT64_MAX
        s.add(a_bv == 0x8000000000000000, b_bv == 0x7FFFFFFFFFFFFFFF)

    elif scenario_name == "SLTU_NEGATIVE_AS_LARGE_UNSIGNED":
        # MSB of a is 1 (negative in signed), but treated as large unsigned, so SLTU should return 0 (False)
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        rand_b = random.randint(1, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv == rand_b) # SLTU should return 0 (False)

    elif scenario_name == "SLTU_SIGNED_VS_UNSIGNED_MISMATCH":
        # Case where signed comparison would differ from unsigned comparison
        rand_a = random.randint(0x8000000000000000, 0xFFFFFFFFFFFFFFFF)
        rand_b = random.randint(0, 0x7FFFFFFFFFFFFFFF)
        s.add(a_bv == rand_a, b_bv == rand_b)

    # ========================================================
    # Solve & Return
    # ========================================================
    if s.check() != sat:
        raise RuntimeError(
        f"Unsat stimulus constraints for scenario: {scenario_name}"
    )
    else:
        # Fallback in case of invalid constraints
        model = s.model()
        return model[a_bv].as_long(), model[b_bv].as_long(), op
