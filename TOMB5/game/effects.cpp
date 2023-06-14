#include "../tomb5/pch.h"
#include "effects.h"
#include "sound.h"
#include "hair.h"
#include "delstuff.h"
#include "control.h"
#include "tomb4fx.h"
#include "items.h"
#include "deltapak.h"
#include "spider.h"
#include "lot.h"
#include "effect2.h"
#include "objects.h"
#include "sphere.h"
#include "../specific/dxsound.h"
#include "footprnt.h"
#include "../specific/function_stubs.h"
#include "../specific/audio.h"
#include "../specific/function_table.h"
#include "../specific/polyinsert.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "lara_states.h"
#include "gameflow.h"
#include "camera.h"
#include "pickup.h"
#include "lara.h"
#include "savegame.h"

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
	void_effect,
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

FX_INFO* effects;
OBJECT_VECTOR* sound_effects;
long number_sound_effects;

void WaterFall(short item_number)
{
	ITEM_INFO* item;
	long x, z, ang;
	static long wf = 256;

	item = &items[item_number];
	ang = item->pos.y_rot + 0x8000;
	x = item->pos.x_pos - (512 * phd_sin(ang) >> W2V_SHIFT);
	z = item->pos.z_pos - (512 * phd_cos(ang) >> W2V_SHIFT);

	switch (ang)
	{
	case 0:
		x += (wf * phd_sin(ang + 0x4000) >> W2V_SHIFT);
		z += (wf * phd_cos(ang + 0x4000) >> W2V_SHIFT);
		break;

	default:
		x += (wf * phd_sin(ang - 0x4000) >> W2V_SHIFT);
		z += (wf * phd_cos(ang - 0x4000) >> W2V_SHIFT);
		break;
	}

	TriggerWaterfallMist(x, item->pos.y_pos, z, item->pos.y_rot + 0x8000);
	SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
}

void void_effect(ITEM_INFO* item)
{

}

void turn180_effect(ITEM_INFO* item)
{
	item->pos.x_rot = -item->pos.x_rot;
	item->pos.y_rot -= 0x8000;
}

void floor_shake_effect(ITEM_INFO* item)
{
	long dx, dy, dz, dist;

	dx = item->pos.x_pos - camera.pos.x;
	dy = item->pos.y_pos - camera.pos.y;
	dz = item->pos.z_pos - camera.pos.z;

	if (abs(dx) < 0x4000 && abs(dy) < 0x4000 && abs(dz) < 0x4000)
	{
		dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
		camera.bounce = -66 * (0x100000 - dist / 256) / 0x100000;
	}
}

void PoseidonSFX(ITEM_INFO* item)
{
	SoundEffect(SFX_J_GRAB_OPEN, 0, SFX_DEFAULT);
	flipeffect = -1;
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
		eq->flags |= IFL_CODEBITS;
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
	else
		lara.mesh_ptrs[LM_RHAND] = tmp;
}

