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
#include "deltapak.h"
#include "delstuff.h"
#include "../specific/matrix_shit.h"

char special1_flip_flag;//original one is on 0051CA84. some sort of flipmap flag for special1 cutscene in the title. original name unknown

unsigned short special2_pistols_info[13] =
{
	0x00C4, 0x00CC, 0x00D4, 0x00DC, 0x00E4, 0x00EC, 0x00F4, 0x00FC, 0x0104, 0x010C,
	0x0114, 0x0121, 0xFFFF
};

unsigned short special3_pistols_info[] =
{
	0x0102, 0x010A, 0x0114, 0xFFFF, 0, 0, 0, 0, 0x00FF, 0x0100, 0x0114, 0xFF9C, 0x0064, 0x0100, 0x0200
};

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
					lara_item->current_anim_state == STATE_LARA_STOP &&
					lara_item->anim_number == ANIMATION_LARA_STAY_IDLE &&
					GLOBAL_inventoryitemchosen == NO_ITEM &&
					have_i_got_object(inv_item_stealth_frigggggs))
				{
					if (CheckGuardOnTrigger())
						GLOBAL_enterinventory = inv_item_stealth_frigggggs;

					return;
				}

				if (GLOBAL_enterinventory != inv_item_stealth_frigggggs || !CheckGuardOnTrigger())
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
						lara_item->current_anim_state == STATE_LARA_UNDERWATER_STOP &&
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
						lara_item->current_anim_state == STATE_LARA_STOP &&
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
						lara_item->current_anim_state == STATE_LARA_STOP &&
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
						lara_item->current_anim_state == STATE_LARA_STOP &&
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

void _special1_control()
{
	if (GLOBAL_cutseq_frame == 300)
	{
		FlipMap(0);
		special1_flip_flag = 1;
	}

	dword_00E916F0 = 1;
}

