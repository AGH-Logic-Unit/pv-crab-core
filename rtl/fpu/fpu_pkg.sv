package fpu_pkg;
  // Exception flags
  typedef struct {
    logic NV;
    logic DZ;
    logic OF;
    logic UF;
    logic NX;
  } flag_t;

  // Rounding
  typedef enum logic [2:0] {
    ROUND_NEAR_EVEN = 3'b000,
    ROUND_NEAR_MAXMAG = 3'b001,
    ROUND_MINMAG = 3'b010,
    ROUND_MIN = 3'b011,
    ROUND_MAX = 3'b100,
    ROUND_ODD = 3'b110
  } rounding_m_t;

  // Operator types
  typedef enum logic [2:0] {
    // Most Sig bit == 0
    FADD   = 3'd0,
    FSUB   = 3'd1,
    FMUL   = 3'd2,
    // Most Sig bit == 1
    FMADD  = 3'd4,
    FMSUB  = 3'd5,
    FNMSUB = 3'd6,
    FNMADD = 3'd7
  } muladd_op_t;

  typedef enum logic {NULL} misc_op_t;

  typedef enum logic [1:0] {
    FDIV,
    FSQRT
  } div_op_t;
endpackage
