#include "../tomb5/pch.h"
#include "larafire.h"
#include "flmtorch.h"
#include "gameflow.h"
#include "lara_states.h"
#include "laraflar.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "control.h"
#include "tomb4fx.h"
#include "effects.h"
#include "sound.h"
#include "items.h"
#include "draw.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "sphere.h"
#include "objects.h"

PHD_3DPOS bum_view;
GAME_VECTOR bum_vdest;
GAME_VECTOR bum_vsrc;
ITEM_INFO* TargetList[8];
ITEM_INFO* LastTargets[8];

static short HoldStates[] =
{
	AS_WALK,
	AS_RUN,
	AS_STOP,
	AS_POSE,
	AS_TURN_R,
	AS_TURN_L, 
	AS_BACK, 
	AS_FASTTURN, 
	AS_STEPLEFT, 
	AS_STEPRIGHT,
	AS_WADE,
	AS_PICKUP,
	AS_SWITCHON,
	AS_SWITCHOFF,
	AS_DUCK,
	AS_DUCKROTL,
	AS_DUCKROTR,
	-1
};

void LaraGun()
{
	if (lara.left_arm.flash_gun > 0)
		lara.left_arm.flash_gun--;

	if (lara.right_arm.flash_gun > 0)
		lara.right_arm.flash_gun--;

	if (lara.gun_type == WEAPON_TORCH)
	{
		DoFlameTorch();
		return;
	}

	if (lara_item->hit_points <= 0)
		lara.gun_status = LG_NO_ARMS;
	else if (lara.gun_status == LG_NO_ARMS)
	{
		if (input & IN_DRAW)
			lara.request_gun_type = lara.last_gun_type;
		else if (input & IN_FLARE)
		{
			if (!(gfLevelFlags & GF_YOUNGLARA))
			{
				if (lara_item->current_anim_state == AS_DUCK && lara_item->anim_number != ANIM_DUCKBREATHE)
					return;

				if (lara.gun_type == WEAPON_FLARE)
				{
					if (!lara.left_arm.frame_number)
						lara.gun_status = LG_UNDRAW_GUNS;
				}
				else if (lara.num_flares)
				{
					if (lara.num_flares != -1)
						lara.num_flares--;

					lara.request_gun_type = WEAPON_FLARE;
				}
			}
		}

		if ((input & IN_DRAW) || lara.request_gun_type != lara.gun_type)
		{
			if ((lara_item->current_anim_state == AS_DUCK ||
				lara_item->current_anim_state == AS_DUCKROTL ||
				lara_item->current_anim_state == AS_DUCKROTR) &&
				(lara.request_gun_type == WEAPON_SHOTGUN || lara.request_gun_type == WEAPON_HK || lara.request_gun_type == WEAPON_CROSSBOW))
			{
				if (lara.gun_type == WEAPON_FLARE)
					lara.request_gun_type = WEAPON_FLARE;
			}
			else if (lara.request_gun_type == WEAPON_FLARE || lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE
				&& lara.water_surface_dist > -weapons[lara.gun_type].gun_height)
			{
				if (lara.gun_type == WEAPON_FLARE)
				{
					CreateFlare(FLARE_ITEM, 0);
					undraw_flare_meshes();
					lara.flare_control_left = 0;
					lara.flare_age = 0;
				}

				lara.gun_type = lara.request_gun_type;
				InitialiseNewWeapon();
				lara.gun_status = LG_DRAW_GUNS;
				lara.right_arm.frame_number = 0;
				lara.left_arm.frame_number = 0;
			}
			else
			{
				lara.last_gun_type = lara.request_gun_type;

				if (lara.gun_type == WEAPON_FLARE)
					lara.request_gun_type = WEAPON_FLARE;
				else
					lara.gun_type = lara.request_gun_type;
			}
		}
	}
	else if (lara.gun_status == LG_READY)
	{
		if (input & IN_DRAW || lara.request_gun_type != lara.gun_type)
			lara.gun_status = LG_UNDRAW_GUNS;
		else if (lara.water_status != LW_ABOVE_WATER &&
			(lara.water_status != LW_WADE || lara.water_surface_dist < -weapons[lara.gun_type].gun_height))
			lara.gun_status = LG_UNDRAW_GUNS;
	}
	else if (lara.gun_status == LG_HANDS_BUSY && input & IN_FLARE && lara_item->current_anim_state == AS_ALL4S && lara_item->anim_number == ANIM_ALL4S)
		lara.request_gun_type = WEAPON_FLARE;

	switch (lara.gun_status)
	{
	case LG_NO_ARMS:
		if (lara.gun_type == WEAPON_FLARE)
		{
			if (CheckForHoldingState(lara_item->current_anim_state))
			{
				if (!lara.flare_control_left)
				{
					lara.left_arm.frame_number = 95;
					lara.flare_control_left = 1;
				}
				else
				{
					if (lara.left_arm.frame_number)
					{
						lara.left_arm.frame_number++;

						if (lara.left_arm.frame_number == 110)
							lara.left_arm.frame_number = 0;
					}
				}
			}
			else
				lara.flare_control_left = 0;

			DoFlareInHand(lara.flare_age);
			set_flare_arm(lara.left_arm.frame_number);
		}

		break;

	case LG_HANDS_BUSY:

		if (lara.gun_type == WEAPON_FLARE)
		{
			if (lara.mesh_ptrs[LM_LHAND] == meshes[objects[FLARE_ANIM].mesh_index + (2 * LM_LHAND)])
			{
				lara.flare_control_left = CheckForHoldingState(lara_item->current_anim_state);
				DoFlareInHand(lara.flare_age);
				set_flare_arm(lara.left_arm.frame_number);
			}
		}

		break;

	case LG_DRAW_GUNS:
		if (lara.gun_type != WEAPON_FLARE && lara.gun_type != WEAPON_NONE)
			lara.last_gun_type = lara.gun_type;

		switch (lara.gun_type)
		{
		case WEAPON_PISTOLS:
		case WEAPON_REVOLVER:
		case WEAPON_UZI:

			if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
				camera.type = COMBAT_CAMERA;

			draw_pistols(lara.gun_type);
			break;

		case WEAPON_SHOTGUN:
		case WEAPON_HK:
		case WEAPON_CROSSBOW:

			if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
				camera.type = COMBAT_CAMERA;

			draw_shotgun(lara.gun_type);
			break;

		case WEAPON_FLARE:
			draw_flare();
			break;

		default:
			lara.gun_status = LG_NO_ARMS;
			break;
		}

		break;

	case LG_UNDRAW_GUNS:
		lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA].mesh_index + (2 * LM_HEAD)];

		switch (lara.gun_type)
		{
		case WEAPON_PISTOLS:
		case WEAPON_REVOLVER:
		case WEAPON_UZI:
			undraw_pistols(lara.gun_type);
			break;

		case WEAPON_SHOTGUN:
		case WEAPON_HK:
		case WEAPON_CROSSBOW:
			undraw_shotgun(lara.gun_type);
			break;

		case WEAPON_FLARE:
			undraw_flare();
			break;
		}

		break;

	case LG_READY:

		if (input & IN_ACTION)
			lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA_SCREAM].mesh_index + (2 * LM_HEAD)];
		else
			lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA].mesh_index + (2 * LM_HEAD)];

		if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
			camera.type = COMBAT_CAMERA;

		if (input & IN_ACTION && !*get_current_ammo_pointer(lara.gun_type))
		{
			if (objects[PISTOLS_ITEM].loaded)
				lara.request_gun_type = WEAPON_PISTOLS;
			else
				lara.request_gun_type = WEAPON_NONE;
		}
		else
		{
			switch (lara.gun_type)
			{
			case WEAPON_PISTOLS:
			case WEAPON_UZI:
				PistolHandler(lara.gun_type);
				break;

			case WEAPON_REVOLVER:
			case WEAPON_SHOTGUN:
			case WEAPON_HK:
			case WEAPON_CROSSBOW:
				RifleHandler(lara.gun_type);
				break;
			}
		}

		break;

	case LG_FLARE:
		draw_flare();
		break;
	}
}

