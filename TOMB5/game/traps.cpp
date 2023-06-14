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
#include "effects.h"
#include "../specific/function_stubs.h"
#include "sphere.h"
#include "lara_states.h"
#include "gameflow.h"
#include "draw.h"
#include "../specific/output.h"
#include "camera.h"
#include "../specific/input.h"
#include "lara.h"

short SPxzoffs[8] = {0, 0, 0x200, 0, 0, 0, -0x200, 0};
short SPyoffs[8] = {-0x400, 0, -0x200, 0, 0, 0, -0x200, 0};
short SPDETyoffs[8] = {0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200};

uchar Flame3xzoffs[16][2] =
{
	{9, 9},
	{24, 9},
	{40, 9},
	{55, 9},
	{9, 24},
	{24, 24},
	{40, 24},
	{55, 24},
	{9, 40},
	{24, 40},
	{40, 40},
	{55, 40},
	{9, 55},
	{24, 55},
	{40, 55},
	{55, 55}
};

static PHD_VECTOR FloorTrapDoorPos = { 0, 0, -655 };
static PHD_VECTOR CeilingTrapDoorPos = { 0, 1056, -480 };

static short FloorTrapDoorBounds[12] =
{
	-256, 256, 0, 0, -1024, -256, -1820, 1820, -5460, 5460, -1820, 1820
};

static short CeilingTrapDoorBounds[12] =
{
	-256, 256, 0, 900, -768, -256, -1820, 1820, -5460, 5460, -1820, 1820
};

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
	ITEM_INFO** itemlist;
	MESH_INFO** meshlist;
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
		item->flags |= IFL_INVISIBLE;

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
				TriggerExplosionSparks(item->pos.x_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], item->pos.y_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], item->pos.z_pos + (GetRandomControl() % 128 - 64) * item->item_flags[2], 2, 0, uw, item->room_number);

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
				GetLaraJointPos(&pos, LMX_HIPS);
				dx = pos.x - item->pos.x_pos;
				dy = pos.y - item->pos.y_pos;
				dz = pos.z - item->pos.z_pos;

				if (abs(dx) < 1024 && abs(dy) < 1024 && abs(dz) < 1024)
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

			itemlist = (ITEM_INFO**)&tsv_buffer[0x2000];
			meshlist = (MESH_INFO**)&tsv_buffer[0x3000];
			GetCollidedObjects(item, 2048, 1, itemlist, meshlist, 1);

			if (*itemlist || *meshlist)
			{
				target = *itemlist;

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

					itemlist++;
					target = *itemlist;
				}

				staticp = *meshlist;

				while (staticp)
				{
					if (staticp->static_number >= 50 && staticp->static_number < 58)
					{
						TriggerExplosionSparks(staticp->x, staticp->y, staticp->z, 3, -2, 0, item->room_number);
						staticp->y -= 128;
						TriggerShockwave((PHD_VECTOR*) &staticp->x, 0xB00028, 64, 0x10806000, 0);
						staticp->y += 128;
						SoundEffect(ShatterSounds[gfCurrentLevel][staticp->static_number - 50], (PHD_3DPOS*)&staticp->x, 0);
						ShatterObject(0, staticp, -128, item->room_number, 0);
						SmashedMeshRoom[SmashedMeshCount] = item->room_number;
						SmashedMesh[SmashedMeshCount] = staticp;
						SmashedMeshCount++;
						staticp->Flags &= ~0x1;
					}

					meshlist++;
					staticp = *meshlist;
				}

				AlertNearbyGuards(item);
			}

			if (item->item_flags[1] >= 2)
			{
				if (item->item_flags[1] == 3)
				{
					NumTrigs = (short)GetSwitchTrigger(item, TriggerItems, 1);

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

void OpenTrapDoor(ITEM_INFO* item)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	ushort pitsky;

	pitsky = item->item_flags[3];
	r = &room[item->room_number];
	floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];

	if (item->pos.y_pos == r->minfloor)
	{
		floor->pit_room = pitsky & 0xFF;
		r = &room[floor->pit_room];
		r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].sky_room = pitsky >> 8;
	}
	else
	{
		floor->sky_room = pitsky >> 8;
		r = &room[floor->sky_room];
		r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].pit_room = pitsky & 0xFF;
	}

	item->item_flags[2] = 0;
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
		dart->pos.y_rot = item->pos.y_rot + 32768;
		dart->speed = 256;
		xLimit = 0;
		zLimit = 0;

		if (x)
			xLimit = abs(x << 1) - 1;
		else
			zLimit = abs(z << 1) - 1;

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
		SoundEffect(SFX_LIFT_DOORS, &dart->pos, SFX_DEFAULT);
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
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
		item->pos.z_pos += (speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.x_pos += (speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.y_pos -= (item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
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

void FlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ulong distance;
	long x, z;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags < 0)
		{
			if ((-item->trigger_flags & 7) != 2 && (-item->trigger_flags & 7) != 7)
			{
				if (item->item_flags[0])
				{
					if (item->item_flags[1])
						item->item_flags[1] = item->item_flags[1] - (item->item_flags[1] >> 2);

					if (item->item_flags[2] < 256)
						item->item_flags[2] += 8;

					item->item_flags[0]--;

					if (!item->item_flags[0])
						item->item_flags[3] = (GetRandomControl() & 0x3F) + 150;
				}
				else
				{
					item->item_flags[3]--;

					if (!item->item_flags[3])
					{
						if (-item->trigger_flags >> 3)
							item->item_flags[0] = (GetRandomControl() & 0x1F) + 30 * (-item->trigger_flags >> 3);
						else
							item->item_flags[0] = (GetRandomControl() & 0x3F) + 60;
					}

					if (item->item_flags[2])
						item->item_flags[2] -= 8;

					if (item->item_flags[1] > -8192)
						item->item_flags[1] -= 512;
				}

				if (item->item_flags[2])
					AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, item->room_number, item->item_flags[2] & 0xFF);

				if (item->item_flags[1])
				{
					SoundEffect(SFX_D_METAL_CAGE_OPEN, &item->pos, 0);

					if (item->item_flags[1] > -8192)
						TriggerSuperJetFlame(item, item->item_flags[1], GlobalCounter & 1);
					else
						TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);

					TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (-item->item_flags[1] >> 10) - (GetRandomControl() & 1) + 16, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
				}
				else
					TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 10 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
			}
			else
			{
				SoundEffect(SFX_D_METAL_CAGE_OPEN, &item->pos, 0);
				TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);
				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (GetRandomControl() & 3) + 20, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
			}

			SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);
		}
		else
		{
			AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, item->room_number, 0);
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 16 - (GetRandomControl() & 1), 192 + (GetRandomControl() & 0x3F), 96 + (GetRandomControl() & 0x1F), 0);
			SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);

			if (!lara.burn && item->trigger_flags != 33 && ItemNearLara(&item->pos, 600))
			{
				x = lara_item->pos.x_pos - item->pos.x_pos;
				z = lara_item->pos.z_pos - item->pos.z_pos;
				distance = SQUARE(x) + SQUARE(z);

				if (distance < 262144)
					LaraBurn();
			}
		}
	}
}

