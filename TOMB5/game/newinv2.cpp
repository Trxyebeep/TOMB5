#include "../tomb5/pch.h"
#include "newinv2.h"
#include "gameflow.h"
#include "../specific/specific.h"
#include "effects.h"
#include "../specific/matrix_shit.h"
#include "sound.h"
#include "../specific/display.h"
#include "objects.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "camera.h"
#include "control.h"
#include "xatracks.h"
#include "lara.h"
#include "larafire.h"
#include "lara_states.h"
#include "../specific/LoadSave.h"
#include "../specific/input.h"
#include "../specific/output.h"
#include "health.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "subsuit.h"
#include "../specific/specificfx.h"
#include "../specific/polyinsert.h"
#include "text.h"

short optmessages[11] =
{
	STR_USE, STR_CHOOSE_AMMO, STR_COMBINE, STR_SEPARATE, STR_EQUIP, STR_COMBINE_WITH, STR_LOAD_GAME, STR_SAVE_GAME, STR_EXAMINE, STR_STATISTICS, STR_CHOOSE_WEAPON_MODE
};

uchar wanky_secrets_table[18] =
{
	0, 3, 3, 3, 3, 3, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

ushort options_table[99] =
{
	0x020A, 0x040A, 0x004A, 0x080A, 0x0812, 0x008A, 0x0092, 0x010A, 0x0112, 0x0004,
	0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
	0x000C, 0x000C, 0x0004, 0x0004, 0x0004, 0x0004, 0x8000, 0x1000, 0x2000, 0x0004,
	0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x000C, 0x000C, 0x000C,
	0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
	0x000C, 0x000C, 0x000C, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
	0x0004, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
	0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0004, 0x0004, 0x0004,
	0x0004, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0004,
	0x0004, 0x0020, 0x0020, 0x0020, 0x0004, 0x4002, 0x0004, 0x000C, 0x000C
};

COMBINELIST dels_handy_combine_table[24] =
{
	{combine_revolver_lasersight, INV_REVOLVER_ITEM1, INV_LASERSIGHT_ITEM, INV_REVOLVER_ITEM2},
	{combine_crossbow_lasersight, INV_CROSSBOW_AMMO2_ITEM1, INV_LASERSIGHT_ITEM, INV_CROSSBOW_AMMO2_ITEM2},
	{combine_HK_SILENCER, INV_HK_ITEM1, INV_SILENCER_ITEM, INV_HK_ITEM2},
	{combine_PuzzleItem1, INV_PUZZLE_ITEM1_COMBO1, INV_PUZZLE_ITEM1_COMBO2, INV_PUZZLE_ITEM1},
	{combine_PuzzleItem2, INV_PUZZLE_ITEM2_COMBO1, INV_PUZZLE_ITEM2_COMBO2, INV_PUZZLE_ITEM2},
	{combine_PuzzleItem3, INV_PUZZLE_ITEM3_COMBO1, INV_PUZZLE_ITEM3_COMBO2, INV_PUZZLE_ITEM3},
	{combine_PuzzleItem4, INV_PUZZLE_ITEM4_COMBO1, INV_PUZZLE_ITEM4_COMBO2, INV_PUZZLE_ITEM4},
	{combine_PuzzleItem5, INV_PUZZLE_ITEM5_COMBO1, INV_PUZZLE_ITEM5_COMBO2, INV_PUZZLE_ITEM5},
	{combine_PuzzleItem6, INV_PUZZLE_ITEM6_COMBO1, INV_PUZZLE_ITEM6_COMBO2, INV_PUZZLE_ITEM6},
	{combine_PuzzleItem7, INV_PUZZLE_ITEM7_COMBO1, INV_PUZZLE_ITEM7_COMBO2, INV_PUZZLE_ITEM7},
	{combine_PuzzleItem8, INV_PUZZLE_ITEM8_COMBO1, INV_PUZZLE_ITEM8_COMBO2, INV_PUZZLE_ITEM8},
	{combine_KeyItem1, INV_KEY_ITEM1_COMBO1, INV_KEY_ITEM1_COMBO2, INV_KEY_ITEM1},
	{combine_KeyItem2, INV_KEY_ITEM2_COMBO1, INV_KEY_ITEM2_COMBO2, INV_KEY_ITEM2},
	{combine_KeyItem3, INV_KEY_ITEM3_COMBO1, INV_KEY_ITEM3_COMBO2, INV_KEY_ITEM3},
	{combine_KeyItem4, INV_KEY_ITEM4_COMBO1, INV_KEY_ITEM4_COMBO2, INV_KEY_ITEM4},
	{combine_KeyItem5, INV_KEY_ITEM5_COMBO1, INV_KEY_ITEM5_COMBO2, INV_KEY_ITEM5},
	{combine_KeyItem6, INV_KEY_ITEM6_COMBO1, INV_KEY_ITEM6_COMBO2, INV_KEY_ITEM6},
	{combine_KeyItem7, INV_KEY_ITEM7_COMBO1, INV_KEY_ITEM7_COMBO2, INV_KEY_ITEM7},
	{combine_KeyItem8, INV_KEY_ITEM8_COMBO1, INV_KEY_ITEM8_COMBO2, INV_KEY_ITEM8},
	{combine_PickupItem1, INV_PICKUP_ITEM1_COMBO1, INV_PICKUP_ITEM1_COMBO2, INV_PICKUP_ITEM1},
	{combine_PickupItem2, INV_PICKUP_ITEM2_COMBO1, INV_PICKUP_ITEM2_COMBO2, INV_PICKUP_ITEM2},
	{combine_PickupItem3, INV_PICKUP_ITEM3_COMBO1, INV_PICKUP_ITEM3_COMBO2, INV_PICKUP_ITEM3},
	{combine_PickupItem4, INV_PICKUP_ITEM4_COMBO1, INV_PICKUP_ITEM4_COMBO2, INV_PICKUP_ITEM4},
	{combine_clothbottle, INV_CLOTH, INV_BOTTLE, INV_WET_CLOTH}
};

int S_CallInventory2()
{
	int return_value;

	if (gfCurrentLevel < LVL5_BASE || gfCurrentLevel > LVL5_SINKING_SUBMARINE)
	{
		inventry_objects_list[INV_REVOLVER_ITEM1].objname = STR_REVOLVER;
		inventry_objects_list[INV_REVOLVER_ITEM2].objname = STR_REVOLVER_LASERSIGHT;
		inventry_objects_list[INV_REVOLVER_AMMO_ITEM].objname = STR_REVOLVER_AMMO;
	}
	else
	{
		inventry_objects_list[INV_REVOLVER_ITEM1].objname = STR_DESERTEAGLE;
		inventry_objects_list[INV_REVOLVER_ITEM2].objname = STR_DESERTEAGLE_LASERSIGHT;
		inventry_objects_list[INV_REVOLVER_AMMO_ITEM].objname = STR_DESERTEAGLE_AMMO;
	}

	if (gfCurrentLevel > LVL5_THIRTEENTH_FLOOR && gfCurrentLevel < LVL5_RED_ALERT)
		inventry_objects_list[INV_BINOCULARS_ITEM].objname = STR_HEADSET;
	else
		inventry_objects_list[INV_BINOCULARS_ITEM].objname = STR_BINOCULARS;

	friggrimmer = 0;
	oldLaraBusy = lara.Busy != 0;

	if (input & IN_SELECT)
		friggrimmer = 1;

	rings[RING_INVENTORY] = &pcring1;
	rings[RING_AMMO] = &pcring2;
	CreateMonoScreen();
	InventoryActive = 1;
	init_new_inventry();
	camera.number_frames = 2;

	while (!reset_flag)
	{
		int val = 0;

		OBJLIST_SPACING = phd_centerx >> 1;
		S_InitialisePolyList();
		SetDebounce = 1;
		S_UpdateInput();
		input = inputBusy;
		UpdatePulseColour();
		GameTimer++;

		if (dbinput & IN_OPTION)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			val = 1;
		}

		return_value = thread_started;

		if (return_value)
			return return_value;

		S_DisplayMonoScreen();

		if (GlobalSoftReset)
		{
			GlobalSoftReset = 0;
			val = 1;
		}

		do_debounced_joystick_poo();

		if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_COMPASS_ITEM
			&& keymap[34] //G
			&& keymap[22] //U
			&& keymap[49] //N
			&& keymap[31])//S
			dels_give_lara_guns_cheat();

		if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_COMPASS_ITEM
			&& keymap[48] //B
			&& keymap[23] //I
			&& keymap[20] //T
			&& keymap[31])//S
		{
			dels_give_lara_items_cheat();
			savegame.CampaignSecrets[0] = 9;
			savegame.CampaignSecrets[1] = 9;
			savegame.CampaignSecrets[2] = 9;
			savegame.CampaignSecrets[3] = 9;
		}

		if (GLOBAL_invkeypadmode)
			do_keypad_mode();
		else if (examine_mode)
			do_examine_mode();
		else if (stats_mode)
			do_stats_mode();
		else
		{
			draw_current_object_list(RING_INVENTORY);
			handle_inventry_menu();
			
			if (rings[RING_AMMO]->ringactive)
				draw_current_object_list(RING_AMMO);

			draw_ammo_selector();
			fade_ammo_selector();
		}

		if (use_the_bitch & !input)
			val = 1;

		S_OutputPolyList();
		camera.number_frames = S_DumpScreen();

		if (loading_or_saving)
		{
			do
			{
				S_InitialisePolyList();
				SetDebounce = 1;
				S_UpdateInput();
				input = inputBusy;
				UpdatePulseColour();

				if (loading_or_saving == 1)
					val = go_and_load_game();
				else if (go_and_save_game())
					val = 1;

			} while (!val);

			if (val == 1 && loading_or_saving == val)
			{
				return_value = 1;
				val = 1;
			}

			friggrimmer2 = 1;
			friggrimmer = 1;
			deselect_debounce = 0;
			go_deselect = 0;
			loading_or_saving = 0;
		}

		if (val)
			break;
	}

	InitialisePickUpDisplay();
	GLOBAL_lastinvitem = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
	update_laras_weapons_status();

	if (use_the_bitch && !GLOBAL_invkeypadmode)
		use_current_item();

	FreeMonoScreen();

	lara.Busy = oldLaraBusy & 1;
	InventoryActive = 0;

	if (GLOBAL_invkeypadmode)
	{
		short room_number;
		ITEM_INFO* item;
		int val;
		
		val = 0;
		GLOBAL_invkeypadmode = 0;

		if (keypadnuminputs == 4)
			val = keypadinputs[3] + 10 * (keypadinputs[2] + 10 * (keypadinputs[1] + 10 * keypadinputs[0]));

		if (GLOBAL_invkeypadcombination == val)
		{
			item = lara_item;
			room_number = lara_item->room_number;
			GetHeight(GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number),
				item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			TestTriggers(trigger_index, 1, 0);
		}
	}

	return return_value;
}

