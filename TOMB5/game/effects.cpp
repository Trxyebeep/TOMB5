#include "pch.h"
#include "effects.h"
#include "sound.h"
#include "hair.h"
#include "delstuff.h"
#include "control.h"
#include "tomb4fx.h"
#include "items.h"
#include "deltapak.h"
#include "bat.h"//for spiders, fix
#include "lot.h"
#include "../specific/specific.h"
#include "xatracks.h"
#include "effect2.h"
#include "objects.h"
#include "sphere.h"

int flare_table[56] =
{
	0x0, 0x0, 0xC83C, 0x00F5, 0xC470, 0x0078, 0xCCE6, 0x00CA, 0x4000, 0x0080, 0x4040, 0x0040, 0xE8EC, 0x00F3, 0x40C0,
	0x0, 0x8000,
	0x0, 0xAC9D, 0x0096, 0x8080, 0x0080, 0xA37B, 0x00CC, 0xA28C, 0x00B1, 0xDFBF,
	0x0, 0xFFDF, 0x006F, 0xD898, 0x00F4, 0xC03C, 0x00F8, 
	0x0, 0x00FC, 0x5F57, 0x00C6, 0x9776, 0x00E2, 0xEBCE, 0x00F8, 0x1E10, 
	0x0, 0xDEA7, 0x00FA, 0xAF75, 0x00DA, 0xBF4A, 0x00E1, 0x8C8D, 0x004D, 0xB59A, 0x0004, 0xAE00, 0x00FF
};

static char footsounds[14] = 
{
	0, 5, 3, 2, 1, 9, 9, 4, 6, 5, 3, 9, 4, 6
};

void(*effect_routines[59])(ITEM_INFO* item) =
{
	turn180_effect,
	floor_shake_effect,
	PoseidonSFX,
	LaraBubbles,
	finish_level_effect,
	ActivateCamera,
	ActivateKey,
	RubbleFX,
	SwapCrowbar,
	void_effect,
	SoundFlipEffect,
	ExplosionFX,
	lara_hands_free,
	void_effect,
	void_effect,
	void_effect,
	shoot_right_gun,
	shoot_left_gun,
	void_effect,
	void_effect,
	void_effect,
	invisibility_on,
	invisibility_off,
	void_effect,
	void_effect,
	void_effect,
	reset_hair,
	void_effect,
	SetFog,
	void_effect,
	LaraLocation,
	ClearSpidersPatch,
	AddFootprint,
	ResetTest,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	LaraLocationPad,
	KillActiveBaddies,
	TL_1,
	TL_2,
	TL_3,
	TL_4,
	TL_5,
	TL_6,
	TL_7,
	TL_8,
	TL_9,
	TL_10,
	TL_11,
	TL_12,
};

void WaterFall(short item_number)//FIXME idek man
{
	ITEM_INFO* item;
	int x;
	int z;

	int a, b, c;

	item = &items[item_number];
	c = ((item->pos.y_rot + 0x8000) >> 3) & 0x1FFE;
	a = item->pos.x_pos - (rcossin_tbl[c] << 11 >> 14);
	b = item->pos.z_pos - (rcossin_tbl[c + 1] << 11 >> 14);
	if (item->pos.y_rot == -32768)
	{
		x = a + ((wf * 4 * rcossin_tbl[2048]) >> 14);
		z = b + ((wf * 4 * rcossin_tbl[2049]) >> 14);
	}
	else
	{
		x = a + (wf * (SIN(item->pos.y_rot + 0x4000) >> 14));
		z = b + (wf * (COS(item->pos.y_rot + 0x4000) >> 14));
	}

	TriggerWaterfallMist(x, item->pos.y_pos, z, item->pos.y_rot + 0x8000);
	SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
}


void void_effect(ITEM_INFO* item)
{
	return;
}

void turn180_effect(ITEM_INFO* item)
{
	item->pos.y_rot -= 32768;
	item->pos.x_rot = -item->pos.x_rot;
}

void floor_shake_effect(ITEM_INFO* item)
{
	int x, y, z, dist;

	x = item->pos.x_pos - camera.pos.x;
	y = item->pos.y_pos - camera.pos.y;
	z = item->pos.z_pos - camera.pos.z;

	if ((ABS(x) < 16384) && (ABS(y) < 16384) && (ABS(z) < 16384))
	{
		dist = (x * x + y * y + z * z) / 256;
		camera.bounce = ((1048576 - dist) * 100) / (1048576);
	}
}

