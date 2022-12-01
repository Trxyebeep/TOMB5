#include "../tomb5/pch.h"
#include "profiler.h"

void mDrawTriangle(long x1, long y1, long x2, long y2, long x3, long y3, long c0, long c1, long c2)
{

}

void mAddProfilerEvent(long c)
{

}

void inject_profiler(bool replace)
{
	INJECT(0x004BD3A0, mDrawTriangle, replace);
	INJECT(0x004BD590, mAddProfilerEvent, replace);
}