static int CheckForHoldingState(int state)
{
	short* holds;

	holds = HoldStates;

	while (*holds >= 0)
	{
		if (state == *holds)
			return 1;

		holds++;
	}

	return 0;
}

void InitialiseNewWeapon()
{
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.z_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.x_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.flash_gun = 0;
	lara.left_arm.flash_gun = 0;

	switch (lara.gun_type)
	{
	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		lara.left_arm.frame_base = objects[PISTOLS_ANIM].frame_base; 
		lara.right_arm.frame_base = objects[PISTOLS_ANIM].frame_base;
		if (lara.gun_status)
			draw_pistol_meshes(lara.gun_type);
		break;

	case WEAPON_REVOLVER:
	case WEAPON_SHOTGUN:
	case WEAPON_HK:
		lara.left_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base; 
		lara.right_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base;
		if (lara.gun_status)
			draw_shotgun_meshes(lara.gun_type);
		break;

	case WEAPON_FLARE:
		lara.left_arm.frame_base = objects[FLARE_ANIM].frame_base; 
		lara.right_arm.frame_base = objects[FLARE_ANIM].frame_base;
		if (lara.gun_status)
			draw_flare_meshes();
		break;

	default:
		lara.left_arm.frame_base = anims[lara_item->anim_number].frame_ptr; 
		lara.right_arm.frame_base = anims[lara_item->anim_number].frame_ptr;
		break;
	}
}

