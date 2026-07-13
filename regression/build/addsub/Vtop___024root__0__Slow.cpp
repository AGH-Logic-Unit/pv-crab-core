// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

VL_ATTR_COLD void Vtop___024root___eval_static(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_static\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
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
            VL_FATAL_MT("/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 1, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
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

VL_ATTR_COLD void Vtop___024root___eval_stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.addsub__DOT__a_i = vlSelfRef.a_i;
        vlSelfRef.addsub__DOT__b_i = vlSelfRef.b_i;
        vlSelfRef.addsub__DOT__op_i = vlSelfRef.op_i;
        if ((vlSelfRef.addsub__DOT__a_i ^ vlSelfRef.addsub__DOT____Vtogcov__a_i)) {
            VL_COV_TOGGLE_CHG_ST_I(32, vlSymsp->__Vcoverage + 0, vlSelfRef.addsub__DOT__a_i, vlSelfRef.addsub__DOT____Vtogcov__a_i);
            vlSelfRef.addsub__DOT____Vtogcov__a_i = vlSelfRef.addsub__DOT__a_i;
        }
        if ((vlSelfRef.addsub__DOT__b_i ^ vlSelfRef.addsub__DOT____Vtogcov__b_i)) {
            VL_COV_TOGGLE_CHG_ST_I(32, vlSymsp->__Vcoverage + 64, vlSelfRef.addsub__DOT__b_i, vlSelfRef.addsub__DOT____Vtogcov__b_i);
            vlSelfRef.addsub__DOT____Vtogcov__b_i = vlSelfRef.addsub__DOT__b_i;
        }
        if (((IData)(vlSelfRef.addsub__DOT__op_i) ^ (IData)(vlSelfRef.addsub__DOT____Vtogcov__op_i))) {
            VL_COV_TOGGLE_CHG_ST_I(1, vlSymsp->__Vcoverage + 128, vlSelfRef.addsub__DOT__op_i, vlSelfRef.addsub__DOT____Vtogcov__op_i);
            vlSelfRef.addsub__DOT____Vtogcov__op_i
                = vlSelfRef.addsub__DOT__op_i;
        }
        vlSelfRef.addsub__DOT__res_o = ((IData)(vlSelfRef.addsub__DOT__op_i)
                                         ? ([&]() {
                    ++(vlSymsp->__Vcoverage[196]);
                }(), (vlSelfRef.addsub__DOT__a_i + vlSelfRef.addsub__DOT__b_i))
                                         : ([&]() {
                    ++(vlSymsp->__Vcoverage[197]);
                }(), (vlSelfRef.addsub__DOT__a_i - vlSelfRef.addsub__DOT__b_i)));
        if (vlSelfRef.addsub__DOT__op_i) {
            ++(vlSymsp->__Vcoverage[194]);
        }
        if ((1U & (~ (IData)(vlSelfRef.addsub__DOT__op_i)))) {
            ++(vlSymsp->__Vcoverage[195]);
        }
        ++(vlSymsp->__Vcoverage[198]);
        if ((vlSelfRef.addsub__DOT__res_o ^ vlSelfRef.addsub__DOT____Vtogcov__res_o)) {
            VL_COV_TOGGLE_CHG_ST_I(32, vlSymsp->__Vcoverage + 130, vlSelfRef.addsub__DOT__res_o, vlSelfRef.addsub__DOT____Vtogcov__res_o);
            vlSelfRef.addsub__DOT____Vtogcov__res_o
                = vlSelfRef.addsub__DOT__res_o;
        }
        vlSelfRef.res_o = vlSelfRef.addsub__DOT__res_o;
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

VL_ATTR_COLD void Vtop___024root___ctor_var_reset(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ctor_var_reset\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->a_i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12042468572559684522ull);
    vlSelf->b_i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15122488259574687123ull);
    vlSelf->op_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4613435319122321902ull);
    vlSelf->res_o = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4084131876098369058ull);
    vlSelf->addsub__DOT__a_i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7327642776334411292ull);
    vlSelf->addsub__DOT__b_i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7751962963933687331ull);
    vlSelf->addsub__DOT__op_i = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7497574108576201415ull);
    vlSelf->addsub__DOT__res_o = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4083192009384270033ull);
    vlSelf->addsub__DOT____Vtogcov__a_i = 0;
    vlSelf->addsub__DOT____Vtogcov__b_i = 0;
    vlSelf->addsub__DOT____Vtogcov__op_i = 0;
    vlSelf->addsub__DOT____Vtogcov__res_o = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
}

VL_ATTR_COLD void Vtop___024root___configure_coverage(Vtop___024root* vlSelf, bool first) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___configure_coverage\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    (void)first;  // Prevent unused variable warning
    vlSelf->__vlCoverToggleInsert(0, 31, 1, &(vlSymsp->__Vcoverage[0]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 2, 24, ".addsub", "v_toggle/addsub", "a_i");
    vlSelf->__vlCoverToggleInsert(0, 31, 1, &(vlSymsp->__Vcoverage[64]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 3, 24, ".addsub", "v_toggle/addsub", "b_i");
    vlSelf->__vlCoverToggleInsert(0, 0, 0, &(vlSymsp->__Vcoverage[128]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 5, 17, ".addsub", "v_toggle/addsub", "op_i");
    vlSelf->__vlCoverToggleInsert(0, 31, 1, &(vlSymsp->__Vcoverage[130]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 7, 25, ".addsub", "v_toggle/addsub", "res_o");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[194]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 11, 13, ".addsub", "v_expr/addsub", "(op_i==1) => 1", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[195]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 11, 13, ".addsub", "v_expr/addsub", "(op_i==0) => 0", "");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[196]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 11, 24, ".addsub", "v_branch/addsub", "cond_then", "11");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[197]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 11, 25, ".addsub", "v_branch/addsub", "cond_else", "11");
    vlSelf->__vlCoverInsert(&(vlSymsp->__Vcoverage[198]), first, "/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 10, 3, ".addsub", "v_line/addsub", "block", "10-11");
}
