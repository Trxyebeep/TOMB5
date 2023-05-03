#include "../tomb5/pch.h"
#include "time.h"
#include "function_stubs.h"

static __int64 frequency;
static __int64 counter;

long Sync()
{
	__int64 PerformanceCount, f;
	long n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / frequency;
	counter += frequency * f;
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

	Log(__FUNCTION__);

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq))
		return 0;

	frequency = pfq / 60;
	TIME_Reset();
	return 1;
}