void LaraTargetInfo(WEAPON_INFO* winfo)
{
//	ITEM_INFO* item;//unused wtf
	short ang[2];
	GAME_VECTOR src, target;


	if (!lara.target)
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		lara.target_angles[1] = 0;
		lara.target_angles[0] = 0;
		return;
	}

	src.x = 0;
	src.y = 0;
	src.z = 0;
	GetLaraJointPos((PHD_VECTOR*)&src, 11);
	src.x = lara_item->pos.x_pos;
	src.z = lara_item->pos.z_pos;
	src.room_number = lara_item->room_number;
	find_target_point(lara.target, &target);
	phd_GetVectorAngles(target.x - src.x, target.y - src.y, target.z - src.z, ang);
	ang[0] -= lara_item->pos.y_rot;
	ang[1] -= lara_item->pos.x_rot;

	if (LOS(&src, &target))
	{
		if (
			ang[0] >= winfo->lock_angles[0] &&
			ang[0] <= winfo->lock_angles[1] &&
			ang[1] >= winfo->lock_angles[2] &&
			ang[1] <= winfo->lock_angles[3]
			)
		{
			lara.left_arm.lock = 1;
			lara.right_arm.lock = 1;
			lara.target_angles[0] = ang[0];
			lara.target_angles[1] = ang[1];
			return;
		}

		if (lara.left_arm.lock)
			if (ang[0] < winfo->left_angles[0] ||
				ang[0] > winfo->left_angles[1] ||
				ang[1] < winfo->left_angles[2] ||
				ang[1] > winfo->left_angles[3])
				lara.left_arm.lock = 0;

		if (lara.right_arm.lock)
			if (ang[0] < winfo->right_angles[0] ||
				ang[0] > winfo->right_angles[1] ||
				ang[1] < winfo->left_angles[2] ||
				ang[1] > winfo->left_angles[3])
			{
				lara.right_arm.lock = 0;
				lara.target_angles[0] = ang[0];
				lara.target_angles[1] = ang[1];
				return;
			}
	}
	else
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
	}

	lara.target_angles[0] = ang[0];
	lara.target_angles[1] = ang[1];
}

