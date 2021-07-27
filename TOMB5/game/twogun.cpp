#include "../tomb5/pch.h"
#include "twogun.h"
#include "lara_states.h"
#include "items.h"
#include "../specific/audio.h"

void ControlZipController(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->item_flags[0])
	{
		bDisableLaraControl = 1;
		item->item_flags[0]++;
	}
	else if (item->item_flags[0] == 1)
	{
		if (lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH)
		{
			lara_item->goal_anim_state = AS_INTO_ZIP;
			lara_item->current_anim_state = AS_INTO_ZIP;
			lara_item->anim_number = ANIM_ZIP_IN;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			item->item_flags[0]++;
			IsAtmospherePlaying = 0;
			S_CDPlay(item->trigger_flags, 0);
		}
	}
	else if (item->item_flags[0] == 2)
	{
		if (!item->item_flags[1])
		{
			if (XATrack == item->trigger_flags)
				item->item_flags[1] = 1;
		}
		else if (XATrack != item->trigger_flags)
		{
			lara_item->goal_anim_state = AS_OUT_ZIP;
			bDisableLaraControl = 0;
			KillItem(item_number);
		}
	}
}

void inject_twogun(bool replace)
{
	INJECT(0x0048E3C0, ControlZipController, replace);
}
