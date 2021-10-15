#include "../tomb5/pch.h"
#include "lasers.h"

void DrawFloorLasers(ITEM_INFO* item)
{
	
}

void inject_lasers(bool replace)
{
	INJECT(0x0045A540, DrawFloorLasers, replace);
}
