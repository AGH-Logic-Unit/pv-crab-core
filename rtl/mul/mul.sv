module mul (
  input logic clk_i,
  input logic rst_ni,

  input logic flush_i,
  input logic disp_valid_i,
  input logic wb_ready_i,

  input logic [63:0] operand_a_i,
  input logic [63:0] operand_b_i,
  input logic [ 2:0] operator_i,


  output logic disp_ready_o,
  output logic wb_valid_o,

  output logic [63:0] wb_result_o
);
  import mul_pkg::*;

  logic [64:0] operand_a_reg, operand_b_reg;
  logic [2:0] operator_reg, operator_reg2;
  logic [32:0] operand_a_h, operand_b_h;
  logic [31:0] operand_a_l, operand_b_l;
  logic [32:0] operand_a_h_reg, operand_b_h_reg;
  logic [31:0] operand_a_l_reg, operand_b_l_reg;
  logic [64:0] mul_stg_2, mul_stg_3;
  logic [63:0] mul_stg_4;
  logic [64:0] mul_stg_2_reg, mul_stg_3_reg;
  logic [63:0] mul_stg_4_reg;
  logic [65:0] mul_stg_1, mul_stg_1_reg;
  logic [129:0] result;
  logic sign_a, sign_b;


  //EX1

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      operand_a_reg <= '0;
      operand_b_reg <= '0;
      operator_reg  <= '0;
      sign_a        <= 0;
      sign_b        <= 0;

    end else begin
      operator_reg <= operator_i;
      case (operator_i)
        MULH: begin
          operand_a_reg <= {operand_a_i[63], operand_a_i};
          operand_b_reg <= {operand_b_i[63], operand_b_i};
        end
        MULHSU: begin
          operand_a_reg <= {operand_a_i[63], operand_a_i};
          operand_b_reg <= {1'b0, operand_b_i};
        end
        default: begin
          operand_a_reg <= {1'b0, operand_a_i};
          operand_b_reg <= {1'b0, operand_b_i};
        end
      endcase
    end
  end


  always_comb begin
    operand_a_l = operand_a_reg[31:0];
    operand_a_h = operand_a_reg[64:32];

    operand_b_l = operand_b_reg[31:0];
    operand_b_h = operand_b_reg[64:32];
  end

  //EX2

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      operand_a_h_reg <= '0;
      operand_a_l_reg <= '0;
      operand_b_h_reg <= '0;
      operand_b_l_reg <= '0;
      operator_reg2   <= '0;
    end else begin
      operand_a_h_reg <= operand_a_h;
      operand_a_l_reg <= operand_a_l;
      operand_b_h_reg <= operand_b_h;
      operand_b_l_reg <= operand_b_l;
      operator_reg2   <= operator_reg;
    end
  end

  always_comb begin
    mul_stg_1 = $signed(operand_a_h_reg) * $signed(operand_b_h_reg);
    mul_stg_2 = $signed(operand_a_h_reg) * operand_b_l_reg;
    mul_stg_3 = operand_a_l_reg * $signed(operand_b_h_reg);
    mul_stg_4 = operand_a_l_reg * operand_b_l_reg;

    mul_stg_1 = mul_stg_1 << 64;
  end

  //EX3

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      mul_stg_1_reg <= '0;
      mul_stg_2_reg <= '0;
      mul_stg_3_reg <= '0;
      mul_stg_4_reg <= '0;

    end else begin
      mul_stg_1_reg <= mul_stg_1;
      mul_stg_2_reg <= mul_stg_2;
      mul_stg_3_reg <= mul_stg_3;
      mul_stg_4_reg <= mul_stg_4;

    end
  end

  always_comb begin
    result = mul_stg_2_reg + mul_stg_3_reg;
    result = result << 32;
    result = result + mul_stg_1_reg + mul_stg_4_reg;

    unique case (operator_reg2)
      MUL: begin
        wb_result_o = result[63:0];
      end
      MULW: begin
        wb_result_o[31:0]  = result[31:0];
        wb_result_o[63:32] = {32{result[32]}};
      end
      MULHU: begin
        wb_result_o = result[127:64];
      end
      MULH: begin
        wb_result_o = result[127:64];
      end
      MULHSU: begin
        wb_result_o = result[127:64];
      end
    endcase
  end

endmodule