void PoseidonSFX(ITEM_INFO* item)
{
	SoundEffect(SFX_J_GRAB_OPEN, NULL, SFX_DEFAULT);
	flipeffect = -1;
}

void LaraBubbles(ITEM_INFO* item)
{
	PHD_VECTOR pos;
	int num;

	SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
	pos.x = 0;

	if (LaraDrawType == LARA_DIVESUIT)
	{
		pos.y = -192;
		pos.z = -160;
		GetLaraJointPos(&pos, 7);
	}
	else
	{
		pos.y = -4;
		pos.z = 64;
		GetLaraJointPos(&pos, 8);
	}

	num = (GetRandomControl() & 1) + 2;

	for (int i = 0; i < num; i++)
		CreateBubble(&pos, item->room_number, 8, 7, 0, 0, 0, 0);
}

void finish_level_effect(ITEM_INFO* item)
{
	gfLevelComplete = gfCurrentLevel + 1;
}

void ActivateCamera(ITEM_INFO* item)
{
	KeyTriggerActive = 2;
}

void ActivateKey(ITEM_INFO* item)
{
	KeyTriggerActive = 1;
}

void RubbleFX(ITEM_INFO* item)
{
	ITEM_INFO* eq;
	
	eq = find_a_fucking_item(EARTHQUAKE);

	if (eq)
	{
		AddActiveItem(eq - items);
		eq->status = ITEM_ACTIVE;
		eq->flags |= IFLAG_ACTIVATION_MASK;
	}
	else
		camera.bounce = -150;

	flipeffect = -1;
}

void SwapCrowbar(ITEM_INFO* item)
{
	short* tmp;
	
	tmp = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];

	if (lara.mesh_ptrs[LM_RHAND] == tmp)
		lara.mesh_ptrs[LM_RHAND] = meshes[objects[CROWBAR_ANIM].mesh_index + (2 * LM_RHAND)];
	else lara.mesh_ptrs[LM_RHAND] = tmp;
}

void SoundFlipEffect(ITEM_INFO* item)
{
	SoundEffect(TriggerTimer, NULL, SFX_DEFAULT);
	flipeffect = -1;
}

void ExplosionFX(ITEM_INFO* item)
{
	SoundEffect(SFX_EXPLOSION1, NULL, SFX_DEFAULT);
	camera.bounce = -75;
	flipeffect = -1;
}

void lara_hands_free(ITEM_INFO* item)
{
	lara.gun_status = LG_NO_ARMS;
}


void shoot_right_gun(ITEM_INFO* item)
{
	lara.right_arm.flash_gun = 3;
}

void shoot_left_gun(ITEM_INFO* item)
{
	lara.left_arm.flash_gun = 3;
}

void invisibility_on(ITEM_INFO* item)
{
	item->status = ITEM_INVISIBLE;
}

void invisibility_off(ITEM_INFO* item)
{
	item->status = ITEM_ACTIVE;
}

void reset_hair(ITEM_INFO* item)
{
	InitialiseHair();
}

void SetFog(ITEM_INFO* item)
{
	unsigned short color_index;
	int color;
	unsigned char r, g, b;

	dword_51CE04 = 0;
	color_index = TriggerTimer;

	if (!IsVolumetric())
	{
		flipeffect = -1;
		return;
	}

	if (TriggerTimer == 100)
	{
		dword_51CE04 = 1;
		flipeffect = -1;
		return;
	}

	color = flare_table[color_index];
	r = (char)color >> 16;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
	SetFogColor(r, g, b);
	g = color;
	b = color & 0xFF;
	gfFogColour.r = r;
	gfFogColour.g = g;
	gfFogColour.b = b;
	flipeffect = -1;
	return;
}

void LaraLocation(ITEM_INFO* item)
{
	lara.location = TriggerTimer;

	if (lara.highest_location < TriggerTimer)
		lara.highest_location = TriggerTimer;

	flipeffect = -1;
}

void ClearSpidersPatch(ITEM_INFO* item)
{
	ClearSpiders();
}

