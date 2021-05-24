#include "../tomb5/pch.h"
#include "traps.h"
#include "items.h"
#include "objects.h"
#include "control.h"

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
