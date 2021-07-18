#include "../tomb5/pch.h"
#include "traps.h"
#include "items.h"
#include "objects.h"
#include "control.h"
#include "tomb4fx.h"
#include "sound.h"
#include "delstuff.h"
#include "debris.h"
#include "../specific/3dmath.h"
#include "box.h"
#include "lara1gun.h"
#include "switch.h"
#include "collide.h"
#include "effect2.h"

short SPxzoffs[8] = {0, 0, 0x200, 0, 0, 0, -0x200, 0};
short SPyoffs[8] = {-0x400, 0, -0x200, 0, 0, 0, -0x200, 0};
short SPDETyoffs[8] = {0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200};

void LaraBurn()
{
	short fire;

	if (!lara.burn && !lara.BurnSmoke)
	{
		fire = CreateEffect(lara_item->room_number);

		if (fire != NO_ITEM)
		{
			effects[fire].object_number = FLAME;
			lara.burn = 1;
		}
	}
}

void LavaBurn(ITEM_INFO* item)
{
	short room_number;

	if (item->hit_points >= 0 && lara.water_status != LW_FLYCHEAT)
	{
		room_number = item->room_number;

		if (item->floor == GetHeight(GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_number), item->pos.x_pos, 32000, item->pos.z_pos))
		{
			item->hit_points = -1;
			item->hit_status = 1;
			LaraBurn();
		}
	}
}

void ControlExplosion(short item_number)
{
	ITEM_INFO** _itemlist;
	MESH_INFO** _staticlist;
	ITEM_INFO* item;
	ITEM_INFO* target;
	ITEM_INFO* switem;
	MESH_INFO* staticp;
	PHD_VECTOR pos;
	long dx, dy, dz, lp, uw;
	short TriggerItems[8], NumTrigs;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		item->flags |= IFLAG_INVISIBLE;

		if (item->item_flags[0] < item->trigger_flags)
			item->item_flags[0]++;
		else if (item->item_flags[0] == item->trigger_flags)
		{
			item->item_flags[0]++;

			if (room[item->room_number].flags & ROOM_UNDERWATER)
				uw = 1;
			else
				uw = item->item_flags[1] == 1 ? 2 : 0;

			SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800004);
			SoundEffect(SFX_EXPLOSION2, &item->pos, 0);
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, uw, item->room_number);

			for (int i = 0; i < item->item_flags[2]; i++)
				TriggerExplosionSparks(item->pos.x_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], item->pos.y_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], item->pos.z_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], 2, 0, i, item->room_number);

			pos.x = item->pos.x_pos;
			pos.y = item->pos.y_pos - 128;
			pos.z = item->pos.z_pos;

			if (item->item_flags[3])
				TriggerShockwave(&pos, (32 * item->item_flags[2] + 304) << 16 | 0x30, 4 * item->item_flags[2] + 96, uw == 2 ? 0x18006080 : 0x18806000, 0x800);
			else if (uw != 2)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetLaraJointPos(&pos, 0);
				dx = pos.x - item->pos.x_pos;
				dy = pos.y - item->pos.y_pos;
				dz = pos.z - item->pos.z_pos;

				if (ABS(dx) < 1024 && ABS(dy) < 1024 && ABS(dz) < 1024)
				{
					lp = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

					if (lp < 2048)
					{
						lara_item->hit_points -= (short) (lp / 16);

						if (lp < 768)
							LaraBurn();
					}
				}
			}

			_itemlist = itemlist;
			_staticlist = staticlist;
			GetCollidedObjects(item, 2048, 1, _itemlist, _staticlist, 1);

			if (*_itemlist || *_staticlist)
			{
				target = *_itemlist;

				while (target)
				{
					if (target->object_number >= SMASH_OBJECT1 && target->object_number <= SMASH_OBJECT8)
					{
						TriggerExplosionSparks(target->pos.x_pos, target->pos.y_pos, target->pos.z_pos, 3, -2, 0, target->room_number);
						target->pos.y_pos -= 128;
						TriggerShockwave((PHD_VECTOR*) &target->pos, 0x1300030, 96, 0x18806000, 0);
						target->pos.y_pos += 128;
						ExplodeItemNode(target, 0, 0, 80);
						SmashObject(target - items);
						KillItem(target - items);
					}
					else if (target->object_number != SWITCH_TYPE7 && target->object_number != SWITCH_TYPE8)
					{
						if (objects[target->object_number].intelligent)
							DoGrenadeDamageOnBaddie(target, item);
					}
					else
						CrossbowHitSwitchType78(item, target, 0);

					_itemlist++;
					target = *_itemlist;
				}

				staticp = *_staticlist;

				while (staticp)
				{
					if (staticp->static_number >= 50 && staticp->static_number < 58)
					{
						TriggerExplosionSparks(staticp->x, staticp->y, staticp->z, 3, -2, 0, item->room_number);
						staticp->y -= 128;
						TriggerShockwave((PHD_VECTOR*) &staticp->x, 0xB00028, 64, 0x10806000, 0);
						staticp->y += 128;
						SoundEffect(ShatterSounds[gfCurrentLevel - 5][staticp->static_number], (PHD_3DPOS*) &staticp->x, 0);
						ShatterObject(NULL, staticp, -128, item->room_number, 0);
						SmashedMeshRoom[SmashedMeshCount] = item->room_number;
						SmashedMesh[SmashedMeshCount] = staticp;
						SmashedMeshCount++;
						staticp->Flags &= ~0x1;
					}

					_staticlist++;
					staticp = *_staticlist;
				}

				AlertNearbyGuards(item);
			}

			if (item->item_flags[1] >= 2)
			{
				if (item->item_flags[1] == 3)
				{
					NumTrigs = GetSwitchTrigger(item, TriggerItems, 1);

					for (int i = 0; i < NumTrigs; i++)
					{
						switem = &items[TriggerItems[i]];
						switem->item_flags[0] = 0;
					}

					item->item_flags[0] = 0;
				}
			}
			else
				KillItem(item_number);
		}
	}
}

