#include "../tomb5/pch.h"
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
#include "../specific/3dmath.h"
#include "sound.h"
#include "hair.h"
#include "bat.h"
#include "spider.h"
#include "rat.h"
#include "objects.h"
#include "../specific/init.h"//for Log
#include "sphere.h"
#include "debris.h"
#include "larafire.h"
#include "switch.h"
#include "draw.h"
#include "joby.h"
#include "../specific/LoadSave.h"
#include "subsuit.h"


long ControlPhase(long _nframes, int demo_mode)
{
	RegeneratePickups();

	if (10 < _nframes)
		_nframes = 10;

	if (bTrackCamInit != 0)
		bUseSpotCam = 0;

	SetDebounce = 1;

	for (framecount += _nframes; framecount > 0; framecount -= 2)
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

#ifdef cutseq_skipper
		if (cutseq_trig != 0)
		{
			if (keymap[1] && !ScreenFading)//skip them with esc
				cutseq_trig = 3;


			input = 0;
		}
#else
		if (cutseq_trig != 0)
			input = 0;
#endif
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
			((lara_item->current_anim_state != AS_STOP || lara_item->anim_number != ANIMATION_LARA_STAY_IDLE)
				&& (!lara.IsDucked
					|| input & IN_DUCK
					|| lara_item->anim_number != ANIMATION_LARA_CROUCH_IDLE
					|| lara_item->goal_anim_state != AS_DUCK)))
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
	GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);

	for (slot = 0; slot < 5; slot++)
	{
		cinfo = &baddie_slots[slot];

		if (cinfo->item_num != -1 && !cinfo->alerted)
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
		diff[0] = (short)adiff;

	*src += (short)(adiff >> speed);
}