void init_new_inventry()
{
	examine_mode = 0;
	stats_mode = 0;
	AlterFOV(14560);
	lara.Busy = 0;
	GLOBAL_inventoryitemchosen = NO_ITEM;
	left_debounce = 0;
	right_debounce = 0;
	up_debounce = 0;
	down_debounce = 0;
	go_left = 0;
	go_right = 0;
	go_up = 0;
	go_down = 0;
	select_debounce = 0;
	deselect_debounce = 0;
	go_select = 0;
	go_deselect = 0;
	left_repeat = 0;
	right_repeat = 0;
	loading_or_saving = 0;
	use_the_bitch = 0;

	if (lara.num_shotgun_ammo1 == -1)
		AmountShotGunAmmo1 = -1;
	else
		AmountShotGunAmmo1 = lara.num_shotgun_ammo1 / 6;

	if (lara.num_shotgun_ammo2 == -1)
		AmountShotGunAmmo2 = -1;
	else
		AmountShotGunAmmo2 = lara.num_shotgun_ammo2 / 6;

	AmountHKAmmo1 = lara.num_hk_ammo1;
	AmountCrossBowAmmo1 = lara.num_crossbow_ammo1;
	AmountCrossBowAmmo2 = lara.num_crossbow_ammo2;
	AmountUziAmmo = lara.num_uzi_ammo;
	AmountRevolverAmmo = lara.num_revolver_ammo;
	AmountPistolsAmmo = lara.num_pistols_ammo;
	construct_object_list();

	if (GLOBAL_enterinventory == NO_ITEM)
	{
		if (GLOBAL_lastinvitem != NO_ITEM)
		{
			if (have_i_got_item(GLOBAL_lastinvitem))
				setup_objectlist_startposition(GLOBAL_lastinvitem);

			GLOBAL_lastinvitem = NO_ITEM;
		}
	}
	else if (GLOBAL_enterinventory == 0xDEADBEEF)
	{
		GLOBAL_invkeypadmode = 1;
		init_keypad_mode();
		GLOBAL_enterinventory = NO_ITEM;
	}
	else
	{
		if (have_i_got_object(GLOBAL_enterinventory))
			setup_objectlist_startposition2(GLOBAL_enterinventory);

		GLOBAL_enterinventory = NO_ITEM;
	}

	ammo_selector_fade_val = 0;
	ammo_selector_fade_dir = 0;
	combine_ring_fade_val = 0;
	combine_ring_fade_dir = 0;
	combine_type_flag = 0;
	seperate_type_flag = 0;
	combine_obj1 = 0;
	combine_obj2 = 0;
	normal_ring_fade_val = 128;
	normal_ring_fade_dir = 0;
	handle_object_changeover(RING_INVENTORY);
}

void do_debounced_joystick_poo()
{
	go_left = 0;
	go_right = 0;
	go_up = 0;
	go_down = 0;
	go_select = 0;
	go_deselect = 0;

	if ((input & IN_LEFT))
	{
		if (left_repeat >= 8)
			go_left = 1;
		else
			left_repeat++;

		if (!left_debounce)
			go_left = 1;

		left_debounce = 1;
	}
	else
	{
		left_debounce = 0;
		left_repeat = 0;
	}

	if ((input & IN_RIGHT))
	{
		if (right_repeat >= 8)
			go_right = 1;
		else
			right_repeat++;

		if (!right_debounce)
			go_right = 1;

		right_debounce = 1;
	}
	else
	{
		right_debounce = 0;
		right_repeat = 0;
	}

	if ((input & IN_FORWARD))
	{
		if (!up_debounce)
			go_up = 1;

		up_debounce = 1;
	}
	else
		up_debounce = 0;

	if ((input & IN_BACK))
	{
		if (!down_debounce)
			go_down = 1;

		down_debounce = 1;
	}
	else
		down_debounce = 0;

	if (input & IN_ACTION || input & IN_SELECT)
		select_debounce = 1;
	else
	{
		if (select_debounce == 1 && !friggrimmer)
			go_select = 1;

		select_debounce = 0;
		friggrimmer = 0;
	}

	if ((input & IN_DESELECT))
		deselect_debounce = 1;
	else
	{
		if (deselect_debounce == 1 && !friggrimmer2)
			go_deselect = 1;

		deselect_debounce = 0;
		friggrimmer2 = 0;
	}
}

void DrawThreeDeeObject2D(int x, int y, int num, int shade, int xrot, int yrot, int zrot, int bright, int overlay)
{
	ITEM_INFO item;
	INVOBJ* objme;

	objme = &inventry_objects_list[num];
	item.pos.x_rot = xrot + objme->xrot;
	item.pos.y_rot = yrot + objme->yrot;
	item.pos.z_rot = zrot + objme->zrot;
	item.object_number = objme->object_number;
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	aLookAt(0, 1024, 0, 100, 0, 200, 0);

	if (!bright)
		pcbright = 0x007F7F7F;
	else if (bright == 1)
		pcbright = 0x002F2F2F;
	else
		pcbright = bright | ((bright | (bright << 8)) << 8);

	SetD3DViewMatrix();
	phd_PushUnitMatrix();
	phd_TranslateRel(0, 0, objme->scale1);
	yoffset = objme->yoff + y;
	item.mesh_bits = objme->meshbits;
	xoffset = x;
	item.shade = -1;
	item.pos.x_pos = 0;
	item.pos.y_pos = 0;
	item.pos.z_pos = 0;
	item.room_number = 0;
	item.il.nCurrentLights = 0;
	item.il.nPrevLights = 0;
	item.il.ambient = 0x007F7F7F;
	item.anim_number = objects[item.object_number].anim_index;

	if (objme->flags & 8)
		DrawInventoryItemMe(&item, shade, overlay, 1);
	else
		DrawInventoryItemMe(&item, shade, overlay, 0);
		
	phd_PopMatrix();
	xoffset = phd_centerx;
	yoffset = phd_centery;
}

void DrawInventoryItemMe(ITEM_INFO* item, long shade, int overlay, int shagflag)
{
//	ANIM_STRUCT* anim;
	OBJECT_INFO* object;
	VECTOR vec;
	long* bone;
	short* rotation1;
	short** meshpp;
	short* frmptr;
	long i, poppush;
	ulong bit;
	int unk_bak;////

	frmptr = anims[item->anim_number].frame_ptr;
	object = &objects[item->object_number];
	phd_PushMatrix();

	if ((item->object_number == PC_LOAD_INV_ITEM || item->object_number == PC_SAVE_INV_ITEM) && !IsHardware())
	{
		if (IsSuperLowRes())
		{
			if (IsSuperLowRes() == 1)
				phd_TranslateRel(0, -390, 0);
			else
				phd_TranslateRel(0, -190, 0);
		}
	}

	if (item->object_number == HK_ITEM && gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		phd_TranslateRel(0, 70, 0);

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	if (item->object_number == PUZZLE_HOLE8 && GLOBAL_invkeypadmode)
	{
		vec.vx = 24576;
		vec.vy = 16384;
		vec.vz = 4096;
		ScaleCurrentMatrix(&vec);
	}

	bit = 1;
	meshpp = &meshes[object->mesh_index];
	bone = &bones[object->bone_index];

	if (!shagflag)
		phd_TranslateRel(frmptr[6], frmptr[7], frmptr[8]);

	rotation1 = &frmptr[9];
	gar_RotYXZsuperpack(&rotation1, 0);

	if ((item->mesh_bits & 1))
	{
		if (overlay)
			phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
		else
		{
			unk_bak = GlobalAlpha;
			GlobalAlpha = 0xFF000000;
			phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
			GlobalAlpha = unk_bak;
		}
	}

	meshpp += 2;

	for (i = 0; i < object->nmeshes - 1; i++, meshpp += 2, bone += 4)
	{
		poppush = *bone;

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rotation1, 0);
		bit <<= 1;

		if ((bit & item->mesh_bits))
		{
			if (overlay)
				phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
			else
			{
				unk_bak = GlobalAlpha;
				GlobalAlpha = 0xFF000000;
				phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
				GlobalAlpha = unk_bak;
			}
		}
	}

	phd_PopMatrix();
}

int go_and_load_game()
{
	return LoadGame();
}

int go_and_save_game()
{
	return SaveGame();
}

void construct_combine_object_list()
{
	rings[RING_AMMO]->numobjectsinlist = 0;

	for (int i = 0; i < 100; i++)
		rings[RING_AMMO]->current_object_list[i].invitem = -1;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		if (lara.sixshooter_type_carried & WTYPE_PRESENT)
		{
			if (lara.sixshooter_type_carried & WTYPE_LASERSIGHT)
				insert_object_into_list_v2(INV_REVOLVER_ITEM2);
			else
				insert_object_into_list_v2(INV_REVOLVER_ITEM1);
		}

		if (lara.hk_type_carried & WTYPE_PRESENT)
			insert_object_into_list_v2(INV_HK_ITEM1);

		if (lara.crossbow_type_carried & WTYPE_PRESENT && (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT))
		{
			if (lara.crossbow_type_carried & WTYPE_LASERSIGHT)
				insert_object_into_list_v2(INV_CROSSBOW_AMMO2_ITEM2);
			else
				insert_object_into_list_v2(INV_CROSSBOW_AMMO2_ITEM1);
		}

		if (lara.lasersight)
			insert_object_into_list_v2(INV_LASERSIGHT_ITEM);

		if (lara.silencer)
			insert_object_into_list_v2(INV_SILENCER_ITEM);
	}

	for (int i = 0; i < 16; i++)
		if (lara.puzzleitemscombo & (1 << i))
			insert_object_into_list_v2(INV_PUZZLE_ITEM1_COMBO1 + i);

	for (int i = 0; i < 16; i++)
		if (lara.keyitemscombo & (1 << i))
			insert_object_into_list_v2(INV_KEY_ITEM1_COMBO1 + i);

	for (int i = 0; i < 8; i++)
		if (lara.pickupitemscombo & (1 << i))
			insert_object_into_list_v2(INV_PICKUP_ITEM1_COMBO1 + i);

	if (lara.wetcloth == CLOTH_DRY)
		insert_object_into_list_v2(INV_CLOTH);

	if (lara.bottle)
		insert_object_into_list_v2(INV_BOTTLE);

	rings[RING_AMMO]->objlistmovement = 0;
	rings[RING_AMMO]->curobjinlist = 0;
	rings[RING_AMMO]->ringactive = 0;
}

void insert_object_into_list_v2(int num)
{
	if (options_table[num] & 9)
	{
		if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem != num)
		{
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].invitem = num;
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].yrot = 0;
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist++].bright = 32;
		}
	}
}