void LaraGetNewTarget(WEAPON_INFO* winfo)
{
	ITEM_INFO* item;
	ITEM_INFO* bestitem;
	short ang[2];
	short bestyrot;
	GAME_VECTOR src, target;
	creature_info* creature;
	int x, y, z, slot, dist, maxdist, maxdist2, bestdist;
	short targets, match;

	if (BinocularRange)
	{
		lara.target = 0;
		return;
	}

	bestitem = 0;
	src.x = lara_item->pos.x_pos;
	src.y = lara_item->pos.y_pos - 650;
	src.z = lara_item->pos.z_pos;
	src.room_number = lara_item->room_number;
	bestyrot = 0x7FFF;
	bestdist = 0x7FFFFFFF;
	maxdist = winfo->target_dist;
	maxdist2 = SQUARE(maxdist);
	creature = baddie_slots;
	targets = 0;

	for (slot = 0; slot < 5; slot++, creature++)
	{
		if (creature->item_num != NO_ITEM)
		{
			item = &items[creature->item_num];

			if (item->hit_points > 0)
			{
				x = item->pos.x_pos - src.x;
				y = item->pos.y_pos - src.y;
				z = item->pos.z_pos - src.z;

				if ((ABS(x)) <= maxdist && (ABS(y)) <= maxdist && (ABS(z)) <= maxdist)
				{
					dist = x * x + y * y + z * z;
					if (dist < maxdist2)
					{
						find_target_point(item, &target);

						if (LOS(&src, &target))
						{
							phd_GetVectorAngles(target.x - src.x, target.y - src.y, target.z - src.z, ang);
							ang[0] -= (lara.torso_y_rot + lara_item->pos.y_rot);
							ang[1] -= (lara.torso_x_rot + lara_item->pos.x_rot);

							if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] &&
								ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3])
							{
								TargetList[targets] = item;
								targets++;

								if (ABS(ang[0]) < bestyrot + 2730 && dist < bestdist)
								{
									bestdist = dist;
									bestyrot = ABS(ang[0]);
									bestitem = item;
								}
							}
						}
					}
				}
			}
		}
	}

	TargetList[targets] = 0;

	if (TargetList[0])
	{
		for (slot = 0; slot < 8; ++slot)
		{
			if (!TargetList[slot])
				lara.target = NULL;

			if (TargetList[slot] == lara.target)
				break;
		}

		if (savegame.AutoTarget || input & IN_TARGET)
		{
			if (!lara.target)
			{
				lara.target = bestitem;
				LastTargets[0] = 0;
			}
			else if (input & IN_TARGET)
			{
				lara.target = 0;

				for (match = 0; match < 8; ++match)
				{
					if (!TargetList[match])
						break;

					for (slot = 0; slot < 8; ++slot)
					{
						if (!LastTargets[slot])
						{
							slot = 8;
							break;
						}

						if (LastTargets[slot] == TargetList[match])
							break;
					}

					if (slot == 8)
					{
						lara.target = TargetList[match];
						break;
					}
				}

				if (!lara.target)
				{
					lara.target = bestitem;
					LastTargets[0] = NULL;
				}
			}
		}
	}
	else
		lara.target = 0;

	if (lara.target != LastTargets[0])
	{
		for (slot = 7; slot > 0; --slot)
			LastTargets[slot] = LastTargets[slot - 1];

		LastTargets[0] = lara.target;
	}

	LaraTargetInfo(winfo);
}

void find_target_point(ITEM_INFO* item, GAME_VECTOR* target)
{
	short* bounds;
	long x, y, z, c, s;

	bounds = GetBestFrame(item);
	x = (bounds[0] + bounds[1]) >> 1;
	y = (bounds[2] + (bounds[3] - bounds[2]) / 3);
	z = (bounds[4] + bounds[5]) / 2;
	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	target->x = item->pos.x_pos + ((x * c + z * s) >> 14);
	target->y = item->pos.y_pos + y;
	target->z = item->pos.z_pos + ((z * c - x * s) >> 14);
	target->room_number = item->room_number;
}

void AimWeapon(WEAPON_INFO* winfo, lara_arm* arm)
{
	short curr, speed, destx, desty;

	desty = 0;
	speed = winfo->aim_speed;

	if (arm->lock)
	{
		desty = lara.target_angles[0];
		destx = lara.target_angles[1];
	}
	else
	{
		destx = 0;
		desty = 0;
	}

	curr = arm->y_rot;

	if ((curr < desty - speed) || (curr > speed + desty))
	{
		if (curr >= desty)
			curr -= speed;
		else
			curr += speed;
	}
	else
		curr = desty;

	arm->y_rot = curr;
	curr = arm->x_rot;

	if ((curr < destx - speed) || (curr > speed + destx))
	{
		if (curr >= destx)
			curr -= speed;
		else
			curr += speed;
	}
	else
		curr = destx;

	arm->x_rot = curr;
	arm->z_rot = 0;
}

