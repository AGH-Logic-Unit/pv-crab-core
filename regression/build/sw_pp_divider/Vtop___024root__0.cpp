// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

void Vtop___024root___eval_triggers_vec__ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VicoFirstIteration)));
}

bool Vtop___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtop___024root___ico_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ico_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__state_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q))) {
        VL_COV_TOGGLE_CHG_ST_I(2, vlSymsp->__Vcoverage + 462, vlSelfRef.sw_pp_divider__DOT__state_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q 
            = vlSelfRef.sw_pp_divider__DOT__state_q;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 466, vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q 
            = vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__operand_a_q 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 474, vlSelfRef.sw_pp_divider__DOT__operand_a_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q 
            = vlSelfRef.sw_pp_divider__DOT__operand_a_q;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__operand_b_q 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 602, vlSelfRef.sw_pp_divider__DOT__operand_b_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q 
            = vlSelfRef.sw_pp_divider__DOT__operand_b_q;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__operator_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q))) {
        VL_COV_TOGGLE_CHG_ST_I(3, vlSymsp->__Vcoverage + 730, vlSelfRef.sw_pp_divider__DOT__operator_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q 
            = vlSelfRef.sw_pp_divider__DOT__operator_q;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__result_q ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 766, vlSelfRef.sw_pp_divider__DOT__result_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q 
            = vlSelfRef.sw_pp_divider__DOT__result_q;
    }
    vlSelfRef.sw_pp_divider__DOT__clk_i = vlSelfRef.clk_i;
    vlSelfRef.sw_pp_divider__DOT__rst_ni = vlSelfRef.rst_ni;
    vlSelfRef.sw_pp_divider__DOT__disp_valid_i = vlSelfRef.disp_valid_i;
    vlSelfRef.sw_pp_divider__DOT__wb_ready_i = vlSelfRef.wb_ready_i;
    if ((0x00007800U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 736, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 0x0000000bU), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                                  >> 0x0000000bU));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x07ffU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00007800U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x000007c0U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(5, vlSymsp->__Vcoverage + 744, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 6U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 6U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x783fU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x000007c0U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x00000020U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 754, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 5U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 5U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7fdfU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00000020U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x00000010U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 756, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 4U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 4U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7fefU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00000010U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x0000000fU & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 758, vlSelfRef.sw_pp_divider__DOT__headers_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7ff0U & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x0000000fU & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    vlSelfRef.sw_pp_divider__DOT__disp_ready_o = (0U 
                                                  == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q));
    vlSelfRef.sw_pp_divider__DOT__wb_valid_o = (2U 
                                                == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q));
    vlSelfRef.sw_pp_divider__DOT__wb_result_o = vlSelfRef.sw_pp_divider__DOT__result_q;
    vlSelfRef.sw_pp_divider__DOT__disp_headers_i = vlSelfRef.disp_headers_i;
    vlSelfRef.sw_pp_divider__DOT__wb_headers_o = vlSelfRef.sw_pp_divider__DOT__headers_q;
    vlSelfRef.sw_pp_divider__DOT__operand_a_i = vlSelfRef.operand_a_i;
    vlSelfRef.sw_pp_divider__DOT__operand_b_i = vlSelfRef.operand_b_i;
    vlSelfRef.sw_pp_divider__DOT__operator_i = vlSelfRef.operator_i;
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__clk_i) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__clk_i))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 0, vlSelfRef.sw_pp_divider__DOT__clk_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__clk_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__clk_i 
            = vlSelfRef.sw_pp_divider__DOT__clk_i;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__rst_ni) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__rst_ni))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 2, vlSelfRef.sw_pp_divider__DOT__rst_ni, vlSelfRef.sw_pp_divider__DOT____Vtogcov__rst_ni);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__rst_ni 
            = vlSelfRef.sw_pp_divider__DOT__rst_ni;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__disp_valid_i) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_valid_i))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 4, vlSelfRef.sw_pp_divider__DOT__disp_valid_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_valid_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_valid_i 
            = vlSelfRef.sw_pp_divider__DOT__disp_valid_i;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__wb_ready_i) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_ready_i))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 302, vlSelfRef.sw_pp_divider__DOT__wb_ready_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_ready_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_ready_i 
            = vlSelfRef.sw_pp_divider__DOT__wb_ready_i;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__disp_ready_o) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 6, vlSelfRef.sw_pp_divider__DOT__disp_ready_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o 
            = vlSelfRef.sw_pp_divider__DOT__disp_ready_o;
    }
    vlSelfRef.disp_ready_o = vlSelfRef.sw_pp_divider__DOT__disp_ready_o;
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__wb_valid_o) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 300, vlSelfRef.sw_pp_divider__DOT__wb_valid_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o 
            = vlSelfRef.sw_pp_divider__DOT__wb_valid_o;
    }
    vlSelfRef.wb_valid_o = vlSelfRef.sw_pp_divider__DOT__wb_valid_o;
    if ((vlSelfRef.sw_pp_divider__DOT__wb_result_o 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 334, vlSelfRef.sw_pp_divider__DOT__wb_result_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o 
            = vlSelfRef.sw_pp_divider__DOT__wb_result_o;
    }
    vlSelfRef.wb_result_o = vlSelfRef.sw_pp_divider__DOT__wb_result_o;
    if ((0x00007800U & ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 8, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                                >> 0x0000000bU), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i) 
                                                  >> 0x0000000bU));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i 
            = ((0x07ffU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)) 
               | (0x00007800U & (IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i)));
    }
    if ((0x000007c0U & ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)))) {
        VL_COV_TOGGLE_CHG_ST_I(5, vlSymsp->__Vcoverage + 16, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                                >> 6U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i) 
                                         >> 6U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i 
            = ((0x783fU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)) 
               | (0x000007c0U & (IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i)));
    }
    if ((0x00000020U & ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 26, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                                >> 5U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i) 
                                         >> 5U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i 
            = ((0x7fdfU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)) 
               | (0x00000020U & (IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i)));
    }
    if ((0x00000010U & ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 28, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                                >> 4U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i) 
                                         >> 4U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i 
            = ((0x7fefU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)) 
               | (0x00000010U & (IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i)));
    }
    if ((0x0000000fU & ((IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 30, vlSelfRef.sw_pp_divider__DOT__disp_headers_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i 
            = ((0x7ff0U & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_headers_i)) 
               | (0x0000000fU & (IData)(vlSelfRef.sw_pp_divider__DOT__disp_headers_i)));
    }
    vlSelfRef.wb_headers_o = vlSelfRef.sw_pp_divider__DOT__wb_headers_o;
    if ((0x00007800U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 304, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 0x0000000bU), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                                  >> 0x0000000bU));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x07ffU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00007800U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x000007c0U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(5, vlSymsp->__Vcoverage + 312, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 6U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 6U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x783fU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x000007c0U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x00000020U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 322, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 5U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 5U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7fdfU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00000020U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x00000010U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 324, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 4U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 4U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7fefU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00000010U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x0000000fU & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 326, vlSelfRef.sw_pp_divider__DOT__wb_headers_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7ff0U & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x0000000fU & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((vlSelfRef.sw_pp_divider__DOT__operand_a_i 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_i)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 38, vlSelfRef.sw_pp_divider__DOT__operand_a_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_i 
            = vlSelfRef.sw_pp_divider__DOT__operand_a_i;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__operand_b_i 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_i)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 166, vlSelfRef.sw_pp_divider__DOT__operand_b_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_i 
            = vlSelfRef.sw_pp_divider__DOT__operand_b_i;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__operator_i) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_i))) {
        VL_COV_TOGGLE_CHG_ST_I(3, vlSymsp->__Vcoverage + 294, vlSelfRef.sw_pp_divider__DOT__operator_i, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_i);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_i 
            = vlSelfRef.sw_pp_divider__DOT__operator_i;
    }
    vlSelfRef.sw_pp_divider__DOT__is_unsigned = (1U 
                                                 & (IData)(vlSelfRef.sw_pp_divider__DOT__operator_i));
    vlSelfRef.sw_pp_divider__DOT__is_rem = (1U & ((IData)(vlSelfRef.sw_pp_divider__DOT__operator_i) 
                                                  >> 1U));
    vlSelfRef.sw_pp_divider__DOT__is_word = (1U & ((IData)(vlSelfRef.sw_pp_divider__DOT__operator_i) 
                                                   >> 2U));
    if (vlSelfRef.sw_pp_divider__DOT__is_word) {
        if (vlSelfRef.sw_pp_divider__DOT__is_unsigned) {
            vlSelfRef.sw_pp_divider__DOT__op_a_prep 
                = (QData)((IData)(vlSelfRef.sw_pp_divider__DOT__operand_a_i));
            vlSelfRef.sw_pp_divider__DOT__op_b_prep 
                = (QData)((IData)(vlSelfRef.sw_pp_divider__DOT__operand_b_i));
            ++(vlSymsp->__Vcoverage[1416]);
        } else {
            vlSelfRef.sw_pp_divider__DOT__op_a_prep 
                = (((QData)((IData)((- (IData)((1U 
                                                & (IData)(
                                                          (vlSelfRef.sw_pp_divider__DOT__operand_a_i 
                                                           >> 0x1fU))))))) 
                    << 0x00000020U) | (QData)((IData)(vlSelfRef.sw_pp_divider__DOT__operand_a_i)));
            vlSelfRef.sw_pp_divider__DOT__op_b_prep 
                = (((QData)((IData)((- (IData)((1U 
                                                & (IData)(
                                                          (vlSelfRef.sw_pp_divider__DOT__operand_b_i 
                                                           >> 0x1fU))))))) 
                    << 0x00000020U) | (QData)((IData)(vlSelfRef.sw_pp_divider__DOT__operand_b_i)));
            ++(vlSymsp->__Vcoverage[1417]);
        }
        ++(vlSymsp->__Vcoverage[1418]);
    } else {
        vlSelfRef.sw_pp_divider__DOT__op_a_prep = vlSelfRef.sw_pp_divider__DOT__operand_a_i;
        vlSelfRef.sw_pp_divider__DOT__op_b_prep = vlSelfRef.sw_pp_divider__DOT__operand_b_i;
        ++(vlSymsp->__Vcoverage[1419]);
    }
    vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero 
        = (0ULL == vlSelfRef.sw_pp_divider__DOT__op_b_prep);
    if (vlSelfRef.sw_pp_divider__DOT__is_word) {
        vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf 
            = (((~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_unsigned)) 
                & (0xffffffff80000000ULL == vlSelfRef.sw_pp_divider__DOT__op_a_prep)) 
               & (0xffffffffffffffffULL == vlSelfRef.sw_pp_divider__DOT__op_b_prep));
        if ((((~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_unsigned)) 
              & (0xffffffff80000000ULL == vlSelfRef.sw_pp_divider__DOT__op_a_prep)) 
             & (0xffffffffffffffffULL == vlSelfRef.sw_pp_divider__DOT__op_b_prep))) {
            ++(vlSymsp->__Vcoverage[1420]);
        }
        if ((0xffffffffffffffffULL != vlSelfRef.sw_pp_divider__DOT__op_b_prep)) {
            ++(vlSymsp->__Vcoverage[1421]);
        }
        if ((0xffffffff80000000ULL != vlSelfRef.sw_pp_divider__DOT__op_a_prep)) {
            ++(vlSymsp->__Vcoverage[1422]);
        }
        if (vlSelfRef.sw_pp_divider__DOT__is_unsigned) {
            ++(vlSymsp->__Vcoverage[1423]);
        }
        ++(vlSymsp->__Vcoverage[1428]);
    } else {
        vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf 
            = (((~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_unsigned)) 
                & (0x8000000000000000ULL == vlSelfRef.sw_pp_divider__DOT__op_a_prep)) 
               & (0xffffffffffffffffULL == vlSelfRef.sw_pp_divider__DOT__op_b_prep));
        if ((((~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_unsigned)) 
              & (0x8000000000000000ULL == vlSelfRef.sw_pp_divider__DOT__op_a_prep)) 
             & (0xffffffffffffffffULL == vlSelfRef.sw_pp_divider__DOT__op_b_prep))) {
            ++(vlSymsp->__Vcoverage[1424]);
        }
        if ((0xffffffffffffffffULL != vlSelfRef.sw_pp_divider__DOT__op_b_prep)) {
            ++(vlSymsp->__Vcoverage[1425]);
        }
        if ((0x8000000000000000ULL != vlSelfRef.sw_pp_divider__DOT__op_a_prep)) {
            ++(vlSymsp->__Vcoverage[1426]);
        }
        if (vlSelfRef.sw_pp_divider__DOT__is_unsigned) {
            ++(vlSymsp->__Vcoverage[1427]);
        }
        ++(vlSymsp->__Vcoverage[1429]);
    }
    vlSelfRef.sw_pp_divider__DOT__bypass_result = 0ULL;
    if (vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero) {
        vlSelfRef.sw_pp_divider__DOT__bypass_result 
            = ((IData)(vlSelfRef.sw_pp_divider__DOT__is_rem)
                ? ([&]() {
                    ++(vlSymsp->__Vcoverage[1432]);
                }(), vlSelfRef.sw_pp_divider__DOT__op_a_prep)
                : ([&]() {
                    ++(vlSymsp->__Vcoverage[1433]);
                }(), 0xffffffffffffffffULL));
        if (vlSelfRef.sw_pp_divider__DOT__is_rem) {
            ++(vlSymsp->__Vcoverage[1430]);
        }
        if ((1U & (~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_rem)))) {
            ++(vlSymsp->__Vcoverage[1431]);
        }
        ++(vlSymsp->__Vcoverage[1440]);
    } else if (vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf) {
        vlSelfRef.sw_pp_divider__DOT__bypass_result 
            = ((IData)(vlSelfRef.sw_pp_divider__DOT__is_rem)
                ? ([&]() {
                    ++(vlSymsp->__Vcoverage[1436]);
                }(), 0ULL) : ([&]() {
                    ++(vlSymsp->__Vcoverage[1437]);
                }(), vlSelfRef.sw_pp_divider__DOT__op_a_prep));
        if (vlSelfRef.sw_pp_divider__DOT__is_rem) {
            ++(vlSymsp->__Vcoverage[1434]);
        }
        if ((1U & (~ (IData)(vlSelfRef.sw_pp_divider__DOT__is_rem)))) {
            ++(vlSymsp->__Vcoverage[1435]);
        }
        ++(vlSymsp->__Vcoverage[1438]);
    } else {
        ++(vlSymsp->__Vcoverage[1439]);
    }
    if (vlSelfRef.sw_pp_divider__DOT__is_word) {
        vlSelfRef.sw_pp_divider__DOT__bypass_result_w 
            = (((QData)((IData)((- (IData)((1U & (IData)(
                                                         (vlSelfRef.sw_pp_divider__DOT__bypass_result 
                                                          >> 0x1fU))))))) 
                << 0x00000020U) | (QData)((IData)(vlSelfRef.sw_pp_divider__DOT__bypass_result)));
        ++(vlSymsp->__Vcoverage[1441]);
    } else {
        vlSelfRef.sw_pp_divider__DOT__bypass_result_w 
            = vlSelfRef.sw_pp_divider__DOT__bypass_result;
        ++(vlSymsp->__Vcoverage[1442]);
    }
    ++(vlSymsp->__Vcoverage[1443]);
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__is_unsigned) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_unsigned))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 898, vlSelfRef.sw_pp_divider__DOT__is_unsigned, vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_unsigned);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_unsigned 
            = vlSelfRef.sw_pp_divider__DOT__is_unsigned;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__is_rem) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_rem))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 900, vlSelfRef.sw_pp_divider__DOT__is_rem, vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_rem);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_rem 
            = vlSelfRef.sw_pp_divider__DOT__is_rem;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__is_word) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_word))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 902, vlSelfRef.sw_pp_divider__DOT__is_word, vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_word);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__is_word 
            = vlSelfRef.sw_pp_divider__DOT__is_word;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__op_a_prep ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_a_prep)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 904, vlSelfRef.sw_pp_divider__DOT__op_a_prep, vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_a_prep);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_a_prep 
            = vlSelfRef.sw_pp_divider__DOT__op_a_prep;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__op_b_prep ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_b_prep)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 1032, vlSelfRef.sw_pp_divider__DOT__op_b_prep, vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_b_prep);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__op_b_prep 
            = vlSelfRef.sw_pp_divider__DOT__op_b_prep;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_div_by_zero))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 894, vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero, vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_div_by_zero);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_div_by_zero 
            = vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_signed_ovf))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 896, vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf, vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_signed_ovf);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__edge_signed_ovf 
            = vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__bypass_result 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 1160, vlSelfRef.sw_pp_divider__DOT__bypass_result, vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result 
            = vlSelfRef.sw_pp_divider__DOT__bypass_result;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__bypass_result_w 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result_w)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 1288, vlSelfRef.sw_pp_divider__DOT__bypass_result_w, vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result_w);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__bypass_result_w 
            = vlSelfRef.sw_pp_divider__DOT__bypass_result_w;
    }
}

