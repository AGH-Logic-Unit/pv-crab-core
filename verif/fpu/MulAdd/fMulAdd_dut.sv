// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

module fMulAdd_dut (
  input  logic                clk_i,
  input  logic                rst_ni,
  input  rounding_m_t         rm_i,
  input  logic         [63:0] operand_a_i,
  input  logic         [63:0] operand_b_i,
  input  logic         [63:0] operand_c_i,
  input  muladd_op_t          opcode_i,
  output logic         [63:0] result_o,
  output logic                result_valid_o,
  output flag_t               wb_fflags_o,
  output exe_headers_t        wb_headers_o
);
  fmuladd_if fpu_if ();

  assign fpu_if.clk_i          = clk_i;
  assign fpu_if.rst_ni         = rst_ni;
  assign fpu_if.flush_i        = 1'b0;
  assign fpu_if.disp_valid_i   = 1'b1;
  assign fpu_if.disp_headers_i = '0;
  assign fpu_if.operand_a_i    = operand_a_i;
  assign fpu_if.operand_b_i    = operand_b_i;
  assign fpu_if.operand_c_i    = operand_c_i;
  assign fpu_if.operator_i     = opcode_i;
  assign fpu_if.rm_i           = rm_i;
  assign fpu_if.wb_ready_i     = 1'b1;

  assign result_o              = fpu_if.wb_result_o;
  assign result_valid_o        = fpu_if.wb_valid_o;
  assign wb_fflags_o           = fpu_if.wb_fflags_o;
  assign wb_headers_o          = fpu_if.wb_headers_o;

  fMulAdd dut (.fpu_i(fpu_if));
endmodule
