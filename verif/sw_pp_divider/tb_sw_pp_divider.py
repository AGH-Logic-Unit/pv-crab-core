# =============================================================================
# Testbench: tb_sw_pp_divider
# Verifies the sw_pp_divider module against RISC-V M-extension specification.
#
# Current scope (Step 3):
#   - Edge case tests only (div-by-zero, signed overflow)
#   - Header pass-through verification
#   - Latency checks
#
# Full random tests will be added in Step 7 after the SRT core is implemented.
# =============================================================================

import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge
import logging

# =============================================================================
# Constants — must match sw_pp_divider_pkg.sv
# =============================================================================
MAX_U64 = (1 << 64) - 1
MAX_U32 = (1 << 32) - 1

# Operator codes (bit encoding: [2]=word, [1]=remainder, [0]=unsigned)
OP_DIV   = 0b000   # Signed   64-bit quotient
OP_DIVU  = 0b001   # Unsigned 64-bit quotient
OP_REM   = 0b010   # Signed   64-bit remainder
OP_REMU  = 0b011   # Unsigned 64-bit remainder
OP_DIVW  = 0b100   # Signed   32-bit quotient  (sign-ext to 64)
OP_DIVUW = 0b101   # Unsigned 32-bit quotient  (sign-ext to 64)
OP_REMW  = 0b110   # Signed   32-bit remainder (sign-ext to 64)
OP_REMUW = 0b111   # Unsigned 32-bit remainder (sign-ext to 64)

OP_NAMES = {
    OP_DIV: "DIV",     OP_DIVU: "DIVU",
    OP_REM: "REM",     OP_REMU: "REMU",
    OP_DIVW: "DIVW",   OP_DIVUW: "DIVUW",
    OP_REMW: "REMW",   OP_REMUW: "REMUW",
}



def to_signed(val, bits):
    """Interpret an unsigned integer as signed (two's complement)."""
    if val >= (1 << (bits - 1)):
        return val - (1 << bits)
    return val


def to_unsigned(val, bits):
    """Convert a signed Python integer to unsigned N-bit representation."""
    return val & ((1 << bits) - 1)


def sign_extend_32_to_64(val):
    """Sign-extend a 32-bit unsigned value to 64 bits."""
    val32 = val & 0xFFFFFFFF
    if val32 >= 0x80000000:
        return val32 | 0xFFFFFFFF00000000
    return val32


def model_riscv_div(a_u64, b_u64, op):
    """
    Golden reference model for RISC-V M-extension division.

    Args:
        a_u64: Dividend as 64-bit unsigned Python int (raw register value)
        b_u64: Divisor  as 64-bit unsigned Python int (raw register value)
        op:    3-bit operator code (see OP_* constants above)

    Returns:
        Expected result as 64-bit unsigned Python int.
    """
    is_unsigned = op & 1
    is_rem      = (op >> 1) & 1
    is_word     = (op >> 2) & 1

    # --- Step 1: Extract operand bits ---
    if is_word:
        a_raw = a_u64 & MAX_U32
        b_raw = b_u64 & MAX_U32
        xlen = 32
    else:
        a_raw = a_u64 & MAX_U64
        b_raw = b_u64 & MAX_U64
        xlen = 64

    # --- Step 2: Interpret as signed or unsigned ---
    if is_unsigned:
        a_val = a_raw
        b_val = b_raw
    else:
        a_val = to_signed(a_raw, xlen)
        b_val = to_signed(b_raw, xlen)

    min_int  = -(1 << (xlen - 1))         # e.g. -2^63 for 64-bit
    all_ones = (1 << xlen) - 1             # e.g. 0xFFFF...F for unsigned div-by-0

    # --- Step 3: Handle edge cases (per RISC-V spec) ---
    if b_val == 0:
        # Division by zero
        if is_rem:
            result = a_val                 # REM by zero → dividend
        else:
            result = all_ones if is_unsigned else -1  # DIV by zero → all ones

    elif (not is_unsigned) and (a_val == min_int) and (b_val == -1):
        # Signed overflow: MIN_INT / -1
        if is_rem:
            result = 0                     # REM → 0
        else:
            result = min_int               # DIV → MIN_INT (wraps)

    else:
        # --- Step 4: Normal division (truncating toward zero) ---
        if is_unsigned:
            q = a_val // b_val
        else:
            # Python's // floors toward -∞, but RISC-V truncates toward 0.
            # So we do: |a| // |b|, then negate if signs differ.
            q = abs(a_val) // abs(b_val)
            if (a_val < 0) != (b_val < 0):
                q = -q

        r = a_val - q * b_val
        result = r if is_rem else q

    # --- Step 5: Convert back to unsigned 64-bit ---
    if is_word:
        return sign_extend_32_to_64(to_unsigned(result, 32))
    else:
        return to_unsigned(result, 64)