void Vtop___024root___eval_ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
        Vtop___024root___ico_sequent__TOP__0(vlSelf);
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtop___024root___eval_phase__ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    Vtop___024root___eval_triggers_vec__ico(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    __VicoExecute = Vtop___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        Vtop___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vtop___024root___eval_triggers_vec__act(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__act\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((((~ (IData)(vlSelfRef.sw_pp_divider__DOT__rst_ni)) 
                                                       & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__rst_ni__0)) 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.sw_pp_divider__DOT__clk_i) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__clk_i__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__clk_i__0 
        = vlSelfRef.sw_pp_divider__DOT__clk_i;
    vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__rst_ni__0 
        = vlSelfRef.sw_pp_divider__DOT__rst_ni;
}

bool Vtop___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtop___024root___nba_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*1:0*/ __Vdly__sw_pp_divider__DOT__state_q;
    __Vdly__sw_pp_divider__DOT__state_q = 0;
    CData/*3:0*/ __Vdly__sw_pp_divider__DOT__cycle_cnt_q;
    __Vdly__sw_pp_divider__DOT__cycle_cnt_q = 0;
    // Body
    __Vdly__sw_pp_divider__DOT__cycle_cnt_q = vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q;
    __Vdly__sw_pp_divider__DOT__state_q = vlSelfRef.sw_pp_divider__DOT__state_q;
    if (vlSelfRef.sw_pp_divider__DOT__rst_ni) {
        if ((0U == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q))) {
            if (vlSelfRef.sw_pp_divider__DOT__disp_valid_i) {
                vlSelfRef.sw_pp_divider__DOT__headers_q 
                    = vlSelfRef.sw_pp_divider__DOT__disp_headers_i;
                if (((IData)(vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero) 
                     | (IData)(vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf))) {
                    ++(vlSymsp->__Vcoverage[1444]);
                    vlSelfRef.sw_pp_divider__DOT__result_q 
                        = vlSelfRef.sw_pp_divider__DOT__bypass_result_w;
                    __Vdly__sw_pp_divider__DOT__state_q = 2U;
                } else {
                    ++(vlSymsp->__Vcoverage[1445]);
                    vlSelfRef.sw_pp_divider__DOT__operand_a_q 
                        = vlSelfRef.sw_pp_divider__DOT__operand_a_i;
                    vlSelfRef.sw_pp_divider__DOT__operand_b_q 
                        = vlSelfRef.sw_pp_divider__DOT__operand_b_i;
                    vlSelfRef.sw_pp_divider__DOT__operator_q 
                        = vlSelfRef.sw_pp_divider__DOT__operator_i;
                    __Vdly__sw_pp_divider__DOT__cycle_cnt_q = 0U;
                    __Vdly__sw_pp_divider__DOT__state_q = 1U;
                }
                if (vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf) {
                    ++(vlSymsp->__Vcoverage[1446]);
                }
                if (vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero) {
                    ++(vlSymsp->__Vcoverage[1447]);
                }
                if ((1U & ((~ (IData)(vlSelfRef.sw_pp_divider__DOT__edge_div_by_zero)) 
                           & (~ (IData)(vlSelfRef.sw_pp_divider__DOT__edge_signed_ovf))))) {
                    ++(vlSymsp->__Vcoverage[1448]);
                }
                ++(vlSymsp->__Vcoverage[1449]);
            } else {
                ++(vlSymsp->__Vcoverage[1450]);
            }
            ++(vlSymsp->__Vcoverage[1451]);
        } else if ((1U == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q))) {
            if ((0x0fU == (IData)(vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q))) {
                ++(vlSymsp->__Vcoverage[1452]);
                __Vdly__sw_pp_divider__DOT__state_q = 2U;
            } else {
                __Vdly__sw_pp_divider__DOT__cycle_cnt_q 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q)));
                ++(vlSymsp->__Vcoverage[1453]);
            }
            ++(vlSymsp->__Vcoverage[1454]);
        } else if ((2U == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q))) {
            if (vlSelfRef.sw_pp_divider__DOT__wb_ready_i) {
                ++(vlSymsp->__Vcoverage[1455]);
                __Vdly__sw_pp_divider__DOT__state_q = 0U;
            } else {
                ++(vlSymsp->__Vcoverage[1456]);
            }
            ++(vlSymsp->__Vcoverage[1457]);
        } else {
            ++(vlSymsp->__Vcoverage[1458]);
            __Vdly__sw_pp_divider__DOT__state_q = 0U;
        }
        ++(vlSymsp->__Vcoverage[1460]);
    } else {
        ++(vlSymsp->__Vcoverage[1459]);
        __Vdly__sw_pp_divider__DOT__state_q = 0U;
        __Vdly__sw_pp_divider__DOT__cycle_cnt_q = 0U;
        vlSelfRef.sw_pp_divider__DOT__result_q = 0ULL;
        vlSelfRef.sw_pp_divider__DOT__headers_q = 0U;
        vlSelfRef.sw_pp_divider__DOT__operand_a_q = 0ULL;
        vlSelfRef.sw_pp_divider__DOT__operand_b_q = 0ULL;
        vlSelfRef.sw_pp_divider__DOT__operator_q = 0U;
    }
    if ((1U & (~ (IData)(vlSelfRef.sw_pp_divider__DOT__rst_ni)))) {
        ++(vlSymsp->__Vcoverage[1461]);
    }
    if (vlSelfRef.sw_pp_divider__DOT__rst_ni) {
        ++(vlSymsp->__Vcoverage[1462]);
    }
    ++(vlSymsp->__Vcoverage[1463]);
    vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q = __Vdly__sw_pp_divider__DOT__cycle_cnt_q;
    vlSelfRef.sw_pp_divider__DOT__state_q = __Vdly__sw_pp_divider__DOT__state_q;
    if ((vlSelfRef.sw_pp_divider__DOT__operand_a_q 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 474, vlSelfRef.sw_pp_divider__DOT__operand_a_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_a_q 
            = vlSelfRef.sw_pp_divider__DOT__operand_a_q;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__operand_b_q 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 602, vlSelfRef.sw_pp_divider__DOT__operand_b_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operand_b_q 
            = vlSelfRef.sw_pp_divider__DOT__operand_b_q;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__operator_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q))) {
        VL_COV_TOGGLE_CHG_ST_I(3, vlSymsp->__Vcoverage + 730, vlSelfRef.sw_pp_divider__DOT__operator_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__operator_q 
            = vlSelfRef.sw_pp_divider__DOT__operator_q;
    }
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 466, vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__cycle_cnt_q 
            = vlSelfRef.sw_pp_divider__DOT__cycle_cnt_q;
    }
    if ((vlSelfRef.sw_pp_divider__DOT__result_q ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 766, vlSelfRef.sw_pp_divider__DOT__result_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__result_q 
            = vlSelfRef.sw_pp_divider__DOT__result_q;
    }
    vlSelfRef.sw_pp_divider__DOT__wb_result_o = vlSelfRef.sw_pp_divider__DOT__result_q;
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__state_q) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q))) {
        VL_COV_TOGGLE_CHG_ST_I(2, vlSymsp->__Vcoverage + 462, vlSelfRef.sw_pp_divider__DOT__state_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__state_q 
            = vlSelfRef.sw_pp_divider__DOT__state_q;
    }
    vlSelfRef.sw_pp_divider__DOT__disp_ready_o = (0U 
                                                  == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q));
    vlSelfRef.sw_pp_divider__DOT__wb_valid_o = (2U 
                                                == (IData)(vlSelfRef.sw_pp_divider__DOT__state_q));
    if ((0x00007800U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 736, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 0x0000000bU), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                                  >> 0x0000000bU));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x07ffU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00007800U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x000007c0U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(5, vlSymsp->__Vcoverage + 744, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 6U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 6U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x783fU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x000007c0U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x00000020U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 754, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 5U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 5U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7fdfU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00000020U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x00000010U & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 756, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                                >> 4U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q) 
                                         >> 4U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7fefU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x00000010U & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    if ((0x0000000fU & ((IData)(vlSelfRef.sw_pp_divider__DOT__headers_q) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 758, vlSelfRef.sw_pp_divider__DOT__headers_q, vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q 
            = ((0x7ff0U & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__headers_q)) 
               | (0x0000000fU & (IData)(vlSelfRef.sw_pp_divider__DOT__headers_q)));
    }
    vlSelfRef.sw_pp_divider__DOT__wb_headers_o = vlSelfRef.sw_pp_divider__DOT__headers_q;
    if ((vlSelfRef.sw_pp_divider__DOT__wb_result_o 
         ^ vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o)) {
        VL_COV_TOGGLE_CHG_ST_Q(64, vlSymsp->__Vcoverage + 334, vlSelfRef.sw_pp_divider__DOT__wb_result_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_result_o 
            = vlSelfRef.sw_pp_divider__DOT__wb_result_o;
    }
    vlSelfRef.wb_result_o = vlSelfRef.sw_pp_divider__DOT__wb_result_o;
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__disp_ready_o) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 6, vlSelfRef.sw_pp_divider__DOT__disp_ready_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__disp_ready_o 
            = vlSelfRef.sw_pp_divider__DOT__disp_ready_o;
    }
    vlSelfRef.disp_ready_o = vlSelfRef.sw_pp_divider__DOT__disp_ready_o;
    if (((IData)(vlSelfRef.sw_pp_divider__DOT__wb_valid_o) 
         ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 300, vlSelfRef.sw_pp_divider__DOT__wb_valid_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_valid_o 
            = vlSelfRef.sw_pp_divider__DOT__wb_valid_o;
    }
    vlSelfRef.wb_valid_o = vlSelfRef.sw_pp_divider__DOT__wb_valid_o;
    vlSelfRef.wb_headers_o = vlSelfRef.sw_pp_divider__DOT__wb_headers_o;
    if ((0x00007800U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 304, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 0x0000000bU), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                                  >> 0x0000000bU));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x07ffU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00007800U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x000007c0U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(5, vlSymsp->__Vcoverage + 312, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 6U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 6U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x783fU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x000007c0U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x00000020U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 322, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 5U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 5U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7fdfU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00000020U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x00000010U & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 324, 
                               ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                                >> 4U), ((IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o) 
                                         >> 4U));
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7fefU & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x00000010U & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
    if ((0x0000000fU & ((IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o) 
                        ^ (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)))) {
        VL_COV_TOGGLE_CHG_ST_I(4, vlSymsp->__Vcoverage + 326, vlSelfRef.sw_pp_divider__DOT__wb_headers_o, vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o);
        vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o 
            = ((0x7ff0U & (IData)(vlSelfRef.sw_pp_divider__DOT____Vtogcov__wb_headers_o)) 
               | (0x0000000fU & (IData)(vlSelfRef.sw_pp_divider__DOT__wb_headers_o)));
    }
}