void construct_object_list()
{
	rings[RING_INVENTORY]->numobjectsinlist = 0;

	for (int i = 0; i < 100; i++)
		rings[RING_INVENTORY]->current_object_list[i].invitem = NO_ITEM;

	CurrentPistolsAmmoType = 0;
	CurrentUziAmmoType = 0;
	CurrentRevolverAmmoType = 0;
	CurrentShotGunAmmoType = 0;
	CurrentGrenadeGunAmmoType = 0;
	CurrentCrossBowAmmoType = 0;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		if (lara.pistols_type_carried & WTYPE_PRESENT)
			insert_object_into_list(INV_PISTOLS_ITEM);

		if (lara.uzis_type_carried & WTYPE_PRESENT)
			insert_object_into_list(INV_UZI_ITEM);
		else if (AmountUziAmmo)
			insert_object_into_list(INV_UZI_AMMO_ITEM);

		if (lara.sixshooter_type_carried & WTYPE_PRESENT)
		{
			if (lara.sixshooter_type_carried & WTYPE_LASERSIGHT)
				insert_object_into_list(INV_REVOLVER_ITEM2);
			else
				insert_object_into_list(INV_REVOLVER_ITEM1);
		}
		else if (AmountRevolverAmmo)
			insert_object_into_list(INV_REVOLVER_AMMO_ITEM);

		if (lara.shotgun_type_carried & WTYPE_PRESENT)
		{
			insert_object_into_list(INV_SHOTGUN_ITEM);

			if (lara.shotgun_type_carried & WTYPE_AMMO_2)
				CurrentShotGunAmmoType = 1;
		}
		else
		{
			if (AmountShotGunAmmo1)
				insert_object_into_list(INV_SHOTGUN_AMMO1_ITEM);

			if (AmountShotGunAmmo2)
				insert_object_into_list(INV_SHOTGUN_AMMO2_ITEM);
		}

		if (lara.hk_type_carried & WTYPE_PRESENT)
		{
			if (lara.hk_type_carried & WTYPE_SILENCER)
				insert_object_into_list(INV_HK_ITEM2);
			else
				insert_object_into_list(INV_HK_ITEM1);

			if (lara.hk_type_carried & WTYPE_AMMO_2)
				CurrentGrenadeGunAmmoType = 1;
			else if (lara.hk_type_carried & WTYPE_AMMO_3)
				CurrentGrenadeGunAmmoType = 2;
		}
		else if (AmountHKAmmo1)
			insert_object_into_list(INV_HK_AMMO_ITEM4);

		if (lara.crossbow_type_carried & WTYPE_PRESENT)
		{
			if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT)
			{
				if (lara.crossbow_type_carried & WTYPE_LASERSIGHT)
					insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM2);
				else
					insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM1);

				if (lara.crossbow_type_carried & WTYPE_AMMO_2)
					CurrentCrossBowAmmoType = 1;
			}
			else
			{
				insert_object_into_list(INV_CROSSBOW_ITEM);
				CurrentCrossBowAmmoType = 0;
			}
		}
		else if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT)
		{
			if (AmountCrossBowAmmo1)
				insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM3);

			if (AmountCrossBowAmmo2)
				insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM4);
		}
		else if (AmountCrossBowAmmo1)
			insert_object_into_list(INV_CROSSBOW_AMMO1_ITEM);

		if (lara.lasersight)
			insert_object_into_list(INV_LASERSIGHT_ITEM);

		if (lara.silencer)
			insert_object_into_list(INV_SILENCER_ITEM);

		if (lara.binoculars)
			insert_object_into_list(INV_BINOCULARS_ITEM);

		if (lara.num_flares)
			insert_object_into_list(INV_FLARE_INV_ITEM);
	}

	insert_object_into_list(INV_COMPASS_ITEM);

	if (lara.num_small_medipack)
		insert_object_into_list(INV_SMALLMEDI_ITEM);

	if (lara.num_large_medipack)
		insert_object_into_list(INV_BIGMEDI_ITEM);

	if (lara.crowbar)
		insert_object_into_list(INV_CROWBAR_ITEM);

	for (int i = 0; i < 8; i++)
		if (lara.puzzleitems[i])
			insert_object_into_list(INV_PUZZLE_ITEM1 + i);

	for (int i = 0; i < 16; i++)
		if (lara.puzzleitemscombo & (1 << i))
			insert_object_into_list(INV_PUZZLE_ITEM1_COMBO1 + i);

	for (int i = 0; i < 8; i++)
		if (lara.keyitems & (1 << i))
			insert_object_into_list(INV_KEY_ITEM1 + i);

	for (int i = 0; i < 16; i++)
		if (lara.keyitemscombo & (1 << i))
			insert_object_into_list(INV_KEY_ITEM1_COMBO1 + i);

	for (int i = 0; i < 4; i++)
		if (lara.pickupitems & (1 << i))
			insert_object_into_list(INV_PICKUP_ITEM1 + i);

	for (int i = 0; i < 8; i++)
		if (lara.pickupitemscombo & (1 << i))
			insert_object_into_list(INV_PICKUP_ITEM1_COMBO1 + i);

	if (lara.examine1)
		insert_object_into_list(INV_EXAMINE1);

	if (lara.examine2)
		insert_object_into_list(INV_EXAMINE2);

	if (lara.examine3)
		insert_object_into_list(INV_EXAMINE3);

	if (lara.wetcloth == CLOTH_WET)
		insert_object_into_list(INV_WET_CLOTH);

	if (lara.wetcloth == CLOTH_DRY)
		insert_object_into_list(INV_CLOTH);

	if (lara.bottle)
		insert_object_into_list(INV_BOTTLE);

	if (Gameflow->LoadSaveEnabled)
	{
		insert_object_into_list(INV_MEMCARD_LOAD_INV_ITEM);
		insert_object_into_list(INV_MEMCARD_SAVE_INV_ITEM);
	}

	rings[RING_INVENTORY]->objlistmovement = 0;
	rings[RING_INVENTORY]->curobjinlist = 0;
	rings[RING_INVENTORY]->ringactive = 1;
	rings[RING_AMMO]->objlistmovement = 0;
	rings[RING_AMMO]->curobjinlist = 0;
	rings[RING_AMMO]->ringactive = 0;
	handle_object_changeover(RING_INVENTORY);
	ammo_active = 0;
}

void insert_object_into_list(int num)
{
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].invitem = num;
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].yrot = 0;
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].bright = 32;
	rings[RING_INVENTORY]->numobjectsinlist++;
}

