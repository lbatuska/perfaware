#pragma once

#include <array>
#include <cstdint>

// We will use FNV-1a and __FILE_NAME__ to generate a per file unique ID to
// solve the instrumentation code's limitation with __COUNTER__
// we will allocate 10 * 2^16 sized array so each file can have at max 10
// profiling blocks

constexpr uint32_t FNV_1a(char const* str) {
  uint32_t FNV_offset_basis = 0x811c9dc5;  //   2166136261
  uint32_t FNV_prime = 0x01000193;         //  16777619
  uint32_t hash = FNV_offset_basis;
  while (*str) {
    hash ^= static_cast<uint8_t>(*str);
    hash *= FNV_prime;
    ++str;
  }
  return hash;
}

constexpr uint16_t hash_to_16bit(uint32_t hash) {
  return static_cast<uint16_t>(
      hash & 0xFFFF);  // Mask the lower 16 bits aka modulo 65536
}

#define GetFileID hash_to_16bit(FNV_1a(__FILE_NAME__))

#define FileIDString                                    \
  {static_cast<char>('0' + ((GetFileID / 10000) % 10)), \
   static_cast<char>('0' + ((GetFileID / 1000) % 10)),  \
   static_cast<char>('0' + ((GetFileID / 100) % 10)),   \
   static_cast<char>('0' + ((GetFileID / 10) % 10)),    \
   static_cast<char>('0' + (GetFileID % 10)),           \
   '\0'}
