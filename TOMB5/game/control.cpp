#pragma once
#include "../tomb5/pch.h"
#include "lara.h"
#include "control.h"
#include "pickup.h"
#include "../specific/specific.h"
#include "../specific/input.h"
#include "newinv2.h"
#include "lara_states.h"
#include "camera.h"
#include "gameflow.h"
#include "items.h"
#include "effect2.h"
#include "box.h"
#include "laramisc.h"
#include "spotcam.h"
#include "tomb4fx.h"
#include "../specific/global.h"
#include "sound.h"
#include "hair.h"
#include "bubbles.h"
#include "bat.h"
#include "effects.h"
#include "objects.h"

long ControlPhase(long nframes, int demo_mode)
{
	RegeneratePickups();

	if (10 < nframes)
		nframes = 10;

	if (bTrackCamInit != 0)
		bUseSpotCam = 0;

	SetDebounce = 1;

	for (framecount += nframes; framecount > 0; framecount -= 2)
	{
		GlobalCounter++;
		UpdateSky();

		if (0 < cdtrack)
			S_CDLoop();

		if (S_UpdateInput() == -1)
			return 0;

		if (bDisableLaraControl)
		{
			if (gfCurrentLevel != 0)
				dbinput = 0;

			input &= IN_LOOK;
		}

		if (cutseq_trig != 0)
			input = 0;

		SetDebounce = 0;

		if (gfCurrentLevel != LVL5_TITLE)
		{
			if ((dbinput & IN_OPTION || GLOBAL_enterinventory != -1) && !cutseq_trig && lara_item->hit_points > 0)
			{
				S_SoundStopAllSamples();

				if (S_CallInventory2())
					return 2;
			}
		}

		if (keymap[23] != 0)//the I button
			dels_give_lara_items_cheat();

		if (gfLevelComplete)
			return 3;

		if (reset_flag || lara.death_count > 300 || (lara.death_count > 60 && input != IN_NONE))
		{
			reset_flag = 0;

			if (Gameflow->DemoDisc && reset_flag)
				return 4;
			else
				return 1;
		}

		if (demo_mode && input == IN_ALL)
			input = IN_NONE;

		if (!lara.death_count && !FadeScreenHeight)
		{
			if (input & IN_SAVE)
				S_LoadSave(IN_SAVE, 0);
			else if (input & IN_LOAD)
			{
				if (S_LoadSave(IN_LOAD, 0) >= 0)
					return 2;
			}

			if (input & IN_PAUSE && gfGameMode == 0)
			{
				if (S_PauseMenu() == 8)
					return 1;
			}
		}

		if (thread_started)
			return 4;

		if (!(input & IN_LOOK)
			|| SniperCamActive
			|| bUseSpotCam
			|| bTrackCamInit
			||
			((lara_item->current_anim_state != STATE_LARA_STOP || lara_item->anim_number != ANIMATION_LARA_STAY_IDLE)
				&& (!lara.IsDucked
					|| input & IN_DUCK
					|| lara_item->anim_number != ANIMATION_LARA_CROUCH_IDLE
					|| lara_item->goal_anim_state != STATE_LARA_CROUCH_IDLE)))
		{
			if (!BinocularRange)
			{
				if (SniperCamActive
					|| bUseSpotCam
					|| bTrackCamInit)
					input &= ~IN_LOOK;
			}
			else
			{
				if (!LaserSight)
					input |= IN_LOOK;
				else
				{
					BinocularRange = 0;
					LaserSight = 0;
					AlterFOV(0x38E0);
					lara_item->mesh_bits = -1;
					lara.Busy = 0;
					camera.type = BinocularOldCamera;
					lara.head_y_rot = 0;
					lara.head_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.torso_x_rot = 0;
					camera.bounce = 0;
					BinocularOn = -8;
					input &= ~IN_LOOK;
				}

				InfraRed = 0;
			}
		}
		else if (!BinocularRange)
		{
			if (lara.gun_status == LG_READY
				&& ((lara.gun_type == WEAPON_REVOLVER && lara.sixshooter_type_carried & WTYPE_LASERSIGHT)
					|| (lara.gun_type == WEAPON_HK)
					|| (lara.gun_type == WEAPON_CROSSBOW && lara.crossbow_type_carried & WTYPE_LASERSIGHT)))
			{
				BinocularRange = 128;
				BinocularOldCamera = camera.old_type;
				lara.Busy = 1;
				LaserSight = 1;

				if (!(gfLevelFlags & GF_LVOP_TRAIN))
					InfraRed = 1;
				else
					InfraRed = 0;
			}
			else
				InfraRed = 0;
		}
		else
		{
			if (LaserSight)
			{
				if (!(gfLevelFlags & GF_LVOP_TRAIN))
					InfraRed = 1;
				else
					InfraRed = 0;
			}
			else
			{
				if ((gfLevelFlags & GF_LVOP_TRAIN) && (inputBusy & IN_ACTION))
					InfraRed = 1;
				else
					InfraRed = 0;
			}
		}

		ClearDynamics();
		ClearFires();
		GotLaraSpheres = 0;
		InItemControlLoop = 1;

		short item_num = next_item_active;

		while (item_num != NO_ITEM)
		{
			ITEM_INFO* item = &items[item_num];
			short nex = item->next_active;

			if (item->after_death < 128)
			{
				if (objects[item->object_number].control)
					objects[item->object_number].control(item_num);
			}
			else
				KillItem(item_num);

			item_num = nex;
		}

		InItemControlLoop = 0;
		KillMoveItems();
		InItemControlLoop = 1;

		short fx_num = next_fx_active;

		while (fx_num != NO_ITEM)
		{
			FX_INFO* fx = &effects[fx_num];
			short nex = fx->next_active;

			if (objects[fx->object_number].control)
				objects[fx->object_number].control(fx_num);

			fx_num = nex;
		}

		InItemControlLoop = 0;
		KillMoveEffects();

		if (KillEverythingFlag)
			KillEverything();

		if (SmokeCountL != 0)
			SmokeCountL--;

		if (SmokeCountR != 0)
			SmokeCountR--;

		if (SplashCount != 0)
			SplashCount--;

		if (WeaponDelay != 0)
			WeaponDelay--;

		if (lara.has_fired && !(wibble & 0x7F))
		{
			AlertNearbyGuards(lara_item);
			lara.has_fired = 0;
		}

		XSoff1 += 150;
		YSoff1 += 230;
		ZSoff1 += 660;
		XSoff2 += 270;
		YSoff2 += 440;
		ZSoff2 += 160;

		if (lara.poisoned != 0 && !GLOBAL_playing_cutseq)
		{
			if (lara.poisoned <= 4096)
			{
				if (lara.dpoisoned)
					lara.dpoisoned++;
				else
					lara.poisoned = 4096;
			}

			if (gfLevelFlags & GF_LVOP_TRAIN && !lara.Gassed)
			{
				if (lara.dpoisoned != 0)
				{
					lara.dpoisoned -= 8;

					if (lara.dpoisoned < 0)
						lara.dpoisoned = 0;
				}
			}

			if (lara.poisoned >= 256 && !(wibble & 0xFF))
			{
				lara_item->hit_points -= lara.poisoned >> (8 - lara.Gassed);
				PoisonFlag = 16;
			}
		}

		lara.skelebob = 0;
		InItemControlLoop = 1;

		if (!GLOBAL_playing_cutseq && !gfGameMode)
		{
			lara.Fired = 0;
			LaraControl(0);

			if (LaraDrawType == LARA_DIVESUIT)
				DoSubsuitStuff();
		}

		InItemControlLoop = 0;
		KillMoveItems();

		if (gfLevelFlags & GF_LVOP_TRAIN && !bUseSpotCam)
		{
			if (room[lara_item->room_number].FlipNumber > 10)
			{
				InitialiseSpotCam(room[lara_item->room_number].FlipNumber);
				bUseSpotCam = 1;
			}
		}

		if (GLOBAL_inventoryitemchosen != -1)
		{
			SayNo();
			GLOBAL_inventoryitemchosen = -1;
		}

		if (GLOBAL_playing_cutseq)
		{
			camera.type = CINEMATIC_CAMERA;
			CalculateCamera();
		}
		else
		{
			if (LaraDrawType != LARA_DIVESUIT)
			{
				HairControl(0, 0, 0);

				if (gfLevelFlags & GF_LVOP_YOUNG_LARA)
					HairControl(0, 1, 0);
			}

			if (!bUseSpotCam)
			{
				bTrackCamInit = 0;
				CalculateCamera();
			}
			else
				CalculateSpotCams();
		}

		CamRot.vy = (mGetAngle(camera.pos.z, camera.pos.x, camera.target.z, camera.target.x) >> 4) & 0xFFF;
		wibble = (wibble + 4) & 0xFC;
		TriggerLaraDrips();

		while (SmashedMeshCount != 0)
		{
			SmashedMeshCount--;
			MESH_INFO* mesh = SmashedMesh[SmashedMeshCount];
			FLOOR_INFO* floor = GetFloor(mesh->x, mesh->y, mesh->z, &SmashedMeshRoom[SmashedMeshCount]);
			GetHeight(floor, mesh->x, mesh->y, mesh->z);
			TestTriggers(trigger_index, 1, 0);
			floor->stopper = 0;
			SmashedMesh[SmashedMeshCount] = NULL;
		}

		UpdateSparks();
		UpdateFireSparks();
		UpdateSmokeSparks();
		UpdateBubbles();
		UpdateSplashes();
		UpdateDebris();
		UpdateBlood();
		UpdateDrips();
		UpdateGunShells();
		UpdateRats();
		UpdateBats();
		UpdateSpiders();
		UpdateShockwaves();
		UpdateLightning();
		UpdateTwogunLasers();
		AnimateWaterfalls();
		UpdatePulseColour();

		if (gfCurrentLevel == LVL5_SINKING_SUBMARINE)
			KlaxonTremor();

		SoundEffects();
		health_bar_timer--;

		if (gfGameMode == 0)
		{
			GameTimer++;

			if (savegame.Level.Timer != 0)
			{
				if (!GLOBAL_playing_cutseq)
					savegame.Level.Timer++;
			}
		}

		UpdateFadeClip();
	}

	return 0;
}

