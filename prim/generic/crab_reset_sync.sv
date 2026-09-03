// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

module crab_reset_sync (
  input  logic clk_i,
  input  logic rst_ni,
  output logic sys_rst_no
);

  logic [1:0] sync_reg_r;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      sync_reg_r <= 2'b00;
    end else begin
      sync_reg_r <= {sync_reg_r[0], 1'b1};
    end
  end

  assign sys_rst_no = sync_reg_r[1];

endmodule : crab_reset_sync