# =============================================================================
# Header Packing/Unpacking
#
# t__exe_headers is a 15-bit packed struct:
#   {tag[3:0], rd_addr[4:0], rd_we[0], exc_valid[0], exc_code[3:0]}
#    bits 14:11   bits 10:6     bit 5      bit 4       bits 3:0
# =============================================================================

def pack_headers(tag=0, rd_addr=0, rd_we=0, exc_valid=0, exc_code=0):
    """Pack individual header fields into a single 15-bit value."""
    return ((tag & 0xF) << 11) | ((rd_addr & 0x1F) << 6) | \
           ((rd_we & 1) << 5)  | ((exc_valid & 1) << 4)  | (exc_code & 0xF)


# =============================================================================
# DUT Control Helpers
# =============================================================================

async def reset_dut(dut):
    """Apply asynchronous active-low reset for 2 clock cycles, then release."""
    dut.rst_ni.value        = 0
    dut.disp_valid_i.value  = 0
    dut.wb_ready_i.value    = 0
    dut.operand_a_i.value   = 0
    dut.operand_b_i.value   = 0
    dut.operator_i.value    = 0
    dut.disp_headers_i.value = 0
    await RisingEdge(dut.clk_i)
    await RisingEdge(dut.clk_i)
    dut.rst_ni.value = 1       # Release reset
    await RisingEdge(dut.clk_i)


async def run_division(dut, a, b, op, headers=0):
    """
    Execute one division operation through the full handshake protocol.

    1. Wait for disp_ready_o (divider is idle)
    2. Assert disp_valid_i with operands → handshake on next rising edge
    3. Wait for wb_valid_o (result ready)
    4. Read result, assert wb_ready_i → writeback handshake

    Returns: (result_u64, output_headers, latency_in_cycles)
    """
    # 1. Wait until divider is ready to accept a new instruction
    timeout = 0
    while not dut.disp_ready_o.value:
        await RisingEdge(dut.clk_i)
        timeout += 1
        if timeout > 200:
            raise TimeoutError("DUT never became ready (disp_ready_o stuck low)")

    # 2. Present inputs on the bus
    dut.operand_a_i.value    = a & MAX_U64
    dut.operand_b_i.value    = b & MAX_U64
    dut.operator_i.value     = op
    dut.disp_headers_i.value = headers
    dut.disp_valid_i.value   = 1

    # 3. Handshake completes on this rising edge (valid & ready both high)
    await RisingEdge(dut.clk_i)
    dut.disp_valid_i.value = 0   # De-assert after one cycle

    # 4. Wait for result
    latency = 0
    dut.wb_ready_i.value = 1     # We are always ready to accept the result
    while True:
        await RisingEdge(dut.clk_i)
        latency += 1
        if dut.wb_valid_o.value:
            break
        if latency > 100:
            raise TimeoutError(
                f"DUT did not produce result within 100 cycles "
                f"(op={OP_NAMES.get(op, '?')}, a=0x{a:016x}, b=0x{b:016x})"
            )

    # 5. Read outputs
    result      = int(dut.wb_result_o.value)
    out_headers = int(dut.wb_headers_o.value)

    # 6. Writeback handshake completes (wb_valid & wb_ready both high)
    #    Wait one more cycle for FSM to return to IDLE
    await RisingEdge(dut.clk_i)
    dut.wb_ready_i.value = 0

    return result, out_headers, latency


# =============================================================================
# Test: Edge Cases (Division by Zero + Signed Overflow)
#
# These tests should PASS right now, even without the SRT division core,
# because edge cases are handled by the 1-cycle bypass logic in the FSM.
# =============================================================================

