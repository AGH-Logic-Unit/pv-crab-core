// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtop.h for the primary calling header

#ifndef VERILATED_VTOP___024ROOT_H_
#define VERILATED_VTOP___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_cov.h"
class Vtop_sw_pp_divider_pkg;


class Vtop__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtop___024root final {
  public:
    // CELLS
    Vtop_sw_pp_divider_pkg* __PVT__sw_pp_divider_pkg;

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk_i,0,0);
        VL_IN8(rst_ni,0,0);
        VL_IN8(disp_valid_i,0,0);
        VL_OUT8(disp_ready_o,0,0);
        VL_IN8(operator_i,2,0);
        VL_OUT8(wb_valid_o,0,0);
        VL_IN8(wb_ready_i,0,0);
        CData/*0:0*/ sw_pp_divider__DOT__clk_i;
        CData/*0:0*/ sw_pp_divider__DOT__rst_ni;
        CData/*0:0*/ sw_pp_divider__DOT__disp_valid_i;
        CData/*0:0*/ sw_pp_divider__DOT__disp_ready_o;
        CData/*2:0*/ sw_pp_divider__DOT__operator_i;
        CData/*0:0*/ sw_pp_divider__DOT__wb_valid_o;
        CData/*0:0*/ sw_pp_divider__DOT__wb_ready_i;
        CData/*1:0*/ sw_pp_divider__DOT__state_q;
        CData/*3:0*/ sw_pp_divider__DOT__cycle_cnt_q;
        CData/*2:0*/ sw_pp_divider__DOT__operator_q;
        CData/*0:0*/ sw_pp_divider__DOT__edge_div_by_zero;
        CData/*0:0*/ sw_pp_divider__DOT__edge_signed_ovf;
        CData/*0:0*/ sw_pp_divider__DOT__is_unsigned;
        CData/*0:0*/ sw_pp_divider__DOT__is_rem;
        CData/*0:0*/ sw_pp_divider__DOT__is_word;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__clk_i;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__rst_ni;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__disp_valid_i;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__disp_ready_o;
        CData/*2:0*/ sw_pp_divider__DOT____Vtogcov__operator_i;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__wb_valid_o;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__wb_ready_i;
        CData/*1:0*/ sw_pp_divider__DOT____Vtogcov__state_q;
        CData/*3:0*/ sw_pp_divider__DOT____Vtogcov__cycle_cnt_q;
        CData/*2:0*/ sw_pp_divider__DOT____Vtogcov__operator_q;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__edge_div_by_zero;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__edge_signed_ovf;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__is_unsigned;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__is_rem;
        CData/*0:0*/ sw_pp_divider__DOT____Vtogcov__is_word;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __VicoPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__sw_pp_divider__DOT__clk_i__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__sw_pp_divider__DOT__rst_ni__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        VL_IN16(disp_headers_i,14,0);
        VL_OUT16(wb_headers_o,14,0);
        SData/*14:0*/ sw_pp_divider__DOT__disp_headers_i;
        SData/*14:0*/ sw_pp_divider__DOT__wb_headers_o;
        SData/*14:0*/ sw_pp_divider__DOT__headers_q;
        SData/*14:0*/ sw_pp_divider__DOT____Vtogcov__disp_headers_i;
        SData/*14:0*/ sw_pp_divider__DOT____Vtogcov__wb_headers_o;
        SData/*14:0*/ sw_pp_divider__DOT____Vtogcov__headers_q;
        IData/*31:0*/ __VactIterCount;
        VL_IN64(operand_a_i,63,0);
        VL_IN64(operand_b_i,63,0);
        VL_OUT64(wb_result_o,63,0);
        QData/*63:0*/ sw_pp_divider__DOT__operand_a_i;
        QData/*63:0*/ sw_pp_divider__DOT__operand_b_i;
        QData/*63:0*/ sw_pp_divider__DOT__wb_result_o;
        QData/*63:0*/ sw_pp_divider__DOT__operand_a_q;
        QData/*63:0*/ sw_pp_divider__DOT__operand_b_q;
        QData/*63:0*/ sw_pp_divider__DOT__result_q;
        QData/*63:0*/ sw_pp_divider__DOT__op_a_prep;
    };
    struct {
        QData/*63:0*/ sw_pp_divider__DOT__op_b_prep;
        QData/*63:0*/ sw_pp_divider__DOT__bypass_result;
        QData/*63:0*/ sw_pp_divider__DOT__bypass_result_w;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__operand_a_i;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__operand_b_i;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__wb_result_o;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__operand_a_q;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__operand_b_q;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__result_q;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__op_a_prep;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__op_b_prep;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__bypass_result;
        QData/*63:0*/ sw_pp_divider__DOT____Vtogcov__bypass_result_w;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };

    // INTERNAL VARIABLES
    Vtop__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtop___024root(Vtop__Syms* symsp, const char* namep);
    ~Vtop___024root();
    VL_UNCOPYABLE(Vtop___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
    void __vlCoverInsert(uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
        const char* hierp, const char* pagep, const char* commentp, const char* linescovp);
    void __vlCoverToggleInsert(int begin, int end, bool ranged, uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
        const char* hierp, const char* pagep, const char* commentp);
};


#endif  // guard