void FlameEmitter2Control(short item_number)
{
	ITEM_INFO* item;
	long r, g;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags >= 0)
		{
			if (item->trigger_flags != 2)
			{
				if (item->trigger_flags == 123)
					AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 1, item->room_number, item->item_flags[3]);
				else
					AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 1 - item->trigger_flags, item->room_number, item->item_flags[3]);
			}

			if (!item->trigger_flags || item->trigger_flags == 2)
			{
				r = (GetRandomControl() & 0x3F) + 192;
				g = (GetRandomControl() & 0x1F) + 96;

				if (item->item_flags[3])
				{
					r = r * item->item_flags[3] >> 8;
					g = g * item->item_flags[3] >> 8;
				}

				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 10, r, g, 0);
			}

			SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);
		}
		else if (!item->item_flags[0])
		{
			if (item->trigger_flags < -100)
				item->trigger_flags += 100;

			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0] == 1)
		{
			FlipMap(-item->trigger_flags);
			flipmap[-item->trigger_flags] ^= IFL_CODEBITS;
			item->item_flags[0] = 2;
		}
	}
}

void FlameEmitter3Control(short item_number)
{
	ITEM_INFO* item, *item2;
	PHD_3DPOS pos;
	PHD_VECTOR s, d;
	long x, z, distance, r, g;
	short g2, b;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags)
		{
			SoundEffect(SFX_2GUNTEX_HIT_GUNS, &item->pos, 0);
			g2 = (GetRandomControl() & 0x3F) + 192;
			b = (GetRandomControl() & 0x3F) + 192;
			s.x = item->pos.x_pos;
			s.y = item->pos.y_pos;
			s.z = item->pos.z_pos;

			if (!(GlobalCounter & 3) && (item->trigger_flags == 2 || item->trigger_flags == 4))
			{
				d.y = item->pos.y_pos;
				d.x = item->pos.x_pos + (phd_sin(item->pos.y_rot - 32768) >> 3);
				d.z = item->pos.z_pos + (phd_cos(item->pos.y_rot - 32768) >> 3);

				if (!(GetRandomControl() & 3))
					TriggerLightning(&s, &d, (GetRandomControl() & 0x1F) + 96, (0x200000 | g2) << 8 | b, 1, 32, 3);
				else
					TriggerLightning(&s, &d, (GetRandomControl() & 0x1F) + 64, (0x180000 | g2) << 8 | b, 0, 32, 3);
			}

			if (item->trigger_flags >= 3 && !(GlobalCounter & 1))
			{
				d.x = 0;
				d.y = -64;
				d.z = 20;
				item2 = &items[item->item_flags[2 + (GlobalCounter >> 2 & 1)]];
				GetJointAbsPosition(item2, &d, 0);

				if (!(GlobalCounter & 3))
				{
					if (!(GetRandomControl() & 3))
						TriggerLightning(&s, &d, (GetRandomControl() & 0x1F) + 96, (0x200000 | g2) << 8 | b, 1, 32, 5);
					else
						TriggerLightning(&s, &d, (GetRandomControl() & 0x1F) + 64, (0x180000 | g2) << 8 | b, 0, 32, 5);
				}

				if (item->trigger_flags != 3 || item2->trigger_flags)
					TriggerLightningGlow(d.x, d.y, d.z, (0x400000 | g2) << 8 | b);
			}

			if ((GlobalCounter & 3) == 2)
			{
				s.x = item->pos.x_pos;
				s.y = item->pos.y_pos;
				s.z = item->pos.z_pos;
				d.x = s.x + (GetRandomControl() & 0x1FF) - 256;
				d.y = s.y + (GetRandomControl() & 0x1FF) - 256;
				d.z = s.z + (GetRandomControl() & 0x1FF) - 256;
				TriggerLightning(&s, &d, (GetRandomControl() & 0xF) + 16, (0x180000 | g2) << 8 | b, 3, 32, 3);
				TriggerLightningGlow(s.x, s.y, s.z, (0x400000 | g2) << 8 | b);
			}
		}
		else
		{
			if (!item->item_flags[0])
			{
				item->item_flags[0] = (GetRandomControl() & 3) + 8;
				distance = GetRandomControl() & 0x3F;
				item->item_flags[1] = (short) (distance == item->item_flags[1] ? (distance + 13) & 0x3F : distance);
			}
			else
				item->item_flags[0]--;

			if (!(wibble & 4))
			{
				x = 16 * (Flame3xzoffs[item->item_flags[1] & 7][0] - 32);
				z = 16 * (Flame3xzoffs[item->item_flags[1] & 7][1] - 32);
				TriggerFireFlame(item->pos.x_pos + x, item->pos.y_pos, item->pos.z_pos + z, -1, 2);
			}

			if (wibble & 4)
			{
				x = 16 * (Flame3xzoffs[(item->item_flags[1] >> 3) + 8][0] - 32);
				z = 16 * (Flame3xzoffs[(item->item_flags[1] >> 3) + 8][1] - 32);
				TriggerFireFlame(item->pos.x_pos + x, item->pos.y_pos, item->pos.z_pos + z, -1, 2);
			}

			SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);
			distance = GetRandomControl();
			r = (distance & 0x3F) + 192;
			g = (distance >> 4 & 0x1F) + 96;
			TriggerDynamic(x, item->pos.y_pos, z, 12, r, g, 0);
			pos.x_pos = item->pos.x_pos;
			pos.y_pos = item->pos.y_pos;
			pos.z_pos = item->pos.z_pos;

			if (ItemNearLara(&pos, 600) && !lara.burn)
			{
				lara_item->hit_points -= 5;
				lara_item->hit_status = 1;
				x = lara_item->pos.x_pos - pos.x_pos;
				z = lara_item->pos.z_pos - pos.z_pos;
				distance = SQUARE(x) + SQUARE(z);

				if (distance < 202500)
					LaraBurn();
			}
		}
	}
}