@cocotb.test()
async def test_edge_cases(dut):
    """Verify all RISC-V edge cases: division by zero and signed overflow."""

    log = logging.getLogger("cocotb.test")
    log.info("=" * 60)
    log.info("  Starting Edge Case Verification")
    log.info("=" * 60)

    # Start the 100 MHz clock
    clock = Clock(dut.clk_i, 10, units="ns")
    cocotb.start_soon(clock.start())
    await reset_dut(dut)

    matches    = 0
    mismatches = 0
    total      = 0

    # ------------------------------------------------------------------
    # Part 1: Division by Zero — all 8 operators × multiple dividends
    # ------------------------------------------------------------------
    log.info("--- Part 1: Division by Zero ---")

    # Test with a variety of dividend values (edge-case-heavy)
    test_dividends = [
        0x0000000000000000,   # zero
        0x0000000000000001,   # one
        0x000000000000002A,   # small positive (42)
        0x7FFFFFFFFFFFFFFF,   # MAX_INT64
        0x8000000000000000,   # MIN_INT64 (most negative signed)
        0xFFFFFFFFFFFFFFFF,   # -1 signed / MAX unsigned
        0x00000000DEADBEEF,   # arbitrary with interesting lower 32
        0xFFFFFFFF80000000,   # MIN_INT32 sign-extended
    ]

    for op in range(8):
        for dividend in test_dividends:
            total += 1

            # Use unique headers for each test to verify pass-through
            headers_in = pack_headers(
                tag      = op & 0xF,
                rd_addr  = total & 0x1F,
                rd_we    = 1,
                exc_valid = 0,
                exc_code = 0
            )

            result, headers_out, latency = await run_division(
                dut, a=dividend, b=0, op=op, headers=headers_in
            )

            expected = model_riscv_div(dividend, 0, op)

            ok = (result == expected) and (headers_out == headers_in)
            if ok:
                matches += 1
            else:
                mismatches += 1
                log.error(
                    f"FAIL div-by-0 | {OP_NAMES[op]:6s} | "
                    f"a=0x{dividend:016x} | "
                    f"got=0x{result:016x} exp=0x{expected:016x} | "
                    f"hdr_ok={headers_out == headers_in}"
                )

            # Edge cases must complete in 1 cycle (bypass path)
            if latency != 1:
                log.warning(
                    f"  Latency={latency} cycles (expected 1) for "
                    f"{OP_NAMES[op]} div-by-zero"
                )

    log.info(f"  Div-by-zero: {matches}/{total} passed")

    # ------------------------------------------------------------------
    # Part 2: Signed Overflow (MIN_INT / -1)
    # Only applies to signed operations: DIV, REM, DIVW, REMW
    # ------------------------------------------------------------------
    log.info("--- Part 2: Signed Overflow ---")

    overflow_tests = [
        # (operator, dividend, divisor, description)
        # 64-bit signed overflow: -2^63 / -1
        (OP_DIV,  0x8000000000000000, 0xFFFFFFFFFFFFFFFF, "DIV  MIN64/-1"),
        (OP_REM,  0x8000000000000000, 0xFFFFFFFFFFFFFFFF, "REM  MIN64/-1"),
        # 32-bit signed overflow: -2^31 / -1 (lower 32 bits matter)
        (OP_DIVW, 0xFFFFFFFF80000000, 0xFFFFFFFFFFFFFFFF, "DIVW MIN32/-1 (sign-ext)"),
        (OP_REMW, 0xFFFFFFFF80000000, 0xFFFFFFFFFFFFFFFF, "REMW MIN32/-1 (sign-ext)"),
        # Same but with garbage in upper 32 bits (W-variant ignores them)
        (OP_DIVW, 0xDEADBEEF80000000, 0x1234567800000000 | 0xFFFFFFFF,
         "DIVW MIN32/-1 (garbage upper)"),
        (OP_REMW, 0x0000000080000000, 0x00000000FFFFFFFF,
         "REMW MIN32/-1 (zero upper)"),
    ]

    for op, dividend, divisor, desc in overflow_tests:
        total += 1

        headers_in = pack_headers(tag=0xB, rd_addr=15, rd_we=1)

        result, headers_out, latency = await run_division(
            dut, a=dividend, b=divisor, op=op, headers=headers_in
        )

        expected = model_riscv_div(dividend, divisor, op)

        ok = (result == expected) and (headers_out == headers_in)
        if ok:
            matches += 1
            log.info(f"  PASS: {desc} → 0x{result:016x}")
        else:
            mismatches += 1
            log.error(
                f"  FAIL: {desc} | "
                f"got=0x{result:016x} exp=0x{expected:016x}"
            )

    # ------------------------------------------------------------------
    # Summary
    # ------------------------------------------------------------------
    log.info("=" * 60)
    log.info(f"  Edge Case Verification Complete")
    log.info(f"  Total:      {total}")
    log.info(f"  Matches:    {matches}")
    log.info(f"  Mismatches: {mismatches}")
    log.info("=" * 60)

    assert mismatches == 0, f"Found {mismatches} mismatches in edge case tests!"
    log.info("All edge case tests PASSED!")


