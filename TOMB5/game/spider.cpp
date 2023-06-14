#include "../tomb5/pch.h"
#include "spider.h"
#include "../specific/LoadSave.h"
#include "control.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "tomb4fx.h"
#include "sound.h"
#include "../specific/function_stubs.h"
#include "lara.h"
#include "effect2.h"

SPIDER_STRUCT* Spiders;
static long next_spider;

long GetFreeSpider()
{
	SPIDER_STRUCT* fx;
	long lp, free;

	free = next_spider;
	lp = 0;
	fx = &Spiders[free];

	while (fx->On)
	{
		if (free == 63)
		{
			free = 0;
			fx = Spiders;
		}
		else
		{
			free++;
			fx++;
		}

		lp++;

		if (lp >= 64)
			return -1;
	}

	next_spider = (free + 1) & 0x3F;
	return free;
}

void ClearSpiders()
{
	if (objects[SPIDER].loaded)
	{
		memset(Spiders, 0, 0x680);
		next_spider = 0;
		flipeffect = -1;
	}
}

void TriggerSpider(short item_number)
{
	SPIDER_STRUCT* fx;
	ITEM_INFO* item;
	short fx_number;

	item = &items[item_number];

	if (item->trigger_flags && (!item->item_flags[2] || !(GetRandomControl() & 0xF)))
	{
		item->trigger_flags--;

		if (item->item_flags[2] && GetRandomControl() & 0x1)
			item->item_flags[2]--;

		fx_number = (short) GetFreeSpider();

		if (fx_number != -1)
		{
			fx = &Spiders[fx_number];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->room_number = item->room_number;

			if (item->item_flags[0])
			{
				fx->pos.y_rot = short(2 * GetRandomControl());
				fx->fallspeed = -16 - (GetRandomControl() & 0x1F);
			}
			else
			{
				fx->fallspeed = 0;
				fx->pos.y_rot = (GetRandomControl() & 0x3FFF) + item->pos.y_rot - 8192;
			}

			fx->pos.x_rot = 0;
			fx->pos.z_rot = 0;
			fx->On = 1;
			fx->flags = 0;
			fx->speed = (GetRandomControl() & 0x1F) + 1;
		}
	}
}

void UpdateSpiders()
{
	SPIDER_STRUCT* fx;
	FLOOR_INFO* floor;
	long h, dx, dy, dz, oldx, oldy, oldz;
	short angle;

	if (objects[SPIDER].loaded)
	{
		for (int i = 0; i < 64; i++)
		{
			fx = &Spiders[i];

			if (fx->On)
			{
				oldx = fx->pos.x_pos;
				oldy = fx->pos.y_pos;
				oldz = fx->pos.z_pos;
				fx->pos.x_pos += fx->speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
				fx->pos.y_pos += fx->fallspeed;
				fx->pos.z_pos += fx->speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
				fx->fallspeed += 6;
				dx = lara_item->pos.x_pos - fx->pos.x_pos;
				dy = lara_item->pos.y_pos - fx->pos.y_pos;
				dz = lara_item->pos.z_pos - fx->pos.z_pos;
				angle = (short)phd_atan(dz, dx) - fx->pos.y_rot;

				if (abs(dz) < 85 && abs(dy) < 85 && abs(dx) < 85)
				{
					lara_item->hit_points -= 3;
					lara_item->hit_status = 1;
					TriggerBlood(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->pos.y_rot, 1);
				}

				if (fx->flags)
				{
					if (abs(dx) + abs(dz) > 768)
					{
						if (fx->speed < (i & 0x1F) + 24)
							fx->speed++;

						if (abs(angle) < 2048)
							fx->pos.y_rot += short(8 * (wibble - i));
						else if (angle < 0)
							fx->pos.y_rot -= 1024;
						else
							fx->pos.y_rot += 1024;
					}
					else
					{
						fx->pos.y_rot += fx->speed & 1 ? 512 : -512;
						fx->speed = 48 - (abs(angle) >> 10);
					}
				}

				floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &fx->room_number);
				h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

				if (h < fx->pos.y_pos - 1280 && h != NO_HEIGHT)
				{
					fx->pos.y_rot += angle > 0 ? 16384 : -16384;
					fx->pos.x_pos = oldx;
					fx->pos.y_pos = oldy;
					fx->pos.z_pos = oldz;
					fx->fallspeed = 0;
				}
				else if (h < fx->pos.y_pos - 64)
				{
					fx->pos.x_rot = 14336;
					fx->pos.x_pos = oldx;
					fx->pos.y_pos = oldy - 8;

					if (!(GetRandomControl() & 0x1F))
						fx->pos.y_rot += -32768;

					fx->fallspeed = 0;
					fx->pos.z_pos = oldz;
				}
				else if (h < fx->pos.y_pos)
				{
					fx->pos.y_pos = h;
					fx->fallspeed = 0;
					fx->flags = 1;
				}
				else if (fx->fallspeed < 500)
					fx->pos.x_rot = -128 * fx->fallspeed;
				else
				{
					fx->On = 0;
					next_spider = 0;
				}

				if (fx->pos.y_pos < room[fx->room_number].maxceiling + 50)
				{
					fx->fallspeed = 1;
					fx->pos.y_rot += -32768;
					fx->pos.y_pos = room[fx->room_number].maxceiling + 50;
				}

				if (!i && !(GetRandomControl() & 4))
					SoundEffect(SFX_BEETLES, &fx->pos, 0);
			}
		}
	}
}

void DrawSpiders()
{
	SPIDER_STRUCT* fx;
	short** meshpp;

	if (objects[SPIDER].loaded)
	{
		for (int i = 0; i < 64; i++)
		{
			fx = &Spiders[i];

			if (fx->On)
			{
				meshpp = &meshes[(wibble >> 2 & 2) + objects[SPIDER].mesh_index];
				phd_PushMatrix();
				phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
				phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
				phd_PutPolygons_train(*meshpp, 0);
				phd_PopMatrix();
			}
		}
	}
}
