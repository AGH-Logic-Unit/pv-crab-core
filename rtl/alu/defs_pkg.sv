package defs_pkg;
  typedef enum logic [2:0] {
    SUM  = 'b000,
    SHFL = 'b001, // logic shift
    SHFA = 'b101, // arthmetic shift
    SLT  = 'b010,  
    SLTU = 'b011,
    XOR  = 'b100,
    OR   = 'b110,
    AND  = 'b111   
  } operator_type_t;

  typedef struct {
    logic word_mode;
    operator_type_t operator_type;
    logic complement; // subtraction and right shift
  } operator_t;
endpackage
