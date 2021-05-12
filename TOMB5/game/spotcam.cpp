#include "../tomb5/pch.h"
#include "spotcam.h"

long Spline(long x, long* knots, int nk)
{
	int span;
	long *k, c1, c2;

	span = x * (nk - 3) >> 16;
	if (span >= nk - 3)
		span = nk - 4;
	k = &knots[span];
	x = x * (nk - 3) - span * 65536;
	c1 = (k[1] >> 1) - (k[2] >> 1) - k[2] + k[1] + (k[3] >> 1) + ((-k[0] - 1) >> 1);
	c2 = 2 * k[2] - 2 * k[1] - (k[1] >> 1) - (k[3] >> 1) + k[0];
	return ((long long) x * (((long long) x * (((long long) x * c1 >> 16) + c2) >> 16) + (k[2] >> 1) + ((-k[0] - 1) >> 1)) >> 16) + k[1];
}

void inject_spotcam()
{
	INJECT(0x0047A890, Spline);
}
