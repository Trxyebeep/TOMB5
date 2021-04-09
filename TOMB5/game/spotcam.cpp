#include "pch.h"
#include "spotcam.h"
#include "../global/types.h"

long Spline(long x, long* knots, int nk)
{
	int64_t v3 = x * (int64_t)(nk - 3) << 16 >> 16;
	int32_t v4 = (int32_t)v3 >> 16;
	if ((int32_t)v3 >> 16 >= nk - 3)
		v4 = nk - 4;
	int32_t v5 = knots[v4];
	int32_t v6 = knots[v4 + 2];
	int32_t nka = knots[v4 + 3] >> 1;
	int32_t v7 = knots[v4 + 1];

	return (int32_t)(v7
		+ (int64_t)(uint64_t)((int32_t)((~v5 >> 1)
			+ (v6 >> 1)
			+ (int64_t)(uint64_t)((int32_t)(v5
				+ (int64_t)(uint64_t)(((~v5 >> 1)
					+ nka
					+ v7
					+ (v7 >> 1)
					- (v6 >> 1)
					- v6)
					* (int64_t)((int32_t)v3 - (v4 << 16)) >> 16)
				- 2 * v7
				+ 2 * v6
				- (v7 >> 1)
				- nka)
				* (int64_t)((int32_t)v3 - (v4 << 16)) >> 16))
			* (int64_t)((int32_t)v3 - (v4 << 16)) >> 16));
}

void inject_spotcam()
{
	INJECT(0x0047A890, Spline);
}
