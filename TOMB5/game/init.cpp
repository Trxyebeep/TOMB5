#include "../tomb5/pch.h"
#include "init.h"
#include "objects.h"
#include "gameflow.h"
#include "draw.h"
#include "traps.h"
#include "control.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "door.h"

void InitialiseTrapDoor(short item_number)
{
	CloseTrapDoor(&items[item_number]);
}

void InitialiseFallingBlock2(short item_number)
{
	items[item_number].mesh_bits = 1;
}

void InitialiseFlameEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags == 33)
	{
		if (item->pos.y_rot == 0)
			item->pos.z_pos += 144;
		else if (item->pos.y_rot == 16384)
			item->pos.x_pos += 144;
		else if (item->pos.y_rot == -32768)
			item->pos.z_pos -= 144;
		else if (item->pos.y_rot == -16384)
			item->pos.x_pos -= 144;

		item->pos.y_pos += 32;
	}
	else if (item->trigger_flags < 0)
	{
		item->item_flags[0] = (GetRandomControl() & 0x3F) + 90;
		item->item_flags[2] = 256;

		if ((-item->trigger_flags & 7) == 7)
		{
			if (item->pos.y_rot == 0)
				item->pos.z_pos += 512;
			else if (item->pos.y_rot == 16384)
				item->pos.x_pos += 512;
			else if (item->pos.y_rot == -32768)
				item->pos.z_pos -= 512;
			else if (item->pos.y_rot == -16384)
				item->pos.x_pos -= 512;
		}
	}
}

void InitialiseFlameEmitter2(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->pos.y_pos -= 64;

	if (item->trigger_flags != 123)
	{
		if (item->pos.y_rot == 0)
		{
			if (item->trigger_flags == 2)
				item->pos.z_pos += 80;
			else
				item->pos.z_pos += 256;
		}
		else if (item->pos.y_rot == 16384)
		{
			if (item->trigger_flags == 2)
				item->pos.x_pos += 80;
			else
				item->pos.x_pos += 256;
		}
		else if (item->pos.y_rot == -32768)
		{
			if (item->trigger_flags == 2)
				item->pos.z_pos -= 80;
			else
				item->pos.z_pos -= 256;
		}
		else if (item->pos.y_rot == -16384)
		{
			if (item->trigger_flags == 2)
				item->pos.x_pos -= 80;
			else
				item->pos.x_pos -= 256;
		}
	}
}

void InitialiseTwoBlockPlatform(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[0] = (short)item->pos.y_pos;
	item->item_flags[1] = 1;
}

void InitialiseScaledSpike(short item_number)
{
	ITEM_INFO* item;
	short xzrots[8];

	item = &items[item_number];
	xzrots[0] = -0x8000;
	xzrots[1] = -24576;
	xzrots[2] = -16384;
	xzrots[3] = -8192;
	xzrots[4] = 0;
	xzrots[5] = 0x2000;
	xzrots[6] = 0x4000;
	xzrots[7] = 24576;
	item->status = ITEM_INVISIBLE;

	if (item->trigger_flags & 8)
	{
		item->pos.x_rot = xzrots[item->trigger_flags & 7];
		item->pos.y_rot = 0x4000;
		item->pos.z_pos-= SPxzoffs[item->trigger_flags & 7];
	}
	else
	{
		item->pos.z_rot = xzrots[item->trigger_flags & 7];
		item->pos.x_pos += SPxzoffs[item->trigger_flags & 7];
	}

	item->item_flags[0] = 1024;
	item->item_flags[2] = 0;
	item->pos.y_pos += SPyoffs[item->trigger_flags & 7];
}

void InitialiseRaisingBlock(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	short room_num;

	item = &items[item_number];
	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	boxes[floor->box].overlap_index &= 0xBF;

	if (item->trigger_flags < 0)
	{
		item->flags |= IFLAG_ACTIVATION_MASK;
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
	}
}

void InitialiseSmashObject(short item_number)
{
	ITEM_INFO* item;
	room_info* rinfo;
	FLOOR_INFO* floor;

	item = &items[item_number];
	item->flags = 0;
	item->mesh_bits = 1;
	rinfo = &room[item->room_number];
	floor = &rinfo->floor[((item->pos.z_pos - rinfo->z) >> 10) + ((item->pos.x_pos - rinfo->x) >> 10) * rinfo->x_size];

	if (boxes[floor->box].overlap_index & 0x8000)
		boxes[floor->box].overlap_index |= 0x4000;
}

