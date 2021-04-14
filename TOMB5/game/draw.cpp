#include "../tomb5/pch.h"
#include "../global/types.h"
#include "draw.h"

short* GetBoundsAccurate(ITEM_INFO* item)
{
	int rate, frac;
	short* frmptr[2];
	short* bptr;
	
	frac = GetFrames(item, frmptr, &rate);

	if (frac == 0)
		return frmptr[0];

	bptr = interpolated_bounds;

	for (int i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
		*bptr = *frmptr[0] + (*frmptr[1] - *frmptr[0]) * frac / rate;

	return interpolated_bounds;
}

short* GetBestFrame(ITEM_INFO* item)
{
	short* frm[2];
	int rate;
	int ret;
	
	ret = GetFrames(item, frm, &rate);

	if (ret > (rate >> 1))
		return frm[1];
	else
		return frm[0];
}


void inject_draw()
{
	INJECT(0x0042CF80, GetBoundsAccurate);
	INJECT(0x0042D020, GetBestFrame);
}
