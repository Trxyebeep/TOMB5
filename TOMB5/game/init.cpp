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
#include "deltapak.h"
#include "pickup.h"
#include "effects.h"
#include "spider.h"
#include "box.h"
#include "switch.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"
#include "joby.h"
#include "../specific/file.h"

static short GlobalPulleyFrigItem;

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
		if (!item->pos.y_rot)
			item->pos.z_pos += 144;
		else if (item->pos.y_rot == 0x4000)
			item->pos.x_pos += 144;
		else if (item->pos.y_rot == -0x8000)
			item->pos.z_pos -= 144;
		else if (item->pos.y_rot == -0x4000)
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
			else if (item->pos.y_rot == 0x4000)
				item->pos.x_pos += 512;
			else if (item->pos.y_rot == -0x8000)
				item->pos.z_pos -= 512;
			else if (item->pos.y_rot == -0x4000)
				item->pos.x_pos -= 512;
		}
	}
}

void InitialiseFlameEmitter2(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->pos.y_pos -= 64;

	if (item->trigger_flags == 123)
		return;

	if (!item->pos.y_rot)
	{
		if (item->trigger_flags == 2)
			item->pos.z_pos += 80;
		else
			item->pos.z_pos += 256;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		if (item->trigger_flags == 2)
			item->pos.x_pos += 80;
		else
			item->pos.x_pos += 256;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		if (item->trigger_flags == 2)
			item->pos.z_pos -= 80;
		else
			item->pos.z_pos -= 256;
	}
	else if (item->pos.y_rot == -0x4000)
	{
		if (item->trigger_flags == 2)
			item->pos.x_pos -= 80;
		else
			item->pos.x_pos -= 256;
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
	xzrots[1] = -0x6000;
	xzrots[2] = -0x4000;
	xzrots[3] = -0x2000;
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
		item->flags |= IFL_CODEBITS;
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
	}
}

void InitialiseSmashObject(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* rinfo;
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
	memset(debris, 0, sizeof(DEBRIS_STRUCT) * 256);
	memset(blood, 0, sizeof(BLOOD_STRUCT) * 32);
	memset(splashes, 0, sizeof(SPLASH_STRUCT) * 4);
	memset(ripples, 0, sizeof(RIPPLE_STRUCT) * MAX_RIPPLES);
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
		if (!item->pos.y_rot)
			item->pos.z_pos += 512;
		else if (item->pos.y_rot == 0x4000)
			item->pos.x_pos += 512;
		else if (item->pos.y_rot == -0x4000)
			item->pos.x_pos -= 512;
		else if (item->pos.y_rot == -0x8000)
			item->pos.z_pos -= 512;

		return;
	}

	if (item->object_number == STEAM_EMITTER)
	{
		if (item->trigger_flags & 8)
		{
			item->item_flags[0] = item->trigger_flags >> 4;

			if (!item->pos.y_rot)
				item->pos.z_pos += 256;
			else if (item->pos.y_rot == 0x4000)
				item->pos.x_pos += 256;
			else if (item->pos.y_rot == -0x4000)
				item->pos.x_pos -= 256;
			else if (item->pos.y_rot == -0x8000)
				item->pos.z_pos -= 256;

			if (item->item_flags[0] <= 0)
			{
				item->item_flags[2] = 4096;
				item->trigger_flags |= 4;
			}
		}
		else if (room[item->room_number].flags & ROOM_UNDERWATER && item->trigger_flags == 1)
		{
			item->item_flags[0] = 20;
			item->item_flags[1] = 1;
		}
	}
}