int FireWeapon(int weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles)
{
	int r, i, nums, bestdist, best;
	short* ammo;
	WEAPON_INFO* winfo;
	SPHERE* sptr;
	short room_number;

	bum_view.x_pos = 0;
	bum_view.y_pos = 0;
	bum_view.z_pos = 0;
	GetLaraJointPos((PHD_VECTOR*)&bum_view, 11);
	ammo = get_current_ammo_pointer(weapon_type);

	if (!*ammo)
		return 0;
	else if (*ammo != -1)
		(*ammo)--;

	bum_view.x_pos = src->pos.x_pos;
	bum_view.z_pos = src->pos.z_pos;
	winfo = &weapons[weapon_type];
	r = winfo->shot_accuracy * (GetRandomControl() - 16384);
	bum_view.x_rot = angles[1] + ((r + (r >> 0x1F & 0xFFFF)) >> 16);
	r = winfo->shot_accuracy * (GetRandomControl() - 16384) / 65536;
	bum_view.y_rot = angles[0] + r;
	bum_view.z_rot = 0;
	phd_GenerateW2V(&bum_view);
	nums = GetSpheres(target, Slist, 0);
	best = -1;
	bestdist = 0x7FFFFFFF;

	if (nums > 0)
	{
		for (i = 0; i < nums; i++)
		{
			sptr = &Slist[i];
			r = sptr->r;

			if ((ABS(sptr->x)) < r && (ABS(sptr->y)) < r && (sptr->z > r) && ((sptr->x * sptr->x) + (sptr->y * sptr->y) <= (r * r)))
			{
				if (sptr->z - r < bestdist)
				{
					bestdist = sptr->z - r;
					best = i;
				}
			}
		}
	}

	lara.has_fired = 1;
	lara.Fired = 1;
	bum_vsrc.x = bum_view.x_pos;
	bum_vsrc.y = bum_view.y_pos;
	bum_vsrc.z = bum_view.z_pos;
	room_number = src->room_number;
	GetFloor(bum_view.x_pos, bum_view.y_pos, bum_view.z_pos, &room_number);
	bum_vsrc.room_number = room_number;

	if (best < 0)
	{
		bum_vdest.x = bum_vsrc.x + (20480 * phd_mxptr[8] >> 14);
		bum_vdest.y = bum_vsrc.y + (20480 * phd_mxptr[9] >> 14);
		bum_vdest.z = bum_vsrc.z + (20480 * phd_mxptr[10] >> 14);

		GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1);
		return -1;
	}
	else
	{
		savegame.Game.AmmoHits++;
		bum_vdest.x = bum_vsrc.x + (bestdist * phd_mxptr[8] >> 14);
		bum_vdest.y = bum_vsrc.y + (bestdist * phd_mxptr[9] >> 14);
		bum_vdest.z = bum_vsrc.z + (bestdist * phd_mxptr[10] >> 14);

		if (!GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1))
			HitTarget(target, &bum_vdest, winfo->damage, 0);

		return 1;
	}
}

void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, int damage, int grenade)
{
	OBJECT_INFO* obj;

	obj = &objects[item->object_number];
	item->hit_status = 1;

	if (item->data != 0 && item != lara_item)
		((int*)item->data)[3] |= 0x10;

	if (hitpos && obj->HitEffect)
	{

		switch (obj->HitEffect)
		{
		case 1:
			DoBloodSplat(hitpos->x, hitpos->y, hitpos->z, (GetRandomControl() & 3) + 3, item->pos.y_rot, item->room_number);
			break;

		case 3:
			TriggerRicochetSpark(hitpos, lara_item->pos.y_rot, 3, 0);
			break;

		case 2:
			TriggerRicochetSpark(hitpos, lara_item->pos.y_rot, 3, -5);
			SoundEffect(SFX_SWORD_GOD_HITMET, &item->pos, SFX_DEFAULT);
			break;
		}
	}

	if (obj->undead && grenade == 0 && item->hit_points != -16384)
		return;

	if (item->hit_points > 0 && damage > item->hit_points)
		savegame.Level.Kills++;

	item->hit_points -= damage;
}