void TranslateItem(ITEM_INFO* item, short x, short y, short z)
{
	short sin;
	short cos;

	cos = phd_cos(item->pos.y_rot);
	sin = phd_sin(item->pos.y_rot);

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

void NeatAndTidyTriggerCutscene(int value, int timer)
{
	int inv_item_stealth_frigggggs;

	if (value == 65)
	{
		if (lara_item->pos.y_pos <= 13824)
		{
			if (have_i_got_object(PICKUP_ITEM1))
				gfLevelComplete = gfCurrentLevel + 1;
		}

		return;
	}

	if ((value == 28 || value == 29 || value == 30 || value == 31) && cutseq_trig == 0)
	{
		cutseq_num = value;
		cutrot = timer & 3;
		return;
	}

	if (!lara.burn)
	{
		if (value == 23)
		{
			if (cutseq_trig == 0)
				if (CheckCutPlayed(23))
					richcutfrigflag = 1;
		}

		if (cutseq_trig == 0 && !CheckCutPlayed(value))
		{
			cutrot = timer & 3;

			if (value <= 4 || value > 63)
			{
				if (value == 2)
					inv_item_stealth_frigggggs = CROWBAR_ITEM;
				else
					inv_item_stealth_frigggggs = WET_CLOTH;

				if (input & IN_ACTION &&
					!BinocularRange &&
					lara.gun_status == LG_NO_ARMS &&
					lara_item->current_anim_state == AS_STOP &&
					lara_item->anim_number == ANIMATION_LARA_STAY_IDLE &&
					GLOBAL_inventoryitemchosen == NO_ITEM &&
					have_i_got_object(inv_item_stealth_frigggggs))
				{
					if (CheckGuardOnTrigger())
						GLOBAL_enterinventory = inv_item_stealth_frigggggs;

					return;
				}

				if (GLOBAL_inventoryitemchosen != inv_item_stealth_frigggggs || !CheckGuardOnTrigger())
					return;

				if (inv_item_stealth_frigggggs == WET_CLOTH)
					lara.wetcloth = 1;

				GLOBAL_inventoryitemchosen = -1;
			}
			else
			{
				switch (value)
				{
				case 43:
					if (cutrot != 1 || is_object_in_room(lara_item->room_number, GREEN_TEETH) && !have_i_got_object(PUZZLE_ITEM2))
						cutseq_num = 43;

					return;

				case 39:
					if (input & IN_ACTION &&
						!BinocularRange &&
						lara.gun_status == LG_NO_ARMS &&
						lara_item->current_anim_state == AS_TREAD &&
						lara_item->anim_number == ANIMATION_LARA_UNDERWATER_IDLE &&
						GLOBAL_inventoryitemchosen == NO_ITEM &&
						have_i_got_object(PUZZLE_ITEM2))
						GLOBAL_enterinventory = PUZZLE_ITEM2;
					else if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM2)
					{
						GLOBAL_inventoryitemchosen = NO_ITEM;
						cutseq_num = 39;
					}

					return;

				case 38:
					if (input & IN_ACTION &&
						!BinocularRange &&
						lara.gun_status == LG_NO_ARMS &&
						lara_item->current_anim_state == AS_STOP &&
						lara_item->anim_number == ANIMATION_LARA_STAY_IDLE &&
						GLOBAL_inventoryitemchosen == NO_ITEM &&
						have_i_got_object(PUZZLE_ITEM1))
						GLOBAL_enterinventory = PUZZLE_ITEM1;
					else if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
					{
						GLOBAL_inventoryitemchosen = NO_ITEM;
						cutseq_num = 38;
					}

					return;

				case 10:
					if (have_i_got_object(PUZZLE_ITEM3))
					{
						SetCutPlayed(10);
						cutseq_num = 9;
						return;
					}

					break;

				case 24:
					short item_num, nex;
					ITEM_INFO* item;

					item_num = room[lara_item->room_number].item_number;

					if (item_num != -1)
					{
						while (1)
						{
							nex = items[item_num].next_item;
							item = &items[item_num];

							if (items[item_num].object_number == SCIENTIST && item->hit_points > 0)
							{
								if (item->anim_number == objects[BLUE_GUARD].anim_index + 62 && item->frame_number == anims[item->anim_number].frame_end)
									break;
							}

							item_num = nex;

							if (nex == -1)
								return;
						}

						cutseq_num = 24;
					}

					return;

				case 20:
					if (input & IN_ACTION &&
						!BinocularRange &&
						lara.gun_status == LG_NO_ARMS &&
						lara_item->current_anim_state == AS_STOP &&
						lara_item->anim_number == ANIMATION_LARA_STAY_IDLE &&
						GLOBAL_inventoryitemchosen == NO_ITEM &&
						have_i_got_object(KEY_ITEM7))
						GLOBAL_enterinventory = KEY_ITEM7;
					else if (GLOBAL_inventoryitemchosen == KEY_ITEM7)
					{
						GLOBAL_inventoryitemchosen = NO_ITEM;
						cutseq_num = 20;
					}

					return;

				case 14:
					if (input & IN_ACTION &&
						!BinocularRange &&
						lara.gun_status == LG_NO_ARMS &&
						lara_item->current_anim_state == AS_STOP &&
						lara_item->anim_number == ANIMATION_LARA_STAY_IDLE &&
						GLOBAL_inventoryitemchosen == NO_ITEM &&
						have_i_got_object(PUZZLE_ITEM2))
						GLOBAL_enterinventory = PUZZLE_ITEM2;
					else if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM2)
					{
						GLOBAL_inventoryitemchosen = NO_ITEM;
						cutseq_num = 14;
						remove_inventory_item(PUZZLE_ITEM2);
					}

					return;

				case 23:
					if (lara.hk_type_carried && !check_xray_machine_trigger() && !richcutfrigflag)
						cutseq_num = 23;

					return;
				}
			}

			cutseq_num = value;
			return;
		}
	}
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

