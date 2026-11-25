// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

package sw_pp_divider_pkg;

  localparam logic [2:0] OpDiv   = 3'b000;
  localparam logic [2:0] OpDivU  = 3'b001;
  localparam logic [2:0] OpRem   = 3'b010;
  localparam logic [2:0] OpRemU  = 3'b011;
  localparam logic [2:0] OpDivW  = 3'b100;
  localparam logic [2:0] OpDivUW = 3'b101;
  localparam logic [2:0] OpRemW  = 3'b110;
  localparam logic [2:0] OpRemUW = 3'b111;

endpackage : sw_pp_divider_pkg
