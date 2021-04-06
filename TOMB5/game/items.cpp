#include "../tomb5/pch.h"
#include "items.h"
#include "../global/types.h"

ITEM_INFO* find_a_fucking_item(int object_number)
{
	int i;

	if (level_items > 0)
	{
		for (i = 0; i < level_items; i++)
		{
			if (items[i].object_number == object_number)
			{
				return &items[i];
			}
		}
	}

	return NULL;
}

void inject_items()
{
	INJECT(0x00423470, find_a_fucking_item);
}
