#include "../tomb5/pch.h"
#include "spider.h"
#include "../specific/LoadSave.h"
#include "control.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "tomb4fx.h"
#include "sound.h"

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
		S_MemSet(Spiders, 0, 1664);
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
				fx->pos.y_rot = 2 * GetRandomControl();
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
	//long lp;
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
				fx->pos.x_pos += fx->speed * phd_sin(fx->pos.y_rot) >> 14;
				fx->pos.y_pos += fx->fallspeed;
				fx->fallspeed += 6;
				fx->pos.z_pos += fx->speed * phd_cos(fx->pos.y_rot) >> 14;
				dx = lara_item->pos.x_pos - fx->pos.x_pos;
				dy = lara_item->pos.y_pos - fx->pos.y_pos;
				dz = lara_item->pos.z_pos - fx->pos.z_pos;
				angle = (short) phd_atan(dz, dx) - fx->pos.y_rot;

				if (ABS(dz) < 85 && ABS(dy) < 85 && ABS(dx) < 85)
				{
					lara_item->hit_points -= 3;
					lara_item->hit_status = 1;
					TriggerBlood(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->pos.y_rot, 1);
				}

				if (fx->flags)
				{
					if (ABS(dx) + ABS(dz) > 768)
					{
						if (fx->speed < (i & 0x1F) + 24)
							fx->speed++;

						if (ABS(angle) < 2048)
							fx->pos.y_rot += (short) (8 * (wibble - i));
						else if (angle < 0)
							fx->pos.y_rot -= 1024;
						else
							fx->pos.y_rot += 1024;
					}
					else
					{
						fx->pos.y_rot += fx->speed & 1 ? 512 : -512;
						fx->speed = 48 - (ABS(angle) >> 10);
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
	//long lp;

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

void inject_spider(bool replace)
{
	INJECT(0x0047A140, GetFreeSpider, replace);
	INJECT(0x0047A1B0, ClearSpiders, replace);
	INJECT(0x0047A200, TriggerSpider, replace);
	INJECT(0x0047A340, UpdateSpiders, replace);
	INJECT(0x0047A730, DrawSpiders, replace);
}