void AddFootprint(ITEM_INFO* item)
{
	FOOTPRINT* print;
	PHD_VECTOR pos;
	short room_num;
	FLOOR_INFO* floor;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (FXType == SFX_LANDONLY)
		GetLaraJointPos(&pos, LM_LFOOT);
	else
		GetLaraJointPos(&pos, LM_RFOOT);

	room_num = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_num);

	if (floor->fx != 6 && floor->fx != 5 && floor->fx != 11)
		SoundEffect(footsounds[floor->fx] + 0x120, &lara_item->pos, 0);

	if (floor->fx < 3 && !OnObject)
	{
		print = &FootPrint[FootPrintNum];
		print->x = pos.x;
		print->y = GetHeight(floor, pos.x, pos.y, pos.z);
		print->z = pos.z;
		print->YRot = item->pos.y_rot;
		print->Active = 512;
		FootPrintNum = FootPrintNum + 1 & 0x1F;
	}
}

void ResetTest(ITEM_INFO* item)
{
	short room_num, item_num;
	ITEM_INFO* target_item;

	item_num = next_item_active;

	if (item_num == NO_ITEM)
	{
		flipeffect = -1;
		return;
	}

	do
	{
		target_item = &items[item_num];

		if (objects[target_item->object_number].intelligent)
		{
			target_item->status |= ITEM_INVISIBLE;
			RemoveActiveItem(item_num);
			DisableBaddieAI(item_num);
			room_num = (target_item->item_flags[2] & 0xFF) * 208;
			target_item->pos.y_pos = room[room_num].minfloor + target_item->item_flags[2];
			target_item->pos.x_pos = room[room_num].x + 512 + target_item->item_flags[1] * 4;
			target_item->pos.z_pos = room[room_num].z + 512 + (target_item->item_flags[1] << 10);
			target_item->pos.y_rot = target_item->TOSSPAD;
			target_item->item_flags[3] = target_item->item_flags[0] & 0xFF;
			target_item->active = 0;
			target_item->collidable = 0;
			target_item->looked_at = 0;
			//v4->_bf15ea |= (((target_item->item_flags[0] >> 8) & 0x1F) << 9);

			if (target_item->object_number == CHEF)
				target_item->anim_number = objects[CHEF].anim_index;
			else
			{
				if (target_item->object_number == TWOGUN)
					target_item->anim_number = objects[TWOGUN].anim_index + 6;
				else
				{
					if (objects[SWAT].loaded)
						target_item->anim_number = objects[SWAT].anim_index;

					target_item->current_anim_state = 1;
					target_item->goal_anim_state = 1;
					target_item->frame_number = anims[target_item->anim_number].frame_base;

					if (target_item->room_number != room_num)
						ItemNewRoom(item_num, room_num);

					item_num = target_item->next_active;

					if (item_num == NO_ITEM)
					{
						flipeffect = -1;
						return;
					}

				}

				target_item->anim_number = objects[BLUE_GUARD].anim_index;
			}

			target_item->current_anim_state = 1;
			target_item->goal_anim_state = 1;
			target_item->frame_number = anims[target_item->anim_number].frame_base;

			if (target_item->room_number != room_num)
				ItemNewRoom(item_num, room_num);
		}

		item_num = target_item->next_active;

		if (item_num == NO_ITEM)
		{
			flipeffect = -1;
			return;
		}

	} while (true);
}

void LaraLocationPad(ITEM_INFO* item)
{
	flipeffect = -1;
	lara.location = TriggerTimer;
	lara.locationPad = TriggerTimer;
}

void KillActiveBaddies(ITEM_INFO* item)
{
	ITEM_INFO* target_item;
	short item_num;

	if (next_item_active != NO_ITEM)
	{
		item_num = next_item_active;

		do
		{
			target_item = &items[item_num];

			if (objects[target_item->object_number].intelligent)
			{
				target_item->status = ITEM_INVISIBLE;

				if (*(int*)&item != 0xABCDEF)
				{
					RemoveActiveItem(item_num);
					DisableBaddieAI(item_num);
					target_item->flags |= IFLAG_INVISIBLE;
				}
			}

			item_num = target_item->next_active;

		} while (item_num != -1);
	}

	flipeffect = -1;
}

