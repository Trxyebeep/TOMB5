#include "../tomb5/pch.h"
#include "init.h"
#include "objects.h"
#include "gameflow.h"
#include "draw.h"

void InitialisePickup(short item_number)
{
	ITEM_INFO* item;
	short* bounds;
	short ocb;

	item = &items[item_number];
	ocb = item->trigger_flags & 0x3F;

	if (item->object_number == HK_ITEM && gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		return;

	bounds = GetBoundsAccurate(item);
	if (ocb == 6)
	{
		item->item_flags[0] = (short)(item->pos.y_pos - bounds[3]);
		item->status = ITEM_INVISIBLE;
	}
	else
	{
		if (ocb == 0 || ocb == 3 || ocb == 4 || ocb == 7 || ocb == 8 || ocb == 11)
			item->pos.y_pos -= bounds[3];

		if (item->trigger_flags & 128)
			RPickups[NumRPickups++] = (unsigned char)item_number;

		if (item->trigger_flags & 256)
			item->mesh_bits = 0;

		if (item->status == ITEM_INVISIBLE)
			item->flags |= IFLAG_TRIGGERED;
	}
}

void inject_init()
{
	INJECT(0x0043E260, InitialisePickup);
}
