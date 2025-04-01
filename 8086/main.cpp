#include <fcntl.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "8086.h"
#include "def.h"
#include "print.h"

u8 inline PeekByte(Decoder &d) { return d.buf.buf[d.at]; }
void inline StepByte(Decoder &d, u8 bytecount = 1) { d.at += bytecount; }
u8 inline ReadByte(Decoder &d) { return d.buf.buf[d.at++]; }

Buffer ReadFile(char const *file) {
  struct stat buffer;
  if (stat(file, &buffer) != 0) {
    fprintf(stderr, "File not found! %s\n", file);
    exit(1);
  }
  FILE *ffd = fopen(file, "r");
  fseek(ffd, 0, SEEK_END);
  Buffer buf;
  buf.len = ftell(ffd) + 1;
  rewind(ffd);
  buf.buf = (u8 *)malloc(buf.len);
  buf.buf[buf.len - 1] = '\0';
  fread((void *)buf.buf, 1, buf.len - 1, ffd);
  fclose(ffd);
  return buf;
}

inline u8 IsWide(u8 c) { return c & 0b1; }

inline void GetEffectiveRegisters(u8 rm, operand &op) {
  switch (rm) {
    case 0b000:
      op.eaddress.Register1 = REG_BX;
      op.eaddress.Register2 = REG_SI;
      op.Type = O_EffectiveAddress2;
      break;
    case 0b001:
      op.eaddress.Register1 = REG_BX;
      op.eaddress.Register2 = REG_DI;
      op.Type = O_EffectiveAddress2;
      break;
    case 0b010:
      op.eaddress.Register1 = REG_BP;
      op.eaddress.Register2 = REG_SI;
      op.Type = O_EffectiveAddress2;
      break;
    case 0b011:
      op.eaddress.Register1 = REG_BP;
      op.eaddress.Register2 = REG_DI;
      op.Type = O_EffectiveAddress2;
      break;
    case 0b100:
      op.eaddress.Register1 = REG_SI;
      op.Type = O_EffectiveAddress1;
      break;
    case 0b101:
      op.eaddress.Register1 = REG_DI;
      op.Type = O_EffectiveAddress1;
      break;
    case 0b110:
      op.eaddress.Register1 = REG_BP;
      op.Type = O_EffectiveAddress1;
      break;
    case 0b111:
      op.eaddress.Register1 = REG_BX;
      op.Type = O_EffectiveAddress1;
      break;
  }
}

inline operand OperandRM(Decoder &decoder, u8 isWide) {
  u8 byte = ReadByte(decoder);
  u8 mod = byte >> 6;
  u8 rm = byte & 0b111;
  operand op{};
  switch (mod) {
    case 0b00:
      // NOTE: Memory mode no displacement, except rm=110
      if (rm == 0b110) {
        // NOTE: 16 bit displacement
        u8 lo = ReadByte(decoder);
        u8 hi = ReadByte(decoder);
        op.Type = O_DirectAddress;
        op.address = ((u16)hi << 8) | lo;
        return op;
      }
      GetEffectiveRegisters(rm, op);
      break;
    case 0b01:
      // NOTE: Memory mode 8 bit displacement
      GetEffectiveRegisters(rm, op);
      op.eaddress.d8 = ReadByte(decoder);
      op.Type = (operand_type)(op.Type + 1);
      break;
    case 0b10: {
      // NOTE: Memory mode 16 bit displacement
      GetEffectiveRegisters(rm, op);
      u8 lo = ReadByte(decoder);
      u8 hi = ReadByte(decoder);
      op.Type = (operand_type)(op.Type + 2);
      op.eaddress.d16 = ((u16)hi << 8) | lo;
      break;
    }
    case 0b11:
      // NOTE: Register mode no displacement
      op.Type = O_Register;
      op.Register = (reg)(isWide << 3 | (rm & 0b111));
      break;
  }
  return op;
}

instruction GetMov(Decoder &decoder) {
  instruction inst{};
  inst.Type = OP_Invalid;
  u8 isWide = 0;
  u8 byte = PeekByte(decoder);
  if ((byte >> 2) == 0b00100010) {
    StepByte(decoder);
    inst.Type = OP_MOV;
    u8 direction = byte & 0b10;
    isWide = IsWide(byte);
    byte = PeekByte(decoder);
    operand Reg;
    Reg.Type = O_Register;
    Reg.Register = (reg)(isWide << 3 | ((byte >> 3) & 0b111));
    operand rm = OperandRM(decoder, isWide);
    inst.operands[0] = direction ? Reg : rm;
    inst.operands[1] = direction ? rm : Reg;
  } else if ((byte >> 2) == 0b101000) {
    inst.Type = OP_MOV;
    operand Reg;
    Reg.Type = O_Register;
    Reg.Register = REG_AL;
    byte = ReadByte(decoder);
    u8 lo = ReadByte(decoder);
    operand op{};
    op.Type = O_Immediate;
    u8 hi = 0;
    if ((byte & 0b10) == 0b10) {
      if (IsWide(byte)) {
        hi = ReadByte(decoder);

        Reg.Register = REG_AX;
      } else {
        ReadByte(decoder);
      }

      inst.operands[1] = Reg;
      op.address = ((u16)hi << 8) | lo;
      inst.operands[0] = op;

    } else if ((byte & 0b00) == 0b00) {
      if (IsWide(byte)) {
        hi = ReadByte(decoder);

        Reg.Register = REG_AX;
      } else {
        ReadByte(decoder);
      }

      inst.operands[0] = Reg;
      op.address = ((u16)hi << 8) | lo;
      inst.operands[1] = op;
    }
  } else {
    printf("Nothing matched with: ");
    PrintBits(byte);
    printf("\n");
    ReadByte(decoder);
  }
  return inst;
}

instruction NextInstruction(Decoder &decoder) {
  instruction i;
  i.Type = OP_Invalid;
  i = GetMov(decoder);
  if (i.Type != OP_Invalid) {
    return i;
  }

  return i;
}

int main(int argc, char *argv[]) {
  printf("bits 16\n");
  Decoder decoder{};
  decoder.buf = ReadFile("./1");

  while (decoder.at < decoder.buf.len - 1) {
    auto inst = NextInstruction(decoder);
    PrintInst(inst);
  }
  return 0;
}
