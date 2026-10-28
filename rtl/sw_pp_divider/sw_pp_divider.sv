module sw_pp_divider
  import sw_pp_divider_pkg::*;
(
    input  logic            clk_i,
    input  logic            rst_ni,

    input  logic            disp_valid_i,
    output logic            disp_ready_o,
    input  t__exe_headers   disp_headers_i,
    input  logic [63:0]     operand_a_i,
    input  logic [63:0]     operand_b_i,
    input  logic [2:0]      operator_i,

    output logic            wb_valid_o,
    input  logic            wb_ready_i,
    output t__exe_headers   wb_headers_o,
    output logic [63:0]     wb_result_o
);

  // FSM states
  typedef enum logic [1:0] {
    S_IDLE,
    S_COMPUTE,
    S_DONE
  } state_e;
  state_e state_q;

  // Cycle counter (0..15 = 16 cycles)
  logic [3:0] cycle_cnt_q;

  // Latched inputs: operand_a_q used as dividend shift register,
  // operand_b_q stores |D| for the duration of computation
  logic [63:0] operand_a_q;
  logic [63:0] operand_b_q;
  logic [2:0]  operator_q;
  t__exe_headers headers_q;

  logic [63:0] result_q;

  // Division core registers
  logic [64:0] pr_q;    // partial remainder (65-bit signed)
  logic [63:0] q_q;     // quotient accumulator

  // Sign tracking: [1]=dividend was negative, [0]=divisor was negative
  logic [1:0]  neg_q;


  assign disp_ready_o = (state_q == S_IDLE);
  assign wb_valid_o   = (state_q == S_DONE) |
                        ((state_q == S_COMPUTE) & (cycle_cnt_q == 4'd15));
  assign wb_headers_o = headers_q;
  assign wb_result_o  = ((state_q == S_COMPUTE) & (cycle_cnt_q == 4'd15)) ?
                        final_result : result_q;


  // ============================================
  // Edge-case detection and preprocessing
  // ============================================
  logic edge_div_by_zero;
  logic edge_signed_ovf;

  logic        is_unsigned, is_rem, is_word;
  logic [63:0] op_a_prep, op_b_prep;
  logic [63:0] bypass_result, bypass_result_w;
  logic [63:0] a_abs, d_abs;

  always_comb begin
    is_unsigned = operator_i[0];
    is_rem      = operator_i[1];
    is_word     = operator_i[2];

    // Word-variant preprocessing: extract low 32 bits with correct extension
    if (is_word) begin
      if (is_unsigned) begin
        op_a_prep = {32'b0, operand_a_i[31:0]};
        op_b_prep = {32'b0, operand_b_i[31:0]};
      end else begin
        op_a_prep = {{32{operand_a_i[31]}}, operand_a_i[31:0]};
        op_b_prep = {{32{operand_b_i[31]}}, operand_b_i[31:0]};
      end
    end else begin
      op_a_prep = operand_a_i;
      op_b_prep = operand_b_i;
    end

    // Edge-case detection
    edge_div_by_zero = (op_b_prep == 64'd0);

    if (is_word) begin
      edge_signed_ovf = ~is_unsigned & (op_a_prep == 64'hFFFF_FFFF_8000_0000) & (op_b_prep == {64{1'b1}});
    end else begin
      edge_signed_ovf = ~is_unsigned & (op_a_prep == 64'h8000_0000_0000_0000) & (op_b_prep == {64{1'b1}});
    end

    // Bypass result for edge cases
    bypass_result = 64'd0;
    if (edge_div_by_zero) begin
      bypass_result = is_rem ? op_a_prep : {64{1'b1}};
    end else if (edge_signed_ovf) begin
      bypass_result = is_rem ? 64'd0 : op_a_prep;
    end

    // Sign-extend result for word variants
    if (is_word) begin
      bypass_result_w = {{32{bypass_result[31]}}, bypass_result[31:0]};
    end else begin
      bypass_result_w = bypass_result;
    end

    // Absolute values for the division core
    a_abs = (op_a_prep[63] & ~is_unsigned) ? (~op_a_prep + 1'b1) : op_a_prep;
    d_abs = (op_b_prep[63] & ~is_unsigned) ? (~op_b_prep + 1'b1) : op_b_prep;
  end


  // ============================================
  // Restoring binary division
  // ============================================
  function automatic logic [65:0] restoring_step(
    input logic [64:0] pr,
    input logic        a_bit,
    input logic [63:0] d
  );
    logic [64:0] pr_shifted, pr_sub;
    pr_shifted = {pr[63:0], a_bit};
    pr_sub     = pr_shifted - {1'b0, d};
    if (!pr_sub[64]) return {1'b1, pr_sub};    // >= 0: success
    else             return {1'b0, pr_shifted}; // < 0: restore
  endfunction


  logic [65:0] rs0, rs1, rs2, rs3;
  logic [63:0] q_next, a_next;
  logic [64:0] pr_next;

  always_comb begin
    rs0 = restoring_step(pr_q,      operand_a_q[63], operand_b_q);
    rs1 = restoring_step(rs0[64:0], operand_a_q[62], operand_b_q);
    rs2 = restoring_step(rs1[64:0], operand_a_q[61], operand_b_q);
    rs3 = restoring_step(rs2[64:0], operand_a_q[60], operand_b_q);

    pr_next = rs3[64:0];
    q_next  = {q_q[59:0], rs0[65], rs1[65], rs2[65], rs3[65]};
    a_next  = {operand_a_q[59:0], 4'b0}; // consume top 4 bits of dividend
  end


  // ============================================
  // Postprocessing — sign correction and result select
  // ============================================
  logic [63:0] quot_abs, rem_abs;
  logic        neg_quot, neg_rem;
  logic [63:0] final_result;

  always_comb begin
    quot_abs = q_next;
    rem_abs  = pr_next[63:0];

    // Signs differ -> negative quotient; remainder takes dividend's sign
    neg_quot = neg_q[1] ^ neg_q[0];
    neg_rem  = neg_q[1];

    if (operator_q[1]) begin // is_rem
      final_result = neg_rem  ? (~rem_abs  + 1'b1) : rem_abs;
    end else begin
      final_result = neg_quot ? (~quot_abs + 1'b1) : quot_abs;
    end

    // Sign-extend for word variants
    if (operator_q[2]) begin // is_word
      final_result = {{32{final_result[31]}}, final_result[31:0]};
    end
  end


  // ============================================
  // Sequential logic — FSM
  // ============================================
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      state_q     <= S_IDLE;
      cycle_cnt_q <= 4'd0;
      result_q    <= 64'd0;
      headers_q   <= '0;
      operand_a_q <= 64'd0;
      operand_b_q <= 64'd0;
      operator_q  <= 3'd0;
      pr_q        <= 65'd0;
      q_q         <= 64'd0;
      neg_q       <= 2'd0;
    end else begin
      case (state_q)

        S_IDLE: begin
          if (disp_valid_i) begin
            headers_q <= disp_headers_i;
            if (edge_div_by_zero || edge_signed_ovf) begin
              // 1-cycle bypass - egde cases
              result_q <= bypass_result_w;
              state_q  <= S_DONE;
            end else begin
              // Latch absolute values; track original signs for postprocessing
              operand_a_q <= a_abs;
              operand_b_q <= d_abs;
              operator_q  <= operator_i;
              pr_q        <= 65'b0;
              q_q         <= 64'b0;
              neg_q       <= {op_a_prep[63] & ~operator_i[0],
                              op_b_prep[63] & ~operator_i[0]};
              cycle_cnt_q <= 4'd0;
              state_q     <= S_COMPUTE;
            end
          end
        end

        S_COMPUTE: begin
          pr_q        <= pr_next;
          q_q         <= q_next;
          operand_a_q <= a_next;
          cycle_cnt_q <= cycle_cnt_q + 4'd1;
          if (cycle_cnt_q == 4'd15) begin
            result_q <= final_result;  // latch for S_DONE fallback
            state_q  <= wb_ready_i ? S_IDLE : S_DONE;
          end
        end

        S_DONE: begin
          if (wb_ready_i) begin
            state_q <= S_IDLE;
          end
        end

        default: state_q <= S_IDLE;

      endcase
    end
  end

endmodule
