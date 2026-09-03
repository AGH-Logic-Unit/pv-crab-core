// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

`include "mulAddRecFN.v"
`include "addRecFN.v"
`include "mulRecFN.v"

import fpu_pkg::*;
import defs_pkg::*;

module fMulAdd #(
  parameter int EXP_WIDTH  = 11,
  parameter int SIG_WIDTH  = 53,
  parameter int MULADD_LAT = 3
) (
  fmuladd_if.dut fpu_i
);
  // Output selector
  logic  [  (EXP_WIDTH+SIG_WIDTH):0] add_out_rec;
  logic  [  (EXP_WIDTH+SIG_WIDTH):0] mul_out_rec;
  logic  [  (EXP_WIDTH+SIG_WIDTH):0] madd_out_rec;

  flag_t                             add_exc;
  flag_t                             mul_exc;
  flag_t                             madd_exc;

  logic  [  (EXP_WIDTH+SIG_WIDTH):0] out_rec_sel;
  logic  [(EXP_WIDTH+SIG_WIDTH-1):0] out_ieee_sel;
  flag_t                             exc_sel;

  /////////////////////
  // BERKLEY MODULES //
  /////////////////////

  logic [(EXP_WIDTH+SIG_WIDTH):0] a_rec, b_rec, c_rec;

  // IEEE -> recFN
  fNToRecFN #(
    .expWidth(11),
    .sigWidth(53)
  ) m_a (
    .in (fpu_i.operand_a_i),
    .out(a_rec)
  );

  fNToRecFN #(
    .expWidth(11),
    .sigWidth(53)
  ) m_b (
    .in (fpu_i.operand_b_i),
    .out(b_rec)
  );

  fNToRecFN #(
    .expWidth(EXP_WIDTH),
    .sigWidth(SIG_WIDTH)
  ) m_c (
    .in (fpu_i.operand_c_i),
    .out(c_rec)
  );

  mulAddRecFN #(
    .expWidth(11),
    .sigWidth(53)
  ) m_muladd (
    .control       (`flControl_tininessAfterRounding),
    .op            (fpu_i.operator_i[1:0]),
    .a             (a_rec),
    .b             (b_rec),
    .c             (c_rec),
    .roundingMode  (fpu_i.rm_i),
    .out           (madd_out_rec),
    .exceptionFlags(madd_exc)
  );  //mulAddRecFN

  addRecFN #(
    .expWidth(11),
    .sigWidth(53)
  ) m_add (
    .control       (`flControl_tininessAfterRounding),
    .subOp         (fpu_i.operator_i == FSUB),
    .a             (a_rec),
    .b             (b_rec),
    .roundingMode  (fpu_i.rm_i),
    .out           (add_out_rec),
    .exceptionFlags(add_exc)
  );  //addRecFN

  mulRecFN #(
    .expWidth(11),
    .sigWidth(53)
  ) m_mul (
    .control       (`flControl_tininessAfterRounding),
    .a             (a_rec),
    .b             (b_rec),
    .roundingMode  (fpu_i.rm_i),
    .out           (mul_out_rec),
    .exceptionFlags(mul_exc)
  );  //mulRecFN

  always_comb begin
    out_rec_sel = '0;
    exc_sel     = '0;

    unique case (fpu_i.operator_i)
      FADD, FSUB: begin
        out_rec_sel = add_out_rec;
        exc_sel     = add_exc;
      end
      FMUL: begin
        out_rec_sel = mul_out_rec;
        exc_sel     = mul_exc;
      end
      FMADD, FMSUB, FNMSUB, FNMADD: begin
        out_rec_sel = madd_out_rec;
        exc_sel     = madd_exc;
      end
      default: begin
        out_rec_sel = '0;
        exc_sel     = '0;
      end
    endcase
  end

  // recFN -> IEEE
  recFNToFN #(
    .expWidth(EXP_WIDTH),
    .sigWidth(SIG_WIDTH)
  ) m_out (
    .in (out_rec_sel),
    .out(out_ieee_sel)
  );

  //////////////
  // PIPELINE //
  //////////////

  logic                                     valid_pipe  [MULADD_LAT];
  logic         [(EXP_WIDTH+SIG_WIDTH-1):0] result_pipe [MULADD_LAT];
  flag_t                                    flag_pipe   [MULADD_LAT];
  exe_headers_t                             headers_pipe[MULADD_LAT];

  // Backpressure
  assign fpu_i.disp_ready_o = fpu_i.wb_ready_i;

  // Pipeline process
  integer i;
  always_ff @(posedge fpu_i.clk_i or negedge fpu_i.rst_ni) begin
    if (!fpu_i.rst_ni) begin
      for (i = 0; i <= MULADD_LAT - 1; i++) begin
        valid_pipe[i]   <= 1'b0;
        result_pipe[i]  <= '0;
        flag_pipe[i]    <= '0;
        headers_pipe[i] <= '0;
      end
    end else if (fpu_i.wb_ready_i) begin
      for (i = MULADD_LAT - 1; i > 0; i--) begin
        valid_pipe[i]   <= valid_pipe[i-1];
        result_pipe[i]  <= result_pipe[i-1];
        flag_pipe[i]    <= flag_pipe[i-1];
        headers_pipe[i] <= headers_pipe[i-1];
      end

      valid_pipe[0]   <= fpu_i.disp_valid_i;
      result_pipe[0]  <= out_ieee_sel;
      flag_pipe[0]    <= exc_sel;
      headers_pipe[0] <= fpu_i.disp_headers_i;
    end
  end

  // Outputs
  assign fpu_i.wb_valid_o   = valid_pipe[MULADD_LAT-1];
  assign fpu_i.wb_result_o  = result_pipe[MULADD_LAT-1];
  assign fpu_i.wb_fflags_o  = flag_pipe[MULADD_LAT-1];
  assign fpu_i.wb_headers_o = headers_pipe[MULADD_LAT-1];
endmodule  // fMulAdd