void FlameControl(short fx_number)
{
	FX_INFO* fx;
	long y, r, g;

	fx = &effects[fx_number];

	if (lara.water_status == LW_FLYCHEAT)
	{
		KillEffect(fx_number);
		lara.burn = 0;
		lara.BurnBlue = 0;
		lara.BurnSmoke = 0;
		return;
	}

	for (int i = 14; i >= 0; i--)
	{
		if (!(wibble & 12))
		{
			fx->pos.x_pos = 0;
			fx->pos.y_pos = 0;
			fx->pos.z_pos = 0;
			GetLaraJointPos((PHD_VECTOR*) &fx->pos, i);

			if (lara.BurnCount)
			{
				lara.BurnCount--;

				if (!lara.BurnCount)
					lara.BurnSmoke = 1;
			}

			TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 255 - lara.BurnSmoke);
		}
	}

	r = (GetRandomControl() & 0x3F) + 192;
	g = (GetRandomControl() & 0x1F) + 96;

	if (!lara.BurnSmoke)
	{
		if (!lara.BurnBlue)
			TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 13, r, g, 0);
		else if (lara.BurnBlue == 1)
			TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 13, 0, g, r);
		else if (lara.BurnBlue == 2)
			TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 13, 0, r, g);
	}

	if (lara_item->room_number != fx->room_number)
		EffectNewRoom(fx_number, lara_item->room_number);

	y = GetWaterHeight(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->room_number);

	if (y != NO_HEIGHT && fx->pos.y_pos > y && !lara.BurnBlue)
	{
		KillEffect(fx_number);
		lara.burn = 0;
	}
	else
	{
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &fx->pos, 0);
		lara_item->hit_points -= 7;
		lara_item->hit_status = 1;
	}
}

void RollingBallCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!TestBoundsCollide(item, laraitem, coll->radius) || !TestCollision(item, laraitem))
		return;

	if (TriggerActive(item) && (item->item_flags[0] || item->fallspeed))
	{
		lara_item->anim_number = ANIM_RBALL_DEATH;
		lara_item->frame_number = anims[ANIM_RBALL_DEATH].frame_base;
		lara_item->goal_anim_state = AS_DEATH;
		lara_item->current_anim_state = AS_DEATH;
		lara_item->gravity_status = 0;
	}
	else
		ObjectCollision(item_number, laraitem, coll);
}

