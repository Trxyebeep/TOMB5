#include "../tomb5/pch.h"
#include "pickup.h"
#include "gameflow.h"
#include "../specific/specific.h"
#include "xatracks.h"
#include "effects.h"
#include "sound.h"
#include "items.h"
#include "control.h"
#include "objects.h"
#include "collide.h"
#include "lara_states.h"
#include "draw.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "newinv2.h"
#include "switch.h"
#include "health.h"

static short PuzzleBounds[12] = 
{
	0, 0, -0x100, 0x100, 0, 0, -0x71C, 0x71C, -0x1554, 0x1554, -0x71C, 0x71C
};

void RegeneratePickups()
{
	for (int lp = 0; lp < NumRPickups; lp++)
	{
		ITEM_INFO* item = &items[RPickups[lp]];

		if (item->status == ITEM_INVISIBLE)
		{
			short* ammo = NULL;

			if (item->object_number == CROSSBOW_AMMO1_ITEM)
				ammo = &lara.num_crossbow_ammo1;
			if (item->object_number == CROSSBOW_AMMO2_ITEM)
				ammo = &lara.num_crossbow_ammo2;
			if (item->object_number == HK_AMMO_ITEM)
				ammo = &lara.num_hk_ammo1;
			if (item->object_number == REVOLVER_AMMO_ITEM)
				ammo = &lara.num_revolver_ammo;
			if (item->object_number == SHOTGUN_AMMO1_ITEM)
				ammo = &lara.num_shotgun_ammo1;
			if (item->object_number == SHOTGUN_AMMO1_ITEM)
				ammo = &lara.num_shotgun_ammo2;

			if (ammo && *ammo == 0)
				item->status = ITEM_INACTIVE;
		}
	}
}

static void PuzzleDone(ITEM_INFO* item, short item_num)
{
	if (item->object_number == PUZZLE_HOLE1 && gfCurrentLevel == LVL5_GALLOWS_TREE)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(CDA_XA6_SPOOKY03, 0);
		SoundEffect(SFX_HANGMAN_LAUGH_OFFCAM, &item->pos, 0);
	}

	item->object_number += 8;//puzzle hole to done, because there's 8 of them
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->required_anim_state = 0;
	item->goal_anim_state = item->anim_number;
	item->current_anim_state = item->anim_number;
	AddActiveItem(item_num);
	item->flags |= IFLAG_ACTIVATION_MASK;
	item->status = ITEM_ACTIVE;

	if (item->trigger_flags == 998)
	{
		for (int i = 0; i < level_items; i++)
		{
			if (items[i].object_number == AIRLOCK_SWITCH && items[i].pos.x_pos == item->pos.x_pos && items[i].pos.z_pos == item->pos.z_pos)
			{
				FlipMap(items[i].trigger_flags - 7);
				flipmap[items[i].trigger_flags - 7] ^= IFLAG_ACTIVATION_MASK;
				items[i].status = ITEM_INACTIVE;
				items[i].flags |= IFLAG_TRIGGERED;
			}
		}
	}

	if (item->trigger_flags > 1024)
	{
		cutrot = 0;
		cutseq_num = item->trigger_flags - 1024;
	}
}

int KeyTrigger(short item_num)
{
	ITEM_INFO* item;
	int oldkey;

	item = &items[item_num];

	if ((item->status != ITEM_ACTIVE || lara.gun_status == LG_HANDS_BUSY) && (!KeyTriggerActive || lara.gun_status != LG_HANDS_BUSY))
		return -1;

	oldkey = KeyTriggerActive;

	if (!KeyTriggerActive)
		item->status = ITEM_DEACTIVATED;

	KeyTriggerActive = 0;
	return oldkey;
}

int PickupTrigger(short item_num)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (item->flags & IFLAG_KILLED ||
		item->status != ITEM_INVISIBLE ||
		item->item_flags[3] != 1 ||
		item->trigger_flags & 128)
		return 0;

	KillItem(item_num);
	return 1;
}

void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	if (items[item_num].trigger_flags != 999 && items[item_num].trigger_flags != 998)
		ObjectCollision(item_num, l, coll);
}

void PuzzleHoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short* bounds;
	long angle;
	int PuzzleType, hole, puzzle;
	short yrot;

	PuzzleType = 0;
	item = &items[item_num];
	hole = item->object_number - PUZZLE_HOLE1;

	if (item->trigger_flags >= 0)
	{
		if (item->trigger_flags <= 1024)
		{
			if (item->trigger_flags && item->trigger_flags != 999 && item->trigger_flags != 998)
				PuzzleType = 3;
		}
		else
			PuzzleType = 2;
	}
	else
		PuzzleType = 1;

	if (((input & IN_ACTION || GLOBAL_inventoryitemchosen != -1) &&
		(!BinocularRange && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && GetKeyTrigger(item))) ||
		(lara.IsMoving && lara.GeneralPtr == (void*)item_num))
	{
		bounds = GetBoundsAccurate(item);
		yrot = item->pos.y_rot;

		PuzzleBounds[0] = bounds[0] - 256;
		PuzzleBounds[1] = bounds[1] + 256;
		PuzzleBounds[4] = bounds[4] - 256;
		PuzzleBounds[5] = bounds[5] + 256;

		if (item->trigger_flags == 1058)
		{
			PuzzleBounds[0] -= 300;
			PuzzleBounds[1] += 300;
			PuzzleBounds[4] -= 300;
			PuzzleBounds[5] += 300;
			item->pos.y_rot = l->pos.y_rot;
		}

		if (hole == 7 && gfCurrentLevel >= LVL5_THIRTEENTH_FLOOR && gfCurrentLevel <= LVL5_ESCAPE_WITH_THE_IRIS)
		{
			PuzzleBounds[4] -= 512;
			PuzzleBounds[0] -= 512;
			PuzzleBounds[1] += 512;
			PuzzleBounds[5] += 512;
			PuzzleType = 0;
		}

		if (TestLaraPosition(PuzzleBounds, item, l))
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			puzzle = GLOBAL_inventoryitemchosen - PUZZLE_ITEM1;

			if (item->trigger_flags == 1058)
			{
				if (!cutseq_trig && !CheckCutPlayed(34))
				{
					pos2.x = 0;
					pos2.y = 0;
					pos2.z = 0;
					GetJointAbsPosition(item, &pos2, 0);
					angle = (mGetAngle(pos2.x, pos2.z, lara_item->pos.x_pos, lara_item->pos.z_pos) - lara_item->pos.y_rot) & 0xFFFF;

					if (angle > 8960 && angle <= 23808)
					{
						cutseq_num = 34;
						cutrot = 0;
						AddDisplayPickup(PUZZLE_ITEM1);
					}
				}
				
				item->pos.y_rot = yrot;
				return;
			}

			if (hole == 7 && gfCurrentLevel >= LVL5_THIRTEENTH_FLOOR && gfCurrentLevel <= LVL5_ESCAPE_WITH_THE_IRIS)
			{
				pos.z = bounds[4] - 100;

				if (MoveLaraPosition(&pos, item, l))
				{
					lara.IsMoving = 0;
					l->anim_number = ANIM_BREATH;
					l->frame_number = anims[ANIM_BREATH].frame_base;
					l->current_anim_state = AS_STOP;
					GLOBAL_invkeypadcombination = item->trigger_flags;
					GLOBAL_enterinventory = 0xDEADBEEF;
					return;
				}

				lara.GeneralPtr = (void*)item_num;
				GLOBAL_inventoryitemchosen = -1;
				return;
			}

			if (!lara.IsMoving)
			{		
				if (GLOBAL_inventoryitemchosen == -1)
				{
					if (have_i_got_object(hole + PUZZLE_ITEM1))
						GLOBAL_enterinventory = hole + PUZZLE_ITEM1;

					item->pos.y_rot = yrot;
					return;
				}
				else if (puzzle != hole)
				{
					item->pos.y_rot = yrot;
					return;
				}	
			}

			pos.z = bounds[4] - 100;

			if ((PuzzleType == 2 && item->trigger_flags != 1036) || MoveLaraPosition(&pos, item, l))
			{
				remove_inventory_item(hole + PUZZLE_ITEM1);

				if (PuzzleType == 1)
				{
					l->anim_number = -item->trigger_flags;
					l->current_anim_state = AS_CONTROLLED;

					if (l->anim_number != 423)
						PuzzleDone(item, item_num);
				}
				else
				{
					l->anim_number = ANIM_USEPUZZLE;
					l->current_anim_state = AS_USEPUZZLE;
					item->item_flags[0] = 1;
				}

				if (hole == 1 && gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
					bDisableLaraControl = 1;

				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->flags |= IFLAG_TRIGGERED;
			}

			lara.GeneralPtr = (void*)item_num;
			GLOBAL_inventoryitemchosen = NO_ITEM;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot = yrot;
	}
	else
	{
		if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_USEPUZZLE && l->frame_number == anims[134].frame_base + 80 && item->item_flags[0])
		{
			if (PuzzleType == 3)
				l->item_flags[0] = item->trigger_flags;
			else
				l->item_flags[0] = 0;

			PuzzleDone(item, item_num);
			item->item_flags[0] = 0;
		}
		else if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_CONTROLLED && l->anim_number == 423 && l->frame_number == anims[423].frame_base + 180)
			PuzzleDone(item, item_num);
		else if (l->current_anim_state != AS_CONTROLLED && PuzzleType != 2)
			ObjectCollision(item_num, l, coll);
	}
}

void inject_pickup()
{
	INJECT(0x00467AF0, RegeneratePickups);
	INJECT(0x004693A0, PuzzleDone);
	INJECT(0x00469550, KeyTrigger);
	INJECT(0x004695E0, PickupTrigger);
	INJECT(0x00468C00, PuzzleDoneCollision);
	INJECT(0x00468C70, PuzzleHoleCollision);
}