long GetHeight(FLOOR_INFO* floor, long x, long y, long z)
{
	long height;
	room_info* r;
	short* data;
	short type, dx, dz;
	unsigned short trigger;
	ITEM_INFO* item;
	short xoff, yoff;
	short tilt, wat;
	short tilt0, tilt1, tilt2, tilt3;

	tiltxoff = 0;
	tiltyoff = 0;
	OnObject = 0;
	height_type = WALL;

	while (floor->pit_room != 0xFF)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		floor = &r->floor[((z - r->z) >> 10) + ((x - r->x) >> 10) * r->x_size];
	}

	height = floor->floor << 8;

	if (height == NO_HEIGHT)
		return height;

	trigger_index = 0;

	if (!floor->index)
		return height;

	data = &floor_data[floor->index];

	do
	{
		type = *(data++);

		switch ((type & 0x1F))
		{
		case DOOR_TYPE:
		case ROOF_TYPE:
		case SPLIT3:
		case SPLIT4:
		case NOCOLC1T:
		case NOCOLC1B:
		case NOCOLC2T:
		case NOCOLC2B:
			data++;
			break;

		case TILT_TYPE:
			xoff = (*data) >> 8;
			yoff = *(char*)(data);
			tiltxoff = xoff;
			tiltyoff = yoff;


			if ((ABS(xoff)) > 2 || (ABS(yoff)) > 2)
				height_type = BIG_SLOPE;
			else
				height_type = SMALL_SLOPE;

			if (xoff < 0)
				height -= (xoff * (z & 1023) >> 2);
			else
				height += (xoff * ((-1 - z) & 1023) >> 2);

			if (yoff < 0)
				height -= yoff * (x & 1023) >> 2;
			else
				height += yoff * ((-1 - x) & 1023) >> 2;

			data++;
			break;

		case TRIGGER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			data++;
			
			do
			{

				trigger = *(data++);

				if (trigger & 0x3C00)
				{
					if ((trigger & 0x3C00) == 1024 || (trigger & 0x3C00) == 12288)
						trigger = *(data++);

					continue;
				}

				item = &items[trigger & 0x3FF];

				if (objects[item->object_number].floor && !(item->flags & 0x8000))
					(*objects[item->object_number].floor)(item, x, y, z, &height);

			} while (!(trigger & 0x8000));

			break;

		case LAVA_TYPE:
			trigger_index = data - 1;
			break;

		case CLIMB_TYPE:
		case MONKEY_TYPE:
		case TRIGTRIGGER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			break;

		case SPLIT1:
		case SPLIT2:
		case NOCOLF1T:
		case NOCOLF1B:
		case NOCOLF2T:
		case NOCOLF2B:

			tilt = *data;
			tilt0 = tilt & 0xF;
			tilt1 = (tilt >> 4) & 0xF;
			tilt2 = (tilt >> 8) & 0xF;
			tilt3 = (tilt >> 12) & 0xF;
			dx = x & 1023;
			dz = z & 1023;
			height_type = SPLIT_TRI;

			if ((type & 0x1F) == SPLIT1 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B)
			{
				if (dx > (1024 - dz))
				{
					wat = (type >> 5) & 0x1F;

					if ((type >> 5) & 0x10)
						wat |= 0xFFF0;

					height += wat << 8;
					xoff = tilt3 - tilt0;
					yoff = tilt3 - tilt2;
				}
				else
				{
					wat = (type >> 10) & 0x1F;

					if ((type >> 10) & 0x10)
						wat |= 0xFFF0;

					height += wat << 8;
					xoff = tilt2 - tilt1;
					yoff = tilt0 - tilt1;
				}
			}
			else
			{
				if (dx > dz)
				{
					wat = (type >> 5) & 0x1F;

					if ((type >> 5) & 0x10)
						wat |= 0xFFF0;

					height += wat << 8;
					xoff = tilt3 - tilt0;
					yoff = tilt0 - tilt1;
				}
				else
				{
					
					wat = (type >> 10) & 0x1F;

					if ((type >> 10) & 0x10)
						wat |= 0xFFF0;

					height += wat << 8;
					xoff = tilt2 - tilt1;
					yoff = tilt3 - tilt2;
				}
			}

			tiltxoff = xoff;
			tiltyoff = yoff;

			if ((ABS(xoff)) > 2 || (ABS(yoff)) > 2)
				height_type = DIAGONAL;

			if (xoff >= 0)
				height += xoff * ((-1 - z) & 1023) >> 2;
			else
				height -= xoff * (z & 1023) >> 2;

			if (yoff >= 0)
				height += yoff * ((-1 - x) & 1023) >> 2;
			else
				height -= yoff * (x & 1023) >> 2;

			data++;
			break;

		default:
		//	Error("GetHeight(): Unknown type");
			Log(0, "**** GetHeight(): Unknown type ****");
			break;
		}

	} while (!(type & 0x8000));

	return height;
}

FLOOR_INFO* GetFloor(int x, int y, int z, short* room_number)
{
	room_info* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor;
	short door;

	r = &room[*room_number];

	do
	{
		x_floor = (z - r->z) >> 10;
		y_floor = (x - r->x) >> 10;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + (y_floor * r->x_size)];
		door = GetDoor(floor);

		if (door == 0xFF)
			break;

		*room_number = door;
		r = &room[door];

	} while (door != 0xFF);

	if (y < floor->floor << 8)
	{
		if (y < floor->ceiling << 8 && floor->sky_room != 0xFF)
		{
			do
			{
				if (CheckNoColCeilingTriangle(floor, x, z) == 1 || CheckNoColCeilingTriangle(floor, x, z) == -1 && y >= r->maxceiling)
					break;

				*room_number = floor->sky_room;
				r = &room[floor->sky_room];
				floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

				if (y >= floor->ceiling << 8)
					break;

			} while (floor->sky_room != 0xFF);
		}
	}
	else if (floor->pit_room != 0xFF)
	{
		while (1)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1 || CheckNoColFloorTriangle(floor, x, z) == -1 && y < r->minfloor)
				break;

			*room_number = floor->pit_room;
			r = &room[floor->pit_room];

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

			if (y < floor->floor << 8)
				break;

			if (floor->pit_room == 0xFF)
				return floor;
		}
	}

	return floor;
}