void InitialiseEffects()
{
	memset(spark, 0, sizeof(SPARKS) * 1024);
	memset(fire_spark, 0, sizeof(FIRE_SPARKS) * 20);
	memset(smoke_spark, 0, sizeof(SMOKE_SPARKS) * 32);
	memset(Gunshells, 0, sizeof(GUNSHELL_STRUCT) * 24);
	memset(Gunflashes, 0, sizeof(GUNFLASH_STRUCT) * 4);
	memset(debris, 0, sizeof(DEBRIS_STRUCT) * 32);
	memset(blood, 0, sizeof(BLOOD_STRUCT) * 32);
	memset(splashes, 0, sizeof(SPLASH_STRUCT) * 4);
	memset(ripples, 0, sizeof(RIPPLE_STRUCT) * 32);
	memset(Bubbles, 0, sizeof(BUBBLE_STRUCT) * 40);
	memset(Drips, 0, sizeof(DRIP_STRUCT) * 32);
	memset(ShockWaves, 0, sizeof(SHOCKWAVE_STRUCT) * 16);

	for (int i = 0; i < 1024; i++)
		spark[i].Dynamic = -1;

	next_fire_spark = 1;
	next_smoke_spark = 0;
	next_gunshell = 0;
	next_bubble = 0;
	next_drip = 0;
	next_debris = 0;
	next_blood = 0;
	WB_room = -1;
}

void InitialiseSmokeEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags == 111)
	{
		if (item->pos.y_rot == 0)
			item->pos.z_pos += 512;
		else if (item->pos.y_rot == 16384)
			item->pos.x_pos += 512;
		else if (item->pos.y_rot == -16384)
			item->pos.x_pos -= 512;
		else if (item->pos.y_rot == -32768)
			item->pos.z_pos -= 512;
	}
	else
	{
		if (item->object_number != STEAM_EMITTER)
			return;

		if (item->trigger_flags & 8)
		{
			item->item_flags[0] = item->trigger_flags >> 4;

			if (item->pos.y_rot == 0)
				item->pos.z_pos += 256;
			else if (item->pos.y_rot == 16384)
				item->pos.x_pos += 256;
			else if (item->pos.y_rot == -16384)
				item->pos.x_pos -= 256;
			else if (item->pos.y_rot == -32768)
				item->pos.z_pos -= 256;

			if (item->item_flags[0] <= 0)
			{
				item->item_flags[2] = 4096;
				item->trigger_flags |= 4;
			}
		}
		else if (room[item->room_number].flags & RF_FILL_WATER && item->trigger_flags == 1)
		{
			item->item_flags[0] = 20;
			item->item_flags[1] = 1;
		}
	}
}

void InitialiseDoor(short item_number)
{
	ITEM_INFO* item;
	room_info* r;
	room_info* b;
	DOOR_DATA* door;
	int dx, dy;
	short two_room, box_number, room_number;

	item = &items[item_number];

	if (item->object_number == SEQUENCE_DOOR1)
		item->flags &= 0xBFFF;

	if (item->object_number >= LIFT_DOORS1 && item->object_number <= LIFT_DOORS2)
		item->item_flags[0] = 4096;

	door = (DOOR_DATA*)game_malloc(sizeof(DOOR_DATA));
	item->data = door;
	door->Opened = 0;
	door->dptr1 = 0;
	door->dptr2 = 0;
	door->dptr3 = 0;
	door->dptr4 = 0;

	dx = 0;
	dy = 0;

	if (item->pos.y_rot == 0)
		dx = -1;
	else if (item->pos.y_rot == -32768)
		dx = 1;
	else if (item->pos.y_rot == 16384)
		dy = -1;
	else
		dy = 1;

	r = &room[item->room_number];
	door->d1.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
	room_number = GetDoor(door->d1.floor);

	if (room_number == 255)
		box_number = door->d1.floor->box;
	else
	{
		b = &room[room_number];
		box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
	}

	door->d1.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
	memcpy(&door->d1.data, door->d1.floor, sizeof(FLOOR_INFO));

	if (r->flipped_room == -1)
		door->d1flip.floor = 0;
	else
	{
		r = &room[r->flipped_room];
		door->d1flip.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
		room_number = GetDoor(door->d1flip.floor);

		if (room_number == 255)
			box_number = door->d1flip.floor->box;
		else
		{
			b = &room[room_number];
			box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
		}

		door->d1flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
		memcpy(&door->d1flip.data, door->d1flip.floor, sizeof(FLOOR_INFO));
	}

	two_room = GetDoor(door->d1.floor);
	ShutThatDoor(&door->d1, door);
	ShutThatDoor(&door->d1flip, door);

	if (two_room == 255)
	{
		door->d2.floor = 0;
		door->d2flip.floor = 0;
	}
	else
	{
		r = &room[two_room];
		door->d2.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
		room_number = GetDoor(door->d2.floor);

		if (room_number == 255)
			box_number = door->d2.floor->box;
		else
		{
			b = &room[room_number];
			box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
		}

		door->d2.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
		memcpy(&door->d2.data, door->d2.floor, sizeof(FLOOR_INFO));

		if (r->flipped_room == -1)
			door->d2flip.floor = 0;
		else
		{
			r = &room[r->flipped_room];
			door->d2flip.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
			room_number = GetDoor(door->d2flip.floor);

			if (room_number == 255)
				box_number = door->d2flip.floor->box;
			else
			{
				b = &room[room_number];
				box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
			}

			door->d2flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
			memcpy(&door->d2flip.data, door->d2flip.floor, sizeof(FLOOR_INFO));
		}

		ShutThatDoor(&door->d2, door);
		ShutThatDoor(&door->d2flip, door);
		room_number = item->room_number;
		ItemNewRoom(item_number, two_room);
		item->room_number = room_number;
		item->draw_room = two_room;
		item->InDrawRoom = 1;

		if (item->object_number >= CLOSED_DOOR1 && item->object_number <= LIFT_DOORS2)
		{
			short* dptr;

			SetupClosedDoorStuff(door, item, two_room, dx, dy);

			dptr = door->dptr1;
			dptr[0] = 0;
			dptr[1] = 0;
			dptr[2] = 0;
			dptr = door->dptr3;
			dptr[0] = 0;
			dptr[1] = 0;
			dptr[2] = 0;

			if (room[item->room_number].flipped_room != -1)
			{
				dptr = door->dptr2;

				if (!dptr)
					*(long*)0x00000001 = 1;//good job

				dptr[0] = 0;
				dptr[1] = 0;
				dptr[2] = 0;
			}

			if (room[item->draw_room].flipped_room != -1)
			{
				dptr = door->dptr4;

				if (!dptr)
					*(long*)0x00000001 = 1;

				dptr[0] = 0;
				dptr[1] = 0;
				dptr[2] = 0;
			}

			door->item = item;
			AddClosedDoor(item);
		}
	}
}

