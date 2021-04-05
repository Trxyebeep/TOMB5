#pragma once
#include "../tomb5/pch.h"
#include "newinv2.h"
#include "../global/types.h"
#include "gameflow.h"
#include "../specific/specific.h"
#include "effects.h"
#include "../specific/matrix_shit.h"
#include "sound.h"
#include "../specific/display.h"
#include "objects.h"

int have_i_got_object(short object_number)
{
	if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
		return (lara.puzzleitemscombo >> (object_number + 76)) & 1;

	if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM8)
		return *((char*)&lara.mesh_ptrs[6] + object_number);//what in the fuck

	if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
		return (lara.keyitemscombo >> (object_number + 52)) & 1;

	if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM8)
		return (lara.keyitems >> (object_number + 60)) & 1;

	if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
		return (lara.pickupitemscombo >> (object_number + 32)) & 1;

	if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
		return (lara.pickupitems >> (object_number + 36)) & 1;

	if (object_number == CROWBAR_ITEM)
		return lara.crowbar;

	if (object_number == WET_CLOTH)
		return lara.wetcloth & 2;

	return 0;
}

void remove_inventory_item(short object_number)
{
	if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
		lara.puzzleitemscombo &= ~(1 << (object_number + 76));

	if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM8)
		lara.puzzleitems[object_number - PUZZLE_ITEM1]--;

	if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
		lara.keyitemscombo &= ~(1 << (object_number + 52));

	if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM8)
		lara.keyitems &= ~(1 << (object_number + 60));

	if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
		lara.pickupitemscombo &= ~(1 << (object_number + 32));

	if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
		lara.pickupitems &= ~(1 << (object_number + 36));
}

int convert_obj_to_invobj(short obj)
{
	int i;

	for (i = 0; i < 100; i++)
	{
		if (inventry_objects_list[i].object_number == obj)
			return i;
	}

	return 27;
}

int convert_invobj_to_obj(int obj)
{
	return inventry_objects_list[obj].object_number;
}

void init_keypad_mode()
{
	keypadx = 0;
	keypady = 0;
	keypadnuminputs = 0;
	keypadpause = 0;
	keypadinputs[0] = 0;
	keypadinputs[1] = 0;
	keypadinputs[2] = 0;
	keypadinputs[3] = 0;
	return;
}

