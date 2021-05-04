#include "../tomb5/pch.h"
#include "laramisc.h"
#include "../global/types.h"
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

void GetLaraDeadlyBounds()
{
	short* bounds;
	short tbounds[6];

	bounds = GetBoundsAccurate(lara_item);
	phd_PushUnitMatrix();
	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);
	phd_SetTrans(0, 0, 0);
	mRotBoundingBoxNoPersp(bounds, tbounds);
	phd_mxptr -= 12;
	aMXPtr -= 12;
	DeadlyBounds[0] = lara_item->pos.x_pos + tbounds[0];
	DeadlyBounds[1] = lara_item->pos.x_pos + tbounds[1];
	DeadlyBounds[2] = lara_item->pos.y_pos + tbounds[2];
	DeadlyBounds[3] = lara_item->pos.y_pos + tbounds[3];
	DeadlyBounds[4] = lara_item->pos.z_pos + tbounds[4];
	DeadlyBounds[5] = lara_item->pos.z_pos + tbounds[5];
}

void InitialiseLaraAnims(ITEM_INFO* item)
{
	if (room[item->room_number].flags & RF_FILL_WATER)
	{
		lara.water_status = LW_UNDERWATER;
		item->fallspeed = 0;
		item->goal_anim_state = AS_TREAD;
		item->current_anim_state = AS_TREAD;
		item->anim_number = ANIMATION_LARA_UNDERWATER_IDLE;
		item->frame_number = anims[ANIMATION_LARA_UNDERWATER_IDLE].frame_base;
	}
	else
	{
		lara.water_status = LW_ABOVE_WATER;
		item->goal_anim_state = AS_STOP;
		item->current_anim_state = AS_STOP;
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
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

	if ((input & IN_WALK) && !(input & IN_LOOK))
	{
		lara.water_status = LW_ABOVE_WATER;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
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

void LaraCheatyBits()
{
	if (objects[CROWBAR_ITEM].loaded)
		lara.crowbar = 1;

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
}

#ifdef VER_JP
void _cheats()
{
	if (!Gameflow->CheatEnabled)
		return;

	if (input & IN_D)
	{
		LaraCheatyBits();
		lara_item->hit_points = 1000;
	}

	if (input & IN_CHEAT)
	{
		dels_give_lara_items_cheat();
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara.water_status = LW_FLYCHEAT;
			lara_item->frame_number = anims[ANIMATION_LARA_DOZY].frame_base;
			lara_item->anim_number = ANIMATION_LARA_DOZY;
			lara_item->current_anim_state = AS_SWIMCHEAT;
			lara_item->goal_anim_state = AS_SWIMCHEAT;
			lara_item->gravity_status = 1;
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
}
#endif

void inject_laramisc()
{
	INJECT(0x004569C0, GetLaraDeadlyBounds);
	INJECT(0x00456900, InitialiseLaraAnims);
	INJECT(0x004568C0, InitialiseLaraLoad);
	INJECT(0x00456320, LaraCheat);
	INJECT(0x00455680, LaraInitialiseMeshes);
	INJECT(0x00473210, InitialiseLara);
	INJECT(0x004557B0, LaraCheatyBits);
//	INJECT(0x004563F0, AnimateLara);
}
