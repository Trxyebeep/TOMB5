#include "../tomb5/pch.h"
#include "items.h"
#include "effect2.h"

void ItemNewRoom(short item_num, short room_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	short linknum;

	if (InItemControlLoop)
	{
		ItemNewRooms[ItemNewRoomNo][0] = item_num;
		ItemNewRooms[ItemNewRoomNo][1] = room_number;
		ItemNewRoomNo++;
		return;
	}

	item = &items[item_num];

	if (item->room_number != NO_ROOM)
	{
		r = &room[item->room_number];
		linknum = r->item_number;

		if (linknum == item_num)
			r->item_number = item->next_item;
		else
		{
			for (; linknum != NO_ITEM; linknum = items[linknum].next_item)
			{
				if (items[linknum].next_item == item_num)
				{
					items[linknum].next_item = item->next_item;
					break;
				}
			}
		}
	}

	item->room_number = room_number;
	r = &room[room_number];
	item->next_item = r->item_number;
	r->item_number = item_num;
}

void InitialiseItemArray(short num)
{
	ITEM_INFO* item;

	item = &items[level_items];
	next_item_free = (short)level_items;
	next_item_active = NO_ITEM;
	
	for (int i = level_items + 1; i < num; i++)
	{
		item->next_item = i;
		item->active = 0;
		item++;
	}

	item->next_item = NO_ITEM;
}

void KillItem(short item_num)
{
	ITEM_INFO* item;
	short linknum;

	if (InItemControlLoop)
	{
		ItemNewRooms[ItemNewRoomNo][0] = item_num | 0x8000;
		ItemNewRoomNo++;
		return;
	}

	DetatchSpark(item_num, 128);
	item = &items[item_num];
	item->active = 0;
	item->really_active = 0;

	if (next_item_active == item_num)
		next_item_active = item->next_active;
	else
	{
		for (linknum = next_item_active; linknum != NO_ITEM; linknum = items[linknum].next_active)
		{
			if (items[linknum].next_active == item_num)
			{
				items[linknum].next_active = item->next_active;
				break;
			}
		}
	}

	if (item->room_number != 255)
	{
		linknum = room[item->room_number].item_number;

		if (linknum == item_num)
			room[item->room_number].item_number = item->next_item;
		else
		{
			for (; linknum != NO_ITEM; linknum = items[linknum].next_item)
			{
				if (items[linknum].next_item == item_num)
				{
					items[linknum].next_item = item->next_item;
					break;
				}
			}
		}
	}

	if (item == lara.target)
		lara.target = 0;

	if (item_num < level_items)
		item->flags |= IFL_CLEARBODY;
	else
	{
		item->next_item = next_item_free;
		next_item_free = item_num;
	}
}

void inject_items(bool replace)
{
	INJECT(0x00440DA0, ItemNewRoom, replace);
	INJECT(0x00440590, InitialiseItemArray, replace);
	INJECT(0x00440620, KillItem, replace);
}
