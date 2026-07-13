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
        vlSelfRef.addsub__DOT____Vtogcov__op_i = vlSelfRef.addsub__DOT__op_i;
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
        vlSelfRef.addsub__DOT____Vtogcov__res_o = vlSelfRef.addsub__DOT__res_o;
    }
    vlSelfRef.res_o = vlSelfRef.addsub__DOT__res_o;
}

void Vtop___024root___eval_ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
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

void Vtop___024root___eval(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("/workspaces/pv-crab-core/rtl/addsub/addsub.sv", 1, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 100 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vtop___024root___eval_phase__ico(vlSelf);
        vlSelfRef.__VicoFirstIteration = 0U;
    } while (vlSelfRef.__VicoPhaseResult);
}

#ifdef VL_DEBUG
void Vtop___024root___eval_debug_assertions(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_debug_assertions\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.op_i & 0xfeU)))) {
        Verilated::overWidthError("op_i");
    }
}
#endif  // VL_DEBUG
