#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "FNV-1a.h"
#include "PlatformMetrics.h"
typedef uint64_t u64;
typedef uint32_t u32;
typedef double f64;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#ifndef PROFILER
#define PROFILER 1
#endif

#if PROFILER

struct profile_anchor {
  u64 TSCElapsedExclusive;
  u64 TSCElapsedInclusive;
  u64 HitCount;
  char const *Label;
};

constexpr uint32_t profileranchorcount = (65536 * 10) + 1;

extern profile_anchor GlobalProfilerAnchors[profileranchorcount];
extern u32 GlobalProfilerParent;

struct profile_block {
  profile_block(char const *Label_, u32 AnchorIndex_) {
    ParentIndex = GlobalProfilerParent;

    AnchorIndex = AnchorIndex_;
    Label = Label_;

    profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
    OldTSCElapsedInclusive = Anchor->TSCElapsedInclusive;

    GlobalProfilerParent = AnchorIndex;
    StartTSC = ReadCPUTimer();
  }

  ~profile_block(void) {
    u64 Elapsed = ReadCPUTimer() - StartTSC;
    GlobalProfilerParent = ParentIndex;

    profile_anchor *Parent = GlobalProfilerAnchors + ParentIndex;
    profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;

    Parent->TSCElapsedExclusive -= Elapsed;
    Anchor->TSCElapsedExclusive += Elapsed;
    Anchor->TSCElapsedInclusive = OldTSCElapsedInclusive + Elapsed;
    ++Anchor->HitCount;

    assert(Anchor->Label == nullptr || strcmp(Anchor->Label, Label) == 0);
    Anchor->Label = Label;
  }

  char const *Label;
  u64 OldTSCElapsedInclusive;
  u64 StartTSC;
  u32 ParentIndex;
  u32 AnchorIndex;
};

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)
#define TimeBlock(Name)                           \
  profile_block NameConcat(Block, __LINE__)(Name, \
                                            (GetFileID * 10) + __COUNTER__);

// ==========================================================================
#define TimeBlockSafe(Name)                        \
  static const int __counter__count = __COUNTER__; \
  assert(__counter__count < 10);                   \
  profile_block NameConcat(Block, __LINE__)(       \
      Name, (GetFileID * 10) + __counter__count);

#define TimeFunctionSafe TimeBlockSafe(__func__)

static void PrintTimeElapsed(u64 TotalTSCElapsed, profile_anchor *Anchor) {
  f64 Percent =
      100.0 * ((f64)Anchor->TSCElapsedExclusive / (f64)TotalTSCElapsed);
  printf("  %s[%llu]: %llu (%.2f%%", Anchor->Label, Anchor->HitCount,
         Anchor->TSCElapsedExclusive, Percent);
  if (Anchor->TSCElapsedInclusive != Anchor->TSCElapsedExclusive) {
    f64 PercentWithChildren =
        100.0 * ((f64)Anchor->TSCElapsedInclusive / (f64)TotalTSCElapsed);
    printf(", %.2f%% w/children", PercentWithChildren);
  }
  printf(")\n");
}

static void PrintAnchorData(u64 TotalCPUElapsed) {
  for (u32 AnchorIndex = 0; AnchorIndex < profileranchorcount; ++AnchorIndex) {
    profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
    if (Anchor->TSCElapsedInclusive) {
      PrintTimeElapsed(TotalCPUElapsed, Anchor);
    }
  }
}

#else

#define TimeBlock(...)
#define PrintAnchorData(...)
#define ProfilerEndOfCompilationUnit

#endif

struct profiler {
  u64 StartTSC;
  u64 EndTSC;
};
static profiler GlobalProfiler;

#define TimeFunction TimeBlock(__func__)

static void BeginProfile(void) { GlobalProfiler.StartTSC = ReadCPUTimer(); }

static void EndAndPrintProfile() {
  GlobalProfiler.EndTSC = ReadCPUTimer();
  u64 CPUFreq = EstimateCPUTimerFreq();

  u64 TotalCPUElapsed = GlobalProfiler.EndTSC - GlobalProfiler.StartTSC;

  if (CPUFreq) {
    printf("\nTotal time: %0.4fms (CPU freq %llu)\n",
           1000.0 * (f64)TotalCPUElapsed / (f64)CPUFreq, CPUFreq);
  }

  PrintAnchorData(TotalCPUElapsed);
}