int WeaponObject(int weapon_type)
{
	switch (weapon_type)
	{
	case WEAPON_REVOLVER:
		return REVOLVER_ANIM;

	case WEAPON_UZI:
		return UZI_ANIM;

	case WEAPON_SHOTGUN:
		return SHOTGUN_ANIM;

	case WEAPON_HK:
		return HK_ANIM;

	case WEAPON_CROSSBOW:
		return CROSSBOW_ANIM;

	default:
		return PISTOLS_ANIM;
	}
}

int WeaponObjectMesh(int weapon_type)
{
	switch (weapon_type)
	{
	case WEAPON_REVOLVER:
		if (lara.sixshooter_type_carried & WTYPE_LASERSIGHT)
			return LARA_REVOLVER_LASER;
		else
			return REVOLVER_ANIM;

	case WEAPON_UZI:
		return UZI_ANIM;

	case WEAPON_SHOTGUN:
		return SHOTGUN_ANIM;

	case WEAPON_HK:
		return HK_ANIM;

	case WEAPON_CROSSBOW:
		if (lara.crossbow_type_carried & WTYPE_LASERSIGHT)
			return LARA_CROSSBOW_LASER;
		else
			return CROSSBOW_ANIM;

	default:
		return PISTOLS_ANIM;
	}
}

