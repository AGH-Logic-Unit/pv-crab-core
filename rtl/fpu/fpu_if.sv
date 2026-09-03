// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

import fpu_pkg::*;
import defs_pkg::*;

interface fdiv_if #(
  // DEFAULT IS DOUBLE PRECISION
  parameter int EXP_WIDTH = 11,
  parameter int SIG_WIDTH = 53
);
  logic                                     clk_i;
  logic                                     rst_ni;
  logic                                     flush_i;
  logic                                     disp_valid_i;
  logic                                     disp_ready_o;
  exe_headers_t                             disp_headers_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_a_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_b_i;
  div_op_t                                  operator_i;
  rounding_m_t                              rm_i;
  logic                                     wb_valid_o;
  logic                                     wb_ready_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] wb_result_o;
  flag_t                                    wb_fflags_o;
  exe_headers_t                             wb_headers_o;

  modport dut(
    input clk_i,
    input rst_ni,
    input flush_i,
    input disp_valid_i,
    output disp_ready_o,
    input disp_headers_i,
    input operand_a_i,
    input operand_b_i,
    input operator_i,
    input rm_i,
    output wb_valid_o,
    input wb_ready_i,
    output wb_result_o,
    output wb_fflags_o,
    output wb_headers_o
  );

  modport tb(
    output clk_i,
    output rst_ni,
    output flush_i,
    output disp_valid_i,
    input disp_ready_o,
    output disp_headers_i,
    output operand_a_i,
    output operand_b_i,
    output operator_i,
    output rm_i,
    input wb_valid_o,
    output wb_ready_i,
    input wb_result_o,
    input wb_fflags_o,
    input wb_headers_o
  );

endinterface  // fdiv_if

interface fmuladd_if #(
  // DEFAULT IS DOUBLE PRECISION
  parameter int EXP_WIDTH = 11,
  parameter int SIG_WIDTH = 53
);
  logic                                     clk_i;
  logic                                     rst_ni;
  logic                                     flush_i;
  logic                                     disp_valid_i;
  logic                                     disp_ready_o;
  exe_headers_t                             disp_headers_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_a_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_b_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_c_i;
  muladd_op_t                               operator_i;
  rounding_m_t                              rm_i;
  logic                                     wb_valid_o;
  logic                                     wb_ready_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] wb_result_o;
  flag_t                                    wb_fflags_o;
  exe_headers_t                             wb_headers_o;

  modport dut(
    input clk_i,
    input rst_ni,
    input flush_i,
    input disp_valid_i,
    output disp_ready_o,
    input disp_headers_i,
    input operand_a_i,
    input operand_b_i,
    input operand_c_i,
    input operator_i,
    input rm_i,
    output wb_valid_o,
    input wb_ready_i,
    output wb_result_o,
    output wb_fflags_o,
    output wb_headers_o
  );

  modport tb(
    output clk_i,
    output rst_ni,
    output flush_i,
    output disp_valid_i,
    input disp_ready_o,
    output disp_headers_i,
    output operand_a_i,
    output operand_b_i,
    output operand_c_i,
    output operator_i,
    output rm_i,
    input wb_valid_o,
    output wb_ready_i,
    input wb_result_o,
    input wb_fflags_o,
    input wb_headers_o
  );

endinterface  // fmuladd_if

interface fmisc_if #(
  // DEFAULT IS DOUBLE PRECISION
  parameter int EXP_WIDTH = 11,
  parameter int SIG_WIDTH = 53
);
  logic                                     clk_i;
  logic                                     rst_ni;
  logic                                     disp_valid_i;
  logic                                     disp_ready_o;
  exe_headers_t                             disp_headers_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_a_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] operand_b_i;
  logic         [                      4:0] operator_i;
  rounding_m_t                              rm_i;
  logic                                     wb_valid_o;
  logic                                     wb_ready_i;
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] wb_result_o;
  flag_t                                    wb_fflags_o;
  exe_headers_t                             wb_headers_o;

  modport dut(
    input clk_i,
    input rst_ni,
    input disp_valid_i,
    output disp_ready_o,
    input disp_headers_i,
    input operand_a_i,
    input operand_b_i,
    input operator_i,
    input rm_i,
    output wb_valid_o,
    input wb_ready_i,
    output wb_result_o,
    output wb_fflags_o,
    output wb_headers_o
  );

  modport tb(
    output clk_i,
    output rst_ni,
    output disp_valid_i,
    input disp_ready_o,
    output disp_headers_i,
    output operand_a_i,
    output operand_b_i,
    output operator_i,
    output rm_i,
    input wb_valid_o,
    output wb_ready_i,
    input wb_result_o,
    input wb_fflags_o,
    input wb_headers_o
  );

endinterface  // fmisc_if
