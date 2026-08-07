from __future__ import annotations

import logging
import random
from collections import deque
from collections.abc import Callable
from typing import (
    Any,
)

import cocotb
from cocotb.clock import Clock
from cocotb.types import LogicArray, RisingEdge
from cocotb_coverage import crv

OP_MAP = {
    "MUL": (0, 0, 0),
    "MULH": (0, 0, 1),
    "MULHSU": (0, 1, 0),
    "MULHU": (0, 1, 1),
    "MULW": (1, 1, 1),
}


class IntegerMultiplierTestbench:
    def __init__(self, dut: Any, name: str | None = None) -> None:
        self.dut = dut
        self.name = name if name is not None else type(self).__qualname__
        self.log = logging.getLogger(self.name)
        self.log.setLevel(logging.INFO)

        self.DATA_WIDTH = 64

        self.clk_drv = Clock(self.dut.clk_i, 10, unit="ns")

    # self.input_drv = DataValidDriver()

    def start(self):
        """Starts sub-components."""
        self.dut.rst_ni.value = 1
        self.clk_drv.start()

    def stop(self) -> None:
        """Stops sub-components."""
        self.clk_drv.stop()

    async def reset(self, cycles: int = 3) -> None:
        """Reset the design under test."""
        self.dut.rst_ni.value = 0
        for _ in range(cycles):
            await RisingEdge(self.dut.clk_i)
        self.dut.rst_ni.value = 1


class PipelineOutputPacket:
    def __init__(
        self,
        result: LogicArray,
        headers: LogicArray,
        valid: bool = True,
        disp_ready: bool = True,
    ):
        self.wb_valid = valid
        self.wb_result = result
        self.wb_headers = headers
        self.disp_ready = disp_ready

    def mark_stall(self):
        self.disp_ready = False

    def __eq__(self, other: PipelineOutputPacket):
        """check equal if valid bit is set"""
        return self.disp_ready == other.disp_ready and (
            (self.wb_valid == 0 and other.wb_valid == 0)
            or (
                self.wb_result == other.wb_result
                and self.wb_headers == other.wb_headers
            )
        )

    def __repr__(self):
        return f"Output(wb_valid={self.wb_valid}, wb_result={hex(self.wb_result)}, wb_headers={hex(self.wb_headers)}, disp_ready={self.disp_ready})"


class PipelineInputPacket:
    def __init__(
        self, a: LogicArray, b: LogicArray, op: LogicArray, headers: LogicArray
    ):
        self.a = a
        self.b = b
        self.op = op
        self.headers = headers

    def calculate(self) -> PipelineOutputPacket:
        return PipelineOutputPacket(self.a * self.b, self.headers)


class MultiplierCommand(crv.Randomized):
    def __init__(self):
        super("packet").__init__()

        self.add_rand("push_valid", [0, 1])
        self.add_rand("sink_ready", [0, 1])
        self.add_rand("flush", [0, 1])

        # placeholders
        self.a = None
        self.b = None
        self.op = None
        self.headers = None
        self.packet = None

    def post_randomize(self):
        """Called automatically by .randomize() after solver finishes."""
        # Generate a true 64-bit random integer efficiently in O(1) time
        self.a = random.getrandbits(64)
        self.b = random.getrandbits(64)
        self.op = random.getrandbits(64)
        self.headers = random.getrandbits(len(cocotb.packages.defs_pkg.exe_headers_t))

        self.packet = PipelineInputPacket(self.a, self.b, self.op, self.headers)


class IntegerMultiplierModel:
    def __init__(self, name: str, DATA_WIDTH: int) -> None:
        self.name = name
        self.log = logging.getLogger(name)

        # initialize the pipeline stages
        self.stages_n = 3
        self.reset()

    def reset(self):
        self.stages = [None] * self.stages_n

    def flush(self):
        self.stages = [None] * self.stages_n

    # returns None for invalid
    def step(
        self,
        packet: PipelineInputPacket,
        push_valid: bool,
        sink_ready: bool,
        flush: bool = False,
        rst_n: bool = False,
    ) -> PipelineOutputPacket:

        if not rst_n:
            # async reset
            self.reset()
            return PipelineOutputPacket(None, None, False, False)

        if flush:
            # fush all the stages
            self.flush()
            return PipelineOutputPacket(None, None, False, True)

        last_stage_has_data = self.stages is not None

        can_shift = (not last_stage_has_data) or sink_ready

        output = PipelineOutputPacket(None, None, False)
        if last_stage_has_data:
            output = self.stages[-1].complete()

        if sink_ready:
            self.stages[-1] = None

        if can_shift:
            # shift data down the pipeline
            for i in range(self.stages_n - 1, 0, -1):
                self.stages[i] = self.stages[i - 1]

            # inject new input if valid
            self.stages[0] = packet if push_valid else None
        else:
            # pipeline stall
            # stage 0 rejects new input
            if output is not None:
                output.mark_stall()

        return output


