# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

from dataclasses import dataclass
from enum import IntEnum


class operator_type_t(IntEnum):
    ADD = 0b000
    SLL = 0b001
    SLT = 0b010
    SLTU = 0b011
    XOR = 0b100
    SRA = 0b101
    OR = 0b110
    AND = 0b111


@dataclass(frozen=True)
class operator_t:
    word_mode: bool
    operator_type: operator_type_t
    complement: bool


class alu_operation_t(IntEnum):
    ADD = 0
    SUB = 1
    AND = 2
    OR = 3
    XOR = 4
    SLL = 5
    SRL = 6
    SRA = 7
    SLT = 8
    SLTU = 9
    ADDW = 10
    SUBW = 11
    SLLW = 12
    SRLW = 13
    SRAW = 14


def _mask(width: int) -> int:
    return (1 << width) - 1


def to_unsigned(x: int, bits: int) -> int:
    return x & _mask(bits)


def to_signed(x: int, bits: int) -> int:
    x &= _mask(bits)
    sign_bit: int = 1 << (bits - 1)

    if x & sign_bit:
        return x - (1 << bits)

    return x


def u32(x: int) -> int:
    return to_unsigned(x, 32)


def s32(x: int) -> int:
    return to_signed(x, 32)


def u64(x: int) -> int:
    return to_unsigned(x, 64)


def s64(x: int) -> int:
    return to_signed(x, 64)


def decode_bits(code: int) -> operator_t:
    code &= 0x1F
    word_mode = bool((code >> 4) & 0x1)
    op3 = operator_type_t((code >> 1) & 0x7)
    complement = bool(code & 0x1)
    return operator_t(word_mode=word_mode, operator_type=op3, complement=complement)


def decode_alu_operation(operator: operator_t) -> alu_operation_t:
    op_type = operator.operator_type
    complement = operator.complement
    word_mode = operator.word_mode

    if op_type == operator_type_t.ADD:
        if word_mode:
            return alu_operation_t.SUBW if complement else alu_operation_t.ADDW
        else:
            return alu_operation_t.SUB if complement else alu_operation_t.ADD
    elif op_type == operator_type_t.SLL:
        if word_mode:
            return alu_operation_t.SRLW if complement else alu_operation_t.SLLW
        else:
            return alu_operation_t.SRL if complement else alu_operation_t.SLL
    elif op_type == operator_type_t.SLT:
        return alu_operation_t.SLT
    elif op_type == operator_type_t.SLTU:
        return alu_operation_t.SLTU
    elif op_type == operator_type_t.XOR:
        return alu_operation_t.XOR
    elif op_type == operator_type_t.SRA:
        return alu_operation_t.SRAW if word_mode else alu_operation_t.SRA
    elif op_type == operator_type_t.OR:
        return alu_operation_t.OR
    elif op_type == operator_type_t.AND:
        return alu_operation_t.AND
    else:
        return alu_operation_t.ADD


def has_w_variant(op: alu_operation_t) -> bool:
    return op in {
        alu_operation_t.ADDW,
        alu_operation_t.SUBW,
        alu_operation_t.SLLW,
        alu_operation_t.SRLW,
        alu_operation_t.SRAW,
    }


def model_alu(a: int, b: int, operator_raw: int) -> tuple[int, int]:
    """Golden reference model for the RV64 ALU."""

    operator = decode_bits(operator_raw)
    alu_op = decode_alu_operation(operator)
    word_mode = operator.word_mode and has_w_variant(alu_op)

    if word_mode:
        a_u = u32(a)
        b_u = u32(b)
        shift = b_u & 0x1F
    else:
        a_u = u64(a)
        b_u = u64(b)
        shift = b_u & 0x3F

    match alu_op:
        case alu_operation_t.ADD:
            result = u64(a_u + b_u)

        case alu_operation_t.SUB:
            result = u64(a_u - b_u)

        case alu_operation_t.AND:
            result = u64(a_u & b_u)

        case alu_operation_t.OR:
            result = u64(a_u | b_u)

        case alu_operation_t.XOR:
            result = u64(a_u ^ b_u)

        case alu_operation_t.SLL:
            result = u64(a_u << shift)

        case alu_operation_t.SRL:
            result = u64(a_u >> shift)

        case alu_operation_t.SRA:
            result = u64(s64(a_u) >> shift)

        case alu_operation_t.SLT:
            result = int(s64(a_u) < s64(b_u))

        case alu_operation_t.SLTU:
            result = int(a_u < b_u)

        case alu_operation_t.ADDW:
            result = s64(s32(a_u + b_u))

        case alu_operation_t.SUBW:
            result = s64(s32(a_u - b_u))

        case alu_operation_t.SLLW:
            result = s64(s32(a_u << shift))

        case alu_operation_t.SRLW:
            result = s64(s32(a_u >> shift))

        case alu_operation_t.SRAW:
            result = s64(s32(s32(a_u) >> shift))

    zero = int(u64(result) == 0)

    return u64(result), zero
