#include "../tomb5/pch.h"
#include "laramisc.h"
#include "lara_states.h"
#include "gameflow.h"
#include "newinv2.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "control.h"
#include "collide.h"
#include "effects.h"
#include "sound.h"
#include "laraswim.h"
#include "objects.h"
#include "rope.h"
#include "../specific/specific.h"
#include "lara.h"
#include "larasurf.h"

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
		lara.water_status = LW_UNDERWATER;
		item->fallspeed = 0;
		item->goal_anim_state = AS_TREAD;
		item->current_anim_state = AS_TREAD;
		item->anim_number = ANIM_TREAD;
		item->frame_number = anims[ANIM_TREAD].frame_base;
	}
	else
	{
		lara.water_status = LW_ABOVE_WATER;
		item->goal_anim_state = AS_STOP;
		item->current_anim_state = AS_STOP;
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
	}
}

void InitialiseLaraLoad(short item_num)
{
	lara.item_number = item_num;
	lara_item = &items[item_num];
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
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
		lara_item->hit_points = cheat_hit_points;
	}
}

void LaraInitialiseMeshes()
{
	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + (2 * i)] = meshes[objects[LARA_SKIN].mesh_index + (2 * i)];

	if (gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL)
		lara.mesh_ptrs[LM_TORSO] = meshes[objects[ANIMATING6_MIP].mesh_index + (2 * LM_TORSO)];

	if (lara.gun_type != WEAPON_HK)
	{
		if (lara.shotgun_type_carried)
		{
			lara.gun_status = LG_NO_ARMS;
			lara.back_gun = WEAPON_UZI;
			lara.target = 0;
			lara.left_arm.frame_number = 0;
			lara.left_arm.lock = 0;
			lara.right_arm.frame_number = 0;
			lara.right_arm.lock = 0;
			return;
		}
		if (!lara.hk_type_carried)
		{
			lara.gun_status = LG_NO_ARMS;
			lara.target = 0;
			lara.left_arm.frame_number = 0;
			lara.left_arm.lock = 0;
			lara.right_arm.frame_number = 0;
			lara.right_arm.lock = 0;
			return;
		}
	}

	lara.gun_status = LG_NO_ARMS;
	lara.back_gun = WEAPON_HK;
	lara.target = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_number = 0;
	lara.right_arm.lock = 0;
}

void InitialiseLara(int restore)
{
	short item_num, gun;

	if (lara.item_number == NO_ITEM)
		return;

	item_num = lara.item_number;
	lara_item->data = &lara;
	lara_item->collidable = 0;

	if (!restore)
	{
		memset(&lara, 0, 0x154u);
		lara.TightRopeOff = 0;
		lara.TightRopeFall = 0;
		lara.ChaffTimer = 0;
	}
	else
	{
		lara_info backup;
		memcpy(&backup, &lara, sizeof(lara));
		memset(&lara, 0, sizeof(lara));
		memcpy(&lara.pistols_type_carried, &backup.pistols_type_carried, 0x38u);
		lara.num_crossbow_ammo2 = backup.num_crossbow_ammo2;
	}

	lara.look = 1;
	lara.item_number = item_num;
	lara.hit_direction = 0;
	lara.air = 1800;
	lara.weapon_item = NO_ITEM;
	PoisonFlag = 0;
	lara.dpoisoned = 0;
	lara.poisoned = 0;
	lara.water_surface_dist = 100;
	lara.holster = 14;
	lara.location = -1;
	lara.highest_location = -1;
	lara.RopePtr = NO_ITEM;
	lara_item->hit_points = 1000;

	if (gfNumPickups > 0)
		for (int i = 0; i < gfNumPickups; i++)
			DEL_picked_up_object(convert_invobj_to_obj(gfPickups[i]));

	gfNumPickups = 0;

	if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA) && (objects[PISTOLS_ITEM].loaded))
		gun = WEAPON_PISTOLS;
	else
		gun = WEAPON_NONE;

	if ((gfLevelFlags & GF_LVOP_TRAIN) && (objects[HK_ITEM].loaded) && (lara.hk_type_carried & WTYPE_PRESENT))
		gun = WEAPON_HK;

	lara.gun_status = LG_NO_ARMS;
	lara.last_gun_type = gun;
	lara.gun_type = gun;
	lara.request_gun_type = gun;
	LaraInitialiseMeshes();
	lara.skelebob = 0;

	if (objects[PISTOLS_ITEM].loaded)
		lara.pistols_type_carried = 9;

	lara.binoculars = 1;

	if (!restore)
	{
		if (objects[FLARE_INV_ITEM].loaded)
			lara.num_flares = 3;
		lara.num_small_medipack = 3;
		lara.num_large_medipack = 1;
	}

	lara.num_pistols_ammo = -1;
	InitialiseLaraAnims(lara_item);
	DashTimer = 120;

	if (gfNumTakeaways > 0)
		for (int i = 0; i < gfNumTakeaways; i++)
			NailInvItem(convert_invobj_to_obj(gfTakeaways[i]));

	gfNumTakeaways = 0;
	weapons[WEAPON_REVOLVER].damage = gfCurrentLevel > LVL5_COLOSSEUM ? 15 : 6;

	switch (gfCurrentLevel)
	{
	case LVL5_DEEPSEA_DIVE:
		lara.pickupitems &= 0xFFF7;
		lara.puzzleitems[0] = 10;
		return;

	case LVL5_SUBMARINE:
		memset(&lara.puzzleitems, 0, 12);
		lara.puzzleitemscombo = 0;
		lara.pickupitems = 0;
		lara.pickupitemscombo = 0;
		lara.keyitems = 0;
		lara.keyitemscombo = 0;
		return;

	case LVL5_SINKING_SUBMARINE:
		lara.puzzleitems[0] = 0;
		lara.pickupitems = 0;
		return;

	case LVL5_ESCAPE_WITH_THE_IRIS:
		lara.pickupitems &= 0xFFFEu;
		lara.puzzleitems[2] = 0;
		lara.puzzleitems[3] = 0;
		break;

	case LVL5_RED_ALERT:
		lara.pickupitems &= 0xFFFDu;
		break;

	default:
		if (gfCurrentLevel < 0xB || gfCurrentLevel > 0xE)
			lara.pickupitems &= 0xFFF7u;
		break;
	}

	lara.bottle = 0;
	lara.wetcloth = CLOTH_MISSING;
}