void CloseTrapDoor(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	ROOM_INFO* r;
	long x, z;
	ushort pitsky;

	r = &room[item->room_number];
	x = (item->pos.z_pos - r->z) >> 10;
	z = (item->pos.x_pos - r->x) >> 10;
	floor = &r->floor[x + (z * r->x_size)];

	if (item->pos.y_pos == r->minfloor)
	{
		pitsky = floor->pit_room;
		floor->pit_room = NO_ROOM;
		r = &room[pitsky];
		x = (item->pos.z_pos - r->z) >> 10;
		z = (item->pos.x_pos - r->x) >> 10;
		floor = &r->floor[x + (z * r->x_size)];
		pitsky |= (floor->sky_room << 8);
		floor->sky_room = NO_ROOM;
		item->item_flags[2] = 1;
		item->item_flags[3] = pitsky;
	}
	else if (item->pos.y_pos == r->maxceiling)
	{
		pitsky = floor->sky_room;
		floor->sky_room = NO_ROOM;
		r = &room[pitsky];
		x = (item->pos.z_pos - r->z) >> 10;
		z = (item->pos.x_pos - r->x) >> 10;
		floor = &r->floor[x + (z * r->x_size)];
		pitsky <<= 8;
		pitsky |= floor->pit_room;
		floor->pit_room = NO_ROOM;
		item->item_flags[2] = 1;
		item->item_flags[3] = pitsky;
	}
	else
	{
		item->item_flags[3] = 0;
		item->item_flags[2] = 1;
	}
}

void DartEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* dart;
	long x, z, xLimit, zLimit, xv, zv, rand;
	short num;

	item = &items[item_number];

	if (item->active)
	{
		if (item->timer > 0)
		{
			item->timer--;
			return;
		}

		item->timer = 24;
	}

	num = CreateItem();

	if (num != NO_ITEM)
	{
		x = 0;
		z = 0;
		dart = &items[num];
		dart->object_number = DARTS;
		dart->room_number = item->room_number;

		if (item->pos.y_rot == 0)
			z = 512;
		else if (item->pos.y_rot == 16384)
			x = 512;
		else if (item->pos.y_rot == -16384)
			x = -512;
		else if (item->pos.y_rot == -32768)
			z = -512;

		dart->pos.x_pos = x + item->pos.x_pos;
		dart->pos.y_pos = item->pos.y_pos - 512;
		dart->pos.z_pos = z + item->pos.z_pos;
		InitialiseItem(num);
		dart->pos.x_rot = 0;
		dart->pos.y_rot += 32768;
		dart->speed = 256;
		xLimit = 0;
		zLimit = 0;

		if (x)
			xLimit = ABS(x << 1) - 1;
		else
			zLimit = ABS(z << 1) - 1;

		for (int i = 0; i < 5; i++)
		{
			rand = -GetRandomControl();

			if (z >= 0)
				zv = zLimit & rand;
			else
				zv = -(zLimit & rand);

			if (x >= 0)
				xv = xLimit & rand;
			else
				xv = -(xLimit & rand);

			TriggerDartSmoke(dart->pos.x_pos, dart->pos.y_pos, dart->pos.z_pos, xv, zv, 0);
		}

		AddActiveItem(num);
		dart->status = ITEM_ACTIVE;
		SoundEffect(SFX_LIFT_DOORS, &dart->pos, 0);
	}
}

void DartsControl(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, z, speed;
	short room_num;

	item = &items[item_number];

	if (item->touch_bits)
	{
		lara_item->hit_points -= 25;
		lara_item->hit_status = 1;
		lara.poisoned += 160;
		DoBloodSplat(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (GetRandomControl() & 3) + 4, lara_item->pos.y_rot, lara_item->room_number);
		KillItem(item_number);
	}
	else
	{
		x = item->pos.x_pos;
		z = item->pos.z_pos;
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> 14;
		item->pos.z_pos += (speed * phd_cos(item->pos.y_rot)) >> 14;
		item->pos.x_pos += (speed * phd_sin(item->pos.y_rot)) >> 14;
		item->pos.y_pos -= (item->speed * phd_sin(item->pos.x_rot)) >> 14;
		room_num = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);

		if (item->room_number != room_num)
			ItemNewRoom(item_number, room_num);

		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (item->pos.y_pos >= item->floor)
		{
			for (int i = 0; i < 4; i++)
				TriggerDartSmoke(x, item->pos.y_pos, z, 0, 0, 1);

			KillItem(item_number);
		}
	}
}

void inject_traps(bool replace)
{
	INJECT(0x0048AD60, LaraBurn, replace);
	INJECT(0x0048ADD0, LavaBurn, replace);
	INJECT(0x0048C6D0, ControlExplosion, replace);
	INJECT(0x00488E30, CloseTrapDoor, replace);
	INJECT(0x00489B30, DartEmitterControl, replace);
	INJECT(0x00489D60, DartsControl, replace);
}