void ControlRollingBall(short item_number)
{
	ITEM_INFO* item;
	ushort tyrot, destyrot;
	short room_number, velnotadjusted;
	long h, fx, fz, fh, fhf, bz, bh, bhf, rx, rh, rhf, lx, lh, lhf;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->fallspeed += 6;
	item->pos.x_pos += item->item_flags[0] >> 5;
	item->pos.y_pos += item->fallspeed;
	item->pos.z_pos += item->item_flags[1] >> 5;
	room_number = item->room_number;
	h = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) - 512;

	if (item->pos.y_pos > h)
	{
		if (abs(item->fallspeed) > 16)
		{
			fz = phd_sqrt(SQUARE(camera.pos.x - item->pos.x_pos) + SQUARE(camera.pos.y - item->pos.y_pos) + SQUARE(camera.pos.z - item->pos.z_pos));

			if (fz < 16384)
				camera.bounce = -(((16384 - fz) * abs(item->fallspeed)) >> W2V_SHIFT);
		}

		if (item->pos.y_pos - h < 512)
			item->pos.y_pos = h;

		if (item->fallspeed > 64)
			item->fallspeed = -(item->fallspeed >> 2);
		else
		{
			if (abs(item->speed) <= 512 || GetRandomControl() & 0x1F)
				item->fallspeed = 0;
			else
				item->fallspeed = -(GetRandomControl() % (item->speed >> 3));
		}
	}

	fx = item->pos.x_pos;
	fz = item->pos.z_pos + 128;
	bz = item->pos.z_pos - 128;
	rx = item->pos.x_pos + 128;
	lx = item->pos.x_pos - 128;
	fh = GetHeight(GetFloor(fx, item->pos.y_pos, fz, &room_number), fx, item->pos.y_pos, fz) - 512;
	bh = GetHeight(GetFloor(fx, item->pos.y_pos, bz, &room_number), fx, item->pos.y_pos, bz) - 512;
	rh = GetHeight(GetFloor(rx, item->pos.y_pos, bz + 128, &room_number), rx, item->pos.y_pos, bz + 128) - 512;
	lh = GetHeight(GetFloor(lx, item->pos.y_pos, bz + 128, &room_number), lx, item->pos.y_pos, bz + 128) - 512;
	fx = item->pos.x_pos;
	fz = item->pos.z_pos + 512;
	bz = item->pos.z_pos - 512;
	rx = item->pos.x_pos + 512;
	lx = item->pos.x_pos - 512;
	fhf = GetHeight(GetFloor(fx, item->pos.y_pos, fz, &room_number), fx, item->pos.y_pos, fz) - 512;
	bhf = GetHeight(GetFloor(fx, item->pos.y_pos, bz, &room_number), fx, item->pos.y_pos, bz) - 512;
	rhf = GetHeight(GetFloor(rx, item->pos.y_pos, bz + 512, &room_number), rx, item->pos.y_pos, bz + 512) - 512;
	lhf = GetHeight(GetFloor(lx, item->pos.y_pos, bz + 512, &room_number), lx, item->pos.y_pos, bz + 512) - 512;

	if (item->pos.y_pos - h > -256 || item->pos.y_pos - fhf >= 512 || item->pos.y_pos - rhf >= 512 ||
		item->pos.y_pos - bhf >= 512 || item->pos.y_pos - lhf >= 512)
	{
		velnotadjusted = 0;

		if (fh - h <= 256)
		{
			if (fhf - h < -1024 || fh - h < -256)
			{
				if (item->item_flags[1] <= 0)
				{
					if (!item->item_flags[1] && item->item_flags[0])
						item->pos.z_pos = (item->pos.z_pos & -512) | 512;
				}
				else
				{
					item->item_flags[1] = -item->item_flags[1] >> 1;
					item->pos.z_pos = (item->pos.z_pos & -512) | 512;
				}
			}
			else if (fh == h)
				velnotadjusted = 1;
			else
				item->item_flags[1] += (short)((fh - h) >> 1);
		}

		if (bh - h > 256)
			velnotadjusted++;
		else if (bhf - h < -1024 || bh - h < -256)
		{
			if (item->item_flags[1] >= 0)
			{
				if (!item->item_flags[1] && item->item_flags[0])
					item->pos.z_pos = (item->pos.z_pos & -512) | 512;
			}
			else
			{
				item->item_flags[1] = -item->item_flags[1] >> 1;
				item->pos.z_pos = (item->pos.z_pos & -512) | 512;
			}
		}
		else if (bh == h)
			velnotadjusted++;
		else
			item->item_flags[1] -= (short)((bh - h) >> 1);

		if (velnotadjusted == 2)
		{
			if (abs(item->item_flags[1]) <= 64)
				item->item_flags[1] = 0;
			else
				item->item_flags[1] -= item->item_flags[1] >> 6;
		}

		velnotadjusted = 0;

		if (lh - h <= 256)
		{
			if (lhf - h < -1024 || lh - h < -256)
			{
				if (item->item_flags[0] >= 0)
				{
					if (!item->item_flags[0] && item->item_flags[1])
						item->pos.x_pos = (item->pos.x_pos & -512) | 512;
				}
				else
				{
					item->item_flags[0] = -item->item_flags[0] >> 1;
					item->pos.x_pos = (item->pos.x_pos & -512) | 512;
				}
			}
			else if (lh == h)
				velnotadjusted = 1;
			else
				item->item_flags[0] -= (short)((lh - h) >> 1);
		}

		if (rh - h <= 256)
		{
			if (rhf - h < -1024 || rh - h < -256)
			{
				if (item->item_flags[0] <= 0)
				{
					if (!item->item_flags[0] && item->item_flags[1])
						item->pos.x_pos = (item->pos.x_pos & -512) | 512;
				}
				else
				{
					item->item_flags[0] = -item->item_flags[0] >> 1;
					item->pos.x_pos = (item->pos.x_pos & -512) | 512;
				}
			}
			else if (rh == h)
				velnotadjusted++;
			else
				item->item_flags[0] += (short)((rh - h) >> 1);
		}

		if (velnotadjusted == 2)
		{
			if (abs(item->item_flags[0]) <= 64)
				item->item_flags[0] = 0;
			else
				item->item_flags[0] -= item->item_flags[0] >> 6;
		}
	}

	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item->item_flags[0] > 3072)
		item->item_flags[0] = 3072;
	else if (item->item_flags[0] < -3072)
		item->item_flags[0] = -3072;

	if (item->item_flags[1] > 3072)
		item->item_flags[1] = 3072;
	else if (item->item_flags[1] < -3072)
		item->item_flags[1] = -3072;

	tyrot = item->pos.y_rot;

	if (item->item_flags[1] || item->item_flags[0])
		destyrot = (ushort)(phd_atan(item->item_flags[0], item->item_flags[0]));
	else
		destyrot = item->pos.y_rot;

	if (tyrot != destyrot)
	{
		if (((destyrot - tyrot) & 0x7FFF) >= 512)
		{
			if (destyrot <= tyrot || destyrot - tyrot >= 32768)
				item->pos.y_rot = tyrot - 512;
			else
				item->pos.y_rot = tyrot + 512;
		}
		else
			item->pos.y_rot = destyrot;
	}

	item->pos.x_rot -= (abs(item->item_flags[0]) + abs(item->item_flags[1])) >> 1;
	GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 1, 0);
}

