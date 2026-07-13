// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtop.h for the primary calling header

#ifndef VERILATED_VTOP_SW_PP_DIVIDER_PKG_H_
#define VERILATED_VTOP_SW_PP_DIVIDER_PKG_H_  // guard

#include "verilated.h"
#include "verilated_cov.h"


class Vtop__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtop_sw_pp_divider_pkg final {
  public:

    // INTERNAL VARIABLES
    Vtop__Syms* vlSymsp;
    const char* vlNamep;

    // PARAMETERS
    static constexpr CData/*2:0*/ OP_DIV = 0U;
    static constexpr CData/*2:0*/ OP_DIVU = 1U;
    static constexpr CData/*2:0*/ OP_REM = 2U;
    static constexpr CData/*2:0*/ OP_REMU = 3U;
    static constexpr CData/*2:0*/ OP_DIVW = 4U;
    static constexpr CData/*2:0*/ OP_DIVUW = 5U;
    static constexpr CData/*2:0*/ OP_REMW = 6U;
    static constexpr CData/*2:0*/ OP_REMUW = 7U;

    // CONSTRUCTORS
    Vtop_sw_pp_divider_pkg();
    ~Vtop_sw_pp_divider_pkg();
    void ctor(Vtop__Syms* symsp, const char* namep);
    void dtor();
    VL_UNCOPYABLE(Vtop_sw_pp_divider_pkg);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
    void __vlCoverInsert(uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
        const char* hierp, const char* pagep, const char* commentp, const char* linescovp);
    void __vlCoverToggleInsert(int begin, int end, bool ranged, uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
        const char* hierp, const char* pagep, const char* commentp);
};


#endif  // guard