void draw_current_object_list(int ringnum)
{
	int n;
	int maxobj;
	int xoff;
	int i;
	int shade;
	int minobj;
	char textbufme[128];
	int objmeup;
	int nummeup;
	short ymeup;
	short yrot;
//	INVOBJ* objme;
	int activenum;

	if (rings[ringnum]->current_object_list <= 0)
		return;

	if (ringnum == RING_AMMO)
	{
		ammo_selector_fade_val = 0;
		ammo_selector_fade_dir = 0;

		if (combine_ring_fade_dir == 1)
		{
			if (combine_ring_fade_val < 128)
				combine_ring_fade_val += 32;

			if (combine_ring_fade_val > 128)
			{
				combine_ring_fade_val = 128;
				combine_ring_fade_dir = 0;
			}
		}
		else if (combine_ring_fade_dir == 2)
		{
			combine_ring_fade_val -= 32;

			if (combine_ring_fade_val <= 0)
			{
				combine_ring_fade_val = 0;
				combine_ring_fade_dir = 0;

				if (combine_type_flag)
					normal_ring_fade_dir = 2;
				else
				{
					rings[RING_INVENTORY]->ringactive = 1;
					menu_active = 1;
					rings[RING_AMMO]->ringactive = 0;
					handle_object_changeover(RING_INVENTORY);
				}

				rings[RING_AMMO]->ringactive = 0;
			}
		}
	}
	else if (normal_ring_fade_dir == 1)
	{
		if (normal_ring_fade_val < 128)
			normal_ring_fade_val += 32;

		if (normal_ring_fade_val > 128)
		{
			normal_ring_fade_val = 128;
			normal_ring_fade_dir = 0;
			rings[RING_INVENTORY]->ringactive = 1;
			menu_active = 1;
		}

	}
	else if (normal_ring_fade_dir == 2)
	{
		normal_ring_fade_val -= 32;

		if (normal_ring_fade_val <= 0)
		{
			normal_ring_fade_val = 0;
			normal_ring_fade_dir = 1;

			if (combine_type_flag == 1)
			{
				combine_type_flag = 0;
				combine_these_two_objects(combine_obj1, combine_obj2);
			}
			else if (seperate_type_flag)
				seperate_object(rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem);

			handle_object_changeover(RING_INVENTORY);
		}
	}

	minobj = 0;
	maxobj = 0;
	xoff = 0;
	n = 0;

	if (rings[ringnum]->numobjectsinlist != 1)
		xoff = (OBJLIST_SPACING * rings[ringnum]->objlistmovement) >> 16;

	if (rings[ringnum]->numobjectsinlist == 2)
	{
		minobj = -1;
		maxobj = 0;
		n = rings[ringnum]->curobjinlist - 1;
	}

	if (rings[ringnum]->numobjectsinlist == 3 || rings[ringnum]->numobjectsinlist == 4)
	{
		minobj = -2;
		maxobj = 1;
		n = rings[ringnum]->curobjinlist - 2;
	}

	if (rings[ringnum]->numobjectsinlist >= 5)
	{
		minobj = -3;
		maxobj = 2;
		n = rings[ringnum]->curobjinlist - 3;
	}

	if (n < 0)
		n += rings[ringnum]->numobjectsinlist;

	if (rings[ringnum]->objlistmovement < 0)
		maxobj++;

	if (minobj <= maxobj)
	{
		for (i = minobj; i <= maxobj; i++)
		{
			if (minobj == i)
			{
				if (rings[ringnum]->objlistmovement < 0)
					shade = 0;
				else
					shade = rings[ringnum]->objlistmovement >> 9;
			}
			else if (i != minobj + 1 || maxobj == minobj + 1)
			{
				if (i != maxobj)
					shade = 128;
				else
				{
					if (rings[ringnum]->objlistmovement < 0)
						shade = (-128 * rings[ringnum]->objlistmovement) >> 16;
					else
						shade = 128 - (short)(rings[ringnum]->objlistmovement >> 9);
				}
			}
			else
			{
				if (rings[ringnum]->objlistmovement < 0)
					shade = 128 - ((-128 * rings[ringnum]->objlistmovement) >> 16);
				else
					shade = 128;
			}

			if (!minobj && !maxobj)
				shade = 128;

			if (ringnum == RING_AMMO && combine_ring_fade_val < 128 && shade)
				shade = combine_ring_fade_val;
			else if (ringnum == RING_INVENTORY && normal_ring_fade_val < 128 && shade)
				shade = normal_ring_fade_val;

			if (!i)
			{
				nummeup = 0;

				switch (inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number)
				{
				case BIGMEDI_ITEM:
					nummeup = lara.num_large_medipack;
					break;

				case SMALLMEDI_ITEM:
					nummeup = lara.num_small_medipack;
					break;

				case FLARE_INV_ITEM:
					nummeup = lara.num_flares;
					break;

				default:
					if (inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number < PUZZLE_ITEM1 ||
						inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number > PUZZLE_ITEM8)
					{
						switch (inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number)
						{
						case SHOTGUN_AMMO1_ITEM:
							nummeup = lara.num_shotgun_ammo1 == -1 ? lara.num_shotgun_ammo1 : lara.num_shotgun_ammo1 / 6;
							break;

						case SHOTGUN_AMMO2_ITEM:
							nummeup = lara.num_crossbow_ammo2 == -1 ? lara.num_shotgun_ammo2 : lara.num_shotgun_ammo2 / 6;
							break;

						case HK_AMMO_ITEM:
							nummeup = lara.num_hk_ammo1;
							break;

						case CROSSBOW_AMMO1_ITEM:
							nummeup = lara.num_crossbow_ammo1;
							break;

						case CROSSBOW_AMMO2_ITEM:
							nummeup = lara.num_crossbow_ammo2;
							break;

						case REVOLVER_AMMO_ITEM:
							nummeup = lara.num_revolver_ammo;
							break;

						case UZI_AMMO_ITEM:
							nummeup = lara.num_uzi_ammo;
							break;

						case BOTTLE:
							nummeup = lara.bottle;
							break;

						case PICKUP_ITEM4:
							nummeup = savegame.Level.Secrets;
							break;
						}
					}
					else
					{
						nummeup = lara.puzzleitems[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number - PUZZLE_ITEM1];

						if (nummeup <= 1)
							sprintf(textbufme, &gfStringWad[gfStringOffset[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].objname]]);
						else
							sprintf(textbufme, "%d x %s", nummeup, &gfStringWad[gfStringOffset[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].objname]]);
					}

					break;
				}

				if (inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number < PUZZLE_ITEM1 ||
					inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number > PUZZLE_ITEM8)
				{
					if (nummeup)
					{
						if (inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].object_number == PICKUP_ITEM4)
							sprintf(textbufme, &gfStringWad[gfStringOffset_bis[STR_SECRETS_NUM]], nummeup, wanky_secrets_table[gfCurrentLevel]);
						else if (nummeup == -1)
							sprintf(textbufme, &gfStringWad[gfStringOffset[STR_UNLIMITED]], &gfStringWad[gfStringOffset[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].objname]]);
						else
							sprintf(textbufme, "%d x %s", nummeup, &gfStringWad[gfStringOffset[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].objname]]);
					}
					else
						sprintf(textbufme, &gfStringWad[gfStringOffset[inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].objname]]);
				}

				if (ringnum == RING_INVENTORY)
					objmeup = (int)(phd_centery - (phd_winymax + 1) * 0.0625 * 3.0);
				else
					objmeup = (int)((phd_winymax + 1) * 0.0625 * 3.0 + phd_centery);

				PrintString(phd_centerx, objmeup, 8, textbufme, FF_CENTER);
			}

			if (!i && !rings[ringnum]->objlistmovement)
			{
				if ((inventry_objects_list[rings[ringnum]->current_object_list[n].invitem].flags & 2))
					rings[ringnum]->current_object_list[n].yrot += 1022;
			}
			else
				spinback(&rings[ringnum]->current_object_list[n].yrot);

			yrot = rings[ringnum]->current_object_list[n].yrot;

			if (rings[ringnum]->objlistmovement)
			{
				if (rings[ringnum]->objlistmovement > 0)
					activenum = -1;
				else
					activenum = 1;
			}
			else
				activenum = 0;

			if (i == activenum)
			{
				if (rings[ringnum]->current_object_list[n].bright < 160)
					rings[ringnum]->current_object_list[n].bright += 16;

				if (rings[ringnum]->current_object_list[n].bright > 160)
					rings[ringnum]->current_object_list[n].bright = 160;
			}
			else
			{
				if (rings[ringnum]->current_object_list[n].bright > 32)
					rings[ringnum]->current_object_list[n].bright -= 16;

				if (rings[ringnum]->current_object_list[n].bright < 32)
					rings[ringnum]->current_object_list[n].bright = 32;
			}

			if (ringnum == RING_INVENTORY)
				ymeup = 42;
			else
				ymeup = 190;

			DrawThreeDeeObject2D((int)((phd_centerx * 0.00390625 * 256.0 + inventry_xpos) + xoff + i * OBJLIST_SPACING),
				(int)(phd_centery * 0.0083333338 * ymeup + inventry_ypos),
				rings[ringnum]->current_object_list[n].invitem,
				shade, 0, yrot, 0, rings[ringnum]->current_object_list[n].bright, 0);

			if (++n >= rings[ringnum]->numobjectsinlist)
				n = 0;
		}

		if (rings[ringnum]->ringactive)
		{
			if (rings[ringnum]->numobjectsinlist != 1 && (ringnum != 1 || combine_ring_fade_val == 128))
			{
				if (rings[ringnum]->objlistmovement > 0)
					rings[ringnum]->objlistmovement += 8192;

				if (rings[ringnum]->objlistmovement < 0)
					rings[ringnum]->objlistmovement -= 8192;

				if (go_left)
				{
					if (!rings[ringnum]->objlistmovement)
					{
						SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
						rings[ringnum]->objlistmovement += 8192;

						if (ammo_selector_flag)
							ammo_selector_fade_dir = 2;
					}
				}

				if (go_right)
				{
					if (!rings[ringnum]->objlistmovement)
					{
						SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
						rings[ringnum]->objlistmovement -= 8192;

						if (ammo_selector_flag)
							ammo_selector_fade_dir = 2;
					}

				}

				if (rings[ringnum]->objlistmovement < 65536)
				{
					if (rings[ringnum]->objlistmovement < -65535)
					{
						rings[ringnum]->curobjinlist++;

						if (rings[ringnum]->curobjinlist >= rings[ringnum]->numobjectsinlist)
							rings[ringnum]->curobjinlist = 0;

						rings[ringnum]->objlistmovement = 0;

						if (ringnum == RING_INVENTORY)
							handle_object_changeover(0);
					}
				}
				else
				{
					rings[ringnum]->curobjinlist--;

					if (rings[ringnum]->curobjinlist < 0)
						rings[ringnum]->curobjinlist = rings[ringnum]->numobjectsinlist - 1;

					rings[ringnum]->objlistmovement = 0;

					if (ringnum == RING_INVENTORY)
						handle_object_changeover(0);
				}
			}
		}
	}
}

void handle_object_changeover(int ringnum)
{
	current_selected_option = 0;
	menu_active = 1;
	setup_ammo_selector();
}

void handle_inventry_menu()
{
	int n;
	int opts;
	int i;
	int ypos;
	int num;

	if (rings[RING_AMMO]->ringactive)
	{
		PrintString(phd_centerx, phd_centery, 1, &gfStringWad[gfStringOffset[optmessages[5]]], FF_CENTER);

		if (rings[RING_INVENTORY]->objlistmovement)
			return;

		if (rings[RING_AMMO]->objlistmovement)
			return;

		if (go_select)
		{
			if (do_these_objects_combine(rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem, rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->curobjinlist].invitem))
			{
				combine_ring_fade_dir = 2;
				combine_type_flag = 1;
				combine_obj1 = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
				combine_obj2 = rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->curobjinlist].invitem;
				SoundEffect(SFX_MENU_COMBINE, 0, SFX_ALWAYS);
			}
			else
			{
				SayNo();
				combine_ring_fade_dir = 2;
			}
		}

		if (go_deselect)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			combine_ring_fade_dir = 2;
			go_deselect = 0;
		}

		return;
	}
	else
	{
		num = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;

		for (n = 0; n < 3; n++)
		{
			current_options[n].type = 0;
			current_options[n].text = 0;
		}

		n = 0;

		if (!ammo_active)
		{
			opts = options_table[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem];

			if ((opts & 0x1000))
			{
				current_options[0].type = 9;
				current_options[0].text = &gfStringWad[gfStringOffset[optmessages[6]]];
				n = 1;
			}

			if ((opts & 0x2000))
			{
				current_options[n].type = 10;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[7]]];
				n++;
			}

			if ((opts & 0x20))
			{
				current_options[n].type = 11;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[8]]];
				n++;
			}

			if ((opts & 0x8000))
			{
				current_options[n].type = 12;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[9]]];
				n++;
			}

			if ((opts & 0x4))
			{
				current_options[n].type = 1;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[0]]];
				n++;
			}

			if ((opts & 0x2))
			{
				current_options[n].type = 5;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[4]]];
				n++;
			}

			if ((opts & 0xC0))
			{
				current_options[n].type = 2;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[1]]];
				n++;
			}

			if ((opts & 0x100))
			{
				current_options[n].type = 2;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[10]]];
				n++;
			}

			if ((opts & 8))
			{
				if (is_item_currently_combinable(num))
				{
					current_options[n].type = 3;
					current_options[n].text = &gfStringWad[gfStringOffset[optmessages[2]]];
					n++;
				}
			}

			if ((opts & 0x1))
			{
				current_options[n].type = 3;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[2]]];
				n++;
			}

			if ((opts & 0x10))
			{
				current_options[n].type = 4;
				current_options[n].text = &gfStringWad[gfStringOffset[optmessages[3]]];
				n++;
			}
		}
		else
		{
			current_options[0].type = 6;
			current_options[0].text = &gfStringWad[gfStringOffset[inventry_objects_list[ammo_object_list[0].invitem].objname]];
			current_options[1].type = 7;
			current_options[1].text = &gfStringWad[gfStringOffset[inventry_objects_list[ammo_object_list[1].invitem].objname]];
			n = 2;

			if ((options_table[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem] & 0x100))
			{
				n = 3;
				current_options[2].type = 8;
				current_options[2].text = &gfStringWad[gfStringOffset[inventry_objects_list[ammo_object_list[2].invitem].objname]];

			}

			current_selected_option = current_ammo_type[0];
		}

		ypos = phd_centery - font_height;

		if (n == 1)
			ypos += font_height;
		else if (n == 2)
			ypos += font_height >> 1;

		if (n > 0)
		{
			for (i = 0; i < n; i++)
			{
				if (i == current_selected_option)
				{
					PrintString(phd_centerx, ypos, 1, current_options[i].text, FF_CENTER);
					ypos += font_height;
				}
				else
				{
					PrintString(phd_centerx, ypos, 5, current_options[i].text, FF_CENTER);
					ypos += font_height;
				}
			}
		}

		if (menu_active && !rings[RING_INVENTORY]->objlistmovement && !rings[RING_AMMO]->objlistmovement)
		{
			if (go_up && current_selected_option > 0)
			{
				current_selected_option--;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}
			else if (go_down && current_selected_option < n - 1)
			{
				current_selected_option++;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			if (ammo_active)
			{
				if (go_left && current_selected_option > 0)
				{
					current_selected_option--;
					SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				}

				if (go_right && current_selected_option < n - 1)
				{
					current_selected_option++;
					SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				}

				current_ammo_type[0] = current_selected_option;
			}

			if (go_select)
			{
				if (current_options[current_selected_option].type != 5 && current_options[current_selected_option].type != 1)
					SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);

				switch (current_options[current_selected_option].type)
				{
				case 2:
					rings[RING_INVENTORY]->ringactive = 0;
					ammo_active = 1;
					Stashedcurrent_selected_option = current_selected_option;
					StashedCurrentPistolsAmmoType = CurrentPistolsAmmoType;
					StashedCurrentUziAmmoType = CurrentUziAmmoType;
					StashedCurrentRevolverAmmoType = CurrentRevolverAmmoType;
					StashedCurrentShotGunAmmoType = CurrentShotGunAmmoType;
					StashedCurrentGrenadeGunAmmoType = CurrentGrenadeGunAmmoType;
					break;

				case 9:
					loading_or_saving = 1;
					break;

				case 10:
					loading_or_saving = 2;
					break;

				case 11:
					examine_mode = 1;
					break;

				case 12:
					stats_mode = 1;
					break;

				case 6:
				case 7:
				case 8:
					ammo_active = 0;
					rings[RING_INVENTORY]->ringactive = 1;
					current_selected_option = 0;
					break;

				case 3:
					construct_combine_object_list();
					rings[RING_INVENTORY]->ringactive = 0;
					rings[RING_AMMO]->ringactive = 1;
					ammo_selector_flag = 0;
					menu_active = 0;
					combine_ring_fade_dir = 1;
					break;

				case 4:
					seperate_type_flag = 1;
					normal_ring_fade_dir = 2;
					break;

				case 5:
				case 1:
					menu_active = 0;
					use_the_bitch = 1;
					break;
				}
			}

			if (go_deselect && ammo_active)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				go_deselect = 0;
				ammo_active = 0;
				rings[RING_INVENTORY]->ringactive = 1;
				CurrentPistolsAmmoType = StashedCurrentPistolsAmmoType;
				CurrentUziAmmoType = StashedCurrentUziAmmoType;
				CurrentRevolverAmmoType = StashedCurrentRevolverAmmoType;
				CurrentShotGunAmmoType = StashedCurrentShotGunAmmoType;
				CurrentGrenadeGunAmmoType = StashedCurrentGrenadeGunAmmoType;
				CurrentCrossBowAmmoType = StashedCurrentCrossBowAmmoType;
				current_selected_option = Stashedcurrent_selected_option;

			}
		}
	}
}

