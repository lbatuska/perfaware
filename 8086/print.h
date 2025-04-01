#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "8086.h"
#include "def.h"

inline void PrintBits(u8 c) {
  u8 a = 128;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 64;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 32;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 16;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 8;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 4;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 2;
  printf("%s", ((c & a) == a) ? "1" : "0");
  a = 1;
  printf("%s", ((c & a) == a) ? "1" : "0");
}

inline auto constexpr RegisterName(reg r) {
  switch (r) {
    case REG_AL:
      return "al";
    case REG_AH:
      return "ah";
    case REG_AX:
      return "ax";
    case REG_BL:
      return "bl";
    case REG_BH:
      return "bh";
    case REG_BX:
      return "bx";
    case REG_CL:
      return "cl";
    case REG_CH:
      return "ch";
    case REG_CX:
      return "cx";
    case REG_DL:
      return "dl";
    case REG_DH:
      return "dh";
    case REG_DX:
      return "dx";
    case REG_BP:
      return "bp";
    case REG_SP:
      return "sp";
    case REG_SI:
      return "si";
    case REG_DI:
      return "di";
    default:
      return "??";
  }
}

inline auto OperandStr(operand op) {
  char buffer[32];
  switch (op.Type) {
    case O_Register: {
      auto a = RegisterName(op.Register);
      return strdup(a);
    }
    case O_Immediate:
      snprintf(buffer, 32, "[%hu]", op.immediate);
      return strdup(buffer);
    case O_DirectAddress:
      snprintf(buffer, 32, "[%hu]", op.address);
      return strdup(buffer);
    case O_EffectiveAddress1:
      snprintf(buffer, 32, "[%s]", RegisterName(op.eaddress.Register1));
      return strdup(buffer);
    case O_EffectiveAddress1D8:
      snprintf(buffer, 32, "[%s+%hhu]", RegisterName(op.eaddress.Register1),
               op.eaddress.d8);
      return strdup(buffer);
    case O_EffectiveAddress1D16:
      snprintf(buffer, 32, "[%s+%hu]", RegisterName(op.eaddress.Register1),
               op.eaddress.d16);
      return strdup(buffer);
    case O_EffectiveAddress2:
      snprintf(buffer, 32, "[%s+%s]", RegisterName(op.eaddress.Register1),
               RegisterName(op.eaddress.Register2));
      return strdup(buffer);
    case O_EffectiveAddress2D8:
      snprintf(buffer, 32, "[%s+%s+%hhu]", RegisterName(op.eaddress.Register1),
               RegisterName(op.eaddress.Register2), op.eaddress.d8);
      return strdup(buffer);
    case O_EffectiveAddress2D16:
      snprintf(buffer, 32, "[%s+%s+%hu]", RegisterName(op.eaddress.Register1),
               RegisterName(op.eaddress.Register2), op.eaddress.d16);
      return strdup(buffer);
    default:
      return strdup("");
  }
}

inline void PrintInst(instruction inst) {
  switch (inst.Type) {
    case OP_MOV: {
      auto op1 = OperandStr(inst.operands[0]);
      auto op2 = OperandStr(inst.operands[1]);
      printf("mov %s,%s\n", op1, op2);
      free(op1);
      free(op2);
      break;
    }
    default:
      break;
  }
}
