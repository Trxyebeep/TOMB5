#pragma once
#include "../global/vars.h"

void inject_profiler(bool replace);

void mDrawTriangle(long x1, long y1, long x2, long y2, long x3, long y3, long c0, long c1, long c2);
void mAddProfilerEvent(long c);
