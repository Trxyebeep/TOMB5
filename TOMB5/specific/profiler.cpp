#include "../tomb5/pch.h"
#include "profiler.h"

static __int64 counter;

void mDrawTriangle(long x1, long y1, long x2, long y2, long x3, long y3, long c0, long c1, long c2)
{

}

void mAddProfilerEvent(long c)
{

}

void mResetTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

void inject_profiler(bool replace)
{
	INJECT(0x004BD3A0, mDrawTriangle, replace);
	INJECT(0x004BD590, mAddProfilerEvent, replace);
	INJECT(0x004BD3C0, mResetTime, replace);
}
