#include "../tomb5/pch.h"
#include "rat.h"
#include "objects.h"
#include "../specific/LoadSave.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "sound.h"
#include "../specific/output.h"
#include "../specific/function_stubs.h"
#include "lara.h"

RAT_STRUCT* Rats;
static long next_rat = 0;

long GetFreeRat()
{
	RAT_STRUCT* fx;
	long free;

	fx = &Rats[next_rat];
	free = next_rat;

	for (int i = 0; i < 32; i++)
	{
		if (!fx->On)
		{
			next_rat = free + 1;
			next_rat &= 31;
			return free;
		}

		if (free == 31)
		{
			fx = Rats;
			free = 0;
		}
		else
		{
			free++;
			fx++;
		}
	}

	return -1;
}

void ClearRats()
{
	if (objects[RAT].loaded)
	{
		memset(Rats, 0, 0x340);
		next_rat = 0;
		flipeffect = -1;
	}
}

void TriggerRat(short item_number)
{
	ITEM_INFO* item;
	RAT_STRUCT* fx;
	long fx_number;

	item = &items[item_number];

	if (item->trigger_flags && (!item->item_flags[2] || !(GetRandomControl() & 0xF)))
	{
		item->trigger_flags--;

		if (item->item_flags[2] && GetRandomControl() & 1)
			item->item_flags[2]--;

		fx_number = GetFreeRat();

		if (fx_number != -1)
		{
			fx = &Rats[fx_number];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->room_number = item->room_number;

			if (item->item_flags[0])
			{
				fx->pos.y_rot = short(GetRandomControl() << 1);
				fx->fallspeed = -16 - (GetRandomControl() & 31);
			}
			else
			{
				fx->fallspeed = 0;
				fx->pos.y_rot = item->pos.y_rot + (GetRandomControl() & 0x3FFF) - 0x2000;
			}

			fx->pos.x_rot = 0;
			fx->pos.z_rot = 0;
			fx->On = 1;
			fx->flags = GetRandomControl() & 0x1E;
			fx->speed = (GetRandomControl() & 0x1F) + 1;
		}
	}
}

void UpdateRats()
{
	RAT_STRUCT* fx;
	FLOOR_INFO* floor;
	long h, dx, dy, dz, oldx, oldy, oldz;
	short angle, old_room;

	if (!objects[RAT].loaded)
		return;

	for (int i = 0; i < 32; i++)
	{
		fx = &Rats[i];

		if (!fx->On)
			continue;

		oldx = fx->pos.x_pos;
		oldy = fx->pos.y_pos;
		oldz = fx->pos.z_pos;
		fx->pos.x_pos += (phd_sin(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;
		fx->pos.y_pos += fx->fallspeed;
		fx->pos.z_pos += (phd_cos(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;
		fx->fallspeed += 6;
		dz = lara_item->pos.z_pos - fx->pos.z_pos;
		dy = lara_item->pos.y_pos - fx->pos.y_pos;
		dx = lara_item->pos.x_pos - fx->pos.x_pos;

		if (fx->flags >= 170)
			angle = short(fx->pos.y_rot - phd_atan(dz, dx));
		else
			angle = short(phd_atan(dz, dx) - fx->pos.y_rot);

		if (abs(dz) < 85 && abs(dy) < 85 && abs(dx) < 85)
		{
			lara_item->hit_points--;
			lara_item->hit_status = 1;
		}

		if (fx->flags & 1)
		{
			if (abs(dz) + abs(dx) <= 1024)
			{
				if (fx->speed & 1)
					fx->pos.y_rot += 512;
				else
					fx->pos.y_rot -= 512;

				fx->speed = 48 - (abs(angle) >> 10);
			}
			else
			{
				if (fx->speed < (i & 0x1F) + 24)
					fx->speed++;

				if (abs(angle) < 2048)
					fx->pos.y_rot += (short)((wibble - i) << 3);
				else
				{
					if (angle >= 0)
						fx->pos.y_rot += 1024;
					else
						fx->pos.y_rot -= 1024;
				}
			}
		}

		old_room = fx->room_number;
		floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &fx->room_number);
		h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

		if (h < fx->pos.y_pos - 1280 || h == NO_HEIGHT)
		{
			if (fx->flags > 170)
			{
				fx->On = 0;
				next_rat = 0;
			}

			if (angle <= 0)
				fx->pos.y_rot -= 0x4000;
			else
				fx->pos.y_rot += 0x4000;

			fx->pos.x_pos = oldx;
			fx->pos.y_pos = oldy;
			fx->pos.z_pos = oldz;
			fx->fallspeed = 0;
		}
		else if (h < fx->pos.y_pos - 64)
		{
			fx->pos.x_rot = 0x3800;
			fx->pos.x_pos = oldx;
			fx->pos.y_pos = oldy - 24;
			fx->pos.z_pos = oldz;
			fx->fallspeed = 0;
		}
		else if (fx->pos.y_pos > h)
		{
			fx->pos.y_pos = h;
			fx->fallspeed = 0;
			fx->flags |= 1;
		}
		else if (fx->fallspeed < 500 && fx->flags < 200)
			fx->pos.x_rot = -(fx->fallspeed << 7);
		else
		{
			fx->On = 0;
			next_rat = 0;
		}

		if (!(wibble & 0x3C))
			fx->flags += 2;

		if (room[fx->room_number].flags & ROOM_UNDERWATER)
		{
			fx->fallspeed = 0;
			fx->speed = 16;
			fx->pos.y_pos = room[fx->room_number].maxceiling + 50;

			if (!(room[old_room].flags & ROOM_UNDERWATER))
			{
				TriggerSmallSplash(fx->pos.x_pos, room[fx->room_number].maxceiling, fx->pos.z_pos, 16);
				SetupRipple(fx->pos.x_pos, room[fx->room_number].maxceiling, fx->pos.z_pos, (GetRandomControl() & 3) + 48, 2);
				SoundEffect(SFX_RATSPLASH, &fx->pos, 0);
			}
			else if (!(GetRandomControl() & 0xF))
				SetupRipple(fx->pos.x_pos, room[fx->room_number].maxceiling, fx->pos.z_pos, (GetRandomControl() & 3) + 48, 2);
		}

		if (!i)
		{
			if (!(GetRandomControl() & 4))
				SoundEffect(SFX_RATS_1, &fx->pos, SFX_DEFAULT);
		}
	}
}

void DrawRats()
{
	RAT_STRUCT* fx;
	short** meshpp;
	
	if (objects[RAT].loaded)
	{
		for (int i = 0; i < 32; i++)
		{
			fx = &Rats[i];

			if (!fx->On)
				continue;

			meshpp = &meshes[objects[RAT].mesh_index + (((wibble + (i << 2)) & 0x38) >> 2)];
			phd_PushMatrix();
			phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			phd_PutPolygons_train(*meshpp, 0);
			phd_PopMatrix();
		}
	}
}

void InitialiseRatGenerator(short item_number)
{
	ITEM_INFO* item;
	short tf;

	item = &items[item_number];
	tf = item->trigger_flags / 1000;
	item->item_flags[0] = tf & 1;
	item->item_flags[1] = tf & 2;
	item->item_flags[2] = tf & 4;
	item->pos.x_rot = 8192;
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

	ClearRats();
}