class ScoreBoard:
    def __init__(self):
        self.expected_queue = deque()
        self.errors = 0
        self.total_checked = 0

    def add_expected(self, packet: PipelineOutputPacket):
        """Called when a transaction enters the reference model pipeline."""
        self.expected_queue.append(packet)

    def check_actual(self, actual_output: PipelineOutputPacket):
        assert (self.expected_queue) > 0, (
            f"[ERROR] DUT asserted valid ({actual_output}), "
            f"but the scoreboard expected queue is empty!"
        )

        expected_output = self.expected_queue.popleft()

        if expected_output != actual_output:
            self.errors += 1
            raise AssertionError(
                f"[MISMATCH #{self.mismatches}]\n"
                f"  Expected: {expected_output}\n"
                f"  Actual:   {actual_output}"
            )
        else:
            self.total_checked += 1
            print(f"[MATCH] Succesfully verified: {actual_output}")

    def assert_empty(self):
        """Ensures all in-flight pipeline items were fully drained and checked."""
        assert len(self.expected_queue) == 0, (
            f"[ERROR] Test ended with {len(self.expected_queue)} "
            f"unverified items left in the scoreboard queue!"
        )


async def constrained_driver(dut, num_transactions, scoreboard):
    """Generates constrained random packets and drives them into the pipeline."""
    dut.s_valid.value = 0
    dut.s_data.value = 0
    await RisingEdge(dut.clk_i)

    for i in range(num_transactions):
        # Instantiate and solve constraints for a new packet
        pkt = MultiplierCommand()
        pkt.randomize()  # Solves domains and constraints safely

        # Wait for input ready (handling pipeline stalls)
        while dut.s_ready.value == 0:
            await RisingEdge(dut.clk_i)

        # Drive transaction fields onto pipeline input pins
        dut.s_valid_i.value = 1
        dut.operand_a_i.value = pkt.a
        dut.operand_b_i.value = pkt.b
        dut.operator_i.value = pkt.op
        dut.disp_headers_i.value = pkt.headers
        await RisingEdge(dut.clk)

    # Clean up valid signal
    dut.s_valid.value = 0


async def output_monitor_coroutine(dut, scoreboard):
    """Monitors the output interface and feeds items to the scoreboard."""
    while True:
        await cocotb.triggers.RisingEdge(dut.clk_i)
        # Randomly assert downstream ready to test backpressure on the queue
        dut.wb_ready_i.value = random.choice([1, 0])

        if dut.wb_valid_o.value == 1 and dut.wb_ready_i.value == 1:
            # Capture actual outputs from pins
            actual_tx = PipelineOutputPacket(
                result=int(dut.wb_result_o.value),
                headers=int(dut.wb_headers_o.value),
                valid=bool(dut.wb_valid_o.value),
                disp_ready=bool(dut.disp_ready.value),
            )

            scoreboard.check_actual(actual_tx)


@cocotb.test()
async def run_mul_full_coverage_test(dut: Any) -> None:

    # initialise the model
    model = IntegerMultiplierModel()

    # initialize the testbench
    tb = IntegerMultiplierTestbench(dut)
    tb.start()

    scoreboard = ScoreBoard()

    # start the background monitor
    cocotb.start_soon(output_monitor_coroutine(dut, scoreboard))

    # apply hardware reset ()
    dut._log.info("Applying initial reset...")
    await tb.reset(3)

    model.reset()

    num_transactoins = 1000

    for i in range(num_transactoins):
        await RisingEdge(dut.clk_i)

        # inject random resets and flushes
        do_reset = i == 40
        do_flush = i == 70

        dut.rst_ni.value = 0 if do_reset else 1
        dut.rst_ni.flush = 1 if do_flush else 0

        if do_reset or do_flush:
            model.reset()
            scoreboard.expected_queue.clear()
            continue

        # randomize input
        push_valid = random.choice([0, 1, 1])

        if push_valid:
            pkt = MultiplierCommand()
            pkt.randomize()  # Solves domains and constraints safely

            # Drive transaction fields onto pipeline input pins
            dut.valid_i.value = 1
            dut.operand_a_i.value = pkt.a
            dut.operand_b_i.value = pkt.b
            dut.operator_i.value = pkt.op
            dut.disp_headers_i.value = pkt.headers
        else:
            dut.valid_i.value = 0

        # randomize wb ready
        dut.wb_ready_i = random.choice([0, 1, 1])

        dut_input_accepted = dut.disp_ready_o == 1
        effective_push = push_valid and dut_input_accepted

        expected_pop_output = model.step(
            packet=pkt.packet,
            push_valid=effective_push,
            sink_ready=bool(dut.wb_ready_o.value),
            rst=bool(dut.rst_i.value),
            flush=bool(dut.flush_i.value),
        )

        scoreboard.add_expected(expected_pop_output)

    dut.disp_valid_i.value = 0
    dut.wb_ready_i.value = 0

    # drain phase: let ramaining items clear out of the pipeline
    dut._log.info("Draining pipeline...")
    for _ in range(10):
        await RisingEdge(dut.clk)
        expected_pop_output = model.step(None, False, sink_ready=True)
        scoreboard.add_expected(expected_pop_output)

    scoreboard.assert_empty()
    dut._log.info("Test passed successfully! All transactions verified.")
