#include "../include/Profiler.h"

profile_anchor GlobalProfilerAnchors[profileranchorcount] = {};

uint32_t GlobalProfilerParent = {profileranchorcount - 1};