# =============================================================================
# Test: Normal Division (restoring division core)
# Covers all 8 operators, latency spec compliance, and word garbage-upper-bits.
# =============================================================================

MAX_LATENCY_NORMAL = 16  # spec: 16 cycles for normal division
MAX_LATENCY_EDGE   = 2   # spec: 1 cycle bypass + 1 cycle to see valid

@cocotb.test()
async def test_normal_division(dut):
    """Verify all 8 RISC-V M-extension operators using the restoring division core."""
    import random
    random.seed(42)

    log = logging.getLogger("cocotb.test")
    log.info("=" * 60)
    log.info("  Starting Normal Division Verification")
    log.info("=" * 60)

    clock = Clock(dut.clk_i, 10, unit="ns")
    cocotb.start_soon(clock.start())
    await reset_dut(dut)

    matches    = 0
    mismatches = 0
    lat_errors = 0

    def check(result, expected, latency, desc, max_lat=MAX_LATENCY_NORMAL):
        nonlocal matches, mismatches, lat_errors
        val_ok = (result == expected)
        lat_ok = (latency <= max_lat)
        if val_ok and lat_ok:
            matches += 1
            log.info(f"  PASS [{latency:2d}cyc] {desc} → 0x{result:016x}")
        else:
            if not val_ok:
                mismatches += 1
                log.error(
                    f"  FAIL [{latency:2d}cyc] {desc} | "
                    f"got=0x{result:016x} exp=0x{expected:016x}"
                )
            if not lat_ok:
                lat_errors += 1
                log.error(f"  LATENCY VIOLATION [{latency}cyc > {max_lat}] {desc}")

    async def run(a, b, op, desc, max_lat=MAX_LATENCY_NORMAL):
        a_u64 = a & MAX_U64
        b_u64 = b & MAX_U64
        exp    = model_riscv_div(a_u64, b_u64, op)
        res, _, lat = await run_division(dut, a_u64, b_u64, op)
        check(res, exp, lat, desc, max_lat)

    # ------------------------------------------------------------------
    # Signed 64-bit (DIV / REM)
    # ------------------------------------------------------------------
    log.info("  --- DIV / REM ---")
    await run(100,           3,   OP_DIV,  "100 / 3 = 33")
    await run(100,           3,   OP_REM,  "100 % 3 = 1")
    await run(1,             1,   OP_DIV,  "1 / 1 = 1")
    await run(0,             5,   OP_DIV,  "0 / 5 = 0")
    await run(0,             5,   OP_REM,  "0 % 5 = 0")
    await run(-7 & MAX_U64,  2,   OP_DIV,  "-7 / 2 = -3")
    await run(-7 & MAX_U64,  2,   OP_REM,  "-7 % 2 = -1")
    await run(7,            -2 & MAX_U64, OP_DIV, "7 / -2 = -3")
    await run(7,            -2 & MAX_U64, OP_REM, "7 % -2 = 1")
    await run(-7 & MAX_U64, -2 & MAX_U64, OP_DIV, "-7 / -2 = 3")
    await run(-7 & MAX_U64, -2 & MAX_U64, OP_REM, "-7 % -2 = -1")
    await run(1000000,       7,   OP_DIV,  "1000000 / 7")
    await run(0x7FFFFFFFFFFFFFFF, 2, OP_DIV, "MAX_S64 / 2")

    # ------------------------------------------------------------------
    # Unsigned 64-bit (DIVU / REMU)
    # ------------------------------------------------------------------
    log.info("  --- DIVU / REMU ---")
    await run(7,                  2,   OP_DIVU, "7u / 2u = 3")
    await run(7,                  2,   OP_REMU, "7u % 2u = 1")
    await run(0xDEAD,          0xFF,   OP_DIVU, "0xDEAD / 0xFF")
    await run(0xDEAD,          0xFF,   OP_REMU, "0xDEAD % 0xFF")
    await run(0x7FFFFFFFFFFFFFFF, 2,   OP_DIVU, "MAX_U63 / 2")
    await run(0xFFFFFFFFFFFFFFFF, 3,   OP_DIVU, "MAX_U64 / 3")
    await run(0xFFFFFFFFFFFFFFFF, 3,   OP_REMU, "MAX_U64 % 3")
    # dividend < divisor → quotient = 0, remainder = dividend
    await run(3,                  7,   OP_DIVU, "3u / 7u = 0")
    await run(3,                  7,   OP_REMU, "3u % 7u = 3")

    # ------------------------------------------------------------------
    # Signed 32-bit word (DIVW / REMW) — upper 32 bits must be ignored
    # ------------------------------------------------------------------
    log.info("  --- DIVW / REMW (with garbage upper bits) ---")
    await run(15,                4,   OP_DIVW, "15w / 4w = 3")
    await run(15,                4,   OP_REMW, "15w % 4w = 3")
    await run(-7 & MAX_U64,      2,   OP_DIVW, "-7w / 2w = -3")
    await run(-7 & MAX_U64,      2,   OP_REMW, "-7w % 2w = -1")
    # Garbage upper 32 bits — must be ignored, same result as above
    await run(0xDEADBEEF_FFFFFFF9, 2, OP_DIVW, "-7w / 2w (garbage upper)")
    await run(0xDEADBEEF_FFFFFFF9, 2, OP_REMW, "-7w % 2w (garbage upper)")
    await run(0x12345678_0000000F, 4, OP_DIVW, "15w / 4w (garbage upper)")

    # ------------------------------------------------------------------
    # Unsigned 32-bit word (DIVUW / REMUW) — upper 32 bits must be ignored
    # ------------------------------------------------------------------
    log.info("  --- DIVUW / REMUW (with garbage upper bits) ---")
    await run(15,                4,   OP_DIVUW, "15uw / 4uw = 3")
    await run(15,                4,   OP_REMUW, "15uw % 4uw = 3")
    await run(0xFFFFFFFF,        3,   OP_DIVUW, "MAX_U32 / 3")
    await run(0xFFFFFFFF,        3,   OP_REMUW, "MAX_U32 % 3")
    # Garbage upper bits
    await run(0xDEADBEEF_0000000F, 4, OP_DIVUW, "15uw / 4uw (garbage upper)")
    await run(0xDEADBEEF_0000000F, 4, OP_REMUW, "15uw % 4uw (garbage upper)")

    # ------------------------------------------------------------------
    # Random cases — all 8 operators, including word variants
    # ------------------------------------------------------------------
    log.info("  --- Random cases (all operators) ---")
    all_ops = [OP_DIV, OP_DIVU, OP_REM, OP_REMU,
               OP_DIVW, OP_DIVUW, OP_REMW, OP_REMUW]
    for i in range(80):
        a  = random.randint(1, MAX_U64)
        b  = random.randint(1, MAX_U64)
        op = all_ops[i % len(all_ops)]
        a_u64 = a & MAX_U64
        b_u64 = b & MAX_U64
        exp = model_riscv_div(a_u64, b_u64, op)
        res, _, lat = await run_division(dut, a_u64, b_u64, op)
        check(res, exp, lat, f"rand {i}: {OP_NAMES[op]}")

    # ------------------------------------------------------------------
    # Summary
    # ------------------------------------------------------------------
    log.info("=" * 60)
    log.info(f"  Normal Division Complete")
    log.info(f"  Matches:           {matches}")
    log.info(f"  Value mismatches:  {mismatches}")
    log.info(f"  Latency errors:    {lat_errors}")
    log.info("=" * 60)

    assert mismatches == 0, f"Found {mismatches} value mismatches!"
    assert lat_errors  == 0, f"Found {lat_errors} latency violations (>{MAX_LATENCY_NORMAL} cycles)!"
    log.info("All normal division tests PASSED!")
