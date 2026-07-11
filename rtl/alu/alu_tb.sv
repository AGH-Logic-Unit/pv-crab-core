`timescale 1ns/1ps // time_unit/time_precision


module alu_tb;
  import defs_pkg::*;
  logic [63:0] a, b, res;
  operator_t op;
  logic zero;

  alu uut(a,b,op,res, zero);

  logic [63:0] a32, b32, tmp;
  logic [63:0] values [] = '{0,1,64, 64'hDEADBEEF, 64'hDEADBEEFDEADBEEF, {64{1'b1}}};

  initial begin
    $dumpfile("dump.vcd");
    $dumpvars(0);
    foreach(values[i]) begin
      a = values[i];
      a32 = {{32{a[31]}}, a[31:0]};
      foreach(values[j]) begin
        b = values[j];
        b32 = {{32{b[31]}}, b[31:0]};

        // ADD
        op = operator_t'{word_mode:0, operator_type: SUM, complement:0}; #10;
        if(res != a+b)
          $display("ERROR expected %0d+%0d=%0d, got %0d", a,b, a+b, res);

        // SUB
        op = operator_t'{word_mode:0, operator_type: SUM, complement:1}; #10;
        if(res != a-b)
          $display("ERROR expected %0d-%0d=%0d, got %0d", a,b, a-b, res);

        if((a==b) != zero)
          $display("ERROR expected %x==%0d=%0d, got %0d", a,b, a==b, zero);

        // AND
        op = operator_t'{word_mode:0, operator_type: AND, complement:0}; #10;
        if(res != (a&b))
          $display("ERROR expected %0d&%0d=%0d, got %0d", a,b, a&b, res);
    
        // OR
        op = operator_t'{word_mode:0, operator_type: OR, complement:0}; #10;
        if(res != (a|b))
          $display("ERROR expected %0d|%0d=%0d, got %0d", a,b, a|b, res);
    
        // XOR
        op = operator_t'{word_mode:0, operator_type: XOR, complement:0}; #10;
        if(res != (a^b))
          $display("ERROR expected %0d^%0d=%0d, got %0d", a,b, a^b, res);
    
        // SLL
        op = operator_t'{word_mode:0, operator_type: SHFL, complement:0}; #10;
        if(res != a<<b[5:0])
          $display("ERROR expected %0b<<%0b=%0b, got %0b", a,b, a<<b[5:0], res);
    
        // SRL
        op = operator_t'{word_mode:0, operator_type: SHFL, complement:1}; #10;
        if(res != a>>b[5:0])
          $display("ERROR expected %0B>>%0B=%0B, got %0B", a,b, a>>b[5:0], res);

        
        // SRA
        op = operator_t'{word_mode:0, operator_type: SHFA, complement:1}; #10;
        if(signed'(res) != signed'(a)>>>b[5:0])
          $display("ERROR expected %0B>>>%0B=%0B, got %0B", a,b[5:0], signed'(a)>>>(b[5:0]), res);
    
        // SLT
        op = operator_t'{word_mode:0, operator_type: SLT, complement:1}; #10;
        if(res != {63'b0, signed'(a)<signed'(b)})
          $display("ERROR expected %0d<%0d=%0d, got %0d", signed'(a),signed'(b), signed'(a)<signed'(b), res);
    
        // SLTU
        op = operator_t'{word_mode:0, operator_type: SLTU, complement:1}; #10;
        if(res != {63'b0, unsigned'(a)<unsigned'(b)})
          $display("ERROR expected u%x<u%0d=%0d, got %0d", a,b, unsigned'(a)<unsigned'(b), res);

        a = {32'b0, a[31:0]};
        b = {32'b0, b[31:0]};
        // ADDW
        op = operator_t'{word_mode:1, operator_type: SUM, complement:0}; #10;
        if(signed'(res) != a32+b32)
          $display("ERROR expected %0d w+ %0d=%0d, got %0d", a,b, a32+b32, res);

        // SUBW
        op = operator_t'{word_mode:1, operator_type: SUM, complement:1}; #10;
        if(signed'(res) != a32-b32)
          $display("ERROR expected %0d w- %0d=%0d, got %0d", a,b, a32-b32, res);

        // SLLW
        tmp = (a32<<b[4:0]);
        op = operator_t'{word_mode:1, operator_type: SHFL, complement:0}; #10;
        if(res != {{32{tmp[31]}}, tmp[31:0]})
          $display("ERROR expected %0bw<<%0b=%0b, got %0b", a,b[4:0], {{32{tmp[31]}}, tmp[31:0]}, res);
    
        // SRLW
        tmp = a>>b[4:0];
        op = operator_t'{word_mode:1, operator_type: SHFL, complement:1}; #10;
        if(res != {{32{tmp[31]}}, tmp[31:0]})
          $display("ERROR expected %0Bw>>%0B=%0B, got %0B", a,b[4:0], {{32{tmp[31]}}, tmp[31:0]}, res);
    
        // SRAW
        tmp = (signed'(a)>>>b[4:0]);
        op = operator_t'{word_mode:1, operator_type: SHFA, complement:1}; #10;
        if(signed'(res) != {{32{tmp[31]}}, tmp[31:0]})
          $display("ERROR expected %0Bw>>>%0B=%0B, got %0B", a,b[4:0], {{32{tmp[31]}}, tmp[31:0]}, res);
    
      end
    end
  end  
endmodule;
