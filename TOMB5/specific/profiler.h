#pragma once
#include "../global/vars.h"

void inject_profiler(bool replace);

void mDrawTriangle(long x1, long y1, long x2, long y2, long x3, long y3, long c0, long c1, long c2);
void mAddProfilerEvent(long c);
void mResetTime();
__int64 mGetTime();
void mInitTimer();
long mSync(long s);
void mInitDrawProfiler(long x, long y, long f, long w);
void mDrawProfiler(double sync, long x, long y);