void InitialisePulley(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[3] = item->trigger_flags;
	item->trigger_flags = ABS(item->trigger_flags);

	if (item_number == GlobalPulleyFrigItem)
		item->item_flags[1] = 1;
}

void InitialisePickup(short item_number)
{
	ITEM_INFO* item;
	short* bounds;
	short ocb;

	item = &items[item_number];
	ocb = item->trigger_flags & 0x3F;

	if (item->object_number == HK_ITEM && gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		return;

	bounds = GetBoundsAccurate(item);
	if (ocb == 6)
	{
		item->item_flags[0] = (short)(item->pos.y_pos - bounds[3]);
		item->status = ITEM_INVISIBLE;
	}
	else
	{
		if (ocb == 0 || ocb == 3 || ocb == 4 || ocb == 7 || ocb == 8 || ocb == 11)
			item->pos.y_pos -= bounds[3];

		if (item->trigger_flags & 128)
			RPickups[NumRPickups++] = (unsigned char)item_number;

		if (item->trigger_flags & 256)
			item->mesh_bits = 0;

		if (item->status == ITEM_INVISIBLE)
			item->flags |= IFLAG_TRIGGERED;
	}
}

void InitialiseClosedDoors()
{
	for (int i = 0; i < 32; i++)
		ClosedDoors[i] = 0;
}

void AddClosedDoor(ITEM_INFO* item)
{
	long	lp;

	for (lp = 0; lp < 32; lp++)
	{
		if (!ClosedDoors[lp])
		{
			ClosedDoors[lp] = (long)item;
			break;
		}
	}
}

void inject_init()
{
	INJECT(0x0043D2F0, InitialiseTrapDoor);
	INJECT(0x0043D330, InitialiseFallingBlock2);
	INJECT(0x0043D370, InitialiseFlameEmitter);
	INJECT(0x0043D4E0, InitialiseFlameEmitter2);
	INJECT(0x0043D5D0, InitialiseTwoBlockPlatform);
	INJECT(0x0043D610, InitialiseScaledSpike);
	INJECT(0x0043D730, InitialiseRaisingBlock);
	INJECT(0x0043D7F0, InitialiseSmashObject);
	INJECT(0x0043D8B0, InitialiseEffects);
	INJECT(0x0043D9D0, InitialiseSmokeEmitter);
	INJECT(0x0043DB60, InitialiseDoor);
	INJECT(0x0043E1F0, InitialisePulley);
	INJECT(0x0043E260, InitialisePickup);
	INJECT(0x0043E380, InitialiseClosedDoors);
	INJECT(0x0043E3B0, AddClosedDoor);
}