void setup_ammo_selector()
{
	int num;
	int opts;

	num = 0;
	opts = options_table[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem];
	ammo_selector_flag = 0;
	num_ammo_slots = 0;

	if (!rings[RING_AMMO]->ringactive)
	{
		ammo_object_list[2].yrot = 0;
		ammo_object_list[1].yrot = 0;
		ammo_object_list[0].yrot = 0;

		if (opts & 0x4FC0)
		{
			ammo_selector_flag = 1;
			ammo_selector_fade_dir = 1;

			if (opts & 0x200)
			{
				ammo_object_list[0].invitem = INV_UZI_AMMO_ITEM;
				ammo_object_list[0].amount = AmountUziAmmo;
				num = 1;
				num_ammo_slots = 1;
				current_ammo_type = &CurrentUziAmmoType;
			}

			if (opts & 0x400)
			{
				num = 1;
				ammo_object_list[0].invitem = INV_PISTOLS_AMMO_ITEM;
				ammo_object_list[0].amount = -1;
				num_ammo_slots = 1;
				current_ammo_type = &CurrentPistolsAmmoType;
			}

			if (opts & 0x800)
			{
				num = 1;
				ammo_object_list[0].invitem = INV_REVOLVER_AMMO_ITEM;
				ammo_object_list[0].amount = AmountRevolverAmmo;
				num_ammo_slots = 1;
				current_ammo_type = &CurrentRevolverAmmoType;
			}

			if (opts & 0x80)
			{
				current_ammo_type = &CurrentCrossBowAmmoType;
				ammo_object_list[num].invitem = INV_CROSSBOW_AMMO2_ITEM3;
				ammo_object_list[num].amount = AmountCrossBowAmmo1;
				num++;
				ammo_object_list[num].invitem = INV_CROSSBOW_AMMO2_ITEM4;
				ammo_object_list[num].amount = AmountCrossBowAmmo2;
				num++;
				num_ammo_slots = num;
			}

			if (opts & 0x100)
			{
				current_ammo_type = &CurrentGrenadeGunAmmoType;
				ammo_object_list[num].invitem = INV_HK_AMMO_ITEM1;
				ammo_object_list[num].amount = AmountHKAmmo1;
				num++;
				ammo_object_list[num].invitem = INV_HK_AMMO_ITEM2;
				ammo_object_list[num].amount = AmountHKAmmo1;
				num++;
				ammo_object_list[num].invitem = INV_HK_AMMO_ITEM3;
				ammo_object_list[num].amount = AmountHKAmmo1;
				num++;
				num_ammo_slots = num;
			}

			if (opts & 0x40)
			{
				current_ammo_type = &CurrentShotGunAmmoType;
				ammo_object_list[num].invitem = INV_SHOTGUN_AMMO1_ITEM;
				ammo_object_list[num].amount = AmountShotGunAmmo1;
				num++;
				ammo_object_list[num].invitem = INV_SHOTGUN_AMMO2_ITEM;
				ammo_object_list[num].amount = AmountShotGunAmmo2;
				num++;
				num_ammo_slots = num;
			}

			if (opts & 0x4000)
			{
				ammo_object_list[0].invitem = INV_CROSSBOW_AMMO1_ITEM;
				ammo_object_list[0].amount = AmountCrossBowAmmo1;
				num_ammo_slots = 1;
				current_ammo_type = &CurrentCrossBowAmmoType;
			}
		}
	}
}

void fade_ammo_selector()
{
	if (rings[RING_INVENTORY]->ringactive && (right_repeat >= 8 || left_repeat >= 8))
		ammo_selector_fade_val = 0;
	else if (ammo_selector_fade_dir == 1)
	{
		if (ammo_selector_fade_val < 128)
			ammo_selector_fade_val += 32;

		if (ammo_selector_fade_val > 128)
		{
			ammo_selector_fade_val = 128;
			ammo_selector_fade_dir = 0;
		}
	}
	else if (ammo_selector_fade_dir == 2)
	{
		if (ammo_selector_fade_val > 0)
			ammo_selector_fade_val -= 32;

		if (ammo_selector_fade_val < 0)
		{
			ammo_selector_fade_val = 0;
			ammo_selector_fade_dir = 0;
		}
	}
}

void draw_ammo_selector()
{
	int n;
	int xpos;
	short yrot;
	INVOBJ* objme;
	char cunter[256];

	if (!ammo_selector_flag)
		return;

	xpos = (2 * phd_centerx - OBJLIST_SPACING) >> 1;

	if (num_ammo_slots == 2)
		xpos -= OBJLIST_SPACING / 2;
	else if (num_ammo_slots == 3)
		xpos -= OBJLIST_SPACING;

	if (num_ammo_slots > 0)
	{
		for (n = 0; n < num_ammo_slots; n++)
		{
			objme = &inventry_objects_list[ammo_object_list[n].invitem];

			if (n == current_ammo_type[0])
			{
				if ((objme->flags & 2))
					ammo_object_list[n].yrot += 1022;
			}
				else
					spinback(&ammo_object_list[n].yrot);

			yrot = ammo_object_list[n].yrot;

			if (n == current_ammo_type[0])
			{
				if (ammo_object_list[n].amount == -1)
					sprintf(&cunter[0], &gfStringWad[gfStringOffset[STR_UNLIMITED]], &gfStringWad[gfStringOffset[inventry_objects_list[ammo_object_list[n].invitem].objname]]);
				else
					sprintf(&cunter[0], "%d x %s", ammo_object_list[n].amount, &gfStringWad[gfStringOffset[inventry_objects_list[ammo_object_list[n].invitem].objname]]);

				if (ammo_selector_fade_val)
					PrintString(phd_centerx, font_height + phd_centery + 2 * font_height - 9, 8, &cunter[0], FF_CENTER);

				if (n == current_ammo_type[0])
					DrawThreeDeeObject2D((int)(phd_centerx * 0.00390625 * 64.0 + inventry_xpos + xpos), (int)(phd_centery * 0.0083333338 * 190.0 + inventry_ypos), ammo_object_list[n].invitem, ammo_selector_fade_val, 0, yrot, 0, 0, 0);
				else
					DrawThreeDeeObject2D((int)(phd_centerx * 0.00390625 * 64.0 + inventry_xpos + xpos), (int)(phd_centery * 0.0083333338 * 190.0 + inventry_ypos), ammo_object_list[n].invitem, ammo_selector_fade_val, 0, yrot, 0, 1, 0);
			}
			else
				DrawThreeDeeObject2D((int)(phd_centerx * 0.00390625 * 64.0 + inventry_xpos + xpos), (int)(phd_centery * 0.0083333338 * 190.0 + inventry_ypos), ammo_object_list[n].invitem, ammo_selector_fade_val, 0, yrot, 0, 1, 0);

			xpos += OBJLIST_SPACING;
		}
	}
}

void spinback(ushort* cock)
{
	ushort val;
	ushort val2;

	val = *cock;

	if (val)
	{
		if (val <= 32768)
		{
			val2 = val;

			if (val2 < 1022)
				val = 1022;
			else if (val2 > 16384)
				val2 = 16384;

			val -= (val2 >> 3);

			if (val > 32768)
				val = 0;
		}
		else
		{
			val2 = -val;

			if (val2 < 1022)
				val = 1022;
			else if (val2 > 16384)
				val2 = 16384;

			val += (val2 >> 3);

			if (val < 32768)
				val = 0;
		}

		*cock = val;
	}
}

void update_laras_weapons_status()
{
	if (lara.shotgun_type_carried & WTYPE_PRESENT)
	{
		lara.shotgun_type_carried &= ~WTYPE_MASK_AMMO;

		if (CurrentShotGunAmmoType)
			lara.shotgun_type_carried |= WTYPE_AMMO_2;
		else
			lara.shotgun_type_carried |= WTYPE_AMMO_1;
	}

	if (lara.hk_type_carried & WTYPE_PRESENT)
	{
		lara.hk_type_carried &= ~WTYPE_MASK_AMMO;

		if (CurrentGrenadeGunAmmoType == 0)
			lara.hk_type_carried |= WTYPE_AMMO_1;
		else if (CurrentGrenadeGunAmmoType == 1)
			lara.hk_type_carried |= WTYPE_AMMO_2;
		else if (CurrentGrenadeGunAmmoType == 2)
			lara.hk_type_carried |= WTYPE_AMMO_3;
	}

	if (lara.crossbow_type_carried & WTYPE_PRESENT)
	{
		lara.crossbow_type_carried &= ~WTYPE_MASK_AMMO;
		if (CurrentCrossBowAmmoType)
			lara.crossbow_type_carried |= WTYPE_AMMO_2;
		else
			lara.crossbow_type_carried |= WTYPE_AMMO_1;
	}
}

