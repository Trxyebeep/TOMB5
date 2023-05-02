#include "../tomb5/pch.h"
#include "door.h"
#include "draw.h"
#include "objects.h"
#include "control.h"
#include "gameflow.h"
#include "effects.h"
#include "sound.h"
#include "newinv2.h"
#include "collide.h"
#include "sphere.h"
#include "lara_states.h"
#include "items.h"
#include "switch.h"
#include "../specific/input.h"
#include "lara.h"
#include "box.h"
#include "lot.h"

long ClosedDoors[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static char LiftDoor = 0;

static short PushPullKickDoorBounds[12] = { -384, 384, 0, 0, -1024, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short UnderwaterDoorBounds[12] = { -256, 256, -1024, 0, -1024, 0, -14560, 14560, -14560, 14560, -14560, 14560 };
static short CrowbarDoorBounds[12] = { -512, 512, -1024, 0, 0, 512, -14560, 14560, -14560, 14560, -14560, 14560 };

static PHD_VECTOR DoubleDoorPos = { 0, 0, 220 };
static PHD_VECTOR PullDoorPos = { -201, 0, 322 };
static PHD_VECTOR PushDoorPos = { 201, 0, -702 };
static PHD_VECTOR KickDoorPos = { 0, 0, -917 };
static PHD_VECTOR UnderwaterDoorPos = { -251, -540, -46 };
static PHD_VECTOR CrowbarDoorPos = { -412, 0, 140 };

void ShutThatDoor(DOORPOS_DATA* d, DOOR_DATA* dd)
{
	CREATURE_INFO* cinfo;
	short* dptr;

	if (d->floor)
	{
		d->floor->box = -1;
		d->floor->ceiling = -127;
		d->floor->floor = -127;
		d->floor->index = 0;
		d->floor->pit_room = NO_ROOM;
		d->floor->sky_room = NO_ROOM;

		if (d->block != 2047)
		{
			boxes[d->block].overlap_index |= 0x4000;

			for (short slot = 0; slot < MAX_LOT; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}

	dptr = dd->dptr1;

	if (dptr)
	{
		dd->dptr1[0] = 0;
		dd->dptr1[1] = 0;
		dd->dptr1[2] = 0;
		dd->dptr3[0] = 0;
		dd->dptr3[1] = 0;
		dd->dptr3[2] = 0;
		dptr = dd->dptr2;

		if (dptr)
		{
			dd->dptr2[0] = 0;
			dd->dptr2[1] = 0;
			dd->dptr2[2] = 0;
		}

		dptr = dd->dptr4;

		if (dptr)
		{
			dd->dptr4[0] = 0;
			dd->dptr4[1] = 0;
			dd->dptr4[2] = 0;
		}
	}
}

void OpenThatDoor(DOORPOS_DATA* d, DOOR_DATA* dd)
{
	CREATURE_INFO* cinfo;
	short* dptr;
	short v;

	if (d->floor)
	{
		*d->floor = d->data;

		if (d->block != 2047)
		{
			if (!LiftDoor)
				boxes[d->block].overlap_index &= 0xBFFF;

			for (short slot = 0; slot < MAX_LOT; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}

	dptr = dd->dptr1;

	if (dptr)
	{
		v = dd->dn1 < 0 ? -1 : 1;

		if (dd->dn1 & 1)
			dd->dptr1[0] = v;
		else if (dd->dn1 & 2)
			dd->dptr1[1] = v;
		else
			dd->dptr1[2] = v;

		v = dd->dn3 < 0 ? -1 : 1;

		if (dd->dn3 & 1)
			dd->dptr3[0] = v;
		else if (dd->dn3 & 2)
			dd->dptr3[1] = v;
		else
			dd->dptr3[2] = v;

		if (dd->dptr2)
		{
			v = dd->dn2 < 0 ? -1 : 1;

			if (dd->dn2 & 1)
				dd->dptr2[0] = v;
			else if (dd->dn2 & 2)
				dd->dptr2[1] = v;
			else
				dd->dptr2[2] = v;
		}

		if (dd->dptr4)
		{
			v = dd->dn4 < 0 ? -1 : 1;

			if (dd->dn4 & 1)
				dd->dptr4[0] = v;
			else if (dd->dn4 & 2)
				dd->dptr4[1] = v;
			else
				dd->dptr4[2] = v;
		}
	}
}

void DoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;
	long bound;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (item->trigger_flags == 1)	//cogwheel doors
	{
		if (item->item_flags[0])
		{
			item->item_flags[0]--;
			item->pos.y_pos -= 12;
			bound = GetBoundsAccurate(item)[2] + *(long*)&item->item_flags[2] - 256;

			if (item->pos.y_pos < bound)
			{
				item->pos.y_pos = bound;
				item->item_flags[0] = 0;
			}

			if (!door->Opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->Opened = 1;
			}
		}
		else
		{
			if (item->pos.y_pos < item->item_flags[2])
				item->pos.y_pos += 2;

			if (item->pos.y_pos >= item->item_flags[2])
			{
				item->pos.y_pos = item->item_flags[2];

				if (door->Opened)
				{
					ShutThatDoor(&door->d1, door);
					ShutThatDoor(&door->d2, door);
					ShutThatDoor(&door->d1flip, door);
					ShutThatDoor(&door->d2flip, door);
					door->Opened = 0;
				}
			}
		}

		return;
	}

	if (item->object_number >= LIFT_DOORS1 && item->object_number <= LIFT_DOORS2)
	{
		if (TriggerActive(item))
		{
			if (item->item_flags[0] > 0)
			{
				if (item->item_flags[0] == 4096)
					SoundEffect(SFX_LIFT_DOORS, &item->pos, SFX_DEFAULT);

				item->item_flags[0] -= 256;
			}

			if (!door->Opened)
			{
				LiftDoor = 1;
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				LiftDoor = 0;
				door->Opened = 1;
			}
		}
		else
		{
			if (item->item_flags[0] >= 4096)
			{
				if (door->Opened)
				{
					ShutThatDoor(&door->d1, door);
					ShutThatDoor(&door->d2, door);
					ShutThatDoor(&door->d1flip, door);
					ShutThatDoor(&door->d2flip, door);
					door->Opened = 0;
				}
			}
			else
			{
				if (!item->item_flags[0])
					SoundEffect(SFX_LIFT_DOORS, &item->pos, SFX_DEFAULT);

				item->item_flags[0] += 256;
			}
		}

		return;
	}


	if (TriggerActive(item))
	{
		if (item->current_anim_state == 0)
			item->goal_anim_state = 1;
		else
		{
			if (!door->Opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->Opened = 1;
			}

			if (item->frame_number == anims[item->anim_number].frame_end)
			{
				if ((gfCurrentLevel == LVL5_THIRTEENTH_FLOOR && (item->object_number == CLOSED_DOOR2 || item->object_number == CLOSED_DOOR3)) ||
					(gfCurrentLevel >= LVL5_ESCAPE_WITH_THE_IRIS && gfCurrentLevel <= LVL5_RED_ALERT && item->object_number == CLOSED_DOOR1))
					item->status = ITEM_INVISIBLE;
			}
		}
	}
	else
	{
		item->status = ITEM_ACTIVE;

		if (item->current_anim_state == 1)
			item->goal_anim_state = 0;
		else if (door->Opened)
		{
			ShutThatDoor(&door->d1, door);
			ShutThatDoor(&door->d2, door);
			ShutThatDoor(&door->d1flip, door);
			ShutThatDoor(&door->d2flip, door);
			door->Opened = 0;
		}
	}

	AnimateItem(item);
}

void DoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (item->trigger_flags == 2 && !(item->status && item->gravity_status) &&
		((input & IN_ACTION || GLOBAL_inventoryitemchosen == CROWBAR_ITEM) &&
			l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->hit_status && lara.gun_status == LG_NO_ARMS ||
			lara.IsMoving && lara.GeneralPtr == (void*)item_num))
	{
		item->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(CrowbarDoorBounds, item, l))
		{
			if (!lara.IsMoving)
			{
				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(CROWBAR_ITEM))
						GLOBAL_enterinventory = CROWBAR_ITEM;
					else if (OldPickupPos.x != l->pos.x_pos || OldPickupPos.y != l->pos.y_pos || OldPickupPos.z != l->pos.z_pos)
					{
						OldPickupPos.x = l->pos.x_pos;
						OldPickupPos.y = l->pos.y_pos;
						OldPickupPos.z = l->pos.z_pos;
						SayNo();
					}

					item->pos.y_rot ^= 0x8000;
					return;
				}

				if (GLOBAL_inventoryitemchosen != CROWBAR_ITEM)
				{
					item->pos.y_rot ^= 0x8000;
					return;
				}
			}

			GLOBAL_inventoryitemchosen = NO_ITEM;

			if (MoveLaraPosition(&CrowbarDoorPos, item, l))
			{
				l->anim_number = ANIM_CROWBARDOOR;
				l->current_anim_state = AS_CONTROLLED;
				l->frame_number = anims[ANIM_CROWBARDOOR].frame_base;
				item->pos.y_rot ^= 0x8000;
				AddActiveItem(item_num);
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
				item->goal_anim_state = 1;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				return;
			}

			lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot ^= 0x8000;
	}

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
	{
		if (item->object_number < LIFT_DOORS1 || item->object_number > LIFT_DOORS2 || item->item_flags[0])
			ItemPushLara(item, l, coll, 0, 1);
	}
}

void PushPullKickDoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (!door->Opened)
	{
		OpenThatDoor(&door->d1, door);
		OpenThatDoor(&door->d2, door);
		OpenThatDoor(&door->d1flip, door);
		OpenThatDoor(&door->d2flip, door);
		door->Opened = 1;
	}

	AnimateItem(item);
}

void PushPullKickDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long flag;

	item = &items[item_num];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE &&
		!l->gravity_status && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		flag = 0;

		if (l->room_number == item->room_number)
		{
			item->pos.y_rot ^= 0x8000;
			flag = 1;
		}

		if (!TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			if (flag)
				item->pos.y_rot ^= 0x8000;

			return;
		}

		if (flag)
		{
			if (MoveLaraPosition(&PullDoorPos, item, l))
			{
				l->anim_number = ANIM_PULLDOOR;
				l->frame_number = anims[ANIM_PULLDOOR].frame_base;
				item->goal_anim_state = 3;
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				l->current_anim_state = AS_CONTROLLED;
				l->goal_anim_state = AS_STOP;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;

				if (flag)
					item->pos.y_rot ^= 0x8000;

				return;
			}
			else
			{
				lara.GeneralPtr = (void*)item_num;

				if (flag)
					item->pos.y_rot ^= 0x8000;

				return;
			}
		}

		if (item->object_number < KICK_DOOR1)
		{
			if (MoveLaraPosition(&PushDoorPos, item, l))
			{
				l->anim_number = ANIM_PUSHDOOR;
				l->frame_number = anims[ANIM_PUSHDOOR].frame_base;
				item->goal_anim_state = 2;
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				l->current_anim_state = AS_CONTROLLED;
				l->goal_anim_state = AS_STOP;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;

				if (flag)
					item->pos.y_rot ^= 0x8000;

				return;
			}
		}
		else if (MoveLaraPosition(&KickDoorPos, item, l))
		{
			l->anim_number = ANIM_KICKDOOR;
			l->frame_number = anims[ANIM_KICKDOOR].frame_base;
			item->goal_anim_state = 2;
			AddActiveItem(item_num);
			item->status = ITEM_ACTIVE;
			l->current_anim_state = AS_CONTROLLED;
			l->goal_anim_state = AS_STOP;
			lara.IsMoving = 0;
			lara.gun_status = LG_HANDS_BUSY;

			if (flag)
				item->pos.y_rot ^= 0x8000;

			return;
		}

		lara.GeneralPtr = (void*)item_num;
		return;
	}

	if (item->current_anim_state == 0)
		DoorCollision(item_num, l, coll);
}

void DoubleDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE &&
		!l->gravity_status && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		item->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			if (MoveLaraPosition(&DoubleDoorPos, item, l))
			{
				l->anim_number = ANIM_TWODOOR;
				l->frame_number = anims[ANIM_TWODOOR].frame_base;
				l->current_anim_state = AS_TWODOOR;
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
			}
			else
				lara.GeneralPtr = (void*)item_num;

			item->pos.y_rot ^= 0x8000;
		}
		else
		{
			if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			item->pos.y_rot ^= 0x8000;
		}
	}
}

void UnderwaterDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_TREAD && lara.water_status == LW_UNDERWATER &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		l->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(UnderwaterDoorBounds, item, l))
		{
			if (MoveLaraPosition(&UnderwaterDoorPos, item, l))
			{
				l->anim_number = ANIM_WATERDOOR;
				l->frame_number = anims[ANIM_WATERDOOR].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				l->fallspeed = 0;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_num);
				item->goal_anim_state = 1;
				AnimateItem(item);
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_num;

			l->pos.y_rot ^= 0x8000;
		}
		else
		{
			if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			l->pos.y_rot ^= 0x8000;
		}
	}
	else if (item->status == ITEM_ACTIVE)
		ObjectCollision(item_num, l, coll);
}

void SequenceDoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (CurrentSequence == 3)
	{
		if (SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]] == item->trigger_flags)
		{
			if (item->current_anim_state == 0)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 0;

			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
		}

		CurrentSequence = 4;
	}

	if (item->current_anim_state == item->goal_anim_state)
	{
		if (item->current_anim_state == 1)
		{
			if (!door->Opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->Opened = 1;
				item->flags |= IFL_CODEBITS;
			}
		}
		else if (door->Opened)
		{
			ShutThatDoor(&door->d1, door);
			ShutThatDoor(&door->d2, door);
			ShutThatDoor(&door->d1flip, door);
			ShutThatDoor(&door->d2flip, door);
			door->Opened = 0;
			item->flags &= ~IFL_CODEBITS;
		}
	}

	AnimateItem(item);
}

void ProcessClosedDoors()
{
	ITEM_INFO* item;
	short room_number;

	for (int i = 0; i < 32; i++)
	{		
		item = (ITEM_INFO*)ClosedDoors[i];

		if (!item)
			return;

		room_number = item->room_number;

		if (!room[room_number].bound_active && !room[item->draw_room].bound_active)
			continue;

		if (room[item->draw_room].bound_active)
		{
			if (!item->InDrawRoom)
			{
				ItemNewRoom(item - items, item->draw_room);
				item->room_number = room_number;
				item->InDrawRoom = 1;
			}
		}
		else if (item->InDrawRoom)
		{
			item->room_number = item->draw_room;
			ItemNewRoom(item - items, room_number);
			item->InDrawRoom = 0;
		}
	}
}
