#include "../tomb5/pch.h"
#include "bat.h"
#include "control.h"
#include "objects.h"
#include "../specific/LoadSave.h"
#include "items.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"
#include "sound.h"
#include "../specific/output.h"
#include "../specific/function_stubs.h"

long GetFreeBat()
{
	BAT_STRUCT* fx;

	fx = &Bats[next_bat];

	for (int free = next_bat, i = 0; i < 64; i++)
	{
		if (fx->On)
		{
			if (free == 63)
			{
				fx = Bats;
				free = 0;
			}
			else
			{
				free++;
				fx++;
			}
		}
		else
		{
			next_bat = free + 1;
			next_bat &= 63;
			return free;
		}
	}

	return -1;
}

void TriggerBat(ITEM_INFO* item)
{
	BAT_STRUCT* fx;
	PHD_VECTOR vec2;
	long fx_number;
	short angle;

	fx_number = GetFreeBat();

	if (fx_number == -1)
		return;

	fx = &Bats[fx_number];
	vec2.x = item->pos.x_pos;
	vec2.y = item->pos.y_pos;
	vec2.z = item->pos.z_pos;
	angle = item->pos.y_rot + 0x8000;
	fx->room_number = item->room_number;
	fx->pos.x_pos = vec2.x;
	fx->pos.y_pos = vec2.y;
	fx->pos.z_pos = vec2.z;
	fx->pos.y_rot = (GetRandomControl() & 0x7FF) + angle - 1024;
	fx->pos.x_rot = (GetRandomControl() & 0x3FF) - 512;
	fx->On = 1;
	fx->flags = 0;
	fx->speed = (GetRandomControl() & 0x1F) + 16;
	fx->LaraTarget = GetRandomControl() & 0x1FF;
	fx->Counter = 20 * ((GetRandomControl() & 7) + 15);
}

void InitialiseBatEmitter(short item_number)
{
	ITEM_INFO* item;
	
	item = &items[item_number];

	if (!item->pos.y_rot)
		item->pos.z_pos += 512;
	else if (item->pos.y_rot == 16384)
		item->pos.x_pos += 512;
	else if (item->pos.y_rot == -32768)
		item->pos.z_pos -= 512;
	else if (item->pos.y_rot == -16384)
		item->pos.x_pos -= 512;

	if (objects[BAT].loaded)//most likely an inlined ClearBats function, similar to the rats, because this check just makes no sense here
		S_MemSet(Bats, 0, 0x780);
}

void ControlBatEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags)
		{
			TriggerBat(item);
			item->trigger_flags--;
		}
		else
			KillItem(item_number);
	}
}