void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	short room_number;
	long ceiling, height, oldtype, oldonobj, oldheight, bs, yang, xs;

	item = &items[item_number];
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	oldheight = GetHeight(floor, x, y, z);
	oldonobj = OnObject;
	oldtype = height_type;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < height)
	{
		if (yv >= 0)
		{
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
			oldheight = GetHeight(floor, item->pos.x_pos, y, item->pos.z_pos);
			oldonobj = OnObject;
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (item->pos.y_pos >= oldheight && oldonobj)
			{
				if (item->fallspeed > 0)
				{
					if (item->fallspeed <= 16)
					{
						item->speed -= 3;
						item->fallspeed = 0;

						if (item->speed < 0)
							item->speed = 0;
					}
					else
					{
						item->fallspeed = -(item->fallspeed >> 2);

						if (item->fallspeed < -100)
							item->fallspeed = -100;
					}
				}

				item->pos.y_pos = oldheight;
			}
		}

		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (ceiling > item->pos.y_pos)
		{
			if (y < ceiling && ((x >> 10 != item->pos.x_pos >> 10) || (z >> 10 != item->pos.z_pos >> 10)))
			{
				if ((x & ~1023) != (item->pos.x_pos & ~1023))
				{

					if (xv > 0)
						item->pos.y_rot = 49152 + (16384 - item->pos.y_rot);
					else
						item->pos.y_rot = 16384 + (49152 - item->pos.y_rot);
				}
				else
					item->pos.y_rot = -item->pos.y_rot;

				item->pos.x_pos = x;
				item->pos.y_pos = y;
				item->pos.z_pos = z;
				item->speed >>= 1;
			}
			else
				item->pos.y_pos = ceiling;

			if (item->fallspeed < 0)
				item->fallspeed = -item->fallspeed;
		}
	}
	else
	{
		bs = 0;

		if ((height_type == BIG_SLOPE || height_type == DIAGONAL) && oldheight < height)
		{
			yang = (ushort) item->pos.y_rot;

			if (tiltyoff < 0)
			{
				if (yang >= 32768)
					bs = 1;
			}
			else
			{
				if (tiltyoff > 0 && yang <= 32768)
					bs = 1;
			}

			if (tiltxoff < 0)
			{
				if (yang >= 16384 && yang <= 49152)
					bs = 1;
			}
			else
			{
				if (tiltxoff > 0 && (yang <= 16384 || yang >= 49152))
					bs = 1;
			}
		}

		if (y > height + 32 && !bs && ((x >> 10) != (item->pos.x_pos >> 10) || (z >> 10) != (item->pos.z_pos >> 10)))
		{
			if ((x & ~1023) != (item->pos.x_pos & ~1023) && (z & ~1023) != (item->pos.z_pos & ~1023))
			{
				if (ABS(x - item->pos.x_pos) < ABS(z - item->pos.z_pos))
					xs = 1;
				else
					xs = 0;
			}
			else
				xs = 1;

			if ((x & ~1023) != (item->pos.x_pos & ~1023) && xs)
			{
				if (xv <= 0)
					item->pos.y_rot = 16384 + (49152 - item->pos.y_rot);
				else
					item->pos.y_rot = 49152 + (16384 - item->pos.y_rot);
			}
			else
				item->pos.y_rot = -item->pos.y_rot;

			item->pos.x_pos = x;
			item->pos.y_pos = y;
			item->pos.z_pos = z;
			item->speed >>= 1;
		}
		else if (height_type != BIG_SLOPE && height_type != DIAGONAL)
		{
			if (item->fallspeed > 0)
			{
				if (item->fallspeed <= 16)
				{
					item->speed -= 3;
					item->fallspeed = 0;

					if (item->speed < 0)
						item->speed = 0;
				}
				else
				{
					item->fallspeed = -(item->fallspeed >> 2);

					if (item->fallspeed < -100)
						item->fallspeed = -100;
				}
			}

			item->pos.y_pos = height;
		}
		else
		{
			item->speed -= item->speed >> 2;

			if (tiltyoff < 0 && ((ABS(tiltyoff)) - (ABS(tiltxoff)) > 1))
			{
				if ((ushort)item->pos.y_rot > 32768)
				{
					item->pos.y_rot = 16384 + (49152 - (ushort)item->pos.y_rot - 1);

					if (0 < item->fallspeed)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed -= tiltyoff << 1;

						if ((ushort)item->pos.y_rot > 16384 && (ushort)item->pos.y_rot < 49152)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < 16384)
								item->pos.y_rot = 16384;
						}
						else if ((ushort)item->pos.y_rot < 16384)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > 16384)
								item->pos.y_rot = 16384;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltyoff > 0 && ABS(tiltyoff) - ABS(tiltxoff) > 1)
			{
				if ((ushort)item->pos.y_rot < 32768)
				{
					item->pos.y_rot = 49152 + (16384 - (ushort)item->pos.y_rot - 1);

					if (0 < item->fallspeed)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += tiltyoff << 1;

						if ((ushort)item->pos.y_rot > 49152 || (ushort)item->pos.y_rot < 16384)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < 49152)
								item->pos.y_rot = -16384;
						}
						else if ((ushort)item->pos.y_rot < 49152)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > 49152)
								item->pos.y_rot = -16384;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltxoff < 0 && ((ABS(tiltxoff)) - (ABS(tiltyoff)) > 1))
			{

				if ((ushort)item->pos.y_rot > 16384 && (ushort)item->pos.y_rot < 49152)
				{
					item->pos.y_rot = 32767 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed -= tiltxoff << 1;

						if ((ushort)item->pos.y_rot < 32768)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot > 61440)
								item->pos.y_rot = 0;
						}
						else if ((ushort)item->pos.y_rot >= 32768)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot < 4096)
								item->pos.y_rot = 0;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltxoff > 0 && ((ABS(tiltxoff) - ABS(tiltyoff)) > 1))
			{

				if ((ushort)item->pos.y_rot > 49152 || (ushort)item->pos.y_rot < 16384)
				{
					item->pos.y_rot = 32767 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += tiltxoff << 1;

						if ((ushort)item->pos.y_rot > 32768)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < 32768)
								item->pos.y_rot = -32768;
						}
						else if ((ushort)item->pos.y_rot < 32768)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > 32768)
								item->pos.y_rot = -32768;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltyoff < 0 && tiltxoff < 0)
			{
				if ((ushort)item->pos.y_rot > 24576 && (ushort)item->pos.y_rot < (ushort)-8192)
				{
					item->pos.y_rot = 8192 + (-24576 - (ushort)item->pos.y_rot - 1);

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += (-tiltyoff) + (-tiltxoff);

						if ((ushort)item->pos.y_rot > 8192 && (ushort)item->pos.y_rot < -24576)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < 8192)
								item->pos.y_rot = 8192;
						}
						else if ((ushort)item->pos.y_rot != 8192)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > 8192)
								item->pos.y_rot = 8192;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}

			}
			else if (tiltyoff < 0 && tiltxoff > 0)
			{

				if ((ushort)item->pos.y_rot > -24576 || (ushort)item->pos.y_rot < 8192)
				{
					item->pos.y_rot = 24576 + (-8192 - (ushort)item->pos.y_rot - 1);

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += (-tiltyoff) + tiltxoff;

						if ((ushort)item->pos.y_rot < -8192 && (ushort)item->pos.y_rot > 24576)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < 24576)
								item->pos.y_rot = 24576;
						}
						else if ((ushort)item->pos.y_rot != 24576)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > 24576)
								item->pos.y_rot = 24576;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltyoff > 0 && tiltxoff > 0)
			{

				if ((ushort)item->pos.y_rot > (ushort)-8192 || (ushort)item->pos.y_rot < 24576)
				{
					item->pos.y_rot = -24576 + (8192 - (ushort)item->pos.y_rot - 1);

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += tiltyoff + tiltxoff;

						if ((ushort)item->pos.y_rot < 8192 || (ushort)item->pos.y_rot > -24576)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < -24576)
								item->pos.y_rot = -24576;
						}
						else if ((ushort)item->pos.y_rot != -24576)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > -24576)
								item->pos.y_rot = -24576;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}
			else if (tiltyoff > 0 && tiltxoff < 0)
			{
				if ((ushort)item->pos.y_rot > 8192 && (ushort)item->pos.y_rot < -24576)
				{
					item->pos.y_rot = -8192 + (24576 - (ushort)item->pos.y_rot - 1);

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += tiltyoff + (-tiltxoff);

						if ((ushort)item->pos.y_rot < 24576 || (ushort)item->pos.y_rot >(ushort) -8192)
						{
							item->pos.y_rot -= 4096;

							if ((ushort)item->pos.y_rot < (ushort)-8192)
								item->pos.y_rot = -8192;
						}
						else if ((ushort)item->pos.y_rot != (ushort)-8192)
						{
							item->pos.y_rot += 4096;

							if ((ushort)item->pos.y_rot > (ushort) -8192)
								item->pos.y_rot = -8192;
						}
					}

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
					else
						item->fallspeed = 0;
				}
			}

			item->pos.x_pos = x;
			item->pos.y_pos = y;
			item->pos.z_pos = z;
		}
	}

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);
}