void Vtop___024root___eval_nba(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_nba\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtop___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vtop___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtop___024root___eval_phase__act(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__act\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtop___024root___eval_triggers_vec__act(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtop___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vtop___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtop___024root___eval_phase__nba(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__nba\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtop___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtop___024root___eval_nba(vlSelf);
        Vtop___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtop___024root___eval(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 1, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 100 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vtop___024root___eval_phase__ico(vlSelf);
        vlSelfRef.__VicoFirstIteration = 0U;
    } while (vlSelfRef.__VicoPhaseResult);
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 1, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vtop___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 1, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactPhaseResult = Vtop___024root___eval_phase__act(vlSelf);
        } while (vlSelfRef.__VactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtop___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vtop___024root___eval_debug_assertions(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_debug_assertions\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk_i & 0xfeU)))) {
        Verilated::overWidthError("clk_i");
    }
    if (VL_UNLIKELY(((vlSelfRef.rst_ni & 0xfeU)))) {
        Verilated::overWidthError("rst_ni");
    }
    if (VL_UNLIKELY(((vlSelfRef.disp_valid_i & 0xfeU)))) {
        Verilated::overWidthError("disp_valid_i");
    }
    if (VL_UNLIKELY(((vlSelfRef.operator_i & 0xf8U)))) {
        Verilated::overWidthError("operator_i");
    }
    if (VL_UNLIKELY(((vlSelfRef.wb_ready_i & 0xfeU)))) {
        Verilated::overWidthError("wb_ready_i");
    }
}
#endif  // VL_DEBUG