void BaddieBiteEffect(ITEM_INFO* item, BITE_INFO* bite)
{
	PHD_VECTOR pos;

	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->mesh_num);
	DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 4, item->pos.y_rot, item->room_number);
}

void TL_1(ITEM_INFO* item)
{
	if (savegame.TLCount < 1)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA2_TL_01, 0);
		savegame.TLCount = 1;
	}
}

void TL_2(ITEM_INFO* item)
{
	if (savegame.TLCount < 2)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_TL_02, 0);
		savegame.TLCount = 2;
	}
}

void TL_3(ITEM_INFO* item)
{
	if (savegame.TLCount < 3)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA3_TL_03, 0);
		savegame.TLCount = 3;
	}
}

void TL_4(ITEM_INFO* item)
{
	if (savegame.TLCount < 4)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA5_TL_04, 0);
		savegame.TLCount = 4;
	}
}

void TL_5(ITEM_INFO* item)
{
	if (savegame.TLCount < 5)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_TL_05, 0);
		savegame.TLCount = 5;
	}
}

void TL_6(ITEM_INFO* item)
{
	if (savegame.TLCount < 6)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA3_TL_06, 0);
		savegame.TLCount = 6;
	}
}

void TL_7(ITEM_INFO* item)
{
	if (savegame.TLCount < 7)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA7_TL_07, 0);
		savegame.TLCount = 7;
	}
}


void TL_8(ITEM_INFO* item)
{
	if (savegame.TLCount < 8)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_TL_08, 0);
		savegame.TLCount = 8;
	}
}

void TL_9(ITEM_INFO* item)
{
	if (savegame.TLCount < 9)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_TL_11, 0);
		savegame.TLCount = 9;
	}
}

void TL_10(ITEM_INFO* item)
{
	if (savegame.TLCount == 9)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA2_TL_10A, 0);
		savegame.TLCount = 10;
	}
}

void TL_11(ITEM_INFO* item)
{
	if (savegame.TLCount == 10)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA1_TL_10B, 0);
		savegame.TLCount = 11;
	}
}

void TL_12(ITEM_INFO* item)
{
	if (savegame.TLCount == 11)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA5_TL_12, 0);
		savegame.TLCount = 12;
	}
}

void inject_effects()
{
	INJECT(0x00432CA0, WaterFall);
	INJECT(0x00432DD0, void_effect);
	INJECT(0x00432E10, turn180_effect);
	INJECT(0x00432E40, floor_shake_effect);
	INJECT(0x00432FD0, PoseidonSFX);
	INJECT(0x00483470, LaraBubbles);
	INJECT(0x00432DF0, finish_level_effect);
	INJECT(0x00433000, ActivateCamera);
	INJECT(0x00433020, ActivateKey);
	INJECT(0x00432F40, RubbleFX);
	INJECT(0x00433040, SwapCrowbar);
	INJECT(0x00432F10, SoundFlipEffect);
	INJECT(0x00433080, ExplosionFX);
	INJECT(0x00433440, lara_hands_free);
	INJECT(0x00433460, shoot_right_gun);
	INJECT(0x00433480, shoot_left_gun);
	INJECT(0x00433570, invisibility_on);
	INJECT(0x00433590, invisibility_off);
	INJECT(0x004335C0, reset_hair);
	INJECT(0x004334A0, SetFog);
	INJECT(0x004330C0, LaraLocation);
	INJECT(0x00433670, ClearSpidersPatch);
	INJECT(0x004346A0, AddFootprint);
	INJECT(0x00433130, ResetTest);
	INJECT(0x00433100, LaraLocationPad);
	INJECT(0x00433360, KillActiveBaddies);
	INJECT(0x004335E0, BaddieBiteEffect);
	INJECT(0x00433690, TL_1);
	INJECT(0x004336D0, TL_2);
	INJECT(0x00433710, TL_3);
	INJECT(0x00433750, TL_4);
	INJECT(0x00433790, TL_5);
	INJECT(0x004337D0, TL_6);
	INJECT(0x00433810, TL_7);
	INJECT(0x00433850, TL_8);
	INJECT(0x00433890, TL_9);
	INJECT(0x004338D0, TL_10);
	INJECT(0x00433910, TL_11);
	INJECT(0x00433950, TL_12);
}
