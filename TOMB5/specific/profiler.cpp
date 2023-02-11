#include "../tomb5/pch.h"
#include "profiler.h"
#include "function_table.h"
#include "dxshell.h"
#include "time.h"

static __int64 counter;
static PROFILER_EVENT ProfilerEvents[30];
static long nProfilerEvents;

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

void mInitDrawProfiler(long x, long y, long f, long w)
{
	long lp, lp2, tx, ty;

	for (lp = 0; lp < f; lp++)
	{
		tx = x;
		ty = y;
		mDrawTriangle(tx, ty, tx - 4, ty - 4, tx + 4, ty - 4, 0x80FF8040, 0x80FF8040, 0x80FF8040);

		ty = y + 9;
		mDrawTriangle(tx, ty, tx - 4, ty + 5, tx + 4, ty + 5, 0x80FF8040, 0x80FF8040, 0x80FF8040);

		x += w / 4;

		for (lp2 = 0; lp2 < 3; lp2++)
		{
			tx = x;
			ty = y;
			mDrawTriangle(tx, ty, tx - 4, ty - 4, tx + 4, ty - 4, 0x80004080, 0x80004080, 0x80004080);

			ty = y + 9;
			mDrawTriangle(tx, ty, tx - 4, ty + 4, tx + 4, ty + 4, 0x80004080, 0x80004080, 0x80004080);

			x += w / 4;
		}
	}

	tx = x;
	ty = y;
	mDrawTriangle(tx, ty, tx - 4, ty - 4, tx + 4, ty - 4, 0x80FF8040, 0x80FF8040, 0x80FF8040);

	ty = y + 9;
	mDrawTriangle(tx, ty, tx - 4, ty + 4, tx + 4, ty + 4, 0x80FF8040, 0x80FF8040, 0x80FF8040);
}

void mDrawProfiler(double sync, long x, long y)
{
	PROFILER_EVENT* event;
	long lp, dt;

	if (t_frequency)
	{
		mInitDrawProfiler(x, y, 2, 320);

		if (nProfilerEvents > 0)
		{
			for (lp = 0; lp < nProfilerEvents - 1; lp++)
			{
				event = &ProfilerEvents[lp];
				dt = long(320 * (event[1].t - event->t) / (long)t_frequency);
				mDrawTriangle(x, y, x + dt, y, dt, y + 10, event->c, event->c, event->c);
				mDrawTriangle(x, y, x + dt, y + 10, x, y + 10, event->c, event->c, event->c);
				x += dt;
			}
		}
	}

	nProfilerEvents = 0;
}

void inject_profiler(bool replace)
{
	INJECT(0x004BD3A0, mDrawTriangle, replace);
	INJECT(0x004BD590, mAddProfilerEvent, replace);
	INJECT(0x004BD3C0, mResetTime, replace);
	INJECT(0x004BD430, mGetTime, replace);
	INJECT(0x004BD3E0, mInitTimer, replace);
	INJECT(0x004BD470, mSync, replace);
	INJECT(0x004BD6A0, mInitDrawProfiler, replace);
	INJECT(0x004BD5B0, mDrawProfiler, replace);
}
