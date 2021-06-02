#include "../tomb5/pch.h"
#include "traps.h"
#include "items.h"
#include "objects.h"
#include "control.h"

short SPxzoffs[8] = {0, 0, 0x200, 0, 0, 0, -0x200, 0};
short SPyoffs[8] = {-0x400, 0, -0x200, 0, 0, 0, -0x200, 0};
short SPDETyoffs[8] = {0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200};

void LaraBurn()
{
	short fire;

	if (!lara.burn && !lara.BurnSmoke)
	{
		fire = CreateEffect(lara_item->room_number);

		if (fire != NO_ITEM)
		{
			effects[fire].object_number = FLAME;
			lara.burn = 1;
		}
	}
}

void LavaBurn(ITEM_INFO* item)
{
	short room_number;

	if (item->hit_points >= 0 && lara.water_status != LW_FLYCHEAT)
	{
		room_number = item->room_number;

		if (item->floor == GetHeight(GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_number), item->pos.x_pos, 32000, item->pos.z_pos))
		{
			item->hit_points = -1;
			item->hit_status = 1;
			LaraBurn();
		}
	}
}

void inject_traps()
{
	INJECT(0x0048AD60, LaraBurn);
	INJECT(0x0048ADD0, LavaBurn);
}