void KlaxonTremor()
{
	static short timer;

	if (!(GlobalCounter & 0x1FF))
		SoundEffect(SFX_KLAXON, 0, 0x1000 | SFX_SETVOL);

	if (timer >= 0)
		timer++;

	if (timer > 450)
	{
		if (!(GetRandomControl() & 0x1FF))
		{
			InGameCnt = 0;
			timer = -32 - (GetRandomControl() & 0x1F);
			return;
		}
	}

	if (timer < 0)
	{
		if ((signed int)InGameCnt >= ABS(timer))
		{
			camera.bounce = -(GetRandomControl() % ABS(timer));
			++timer;
		}
		else
			camera.bounce = -(GetRandomControl() % ++InGameCnt);
	}
}

int GetRandomControl()
{
	rand_1 = 1103515245 * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

int GetRandomDraw()
{
	rand_2 = 1103515245 * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

int GetChange(ITEM_INFO* item, ANIM_STRUCT* anim)
{
	CHANGE_STRUCT* change;
	RANGE_STRUCT* range;

	if (item->current_anim_state == item->goal_anim_state)
		return 0;

	if (anim->number_changes <= 0)
		return 0;


	change = &changes[anim->change_index];

	for (int i = 0; i < anim->number_changes; i++, change++)
	{
		if (change->goal_anim_state == item->goal_anim_state)
		{
			if (change->number_ranges > 0)
			{
				range = &ranges[change->range_index];

				for (int j = 0; j < change->number_ranges; j++, range++)
				{
					if (item->frame_number >= range->start_frame && item->frame_number <= range->end_frame)
					{
						item->anim_number = range->link_anim_num;
						item->frame_number = range->link_frame_num;
						return (1);
					}
				}
			}
		}
	}

	return 0;
}

int CheckGuardOnTrigger()
{
	int slot;
	short room_number;
	creature_info* cinfo;
	ITEM_INFO* item;

	room_number = lara_item->room_number;
	cinfo = &baddie_slots[0];
	GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);

	for (slot = 0; slot < 5; slot++, cinfo++)
	{
		if (cinfo->item_num != -1 && cinfo->alerted)
		{
			item = &items[cinfo->item_num];

			if (room_number == item->room_number && item->current_anim_state == 1)
			{
				if (ABS(item->pos.x_pos - lara_item->pos.x_pos) < 1024 &&
					ABS(item->pos.z_pos - lara_item->pos.z_pos) < 1024 &&
					ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256)
					return 1;
			}
		}
	}

	return 0;
}

void InterpolateAngle(short dest, short* src, short* diff, short speed)
{
	long adiff;

	adiff = (dest & 0xFFFF) - *src;

	if (adiff > 32768)
		adiff -= 65536;
	else if (adiff < -32768)
		adiff += 65536;

	if (diff)
		diff[0] = adiff;

	*src += adiff >> speed;
}

void TranslateItem(ITEM_INFO* item, short x, short y, short z)
{
	short sin;
	short cos;

	cos = COS(item->pos.y_rot);
	sin = SIN(item->pos.y_rot);

	item->pos.x_pos += ((cos * x) + (sin * z)) >> 14;
	item->pos.y_pos += y;
	item->pos.z_pos += ((-sin * x) + (cos * z)) >> 14;
}

void InitCutPlayed()
{
	_CutSceneTriggered1 = 0;
	_CutSceneTriggered2 = 0;
}

void SetCutPlayed(int num)
{
	if (num < 1 || num > 4)
	{
		if (num < 32)
			_CutSceneTriggered1 |= 1 << num;
		else
			_CutSceneTriggered2 |= 1 << (num - 32);
	}
}

void SetCutNotPlayed(int num)
{
	if (num < 32)
		_CutSceneTriggered1 &= ~(1 << num);
	else
		_CutSceneTriggered2 &= ~(1 << (num - 32));
}

int CheckCutPlayed(int num)
{
	int ret;

	if (num < 32)
		ret = _CutSceneTriggered1 & (1 << num);
	else
		ret = _CutSceneTriggered2 & (1 << (num - 32));

	return ret;
}

int is_object_in_room(int roomnumber, int objnumber)
{
	short item_num, nex;
	ITEM_INFO* item;

	item_num = room[roomnumber].item_number;

	for (nex = item_num; nex != -1; nex = item->next_item)
	{
		item = &items[nex];

		if (item->object_number == objnumber)
			return 1;
	}

	return 0;
}

int check_xray_machine_trigger()
{
	for (int i = 0; i < level_items; i++)
		if (items[i].object_number == XRAY_CONTROLLER &&
			items[i].trigger_flags == 0 &&
			items[i].item_flags[0] == 666)
			return 1;

	return 0;
}

void inject_control()
{
	INJECT(0x004147C0, ControlPhase);
	INJECT(0x00442C90, KlaxonTremor);
	INJECT(0x004A7C10, GetRandomControl);
	INJECT(0x004A7C70, SeedRandomControl);
	INJECT(0x004A7C40, GetRandomDraw);
	INJECT(0x004A7C90, SeedRandomDraw);
	INJECT(0x00415890, GetChange);
	INJECT(0x0041AD60, CheckGuardOnTrigger);
	INJECT(0x0041AEA0, InterpolateAngle);
	INJECT(0x00415960, TranslateItem);
	INJECT(0x0041B180, InitCutPlayed);
	INJECT(0x0041B1A0, SetCutPlayed);
	INJECT(0x0041B1F0, SetCutNotPlayed);
	INJECT(0x0041B240, CheckCutPlayed);
	INJECT(0x0041B8B0, is_object_in_room);
	INJECT(0x0041B930, check_xray_machine_trigger);
}