void LaraCheatGetStuff()
{
	if (objects[CROWBAR_ITEM].loaded)
		lara.crowbar = 1;

#ifdef BETTER_CHEATS
	if (objects[FLARE_INV_ITEM].loaded)
		lara.num_flares = -1;

	if (objects[SMALLMEDI_ITEM].loaded)
		lara.num_small_medipack = -1;

	if (objects[BIGMEDI_ITEM].loaded)
		lara.num_large_medipack = -1;

	if (objects[LASERSIGHT_ITEM].loaded)
		lara.lasersight = 1;

	if (objects[UZI_ITEM].loaded)
	{
		lara.uzis_type_carried = 9;
		lara.num_uzi_ammo = -1;
	}

	if (objects[SHOTGUN_ITEM].loaded)
	{
		lara.shotgun_type_carried = 9;
		lara.num_shotgun_ammo1 = -1;
		lara.num_shotgun_ammo2 = -1;
	}

	if (objects[REVOLVER_ITEM].loaded)
	{
		lara.sixshooter_type_carried = 9;
		lara.num_revolver_ammo = -1;
	}
	
	if (objects[HK_ITEM].loaded)
	{
		lara.silencer = 1;
		lara.hk_type_carried = 1;
		lara.num_hk_ammo1 = -1;
	}

	if (objects[CROSSBOW_ITEM].loaded)
	{
		lara.crossbow_type_carried = 1;
		lara.num_crossbow_ammo1 = -1;
		lara.num_crossbow_ammo2 = 0;
	}
#else
	lara.num_flares = -1;
	lara.num_small_medipack = -1;
	lara.num_large_medipack = -1;
	lara.lasersight = 1;
	lara.uzis_type_carried = 9;
	lara.shotgun_type_carried = 9;
	lara.sixshooter_type_carried = 9;
	lara.num_uzi_ammo = -1;
	lara.num_revolver_ammo = -1;
	lara.num_shotgun_ammo1 = -1;
#endif
}

#ifdef VER_JP
void LaraCheatyBits()
{
#ifndef BETTER_CHEATS
	if (!Gameflow->CheatEnabled)
		return;

	if (input & IN_D)
#else
	if (keymap[59])//F1
#endif
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

#ifndef BETTER_CHEATS
	if (input & IN_CHEAT)
#else
	if (keymap[60])//F2
#endif
	{
		dels_give_lara_items_cheat();
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara.water_status = LW_FLYCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->current_anim_state = AS_SWIMCHEAT;
			lara_item->goal_anim_state = AS_SWIMCHEAT;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 5460;
			lara_item->fallspeed = 30;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			cheat_hit_points = lara_item->hit_points;
		}
	}

#ifdef BETTER_CHEATS
	if (keymap[61])//F3
		gfLevelComplete = gfCurrentLevel + 1;