void InitialiseDoor(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	ROOM_INFO* b;
	DOOR_DATA* door;
	long dx, dy;
	short two_room, box_number, room_number;

	item = &items[item_number];

	if (gfCurrentLevel == LVL5_TITLE)	//someone thought it was an incredible idea to place two doors in the same place <3
										//this fixes the random lighting shift in the home title flyby sequence
	{
		if (item_number == 6)
			item->room_number = items[5].room_number;
	}

	if (item->object_number == SEQUENCE_DOOR1)
		item->flags &= ~IFL_REVERSE;

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

	if (!item->pos.y_rot)
		dx = -1;
	else if (item->pos.y_rot == -0x8000)
		dx = 1;
	else if (item->pos.y_rot == 0x4000)
		dy = -1;
	else
		dy = 1;

	r = &room[item->room_number];
	door->d1.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
	room_number = GetDoor(door->d1.floor);

	if (room_number == NO_ROOM)
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

		if (room_number == NO_ROOM)
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

	if (two_room == NO_ROOM)
	{
		door->d2.floor = 0;
		door->d2flip.floor = 0;
	}
	else
	{
		r = &room[two_room];
		door->d2.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
		room_number = GetDoor(door->d2.floor);

		if (room_number == NO_ROOM)
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

			if (room_number == NO_ROOM)
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
					*(long*)0x00000001 = 1;

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
	item->trigger_flags = abs(item->trigger_flags);

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

	if (ocb == 5)
	{
		item->item_flags[0] = short(item->pos.y_pos - bounds[3]);
		item->status = ITEM_INVISIBLE;
		return;
	}

	if (!ocb || ocb == 3 || ocb == 4 || ocb == 7 || ocb == 8 || ocb == 11)
		item->pos.y_pos -= bounds[3];

	if (item->trigger_flags & 0x80)
		RPickups[NumRPickups++] = (uchar)item_number;

	if (item->trigger_flags & 0x100)
		item->mesh_bits = 0;

	if (item->status == ITEM_INVISIBLE)
		item->flags |= IFL_TRIGGERED;
}

void InitialiseClosedDoors()
{
	for (int i = 0; i < 32; i++)
		ClosedDoors[i] = 0;
}

void AddClosedDoor(ITEM_INFO* item)
{
	for (int i = 0; i < 32; i++)
	{
		if (!ClosedDoors[i])
		{
			ClosedDoors[i] = (long)item;
			break;
		}
	}
}

void SetupClosedDoorStuff(DOOR_DATA* door, ITEM_INFO* item, short room2, long dx, long dy)
{
	ROOM_INFO* r;
	long ox, oz;

	dx <<= 10;
	dy <<= 10;
	oz = item->pos.z_pos + dx;
	ox = item->pos.x_pos + dy;
	r = &room[item->room_number];
	SCDS(r, &door->dptr1, &door->dn1, dx, dy, ox, oz);

	if (r->flipped_room != -1)
		SCDS(&room[r->flipped_room], &door->dptr2, &door->dn2, dx, dy, ox, oz);

	r = &room[room2];
	SCDS(r, &door->dptr3, &door->dn3, dx, dy, ox, oz);

	if (r->flipped_room != -1)
		SCDS(&room[r->flipped_room], &door->dptr4, &door->dn4, dx, dy, ox, oz);
}

void SCDS(ROOM_INFO* r, short** dptr, char* dn, long dx, long dy, long ox, long oz)
{
	short* d;
	long minx, maxx, minz, maxz, wx, wz;

	d = r->door;

	if (!d)
		return;
	
	for (int i = *d++; i > 0; i--, d += 15)
	{
		d++;
		minx = r->x + (dy >> 1) + ((d[3] + 128) & 0xFFFFFF00);
		maxx = r->x + (dy >> 1) + ((d[9] + 128) & 0xFFFFFF00);

		if (minx > maxx)
		{
			wx = minx;
			minx = maxx;
			maxx = wx;
		}

		minz = r->z + (dx >> 1) + ((d[5] + 128) & 0xFFFFFF00);
		maxz = r->z + (dx >> 1) + ((d[11] + 128) & 0xFFFFFF00);

		if (minz > maxz)
		{
			wz = minz;
			minz = maxz;
			maxz = wz;
		}

		if (ox >= minx && ox <= maxx && oz >= minz && oz <= maxz)
		{
			*dptr = d;

			if (*d)
				*dn = (d[0] & 0x80) | 1;
			else if (d[1])
				*dn = (d[1] & 0x80) | 2;
			else
				*dn = (d[2] & 0x80) | 4;
		}
	}
}

void InitialiseLasers(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	LASER_STRUCT* ls;
	long Xadd, Yadd, Zadd, width, height, Lh, Hadd;
	short room_num;

	item = &items[item_number];
	item->data = (LASER_STRUCT*)game_malloc(sizeof(LASER_STRUCT));
	width = (item->trigger_flags & 0xFF) << 10;

	if (!(item->trigger_flags & 1))
	{
		Xadd = (width / 2) - 512;
		item->pos.z_pos += (Xadd * phd_cos(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
		item->pos.x_pos += (Xadd * phd_sin(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
	}

	if ((item->trigger_flags & 255) == 1)
		item->item_flags[1] = 1;

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	item->item_flags[0] = short(item->pos.y_pos - GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos));
	height = item->item_flags[0];
	item->trigger_flags >>= 8;
	Yadd = height / 8;
	Zadd = abs((width * phd_cos(item->pos.y_rot + 0x8000)) >> 15);
	Xadd = abs((width * phd_sin(item->pos.y_rot + 0x8000)) >> 15);
	ls = (LASER_STRUCT*)item->data;
	Lh = Yadd >> 1;
	height = -Yadd;

	for (int i = 0; i < 3; i++)
	{
		Hadd = (Lh >> 1) * (i - 1);
		ls->v1[i].x = (short)Xadd;
		ls->v1[i].y = short(height - Lh + Hadd);
		ls->v1[i].z = (short)Zadd;
		ls->v2[i].x = (short)-Xadd;
		ls->v2[i].y = short(height - Lh + Hadd);
		ls->v2[i].z = (short)-Zadd;
		ls->v3[i].x = (short)Xadd;
		ls->v3[i].y = short(height + Lh + Hadd);
		ls->v3[i].z = (short)Zadd;
		ls->v4[i].x = (short)-Xadd;
		ls->v4[i].y = short(height + Lh + Hadd);
		ls->v4[i].z = (short)-Zadd;
		height -= Yadd * 3;
	}

	for (int i = 0; i < 18; i++)
		ls->Rand[i] = short(GetRandomControl() << 1);
}

void InitialiseSteamLasers(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	STEAMLASER_STRUCT* ls;
	long height, width, Xadd, Yadd, Zadd;
	short room_num;

	item = &items[item_number];
	item->data = (STEAMLASER_STRUCT*)game_malloc(sizeof(STEAMLASER_STRUCT));
	height = 1536;
	width = 2048;
	Xadd = (width / 2) - 512;
	item->pos.x_pos += (Xadd * phd_sin(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
	item->pos.z_pos += (Xadd * phd_cos(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	Xadd = abs((width * phd_sin(item->pos.y_rot + 0x8000)) >> 15);
	Zadd = abs((width * phd_cos(item->pos.y_rot + 0x8000)) >> 15);
	Yadd = height / 4;
	ls = (STEAMLASER_STRUCT*)item->data;
	height = -Yadd;

	for (int i = 0; i < 2; i++)
	{
		ls->v1[i].x = (short)Xadd;
		ls->v1[i].y = short(height - 64);
		ls->v1[i].z = (short)Zadd;
		ls->v2[i].x = (short)-Xadd;
		ls->v2[i].y = short(height - 64);
		ls->v2[i].z = (short)-Zadd;
		ls->v3[i].x = (short)Xadd;
		ls->v3[i].y = short(height + 64);
		ls->v3[i].z = (short)Zadd;
		ls->v4[i].x = (short)-Xadd;
		ls->v4[i].y = short(height + 64);
		ls->v4[i].z = (short)-Zadd;
		height -= Yadd;
	}

	for (int i = 0; i < 27; i++)
		ls->Rand[i] = short(GetRandomControl() << 1);
}

void InitialiseFloorLasers(short item_number)
{
	ITEM_INFO* item;
	FLOORLASER_STRUCT* ls;
	long width, height;

	item = &items[item_number];
	item->data = (FLOORLASER_STRUCT*)game_malloc(sizeof(FLOORLASER_STRUCT));
	ls = (FLOORLASER_STRUCT*)item->data;

	width = item->trigger_flags % 10;
	height = item->trigger_flags / 10;

	ls->v1.x = -512;
	ls->v1.y = -128;
	ls->v1.z = -512;

	ls->v2.x = -512;
	ls->v2.y = -128;
	ls->v2.z = short((width << 10) - 512);

	ls->v3.x = short((height << 10) - 512);
	ls->v3.y = -128;
	ls->v3.z = -512;

	ls->v4.x = short((height << 10) - 512);
	ls->v4.y = -128;
	ls->v4.z = short((width << 10) - 512);

	item->item_flags[0] = (short)width;
	item->item_flags[1] = (short)height;
	width = (width << 1) + 1;
	height = (height << 1) + 1;

	for (int i = 0; i < width * height; i++)
		ls->Rand[i] = short(GetRandomControl() << 1);

	item->trigger_flags = 0;
}

void InitialiseFishtank(short item_number)
{
	items[item_number].item_flags[1] = 4096;
}

void InitialiseWreckingBall(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	FLOOR_INFO* floor;
	short room_number;

	item = &items[item_number];
	item2 = find_a_fucking_item(ANIMATING16);
	item->item_flags[3] = item2 - items;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 1644;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void InitialiseRomeHammer(short item_number)
{
	*(long*)&items[item_number].item_flags[0] = 2;
	items[item_number].item_flags[3] = 250;
}

void InitialiseCrowDoveSwitch(short item_number)
{
	items[item_number].mesh_bits = 3;
}

void InitialiseTightRope(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->pos.y_rot)
		item->pos.z_pos -= 256;
	else if (item->pos.y_rot == 0x4000)
		item->pos.x_pos -= 256;
	else if (item->pos.y_rot == -0x4000)
		item->pos.x_pos += 256;
	else if (item->pos.y_rot == -0x8000)
		item->pos.z_pos += 256;
}

void InitialiseSearchObject(short item_number)
{
	ITEM_INFO* item;
	short ObjNum;

	item = &items[item_number];
	ObjNum = 3 - ((SEARCH_OBJECT4 - item->object_number) >> 1);

	if (ObjNum == 1)
		item->mesh_bits = 2;
	else if (!ObjNum)
	{
		item->meshswap_meshbits = -1;
		item->mesh_bits = 7;
	}
	else if (ObjNum == 3)
	{
		item->item_flags[1] = -1;
		item->mesh_bits = 9;

		for (int i = 0; i < level_items; ++i)
		{
			if ((items[i].object_number == EXPLOSION || objects[items[i].object_number].collision == PickUpCollision) &&
				item->pos.x_pos == items[i].pos.x_pos && item->pos.y_pos == items[i].pos.y_pos && item->pos.z_pos == items[i].pos.z_pos)
			{
				item->item_flags[1] = i;
				break;
			}

		}

		AddActiveItem(item_number);
		item->flags |= IFL_CODEBITS;
		item->status = ITEM_ACTIVE;
	}
}

void InitialiseExplosion(short item_number)
{
	ITEM_INFO* item;
	short size, blastwave;

	item = &items[item_number];

	if (item->trigger_flags >= 30000)
	{
		item->item_flags[1] = 3;
		item->trigger_flags -= 30000;
	}

	if (item->trigger_flags >= 20000)
	{
		item->item_flags[1] = 2;
		item->trigger_flags -= 20000;
	}
	else if (item->trigger_flags >= 10000)
	{
		item->item_flags[1] = 1;
		item->trigger_flags -= 10000;
	}

	if (item->trigger_flags >= 1000)
	{
		item->item_flags[3] = 1;
		item->trigger_flags -= 1000;
	}

	size = item->trigger_flags / 100;
	item->item_flags[2] = size;
	blastwave = 7 * (item->trigger_flags - 100 * size);
	item->trigger_flags = blastwave;
}

void InitialiseSecurityScreens(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[1] = item->trigger_flags;
	item->item_flags[0] = (item->trigger_flags & 3) << 6;
	item->mesh_bits = (2 << item->trigger_flags) + (32 << item->trigger_flags);
}

void InitialiseSteelDoor(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->mesh_bits = 1;
	item->pos.y_pos -= 1024;
}

void InitialiseMotionSensors(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->pos.x_pos -= (360 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.y_pos -= 88;
	item->pos.z_pos -= (360 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	item->item_flags[3] = item->pos.y_rot - 0x2800;
}

void InitialiseGrapplingTarget(short item_number)
{
	items[item_number].mesh_bits = 1;
}

void InitialiseSpiderGenerator(short item_number)
{
	ITEM_INFO* item;
	short tf;

	item = &items[item_number];
	tf = item->trigger_flags / 1000;
	item->item_flags[0] = tf & 1;
	item->item_flags[1] = tf & 2;
	item->item_flags[2] = tf & 4;
	item->pos.x_rot = 0x2000;
	item->trigger_flags %= 1000;

	if (!item->item_flags[0])
	{
		if (item->pos.y_rot > 0x1000 && item->pos.y_rot < 0x7000)
			item->pos.x_pos -= 512;
		else if (item->pos.y_rot < -0x1000 && item->pos.y_rot > -0x7000)
			item->pos.x_pos += 512;
		if (item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x2000)
			item->pos.z_pos -= 512;
		else if (item->pos.y_rot < -0x5000 || item->pos.y_rot > 0x5000)
			item->pos.z_pos += 512;
	}

	ClearSpiders();
}

void InitialisePropeller(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[0] = item->trigger_flags << 10;

	if (item->item_flags[0] < 2048)
		item->item_flags[0] = 3072;
}

void InitialiseSas(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	FLOOR_INFO* floor;
	short anim_index, room_num;

	item = &items[item_number];
	InitialiseCreature(item_number);
	anim_index = objects[SWAT].anim_index;

	if (!objects[SWAT].loaded)
		anim_index = objects[BLUE_GUARD].anim_index;

	switch (item->trigger_flags)
	{
	case 0:
	case 10:
		item->anim_number = anim_index;
		item->goal_anim_state = 1;
		break;

	case 1:
		item->anim_number = anim_index + 23;
		item->goal_anim_state = 11;
		break;

	case 2:
		item->anim_number = anim_index + 25;
		item->goal_anim_state = 13;
		item->status -= ITEM_INVISIBLE;
		break;

	case 3:
		item->anim_number = anim_index + 28;
		item->goal_anim_state = 15;
		item->meshswap_meshbits = 9216;

		for (int i = room[item->room_number].item_number; i != NO_ITEM; i = target->next_item)
		{
			target = &items[i];

			if (target->object_number >= ANIMATING1 && target->object_number <= ANIMATING15 && 
				target->room_number == item->room_number && target->trigger_flags == 3)
			{
				target->mesh_bits = -5;
				break;
			}
		}

		break;

	case 4:
		item->goal_anim_state = 17;
		item->meshswap_meshbits = 8192;
		item->anim_number = anim_index + 30;
		break;

	case 5:
		item->anim_number = anim_index + 26;
		item->goal_anim_state = 14;
		room_num = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
		GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) - 2048;
		break;

	case 6:
		item->goal_anim_state = 19;
		item->anim_number = anim_index + 32;
		break;

	case 7:
	case 9:
		item->anim_number = anim_index + 59;
		item->goal_anim_state = 38;
		item->pos.x_pos -= 128 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.z_pos -= 128 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		break;

	case 8:
		item->anim_number = anim_index + 46;
		item->goal_anim_state = 31;
		break;

	case 11:
		item->anim_number = anim_index + 12;
		item->goal_anim_state = 7;
		break;
	}

	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = item->goal_anim_state;
}

void InitialiseBurningRoots(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->meshswap_meshbits = 0;
	item->item_flags[1] = NO_ITEM;

	for (int i = 0; i < level_items; i++)
	{
		if (items[i].object_number == PUZZLE_ITEM1)
		{
			item->item_flags[1] = i;
			return;
		}
	}
}

void InitialiseCookerFlame(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	switch (item->trigger_flags)
	{
	case 0:
		item->pos.x_pos -= 256;
		item->pos.z_pos -= 256;
		break;

	case 1:
		item->pos.x_pos -= 256;
		item->pos.z_pos += 256;
		break;

	case 2:
		item->pos.x_pos += 256;
		item->pos.z_pos += 256;
		break;

	case 3:
		item->pos.x_pos += 256;
		item->pos.z_pos -= 256;
		break;
	}
}

void InitialiseAutogun(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->mesh_bits = 1024;
	item->data = game_malloc(sizeof(CREATURE_INFO));
}

void InitialiseKeyhole(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number == KEY_HOLE8 && item->trigger_flags == 1)
		item->mesh_bits = 1;
}

void InitialiseCutsceneRope(short item_number)
{
	ITEM_INFO* rope;

	rope = &items[item_number];
	rope->item_flags[2] = find_a_fucking_item(ANIMATING4) - items;
	rope->item_flags[3] = find_a_fucking_item(ANIMATING16_MIP) - items;
	rope->flags |= IFL_CODEBITS;
	AddActiveItem(item_number);
	rope->status = ITEM_ACTIVE;
}

void InitialiseXRayMachine(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags == 222)
		item->item_flags[0] = find_a_fucking_item(PUZZLE_HOLE1) - items;
}

void InitialisePortalDoor(short item_number)
{
	ITEM_INFO* item;
	PORTAL_STRUCT* portal;

	item = &items[item_number];
	portal = (PORTAL_STRUCT*)game_malloc(sizeof(PORTAL_STRUCT));
	item->pos.x_pos -= 512;
	item->data = portal;

	portal->v1.x = 0;
	portal->v1.y = -1024;
	portal->v1.z = -512;

	portal->v2.x = 0;
	portal->v2.y = -1024;
	portal->v2.z = 512;

	portal->v3.x = 0;
	portal->v3.y = 0;
	portal->v3.z = -512;

	portal->v4.x = 0;
	portal->v4.y = 0;
	portal->v4.z = 512;
	

	for (int i = 0; i < 64; i++)
	{
		portal->Rand[i] = (GetRandomControl() & 0x7F) - 64;

		portal->rgb[i].r = (GetRandomControl() & 0x7F) + 64;
		portal->rgb[i].g = (GetRandomControl() & 0x7F) + 64;
		portal->rgb[i].b = (GetRandomControl() & 0x7F) + 64;

		portal->orgb[i].r = portal->rgb[i].r;
		portal->orgb[i].g = portal->rgb[i].g;
		portal->orgb[i].b = portal->rgb[i].b;
	}
}

void InitialiseExplodingSwitch(short item_number)
{
	if (items[item_number].trigger_flags == 444)
		items[item_number].mesh_bits &= ~(1 << (objects[items[item_number].object_number].nmeshes - 2));
}

void InitialiseRaisingPlinth(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long x, y, z;

	x = 0;
	y = 0;
	z = 0;

	item = &items[item_number];

	for (int i = 0; i < level_items; i++)
	{
		item2 = &items[i];

		if (item2->object_number == TRIGGER_TRIGGERER)
		{
			if (item2->trigger_flags == 111)
				item->item_flags[3] |= i & 0xFF;
			else if (item2->trigger_flags == 112)
			{
				x = item2->pos.x_pos;
				y = item2->pos.y_pos;
				z = item2->pos.z_pos;
			}
		}
		else if (item2->object_number == PUZZLE_ITEM4_COMBO2)
		{
			item->item_flags[3] |= (i & 0xFF) << 8;
			item2->pos.y_pos = item->pos.y_pos - 512;
		}
	}

	for (int i = 0; i < level_items; i++)
	{
		item2 = &items[i];

		if (item2->object_number == PULLEY && item2->pos.x_pos == x && item2->pos.y_pos == y && item2->pos.z_pos == z)
		{
			item->item_flags[2] |= i & 0xFF;
			return;
		}
	}
}

void InitialiseTeleporter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags == 512)
		item->item_flags[1] = find_a_fucking_item(PUZZLE_HOLE2) - items;
}

void InitialiseGasCloud(short item_number)
{
	ITEM_INFO* item;
	GAS_CLOUD* cloud;
	AIOBJECT* ai;
	short clouds[8];

	item = &items[item_number];

	if (item->trigger_flags > 1)
	{
		if (item->trigger_flags == 3 || item->trigger_flags == 4)
			item->pos.y_pos -= 256;
		else if (item->trigger_flags == 2)
		{
			if (!item->pos.y_rot)
				item->pos.z_pos += 512;
			else if (item->pos.y_rot == 0x4000)
				item->pos.x_pos += 512;
			else if (item->pos.y_rot == -0x8000)
				item->pos.z_pos -= 512;
			else if (item->pos.y_rot == -0x4000)
				item->pos.x_pos -= 512;
		}

		return;
	}

	cloud = (GAS_CLOUD*)game_malloc(sizeof(GAS_CLOUD) * 8);
	item->data = cloud;
	memset(clouds, NO_ITEM, sizeof(clouds));

	for (int i = 0, j = 1; i < nAIObjects; i++)
	{
		ai = &AIObjects[i];

		if (ai->room_number == item->room_number)
		{
			clouds[j] = i;
			j++;

			if (j >= 7)
				break;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (!i)
		{
			cloud->t.x = 0;
			cloud->t.y = 0;
			cloud->t.z = 0;
		}
		else if (clouds[i] != NO_ITEM)
		{
			cloud->t.x = short(AIObjects[clouds[i]].x - item->pos.x_pos);
			cloud->t.y = short(AIObjects[clouds[i]].y - item->pos.y_pos);
			cloud->t.z = short(AIObjects[clouds[i]].z - item->pos.z_pos);
		}
		else
		{
			cloud->t.x = -1;
			break;
		}

		cloud->v1.x = -512;
		cloud->v1.y = 0;
		cloud->v1.z = -512;

		cloud->v2.x = -512;
		cloud->v2.y = 0;
		cloud->v2.z = 512;

		cloud->v3.x = 512;
		cloud->v3.y = 0;
		cloud->v3.z = -512;

		cloud->v4.x = 512;
		cloud->v4.y = 0;
		cloud->v4.z = 512;

		for (int j = 0; j < 36; j++)
			cloud->Rand[j] = short(GetRandomControl() << 1);

		cloud->mTime = 0;
		cloud->sTime = 0;
		cloud->num = 0;
		cloud++;
	}

	item->item_flags[1] = short(item->pos.x_pos >> 1);
	item->item_flags[2] = short(item->pos.y_pos >> 1);
	item->item_flags[3] = short(item->pos.z_pos >> 1);
	item->current_anim_state = short(GetRandomControl() << 1);
	item->goal_anim_state = short(GetRandomControl() << 1);
	item->required_anim_state = short(GetRandomControl() << 1);
}

void InitialiseSwitch(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags >= 1000)
	{
		item->mesh_bits = 0xAAAAB;
		item->item_flags[3] = ((item->trigger_flags - 1000) % 10) | (((item->trigger_flags - 1000) / 10) << 4);
		item->trigger_flags = 6;
	}
}

void InitialiseAnimatingSlots(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* c;

	item = &items[item_number];

	if (item->trigger_flags == 666 || item->trigger_flags == 667)
	{
		c = (CREATURE_INFO*)game_malloc(sizeof(CREATURE_INFO));
		item->data = c;
		c->joint_rotation[0] = 0;
		c->joint_rotation[1] = 0;

		if (item->trigger_flags == 667)
		{
			item->anim_number = objects[item->object_number].anim_index + 1;
			item->frame_number = anims[item->anim_number].frame_base;
		}
	}
	else if (item->trigger_flags > 900)
	{
		switch (item->trigger_flags)
		{
		case 901:
			item->anim_number = objects[item->object_number].anim_index;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 1;
			item->goal_anim_state = 1;
			break;

		case 902:
			item->anim_number = objects[item->object_number].anim_index + 2;
			item->frame_number = anims[item->anim_number].frame_base + 60;
			item->current_anim_state = 1;
			item->goal_anim_state = 1;
			break;

		case 903:
			item->anim_number = objects[item->object_number].anim_index;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 0;
			item->goal_anim_state = 1;
			break;

		case 904:
			item->anim_number = objects[item->object_number].anim_index + 2;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 1;
			item->goal_anim_state = 1;
			break;
		}
	}
}

void InitialiseGenSlot3(short item_number)
{
	if (gfCurrentLevel != LVL5_SINKING_SUBMARINE)
		items[item_number].mesh_bits = items[item_number].trigger_flags;
}

void InitialiseRaisingCog(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	short TriggerItems[4];
	short NumTrigs;

	item = &items[item_number];
	NumTrigs = (short)GetSwitchTrigger(item, TriggerItems, 1);

	for (int i = NumTrigs; i > 0; i--)
	{
		item2 = &items[TriggerItems[i - 1]];

		if (item2->object_number == TRIGGER_TRIGGERER)
			item->item_flags[1] = item2->room_number;
		else if (item2->object_number == PULLEY && item2->room_number == item->item_flags[1])
		{
			item2->item_flags[1] = 1;
			GlobalPulleyFrigItem = TriggerItems[i - 1];
		}
	}
}

void InitialiseArea51Laser(short item_number)
{
	ITEM_INFO* item;
	long x, z;

	item = &items[item_number];
	x = item->pos.x_pos + (2560 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
	z = item->pos.z_pos + (2560 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
	item->item_flags[0] = ((item->pos.x_pos >> 9) & 0xFF) | ((item->pos.z_pos >> 9) & 0xFF) << 8;
	item->item_flags[1] = ((x >> 9) & 0xFF) | ((z >> 9) & 0xFF) << 8;
	item->item_flags[2] = 1;
	item->item_flags[3] = 0;
	item->trigger_flags = 0;
}
