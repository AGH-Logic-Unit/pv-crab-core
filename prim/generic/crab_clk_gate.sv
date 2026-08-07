// Copyright (c) 2026 AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

module crab_clk_gate (
  input  logic clk_i,
  input  logic en_i,
  input  logic test_i,
  output logic clk_o
);
  logic latch_en;

  always_latch begin
    if (!clk_i) begin
      latch_en <= en_i | test_i;
    end
  end

  assign clk_o = clk_i & latch_en;
endmodule