#endif

}
#endif

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
		item->pos.x_pos += (phd_sin(lara.move_angle) * item->speed) >> 14;
		item->pos.z_pos += (phd_cos(lara.move_angle) * item->speed) >> 14;
		item->pos.x_pos += (phd_sin(lara.move_angle + 16384) * speed2) >> 14;
		item->pos.z_pos += (phd_cos(lara.move_angle + 16384) * speed2) >> 14;
	}
}

void LaraControl(short item_number)
{
	ITEM_INFO* item;
	long oldx, oldy, oldz;
	int wh, wd ,hfw, room_water_state;
	short room_number;

	item = lara_item;

	if (lara.IsMoving && lara.MoveCount++ > 90)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

#ifdef VER_JP
	LaraCheatyBits();
#endif

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
	room_water_state = room[item->room_number].flags & ROOM_UNDERWATER;
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
			else if (room_water_state)
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

		if (hfw >= 256)
		{
			if (hfw > 730)
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
		}
		else
		{
			lara.water_status = LW_ABOVE_WATER;

			if (item->current_anim_state == AS_WADE)
				item->goal_anim_state = 1;
		}

		break;

	case LW_UNDERWATER:
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos, &room_number);

		if (wd == NO_HEIGHT || ABS(hfw) >= 256 || room[room_number].flags & ROOM_UNDERWATER || (item->anim_number == 114 || item->anim_number == 119))
		{
			if (!room_water_state)
			{
				if (wd != NO_HEIGHT && ABS(hfw) < 256)
				{
					lara.water_status = LW_SURFACE;
					item->pos.y_pos = wh;
					item->anim_number = 114;
					item->frame_number = anims[114].frame_base;
					item->goal_anim_state = AS_SURFTREAD;
					item->current_anim_state = AS_SURFTREAD;
					item->fallspeed = 0;
					lara.dive_count = 11;
					item->pos.x_rot = 0;
					item->pos.z_rot = 0;
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
					item->goal_anim_state = AS_FORWARDJUMP;
					item->current_anim_state = AS_FORWARDJUMP;
					item->speed = item->fallspeed / 4;
					item->gravity_status = 1;
					item->fallspeed = 0;
					item->pos.x_rot = 0;
					item->pos.z_rot = 0;
					lara.torso_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.head_x_rot = 0;
					lara.head_y_rot = 0;
				}
			}
		}
		else
		{
			lara.water_status = LW_SURFACE;
			item->pos.y_pos = wh + 1;
			item->anim_number = 114;
			item->frame_number = anims[114].frame_base;
			item->goal_anim_state = AS_SURFTREAD;
			item->current_anim_state = AS_SURFTREAD;
			item->fallspeed = 0;
			lara.dive_count = 11;
			item->pos.x_rot = 0;
			item->pos.z_rot = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
			UpdateLaraRoom(item, 0);
			SoundEffect(SFX_LARA_BREATH, &lara_item->pos, SFX_ALWAYS);
		}

		break;

	case LW_SURFACE:

		if (!room_water_state)
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

	S_SetReverbType(room[item->room_number].ReverbType);

	if (item->hit_points <= 0)
	{
		item->hit_points = -1;

		if (!lara.death_count)
			S_CDStop();

		lara.death_count++;

		if (lara_item->flags & 0x100)
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

		LaraAboveWater(item, &lara_coll);
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

		LaraUnderWater(item, &lara_coll);
		break;

	case LW_SURFACE:

		if (item->hit_points >= 0)
		{
			lara.air += 10;

			if (lara.air > 1800)
				lara.air = 1800;
		}

		LaraSurface(item, &lara_coll);
		break;

	case LW_FLYCHEAT:
		LaraCheat(item, &lara_coll);
		break;
	}

	savegame.Game.Distance += phd_sqrt(SQUARE(item->pos.x_pos - oldx) + SQUARE(item->pos.y_pos - oldy) + SQUARE(item->pos.z_pos - oldz));
	return;
}

void inject_laramisc()
{
	INJECT(0x004569C0, GetLaraDeadlyBounds);
	INJECT(0x00456900, InitialiseLaraAnims);
	INJECT(0x004568C0, InitialiseLaraLoad);
	INJECT(0x00456320, LaraCheat);
	INJECT(0x00455680, LaraInitialiseMeshes);
	INJECT(0x00473210, InitialiseLara);
	INJECT(0x004557B0, LaraCheatGetStuff);
	INJECT(0x004563F0, AnimateLara);
	INJECT(0x00455830, LaraControl);
}
