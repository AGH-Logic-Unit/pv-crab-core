// Copyright (c) 2026 AGH University of Krakow
// Developed by AGH Logic Unit
// SPDX-License-Identifier: Apache-2.0

package defs_pkg;

  // Default Global Core Parameters
  parameter int DEFAULT_XLEN = 64;
  parameter int DEFAULT_FLEN = 64;
  parameter int DEFAULT_RB_SIZE = 16;
  parameter int DEFAULT_TAG_WIDTH = $clog2(DEFAULT_RB_SIZE);
  parameter int DEFAULT_PA_LEN = 56;
  parameter int DEFAULT_VA_LEN = 39;
  parameter int DEFAULT_CACHE_LINE_BYTES = 64;
  parameter int DEFAULT_PMP_ENTRIES = 16;

  // Execution Pipeline Tracking Header
  typedef struct packed {logic [DEFAULT_TAG_WIDTH-1:0] tag;} exe_headers_t;

  // Privilege Levels
  typedef enum logic [1:0] {
    PRIV_USER       = 2'b00,
    PRIV_SUPERVISOR = 2'b01,
    PRIV_MACHINE    = 2'b11
  } priv_mode_t;

  // CSR Operation Type
  typedef enum logic [1:0] {
    CSR_OP_NONE  = 2'b00,
    CSR_OP_WRITE = 2'b01,
    CSR_OP_SET   = 2'b10,
    CSR_OP_CLEAR = 2'b11
  } csr_op_t;

  // Instruction Execution Class
  typedef enum logic [2:0] {
    OP_CLASS_NORMAL = 3'b000,
    OP_CLASS_CSR    = 3'b001,
    OP_CLASS_FPU    = 3'b010,
    OP_CLASS_STORE  = 3'b011,
    OP_CLASS_SYSTEM = 3'b100
  } op_class_t;

  typedef struct packed {
    logic [8:0] unused;
    logic [4:0] fflags;
  } rb_meta_fp_t;

  typedef struct packed {
    logic [6:0] unused;
    logic       is_interrupt;
    logic [5:0] cause;
  } rb_meta_trap_t;

  typedef struct packed {
    csr_op_t     op;
    logic [11:0] addr;
  } rb_meta_csr_t;

  // rb_meta_t (Packed Metadata Union)
  typedef union packed {
    rb_meta_fp_t   fp;
    rb_meta_trap_t trap;
    rb_meta_csr_t  csr;
  } rb_meta_t;

endpackage : defs_pkg