void TrapDoorControl(short item_number)
{
	ITEM_INFO* item;
	short room_number;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->current_anim_state == 0 && item->trigger_flags >= 0)
			item->goal_anim_state = 1;
		else if (item->frame_number == anims[item->anim_number].frame_end && gfCurrentLevel == LVL5_RED_ALERT && item->object_number == TRAPDOOR1)
			item->status = ITEM_INVISIBLE;
	}
	else
	{
		item->status = ITEM_ACTIVE;

		if (item->current_anim_state == 1)
			item->goal_anim_state = 0;
	}

	AnimateItem(item);

	if (item->current_anim_state == 1 && (item->item_flags[2] || JustLoaded))
		OpenTrapDoor(item);
	else if (!item->current_anim_state && !item->item_flags[2])
		CloseTrapDoor(item);

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void FloorTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long y;

	item = &items[item_number];

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH
		&& lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(FloorTrapDoorBounds, item, l))
		{
			if (MoveLaraPosition(&FloorTrapDoorPos, item, l))
			{
				l->anim_number = ANIM_LIFTTRAP;
				l->frame_number = anims[ANIM_LIFTTRAP].frame_base;
				l->current_anim_state = AS_LIFTTRAP;
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				AddActiveItem(item_number);
				item->goal_anim_state = 1;
				item->status = ITEM_ACTIVE;
				UseForcedFixedCamera = 1;
				ForcedFixedCamera.x = item->pos.x_pos - ((2048 * phd_sin(item->pos.y_rot) >> W2V_SHIFT));
				ForcedFixedCamera.z = item->pos.z_pos - ((2048 * phd_cos(item->pos.y_rot) >> W2V_SHIFT));
				y = item->pos.y_pos - 2048;

				if (y < room[item->room_number].maxceiling)
					y = room[item->room_number].maxceiling;

				ForcedFixedCamera.y = y;
				ForcedFixedCamera.room_number = item->room_number;
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
	}
	else if (item->current_anim_state == 1)
		UseForcedFixedCamera = 0;

	if (item->current_anim_state == 1 && item->frame_number == anims[item->anim_number].frame_end)
		ObjectCollision(item_number, l, coll);
}

void CeilingTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short pass1, pass2;

	item = &items[item_number];

	pass1 = (short)TestLaraPosition(CeilingTrapDoorBounds, item, l);
	l->pos.y_rot += 32768;
	pass2 = (short)TestLaraPosition(CeilingTrapDoorBounds, item, l);
	l->pos.y_rot += 32768;

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_UPJUMP &&
		l->gravity_status && lara.gun_status == LG_NO_ARMS && (pass1 || pass2))
	{
		AlignLaraPosition(&CeilingTrapDoorPos, item, l);

		if (pass2)
			lara_item->pos.y_rot += 32768;

		lara.head_y_rot = 0;
		lara.head_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.gun_status = LG_HANDS_BUSY;
		l->gravity_status = 0;
		l->fallspeed = 0;
		l->anim_number = ANIM_PULLTRAP;
		l->frame_number = anims[ANIM_PULLTRAP].frame_base;
		l->current_anim_state = AS_PULLTRAP;
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
		item->goal_anim_state = 1;
		UseForcedFixedCamera = 1;
		ForcedFixedCamera.x = item->pos.x_pos - ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT);
		ForcedFixedCamera.y = item->pos.y_pos + 1024;
		ForcedFixedCamera.z = item->pos.z_pos - ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT);
		ForcedFixedCamera.room_number = item->room_number;
	}
	else if (item->current_anim_state == 1)
		UseForcedFixedCamera = 0;

	if (item->current_anim_state == 1 && item->frame_number == anims[item->anim_number].frame_end)
		ObjectCollision(item_number, l, coll);
}

void TrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->current_anim_state == 1 && item->frame_number == anims[item->anim_number].frame_end)
		ObjectCollision(item_number, l, coll);
}

void FallingBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long x, z, tx, tz;

	item = &items[item_number];
	x = l->pos.x_pos;
	z = l->pos.z_pos;
	tx = item->pos.x_pos;
	tz = item->pos.z_pos;

	if (!item->item_flags[0] && !item->trigger_flags && item->pos.y_pos == l->pos.y_pos && !((tx ^ x) & -1024) && !((z ^ tz) & -1024))
	{
		SoundEffect(SFX_ROCK_FALL_CRUMBLE, &item->pos, SFX_DEFAULT);
		AddActiveItem(item_number);
		item->item_flags[0] = 0;
		item->status = ITEM_ACTIVE;
		item->flags |= IFL_CODEBITS;
	}
}

void DrawScaledSpike(ITEM_INFO* item)
{
	OBJECT_INFO* object;
	ROOM_INFO* r;
	PHD_VECTOR v;
	short** meshpp;
	short* frmptr[2];
	long clip, rate;

	if ((item->object_number == TEETH_SPIKES && !item->item_flags[1]) || item->object_number == RAISING_BLOCK1 && item->trigger_flags <= -1)
		return;

	r = &room[item->room_number];
	phd_left = r->left;
	phd_right = r->right;
	phd_top = r->top;
	phd_bottom = r->bottom;
	GetFrames(item, frmptr, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotX(item->pos.x_rot);
	phd_RotZ(item->pos.z_rot);
	phd_RotY(item->pos.y_rot);
	clip = S_GetObjectBounds(frmptr[0]);

	if (clip)
	{
		object = &objects[item->object_number];
		meshpp = &meshes[object->mesh_index];

		if (item->object_number == CUTSCENE_ROPE)
		{
			v.x = 16384;
			v.y = 16384;
			v.z = item->item_flags[1] << 2;
		}
		else
		{
			v.x = 16384;
			v.y = item->item_flags[1] << 2;
			v.z = 16384;
		}

		ScaleCurrentMatrix(&v);
		CalculateObjectLighting(item, frmptr[0]);
		phd_PutPolygons(*meshpp, clip);
	}

	phd_left = 0;
	phd_right = phd_winwidth;
	phd_top = 0;
	phd_bottom = phd_winheight;
	phd_PopMatrix();
}

void FallingBlock(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags)
		item->trigger_flags--;
	else
	{
		if (!item->item_flags[0])
		{
			item->mesh_bits = -2;
			ExplodingDeath2(item_number, -1, 0x3BA1);
			item->item_flags[0]++;
		}
		else if (item->item_flags[0] >= 60)
			KillItem(item_number);
		else if (item->item_flags[0] >= 52)
		{
			item->item_flags[1] += 2;
			item->item_flags[0]++;
			item->pos.y_pos += item->item_flags[1];
		}
		else
		{
			if (!(GetRandomControl() % (0x3E - item->item_flags[0])))
				item->pos.y_pos += (GetRandomControl() & 3) + 1;

			item->item_flags[0]++;
		}
	}
}

void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long tx, tz;

	tx = x ^ item->pos.x_pos;
	tz = z ^ item->pos.z_pos;

	if (tx & ~1023 || tz & ~1023)
		return;

	if (y <= item->pos.y_pos)
	{
		*height = item->pos.y_pos;
		height_type = 0;
		OnObject = 1;
	}
}

void FallingBlockCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long tx, tz;

	tx = x ^ item->pos.x_pos;
	tz = z ^ item->pos.z_pos;

	if (tx & ~1023 || tz & ~1023)
		return;

	if (y > item->pos.y_pos)
		*height = item->pos.y_pos + 256;
}

