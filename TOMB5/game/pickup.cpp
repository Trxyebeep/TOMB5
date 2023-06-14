#include "../tomb5/pch.h"
#include "pickup.h"
#include "gameflow.h"
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
#include "larafire.h"
#include "laraflar.h"
#include "flmtorch.h"
#include "deltapak.h"
#include "camera.h"
#include "spotcam.h"
#include "../specific/input.h"
#include "lara.h"

uchar NumRPickups;
uchar RPickups[16];
char KeyTriggerActive = 0;

static short PuzzleBounds[12] = { 0, 0, -256, 256, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short SOBounds[12] = { 0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short MSBounds[12] = { 0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short KeyHoleBounds[12] = { -256, 256, 0, 0, 0, 412, -1820, 1820, -5460, 5460, -1820, 1820 };
static short PickUpBoundsUW[12] = { -512, 512, -512, 512, -512, 512, -8190, 8190, -8190, 8190, -8190, 8190 };
static short PickUpBounds[12] = { -256, 256, -200, 200, -256, 256, -1820, 1820, 0, 0, 0, 0 };
static short HiddenPickUpBounds[12] = { -256, 256, -100, 100, -800, -256, -1820, 1820, -5460, 5460, 0, 0 };
static short PlinthPickUpBounds[12] = { -256, 256, -640, 640, -511, 0, -1820, 1820, -5460, 5460, 0, 0 };
static short JobyCrowPickUpBounds[12] = { -512, 0, -100, 100, 0, 512, -1820, 1820, -5460, 5460, 0, 0 };
static short CrowbarPickUpBounds[12] = { -256, 256, -100, 100, 200, 512, -1820, 1820, -5460, 5460, 0, 0 };

static PHD_VECTOR SOPos = { 0, 0, 0 };
static PHD_VECTOR MSPos = { 0, 0, 0 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 312 };
static short SearchCollectFrames[4] = { 180, 100, 153, 83 };
static short SearchOffsets[4] = { 160, 96, 160, 112 };
static short SearchAnims[4] = { ANIM_SCABINET, ANIM_SDRAWERS, ANIM_SSHELVES, ANIM_SBOX };
static PHD_VECTOR PickUpPositionUW = { 0, -200, -350 };
static PHD_VECTOR PickUpPosition = { 0, 0, -100 };
static PHD_VECTOR HiddenPickUpPosition = { 0, 0, -690 };
static PHD_VECTOR PlinthPickUpPosition = { 0, 0, -460 };
static PHD_VECTOR JobyCrowPickUpPosition = { -224, 0, 240 };
static PHD_VECTOR CrowbarPickUpPosition = { 0, 0, 215 };

void RegeneratePickups()
{
	ITEM_INFO* item;
	short* ammo;

	for (int i = 0; i < NumRPickups; i++)
	{
		item = &items[RPickups[i]];

		if (item->status != ITEM_INVISIBLE)
			continue;

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

static void PuzzleDone(ITEM_INFO* item, short item_num)
{
	if (item->object_number == PUZZLE_HOLE1 && gfCurrentLevel == LVL5_GALLOWS_TREE)
	{
		IsAtmospherePlaying = 0;
		S_CDPlay(45, 0);
		SoundEffect(SFX_HANGMAN_LAUGH_OFFCAM, &item->pos, SFX_DEFAULT);
	}

	item->object_number += 8;//puzzle hole to done, because there's 8 of them
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = item->anim_number;
	item->goal_anim_state = item->anim_number;
	item->required_anim_state = 0;
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

long KeyTrigger(short item_num)
{
	ITEM_INFO* item;
	long oldkey;

	item = &items[item_num];

	if ((item->status != ITEM_ACTIVE || lara.gun_status == LG_HANDS_BUSY) && (!KeyTriggerActive || lara.gun_status != LG_HANDS_BUSY))
		return -1;

	oldkey = KeyTriggerActive;

	if (!KeyTriggerActive)
		item->status = ITEM_DEACTIVATED;

	KeyTriggerActive = 0;
	return oldkey;
}

long PickupTrigger(short item_num)
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

	if (item->trigger_flags < 0)
		PuzzleType = 1;
	else if (item->trigger_flags > 1024)
		PuzzleType = 2;
	else if (item->trigger_flags && item->trigger_flags != 999 && item->trigger_flags != 998)
		PuzzleType = 3;

	if (((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) &&
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

					if (angle > 0x2300 && angle <= 0x5D00)
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
				GLOBAL_inventoryitemchosen = NO_ITEM;
				return;
			}

			if (!lara.IsMoving)
			{		
				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(short(hole + PUZZLE_ITEM1)))
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
				remove_inventory_item(short(hole + PUZZLE_ITEM1));

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
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
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
	else if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_USEPUZZLE && l->frame_number == anims[134].frame_base + 80 && item->item_flags[0])
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

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS &&
		(item->status == ITEM_INACTIVE && ObjNum != 3 || !item->item_flags[0]) || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
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
				l->anim_number = SearchAnims[ObjNum];
				l->frame_number = anims[l->anim_number].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
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
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
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

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS &&
		item->status == ITEM_INACTIVE || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
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
				l->anim_number = ANIM_ONEHANDPUSHSW;
				l->frame_number = anims[l->anim_number].frame_base;
				l->current_anim_state = AS_SWITCHON;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
				item->flags |= IFL_TRIGGERED;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
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
				item->trigger_flags &= ~0x3F;
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

				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(short(hole + KEY_ITEM1)))
						GLOBAL_enterinventory = hole + KEY_ITEM1;

					return;
				}

				if (key != hole)
					return;
			}

			if (MoveLaraPosition(&KeyHolePosition, item, l))
			{
				if (item->object_number != KEY_HOLE8)
					remove_inventory_item(short(hole + KEY_ITEM1));

				if (item->object_number == KEY_HOLE8)
					l->anim_number = ANIM_KEYCARD;
				else
					l->anim_number = ANIM_USEKEY;

				l->frame_number = anims[l->anim_number].frame_base;
				l->current_anim_state = AS_USEKEY;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
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

static short* FindPlinth(ITEM_INFO* item)
{
	ITEM_INFO* plinth;
	ROOM_INFO* r;
	MESH_INFO* mesh;
	short* p;
	short* o;
	long lp;
	short item_num;
	
	o = 0;
	r = &room[item->room_number];
	mesh = r->mesh;

	for (lp = r->num_meshes; lp > 0; lp--)
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

	if (lp)
		return o;

	item_num = r->item_number;

	while (item_num != NO_ITEM)
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
	}

	if (item_num == NO_ITEM)
		return 0;

	return GetBestFrame(&items[item_num]);
}

void PickUpCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* itemme;
	short* bounds;
	long flag;
	short rotx, roty, rotz, ocb;

	item = &items[item_number];
	ocb = item->trigger_flags & 0x3F;

	if (item->status == ITEM_INVISIBLE || ocb == 5 || ocb == 10)
		return;

	if (item->object_number == FLARE_ITEM && lara.gun_type == WEAPON_FLARE)
		return;

	rotx = item->pos.x_rot;
	roty = item->pos.y_rot;
	rotz = item->pos.z_rot;
	item->pos.y_rot = l->pos.y_rot;
	item->pos.z_rot = 0;

	if (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE)
	{
		if ((input & IN_ACTION || (GLOBAL_inventoryitemchosen != NO_ITEM && ocb == 2)) && !BinocularRange &&
			((l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS) ||
				(l->current_anim_state == AS_DUCK && l->anim_number == ANIM_DUCKBREATHE && lara.gun_status == LG_NO_ARMS) ||
				(l->current_anim_state == AS_ALL4S && l->anim_number == ANIM_ALL4S)) ||
			lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			flag = 0;
			item->pos.x_rot = 0;

			switch (ocb)
			{
			case 1:

				if (!lara.IsDucked && TestLaraPosition(HiddenPickUpBounds, item, l))
				{
					if (MoveLaraPosition(&HiddenPickUpPosition, item, l))
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			case 2:
				item->pos.y_rot = roty;

				if (!lara.IsDucked && TestLaraPosition(CrowbarPickUpBounds, item, l))
				{
					if (!lara.IsMoving)
					{
						if (GLOBAL_inventoryitemchosen == NO_ITEM)
						{
							if (have_i_got_object(CROWBAR_ITEM))
								GLOBAL_enterinventory = CROWBAR_ITEM;

							break;
						}

						if (GLOBAL_inventoryitemchosen != CROWBAR_ITEM)
							break;

						GLOBAL_inventoryitemchosen = NO_ITEM;
					}

					if (MoveLaraPosition(&CrowbarPickUpPosition, item, l))
					{
						l->anim_number = ANIM_CROWBAR;
						l->current_anim_state = AS_PICKUP;
						item->status = ITEM_ACTIVE;
						AddActiveItem(item_number);
						AnimateItem(item);
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			case 3:
			case 4:
			case 7:
			case 8:
				bounds = FindPlinth(item);

				if (!bounds)
					break;

				PlinthPickUpBounds[0] = bounds[0];
				PlinthPickUpBounds[1] = bounds[1];
				PlinthPickUpBounds[5] = bounds[5] + 320;
				PlinthPickUpPosition.z = -200 - bounds[5];
				PlinthPickUpBounds[3] = (short)(l->pos.y_pos - item->pos.y_pos + 100);

				if (TestLaraPosition(PlinthPickUpBounds, item, l) && !lara.IsDucked)
				{
					if (item->pos.y_pos == l->pos.y_pos)
						PlinthPickUpPosition.y = 0;
					else
						PlinthPickUpPosition.y = l->pos.y_pos - item->pos.y_pos;

					if (MoveLaraPosition(&PlinthPickUpPosition, item, l))
					{
						if (ocb == 3 || ocb == 7)
						{
							l->anim_number = ANIM_PLINTHHI;
							l->current_anim_state = AS_PICKUP;
						}
						else
						{
							l->anim_number = ANIM_PLINTHLO;
							l->current_anim_state = AS_PICKUP;
						}

						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			case 9:
				item->pos.y_rot = roty;

				if (TestLaraPosition(JobyCrowPickUpBounds, item, l))
				{
					if (MoveLaraPosition(&JobyCrowPickUpPosition, item, l))
					{
						l->anim_number = ANIM_JOBYCROW;
						l->current_anim_state = AS_PICKUP;
						item->status = ITEM_ACTIVE;
						AddActiveItem(item_number);
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}

				break;

			default:

				if (TestLaraPosition(PickUpBounds, item, l))
				{
					PickUpPosition.y = l->pos.y_pos - item->pos.y_pos;

					if (l->current_anim_state == AS_DUCK)
					{
						AlignLaraPosition(&PickUpPosition, item, l);

						if (item->object_number == FLARE_ITEM)
						{
							l->anim_number = ANIM_DUCKPICKUPF;
							l->current_anim_state = AS_FLAREPICKUP;
						}
						else
						{
							l->anim_number = ANIM_DUCKPICKUP;
							l->current_anim_state = AS_PICKUP;
						}

						flag = 1;
					}
					else if (l->current_anim_state == AS_ALL4S)
						l->goal_anim_state = AS_DUCK;
					else if (MoveLaraPosition(&PickUpPosition, item, l))
					{
						if (item->object_number == FLARE_ITEM)
						{
							l->anim_number = ANIM_PICKUPF;
							l->current_anim_state = AS_FLAREPICKUP;
						}
						else
						{
							l->anim_number = ANIM_PICKUP;
							l->current_anim_state = AS_PICKUP;
						}

						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;
			}

			if (flag)
			{
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && (l->current_anim_state == AS_PICKUP || l->current_anim_state == AS_HIDDENPICKUP))
		{
			if (l->frame_number == anims[ANIM_PICKUP].frame_base + 15 ||
				l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 22 ||
				l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 20 ||
				l->frame_number == anims[ANIM_PLINTHLO].frame_base + 29 ||
				l->frame_number == anims[ANIM_PLINTHHI].frame_base + 45 ||
				l->frame_number == anims[ANIM_HIDDENPICKUP].frame_base + 42 ||
				l->frame_number == anims[ANIM_JOBYCROW].frame_base + 183 ||
				(l->anim_number == ANIM_CROWBAR && l->frame_number == anims[ANIM_CROWBAR].frame_base + 123))
			{
				if (item->object_number == BURNING_TORCH_ITEM)
				{
					AddDisplayPickup(BURNING_TORCH_ITEM);
					GetFlameTorch();
					lara.LitTorch = item->item_flags[3] & 1;
					KillItem(item_number);
				}
				else
				{
					if (item->object_number != FLARE_ITEM)
					{
						AddDisplayPickup(item->object_number);

						if (item->trigger_flags & 0x100)
						{
							for (int i = 0; i < level_items; i++)
							{
								itemme = &items[i];

								if (itemme->object_number == item->object_number)
									KillItem(i);
							}
						}
					}

					if (!(item->trigger_flags & 0xC0))
						KillItem(item_number);
					else
					{
						item->item_flags[3] = 1;
						item->flags |= IFL_TRIGGERED;
						item->status = ITEM_INVISIBLE;
					}
				}
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_FLAREPICKUP)
		{
			if ((l->anim_number == ANIM_DUCKPICKUPF && l->frame_number == anims[ANIM_DUCKPICKUPF].frame_base + 22) ||
				l->frame_number == anims[ANIM_PICKUPF].frame_base + 58)
			{
				lara.request_gun_type = 7;
				lara.gun_type = 7;
				InitialiseNewWeapon();
				lara.gun_status = 5;
				lara.flare_age = (long)(item->data) & 0x7FFF;
				KillItem(item_number);
			}
		}
	}
	else if (lara.water_status == LW_UNDERWATER)
	{
		item->pos.x_rot = -4550;

		if (input & IN_ACTION && item->object_number != BURNING_TORCH_ITEM && l->current_anim_state == AS_TREAD &&
			lara.gun_status == LG_NO_ARMS && TestLaraPosition(PickUpBoundsUW, item, l) ||
			lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			if (TestLaraPosition(PickUpBoundsUW, item, l))
			{
				if (MoveLaraPosition(&PickUpPositionUW, item, l))
				{
					if (item->object_number == FLARE_ITEM)
					{
						l->anim_number = ANIM_PICKUPF_UW;
						l->current_anim_state = AS_FLAREPICKUP;
						l->fallspeed = 0;
					}
					else
					{
						l->anim_number = ANIM_PICKUP_UW;
						l->current_anim_state = AS_PICKUP;
					}

					l->frame_number = anims[l->anim_number].frame_base;
					l->goal_anim_state = AS_TREAD;
					lara.IsMoving = 0;
					lara.gun_status = LG_HANDS_BUSY;
				}

				lara.GeneralPtr = (void*)item_number;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_PICKUP && l->frame_number == anims[ANIM_PICKUP_UW].frame_base + 18)
		{
			AddDisplayPickup(item->object_number);

			if (!(item->trigger_flags & 0xC0))
				KillItem(item_number);
			else
			{
				item->item_flags[3] = 1;
				item->flags |= IFL_TRIGGERED;
				item->status = ITEM_INVISIBLE;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_FLAREPICKUP && l->frame_number == anims[ANIM_PICKUPF_UW].frame_base + 20)
		{
			lara.request_gun_type = WEAPON_FLARE;
			lara.gun_type = WEAPON_FLARE;
			InitialiseNewWeapon();
			lara.gun_status = LG_FLARE;
			lara.flare_age = (long)(item->data) & 0x7FFF;
			draw_flare_meshes();
			KillItem(item_number);
		}
	}

	item->pos.x_rot = rotx;
	item->pos.y_rot = roty;
	item->pos.z_rot = rotz;
}

void CollectCarriedItems(ITEM_INFO* item)
{
	ITEM_INFO* pickup;
	short pickup_number;

	pickup_number = item->carried_item;

	while (pickup_number != NO_ITEM)
	{
		pickup = &items[pickup_number];
		AddDisplayPickup(pickup->object_number);
		KillItem(pickup_number);
		pickup_number = pickup->carried_item;
	}

	item->carried_item = NO_ITEM;
}