void do_keypad_mode()
{
	INVOBJ* objme;
	char buf[5];
	int n, val, val2;

	val = 0x1FFF;

	if (keypadnuminputs != 0)
	{
		for (n = 0; n < (int)keypadnuminputs; n++)
		{
			val2 = keypadinputs[n];

			if (!val2)
				val2 = 11;

			val = val & ~(1 << (val2 & 0x1f)) | 1 << (val2 + 0xc & 0x1f);
		}
	}

	objme = &inventry_objects_list[INV_PUZZLE_HOLE8];

	if (!(GnFrameCounter & 2) && !keypadpause)
		objme->meshbits = val & ~(1 << (((3 * keypady + keypadx) + 13) & 0x1F)) | 1 << (((3 * keypady + keypadx) + 1) & 0x1F);
	else
		objme->meshbits = val & ~(1 << (((keypadx + 3 * keypady) + 1) & 0x1F)) | 1 << (((keypadx + 3 * keypady) + 13) & 0x1F);

	DrawThreeDeeObject2D((phd_centerx * 0.00390625 * 256.0 + inventry_xpos), (phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2, INV_PUZZLE_HOLE8, 128, 0x8000, 0x4000, 0x4000, 0, 0);
	PrintString(0x100, (phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2 - 64, 6, &gfStringWad[gfStringOffset_bis[STR_ENTER_COMBINATION]], 0x8000);
	buf[0] = 45;
	buf[1] = 45;
	buf[2] = 45;
	buf[3] = 45;
	
	if (keypadnuminputs != 0)
	{		
		for (int i = 0; i < keypadnuminputs; ++i)
			buf[i] = keypadinputs[i] + 48;
	}

	PrintString(0x100, (phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2 + 64, 1, buf, 0x8000);

	if (keypadpause)
	{
		keypadpause--;

		if (keypadpause <= 0)
		{
			menu_active = 0;
			use_the_bitch = 1;
			return;
		}
	}

	if (go_select)
	{
		unsigned char va = keypady * 3 + keypadx + 1;
		unsigned int va2 = (unsigned int)(va);

		switch (va)
		{
		case 10://#, resets input
			SoundEffect(SFX_KEYPAD_HASH, 0, SFX_ALWAYS);
			keypadnuminputs = 0;
			return;

		case 11://zero
			va = 0;
			va2 = 0;
			break;

		case 12://*, submits input
			keypadpause = 30;
			SoundEffect(SFX_KEYPAD_STAR, 0, SFX_ALWAYS);
			return;
		}

		if (keypadnuminputs == 4)
			return;

		n = 0;

		if (keypadnuminputs != 0)
		{
			do
			{
				if (keypadinputs[n] == va)
					return;

				n = n + 1;

			} while (n < keypadnuminputs);
		}

		SoundEffect(va2 + SFX_KEYPAD_0, 0, SFX_ALWAYS);
		keypadinputs[keypadnuminputs] = va;
		keypadnuminputs++;

		if (keypadnuminputs == 4)
		{
			keypadx = 2;
			keypady = 3;
		}
	}

	if (go_left && keypadx)
		keypadx--;

	if (go_right && keypadx < 2)
		keypadx++;

	if (go_up && keypady)
		keypady--;

	if (go_down && keypady < 3)
		keypady++;

	return;
}

void do_examine_mode()
{
	INVOBJ* objme = &inventry_objects_list[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem];
	int saved_scale = objme->scale1;

	examine_mode += 8;
	if (examine_mode > 128)
		examine_mode = 128;

	objme->scale1 = 300;

	DrawThreeDeeObject2D(
		(phd_centerx + inventry_xpos),
		(phd_centery / 120.0 * 256.0 + inventry_xpos) / 2,
		rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem,
		examine_mode,
		0x8000,
		0x4000,
		0x4000,
		96,
		0);

	objme->scale1 = saved_scale;

	if (go_deselect)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		go_deselect = 0;
		examine_mode = 0;
	}
}

void do_stats_mode()
{
	stats_mode += 8;
	if (stats_mode > 0x80)
		stats_mode = 0x80;

	DisplayStatsUCunt();

	if (go_deselect)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		go_deselect = 0;
		stats_mode = 0;
	}
	return;
}

void dels_give_lara_items_cheat()
{
	//original function is EMPTY on Steam/GOG, but exists on JP and the DEMO EXEs.
	int piss;

	if (objects[CROWBAR_ITEM].loaded)
		lara.crowbar = 1;

	for (piss = 0; piss < 8; ++piss)
	{
		if (objects[PUZZLE_ITEM1 + piss].loaded)
			lara.puzzleitems[piss] = 1;
	}

	for (piss = 0; piss < 8; ++piss)
	{
		if (objects[KEY_ITEM1 + piss].loaded)
			lara.keyitems |= (1 << (piss & 0x1F));
	}

	for (piss = 0; piss < 3; ++piss)
	{
		if (objects[PICKUP_ITEM1 + piss].loaded)
			lara.pickupitems |= (1 << (piss & 0x1F));
	}

	if (gfCurrentLevel == LVL5_SUBMARINE)
	{
		lara.puzzleitems[0] = 0;
		lara.puzzleitemscombo = 0;
		lara.keyitemscombo = 0;
		lara.pickupitemscombo = 0;
	}

	if (gfCurrentLevel == LVL5_OLD_MILL)
	{
		lara.puzzleitems[2] = 0;
		lara.puzzleitemscombo = 0;
		lara.keyitemscombo = 0;
		lara.pickupitemscombo = 0;
	}

	return;
}

void dels_give_lara_guns_cheat()
{
	//original function is EMPTY on all PC EXEs, this is taken from PSX code.
	if (objects[FLARE_INV_ITEM].loaded)
		lara.num_flares = -1;

	lara.num_small_medipack = -1;
	lara.num_large_medipack = -1;

	if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA))
	{
		if (objects[SHOTGUN_ITEM].loaded)
		{
			lara.num_shotgun_ammo1 = -1;
			lara.num_shotgun_ammo2 = -1;
			lara.shotgun_type_carried |= -1;
		}

		if (objects[REVOLVER_ITEM].loaded)
		{
			lara.num_revolver_ammo = -1;
			lara.sixshooter_type_carried |= -1;
		}

		if (objects[CROSSBOW_ITEM].loaded)
		{
			lara.num_crossbow_ammo1 = -1;
			lara.num_crossbow_ammo2 = -1;
			lara.crossbow_type_carried |= -1;

			if (gfCurrentLevel < LVL5_GIBBY_LEVEL)
			{
				lara.crossbow_type_carried = WTYPE_PRESENT | WTYPE_LASERSIGHT | WTYPE_AMMO_1;
				lara.num_crossbow_ammo2 = 0;
			}
		}
		if (objects[HK_ITEM].loaded)
		{
			lara.num_hk_ammo1 = -1;
			lara.hk_type_carried |= 1;
		}

		if (objects[UZI_ITEM].loaded)
		{
			lara.num_uzi_ammo = -1;
			lara.uzis_type_carried |= 1;
		}

		if (objects[LASERSIGHT_ITEM].loaded)
			lara.lasersight = 1;

		if (objects[SILENCER_ITEM].loaded)
			lara.silencer = 1;
	}
}

void S_DrawPickup(short object_number)
{
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);

	SetD3DViewMatrix();

	aSetViewMatrix();

	DrawThreeDeeObject2D(
		(phd_winxmax * 0.001953125 * 448.0 + PickupX),
		(phd_winymax * 0.00390625 * 216.0),
		convert_obj_to_invobj(object_number),
		128,
		0,
		(GnFrameCounter & 0x7F) << 9,
		0,
		0,
		1);
}

void inject_newinv2()
{
	INJECT(0x00464360, have_i_got_object);
	INJECT(0x00464490, remove_inventory_item);
	INJECT(0x004645B0, convert_obj_to_invobj);
	INJECT(0x004645F0, convert_invobj_to_obj);
	INJECT(0x00464610, init_keypad_mode);
	INJECT(0x00464650, do_keypad_mode);
	INJECT(0x00464AB0, do_examine_mode);
	INJECT(0x00464BF0, do_stats_mode);
	INJECT(0x00464C60, dels_give_lara_items_cheat);
	INJECT(0x00464C80, dels_give_lara_guns_cheat);
	INJECT(0x004B78D0, S_DrawPickup);
}
