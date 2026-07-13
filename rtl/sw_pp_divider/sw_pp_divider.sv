module sw_pp_divider
  import sw_pp_divider_pkg::*;
(
    input logic clk_i,
    input logic rst_ni,

    input logic disp_valid_i,
    output logic disp_ready_o,
    input t__exe_headers disp_headers_i,
    input logic [63:0] operand_a_i,
    input logic [63:0] operand_b_i,
    input logic [2:0] operator_i,

    output logic wb_valid_o,
    input logic wb_ready_i,
    output t__exe_headers wb_headers_o,
    output logic [63:0] wb_result_o
);

  //FSM 

  typedef enum logic [1:0] {
    S_IDLE,
    S_COMPUTE,
    S_DONE
  } state_e;
  state_e state_q;

  //cycle counter (max 16)
  logic [3:0] cycle_cnt_q;

  logic [63:0] operand_a_q;
  logic [63:0] operand_b_q;
  logic [2:0] operator_q;
  t__exe_headers headers_q;

  logic [63:0] result_q;


  assign disp_ready_o = (state_q == S_IDLE);
  assign wb_valid_o   = (state_q == S_DONE);
  assign wb_headers_o = headers_q;
  assign wb_result_o  = result_q;



  // ============================================
  //  Edge-cases i Preprocessing
  // ============================================
  logic edge_div_by_zero;
  logic edge_signed_ovf;

  logic is_unsigned, is_rem, is_word;
  logic [63:0] op_a_prep, op_b_prep;
  logic [63:0] bypass_result;
  logic [63:0] bypass_result_w;

  always_comb begin
    is_unsigned = operator_i[0];
    is_rem      = operator_i[1];
    is_word     = operator_i[2];

    if (is_word) begin
      if (is_unsigned) begin
        op_a_prep = {32'b0, operand_a_i[31:0]};  // zero-extend dla unsigned
        op_b_prep = {32'b0, operand_b_i[31:0]};
      end else begin
        op_a_prep = {{32{operand_a_i[31]}}, operand_a_i[31:0]};  // sign-extend dla signed
        op_b_prep = {{32{operand_b_i[31]}}, operand_b_i[31:0]};
      end
    end else begin
      op_a_prep = operand_a_i;
      op_b_prep = operand_b_i;
    end
    //EDGE-CASES
    edge_div_by_zero = (op_b_prep == 64'd0);

    if (is_word) begin
      edge_signed_ovf = ~is_unsigned & (op_a_prep == 64'hFFFF_FFFF_8000_0000) & (op_b_prep == {64{1'b1}});
    end else begin
      edge_signed_ovf = ~is_unsigned & (op_a_prep == 64'h8000_0000_0000_0000) & (op_b_prep == {64{1'b1}});
    end
    bypass_result = 64'd0;

    if (edge_div_by_zero) begin
      bypass_result = is_rem ? op_a_prep : {64{1'b1}};
    end else if (edge_signed_ovf) begin
      bypass_result = is_rem ? 64'd0 : op_a_prep;
    end

    if (is_word) begin
      bypass_result_w = {{32{bypass_result[31]}}, bypass_result[31:0]};
    end else begin
      bypass_result_w = bypass_result;
    end
  end


  always_ff @(posedge clk_i or negedge rst_ni) begin

    if (!rst_ni) begin
      state_q <= S_IDLE;
      cycle_cnt_q <= 4'd0;
      result_q <= 64'd0;
      headers_q <= '0;
      operand_a_q <= 64'd0;
      operand_b_q <= 64'd0;
      operator_q <= 3'd0;
    end else begin
      case (state_q)
        S_IDLE: begin
          if (disp_valid_i) begin
            headers_q <= disp_headers_i;
            if (edge_div_by_zero || edge_signed_ovf) begin
              result_q <= bypass_result_w;
              state_q  <= S_DONE;
            end else begin
              operand_a_q <= operand_a_i;
              operand_b_q <= operand_b_i;
              operator_q <= operator_i;
              cycle_cnt_q <= 4'd0;
              state_q <= S_COMPUTE;
            end
          end
        end

        S_COMPUTE: begin
          if (cycle_cnt_q == 4'd15) begin
            state_q <= S_DONE;
          end else begin
            cycle_cnt_q <= cycle_cnt_q + 4'd1;
          end
        end

        S_DONE: begin
          if (wb_ready_i) begin
            state_q <= S_IDLE;
          end
        end

        default: begin
          state_q <= S_IDLE;
        end


      endcase
    end
  end



endmodule


