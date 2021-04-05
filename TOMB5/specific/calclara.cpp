#pragma once
#include "../tomb5/pch.h"
#include "calclara.h"
#include "../global/types.h"
#include "../game/draw.h"

short* GetBoundsAccurate(ITEM_INFO* item)
{
	int rate;
	short* frmptr[2];
	int frac = GetFrames(item, frmptr, &rate);

	if (frac == 0)
		return frmptr[0];

	short* bptr = interpolated_bounds;

	for (int i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
		*bptr = *frmptr[0] + (*frmptr[1] - *frmptr[0]) * frac / rate;

	return interpolated_bounds;
}

void inject_calclara()
{
	INJECT(0x0042CF80, GetBoundsAccurate);
}