void _special1_end()
{
	if (special1_flip_flag)
	{
		FlipMap(0);
		special1_flip_flag = 0;
	}

	if (!bDoCredits)
	{
		lara_item->mesh_bits = 0;
		trigger_title_spotcam(2);
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special2_control()
{
	PHD_VECTOR pos;
	ITEM_INFO* item;

	pos.x = 12;
	pos.y = 200;
	pos.z = 92;
	deal_with_actor_shooting(special2_pistols_info, 1, 13, &pos);

	switch (GLOBAL_cutseq_frame)
	{
	case 197:
		item = find_a_fucking_item(ANIMATING1_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 200:
		item = find_a_fucking_item(ANIMATING2_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 201:
		triggerwindowsmash(50);
		break;
	case 207:
		item = find_a_fucking_item(ANIMATING3_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 209:
		triggerwindowsmash(52);
		break;
	case 221:
		item = find_a_fucking_item(ANIMATING4_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 224:
		triggerwindowsmash(54);
		break;
	case 229:
		Cutanimate(446);
		break;
	case 245:
		triggerwindowsmash(56);
		break;
	case 261:
		item = find_a_fucking_item(ANIMATING5_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 269:
		triggerwindowsmash(58);
		break;
	case 280:
		item = find_a_fucking_item(ANIMATING6_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 282:
		item = find_a_fucking_item(ANIMATING7_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 284:
		Cutanimate(ANIMATING5);
		break;
	case 291:
		triggerwindowsmash(60);
		break;
	}
}

void _special2_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(3);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special3_control()
{
	PHD_VECTOR pos, pos2;
	SPARKS* sptr;

	pos.x = 12;
	pos.y = 200;
	pos.z = 92;
	deal_with_actor_shooting(special3_pistols_info, 1, 13, &pos);

	if (GLOBAL_cutseq_frame == 92 || GLOBAL_cutseq_frame == 93 || GLOBAL_cutseq_frame == 143 || GLOBAL_cutseq_frame == 144)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, 0);
		pos.z -= 256;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
	}

	if (GLOBAL_cutseq_frame == 472 || GLOBAL_cutseq_frame == 528)
	{
		pos.x = 8;
		pos.y = 230;
		pos.z = 40;
		GetActorJointAbsPosition(2, 5, &pos);

		pos2.x = 8;
		pos2.y = 4326;
		pos2.z = 40;
		GetActorJointAbsPosition(2, 5, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);
	}

	if (GLOBAL_cutseq_frame == 500)
	{
		pos.x = 0;
		pos.y = 230;
		pos.z = 40;
		GetActorJointAbsPosition(2, 8, &pos);

		pos2.x = 0;
		pos2.y = 4326;
		pos2.z = 40;
		GetActorJointAbsPosition(2, 8, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);

	}

	if (GLOBAL_cutseq_frame == 610)
	{
		pos.x = 0;
		pos.y = 2278;
		pos.z = 40;
		GetActorJointAbsPosition(2, 8, &pos);

		pos2.x = 0;
		pos2.y = 0;
		pos2.z = -1024;
		GetActorJointAbsPosition(1, 0, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);
	}

	if (GLOBAL_cutseq_frame >= 610 && GLOBAL_cutseq_frame <= 642)
	{
		pos2.x = 0;
		pos2.y = 0;
		pos2.z = -1024;
		GetActorJointAbsPosition(1, 0, &pos2);

		TriggerDynamic(pos2.x, pos2.y, pos2.z, (GetRandomControl() & 3) + 8, 0,
			(((GetRandomControl() & 0x3F) + 64) * (642 - GLOBAL_cutseq_frame)) >> 5,
			((642 - GLOBAL_cutseq_frame) * ((GetRandomControl() & 0x3F) + 180)) >> 5);

		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->dR = 0;
		sptr->sB = (16 * ((GetRandomControl() & 0x7F) + 128)) >> 4;
		sptr->sR = sptr->sB - (sptr->sB >> 2);
		sptr->sG = sptr->sB - (sptr->sB >> 2);
		sptr->dB = (16 * ((GetRandomControl() & 0x7F) + 32)) >> 4;
		sptr->dG = (unsigned char)sptr->dB >> 2;
		sptr->FadeToBlack = 8;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 24;
		sptr->sLife = (GetRandomControl() & 3) + 24;
		sptr->x = pos2.x;
		sptr->y = pos2.y;
		sptr->z = pos2.z;
		sptr->Xvel = ((COS(2 * GetRandomControl()) >> 2) * SIN(GetRandomControl() * 2)) >> 14;
		sptr->Zvel = ((COS(2 * GetRandomControl()) >> 2) * COS(GetRandomControl() * 2)) >> 14;
		sptr->Yvel = SIN(-GetRandomControl() * 2) >> 4;
		sptr->Friction = 0;
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x7F) - 64;
		sptr->MaxYvel = 0;
		sptr->Scalar = 2;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
		sptr->dSize = 1;
		sptr->sSize = (GetRandomControl() & 0x3F) + 16;
		sptr->Size = (GetRandomControl() & 0x3F) + 16;
	}
}

void _special3_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(4);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special4_control()
{
	PHD_VECTOR pos;

	pos.x = 85834;
	pos.z = 72300;
	pos.y = -3138;

	TriggerFireFlame(85834, -3010, 72300, -1, 1);
	TriggerFireFlame(85834, -3010, 72044, -1, 1);
	TriggerFireFlame(85834, -3010, 72556, -1, 1);
	TriggerFireFlame(85578, -3010, 72300, -1, 1);
	TriggerFireFlame(86090, -3010, 72300, -1, 1);

	if (GLOBAL_cutseq_frame >= 460)
		FlamingHell(&pos);

	if (GLOBAL_cutseq_frame < 470)
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 31, (GetRandomControl() & 0xF) + 31, GetRandomControl() & 0x3F);
	else
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x7F) + 127, (GetRandomControl() & 0x7F) + 127, GetRandomControl() & 0x3F);

	if (GLOBAL_cutseq_frame == 390)
		Cutanimate(STROBE_LIGHT);
}

void _special4_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(1);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void ResetCutItem(int item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = -1;
}

void resetwindowsmash(int item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = 1;
}

void triggerwindowsmash(int item_num)
{
	ITEM_INFO* item = find_a_fucking_item(item_num);
	ExplodeItemNode(item, 0, 0, 64);
	item->mesh_bits = 2;
}

void FlamingHell(PHD_VECTOR* pos)
{

	SPARKS* sptr;
	int r, size;

	r = (GetRandomControl() & 0x1FF) - 128;

	if (r < 512)
		r = 512;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dB = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0x3F) + 90;
	sptr->sLife = (GetRandomControl() & 0x3F) + 90;
	sptr->x = (GetRandomControl() & 0xFF) + pos->x - 128;
	sptr->y = (GetRandomControl() & 0xFF) + (pos->y - 128) - 128;
	sptr->z = (GetRandomControl() & 0xFF) + pos->z - 128;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Flags = 538;
	sptr->Scalar = 2;
	size = (GetRandomControl() & 0xF) + (r >> 6) + 16;
	sptr->dSize = size;
	sptr->sSize = size >> 1;
	sptr->Size = size >> 1;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -(short)r;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->dSize += sptr->dSize >> 2;
}

void FireTwoGunTitleWeapon(PHD_VECTOR* pos, PHD_VECTOR* pos2)
{
	TWOGUN_INFO* gun;
	PHD_VECTOR pos3;
	short angles[2];
	int i;

	phd_GetVectorAngles(pos2->x - pos->x, pos2->y - pos->y, pos2->z - pos->z, &angles[0]);
	gun = &twogun[0];

	i = 0;

	if (gun->life != 0)
	{
		gun++;

		do
		{
			if (++i < 4)
			{
				if (gun->life == 0)
					break;
			}

		} while (i++ != 3);
	}

	gun->pos.x_pos = pos->x;
	gun->pos.y_pos = pos->y;
	gun->pos.z_pos = pos->z;
	gun->pos.y_rot = angles[0];
	gun->pos.x_rot = angles[1];
	gun->pos.z_rot = 0;
	gun->life = 17;
	gun->spin = (short)(GetRandomControl() << 11);
	gun->dlength = 4096;
	gun->r = 0;
	gun->b = -1;
	gun->g = 96;
	gun->fadein = 8;
	TriggerLightningGlow(gun->pos.x_pos, gun->pos.y_pos, gun->pos.z_pos, ((gun->b >> 1) << 16) | ((gun->g & 0xFFFFFFFE) << 7) | (((GetRandomControl() & 0x3) + 64) << 24));
	TriggerLightning(pos, pos2, (GetRandomControl() & 7) + 8, (gun->b & 0xFF) | (((gun->b & 0xFF) | 0x160000) >> 8), 12, 80, 5);
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
	INJECT(0x0041B280, NeatAndTidyTriggerCutscene);
	INJECT(0x0041B8B0, is_object_in_room);
	INJECT(0x0041B930, check_xray_machine_trigger);
	INJECT(0x0041BCF0, _special1_control);
	INJECT(0x0041BD40, _special1_end);
	INJECT(0x0041BE90, _special2_control);
	INJECT(0x0041C0F0, _special2_end);
	INJECT(0x0041C160, _special3_control);
	INJECT(0x0041C610, _special3_end);
	INJECT(0x0041C690, _special4_control);
	INJECT(0x0041C860, _special4_end);
	INJECT(0x0041C8B0, ResetCutItem);
	INJECT(0x0041C920, resetwindowsmash);
	INJECT(0x0041C8E0, triggerwindowsmash);
	INJECT(0x0041C950, FlamingHell);
	INJECT(0x0041CB10, FireTwoGunTitleWeapon);
}
