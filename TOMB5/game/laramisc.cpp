#include "../tomb5/pch.h"
#include "laramisc.h"
#include "lara_states.h"
#include "gameflow.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "control.h"
#include "collide.h"
#include "effects.h"
#include "sound.h"
#include "laraswim.h"
#include "objects.h"
#include "rope.h"
#include "lara.h"
#include "larasurf.h"
#include "../specific/function_stubs.h"
#include "../specific/audio.h"
#include "../specific/file.h"
#include "camera.h"
#include "spotcam.h"
#include "effect2.h"
#include "../specific/input.h"
#include "savegame.h"
#include "newinv2.h"
#include "../specific/dxshell.h"
#include "../tomb5/tomb5.h"

COLL_INFO mycoll;

static COLL_INFO* lara_coll = &mycoll;

void GetLaraDeadlyBounds()
{
	short* bounds;
	short tbounds[6];

	bounds = GetBoundsAccurate(lara_item);

	phd_PushUnitMatrix();
	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);
	phd_SetTrans(0, 0, 0);
	mRotBoundingBoxNoPersp(bounds, tbounds);
	phd_PopMatrix();

	DeadlyBounds[0] = lara_item->pos.x_pos + tbounds[0];
	DeadlyBounds[1] = lara_item->pos.x_pos + tbounds[1];
	DeadlyBounds[2] = lara_item->pos.y_pos + tbounds[2];
	DeadlyBounds[3] = lara_item->pos.y_pos + tbounds[3];
	DeadlyBounds[4] = lara_item->pos.z_pos + tbounds[4];
	DeadlyBounds[5] = lara_item->pos.z_pos + tbounds[5];
}

void InitialiseLaraAnims(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->anim_number = ANIM_TREAD;
		item->frame_number = anims[ANIM_TREAD].frame_base;
		item->current_anim_state = AS_TREAD;
		item->goal_anim_state = AS_TREAD;

		item->fallspeed = 0;
		lara.water_status = LW_UNDERWATER;
	}
	else
	{
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->current_anim_state = AS_STOP;
		item->goal_anim_state = AS_STOP;

		lara.water_status = LW_ABOVE_WATER;
	}
}

void InitialiseLaraLoad(short item_num)
{
	lara.item_number = item_num;
	lara_item = &items[item_num];
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.death_count = 0;
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);

	if (input & IN_WALK && !(input & IN_LOOK))
	{
		lara.water_status = LW_ABOVE_WATER;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->anim_number = ANIM_STOP;
		item->pos.z_rot = 0;
		item->pos.x_rot = 0;
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.head_y_rot = 0;
		lara.head_x_rot = 0;
		lara.gun_status = 0;
		LaraInitialiseMeshes();
		lara.mesh_effects = 0;
		lara_item->hit_points = 1000;
	}
}

void LaraInitialiseMeshes()
{
	for (int i = 0; i < 15; i++)
	{
		meshes[objects[LARA].mesh_index + 2 * i] = meshes[objects[LARA_SKIN].mesh_index + 2 * i];
		lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + 2 * i];
	}

	if (gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL)
		lara.mesh_ptrs[LM_TORSO] = meshes[objects[ANIMATING6_MIP].mesh_index + 2 * LM_TORSO];

	if (lara.gun_type == WEAPON_HK)
		lara.back_gun = HK_ANIM;
	else if (lara.shotgun_type_carried != W_NONE)
		lara.back_gun = SHOTGUN_ANIM;
	else if (lara.hk_type_carried != W_NONE)
		lara.back_gun = HK_ANIM;

	lara.gun_status = LG_NO_ARMS;
	lara.target = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_number = 0;
	lara.right_arm.lock = 0;
}

