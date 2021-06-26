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

	door = (DOOR_DATA*)game_malloc(sizeof(DOOR_DATA), 0);
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
	if (ocb == 5)
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

void SetupClosedDoorStuff(DOOR_DATA* door, ITEM_INFO* item, short room2, int dx, int dy)
{
	room_info* r;
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

void SCDS(room_info* r, short** dptr, char* dn, long dx, long dy, long ox, long oz)
{
	short* d;
	long minx, maxx, minz, maxz, wx, wz;

	d = r->door;

	if (d)
	{
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
					*dn = (*d & 128) | 1;
				else if (d[1])
					*dn = (d[1] & 128) | 2;
				else
					*dn = (d[2] & 128) | 4;
			}
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
	item->data = (LASER_STRUCT*)game_malloc(sizeof(LASER_STRUCT), 0);
	width = (item->trigger_flags & 255) << 10;

	if (!(item->trigger_flags & 1))
	{
		Xadd = (width / 2) - 512;
		item->pos.z_pos += (Xadd * phd_cos(item->pos.y_rot + 32768)) >> 14;
		item->pos.x_pos += (Xadd * phd_sin(item->pos.y_rot + 32768)) >> 14;
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
	Zadd = abs((width * phd_cos(item->pos.y_rot + 32768)) >> 15);
	Xadd = abs((width * phd_sin(item->pos.y_rot + 32768)) >> 15);
	ls = (LASER_STRUCT*)item->data;
	Lh = Yadd >> 1;
	height = -Yadd;

	for (int i = 0; i < 3; i++)
	{
		Hadd = (Lh >> 1) * (i - 1);
		ls->v1[i].vx = short(Xadd);
		ls->v1[i].vy = short(height - Lh + Hadd);
		ls->v1[i].vz = short(Zadd);
		ls->v2[i].vx = short(-Xadd);
		ls->v2[i].vy = short(height - Lh + Hadd);
		ls->v2[i].vz = short(-Zadd);
		ls->v3[i].vx = short(Xadd);
		ls->v3[i].vy = short(height + Lh + Hadd);
		ls->v3[i].vz = short(Zadd);
		ls->v4[i].vx = short(-Xadd);
		ls->v4[i].vy = short(height + Lh + Hadd);
		ls->v4[i].vz = short(-Zadd);
		height -= Yadd * 3;
	}

	for (int i = 0; i < 18; i++)
		ls->Rand[i] = GetRandomControl() << 1;
}

void InitialiseSteamLasers(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	STEAMLASER_STRUCT* ls;
	long height, width, Xadd, Yadd, Zadd;
	short room_num;

	item = &items[item_number];
	item->data = (STEAMLASER_STRUCT*)game_malloc(sizeof(STEAMLASER_STRUCT), 0);
	height = 1536;
	width = 2048;
	Xadd = (width / 2) - 512;
	item->pos.x_pos += (Xadd * phd_sin(item->pos.y_rot + 32768)) >> 14;
	item->pos.z_pos += (Xadd * phd_cos(item->pos.y_rot + 32768)) >> 14;
	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	Xadd = abs((width * phd_sin(item->pos.y_rot + 32768)) >> 15);
	Zadd = abs((width * phd_cos(item->pos.y_rot + 32768)) >> 15);
	Yadd = height / 4;
	ls = (STEAMLASER_STRUCT*)item->data;
	height = -Yadd;

	for (int i = 0; i < 2; i++)
	{
		ls->v1[i].vx = (short)Xadd;
		ls->v1[i].vy = (short)(height - 64);
		ls->v1[i].vz = (short)Zadd;
		ls->v2[i].vx = (short)-Xadd;
		ls->v2[i].vy = (short)(height - 64);
		ls->v2[i].vz = (short)-Zadd;
		ls->v3[i].vx = (short)Xadd;
		ls->v3[i].vy = (short)(height + 64);
		ls->v3[i].vz = (short)Zadd;
		ls->v4[i].vx = (short)-Xadd;
		ls->v4[i].vy = (short)(height + 64);
		ls->v4[i].vz = (short)-Zadd;
		height -= Yadd;
	}

	for (int i = 0; i < 27; i++)
		ls->Rand[i] = GetRandomControl() << 1;
}

void InitialiseFloorLasers(short item_number)
{
	ITEM_INFO* item;
	FLOORLASER_STRUCT* ls;
	long width, height;

	item = &items[item_number];
	item->data = (FLOORLASER_STRUCT*)game_malloc(sizeof(FLOORLASER_STRUCT), 0);
	ls = (FLOORLASER_STRUCT*)item->data;
	width = item->trigger_flags % 10;
	height = item->trigger_flags / 10;
	ls->v1.vx = -512;
	ls->v1.vy = -128;
	ls->v1.vz = -512;
	ls->v2.vx = -512;
	ls->v2.vy = -128;
	ls->v2.vz = (short)((width << 10) - 512);
	ls->v3.vx = (short)((height << 10) - 512);
	ls->v3.vy = -128;
	ls->v3.vz = -512;
	ls->v4.vx = (short)((height << 10) - 512);
	ls->v4.vy = -128;
	ls->v4.vz = (short)((width << 10) - 512);
	item->item_flags[0] = (short)width;
	item->item_flags[1] = (short)height;
	width = (width << 1) + 1;
	height = (height << 1) + 1;

	for (int i = 0; i < width * height; i++)
		ls->Rand[i] = (GetRandomControl() << 1);

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

	if (room_number != item->room_number)
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
	else if (item->pos.y_rot == 16384)
		item->pos.x_pos -= 256;
	else if (item->pos.y_rot == -16384)
		item->pos.x_pos += 256;
	else if (item->pos.y_rot == -32768)
		item->pos.z_pos += 256;
}

void InitialiseSearchObject(short item_number)
{
	ITEM_INFO* item;
	short ObjNum;
	int i;

	item = &items[item_number];
	ObjNum = 3 - ((SEARCH_OBJECT4 - item->object_number) >> 1);

	if (ObjNum == 1)
	{
		item->mesh_bits = 2;
	}
	else if (!ObjNum)
	{
		item->meshswap_meshbits = -1;
		item->mesh_bits = 7;
	}
	else if (ObjNum == 3)
	{
		item->item_flags[1] = -1;
		item->mesh_bits = 9;

		for (i = 0; i < level_items; ++i)
		{
			if (items[i].object_number == EXPLOSION)
			{
				if (item->pos.x_pos == items[i].pos.x_pos && item->pos.y_pos == items[i].pos.y_pos && item->pos.z_pos == items[i].pos.z_pos)
				{
					item->item_flags[1] = i;
					break;
				}
			}
			else if (objects[items[i].object_number].collision == PickupCollision && item->pos.x_pos == items[i].pos.x_pos && item->pos.y_pos == items[i].pos.y_pos && item->pos.z_pos == items[i].pos.z_pos)
			{
				item->item_flags[1] = i;
				break;
			}

		}

		AddActiveItem(item_number);
		item->flags |= IFLAG_ACTIVATION_MASK;
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
	INJECT(0x0043E3F0, SetupClosedDoorStuff);
	INJECT(0x0043E550, SCDS);
	INJECT(0x0043E6C0, InitialiseLasers);
	INJECT(0x0043E980, InitialiseSteamLasers);
	INJECT(0x0043EB50, InitialiseFloorLasers);
	INJECT(0x0043EC70, InitialiseFishtank);
	INJECT(0x0043EF20, InitialiseWreckingBall);
	INJECT(0x0043ECB0, InitialiseRomeHammer);
	INJECT(0x0043ECF0, InitialiseCrowDoveSwitch);
	INJECT(0x0043ED30, InitialiseTightRope);
	INJECT(0x0043EDB0, InitialiseSearchObject);
	INJECT(0x0043F070, InitialiseExplosion);
}
