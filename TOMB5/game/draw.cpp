#pragma once
#include "../tomb5/pch.h"
#include "../global/types.h"
#include "draw.h"

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
	INJECT(0x0042D020, GetBestFrame);
}
