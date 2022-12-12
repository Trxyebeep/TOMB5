#include "../tomb5/pch.h"
#include "profiler.h"
#include "function_table.h"
#include "dxshell.h"

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

__int64 mGetTime()
{
	__int64 pc;

	QueryPerformanceCounter((LARGE_INTEGER*)&pc);
	return pc - counter;
}

void mInitTimer()
{
	__int64 fq;

	QueryPerformanceFrequency((LARGE_INTEGER*)&fq);
	t_frequency = fq / 60;
	mResetTime();
}

long mSync(long s)
{
	__int64 pc, t;
	static long init = 1;
	long f;

	if (init == 1)
	{
		mInitTimer();
		init = 0;
	}
	
	_EndScene();
	DXShowFrame();

	if (!s)
		return 0;

	f = 0;

	while (f < s)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&pc);
		t = pc - counter;
		f = long(t / t_frequency);
	}

	counter += t / t_frequency * t_frequency;
	return f;
}

void inject_profiler(bool replace)
{
	INJECT(0x004BD3A0, mDrawTriangle, replace);
	INJECT(0x004BD590, mAddProfilerEvent, replace);
	INJECT(0x004BD3C0, mResetTime, replace);
	INJECT(0x004BD430, mGetTime, replace);
	INJECT(0x004BD3E0, mInitTimer, replace);
	INJECT(0x004BD470, mSync, replace);
}