void ControlScaledSpike(short item_number)
{
	ITEM_INFO* item;

	short* bounds;
	short* larabounds;
	long dx, dy, dz, num;
	short room_number, yt, yb, iyb1, iyb2, hit;

	item = &items[item_number];

	if (!TriggerActive(item) || item->item_flags[2])
	{
		if (TriggerActive(item))
		{
			item->item_flags[0] += (item->item_flags[0] >> 3) + 32;
			item->item_flags[1] -= item->item_flags[0];

			if (item->item_flags[1] < 0)
			{
				item->item_flags[0] = 1024;
				item->item_flags[1] = 0;
				item->status = ITEM_INVISIBLE;
			}

			if (item->trigger_flags & 32)
				item->item_flags[2] = 1;
			else if (item->item_flags[2])
				item->item_flags[2]--;
		}
		else if (!item->timer)
		{
			item->item_flags[0] += (item->item_flags[0] >> 3) + 32;

			if (item->item_flags[1] > 0)
			{
				item->item_flags[1] -= item->item_flags[0];

				if (item->item_flags[1] < 0)
					item->item_flags[1] = 0;
			}
		}
	}
	else
	{
		if (item->item_flags[0] == 1024)
			SoundEffect(SFX_TEETH_SPIKES, &item->pos, SFX_DEFAULT);

		item->status = ITEM_ACTIVE;
		hit = (short)TestBoundsCollideTeethSpikes(item);

		if (lara_item->hit_points > 0 && hit)
		{
			bounds = GetBestFrame(item);
			larabounds = GetBestFrame(lara_item);
			num = 0;

			if ((item->item_flags[0] > 1024 || lara_item->gravity_status) && (item->trigger_flags & 7) > 2 && (item->trigger_flags & 7) < 6)
			{
				if (lara_item->fallspeed > 6 || item->item_flags[0] > 1024)
				{
					lara_item->hit_points = -1;
					num = 20;
				}
			}
			else if (lara_item->speed < 30)
				num = 0;
			else
			{
				lara_item->hit_points -= 8;
				num = (GetRandomControl() & 3) + 2;
			}

			yt = (short)(item->pos.y_pos + larabounds[2]);
			yb = (short)(item->pos.y_pos + larabounds[3]);

			if ((item->trigger_flags & 0xF) == 8 || !(item->trigger_flags & 0xF))
			{
				iyb1 = -bounds[3];
				iyb2 = -bounds[2];
			}
			else
			{
				iyb1 = bounds[2];
				iyb2 = bounds[3];
			}

			if (yt < item->pos.y_pos + iyb1)
				yt = (short)(iyb1 + item->pos.y_pos);

			if (yb > item->pos.y_pos + iyb2)
				yb = (short)(iyb2 + item->pos.y_pos);

			dy = abs(yt - yb) + 1;

			if ((item->trigger_flags & 7) == 2 || (item->trigger_flags & 7) == 6)
				num >>= 1;

			while (num > 0)
			{
				dx = (GetRandomControl() & 0x7F) + lara_item->pos.x_pos - 64;
				dz = (GetRandomControl() & 0x7F) + lara_item->pos.z_pos - 64;
				TriggerBlood(dx, yb - GetRandomControl() % dy, dz, GetRandomControl() << 1, 1);
				num--;
			}

			if (lara_item->hit_points <= 0)
			{
				room_number = lara_item->room_number;
				dy = GetHeight(GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number),
					lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

				if (item->pos.y_pos >= lara_item->pos.y_pos && dy - lara_item->pos.y_pos < 50)
				{
					lara_item->anim_number = ANIM_SPIKED;
					lara_item->frame_number = anims[ANIM_SPIKED].frame_base;
					lara_item->current_anim_state = AS_DEATH;
					lara_item->goal_anim_state = AS_DEATH;
					lara_item->gravity_status = 0;
				}
			}
		}

		item->item_flags[0] += 128;
		item->item_flags[1] += item->item_flags[0];

		if (item->item_flags[1] >= 5120)
		{
			item->item_flags[1] = 5120;

			if (item->item_flags[0] <= 1024)
			{
				item->item_flags[0] = 0;

				if (!(item->trigger_flags & 16) && lara_item->hit_points > 0)
					item->item_flags[2] = 64;
			}
			else
				item->item_flags[0] = -item->item_flags[0] >> 1;
		}
	}
}

void ControlRaisingBlock(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (!item->item_flags[2])
		{
			if (item->object_number == RAISING_BLOCK1)
			{
				if (item->trigger_flags == -1)
					AlterFloorHeight(item, -255);
				else if (item->trigger_flags == -3)
					AlterFloorHeight(item, -1023);
				else
					AlterFloorHeight(item, -1024);
			}
			else if (item->object_number == RAISING_BLOCK2)
				AlterFloorHeight(item, -2048);

			item->item_flags[2] = 1;
		}

		if (item->trigger_flags < 0)
			item->item_flags[1] = 1;
		else
		{
			if (item->item_flags[1] < 4096)
			{
				SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, 0);
				item->item_flags[1] += 64;

				if (item->trigger_flags > 0 && abs(item->pos.x_pos - camera.pos.x) < 10240 && 
					abs(item->pos.y_pos - camera.pos.y) < 10240 && abs(item->pos.z_pos - camera.pos.z) < 10240)
				{
					if (item->item_flags[1] == 64 || item->item_flags[1] == 4096)
						camera.bounce = -32;
					else
						camera.bounce = -16;
				}
			}
		}
	}
	else
	{
		if (item->item_flags[1] <= 0 || item->trigger_flags < 0)
		{
			if (item->item_flags[2])
			{
				item->item_flags[1] = 0;

				if (item->object_number == RAISING_BLOCK1)
				{
					if (item->trigger_flags == -1)
						AlterFloorHeight(item, 255);
					else if (item->trigger_flags == -3)
						AlterFloorHeight(item, 1023);
					else
						AlterFloorHeight(item, 1024);
				}
				else if (item->object_number == RAISING_BLOCK2)
					AlterFloorHeight(item, 2048);

				item->item_flags[2] = 0;
			}
		}
		else
		{
			SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, 0);

			if (item->trigger_flags >= 0 && abs(item->pos.x_pos - camera.pos.x) < 10240 &&
				abs(item->pos.y_pos - camera.pos.y) < 10240 && abs(item->pos.z_pos - camera.pos.z) < 10240)
			{
				if (item->item_flags[1] == 64 || item->item_flags[1] == 4096)
					camera.bounce = -32;
				else
					camera.bounce = -16;
			}

			item->item_flags[1] -= 64;
		}
	}
}

