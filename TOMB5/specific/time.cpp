#include "../tomb5/pch.h"
#include "time.h"
#include "function_stubs.h"

static __int64 counter;

long Sync()
{
	__int64 PerformanceCount, f;
	long n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / t_frequency;
	counter += t_frequency * f;
	n = (long)f;
	return n;
}

void TIME_Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

bool TIME_Init()
{
	__int64 pfq;

	Log(2, "TIME_Init");

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq))
		return 0;

	t_frequency = pfq / 60;
	TIME_Reset();
	return 1;
}

void inject_time(bool replace)
{
	INJECT(0x004D1A40, Sync, replace);
	INJECT(0x004D19B0, TIME_Reset, replace);
	INJECT(0x004D19D0, TIME_Init, replace);
}
