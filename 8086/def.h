#pragma once

#include <cstdint>
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

struct Buffer {
  u8 *buf;
  u32 len;
};

struct Decoder {
  Buffer buf;
  u32 at;
};