void LaraCheatGetStuff()
{
	if (objects[FLARE_INV_ITEM].loaded)
		lara.num_flares = -1;

	if (objects[SMALLMEDI_ITEM].loaded)
		lara.num_small_medipack = -1;

	if (objects[BIGMEDI_ITEM].loaded)
		lara.num_large_medipack = -1;

	if (objects[LASERSIGHT_ITEM].loaded)
		lara.lasersight = 1;

	if (objects[BOTTLE].loaded)
		lara.bottle = 1;

	if (objects[CLOTH].loaded)
		lara.wetcloth = CLOTH_DRY;

	if (objects[UZI_ITEM].loaded)
	{
		lara.uzis_type_carried = W_PRESENT | W_AMMO1;
		lara.num_uzi_ammo = -1;
	}

	if (objects[SHOTGUN_ITEM].loaded)
	{
		lara.shotgun_type_carried = W_PRESENT | W_AMMO1;
		lara.num_shotgun_ammo1 = -1;
		lara.num_shotgun_ammo2 = -1;
	}

	if (objects[REVOLVER_ITEM].loaded)
	{
		lara.sixshooter_type_carried = W_PRESENT | W_AMMO1;
		lara.num_revolver_ammo = -1;
	}
	
	if (objects[HK_ITEM].loaded)
	{
		lara.hk_type_carried = W_PRESENT | W_AMMO3;
		lara.num_hk_ammo1 = -1;
	}

	if (objects[CROSSBOW_ITEM].loaded)
	{
		lara.crossbow_type_carried = W_PRESENT | W_AMMO1 | W_LASERSIGHT;
		lara.num_crossbow_ammo1 = -1;
		lara.num_crossbow_ammo2 = 0;
	}

	dels_give_lara_items_cheat();
}

void LaraCheatyBits()
{
#ifndef _DEBUG	//public releases have cheats in inventory, except for DOZY
	if (keymap[DIK_D] && keymap[DIK_O] && keymap[DIK_Z] && keymap[DIK_Y])
	{
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->current_anim_state = AS_SWIMCHEAT;
			lara_item->goal_anim_state = AS_SWIMCHEAT;
			lara.water_status = LW_FLYCHEAT;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 5460;
			lara_item->fallspeed = 30;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
		}
	}
#else
	if (keymap[DIK_F1])
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

	if (keymap[DIK_F2])
	{
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->current_anim_state = AS_SWIMCHEAT;
			lara_item->goal_anim_state = AS_SWIMCHEAT;
			lara.water_status = LW_FLYCHEAT;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 5460;
			lara_item->fallspeed = 30;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
		}
	}

	if (keymap[DIK_F3])
	{
		gfLevelComplete = gfCurrentLevel + 1;
		SCNoDrawLara = 0;
		bDisableLaraControl = 0;
	}
#endif
}

