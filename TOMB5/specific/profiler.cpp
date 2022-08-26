#include "../tomb5/pch.h"
#include "profiler.h"

void mDrawTriangle()
{

}

void mAddProfilerEvent()
{

}

void inject_profiler(bool replace)
{
	INJECT(0x004BD3A0, mDrawTriangle, replace);
	INJECT(0x004BD590, mAddProfilerEvent, replace);
}
