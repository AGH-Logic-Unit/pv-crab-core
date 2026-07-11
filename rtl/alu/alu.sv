

module barrel_shifter (
		input logic [63:0] value_i,
		input logic [5:0] shift_amt_i,
		input logic filler_i,
		output logic [63:0] result_o
	);

	logic [63:0] stage [0:6];
	assign stage[0] = value_i;

	generate
		for (genvar i=0;i<=5;++i) begin : shifter_stages
			localparam int sh = 2**i;
			assign stage [1+i] = shift_amt_i[i] ? {stage[i][63-sh:0], {(sh){filler_i}}} : stage[i];
		end : shifter_stages
	endgenerate

	assign result_o = stage[6];
endmodule;

module reverser #(N=64) (
		input logic[N-1:0] value_i,
		output logic[N-1:0] reversed_o
	);
	generate
		for(genvar i=0;i<N;++i) begin : reverser_bits
			assign reversed_o[i] = value_i[N-1-i];
		end : reverser_bits
	endgenerate
endmodule

module alu
	import defs_pkg::*;
	(
		input logic [63:0] operand_a_i,
		input logic [63:0] operand_b_i,
		input operator_t operator_i,
		output logic [63:0] result_o,
		output logic zero_o
	);

	logic shifter_fill, slt_res, sltu_res, shift_arth;
	logic [5:0] shifter_amt;
	logic [63:0] shifter_value, shifter_res, rev_shifter_res, two_complement_b, rev_operand_a;
	logic [63:0] adder, and_res, or_res, xor_res, shift_res;

	// negate operand_b if sub
	assign two_complement_b = operator_i.complement ? (~operand_b_i + 1) : operand_b_i;
	assign adder = operand_a_i + two_complement_b;

	// logical operations
	assign and_res	= operand_a_i & operand_b_i;
	assign or_res		= operand_a_i | operand_b_i;
	assign xor_res	= operand_a_i ^ operand_b_i;

	// reverse input to the shifter if right shift
	reverser reverser_a (operand_a_i, rev_operand_a);
	// WARNING: THIS CAN"T STAY LIKE THAT (just a quick fix)
	assign shifter_value = !operator_i.complement ? operand_a_i : operator_i.word_mode ? {rev_operand_a[63:32], 32'b0} : rev_operand_a;

	
	// prepare operands for the shifter
	assign shift_arth = operator_i.operator_type[2];
	assign shifter_amt = operator_i.word_mode ? {1'b0, operand_b_i[4:0]} : operand_b_i[5:0];
	assign shifter_fill = !shift_arth ? 1'b0 :
												operator_i.word_mode ? operand_a_i[31] : operand_a_i[63];
												
	barrel_shifter barrel_shifter(shifter_value, shifter_amt, shifter_fill, shifter_res);

	// reverse output of the shifter if right shift
	reverser reverser_shift(shifter_res, rev_shifter_res);
	assign shift_res = operator_i.complement ? rev_shifter_res : shifter_res;

	// evaluate less-than and unsigned less-than
	assign slt_res = adder[63] ^ ((operand_a_i[63]&& !operand_b_i[63] && !adder[63]) || (!operand_a_i[63] && operand_b_i[63] && adder[63]));
	assign sltu_res = (!operand_a_i[63] && operand_b_i[63]) | (operand_b_i[63] && adder[63]) | (!operand_a_i[63] && adder[63]);

	// output zero flag
	assign zero_o = (result_o == 64'd0);
	
	// final result multiplexer
	always_comb begin
		result_o = adder;
		unique case(operator_i.operator_type)
			SUM: result_o = operator_i.word_mode ? {{32{adder[31]}},adder[31:0]} : adder;
			SHFL, SHFA: result_o = operator_i.word_mode ? {{32{shift_res[31]}}, shift_res[31:0]} : shift_res;
			SLT: result_o = {63'b0, slt_res};
			SLTU: result_o = {63'b0, sltu_res};
			XOR: result_o = xor_res;
			OR: result_o = or_res;
			AND: result_o = and_res;
		endcase
	end
endmodule;