short* get_current_ammo_pointer(int num)
{
	short* ammo;

	switch (num)
	{
	case WEAPON_REVOLVER:
		ammo = &lara.num_revolver_ammo;
		break;

	case WEAPON_UZI:
		ammo = &lara.num_uzi_ammo;
		break;

	case WEAPON_SHOTGUN:
		if ((lara.shotgun_type_carried & 8))
			ammo = &lara.num_shotgun_ammo1;
		else
			ammo = &lara.num_shotgun_ammo2;

		break;

	case WEAPON_HK:
		ammo = &lara.num_hk_ammo1;
		break;

	case WEAPON_CROSSBOW:
		if ((lara.crossbow_type_carried & 8))
			ammo = &lara.num_crossbow_ammo1;
		else
			ammo = &lara.num_crossbow_ammo2;

		break;

	default:
		ammo = &lara.num_pistols_ammo;
		break;
	}

	return ammo;
}

void inject_larafire()
{
	INJECT(0x00452430, LaraGun);
	INJECT(0x00452AF0, CheckForHoldingState);
	INJECT(0x00452B30, InitialiseNewWeapon);
	INJECT(0x00452CC0, LaraTargetInfo);
	INJECT(0x00452ED0, LaraGetNewTarget);
	INJECT(0x004533A0, find_target_point);
	INJECT(0x00453490, AimWeapon);
	INJECT(0x00453580, FireWeapon);
	INJECT(0x00453930, HitTarget);
	INJECT(0x00453AE0, WeaponObject);
	INJECT(0x00453B50, WeaponObjectMesh);
	INJECT(0x00453BE0, DoProperDetection);
	INJECT(0x004546C0, get_current_ammo_pointer);
}
