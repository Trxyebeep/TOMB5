#include "../tomb5/pch.h"
#include "box.h"
#include "lot.h"

void InitialiseCreature(short item_number)
{
	ITEM_INFO* item; // Comes from PSX symbols

	item = &items[item_number];
	item->collidable = 1;
	item->data = 0;
	item->draw_room = ((item->pos.x_pos - room[item->room_number].x) >> 10 & 0xFF) << 8 |
		((item->pos.z_pos - room[item->room_number].z) >> 10 & 0xFF);
	item->item_flags[2] = item->pos.y_pos - room[item->room_number].minfloor & 0xFF00 |
		item->room_number & 0xFF;
	item->TOSSPAD = item->pos.y_rot & 0xE000;
}

int CreatureActive(short item_number)
{
	ITEM_INFO* item; // Comes from PSX symbols

	item = &items[item_number];

	if (item->flags & 0x8000)
		return 0;

	if (item->status == 3)
	{

		if (!EnableBaddieAI(item_number, 0))
			return 0;

		item->status = 1;

	}

	return 1;
}

void inject_box(bool replace)
{
	INJECT(0x00408550, InitialiseCreature, replace);
	INJECT(0x00408630, CreatureActive, replace);
}