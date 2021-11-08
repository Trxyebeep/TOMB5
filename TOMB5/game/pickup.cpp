#include "../tomb5/pch.h"
#include "pickup.h"
#include "gameflow.h"
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
#include "../specific/audio.h"
#include "andy.h"

static PHD_VECTOR SOPos = { 0, 0, 0 };
static PHD_VECTOR MSPos = { 0, 0, 0 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 312 };
static short SearchCollectFrames[4] = { 180, 100, 153, 83 };
static short SearchOffsets[4] = { 160, 96, 160, 112 };
static short SearchAnims[4] = { ANIM_SCABINET, ANIM_SDRAWERS, ANIM_SSHELVES, ANIM_SBOX };

static short PuzzleBounds[12] = 
{
	0, 0, -256, 256, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820
};

static short SOBounds[12] =
{
	0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820
};

static short MSBounds[12] =
{
	0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820
};

static short KeyHoleBounds[12] =
{
	-256, 256, 0, 0, 0, 412, -1820, 1820, -5460, 5460, -1820, 1820
};

void RegeneratePickups()
{
	ITEM_INFO* item;
	short* ammo;

	for (int i = 0; i < NumRPickups; i++)
	{
		item = &items[RPickups[i]];

		if (item->status == ITEM_INVISIBLE)
		{
			ammo = 0;

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

			if (ammo && !*ammo)
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
	item->flags |= IFL_CODEBITS;
	item->status = ITEM_ACTIVE;

	if (item->trigger_flags == 998)
	{
		for (int i = 0; i < level_items; i++)
		{
			if (items[i].object_number == AIRLOCK_SWITCH && items[i].pos.x_pos == item->pos.x_pos && items[i].pos.z_pos == item->pos.z_pos)
			{
				FlipMap(items[i].trigger_flags - 7);
				flipmap[items[i].trigger_flags - 7] ^= IFL_CODEBITS;
				items[i].status = ITEM_INACTIVE;
				items[i].flags |= IFL_TRIGGERED;
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

	if (item->flags & IFL_CLEARBODY || item->status != ITEM_INVISIBLE || item->item_flags[3] != 1 || item->trigger_flags & 128)
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
	long angle, PuzzleType, hole, puzzle;
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
					if (have_i_got_object((short)(hole + PUZZLE_ITEM1)))
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
				remove_inventory_item((short)(hole + PUZZLE_ITEM1));

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
				item->flags |= IFL_TRIGGERED;
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
		if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_USEPUZZLE &&
			l->frame_number == anims[134].frame_base + 80 && item->item_flags[0])
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

void SearchObjectControl(short item_number)
{
	ITEM_INFO* item;
	short ObjNum, frame, flip;

	item = &items[item_number];
	ObjNum = 3 - ((SEARCH_OBJECT4 - item->object_number) >> 1);

	if (ObjNum != 3 || item->item_flags[0] == 1)
		AnimateItem(item);

	frame = item->frame_number - anims[item->anim_number].frame_base;

	if (ObjNum == 1)
	{
		if (frame == 18)
			item->mesh_bits = 1;
		else if (frame == 172)
			item->mesh_bits = 2;
	}
	else if (!ObjNum)
	{
		if (frame > 0)
		{
			item->meshswap_meshbits = 0;
			item->mesh_bits = -1;
		}
		else
		{
			item->meshswap_meshbits = -1;
			item->mesh_bits = 7;
		}
	}
	else if (ObjNum == 3)
	{
		flip = (short)flip_stats[0];
		item->mesh_bits = flip ? 48 : 9;

		if (frame >= 45 && frame <= 131)
			item->mesh_bits |= flip ? 4 : 2;

		if (item->item_flags[1] != -1 && objects[items[item->item_flags[1]].object_number].collision == PickUpCollision)
			items[item->item_flags[1]].status = flip ? ITEM_INACTIVE : ITEM_INVISIBLE;
	}

	if (frame == SearchCollectFrames[ObjNum])
	{
		if (ObjNum == 3)
		{
			if (item->item_flags[1] != -1)
			{
				if (objects[items[item->item_flags[1]].object_number].collision == PickUpCollision)
				{
					AddDisplayPickup(items[item->item_flags[1]].object_number);
					KillItem(item->item_flags[1]);
				}
				else
				{
					AddActiveItem(item->item_flags[1]);
					items[item->item_flags[1]].flags |= IFL_CODEBITS;
					items[item->item_flags[1]].status = ITEM_ACTIVE;
					lara_item->hit_points = 640;
				}

				item->item_flags[1] = -1;
			}
		}
		else
			CollectCarriedItems(item);
	}

	if (item->status == ITEM_DEACTIVATED)
	{
		if (ObjNum == 3)
		{
			item->item_flags[0] = 0;
			item->status = ITEM_ACTIVE;
		}
		else
		{
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
		}
	}
}

void SearchObjectCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;
	short ObjNum;

	item = &items[item_num];
	ObjNum = 3 - ((SEARCH_OBJECT4 - item->object_number) >> 1);

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && (item->status == ITEM_INACTIVE && ObjNum != 3 || !item->item_flags[0]) || lara.IsMoving && lara.GeneralPtr == (void *) item_num)
	{
		bounds = GetBoundsAccurate(item);

		if (ObjNum)
		{
			SOBounds[0] = bounds[0] - 128;
			SOBounds[1] = bounds[1] + 128;
		}
		else
		{
			SOBounds[0] = bounds[0] + 64;
			SOBounds[1] = bounds[1] - 64;
		}

		SOBounds[4] = bounds[4] - 200;
		SOBounds[5] = bounds[5] + 200;
		SOPos.z = bounds[4] - SearchOffsets[ObjNum];

		if (TestLaraPosition(SOBounds, item, l))
		{
			if (MoveLaraPosition(&SOPos, item, l))
			{
				l->current_anim_state = AS_CONTROLLED;
				l->anim_number = SearchAnims[ObjNum];
				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = false;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;

				if (ObjNum == 3)
					item->item_flags[0] = 1;
				else
				{
					AddActiveItem(item_num);
					item->status = ITEM_ACTIVE;
				}

				item->anim_number = objects[item->object_number].anim_index + 1;
				item->frame_number = anims[item->anim_number].frame_base;
				AnimateItem(item);
			}
			else
				lara.GeneralPtr = (void *) item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void *) item_num)
		{
			lara.IsMoving = false;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (l->current_anim_state != AS_CONTROLLED)
		ObjectCollision(item_num, l, coll);
}

void MonitorScreenCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;

	item = &items[item_num];

	if (l->anim_number == ANIM_ONEHANDPUSHSW && l->frame_number == anims[l->anim_number].frame_base + 24)
		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && item->status == ITEM_INACTIVE || lara.IsMoving && lara.GeneralPtr == (void*) item_num)
	{
		bounds = GetBoundsAccurate(item);
		MSBounds[0] = bounds[0] - 256;
		MSBounds[1] = bounds[1] + 256;
		MSBounds[4] = bounds[4] - 512;
		MSBounds[5] = bounds[5] + 512;
		MSPos.z = bounds[4] - 256;

		if (TestLaraPosition(MSBounds, item, l))
		{
			if (MoveLaraPosition(&MSPos, item, l))
			{
				l->current_anim_state = AS_SWITCHON;
				l->anim_number = ANIM_ONEHANDPUSHSW;
				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
				item->flags |= IFL_TRIGGERED;
			}
			else
				lara.GeneralPtr = (void*) item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*) item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else
		ObjectCollision(item_num, l, coll);
}

void AnimatingPickUp(short item_number)
{
	ITEM_INFO* item;
	short room_number;

	item = &items[item_number];

	switch (item->trigger_flags & 0x3F)
	{
	case 5:
		item->fallspeed += 6;
		item->pos.y_pos += item->fallspeed;
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

		if (item->pos.y_pos > item->item_flags[0])
		{
			item->pos.y_pos = item->item_flags[0];

			if (item->fallspeed <= 64)
				item->trigger_flags &= -64;
			else
				item->fallspeed = -item->fallspeed >> 2;
		}

		if (item->room_number != room_number)
			ItemNewRoom(item_number, room_number);

		break;

	case 2:
	case 6:
	case 7:
	case 8:
	case 9:
		AnimateItem(item);
		break;

	case 11:
		TriggerCoinGlow(item_number);
		break;
	}
}

void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long key, hole;

	item = &items[item_number];

	if (item->trigger_flags == 1 && item->object_number == KEY_HOLE8)
	{
		if (item->item_flags[3])
		{
			item->item_flags[3]--;

			if (!item->item_flags[3])
				item->mesh_bits = 2;
		}
	}

	if ((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) && !BinocularRange && lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		key = GLOBAL_inventoryitemchosen - KEY_ITEM1;
		hole = item->object_number - KEY_HOLE1;

		if (TestLaraPosition(KeyHoleBounds, item, l))
		{
			if (!lara.IsMoving)
			{
				if (item->status == ITEM_INVISIBLE)
					return;

				if (GLOBAL_inventoryitemchosen == -1)
				{
					if (have_i_got_object((short)(hole + KEY_ITEM1)))
						GLOBAL_enterinventory = hole + KEY_ITEM1;

					return;
				}

				if (key != hole)
					return;
			}

			if (MoveLaraPosition(&KeyHolePosition, item, l))
			{
				if (item->object_number != KEY_HOLE8)
					remove_inventory_item((short)(hole + KEY_ITEM1));

				if (item->object_number == KEY_HOLE8)
					l->anim_number = ANIM_KEYCARD;
				else
					l->anim_number = ANIM_USEKEY;

				l->current_anim_state = AS_USEKEY;
				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
				item->flags &= IFL_TRIGGERED;

				if (item->trigger_flags == 1 && item->object_number == KEY_HOLE8)
					item->item_flags[3] = 92;
			}
			else
				lara.GeneralPtr = (void*)item_number;

			GLOBAL_inventoryitemchosen = NO_ITEM;
		}
	}
	else if (item->object_number < KEY_HOLE6)
		ObjectCollision(item_number, l, coll);
}

short* FindPlinth(ITEM_INFO* item)
{
	ITEM_INFO* plinth;
	ROOM_INFO* r;
	MESH_INFO* mesh;
	short* p;
	short* o;
	long i;
	short item_num;
	
	o = 0;
	r = &room[item->room_number];
	mesh = r->mesh;

	for (i = r->num_meshes; i > 0; i--)
	{
		if (mesh->Flags & 1 && item->pos.x_pos == mesh->x && item->pos.z_pos == mesh->z)
		{
			p = GetBestFrame(item);
			o = &static_objects[mesh->static_number].x_minc;

			if (p[0] <= o[1] && p[1] >= o[0] && p[4] <= o[5] && p[5] >= o[4] && (o[0] || o[1]))
				break;
		}

		mesh++;
	}

	if (i)
		return o;

	item_num = r->item_number;

	while (1)
	{
		plinth = &items[item_num];

		if (item != plinth)
		{
			if (objects[plinth->object_number].collision != PickUpCollision && item->pos.x_pos == plinth->pos.x_pos &&
				item->pos.y_pos <= plinth->pos.y_pos && item->pos.z_pos == plinth->pos.z_pos &&
				(plinth->object_number != HIGH_OBJECT1 || plinth->item_flags[0] == 5))
				break;
		}

		item_num = plinth->next_item;

		if (item_num == NO_ITEM)
			return 0;
	}

	return GetBestFrame(&items[item_num]);
}

void inject_pickup(bool replace)
{
	INJECT(0x00467AF0, RegeneratePickups, replace);
	INJECT(0x004693A0, PuzzleDone, replace);
	INJECT(0x00469550, KeyTrigger, replace);
	INJECT(0x004695E0, PickupTrigger, replace);
	INJECT(0x00468C00, PuzzleDoneCollision, replace);
	INJECT(0x00468C70, PuzzleHoleCollision, replace);
	INJECT(0x00469660, SearchObjectControl, replace);
	INJECT(0x004699A0, SearchObjectCollision, replace);
	INJECT(0x00469D10, MonitorScreenCollision, replace);
	INJECT(0x004679D0, AnimatingPickUp, replace);
	INJECT(0x00468930, KeyHoleCollision, replace);
	INJECT(0x00468770, FindPlinth, replace);
}
