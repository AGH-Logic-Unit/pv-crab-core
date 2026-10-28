// =============================================================================
// Package: sw_pp_divider_pkg
// Description: Type definitions and constants for the integer divider module.
// =============================================================================

package sw_pp_divider_pkg;

  // ---------------------------------------------------------------------------
  // Execution stage header — metadata that travels with each instruction
  // through the pipeline. The divider receives it from the Dispatcher and
  // passes it unchanged to the Writeback Buffer.
  //
  // See: docs/uarch/types.md
  // ---------------------------------------------------------------------------
  typedef struct packed {
    logic [3:0] tag;        // Instruction ID for scoreboard tracking
    logic [4:0] rd_addr;    // Destination register address
    logic       rd_we;      // Register write enable
    logic       exc_valid;  // Exception flag (1 = exception occurred)
    logic [3:0] exc_code;   // RISC-V exception cause code
  } t__exe_headers;

  // ---------------------------------------------------------------------------
  // Operator codes for the 3-bit operator_i input.
  // Bit[0] selects signed (0) vs unsigned (1).
  // Bit[1] selects quotient (0) vs remainder (1).
  // Bit[2] selects 64-bit (0) vs 32-bit W-variant (1).
  // ---------------------------------------------------------------------------
  localparam logic [2:0] OP_DIV = 3'b000;  // Signed   64-bit quotient
  localparam logic [2:0] OP_DIVU = 3'b001;  // Unsigned 64-bit quotient
  localparam logic [2:0] OP_REM = 3'b010;  // Signed   64-bit remainder
  localparam logic [2:0] OP_REMU = 3'b011;  // Unsigned 64-bit remainder
  localparam logic [2:0] OP_DIVW = 3'b100;  // Signed   32-bit quotient  (sign-ext to 64)
  localparam logic [2:0] OP_DIVUW = 3'b101;  // Unsigned 32-bit quotient  (sign-ext to 64)
  localparam logic [2:0] OP_REMW = 3'b110;  // Signed   32-bit remainder (sign-ext to 64)
  localparam logic [2:0] OP_REMUW = 3'b111;  // Unsigned 32-bit remainder (sign-ext to 64)

endpackage