int is_item_currently_combinable(short obj)
{
	for (int n = 0; n < 24; n++)
	{
		if (dels_handy_combine_table[n].item1 == obj)
			if (have_i_got_item(dels_handy_combine_table[n].item2))
				return 1;

		if (dels_handy_combine_table[n].item2 == obj)
			if (have_i_got_item(dels_handy_combine_table[n].item1))
				return 1;
	}

	return 0;
}

int have_i_got_item(short obj)
{
	for (int i = 0; i < 100; i++)
		if (rings[RING_INVENTORY]->current_object_list[i].invitem == obj)
			return 1;

	return 0;
}

int do_these_objects_combine(int obj1, int obj2)
{
	for (int n = 0; n < 24; n++)
	{
		if (dels_handy_combine_table[n].item1 == obj1 &&
			dels_handy_combine_table[n].item2 == obj2)
			return 1;

		if (dels_handy_combine_table[n].item1 == obj2 &&
			dels_handy_combine_table[n].item2 == obj1)
			return 1;
	}

	return 0;
}

void combine_these_two_objects(short obj1, short obj2)
{
	int n;

	for (n = 0; n < 24; n++)
	{
		if (dels_handy_combine_table[n].item1 == obj1 &&
			dels_handy_combine_table[n].item2 == obj2)
			break;

		if (dels_handy_combine_table[n].item1 == obj2 &&
			dels_handy_combine_table[n].item2 == obj1)
			break;
	}

	dels_handy_combine_table[n].combine_routine(0);
	construct_object_list();
	setup_objectlist_startposition(dels_handy_combine_table[n].combined_item);
	handle_object_changeover(0);
}

void seperate_object(short obj)
{
	int n;

	for (n = 0; n < 24; n++)
		if (dels_handy_combine_table[n].combined_item == obj)
			break;

	dels_handy_combine_table[n].combine_routine(1);
	construct_object_list();
	setup_objectlist_startposition(dels_handy_combine_table[n].item1);
}

void combine_HK_SILENCER(int flag)
{
	if (flag)
	{
		lara.silencer = 1;
		lara.hk_type_carried &= ~WTYPE_SILENCER;
		
	}
	else
	{
		lara.silencer = 0;
		lara.hk_type_carried |= WTYPE_SILENCER;
	}
}

void combine_revolver_lasersight(int flag)
{
	if (flag)
	{
		lara.lasersight = 1;
		lara.sixshooter_type_carried &= ~WTYPE_LASERSIGHT;
	}
	else
	{
		lara.lasersight = 0;
		lara.sixshooter_type_carried |= WTYPE_LASERSIGHT;
	}

	if (lara.gun_status && lara.gun_type == WEAPON_REVOLVER)
	{
		undraw_pistol_mesh_right(WEAPON_REVOLVER);
		draw_pistol_meshes(WEAPON_REVOLVER);
	}
}

void combine_crossbow_lasersight(int flag)
{
	if (flag)
	{
		lara.lasersight = 1;
		lara.crossbow_type_carried &= ~WTYPE_LASERSIGHT;
	}
	else
	{
		lara.lasersight = 0;
		lara.crossbow_type_carried |= WTYPE_LASERSIGHT;
	}

	if (lara.gun_status && lara.gun_type == WEAPON_CROSSBOW)
	{
		undraw_shotgun_meshes(WEAPON_CROSSBOW);
		draw_shotgun_meshes(WEAPON_CROSSBOW);
	}
}

void combine_PuzzleItem1(int flag)
{
	lara.puzzleitemscombo &= 0xFFFC;
	lara.puzzleitems[0] = 1;
}

void combine_PuzzleItem2(int flag)
{
	lara.puzzleitemscombo &= 0xFFF3;
	lara.puzzleitems[1] = 1;
}

void combine_PuzzleItem3(int flag)
{
	lara.puzzleitemscombo &= 0xFFCF;
	lara.puzzleitems[2] = 1;
}

void combine_PuzzleItem4(int flag)
{
	lara.puzzleitemscombo &= 0xFF3F;
	lara.puzzleitems[3] = 1;
}

void combine_PuzzleItem5(int flag)
{
	lara.puzzleitemscombo &= 0xFCFF;
	lara.puzzleitems[4] = 1;
}

void combine_PuzzleItem6(int flag)
{
	lara.puzzleitemscombo &= 0xF3FF;
	lara.puzzleitems[5] = 1;
}

void combine_PuzzleItem7(int flag)
{
	lara.puzzleitemscombo &= 0xCFFF;
	lara.puzzleitems[6] = 1;
}

void combine_PuzzleItem8(int flag)
{
	lara.puzzleitemscombo &= 0x3FFF;
	lara.puzzleitems[7] = 1;
}

void combine_KeyItem1(int flag)
{
	lara.keyitems |= 1;
	lara.keyitemscombo &= 0xFFFC;
}

void combine_KeyItem2(int flag)
{
	lara.keyitems |= 2;
	lara.keyitemscombo &= 0xFFF3;
}

void combine_KeyItem3(int flag)
{
	lara.keyitems |= 4;
	lara.keyitemscombo &= 0xFFCF;
}

void combine_KeyItem4(int flag)
{
	lara.keyitems |= 8;
	lara.keyitemscombo &= 0xFF3F;
}

void combine_KeyItem5(int flag)
{
	lara.keyitems |= 16;
	lara.keyitemscombo &= 0xFCFF;
}

void combine_KeyItem6(int flag)
{
	lara.keyitems |= 32;
	lara.keyitemscombo &= 0xF3FF;
}

void combine_KeyItem7(int flag)
{
	lara.keyitems |= 64;
	lara.keyitemscombo &= 0xCFFF;
}

void combine_KeyItem8(int flag)
{
	lara.keyitems |= 128;
	lara.keyitemscombo &= 0x3FFF;
}

void combine_PickupItem1(int flag)
{
	lara.pickupitems |= 1;
	lara.pickupitemscombo &= 0xFFFC;
}

void combine_PickupItem2(int flag)
{
	lara.pickupitems |= 2;
	lara.pickupitemscombo &= 0xFFF3;
}

void combine_PickupItem3(int flag)
{
	lara.pickupitems |= 4;
	lara.pickupitemscombo &= 0xFFCF;
}

void combine_PickupItem4(int flag)
{
	lara.pickupitems |= 8;
	lara.pickupitemscombo &= 0xFF3F;
}

void combine_clothbottle(int flag)
{
	lara.wetcloth = CLOTH_WET;
	lara.bottle--;
}

void setup_objectlist_startposition(short newobj)
{
	for (int i = 0; i < 100; i++)
		if (rings[RING_INVENTORY]->current_object_list[i].invitem == newobj)
			rings[RING_INVENTORY]->curobjinlist = i;
}

void setup_objectlist_startposition2(short newobj)
{
	for (int i = 0; i < 100; i++)
		if (inventry_objects_list[rings[RING_INVENTORY]->current_object_list[i].invitem].object_number == newobj)
			rings[RING_INVENTORY]->curobjinlist = i;
}

void use_current_item()
{
	short invobject, gmeobject;
	long OldBinocular;

	OldBinocular = BinocularRange;
	oldLaraBusy = 0;
	BinocularRange = 0;
	lara_item->mesh_bits = -1;
	invobject = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
	gmeobject = inventry_objects_list[invobject].object_number;

	if (gfCurrentLevel == LVL5_DEEPSEA_DIVE && gmeobject == PUZZLE_ITEM1)
	{
		FireChaff();
		return;
	}

	if (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE)
	{
		if (gmeobject == PISTOLS_ITEM)
		{
			lara.request_gun_type = WEAPON_PISTOLS;

			if (lara.gun_status != LG_NO_ARMS)
				return;

			if (lara.gun_type == WEAPON_PISTOLS)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == UZI_ITEM)
		{
			lara.request_gun_type = WEAPON_UZI;

			if (lara.gun_status != LG_NO_ARMS)
				return;

			if (lara.gun_type == WEAPON_UZI)
				lara.gun_status = LG_DRAW_GUNS;

			return;

		}
	}

	if (gmeobject != SHOTGUN_ITEM && gmeobject != REVOLVER_ITEM && gmeobject != HK_ITEM && gmeobject != CROSSBOW_ITEM)
	{
		if (gmeobject == FLARE_INV_ITEM)
		{
			if (lara.gun_status == LG_NO_ARMS)
			{
				if (lara_item->current_anim_state != AS_ALL4S &&
					lara_item->current_anim_state != AS_CRAWL &&
					lara_item->current_anim_state != AS_ALL4TURNL &&
					lara_item->current_anim_state != AS_ALL4TURNR &&
					lara_item->current_anim_state != AS_CRAWLBACK &&
					lara_item->current_anim_state != AS_CRAWL2HANG)
				{
					if (lara.gun_type != 7)
					{
						input = IN_FLARE;
						LaraGun();
						input = 0;
					}

					return;
				}
			}

			SayNo();
			return;
		}

		switch (invobject)
		{
		case INV_BINOCULARS_ITEM:

			if ((lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH
				|| lara.IsDucked && !(input & IN_DUCK))
				&& !SniperCamActive
				&& !bUseSpotCam
				&& !bTrackCamInit)
			{
				oldLaraBusy = 1;
				BinocularRange = 128;

				if (lara.gun_status != LG_NO_ARMS)
					lara.gun_status = LG_UNDRAW_GUNS;
			}

			if (OldBinocular)
				BinocularRange = OldBinocular;
			else
				BinocularOldCamera = camera.old_type;

			return;

		case INV_SMALLMEDI_ITEM:

			if ((lara_item->hit_points <= 0 || lara_item->hit_points >= 1000) && !lara.poisoned)
			{
				SayNo();
				return;
			}

			if (lara.num_small_medipack != -1)
				lara.num_small_medipack--;

			lara.dpoisoned = 0;
			lara_item->hit_points += 500;

			if (lara_item->hit_points > 1000)
				lara_item->hit_points = 1000;

			SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
			savegame.Game.HealthUsed++;
			return;

		case INV_BIGMEDI_ITEM:

			if ((lara_item->hit_points <= 0 || lara_item->hit_points >= 1000) && !lara.poisoned)
			{
				SayNo();
				return;
			}

			if (lara.num_large_medipack != -1)
				lara.num_large_medipack--;

			lara.dpoisoned = 0;
			lara_item->hit_points += 1000;

			if (lara_item->hit_points > 1000)
				lara_item->hit_points = 1000;

			SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
			savegame.Game.HealthUsed++;
			return;

		default:
			GLOBAL_inventoryitemchosen = gmeobject;
			return;
		}

		return;
	}

	if (lara.gun_status == LG_HANDS_BUSY)
	{
		SayNo();
		return;
	}

	if (lara_item->current_anim_state == AS_ALL4S ||
		lara_item->current_anim_state == AS_CRAWL ||
		lara_item->current_anim_state == AS_ALL4TURNL ||
		lara_item->current_anim_state == AS_ALL4TURNR ||
		lara_item->current_anim_state == AS_CRAWLBACK ||
		lara_item->current_anim_state == AS_CRAWL2HANG ||
		lara_item->current_anim_state == AS_DUCK ||
		lara_item->current_anim_state == AS_DUCKROTL ||
		lara_item->current_anim_state == AS_DUCKROTR)
	{
		SayNo();
		return;
	}

	if (gmeobject == SHOTGUN_ITEM)
	{
		lara.request_gun_type = WEAPON_SHOTGUN;

		if (lara.gun_status != LG_NO_ARMS)
			return;

		if (lara.gun_type == 4)
			lara.gun_status = LG_DRAW_GUNS;

		return;
	}

	if (gmeobject == REVOLVER_ITEM)
	{
		lara.request_gun_type = WEAPON_REVOLVER;

		if (lara.gun_status != LG_NO_ARMS)
			return;

		if (lara.gun_type == WEAPON_REVOLVER)
			lara.gun_status = WEAPON_REVOLVER;

		return;
	}
	else if (gmeobject == HK_ITEM)
	{
		lara.request_gun_type = WEAPON_HK;

		if (lara.gun_status != 0)
			return;

		if (lara.gun_type == 5)
			lara.gun_status = 2;

		return;
	}
	else
	{
		lara.request_gun_type = WEAPON_CROSSBOW;

		if (lara.gun_status != LG_NO_ARMS)
			return;

		if (lara.gun_type == WEAPON_CROSSBOW)
			lara.gun_status = 2;

		return;
	}
}