int ExplodeItemNode(ITEM_INFO* item, int Node, int NoXZVel, long bits)
{
	OBJECT_INFO* object;
	short** meshpp;

	if (!(item->mesh_bits & (1 << Node)))
		return 0;

	if (item->object_number == SWITCH_TYPE7 && (gfCurrentLevel == LVL5_SINKING_SUBMARINE || gfCurrentLevel == LVL5_BASE))
		SoundEffect(SFX_SMASH_METAL, &item->pos, SFX_DEFAULT);
	else if (bits == 256)
		bits = -64;

	GetSpheres(item, Slist, 3);
	object = &objects[item->object_number];
	ShatterItem.YRot = item->pos.y_rot;
	meshpp = &meshes[object->mesh_index + 2 * Node];
	ShatterItem.Bit = 1 << Node;
	ShatterItem.meshp = *meshpp;
	ShatterItem.Sphere.x = Slist[Node].x;
	ShatterItem.Sphere.y = Slist[Node].y;
	ShatterItem.Sphere.z = Slist[Node].z;
	ShatterItem.il = &item->il;
	ShatterItem.Flags = item->object_number != CROSSBOW_BOLT ? 0 : 1024;
	ShatterObject(&ShatterItem, 0, (short)bits, item->room_number, NoXZVel);
	item->mesh_bits &= ~ShatterItem.Bit;
	return 1;
}

short GetDoor(FLOOR_INFO* floor)
{
	short* data;
	short type;

	if (!floor->index)
		return 255;

	data = &floor_data[floor->index];
	type = *data++;

	if ((type & 0x1F) == TILT_TYPE
		|| (type & 0x1F) == SPLIT1
		|| (type & 0x1F) == SPLIT2
		|| (type & 0x1F) == NOCOLF1B
		|| (type & 0x1F) == NOCOLF1T
		|| (type & 0x1F) == NOCOLF2B
		|| (type & 0x1F) == NOCOLF2T)
	{
		if (type < 0)
			return 255;
	
		data++;
		type = *data++;
	}

	if ((type & 0x1F) == ROOF_TYPE
		|| (type & 0x1F) == SPLIT3
		|| (type & 0x1F) == SPLIT4
		|| (type & 0x1F) == NOCOLC1B
		|| (type & 0x1F) == NOCOLC1T
		|| (type & 0x1F) == NOCOLC2B
		|| (type & 0x1F) == NOCOLC2T)
	{
		if (type < 0)
			return 255;

		data++;
		type = *data++;
	}

	if ((type & 0x1F) == DOOR_TYPE)
		return *data;

	return 255;
}