void SoundFlipEffect(ITEM_INFO* item)
{
	SoundEffect(TriggerTimer, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void ExplosionFX(ITEM_INFO* item)
{
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
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

void ResetTest(ITEM_INFO* item)
{
	ITEM_INFO* target_item;
	short room_num, item_num;

	for (item_num = next_item_active; item_num != NO_ITEM; item_num = target_item->next_active)
	{
		target_item = &items[item_num];

		if (objects[target_item->object_number].intelligent)
		{
			target_item->status = ITEM_INVISIBLE;
			RemoveActiveItem(item_num);
			DisableBaddieAI(item_num);
			room_num = target_item->item_flags[2] & 0xFF;
			target_item->pos.y_pos = target_item->item_flags[2] + room[room_num].minfloor;
			target_item->pos.x_pos = ((target_item->item_flags[1] << 2) + room[room_num].x + 512);
			target_item->pos.z_pos = (((target_item->item_flags[1] & 0xFF) << 10) + room[room_num].z + 512);
			target_item->pos.y_rot = target_item->TOSSPAD;
			target_item->item_flags[3] = target_item->item_flags[0] & 0xFF;
			target_item->ai_bits = target_item->item_flags[0] >> 8;

			if (target_item->object_number == CHEF)
				target_item->anim_number = objects[CHEF].anim_index + 0;
			else if (target_item->object_number == TWOGUN)
				target_item->anim_number = objects[TWOGUN].anim_index + 6;
			else if (objects[SWAT].loaded)
				target_item->anim_number = objects[SWAT].anim_index + 0;
			else
				target_item->anim_number = objects[BLUE_GUARD].anim_index + 0;

			target_item->current_anim_state = 1;
			target_item->goal_anim_state = 1;
			target_item->frame_number = anims[target_item->anim_number].frame_base;

			if (target_item->room_number != room_num)
				ItemNewRoom(item_num, room_num);
		}
	}

	flipeffect = -1;
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

	for (item_num = next_item_active; item_num != NO_ITEM; item_num = target_item->next_active)
	{
		target_item = &items[item_num];

		if (objects[target_item->object_number].intelligent)
		{
			target_item->status = ITEM_INVISIBLE;

			if (item != ((void*)0xABCDEF))
			{
				RemoveActiveItem(item_num);
				DisableBaddieAI(item_num);
				target_item->flags |= IFL_INVISIBLE;
			}
		}
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
		S_CDPlay(9, 0);
		savegame.TLCount = 1;
	}
}

void TL_2(ITEM_INFO* item)
{
	if (savegame.TLCount < 2)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(7, 0);
		savegame.TLCount = 2;
	}
}

void TL_3(ITEM_INFO* item)
{
	if (savegame.TLCount < 3)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(23, 0);
		savegame.TLCount = 3;
	}
}

void TL_4(ITEM_INFO* item)
{
	if (savegame.TLCount < 4)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(39, 0);
		savegame.TLCount = 4;
	}
}

void TL_5(ITEM_INFO* item)
{
	if (savegame.TLCount < 5)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(2, 0);
		savegame.TLCount = 5;
	}
}

void TL_6(ITEM_INFO* item)
{
	if (savegame.TLCount < 6)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(22, 0);
		savegame.TLCount = 6;
	}
}

void TL_7(ITEM_INFO* item)
{
	if (savegame.TLCount < 7)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(51, 0);
		savegame.TLCount = 7;
	}
}


void TL_8(ITEM_INFO* item)
{
	if (savegame.TLCount < 8)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(3, 0);
		savegame.TLCount = 8;
	}
}

void TL_9(ITEM_INFO* item)
{
	if (savegame.TLCount < 9)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(4, 0);
		savegame.TLCount = 9;
	}
}

void TL_10(ITEM_INFO* item)
{
	if (savegame.TLCount == 9)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(13, 0);
		savegame.TLCount = 10;
	}
}

void TL_11(ITEM_INFO* item)
{
	if (savegame.TLCount == 10)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(0, 0);
		savegame.TLCount = 11;
	}
}

void TL_12(ITEM_INFO* item)
{
	if (savegame.TLCount == 11)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(35, 0);
		savegame.TLCount = 12;
	}
}

void SoundEffects()
{
	OBJECT_VECTOR* sound;
	SoundSlot* slot;
	
	for (int i = 0; i < number_sound_effects; i++)
	{
		sound = &sound_effects[i];

		if (flip_stats[((sound->flags & 1) + (sound->flags & 2)
			+ 3 * (((sound->flags & 0x1F) >> 2) & 1)
			+ 5 * (((sound->flags & 0x1F) >> 4) & 1)
			+ 4 * (((sound->flags & 0x1F) >> 3) & 1))])
		{
			if (sound->flags & 0x40)
			{
				SoundEffect(sound->data, (PHD_3DPOS*)sound, 0);
				continue;
			}
		}
		else if (sound->flags & 0x80)
		{
			SoundEffect(sound->data, (PHD_3DPOS*)sound, 0);
			continue;
		}
	}

	if (flipeffect != -1)
		effect_routines[flipeffect](0);

	if (!sound_active)
		return;

	for (int i = 0; i < 32; i++)
	{
		slot = &LaSlot[i];

		if (slot->nSampleInfo >= 0)
		{
			if ((sample_infos[slot->nSampleInfo].flags & 3) != 3)
			{
				if (!S_SoundSampleIsPlaying(i))
					slot->nSampleInfo = -1;
				else
				{
					GetPanVolume(slot);
					S_SoundSetPanAndVolume(i, (short)slot->nPan, (ushort)slot->nVolume);
				}
			}
			else
			{
				if (!slot->nVolume)
				{
					S_SoundStopSample(i);
					slot->nSampleInfo = -1;
				}
				else
				{
					S_SoundSetPanAndVolume(i, (short)slot->nPan, (ushort)slot->nVolume);
					S_SoundSetPitch(i, slot->nPitch);
					slot->nVolume = 0;
				}
			}
		}
	}
}