void DEL_picked_up_object(short objnum)
{
	switch (objnum)
	{
	case UZI_ITEM:
		if (!(lara.uzis_type_carried & WTYPE_PRESENT))
			lara.uzis_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

		if (lara.num_uzi_ammo != -1)
			lara.num_uzi_ammo += 30;

		return;

	case PISTOLS_ITEM:
		if (!(lara.uzis_type_carried & WTYPE_PRESENT))
			lara.pistols_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

		lara.num_pistols_ammo = -1;
		return;

	case SHOTGUN_ITEM:
		if (!(lara.shotgun_type_carried & WTYPE_PRESENT))
			lara.shotgun_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

		if (lara.num_shotgun_ammo1 != -1)
			lara.num_shotgun_ammo1 += 36;

		return;

	case REVOLVER_ITEM:
		if (!(lara.sixshooter_type_carried & WTYPE_PRESENT))
			lara.sixshooter_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

		if (lara.num_revolver_ammo != -1)
			lara.num_revolver_ammo += 6;

		return;

	case CROSSBOW_ITEM:
		if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT)
		{
			if (!(lara.crossbow_type_carried & WTYPE_PRESENT))
				lara.crossbow_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

			if (lara.num_crossbow_ammo1 != -1)
				lara.num_crossbow_ammo1 += 10;
		}
		else
		{
			lara.crossbow_type_carried = WTYPE_PRESENT | WTYPE_LASERSIGHT | WTYPE_AMMO_1;
			lara.num_crossbow_ammo2 = 0;
		}

		return;

	case HK_ITEM:
		SetCutNotPlayed(23);

		if (!(lara.hk_type_carried & WTYPE_PRESENT))
			lara.hk_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

		if (gfCurrentLevel != LVL5_ESCAPE_WITH_THE_IRIS)
			if (lara.num_hk_ammo1 != -1)
				lara.num_hk_ammo1 += 30;

		return;

	case SHOTGUN_AMMO1_ITEM:
		if (lara.num_shotgun_ammo1 != -1)
			lara.num_shotgun_ammo1 += 36;

		return;

	case SHOTGUN_AMMO2_ITEM:
		if (lara.num_shotgun_ammo2 != -1)
			lara.num_shotgun_ammo2 += 36;

		return;

	case HK_AMMO_ITEM:
		if (lara.num_hk_ammo1 != -1)
			lara.num_hk_ammo1 += 30;

		return;

	case CROSSBOW_AMMO1_ITEM:
		if (lara.num_crossbow_ammo1 != -1)
			lara.num_crossbow_ammo1 += 10;

		return;

	case CROSSBOW_AMMO2_ITEM:
		if (lara.num_crossbow_ammo2 != -1)
			lara.num_crossbow_ammo2 += 10;

		return;

	case REVOLVER_AMMO_ITEM:
		if (lara.num_revolver_ammo != -1)
			lara.num_revolver_ammo += 6;

		return;

	case UZI_AMMO_ITEM:
		if (lara.num_uzi_ammo != -1)
			lara.num_uzi_ammo += 30;

		return;
		
	case FLARE_INV_ITEM:
		if (lara.num_flares != -1)
			lara.num_flares += 12;

		return;

	case SILENCER_ITEM:
		if (!((lara.uzis_type_carried |lara.pistols_type_carried | lara.shotgun_type_carried | lara.sixshooter_type_carried | lara.crossbow_type_carried | lara.hk_type_carried) & WTYPE_SILENCER))
			lara.silencer = 1;

		return;

	case LASERSIGHT_ITEM:
		if (!((lara.uzis_type_carried | lara.pistols_type_carried | lara.shotgun_type_carried | lara.sixshooter_type_carried | lara.crossbow_type_carried | lara.hk_type_carried) & WTYPE_LASERSIGHT))
			lara.lasersight = 1;

		return;

	case BIGMEDI_ITEM:
		lara.num_large_medipack++;
		return;

	case SMALLMEDI_ITEM:
		lara.num_small_medipack++;
		return;

	case BINOCULARS_ITEM:
		lara.binoculars = 1;
		return;

	case PICKUP_ITEM4:
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_SECRET, 0);
		lara.pickupitems |= 8;
		savegame.Level.Secrets++;
		savegame.Game.Secrets++;

		if (gfCurrentLevel >= LVL5_THIRTEENTH_FLOOR && gfCurrentLevel <= LVL5_RED_ALERT)
			savegame.CampaignSecrets[3]++;
		else if (gfCurrentLevel >= LVL5_BASE && gfCurrentLevel <= LVL5_SINKING_SUBMARINE)
			savegame.CampaignSecrets[2]++;
		else if (gfCurrentLevel >= LVL5_STREETS_OF_ROME && gfCurrentLevel <= LVL5_COLOSSEUM)
			savegame.CampaignSecrets[0]++;
		else if (gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL)
			savegame.CampaignSecrets[1]++;

		return;

	case CROWBAR_ITEM:
		lara.crowbar = 1;
		return;

	case EXAMINE1:
		lara.examine1 = 1;
		return;

	case EXAMINE2:
		lara.examine2 = 1;
		return;

	case EXAMINE3:
		lara.examine3 = 1;
		return;

	case WET_CLOTH:
		lara.wetcloth = CLOTH_WET;
		return;

	case CLOTH:
		lara.wetcloth = CLOTH_DRY;
		return;

	case BOTTLE:
		lara.bottle++;
		return;

	default:

		if (objnum >= PICKUP_ITEM1 && objnum <= PICKUP_ITEM4)
			lara.pickupitems |= 1 << (objnum - PICKUP_ITEM1);
		else if (objnum >= PICKUP_ITEM1_COMBO1 && objnum <= PICKUP_ITEM4_COMBO2)
			lara.pickupitemscombo |= 1 << (objnum - PICKUP_ITEM1_COMBO1);
		else if (objnum >= KEY_ITEM1 && objnum <= KEY_ITEM8)
			lara.keyitems |= 1 << (objnum - KEY_ITEM1);
		else if (objnum >= KEY_ITEM1_COMBO1 && objnum <= KEY_ITEM8_COMBO2)
			lara.keyitemscombo |= 1 << (objnum - KEY_ITEM1_COMBO1);
		else if (objnum >= PUZZLE_ITEM1 && objnum <= PUZZLE_ITEM8)
			lara.puzzleitems[objnum - PUZZLE_ITEM1]++;
		else if (objnum >= PUZZLE_ITEM1_COMBO1 && objnum <= PUZZLE_ITEM8_COMBO2)
			lara.puzzleitemscombo |= 1 << (objnum - PUZZLE_ITEM1_COMBO1);
	}
}

void NailInvItem(short objnum)
{
	switch (objnum)
	{
	case UZI_ITEM:
		lara.uzis_type_carried = WTYPE_MISSING;
		lara.num_uzi_ammo = 0;
		break;

	case PISTOLS_ITEM:
		lara.holster = LARA_HOLSTERS;
		lara.pistols_type_carried = WTYPE_MISSING;
		lara.gun_status = LG_NO_ARMS;
		lara.last_gun_type = WEAPON_NONE;
		lara.gun_type = WEAPON_NONE;
		lara.request_gun_type = WEAPON_NONE;
		break;

	case SHOTGUN_ITEM:
		lara.shotgun_type_carried = WTYPE_MISSING;
		lara.num_shotgun_ammo1 = 0;
		break;

	case REVOLVER_ITEM:
		lara.sixshooter_type_carried = WTYPE_MISSING;
		lara.num_revolver_ammo = 0;
		break;

	case CROSSBOW_ITEM:
		lara.crossbow_type_carried = WTYPE_MISSING;
		lara.num_crossbow_ammo1 = 0;
		break;

	case HK_ITEM:
		lara.hk_type_carried = WTYPE_MISSING;
		lara.num_hk_ammo1 = 0;
		break;

	case FLARE_INV_ITEM:
		lara.num_flares = 0;
		break;

	case SILENCER_ITEM:
		lara.silencer = WTYPE_MISSING;
		break;

	case LASERSIGHT_ITEM:
		lara.lasersight = WTYPE_MISSING;
		break;

	case BIGMEDI_ITEM:
		lara.num_large_medipack = 0;
		break;

	case SMALLMEDI_ITEM:
		lara.num_small_medipack = 0;
		break;

	case BINOCULARS_ITEM:
		lara.binoculars = WTYPE_MISSING;
		break;

	case CROWBAR_ITEM:
		lara.crowbar = 0;
		break;

	case EXAMINE1:
		lara.examine1 = 0;
		break;

	case EXAMINE2:
		lara.examine2 = 0;
		break;

	case EXAMINE3:
		lara.examine3 = 0;
		break;

	case WET_CLOTH:
		lara.wetcloth = CLOTH_MISSING;
		break;

	case CLOTH:
		lara.wetcloth = CLOTH_MISSING;
		break;

	case BOTTLE:
		lara.bottle = 0;
		break;

	default:

		if (objnum >= PICKUP_ITEM1 && objnum <= PICKUP_ITEM4)
			lara.pickupitems &= ~(1 << (objnum - PICKUP_ITEM1));
		else if (objnum >= PICKUP_ITEM1_COMBO1 && objnum <= PICKUP_ITEM4_COMBO2)
			lara.pickupitemscombo &= ~(1 << (objnum - PICKUP_ITEM1_COMBO1));
		else if (objnum >= KEY_ITEM1 && objnum <= KEY_ITEM8)
			lara.keyitems &= ~(1 << (objnum - KEY_ITEM1));
		else if (objnum >= KEY_ITEM1_COMBO1 && objnum <= KEY_ITEM8_COMBO2)
			lara.keyitemscombo &= ~(1 << (objnum - KEY_ITEM1_COMBO1));
		else if (objnum >= PUZZLE_ITEM1 && objnum <= PUZZLE_ITEM8)
			lara.puzzleitems[objnum - PUZZLE_ITEM1] = 0;
		else if (objnum >= PUZZLE_ITEM1_COMBO1 && objnum <= PUZZLE_ITEM8_COMBO2)
			lara.puzzleitemscombo &= ~(1 << (objnum - PUZZLE_ITEM1_COMBO1));

		break;
	}
}