long GetCeiling(FLOOR_INFO* floor, long x, long y, long z)
{
	long xoff, yoff, height, h1, h2;
	room_info* r;
	ITEM_INFO* item;
	FLOOR_INFO* f;
	short* data, type, trigger, dx, dz, t0, t1, t2, t3, hadj, ended;

	f = floor;

	while (f->sky_room != 0xFF)
	{
		if (CheckNoColCeilingTriangle(floor, x, z) == 1)
			break;

		r = &room[f->sky_room];
		xoff = (z - r->z) >> 10;
		yoff = (x - r->x) >> 10;
		f = &r->floor[xoff + r->x_size * yoff];
	}

	height = 256 * f->ceiling;

	if (height != NO_HEIGHT)
	{
		if (f->index)
		{
			data = &floor_data[f->index];
			type = *data;
			++data;
			ended = 0;

			if ((type & 0x1F) == TILT_TYPE || (type & 0x1F) == SPLIT1 || (type & 0x1F) == SPLIT2 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B || (type & 0x1F) == NOCOLF2T || (type & 0x1F) == NOCOLF2B)
			{
				++data;

				if (type & 0x8000)
					ended = 1;

				type = *data;
				++data;
			}

			if (!ended)
			{
				h1 = 0;
				h2 = 0;

				if ((type & 0x1F) != ROOF_TYPE)
				{
					if ((type & 0x1F) == SPLIT3 || (type & 0x1F) == SPLIT4 || (type & 0x1F) == NOCOLC1T || (type & 0x1F) == NOCOLC1B || (type & 0x1F) == NOCOLC2T || (type & 0x1F) == NOCOLC2B)
					{
						dx = x & 0x3FF;
						dz = z & 0x3FF;
						t0 = -(*data & 0xF);
						t1 = -(*data >> 4 & 0xF);
						t2 = -(*data >> 8 & 0xF);
						t3 = -(*data >> 12 & 0xF);

						if ((type & 0x1F) == SPLIT3 || (type & 0x1F) == NOCOLC1T || (type & 0x1F) == NOCOLC1B)
						{
							if (dx <= 1024 - dz)
							{
								hadj = type >> 10 & 0x1F;

								if (hadj & 0x10)
									hadj |= 0xFFF0;

								height += 256 * hadj;
								h1 = t2 - t1;
								h2 = t3 - t2;
							}
							else
							{
								hadj = type >> 5 & 0x1F;

								if (hadj & 0x10)
									hadj |= 0xFFF0;

								height += 256 * hadj;
								h1 = t3 - t0;
								h2 = t0 - t1;
							}
						}
						else
						{
							if (dx <= dz)
							{
								hadj = type >> 10 & 0x1F;

								if (hadj & 0x10)
									hadj |= 0xFFF0;

								height += 256 * hadj;
								h1 = t2 - t1;
								h2 = t0 - t1;
							}
							else
							{
								hadj = type >> 5 & 0x1F;

								if (hadj & 0x10)
									hadj |= 0xFFF0;

								height += 256 * hadj;
								h1 = t3 - t0;
								h2 = t3 - t2;
							}
						}
					}
				}
				else
				{
					h1 = *data >> 8;
					h2 = *(char*) data;
				}

				if (h1 < 0)
					height += (z & 0x3FF) * h1 >> 2;
				else
					height -= (-1 - z & 0x3FF) * h1 >> 2;

				if (h2 < 0)
					height += (-1 - x & 0x3FF) * h2 >> 2;
				else
					height -= (x & 0x3FF) * h2 >> 2;
			}
		}

		while (floor->pit_room != 0xFF)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->pit_room];
			xoff = (z - r->z) >> 10;
			yoff = (x - r->x) >> 10;
			floor = &r->floor[xoff + r->x_size * yoff];
		}

		if (floor->index)
		{
			data = &floor_data[floor->index];

			do
			{
				type = *data;
				++data;

				switch (type & 0x1F)
				{
				case DOOR_TYPE:
				case TILT_TYPE:
				case ROOF_TYPE:
				case SPLIT1:
				case SPLIT2:
				case SPLIT3:
				case SPLIT4:
				case NOCOLF1T:
				case NOCOLF1B:
				case NOCOLF2T:
				case NOCOLF2B:
				case NOCOLC1T:
				case NOCOLC1B:
				case NOCOLC2T:
				case NOCOLC2B:
					++data;
					break;

				case TRIGGER_TYPE:
					++data;

					do
					{
						trigger = *data;
						++data;

						if (trigger & 0x3C00)
						{
							if ((trigger & 0x3C00) == 1024 || (trigger & 0x3C00) == 12288)
							{
								trigger = *data;
								++data;
							}
						}
						else
						{
							item = &items[trigger & 0x3FF];

							if (objects[item->object_number].ceiling && !(item->flags & 0x8000))
								objects[item->object_number].ceiling(item, x, y, z, &height);
						}
					}
					while (!(trigger & 0x8000));
					break;

				default:
					Log(0, "**** GetCeiling(): Unknown type ****");
					break;
				}
			}
			while (!(type & 0x8000));
		}
	}

	return height;
}

int LOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	int los1, los2;

	target->room_number = start->room_number;

	if (ABS(target->z - start->z) > ABS(target->x - start->x))
	{
		los1 = xLOS(start, target);
		los2 = zLOS(start, target);
	}
	else
	{
		los1 = zLOS(start, target);
		los2 = xLOS(start, target);
	}

	if (los2)
	{
		GetFloor(target->x, target->y, target->z, &target->room_number);

		if (ClipTarget(start, target) && los1 == 1 && los2 == 1)
			return 1;
	}
	return 0;
}

int xLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	long dx, dy, dz, x, y, z;
	short room_number, last_room;
	FLOOR_INFO* floor;

	dx = target->x - start->x;

	if (!dx)
		return 1;

	dy = 1024 * (target->y - start->y) / dx;
	dz = 1024 * (target->z - start->z) / dx;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dx < 0)
	{
		x = start->x & 0xFFFFFC00;
		y = ((x - start->x) * dy >> 10) + start->y;
		z = ((x - start->x) * dz >> 10) + start->z;

		while (x > target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x - 1, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x - 1, y, z) || y < GetCeiling(floor, x - 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			x -= 1024;
			y -= dy;
			z -= dz;
		}
	}
	else
	{
		x = start->x | 0x3FF;
		y = ((x - start->x) * dy >> 10) + start->y;
		z = ((x - start->x) * dz >> 10) + start->z;

		while (x < target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x + 1, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x + 1, y, z) || y < GetCeiling(floor, x + 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			x += 1024;
			y += dy;
			z += dz;
		}
	}

	target->room_number = room_number;
	return 1;
}

int zLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	long dx, dy, dz, x, y, z;
	short room_number, last_room;
	FLOOR_INFO* floor;

	dz = target->z - start->z;

	if (!dz)
		return 1;

	dx = 1024 * (target->x - start->x) / dz;
	dy = 1024 * (target->y - start->y) / dz;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dz < 0)
	{
		z = start->z & 0xFFFFFC00;
		x = ((z - start->z) * dx >> 10) + start->x;
		y = ((z - start->z) * dy >> 10) + start->y;

		while (z > target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x, y, z - 1, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z - 1) || y < GetCeiling(floor, x, y, z - 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			z -= 1024;
			x -= dx;
			y -= dy;
		}
	}
	else
	{
		z = start->z | 0x3FF;
		x = ((z - start->z) * dx >> 10) + start->x;
		y = ((z - start->z) * dy >> 10) + start->y;

		while (z < target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x, y, z + 1, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				++number_los_rooms;
			}

			if (y > GetHeight(floor, x, y, z + 1) || y < GetCeiling(floor, x, y, z + 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			z += 1024;
			x += dx;
			y += dy;
		}
	}

	target->room_number = room_number;
	return 1;
}

int ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target)
{
	long dx, dy, dz, lp;
	short room_no;
	GAME_VECTOR src;

	room_no = target->room_number;

	if (target->y > GetHeight(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = (7 * (target->x - start->x) >> 3) + start->x;
		src.y = (7 * (target->y - start->y) >> 3) + start->y;
		src.z = (7 * (target->z - start->z) >> 3) + start->z;

		for (lp = 3; lp > 0; --lp)
		{
			dx = ((target->x - src.x) * lp >> 2) + src.x;
			dy = ((target->y - src.y) * lp >> 2) + src.y;
			dz = ((target->z - src.z) * lp >> 2) + src.z;

			if (dy < GetHeight(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	room_no = target->room_number;

	if (target->y < GetCeiling(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = (7 * (target->x - start->x) >> 3) + start->x;
		src.y = (7 * (target->y - start->y) >> 3) + start->y;
		src.z = (7 * (target->z - start->z) >> 3) + start->z;

		for (lp = 3; lp > 0; --lp)
		{
			dx = ((target->x - src.x) * lp >> 2) + src.x;
			dy = ((target->y - src.y) * lp >> 2) + src.y;
			dz = ((target->z - src.z) * lp >> 2) + src.z;

			if (dy > GetCeiling(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	return 1;
}

int GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, int DrawTarget, int firing)
{
	GAME_VECTOR target;
	PHD_VECTOR v;
//	MESH_INFO* StaticMesh, *Mesh;//StaticMesh: unreferenced local variable
	MESH_INFO* Mesh;
	short item_no, hit, ricochet, room_number, TriggerItems[8], NumTrigs;
	int i;
	ITEM_INFO* shotitem;

	target.x = dest->x;
	target.y = dest->y;
	target.z = dest->z;
	ricochet = LOS(src, &target);
	GetFloor(target.x, target.y, target.z, &target.room_number);

	if (firing && LaserSight)
	{
		lara.has_fired = 1;
		lara.Fired = 1;

		if (lara.gun_type == WEAPON_REVOLVER)
			SoundEffect(SFX_REVOLVER, NULL, 0);
	}

	hit = 0;
	item_no = ObjectOnLOS2(src, dest, &v, &Mesh);

	if (item_no != 999)
	{
		target.x = v.x - ((v.x - src->x) >> 5);
		target.y = v.y - ((v.y - src->y) >> 5);
		target.z = v.z - ((v.z - src->z) >> 5);
		GetFloor(target.x, target.y, target.z, &target.room_number);

		if (item_no >= 0)
			lara.target = &items[item_no];

		if (firing)
		{
			if (lara.gun_type != WEAPON_CROSSBOW)
			{
				if (item_no < 0)
				{
					if (Mesh->static_number >= 50 && Mesh->static_number < 58)
					{
						ShatterObject(NULL, Mesh, 128, target.room_number, 0);
						SmashedMeshRoom[SmashedMeshCount] = target.room_number;
						SmashedMesh[SmashedMeshCount] = Mesh;
						++SmashedMeshCount;
						Mesh->Flags &= ~0x1;
						SoundEffect(ShatterSounds[gfCurrentLevel - 5][Mesh->static_number], (PHD_3DPOS *) Mesh, 0);
						//to reach the block Mesh->static_number has to be bigger than 50, and the range for ShatterSounds[][here] is 10.
						//this is an original game bug. fixing static_number back to range with -50 removes errors, but messes up the
						//sounds of shattering ingame.
					}

					TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
					TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
				}
				else
				{
					shotitem = &items[item_no];

					if (shotitem->object_number != SWITCH_TYPE7 && shotitem->object_number != SWITCH_TYPE8)
					{
						if (objects[shotitem->object_number].explodable_meshbits & ShatterItem.Bit && LaserSight)
						{
							if (!objects[shotitem->object_number].intelligent)
							{
								ShatterObject(&ShatterItem, 0, 128, target.room_number, 0);
								shotitem->mesh_bits &= ~ShatterItem.Bit;
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
							}
							else if (shotitem->object_number != TWOGUN)
							{
								shotitem->hit_points -= 30;

								if (shotitem->hit_points < 0)
									shotitem->hit_points = 0;

								HitTarget(shotitem, &target, weapons[lara.gun_type].damage, 0);
							}
							else if (ABS(phd_atan(lara_item->pos.z_pos - shotitem->pos.z_pos, lara_item->pos.x_pos - shotitem->pos.x_pos) - shotitem->pos.y_rot) < 16384)
							{
								shotitem->hit_points = 0;
								HitTarget(shotitem, &target, weapons[lara.gun_type].damage, 0);
							}
						}
						else if (DrawTarget && (lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_HK))
						{
							if (objects[shotitem->object_number].intelligent)
								HitTarget(shotitem, &target, weapons[lara.gun_type].damage, 0);
							else if (objects[shotitem->object_number].HitEffect == 3)
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
						}
						else if (shotitem->object_number >= SMASH_OBJECT1 && shotitem->object_number <= SMASH_OBJECT8)
							SmashObject(item_no);
						else
						{
							if (objects[shotitem->object_number].HitEffect == 1)
								DoBloodSplat(target.x, target.y, target.z, (GetRandomControl() & 3) + 3, shotitem->pos.y_rot, shotitem->room_number);
							else if (objects[shotitem->object_number].HitEffect == 2)
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, -5);
							else if (objects[shotitem->object_number].HitEffect == 3)
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);

							shotitem->hit_status = 1;

							if (!objects[shotitem->object_number].undead)
								shotitem->hit_points -= weapons[lara.gun_type].damage;
						}
					}
					else
					{
						if (ShatterItem.Bit == 1 << (objects[shotitem->object_number].nmeshes - 1))
						{
							if (!(shotitem->flags & IFLAG_UNK40))
							{
								if (shotitem->object_number == SWITCH_TYPE7)
									ExplodeItemNode(shotitem, objects[shotitem->object_number].nmeshes - 1, 0, 64);

								if (shotitem->trigger_flags == 444 && shotitem->object_number == SWITCH_TYPE8)
									ProcessExplodingSwitchType8(shotitem);
								else if (shotitem->flags & IFLAG_ACTIVATION_MASK && (shotitem->flags & IFLAG_ACTIVATION_MASK) != IFLAG_ACTIVATION_MASK)
								{
									room_number = shotitem->room_number;
									GetHeight(GetFloor(shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos, &room_number), shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos);
									TestTriggers(trigger_index, 1, shotitem->flags & IFLAG_ACTIVATION_MASK);
								}
								else
								{
									NumTrigs = GetSwitchTrigger(shotitem, TriggerItems, 1);

									for (i = NumTrigs - 1; i >= 0; --i)
									{
										AddActiveItem(TriggerItems[i]);
										items[TriggerItems[i]].status = ITEM_ACTIVE;
										items[TriggerItems[i]].flags |= IFLAG_ACTIVATION_MASK;
									}
								}
							}

							if (shotitem->status != ITEM_DEACTIVATED)
							{
								AddActiveItem(item_no);
								shotitem->status = ITEM_ACTIVE;
								shotitem->flags |= IFLAG_UNK40 | IFLAG_ACTIVATION_MASK;
							}
						}

						TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
					}
				}
			}
			else if (LaserSight && item_no >= 0)
			{
				shotitem = &items[item_no];

				if (shotitem->object_number == GRAPPLING_TARGET && shotitem->mesh_bits & 1)
				{
					LaserSightCol = gfLevelFlags & GF_LVOP_TRAIN;
					if (gfLevelFlags & GF_LVOP_TRAIN)
					{
						target.x = shotitem->pos.x_pos;
						target.y = shotitem->pos.y_pos;
						target.z = shotitem->pos.z_pos;
					}
					FireCrossBowFromLaserSight(src, &target);
				}
			}
		}
		else if (item_no >= 0)
		{
			shotitem = &items[item_no];

			if (shotitem->object_number == GRAPPLING_TARGET && lara.gun_type == WEAPON_CROSSBOW && shotitem->mesh_bits & 1)
				LaserSightCol = gfLevelFlags & GF_LVOP_TRAIN;
		}

		hit = 1;
	}
	else if (lara.gun_type != WEAPON_CROSSBOW)
	{
		target.x -= (target.x - src->x) >> 5;
		target.y -= (target.y - src->y) >> 5;
		target.z -= (target.z - src->z) >> 5;

		if (firing && !ricochet)
			TriggerRicochetSpark(&target, lara_item->pos.y_rot, 8, 0);
	}
	else if (firing && LaserSight && LaserSightCol == (gfLevelFlags & GF_LVOP_TRAIN))
		FireCrossBowFromLaserSight(src, &target);

	if (DrawTarget && (hit || !ricochet))
	{
		TriggerDynamic(target.x, target.y, target.z, 64, 255, 0, 0);
		LaserSightActive = 1;
		LaserSightX = target.x;
		LaserSightY = target.y;
		LaserSightZ = target.z;
	}

	return hit;
}

int ObjectOnLOS2(GAME_VECTOR* start, GAME_VECTOR* target, PHD_VECTOR* Coord, MESH_INFO** StaticMesh)
{
	ITEM_INFO* item;
	MESH_INFO* mesh;
	room_info* r;
	PHD_3DPOS ItemPos;
	long dx, dy, dz;
	short item_number, i, lp;
	short* bounds;

	ClosestItem = 999;
	dx = target->x - start->x;
	dy = target->y - start->y;
	dz = target->z - start->z;
	ClosestDist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

	for (i = 0; i < number_los_rooms; ++i)
	{
		r = &room[los_rooms[i]];

		for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (item->status != ITEM_DEACTIVATED && item->status != ITEM_INVISIBLE && (item->object_number != LARA && objects[item->object_number].collision || item->object_number == LARA && GetLaraOnLOS))
			{
				bounds = GetBoundsAccurate(item);
				ItemPos.x_pos = item->pos.x_pos;
				ItemPos.y_pos = item->pos.y_pos;
				ItemPos.z_pos = item->pos.z_pos;
				ItemPos.y_rot = item->pos.y_rot;

				if (DoRayBox(start, target, bounds, &ItemPos, Coord, item_number))
					target->room_number = los_rooms[i];
			}
		}

		for (lp = 0; lp < r->num_meshes; ++lp)
		{
			mesh = &r->mesh[lp];

			if (mesh->Flags & 1)
			{
				ItemPos.x_pos = mesh->x;
				ItemPos.y_pos = mesh->y;
				ItemPos.z_pos = mesh->z;
				ItemPos.y_rot = mesh->y_rot;

				if (DoRayBox(start, target, &static_objects[mesh->static_number].x_minc, &ItemPos, Coord, -1 - mesh->static_number))
				{
					*StaticMesh = mesh;
					target->room_number = los_rooms[i];
				}
			}
		}
	}

	Coord->x = ClosestCoord.x;
	Coord->y = ClosestCoord.y;
	Coord->z = ClosestCoord.z;
	return ClosestItem;
}

void inject_control()
{
	INJECT(0x004147C0, ControlPhase);
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
	INJECT(0x0041B280, NeatAndTidyTriggerCutscene);
	INJECT(0x0041B8B0, is_object_in_room);
	INJECT(0x0041B930, check_xray_machine_trigger);
	INJECT(0x00415FB0, GetHeight);
	INJECT(0x00415B20, GetFloor);
	INJECT(0x0041ABF0, ExplodeItemNode);
	INJECT(0x00417C00, GetDoor);
	INJECT(0x00417640, GetCeiling);
	INJECT(0x00417CF0, LOS);
	INJECT(0x00417DC0, zLOS);
	INJECT(0x004181F0, xLOS);
	INJECT(0x00418620, ClipTarget);
	INJECT(0x0041A170, GetTargetOnLOS);
	INJECT(0x00419110, ObjectOnLOS2);
}