short DoBloodSplat(long x, long y, long z, short random, short y_rot, short room_number)
{
	GetFloor(x, y, z, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterBlood(x, y, z, random);
	else
		TriggerBlood(x, y, z, y_rot >> 4, random);

	return -1;
}

void DoLotsOfBlood(long x, long y, long z, short speed, short ang, short room_number, long num)
{
	long bx, by, bz;

	for (; num > 0; num--)
	{
		bx = x - (GetRandomControl() << 9) / 0x8000 + 256;
		by = y - (GetRandomControl() << 9) / 0x8000 + 256;
		bz = z - (GetRandomControl() << 9) / 0x8000 + 256;
		DoBloodSplat(bx, by, bz, speed, ang, room_number);
	}
}

long ItemNearLara(PHD_3DPOS* pos, long rad)
{
	short* bounds;
	long dx, dy, dz;

	dx = pos->x_pos - lara_item->pos.x_pos;
	dy = pos->y_pos - lara_item->pos.y_pos;
	dz = pos->z_pos - lara_item->pos.z_pos;

	if (dx >= -rad && dx <= rad && dz >= -rad && dz <= rad && dy >= -3072 && dy <= 3072 && SQUARE(dx) + SQUARE(dz) <= SQUARE(rad))
	{
		bounds = GetBoundsAccurate(lara_item);

		if (dy >= bounds[2] && dy <= bounds[3] + 100)
			return 1;
	}

	return 0;
}

void Richochet(GAME_VECTOR* pos)
{
	TriggerRicochetSpark(pos, mGetAngle(pos->z, pos->x, lara_item->pos.z_pos, lara_item->pos.x_pos) >> 4, 3, 0);
	SoundEffect(SFX_LARA_RICOCHET, (PHD_3DPOS*)pos, SFX_DEFAULT);
}

void WadeSplash(ITEM_INFO* item, long water, long depth)
{
	short* bounds;
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (!(room[room_number].flags & ROOM_UNDERWATER))
		return;

	bounds = GetBestFrame(item);

	if (item->pos.y_pos + bounds[2] > water || item->pos.y_pos + bounds[3] < water)
		return;

	if (item->fallspeed > 0 && depth < 474 && !SplashCount)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = water;
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerRad = 16;
		splash_setup.InnerSize = 12;
		splash_setup.InnerRadVel = 160;
		splash_setup.InnerYVel = -72 * item->fallspeed;
		splash_setup.MiddleRad = 24;
		splash_setup.MiddleSize = 24;
		splash_setup.MiddleRadVel = 224;
		splash_setup.MiddleYVel = -36 * item->fallspeed;
		splash_setup.OuterRad = 32;
		splash_setup.OuterSize = 32;
		splash_setup.OuterRadVel = 272;
		SetupSplash(&splash_setup);
		SplashCount = 16;
	}
	else if (!(wibble & 0xF) && (!(GetRandomControl() & 0xF) || item->current_anim_state != AS_STOP))
	{
		if (item->current_anim_state == AS_STOP)
			SetupRipple(item->pos.x_pos, water, item->pos.z_pos, (GetRandomControl() & 0xF) + 112, 16);
		else
			SetupRipple(item->pos.x_pos, water, item->pos.z_pos, (GetRandomControl() & 0xF) + 112, 18);
	}
}

void Splash(ITEM_INFO* item)
{
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerRad = 32;
		splash_setup.InnerSize = 8;
		splash_setup.InnerRadVel = 320;
		splash_setup.InnerYVel = -40 * item->fallspeed;
		splash_setup.MiddleRad = 48;
		splash_setup.MiddleSize = 32;
		splash_setup.MiddleRadVel = 480;
		splash_setup.MiddleYVel = -20 * item->fallspeed;
		splash_setup.OuterRad = 32;
		splash_setup.OuterSize = 128;
		splash_setup.OuterRadVel = 544;
		SetupSplash(&splash_setup);
	}
}
