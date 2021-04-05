#pragma once
#include "../tomb5/pch.h"
#include "pickup.h"
#include "../global/types.h"
#include "gameflow.h"
#include "../specific/specific.h"
#include "xatracks.h"
#include "effects.h"
#include "sound.h"
#include "items.h"
#include "control.h"
#include "objects.h"

//0x0043E260 is initialisepickup//the function that corrects the pickups' Y pos depending on their bounding box..

void RegeneratePickups()
{
	for (int lp = 0; lp < NumRPickups; lp++)
	{
		ITEM_INFO* item = &items[RPickups[lp]];

		if (item->status == ITEM_INVISIBLE)
		{
			short* ammo = NULL;

			if (item->object_number == CROSSBOW_AMMO1_ITEM)
				ammo = &lara.num_crossbow_ammo1;
			if (item->object_number == CROSSBOW_AMMO2_ITEM)
				ammo = &lara.num_crossbow_ammo2;
			if (item->object_number == HK_AMMO_ITEM)
				ammo = &lara.num_hk_ammo1;
			if (item->object_number == REVOLVER_AMMO_ITEM)
				ammo = &lara.num_revolver_ammo;
			if (item->object_number == SHOTGUN_AMMO1_ITEM)
				ammo = &lara.num_shotgun_ammo1;
			if (item->object_number == SHOTGUN_AMMO1_ITEM)
				ammo = &lara.num_shotgun_ammo2;

			if (ammo && *ammo == 0)
				item->status = ITEM_INACTIVE;
		}
	}
}

static void PuzzleDone(ITEM_INFO* item, short item_num)
{
	int lp;

	if (item->object_number == PUZZLE_HOLE1 && gfCurrentLevel == LVL5_GALLOWS_TREE)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA6_SPOOKY03, 0);
		SoundEffect(SFX_HANGMAN_LAUGH_OFFCAM, &item->pos, 0);
	}

	item->object_number += 8;//puzzle hole to done, because there's 8 of them
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->required_anim_state = 0;
	item->goal_anim_state = item->anim_number;
	item->current_anim_state = item->anim_number;
	AddActiveItem(item_num);
	item->flags |= IFLAG_ACTIVATION_MASK;
	item->status = ITEM_ACTIVE;

	if (item->trigger_flags == 998 && level_items > 0)
	{
		for (lp = 0; lp < level_items; lp++)
		{
			if (items[lp].object_number == AIRLOCK_SWITCH
				&& items[lp].pos.x_pos == item->pos.x_pos
				&& items[lp].pos.z_pos == item->pos.z_pos)
			{
				FlipMap(items[lp].trigger_flags - 7);
				flipmap[items[lp].trigger_flags - 7] ^= IFLAG_ACTIVATION_MASK;
				items[lp].status = ITEM_INACTIVE;
				items[lp].flags |= 0x20;
			}
		}
	}

	if (item->trigger_flags > 1024)
	{
		cutrot = 0;
		cutseq_num = item->trigger_flags - 1024;
	}
}

void inject_pickup()
{
	INJECT(0x00467AF0, RegeneratePickups);
	INJECT(0x004693A0, PuzzleDone);
}
