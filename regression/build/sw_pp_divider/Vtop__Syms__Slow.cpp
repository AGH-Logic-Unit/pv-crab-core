// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtop__pch.h"

Vtop__Syms::Vtop__Syms(VerilatedContext* contextp, const char* namep, Vtop* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup top module instance
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(254);
    // Setup sub module instances
    TOP__sw_pp_divider_pkg.ctor(this, "sw_pp_divider_pkg");
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    TOP.__PVT__sw_pp_divider_pkg = &TOP__sw_pp_divider_pkg;
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    TOP__sw_pp_divider_pkg.__Vconfigure(true);
    // Setup scopes
    __Vscopep_TOP = new VerilatedScope{this, "TOP", "TOP", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_sw_pp_divider = new VerilatedScope{this, "sw_pp_divider", "sw_pp_divider", "sw_pp_divider", -12, VerilatedScope::SCOPE_MODULE};
    __Vscopep_sw_pp_divider_pkg = new VerilatedScope{this, "sw_pp_divider_pkg", "sw_pp_divider_pkg", "sw_pp_divider_pkg", -12, VerilatedScope::SCOPE_PACKAGE};
    // Set up scope hierarchy
    __Vhier.add(0, __Vscopep_sw_pp_divider);
    __Vhier.add(0, __Vscopep_sw_pp_divider_pkg);
    // Setup export functions - final: 0
    // Setup export functions - final: 1
    // Setup public variables
    __Vscopep_TOP->varInsert("clk_i", &(TOP.clk_i), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("disp_headers_i", &(TOP.disp_headers_i), false, VLVT_UINT16, VLVD_IN|VLVF_PUB_RW, 0, 1 ,14,0);
    __Vscopep_TOP->varInsert("disp_ready_o", &(TOP.disp_ready_o), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_TOP->varInsert("disp_valid_i", &(TOP.disp_valid_i), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("operand_a_i", &(TOP.operand_a_i), false, VLVT_UINT64, VLVD_IN|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_TOP->varInsert("operand_b_i", &(TOP.operand_b_i), false, VLVT_UINT64, VLVD_IN|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_TOP->varInsert("operator_i", &(TOP.operator_i), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_TOP->varInsert("rst_ni", &(TOP.rst_ni), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("wb_headers_o", &(TOP.wb_headers_o), false, VLVT_UINT16, VLVD_OUT|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,14,0);
    __Vscopep_TOP->varInsert("wb_ready_i", &(TOP.wb_ready_i), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("wb_result_o", &(TOP.wb_result_o), false, VLVT_UINT64, VLVD_OUT|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,63,0);
    __Vscopep_TOP->varInsert("wb_valid_o", &(TOP.wb_valid_o), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("bypass_result", &(TOP.sw_pp_divider__DOT__bypass_result), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("bypass_result_w", &(TOP.sw_pp_divider__DOT__bypass_result_w), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("clk_i", &(TOP.sw_pp_divider__DOT__clk_i), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("cycle_cnt_q", &(TOP.sw_pp_divider__DOT__cycle_cnt_q), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_sw_pp_divider->varInsert("disp_headers_i", &(TOP.sw_pp_divider__DOT__disp_headers_i), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,14,0);
    __Vscopep_sw_pp_divider->varInsert("disp_ready_o", &(TOP.sw_pp_divider__DOT__disp_ready_o), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("disp_valid_i", &(TOP.sw_pp_divider__DOT__disp_valid_i), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("edge_div_by_zero", &(TOP.sw_pp_divider__DOT__edge_div_by_zero), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("edge_signed_ovf", &(TOP.sw_pp_divider__DOT__edge_signed_ovf), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("headers_q", &(TOP.sw_pp_divider__DOT__headers_q), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,14,0);
    __Vscopep_sw_pp_divider->varInsert("is_rem", &(TOP.sw_pp_divider__DOT__is_rem), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("is_unsigned", &(TOP.sw_pp_divider__DOT__is_unsigned), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("is_word", &(TOP.sw_pp_divider__DOT__is_word), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("op_a_prep", &(TOP.sw_pp_divider__DOT__op_a_prep), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("op_b_prep", &(TOP.sw_pp_divider__DOT__op_b_prep), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("operand_a_i", &(TOP.sw_pp_divider__DOT__operand_a_i), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("operand_a_q", &(TOP.sw_pp_divider__DOT__operand_a_q), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("operand_b_i", &(TOP.sw_pp_divider__DOT__operand_b_i), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("operand_b_q", &(TOP.sw_pp_divider__DOT__operand_b_q), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("operator_i", &(TOP.sw_pp_divider__DOT__operator_i), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider->varInsert("operator_q", &(TOP.sw_pp_divider__DOT__operator_q), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider->varInsert("result_q", &(TOP.sw_pp_divider__DOT__result_q), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("rst_ni", &(TOP.sw_pp_divider__DOT__rst_ni), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("state_q", &(TOP.sw_pp_divider__DOT__state_q), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,1,0);
    __Vscopep_sw_pp_divider->varInsert("wb_headers_o", &(TOP.sw_pp_divider__DOT__wb_headers_o), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,14,0);
    __Vscopep_sw_pp_divider->varInsert("wb_ready_i", &(TOP.sw_pp_divider__DOT__wb_ready_i), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_sw_pp_divider->varInsert("wb_result_o", &(TOP.sw_pp_divider__DOT__wb_result_o), false, VLVT_UINT64, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,63,0);
    __Vscopep_sw_pp_divider->varInsert("wb_valid_o", &(TOP.sw_pp_divider__DOT__wb_valid_o), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_DIV", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_DIV))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_DIVU", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_DIVU))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_DIVUW", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_DIVUW))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_DIVW", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_DIVW))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_REM", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_REM))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_REMU", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_REMU))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_REMUW", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_REMUW))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_sw_pp_divider_pkg->varInsert("OP_REMW", const_cast<void*>(static_cast<const void*>(&(TOP__sw_pp_divider_pkg.OP_REMW))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
}

Vtop__Syms::~Vtop__Syms() {
    // Tear down scope hierarchy
    __Vhier.remove(0, __Vscopep_sw_pp_divider);
    __Vhier.remove(0, __Vscopep_sw_pp_divider_pkg);
    // Clear keys from hierarchy map after values have been removed
    __Vhier.clear();
    // Tear down scopes
    VL_DO_CLEAR(delete __Vscopep_TOP, __Vscopep_TOP = nullptr);
    VL_DO_CLEAR(delete __Vscopep_sw_pp_divider, __Vscopep_sw_pp_divider = nullptr);
    VL_DO_CLEAR(delete __Vscopep_sw_pp_divider_pkg, __Vscopep_sw_pp_divider_pkg = nullptr);
    // Tear down sub module instances
    TOP__sw_pp_divider_pkg.dtor();
}