void UpdateBats()
{
	BAT_STRUCT* fx;
	short* lb;
	long bounds[6];
	long speed, ox, oy, oz, closestdist, closestnum;
	short angles[2];
	short max_turn;
	
	if (!objects[BAT].loaded)
		return;

	lb = GetBoundsAccurate(lara_item);
	bounds[0] = lb[0] - (lb[0] >> 2) + lara_item->pos.x_pos;
	bounds[1] = lb[1] - (lb[1] >> 2) + lara_item->pos.x_pos;
	bounds[2] = lb[2] - (lb[2] >> 2) + lara_item->pos.y_pos;
	bounds[3] = lb[3] - (lb[3] >> 2) + lara_item->pos.y_pos;
	bounds[4] = lb[4] - (lb[4] >> 2) + lara_item->pos.z_pos;
	bounds[5] = lb[5] - (lb[5] >> 2) + lara_item->pos.z_pos;
	closestdist = 0xFFFFFFF;
	closestnum = -1;

	for (int i = 0; i < 64; i++)
	{
		fx = &Bats[i];

		if (fx->On)
		{
			if ((lara.burn || lara_item->hit_points <= 0) && fx->Counter > 90 && !(GetRandomControl() & 7))
				fx->Counter = 90;

			fx->Counter--;

			if (!fx->Counter)
			{
				fx->On = 0;
				continue;
			}

			if (!(GetRandomControl() & 7))
			{
				fx->LaraTarget = (GetRandomControl() % 640) + 128;
				fx->XTarget = (GetRandomControl() & 127) - 64;
				fx->ZTarget = (GetRandomControl() & 127) - 64;
			}

			phd_GetVectorAngles(
				lara_item->pos.x_pos + (fx->XTarget << 3) - fx->pos.x_pos,
				lara_item->pos.y_pos - fx->LaraTarget - fx->pos.y_pos,
				lara_item->pos.z_pos + (fx->ZTarget << 3) - fx->pos.z_pos,
				&angles[0]);
			ox = SQUARE(lara_item->pos.x_pos - fx->pos.x_pos);
			oz = SQUARE(lara_item->pos.z_pos - fx->pos.z_pos);

			if (ox + oz < closestdist)
			{
				closestdist = ox + oz;
				closestnum = i;
			}

			ox = phd_sqrt(ox + oz) >> 3;

			if (ox > 128)
				ox = 128;
			else if (ox < 48)
				ox = 48;

			if (fx->speed < ox)
				fx->speed++;
			else if (fx->speed > ox)
				fx->speed--;

			if (fx->Counter > 90)
			{
				max_turn = fx->speed << 7;

				oy = (ushort)angles[0] - (ushort)fx->pos.y_rot;

				if (abs(oy) > 32768)
					oy = (ushort)fx->pos.y_rot - (ushort)angles[0];

				ox = (ushort)angles[1] - (ushort)fx->pos.x_rot;

				if (abs(ox) > 32768)
					ox = (ushort)fx->pos.x_rot - (ushort)angles[0];

				ox >>= 3;
				oy >>= 3;

				if (oy > max_turn)
					oy = max_turn;
				else if (oy < -max_turn)
					oy = -max_turn;
				if (ox > max_turn)
					ox = max_turn;
				else if (ox < -max_turn)
					ox = -max_turn;

				fx->pos.y_rot += (short)oy;
				fx->pos.x_rot += (short)ox;
			}

			ox = fx->pos.x_pos;
			oy = fx->pos.y_pos;
			oz = fx->pos.z_pos;
			speed = fx->speed * phd_cos(fx->pos.x_rot) >> 14;
			fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot) >> 14);
			fx->pos.y_pos += (fx->speed * phd_sin(-fx->pos.x_rot) >> 14);
			fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot) >> 14);

			if (!(i & 1))
			{
				if (fx->pos.x_pos > bounds[0] && fx->pos.x_pos < bounds[1] && fx->pos.y_pos > bounds[2] &&
					fx->pos.y_pos < bounds[3] && fx->pos.z_pos > bounds[4] && fx->pos.z_pos < bounds[5])
				{
					TriggerBlood(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, GetRandomControl() << 1, 2);

					if (lara_item->hit_points > 0)
						lara_item->hit_points -= 2;
				}
			}
		}
	}

	if (closestnum != -1)
	{
		fx = &Bats[closestnum];

		if (!(GetRandomControl() & 0x4))
			SoundEffect(SFX_BATS_1, &fx->pos, SFX_DEFAULT);
	}
}

void DrawBats()
{
	BAT_STRUCT* fx;
	short** meshpp;

	if (objects[BAT].loaded)
	{
		for (int i = 0; i < 64; i++)
		{
			fx = &Bats[i];

			if (fx->On)
			{
				meshpp = &meshes[objects[BAT].mesh_index + (((GlobalCounter * 3) & 3) << 1)];
				phd_PushMatrix();
				phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
				phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
				phd_PutPolygons_train(*meshpp, 0);
				phd_PopMatrix();
			}
		}
	}
}

void inject_bats(bool replace)
{
	INJECT(0x00407D30, GetFreeBat, replace);
	INJECT(0x00407DD0, TriggerBat, replace);
	INJECT(0x00407EC0, InitialiseBatEmitter, replace);
	INJECT(0x00407F50, ControlBatEmitter, replace);
	INJECT(0x00407FD0, UpdateBats, replace);
	INJECT(0x00408480, DrawBats, replace);
}
