#pragma once

#include <cstdint>
#include <cstdio>

#include "def.h"

enum operation_type {
  OP_Invalid,
  OP_MOV,
  OP_PUSH,
  OP_POP,
  OP_XCHG,
  OP_IN,
  OP_OUT,
  OP_XLAT,
  OP_LEA,
  OP_LDS,
  OP_LES,
  OP_LAHF,
  OP_SAHF,
  OP_PUSHF,
  OP_POPF,
  OP_ADD,
  OP_ADC,
  OP_INC,
  OP_AAA,
  OP_DAA,
  OP_SUB,
  OP_SBB,
  OP_DEC,
  OP_NEG,
  OP_CMP,
  OP_AAS,
  OP_DAS,
  OP_MUL,
  OP_IMUL,
  OP_AAM,
  OP_DIV,
  OP_IDIV,
  OP_AAD,
  OP_CBW,
  OP_CWD,
  OP_NOT,
  OP_SHL,
  OP_SAL,
  OP_SHR,
  OP_SAR,
  OP_ROL,
  OP_ROR,
  OP_RCL,
  OP_RCR,
  OP_AND,
  OP_TEST,
  OP_OR,
  OP_XOR,
  OP_REP,
  OP_MOVS,
  OP_CMPS,
  OP_SCAS,
  OP_LODS,
  OP_STOS,  // STDS was a misprint
  OP_CALL,
  OP_JMP,
  OP_RET,
  OP_JE,
  OP_JZ,
  OP_JL,
  OP_JNGE,
  OP_JLE,
  OP_JNG,
  OP_JB,
  OP_JNAE,
  OP_JBE,
  OP_JNA,
  OP_JP,
  OP_JPE,
  OP_JO,
  OP_JS,
  OP_JNE,
  OP_JNZ,
  OP_JNL,
  OP_JGE,
  OP_JNLE,
  OP_JG,
  OP_JNB,
  OP_JAE,
  OP_JNBE,
  OP_JA,
  OP_JNP,
  OP_JPO,
  OP_JNO,
  OP_JNS,
  OP_LOOP,
  OP_LOOPZ,
  OP_LOOPE,
  OP_LOOPNZ,
  OP_LOOPNE,
  OP_JCXZ,
  OP_INT,
  OP_INTO,
  OP_IRET,
  OP_CLC,
  OP_CMT,
  OP_STC,
  OP_CLD,
  OP_STD,
  OP_CLI,
  OP_STI,
  OP_HLT,
  OP_WAIT,
  OP_ESC,
  OP_LOCK,
  OP_SEGMENT
};

enum operand_type {
  O_None,
  O_Immediate,
  O_Register,
  O_EffectiveAddress1,
  O_EffectiveAddress1D8 = O_EffectiveAddress1 + 1,
  O_EffectiveAddress1D16 = O_EffectiveAddress1 + 2,
  O_EffectiveAddress2,
  O_EffectiveAddress2D8 = O_EffectiveAddress2 + 1,
  O_EffectiveAddress2D16 = O_EffectiveAddress2 + 2,
  O_DirectAddress,
};

// W | REG
enum reg {
  REG_AL = 0b0000,
  REG_CL = 0b0001,
  REG_DL = 0b0010,
  REG_BL = 0b0011,
  REG_AH = 0b0100,
  REG_CH = 0b0101,
  REG_DH = 0b0110,
  REG_BH = 0b0111,
  REG_AX = 0b1000,
  REG_CX = 0b1001,
  REG_DX = 0b1010,
  REG_BX = 0b1011,
  REG_SP = 0b1100,
  REG_BP = 0b1101,
  REG_SI = 0b1110,
  REG_DI = 0b1111,
};

struct effective_address {
  reg Register1;
  reg Register2;
  union {
    u8 d8;
    u16 d16;
  };
};

struct operand {
  operand_type Type;
  union {
    reg Register;
    effective_address eaddress;
    u16 address;
    u16 immediate;
  };
};

struct instruction {
  operation_type Type;
  operand operands[2];  // [dst, src]
  uint8_t size;
};