int have_i_got_object(short object_number)
{
	if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
		return (lara.puzzleitemscombo >> (object_number - PUZZLE_ITEM1_COMBO1)) & 1;

	if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM8)
		return lara.puzzleitems[object_number - PUZZLE_ITEM1];

	if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
		return (lara.keyitemscombo >> (object_number - KEY_ITEM1_COMBO1)) & 1;

	if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM8)
		return (lara.keyitems >> (object_number - KEY_ITEM1)) & 1;

	if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
		return (lara.pickupitemscombo >> (object_number - PICKUP_ITEM1_COMBO1)) & 1;

	if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
		return (lara.pickupitems >> (object_number - PICKUP_ITEM1)) & 1;

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
	for (int i = 0; i < 100; i++)
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

	if (keypadnuminputs)
	{
		for (n = 0; n < (int)keypadnuminputs; n++)
		{
			val2 = keypadinputs[n];

			if (!val2)
				val2 = 11;

			val = val & ~(1 << (val2 & 31)) | 1 << (val2 + 12 & 31);
		}
	}

	objme = &inventry_objects_list[INV_PUZZLE_HOLE8];

	if (!(GnFrameCounter & 2) && !keypadpause)
		objme->meshbits = val & ~(1 << (((3 * keypady + keypadx) + 13) & 0x1F)) | 1 << (((3 * keypady + keypadx) + 1) & 0x1F);
	else
		objme->meshbits = val & ~(1 << (((keypadx + 3 * keypady) + 1) & 0x1F)) | 1 << (((keypadx + 3 * keypady) + 13) & 0x1F);

	DrawThreeDeeObject2D((int)(phd_centerx * 0.00390625 * 256.0 + inventry_xpos), (int)((phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2), INV_PUZZLE_HOLE8, 128, 0x8000, 0x4000, 0x4000, 0, 0);
	PrintString(0x100, (ushort)((phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2 - 64), 6, &gfStringWad[gfStringOffset_bis[STR_ENTER_COMBINATION]], FF_CENTER);
	buf[0] = 45;
	buf[1] = 45;
	buf[2] = 45;
	buf[3] = 45;
	buf[5] = 45;

	if (keypadnuminputs)
		for (n = 0; n < keypadnuminputs; n++)
			buf[n] = keypadinputs[n] + 48;

	PrintString(0x100, (ushort)((phd_centery * 0.0083333338 * 256.0 + inventry_ypos) / 2 + 64), 1, buf, FF_CENTER);

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
		uchar va = keypady * 3 + keypadx + 1;
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

		if (keypadnuminputs)
		{
			do
			{
				if (keypadinputs[n] == va)
					return;

				n++;

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
	INVOBJ* objme;
	int saved_scale;

	objme = &inventry_objects_list[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem];
	saved_scale = objme->scale1;
	examine_mode += 8;

	if (examine_mode > 128)
		examine_mode = 128;

	objme->scale1 = 300;
	DrawThreeDeeObject2D((int)(phd_centerx + inventry_xpos), (int)(phd_centery / 120.0 * 256.0 + inventry_xpos) / 2,
		rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem,
		examine_mode, 32768, 16384, 16384, 96, 0);
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
	if (stats_mode > 128)
		stats_mode = 128;

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
#ifdef VER_JP
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
#endif
	return;
}

void dels_give_lara_guns_cheat()
{
#ifdef VER_JP
	//actually this isn't in the JP exe either, it's taken from PSX code
	if (objects[FLARE_INV_ITEM].loaded)
		lara.num_flares = -1;

	lara.num_small_medipack = -1;
	lara.num_large_medipack = -1;

	if (!(gfLevelFlags & GF_YOUNGLARA))
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
#else
	return;
#endif
}

int LoadGame()
{
	return S_LoadSave(IN_LOAD, 1) < 0 ? -1 : 1;
}

int SaveGame()
{
	input = 0;
	dbinput = 0;
	return S_LoadSave(IN_SAVE, 1) < 0 ? -1 : 1;
}

void DelDrawSprite(int x, int y, int def, int z)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT Tex;
	long x1, y1, x2, y2, x3, y3, x4, y4;
	float u1, u2, v1, v2;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + def];

	if (z >= 200)
		z = (int)(f_mzfar - 20.0);
	else
		z = (int)(f_mznear + 20.0);

	x1 = x4 = (long)((float)x * (float)phd_centerx * (1.0f / 256.0f));
	x2 = x3 = (long)(((float)((sprite->width >> 8) + x + 1)) * (float)phd_centerx * (1.0f / 256.0f));
	y1 = y2 = (long)((float)y * (float)phd_centery * (1.0f / 120.0f));
	y3 = y4 = (long)(((float)((sprite->height >> 8) + y + 1)) * (float)phd_centery * (1.0f / 120.0f));
	setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z, clipflags);
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	v[0].color = 0xFFFFFFFF;
	v[1].color = 0xFFFFFFFF;
	v[2].color = 0xFFFFFFFF;
	v[3].color = 0xFFFFFFFF;
	u1 = sprite->x1;
	v1 = sprite->y1;
	u2 = sprite->x2;
	v2 = sprite->y2;
	Tex.drawtype = 1;
	Tex.flag = 0;
	Tex.tpage = sprite->tpage;
	Tex.u1 = u1;
	Tex.v1 = v1;
	Tex.u2 = u2;
	Tex.v2 = v1;
	Tex.u3 = u2;
	Tex.v3 = v2;
	Tex.u4 = u1;
	Tex.v4 = v2;
	AddQuadClippedSorted(v, 0, 1, 2, 3, &Tex, 0);
}

void inject_newinv2(bool replace)
{
	INJECT(0x0045F9D0, S_CallInventory2, replace);
	INJECT(0x0045FEF0, init_new_inventry, replace);
	INJECT(0x004601A0, do_debounced_joystick_poo, replace);
	INJECT(0x00460350, DrawThreeDeeObject2D, replace);
	INJECT(0x00460580, DrawInventoryItemMe, replace);
	INJECT(0x00460920, go_and_load_game, replace);
	INJECT(0x00460940, go_and_save_game, replace);
	INJECT(0x00460960, construct_combine_object_list, replace);
	INJECT(0x00460B40, insert_object_into_list_v2, replace);
	INJECT(0x00460BD0, construct_object_list, replace);
	INJECT(0x00461120, insert_object_into_list, replace);
	INJECT(0x00461190, draw_current_object_list, replace);
	INJECT(0x00461D90, handle_object_changeover, replace);
	INJECT(0x00461DC0, handle_inventry_menu, replace);
	INJECT(0x00462740, setup_ammo_selector, replace);
	INJECT(0x00462A00, fade_ammo_selector, replace);
	INJECT(0x00462AD0, draw_ammo_selector, replace);
	INJECT(0x00462DD0, spinback, replace);
	INJECT(0x00462E60, update_laras_weapons_status, replace);
	INJECT(0x00462EF0, is_item_currently_combinable, replace);
	INJECT(0x00462F60, have_i_got_item, replace);
	INJECT(0x00462FA0, do_these_objects_combine, replace);
	INJECT(0x00462FF0, combine_these_two_objects, replace);
	INJECT(0x00463080, seperate_object, replace);
	INJECT(0x004630F0, combine_HK_SILENCER, replace);
	INJECT(0x00463130, combine_revolver_lasersight, replace);
	INJECT(0x004631B0, combine_crossbow_lasersight, replace);
	INJECT(0x00463230, combine_PuzzleItem1, replace);
	INJECT(0x00463260, combine_PuzzleItem2, replace);
	INJECT(0x00463290, combine_PuzzleItem3, replace);
	INJECT(0x004632C0, combine_PuzzleItem4, replace);
	INJECT(0x004632F0, combine_PuzzleItem5, replace);
	INJECT(0x00463320, combine_PuzzleItem6, replace);
	INJECT(0x00463350, combine_PuzzleItem7, replace);
	INJECT(0x00463380, combine_PuzzleItem8, replace);
	INJECT(0x004633B0, combine_KeyItem1, replace);
	INJECT(0x004633E0, combine_KeyItem2, replace);
	INJECT(0x00463410, combine_KeyItem3, replace);
	INJECT(0x00463440, combine_KeyItem4, replace);
	INJECT(0x00463470, combine_KeyItem5, replace);
	INJECT(0x004634A0, combine_KeyItem6, replace);
	INJECT(0x004634D0, combine_KeyItem7, replace);
	INJECT(0x00463500, combine_KeyItem8, replace);
	INJECT(0x00463530, combine_PickupItem1, replace);
	INJECT(0x00463560, combine_PickupItem2, replace);
	INJECT(0x00463590, combine_PickupItem3, replace);
	INJECT(0x004635C0, combine_PickupItem4, replace);
	INJECT(0x004635F0, combine_clothbottle, replace);
	INJECT(0x00463620, setup_objectlist_startposition, replace);
	INJECT(0x00463660, setup_objectlist_startposition2, replace);
	INJECT(0x004636B0, use_current_item, replace);
	INJECT(0x00463B60, DEL_picked_up_object, replace);
	INJECT(0x004640B0, NailInvItem, replace);
	INJECT(0x00464360, have_i_got_object, replace);
	INJECT(0x00464490, remove_inventory_item, replace);
	INJECT(0x004645B0, convert_obj_to_invobj, replace);
	INJECT(0x004645F0, convert_invobj_to_obj, replace);
	INJECT(0x00464610, init_keypad_mode, replace);
	INJECT(0x00464650, do_keypad_mode, replace);
	INJECT(0x00464AB0, do_examine_mode, replace);
	INJECT(0x00464BF0, do_stats_mode, replace);
	INJECT(0x00464C60, dels_give_lara_items_cheat, replace);
	INJECT(0x00464C80, dels_give_lara_guns_cheat, replace);
	INJECT(0x00464EF0, LoadGame, replace);
	INJECT(0x00464F20, SaveGame, replace);
	INJECT(0x00464CA0, DelDrawSprite, replace);
}