void ControlTwoBlockPlatform(short item_number)
{
	ITEM_INFO* item;
	long height;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags)
	{
		if (item->pos.y_pos > item->item_flags[0] - (item->trigger_flags << 4))
			item->pos.y_pos -= item->trigger_flags & 0xF;

		room_number = item->room_number;
		item->floor = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
			item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);
	}
	else
	{
		OnObject = 0;
		height = lara_item->pos.y_pos + 1;
		TwoBlockPlatformFloor(item, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &height);

		if (!OnObject || lara_item->anim_number == 89)
			item->item_flags[1] = -1;
		else
			item->item_flags[1] = 1;

		if (item->item_flags[1] > 0)
		{
			if (item->pos.y_pos >= item->item_flags[0] + 128)
				item->item_flags[1] = -1;
			else
			{
				SoundEffect(SFX_2GUNTEX_FALL_BIG, &item->pos, SFX_DEFAULT);
				item->pos.y_pos += 4;
			}
		}
		else if (item->item_flags[1] < 0)
		{
			if (item->pos.y_pos <= item->item_flags[0])
				item->item_flags[1] = 1;
			else
			{
				SoundEffect(SFX_2GUNTEX_FALL_BIG, &item->pos, SFX_DEFAULT);
				item->pos.y_pos -= 4;
			}
		}
	}
}

static long OnTwoBlockPlatform(ITEM_INFO* item, long x, long z)
{
	long tx, tz;

	if (!item->mesh_bits)
		return 0;

	x >>= 10;
	z >>= 10;
	tx = item->pos.x_pos >> 10;
	tz = item->pos.z_pos >> 10;

	if (!item->pos.y_rot && (x == tx || x == tx - 1) && (z == tz || z == tz + 1))
		return 1;

	if (item->pos.y_rot == 0x8000 && (x == tx || x == tx + 1) && (z == tz || z == tz - 1))
		return 1;

	if (item->pos.y_rot == 0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx + 1))
		return 1;

	if (item->pos.y_rot == -0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx - 1))
		return 1;

	return 0;
}

void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y <= item->pos.y_pos + 32 && item->pos.y_pos < *height)
		{
			*height = item->pos.y_pos;
			OnObject = 1;
			height_type = 0;
		}
	}
}

void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y > item->pos.y_pos + 32 && item->pos.y_pos > *height)
			*height = item->pos.y_pos + 256;
	}
}

void FallingCeiling(short item_number)
{
	ITEM_INFO* item;
	short room_number;

	item = &items[item_number];

	if (item->current_anim_state == 0)
	{
		item->gravity_status = 1;
		item->goal_anim_state = 1;
	}
	else if (item->current_anim_state == 1 && item->touch_bits)
	{
		lara_item->hit_points -= 300;
		lara_item->hit_status = 1;
	}

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
		RemoveActiveItem(item_number);
	else
	{
		room_number = item->room_number;
		item->floor = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
			item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);

		if (item->current_anim_state == 1 && item->pos.y_pos >= item->floor)
		{
			item->pos.y_pos = item->floor;
			item->goal_anim_state = 2;
			item->gravity_status = 0;
			item->fallspeed = 0;
		}
	}
}

long TestBoundsCollideTeethSpikes(ITEM_INFO* item)
{
	short* bounds;
	long x, y, z, rad, xMin, xMax, zMin, zMax;

	if (item->trigger_flags & 8)
	{
		x = item->pos.x_pos & ~0x3FF | 0x200;
		z = (item->pos.z_pos + SPxzoffs[item->trigger_flags & 7]) & ~0x3FF | 0x200;
	}
	else
	{
		x = (item->pos.x_pos - SPxzoffs[item->trigger_flags & 7]) & ~0x3FF | 0x200;
		z = item->pos.z_pos & ~0x3FF | 0x200;
	}

	if (item->trigger_flags & 1)
		rad = 300;
	else
		rad = 480;

	y = item->pos.y_pos + SPDETyoffs[item->trigger_flags & 7];
	bounds = GetBestFrame(lara_item);

	if (lara_item->pos.y_pos + bounds[2] > y || lara_item->pos.y_pos + bounds[3] < y - 900)
		return 0;

	xMin = lara_item->pos.x_pos + bounds[0];
	xMax = lara_item->pos.x_pos + bounds[1];
	zMin = lara_item->pos.z_pos + bounds[4];
	zMax = lara_item->pos.z_pos + bounds[5];
	return xMin <= x + rad && xMax >= x - rad && zMin <= z + rad && zMax >= z - rad;
}
