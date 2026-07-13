// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

VL_ATTR_COLD void Vtop___024root___eval_static(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_static\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__clk_i__0 
        = vlSelfRef.sw_pp_divider__DOT__clk_i;
    vlSelfRef.__Vtrigprevexpr___TOP__sw_pp_divider__DOT__rst_ni__0 
        = vlSelfRef.sw_pp_divider__DOT__rst_ni;
}

VL_ATTR_COLD void Vtop___024root___eval_initial(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_initial\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtop___024root___eval_final(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_final\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtop___024root___eval_phase__stl(Vtop___024root* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_settle(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_settle\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 1, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtop___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtop___024root___eval_triggers_vec__stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtop___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtop___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__stl\n"); );
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

void Vtop___024root___ico_sequent__TOP__0(Vtop___024root* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtop___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtop___024root___eval_phase__stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtop___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtop___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtop___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtop___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vtop___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge sw_pp_divider.clk_i)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge sw_pp_divider.rst_ni)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtop___024root___ctor_var_reset(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ctor_var_reset\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->clk_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11908517815223722933ull);
    vlSelf->rst_ni = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3161515032326629241ull);
    vlSelf->disp_valid_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5062724725204529471ull);
    vlSelf->disp_ready_o = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7420104838662577707ull);
    vlSelf->disp_headers_i = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10478367111346368735ull);
    vlSelf->operand_a_i = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13041378484246415997ull);
    vlSelf->operand_b_i = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3617743881988722963ull);
    vlSelf->operator_i = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17112281382430016220ull);
    vlSelf->wb_valid_o = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 598007754215482289ull);
    vlSelf->wb_ready_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9191942747297380762ull);
    vlSelf->wb_headers_o = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 15206968765170477857ull);
    vlSelf->wb_result_o = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3923007969778321329ull);
    vlSelf->sw_pp_divider__DOT__clk_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 760588907148689268ull);
    vlSelf->sw_pp_divider__DOT__rst_ni = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18303614842031358724ull);
    vlSelf->sw_pp_divider__DOT__disp_valid_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17794302707890150125ull);
    vlSelf->sw_pp_divider__DOT__disp_ready_o = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15210656703430543678ull);
    vlSelf->sw_pp_divider__DOT__disp_headers_i = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 4719490471538760013ull);
    vlSelf->sw_pp_divider__DOT__operand_a_i = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5371328280828961791ull);
    vlSelf->sw_pp_divider__DOT__operand_b_i = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 6743177094062096502ull);
    vlSelf->sw_pp_divider__DOT__operator_i = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 5471385639184903502ull);
    vlSelf->sw_pp_divider__DOT__wb_valid_o = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4027486161521673745ull);
    vlSelf->sw_pp_divider__DOT__wb_ready_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4167268130391392373ull);
    vlSelf->sw_pp_divider__DOT__wb_headers_o = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 5126435324508097252ull);
    vlSelf->sw_pp_divider__DOT__wb_result_o = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13280418518650282934ull);
    vlSelf->sw_pp_divider__DOT__state_q = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5441986913813590786ull);
    vlSelf->sw_pp_divider__DOT__cycle_cnt_q = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15589388151983283981ull);
    vlSelf->sw_pp_divider__DOT__operand_a_q = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2758340765130806063ull);
    vlSelf->sw_pp_divider__DOT__operand_b_q = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 16215498136201722119ull);
    vlSelf->sw_pp_divider__DOT__operator_q = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 9856938674625567119ull);
    vlSelf->sw_pp_divider__DOT__headers_q = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 808094436434703414ull);
    vlSelf->sw_pp_divider__DOT__result_q = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14570697846388816660ull);
    vlSelf->sw_pp_divider__DOT__edge_div_by_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10233078272550295982ull);
    vlSelf->sw_pp_divider__DOT__edge_signed_ovf = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17147680122847549853ull);
    vlSelf->sw_pp_divider__DOT__is_unsigned = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7917320281852599489ull);
    vlSelf->sw_pp_divider__DOT__is_rem = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 425986347495032299ull);
    vlSelf->sw_pp_divider__DOT__is_word = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1639366949804245343ull);
    vlSelf->sw_pp_divider__DOT__op_a_prep = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 12641356217803240784ull);
    vlSelf->sw_pp_divider__DOT__op_b_prep = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2779614066790458162ull);
    vlSelf->sw_pp_divider__DOT__bypass_result = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13455267503884285296ull);
    vlSelf->sw_pp_divider__DOT__bypass_result_w = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13214893101200390ull);
    vlSelf->sw_pp_divider__DOT____Vtogcov__clk_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__rst_ni = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__disp_valid_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__disp_ready_o = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__disp_headers_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operand_a_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operand_b_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operator_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__wb_valid_o = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__wb_ready_i = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__wb_headers_o = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__wb_result_o = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__state_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__cycle_cnt_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operand_a_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operand_b_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__operator_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__headers_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__result_q = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__edge_div_by_zero = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__edge_signed_ovf = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__is_unsigned = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__is_rem = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__is_word = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__op_a_prep = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__op_b_prep = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__bypass_result = 0;
    vlSelf->sw_pp_divider__DOT____Vtogcov__bypass_result_w = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__sw_pp_divider__DOT__clk_i__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__sw_pp_divider__DOT__rst_ni__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}

