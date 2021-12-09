#include "../tomb5/pch.h"
#include "lot.h"
#include "../specific/function_stubs.h"

void InitialiseLOTarray(long allocmem)
{
	CREATURE_INFO* creature;

	if (allocmem)
		baddie_slots = (CREATURE_INFO*)game_malloc(sizeof(CREATURE_INFO) * 5, 0);

	for (int i = 0; i < 5; i++)
	{
		creature = &baddie_slots[i];
		creature->item_num = NO_ITEM;

		if (allocmem)
			creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * number_boxes, 0);
	}

	slots_used = 0;
}

void inject_lot(bool replace)
{
	INJECT(0x0045B0C0, InitialiseLOTarray, replace);
}
