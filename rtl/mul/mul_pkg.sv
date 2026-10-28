package mul_pkg;

  typedef enum logic [2:0] {
    MUL    = 3'b000,
    MULH   = 3'b001,
    MULHSU = 3'b010,
    MULHU  = 3'b011,
    MULW   = 3'b111
  } operator_t;

endpackage
