#include "../tomb5/pch.h"
#include "items.h"

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

void inject_items(bool replace)
{
	INJECT(0x00440DA0, ItemNewRoom, replace);
	INJECT(0x00440590, InitialiseItemArray, replace);
}
