// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

// Parameter definitions for Vtop_sw_pp_divider_pkg
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_DIV;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_DIVU;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_REM;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_REMU;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_DIVW;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_DIVUW;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_REMW;
constexpr CData/*2:0*/ Vtop_sw_pp_divider_pkg::OP_REMUW;


void Vtop_sw_pp_divider_pkg___ctor_var_reset(Vtop_sw_pp_divider_pkg* vlSelf);

Vtop_sw_pp_divider_pkg::Vtop_sw_pp_divider_pkg() = default;
Vtop_sw_pp_divider_pkg::~Vtop_sw_pp_divider_pkg() = default;

void Vtop_sw_pp_divider_pkg::ctor(Vtop__Syms* symsp, const char* namep) {
    vlSymsp = symsp;
    vlNamep = strdup(Verilated::catName(vlSymsp->name(), namep));
    // Reset structure values
    Vtop_sw_pp_divider_pkg___ctor_var_reset(this);
}

void Vtop_sw_pp_divider_pkg___configure_coverage(Vtop_sw_pp_divider_pkg* vlSelf, bool first);

void Vtop_sw_pp_divider_pkg::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
    Vtop_sw_pp_divider_pkg___configure_coverage(this, first);
}

void Vtop_sw_pp_divider_pkg::dtor() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}

// Coverage
void Vtop_sw_pp_divider_pkg::__vlCoverInsert(uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
    const char* hierp, const char* pagep, const char* commentp, const char* linescovp) {
    uint32_t* count32p = countp;
    static uint32_t fake_zero_count = 0;
    std::string fullhier = std::string{vlNamep} + hierp;
    if (!fullhier.empty() && fullhier[0] == '.') fullhier = fullhier.substr(1);
    if (!enable) count32p = &fake_zero_count;
    *count32p = 0;
    VL_COVER_INSERT(vlSymsp->_vm_contextp__->coveragep(), vlNamep, count32p,  "filename",filenamep,  "lineno",lineno,  "column",column,
        "hier",fullhier,  "page",pagep,  "comment",commentp,  (linescovp[0] ? "linescov" : ""), linescovp);
}

// Toggle Coverage
void Vtop_sw_pp_divider_pkg::__vlCoverToggleInsert(int begin, int end, bool ranged, uint32_t* countp, bool enable, const char* filenamep, int lineno, int column,
    const char* hierp, const char* pagep, const char* commentp) {
    int step = (end >= begin) ? 1 : -1;
    for (int i = begin; i != end + step; i += step) {
        for (int j = 0; j < 2; j++) {
            uint32_t* count32p = countp;
            static uint32_t fake_zero_count = 0;
            std::string fullhier = std::string{vlNamep} + hierp;
            if (!fullhier.empty() && fullhier[0] == '.') fullhier = fullhier.substr(1);
            std::string commentWithIndex = commentp;
            if (ranged) commentWithIndex += '[' + std::to_string(i) + ']';
            commentWithIndex += j ? ":0->1" : ":1->0";
            if (!enable) count32p = &fake_zero_count;
            *count32p = 0;
            VL_COVER_INSERT(vlSymsp->_vm_contextp__->coveragep(), vlNamep, count32p,  "filename",filenamep,  "lineno",lineno,  "column",column,
                "hier",fullhier,  "page",pagep,  "comment",commentWithIndex.c_str(),  "", "");
            ++countp;
        }
    }
}
