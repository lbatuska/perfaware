#pragma once

#include <sys/time.h>
#include <time.h>
#include <x86intrin.h>

#include <cstdint>
#include <cstdio>

static uint64_t GetOSTimerFreq(void) {
  struct timespec res;
  if (clock_getres(CLOCK_MONOTONIC_RAW, &res) == 0) {
    return 1000000000 / res.tv_nsec;
  }
  return 0;
}

static uint64_t ReadOSTimer(void) {
  struct timeval Value;
  gettimeofday(&Value, 0);

  uint64_t Result =
      GetOSTimerFreq() * (uint64_t)Value.tv_sec + (uint64_t)Value.tv_usec;
  return Result;
}

inline uint64_t ReadCPUTimer(void) { return __rdtsc(); }

typedef uint64_t u64;
inline u64 EstimateCPUTimerFreq(void) {
  u64 MillisecondsToWait = 100;
  u64 OSFreq = GetOSTimerFreq();

  u64 CPUStart = ReadCPUTimer();
  u64 OSStart = ReadOSTimer();
  u64 OSEnd = 0;
  u64 OSElapsed = 0;
  u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
  while (OSElapsed < OSWaitTime) {
    OSEnd = ReadOSTimer();
    OSElapsed = OSEnd - OSStart;
  }

  u64 CPUEnd = ReadCPUTimer();
  u64 CPUElapsed = CPUEnd - CPUStart;

  u64 CPUFreq = 0;
  if (OSElapsed) {
    CPUFreq = OSFreq * CPUElapsed / OSElapsed;
  }

  return CPUFreq;
}
