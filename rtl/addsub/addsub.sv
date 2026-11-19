// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

module addsub (
    input logic [31:0] a_i,
    input logic [31:0] b_i,

    input logic op_i,

    output logic [31:0] res_o
);

  always_comb begin
    res_o = op_i ? a_i + b_i : a_i - b_i;
  end

endmodule