void AnimateLara(ITEM_INFO* item)
{
	ANIM_STRUCT* anim;
	short* cmd;
	long speed, speed2;

	item->frame_number++;
	anim = &anims[item->anim_number];
	
	if (anim->number_changes > 0 && GetChange(item, anim))
	{
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands > 0)
		{
			cmd = &commands[anim->command_index];

			for (int i = anim->number_commands; i > 0; i--)
			{
				switch (*cmd++)
				{
				case 1:
					TranslateItem(item, *cmd, cmd[1], cmd[2]);
					UpdateLaraRoom(item, -381);
					cmd += 3;
					break;

				case 2:
					item->fallspeed = *cmd++;
					item->speed = *cmd++;
					item->gravity_status = 1;

					if (lara.calc_fallspeed)
					{
						item->fallspeed = lara.calc_fallspeed;
						lara.calc_fallspeed = 0;
					}

					break;

				case 3:

					if (lara.gun_status != LG_FLARE)
						lara.gun_status = LG_NO_ARMS;

					break;

				case 5:
				case 6:
					cmd += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
	}

	if (anim->number_commands > 0)
	{
		cmd = &commands[anim->command_index];

		for (int i = anim->number_commands; i > 0; i--)
		{
			switch (*cmd++)
			{
			case 1:
				cmd += 3;
				break;

			case 2:
				cmd += 2;
				break;

			case 5:

				if (item->frame_number == *cmd)
				{
					if ((cmd[1] & 0xC000) == SFX_LANDANDWATER ||
						((cmd[1] & 0xC000) == SFX_LANDONLY && (lara.water_surface_dist >= 0 || lara.water_surface_dist == NO_HEIGHT)) ||
						((cmd[1] & 0xC000) == SFX_WATERONLY && lara.water_surface_dist < 0 && lara.water_surface_dist != NO_HEIGHT))
						SoundEffect(cmd[1] & 0x3FFF, &item->pos, SFX_ALWAYS);
				}

				cmd += 2;
				break;

			case 6:

				if (item->frame_number == *cmd)
				{
					FXType = cmd[1] & 0xC000;
					effect_routines[cmd[1] & 0x3FFF](item);
				}

				cmd += 2;
				break;
			}
		}
	}

	speed2 = anim->Xvelocity;

	if (anim->Xacceleration)
		speed2 += anim->Xacceleration * (item->frame_number - anim->frame_base);

	speed2 >>= 16;

	if (item->gravity_status)
	{
		speed = anim->velocity + anim->acceleration * (item->frame_number - anim->frame_base - 1);
		item->speed -= speed >> 16;
		speed += anim->acceleration;
		item->speed += speed >> 16;
		item->fallspeed += (item->fallspeed >= 128) ? 1 : 6;
		item->pos.y_pos += item->fallspeed;
	}
	else
	{
		speed = anim->velocity;

		if (anim->acceleration)
			speed += anim->acceleration * (item->frame_number - anim->frame_base);

		item->speed = speed >> 16;
	}

	if (lara.RopePtr != -1)
		AlignLaraToRope(item);

	if (!lara.IsMoving)
	{
		item->pos.x_pos += item->speed * phd_sin(lara.move_angle) >> W2V_SHIFT;
		item->pos.z_pos += item->speed * phd_cos(lara.move_angle) >> W2V_SHIFT;
		item->pos.x_pos += speed2 * phd_sin(lara.move_angle + 0x4000) >> W2V_SHIFT;
		item->pos.z_pos += speed2 * phd_cos(lara.move_angle + 0x4000) >> W2V_SHIFT;
	}
}

void LaraControl(short item_number)
{
	ITEM_INFO* item;
	long oldx, oldy, oldz;
	long wh, wd, hfw, water;
	short room_number;
	static short SubsuitAir = 0;

	item = lara_item;

	if (lara.IsMoving && lara.MoveCount++ > 90)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

	if (tomb5.enable_cheats)
		LaraCheatyBits();

	if (!bDisableLaraControl)
		lara.locationPad = -128;

	oldx = lara_item->pos.x_pos;
	oldy = lara_item->pos.y_pos;
	oldz = lara_item->pos.z_pos;

	if (lara.gun_status == LG_HANDS_BUSY && item->current_anim_state == AS_STOP && item->goal_anim_state == AS_STOP
		&& item->anim_number == ANIM_BREATH && !item->gravity_status)
		lara.gun_status = LG_NO_ARMS;

	if (item->current_anim_state != AS_DASH && DashTimer < 120)
		DashTimer++;

	lara.IsDucked = 0;
	water = room[item->room_number].flags & ROOM_UNDERWATER;
	wd = GetWaterDepth(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (wh != NO_HEIGHT)
		hfw = item->pos.y_pos - wh;
	else
		hfw = NO_HEIGHT;

	lara.water_surface_dist = -hfw;
	WadeSplash(item, wh, wd);

	switch (lara.water_status)
	{
	case LW_ABOVE_WATER:

		if (hfw != NO_HEIGHT && hfw >= 265)
		{
			if (wd <= 474)
			{
				if (hfw > 256)
				{
					lara.water_status = LW_WADE;

					if (!item->gravity_status)
						item->goal_anim_state = AS_STOP;
				}
			}
			else if (water)
			{
				lara.air = 1800;
				lara.water_status = LW_UNDERWATER;
				item->gravity_status = 0;
				item->pos.y_pos += 100;
				UpdateLaraRoom(item, 0);
				StopSoundEffect(SFX_LARA_FALL);

				if (item->current_anim_state == AS_SWANDIVE)
				{
					item->pos.x_rot = -8190;
					item->goal_anim_state = AS_DIVE;
					AnimateLara(item);
					item->fallspeed *= 2;
				}
				else if (item->current_anim_state == AS_FASTDIVE)
				{
					item->pos.x_rot = -15470;
					item->goal_anim_state = AS_DIVE;
					AnimateLara(item);
					item->fallspeed *= 2;
				}
				else
				{
					item->pos.x_rot = -8190;
					item->anim_number = 112;
					item->frame_number = anims[112].frame_base;
					item->current_anim_state = AS_DIVE;
					item->goal_anim_state = AS_SWIM;
					item->fallspeed = 3 * item->fallspeed / 2;
				}

				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				Splash(lara_item);
			}
		}
		else
			break;

	case LW_WADE:
		camera.target_elevation = -4004;

		if (hfw <= 256)
		{
			lara.water_status = LW_ABOVE_WATER;

			if (item->current_anim_state == AS_WADE)
				item->goal_anim_state = 1;
		}
		else if (hfw > 730)
		{
			lara.water_status = LW_SURFACE;
			item->pos.y_pos += 1 - hfw;

			switch (item->current_anim_state)
			{
			case AS_BACK:
				item->current_anim_state = AS_SURFBACK;
				item->goal_anim_state = AS_SURFBACK;
				item->anim_number = 140;
				item->frame_number = anims[140].frame_base;
				break;

			case AS_STEPRIGHT:
				item->current_anim_state = AS_SURFRIGHT;
				item->goal_anim_state = AS_SURFRIGHT;
				item->anim_number = 144;
				item->frame_number = anims[144].frame_base;
				break;

			case AS_STEPLEFT:
				item->current_anim_state = AS_SURFLEFT;
				item->goal_anim_state = AS_SURFLEFT;
				item->anim_number = 143;
				item->frame_number = anims[143].frame_base;
				break;

			default:
				item->current_anim_state = AS_SURFSWIM;
				item->goal_anim_state = AS_SURFSWIM;
				item->anim_number = 116;
				item->frame_number = anims[116].frame_base;
				break;
			}

			item->gravity_status = 0;
			item->fallspeed = 0;
			lara.dive_count = 0;
			item->pos.x_rot = 0;
			item->pos.z_rot = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
			UpdateLaraRoom(item, 0);
		}

		break;

	case LW_UNDERWATER:
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos, &room_number);

		if (wd != NO_HEIGHT && abs(hfw) < 256 && !(room[room_number].flags & ROOM_UNDERWATER) && item->anim_number != 114 && item->anim_number != 119)
		{
			lara.water_status = LW_SURFACE;
			item->pos.y_pos = wh + 1;
			item->anim_number = 114;
			item->frame_number = anims[114].frame_base;
			item->current_anim_state = AS_SURFTREAD;
			item->goal_anim_state = AS_SURFTREAD;
			item->pos.x_rot = 0;
			item->pos.z_rot = 0;
			item->fallspeed = 0;
			lara.dive_count = 11;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
			UpdateLaraRoom(item, 0);
			SoundEffect(SFX_LARA_BREATH, &lara_item->pos, SFX_ALWAYS);
		}
		else if (!water)
		{
			if (wd != NO_HEIGHT && abs(hfw) < 256)
			{
				lara.water_status = LW_SURFACE;
				item->pos.y_pos = wh;
				item->anim_number = 114;
				item->frame_number = anims[114].frame_base;
				item->current_anim_state = AS_SURFTREAD;
				item->goal_anim_state = AS_SURFTREAD;
				item->pos.x_rot = 0;
				item->pos.z_rot = 0;
				item->fallspeed = 0;
				lara.dive_count = 11;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				UpdateLaraRoom(item, -381);
				SoundEffect(SFX_LARA_BREATH, &item->pos, SFX_ALWAYS);
			}
			else
			{
				lara.water_status = LW_ABOVE_WATER;
				item->anim_number = ANIM_FALLDOWN;
				item->frame_number = anims[ANIM_FALLDOWN].frame_base;
				item->current_anim_state = AS_FORWARDJUMP;
				item->goal_anim_state = AS_FORWARDJUMP;
				item->pos.x_rot = 0;
				item->pos.z_rot = 0;
				item->speed = item->fallspeed / 4;
				item->gravity_status = 1;
				item->fallspeed = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
			}
		}

		break;

	case LW_SURFACE:

		if (!water)
		{
			if (hfw <= 256)
			{
				lara.water_status = LW_ABOVE_WATER;
				item->anim_number = ANIM_FALLDOWN;
				item->frame_number = anims[ANIM_FALLDOWN].frame_base;
				item->goal_anim_state = AS_FORWARDJUMP;
				item->current_anim_state = AS_FORWARDJUMP;
				item->speed = item->fallspeed / 4;
				item->gravity_status = 1;			
			}
			else
			{
				lara.water_status = LW_WADE;
				item->anim_number = ANIM_BREATH;
				item->frame_number = anims[ANIM_BREATH].frame_base;
				item->current_anim_state = AS_STOP;
				item->goal_anim_state = AS_WADE;
				AnimateItem(item);
			}

			item->fallspeed = 0;
			item->pos.x_rot = 0;
			item->pos.z_rot = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
		}

		break;
	}

//	S_SetReverbType(room[item->room_number].ReverbType);

	if (item->hit_points <= 0)
	{
		item->hit_points = -1;

		if (!lara.death_count)
			S_CDStop();

		lara.death_count++;

		if (lara_item->flags & IFL_INVISIBLE)
		{
			lara.death_count++;
			return;
		}
	}

	switch (lara.water_status)
	{
	case LW_ABOVE_WATER:
	case LW_WADE:

		if (lara.Gassed)
		{
			if (item->hit_points >= 0)
			{
				lara.air--;

				if (lara.air < 0)
				{
					lara.air = -1;
					item->hit_points -= 5;
				}
			}
		}
		else if (lara.air < 1800 && item->hit_points >= 0)
		{
			lara.air += 10;

			if (lara.air > 1800)
				lara.air = 1800;
		}

		LaraAboveWater(item, lara_coll);
		break;

	case LW_UNDERWATER:

		if (item->hit_points >= 0)
		{
			if (LaraDrawType == LARA_DIVESUIT)
			{
				if (CheckCutPlayed(40))
				{
					SubsuitAir += lara.Anxiety + 8;

					while (SubsuitAir > 80)
					{
						lara.air--;
						SubsuitAir -= 80;
					}
				}
			}
			else
				lara.air--;

			if (lara.air < 0)
			{
				if (LaraDrawType == LARA_DIVESUIT && lara.Anxiety < 251)
					lara.Anxiety += 4;

				lara.air = -1;
				item->hit_points -= 5;
			}
		}

		LaraUnderWater(item, lara_coll);
		break;

	case LW_SURFACE:

		if (item->hit_points >= 0)
		{
			lara.air += 10;

			if (lara.air > 1800)
				lara.air = 1800;
		}

		LaraSurface(item, lara_coll);
		break;

	case LW_FLYCHEAT:
		LaraCheat(item, lara_coll);
		break;
	}

	savegame.Game.Distance += phd_sqrt(SQUARE(item->pos.x_pos - oldx) + SQUARE(item->pos.y_pos - oldy) + SQUARE(item->pos.z_pos - oldz));
}
