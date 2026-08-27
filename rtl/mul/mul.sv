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

  logic [63:0] operand_a_reg, operand_b_reg;
  logic [2:0] operator_reg;
  logic [31:0] operand_a_h, operand_b_h, operand_a_l, operand_b_l;
  logic [63:0] mul_stg_4, mul_stg_2, mul_stg_3;
  logic [127:0] mul_stg_1, mul_stg_1_reg, mul_stg_1_reg2, mul_stg_4_reg2;
  logic [63:0] mul_stg_4_reg, mul_stg_2_reg, mul_stg_3_reg;
  logic [127:0] result_1, result_1_reg, result_2;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      operand_a_reg <= '0;
      operand_b_reg <= '0;
      operator_reg  <= '0;
    end else begin
      operand_a_reg <= operand_a_i;
      operand_b_reg <= operand_b_i;
      operator_reg  <= operator_i;
    end
  end


  always_comb begin
    {operand_a_h, operand_a_l} = operand_a_reg;
    {operand_b_h, operand_b_l} = operand_b_reg;

    mul_stg_1                  = operand_a_h * operand_b_h;
    mul_stg_2                  = operand_a_h * operand_b_l;
    mul_stg_3                  = operand_a_l * operand_b_h;
    mul_stg_4                  = operand_a_l * operand_b_l;

    mul_stg_1                  = mul_stg_1 << 64;
  end

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
    result_1 = mul_stg_2_reg + mul_stg_3_reg;
  end

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      result_1_reg   <= '0;
      mul_stg_1_reg2 <= '0;
      mul_stg_4_reg2 <= '0;

    end else begin
      result_1_reg   <= result_1;
      mul_stg_1_reg2 <= mul_stg_1_reg;
      mul_stg_4_reg2 <= mul_stg_4_reg;

    end
  end

  always_comb begin
    result_1_reg = result_1_reg << 32;
    result_2     = result_1_reg + mul_stg_1_reg2 + mul_stg_4_reg2;
    wb_result_o  = result_2[127:64];
  end

endmodule