VL_ATTR_COLD void Vtop___024root___configure_coverage(Vtop___024root* vlSelf, bool first) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___configure_coverage\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    (void)first;  // Prevent unused variable warning
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[0]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 4, 17, ".sw_pp_divider", "v_toggle/sw_pp_divider", "clk_i");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[2]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 5, 17, ".sw_pp_divider", "v_toggle/sw_pp_divider", "rst_ni");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[4]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 7, 17, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_valid_i");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[6]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 8, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_ready_o");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[8]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 9, 26, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_headers_i.tag");
    vlSelf->__vlCoverToggleInsert(0, 4, 1, &(vlSymsp->__Vcoverage[16]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 9, 26, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_headers_i.rd_addr");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[26]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 9, 26, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_headers_i.rd_we");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[28]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 9, 26, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_headers_i.exc_valid");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[30]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 9, 26, ".sw_pp_divider", "v_toggle/sw_pp_divider", "disp_headers_i.exc_code");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[38]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 10, 24, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operand_a_i");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[166]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 11, 24, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operand_b_i");
    vlSelf->__vlCoverToggleInsert(0, 2, 1, &(vlSymsp->__Vcoverage[294]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 12, 23, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operator_i");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[300]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 14, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_valid_o");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[302]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 15, 17, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_ready_i");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[304]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 16, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_headers_o.tag");
    vlSelf->__vlCoverToggleInsert(0, 4, 1, &(vlSymsp->__Vcoverage[312]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 16, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_headers_o.rd_addr");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[322]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 16, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_headers_o.rd_we");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[324]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 16, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_headers_o.exc_valid");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[326]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 16, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_headers_o.exc_code");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[334]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 17, 25, ".sw_pp_divider", "v_toggle/sw_pp_divider", "wb_result_o");
    vlSelf->__vlCoverToggleInsert(0, 1, 1, &(vlSymsp->__Vcoverage[462]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 27, 11, ".sw_pp_divider", "v_toggle/sw_pp_divider", "state_q");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[466]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 30, 15, ".sw_pp_divider", "v_toggle/sw_pp_divider", "cycle_cnt_q");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[474]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 32, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operand_a_q");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[602]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 33, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operand_b_q");
    vlSelf->__vlCoverToggleInsert(0, 2, 1, &(vlSymsp->__Vcoverage[730]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 34, 15, ".sw_pp_divider", "v_toggle/sw_pp_divider", "operator_q");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[736]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 35, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "headers_q.tag");
    vlSelf->__vlCoverToggleInsert(0, 4, 1, &(vlSymsp->__Vcoverage[744]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 35, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "headers_q.rd_addr");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[754]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 35, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "headers_q.rd_we");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[756]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 35, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "headers_q.exc_valid");
    vlSelf->__vlCoverToggleInsert(0, 3, 1, &(vlSymsp->__Vcoverage[758]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 35, 18, ".sw_pp_divider", "v_toggle/sw_pp_divider", "headers_q.exc_code");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[766]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 37, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "result_q");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[894]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 50, 9, ".sw_pp_divider", "v_toggle/sw_pp_divider", "edge_div_by_zero");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[896]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 51, 9, ".sw_pp_divider", "v_toggle/sw_pp_divider", "edge_signed_ovf");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[898]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 53, 9, ".sw_pp_divider", "v_toggle/sw_pp_divider", "is_unsigned");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[900]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 53, 22, ".sw_pp_divider", "v_toggle/sw_pp_divider", "is_rem");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[902]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 53, 30, ".sw_pp_divider", "v_toggle/sw_pp_divider", "is_word");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[904]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 54, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "op_a_prep");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[1032]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 54, 27, ".sw_pp_divider", "v_toggle/sw_pp_divider", "op_b_prep");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[1160]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 55, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "bypass_result");
    vlSelf->__vlCoverToggleInsert(0, 63, 1, &(vlSymsp->__Vcoverage[1288]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 56, 16, ".sw_pp_divider", "v_toggle/sw_pp_divider", "bypass_result_w");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1416]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 64, 7, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "64-66");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1417]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 64, 8, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "67-69");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1418]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 63, 5, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "63");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1419]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 63, 6, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "71-73");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1420]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 79, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_unsigned==0 && (op_a_prep == 64'hffffffff80000000)==1 && (op_b_prep == {32'sh40{1'h1}})==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1421]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 79, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "((op_b_prep == {32'sh40{1'h1}})==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1422]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 79, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "((op_a_prep == 64'hffffffff80000000)==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1423]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 79, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_unsigned==1) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1424]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 81, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_unsigned==0 && (op_a_prep == 64'h8000000000000000)==1 && (op_b_prep == {32'sh40{1'h1}})==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1425]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 81, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "((op_b_prep == {32'sh40{1'h1}})==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1426]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 81, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "((op_a_prep == 64'h8000000000000000)==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1427]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 81, 79, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_unsigned==1) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1428]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 78, 5, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "78-79");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1429]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 78, 6, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "80-81");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1430]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 86, 23, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_rem==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1431]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 86, 23, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_rem==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1432]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 86, 32, ".sw_pp_divider", "v_branch/sw_pp_divider", "cond_then", "86");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1433]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 86, 33, ".sw_pp_divider", "v_branch/sw_pp_divider", "cond_else", "86");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1434]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 88, 23, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_rem==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1435]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 88, 23, ".sw_pp_divider", "v_expr/sw_pp_divider", "(is_rem==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1436]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 88, 32, ".sw_pp_divider", "v_branch/sw_pp_divider", "cond_then", "88");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1437]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 88, 33, ".sw_pp_divider", "v_branch/sw_pp_divider", "cond_else", "88");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1438]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 87, 14, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "87-88");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1439]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 87, 15, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1440]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 85, 5, ".sw_pp_divider", "v_line/sw_pp_divider", "elsif", "85-86");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1441]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 91, 5, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "91-92");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1442]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 91, 6, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "93-94");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1443]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 58, 3, ".sw_pp_divider", "v_line/sw_pp_divider", "block", "58-61,76,83");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1444]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 114, 13, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "114-116");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1445]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 114, 14, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "117-122");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1446]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 114, 34, ".sw_pp_divider", "v_expr/sw_pp_divider", "(edge_signed_ovf==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1447]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 114, 34, ".sw_pp_divider", "v_expr/sw_pp_divider", "(edge_div_by_zero==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1448]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 114, 34, ".sw_pp_divider", "v_expr/sw_pp_divider", "(edge_div_by_zero==0 && edge_signed_ovf==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1449]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 112, 11, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "112-113");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1450]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 112, 12, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1451]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 111, 15, ".sw_pp_divider", "v_line/sw_pp_divider", "case", "111");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1452]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 128, 11, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "128-129");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1453]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 128, 12, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "130-131");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1454]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 127, 18, ".sw_pp_divider", "v_line/sw_pp_divider", "case", "127");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1455]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 136, 11, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "136-137");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1456]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 136, 12, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1457]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 135, 15, ".sw_pp_divider", "v_line/sw_pp_divider", "case", "135");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1458]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 141, 9, ".sw_pp_divider", "v_line/sw_pp_divider", "case", "141-142");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1459]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 101, 5, ".sw_pp_divider", "v_branch/sw_pp_divider", "if", "101-108");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1460]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 101, 6, ".sw_pp_divider", "v_branch/sw_pp_divider", "else", "109-110");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1461]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 101, 9, ".sw_pp_divider", "v_expr/sw_pp_divider", "(rst_ni==0) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1462]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 101, 9, ".sw_pp_divider", "v_expr/sw_pp_divider", "(rst_ni==1) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[1463]), first, "/workspaces/pv-crab-core/rtl/sw_pp_divider/sw_pp_divider.sv", 99, 3, ".sw_pp_divider", "v_line/sw_pp_divider", "block", "99");
}
