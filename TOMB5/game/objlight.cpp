#include "../tomb5/pch.h"
#include "objlight.h"
#include "control.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "sphere.h"
#include "../specific/3dmath.h"

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, long angle, long room_no, long falloff)
{
	GAME_VECTOR src, target;
	long s, c;

	src.x = x;
	src.y = y;
	src.z = z;
	GetFloor(x, y, z, (short*)&room_no);
	src.room_number = (short)room_no;
	s = phd_sin(angle << 4);
	c = phd_cos(angle << 4);
	target.x = x + s;
	target.y = y;
	target.z = z + c;

	if (!LOS(&src, &target))
		TriggerDynamic(target.x, target.y, target.z, falloff, r, g, b);
}

void ControlStrobeLight(short item_number)
{
	ITEM_INFO* item;
	long angle, s, c, r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->pos.y_rot += 2912;
	r = (item->trigger_flags & 0x1F) << 3;
	g = (item->trigger_flags >> 2) & 0xF8;
	b = (item->trigger_flags >> 7) & 0xF8;
	angle = ((item->pos.y_rot + 0x5800) >> 4) & 0xFFF;
	s = rcossin_tbl[angle << 1] >> 4;
	c = rcossin_tbl[(angle << 1) + 1] >> 4;
	TriggerAlertLight(item->pos.x_pos, item->pos.y_pos - 512, item->pos.z_pos, r, g, b, angle, item->room_number, 12);
	TriggerDynamic(item->pos.x_pos + s, item->pos.y_pos - 768, item->pos.z_pos + c, 8, r, g, b);
}

void ControlPulseLight(short item_number)
{
	ITEM_INFO* item;
	long s, r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->item_flags[0] -= 1024;
	
	s = abs(phd_sin(item->item_flags[0] + ((item->pos.y_pos & 0x3FFF) << 2)) >> 6);

	if (s > 255)
		s = 255;

	r = (s * ((item->trigger_flags & 0x1F) << 3)) >> 9;
	g = (s * ((item->trigger_flags >> 2) & 0xF8)) >> 9;
	b = (s * ((item->trigger_flags >> 7) & 0xF8)) >> 9;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, r, g, b);
}

void ControlColouredLight(short item_number)
{
	ITEM_INFO* item;
	long r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	r = (item->trigger_flags & 0x1F) << 3;
	g = (item->trigger_flags >> 2) & 0xF8;
	b = (item->trigger_flags >> 7) & 0xF8;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, r, g, b);
}

void ControlElectricalLight(short item_number)
{
	ITEM_INFO* item;
	long shade, r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		item->item_flags[0] = 0;
		return;
	}

	if (item->trigger_flags > 0)
	{
		if (item->item_flags[0] < 16)
		{
			shade = (GetRandomControl() & 0x3F) << 2;
			item->item_flags[0]++;
		}
		else if (item->item_flags[0] >= 96)
		{
			if (item->item_flags[0] >= 160)
				shade = 255 - (GetRandomControl() & 0x1F);
			else
			{
				shade = 96 - (GetRandomControl() & 0x1F);

				if (!(GetRandomControl() & 0x1F) && item->item_flags[0] > 128)
					item->item_flags[0] = 160;
				else
					item->item_flags[0]++;
			}
		}
		else
		{
			if (wibble & 0x3F && GetRandomControl() & 7)
				shade = GetRandomControl() & 0x3F;
			else
				shade = 192 - (GetRandomControl() & 0x3F);

			item->item_flags[0]++;
		}
	}
	else
	{
		if (item->item_flags[0] <= 0)
		{
			item->item_flags[0] = (GetRandomControl() & 3) + 4;
			item->item_flags[1] = (GetRandomControl() & 0x7F) + 128;
			item->item_flags[2] = GetRandomControl() & 1;
		}

		item->item_flags[0]--;

		if (item->item_flags[2])
			return;

		item->item_flags[0]--;
		shade = item->item_flags[1] - (GetRandomControl() & 0x7F);

		if (shade > 64)
			SoundEffectCS(SFX_ELEC_LIGHT_CRACKLES, &item->pos, ((shade >> 3) << 5) | SFX_SETVOL);
	}

	r = ((shade * (item->trigger_flags & 0x1F)) << 3) >> 8;
	g = (shade * ((item->trigger_flags >> 2) & 0xF8)) >> 8;
	b = (shade * ((item->trigger_flags >> 7) & 0xF8)) >> 8;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, r, g, b);
}

void ControlBlinker(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	long r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->item_flags[0]--;

	if (item->item_flags[0] >= 3)
		item->mesh_bits = 1;
	else
	{
		pos.z = 0;
		pos.y = 0;
		pos.x = 0;
		GetJointAbsPosition(item, &pos, 0);

		r = (item->trigger_flags & 0x1F) << 3;
		g = (item->trigger_flags >> 2) & 0xF8;
		b = (item->trigger_flags >> 7) & 0xF8;
		TriggerDynamic(pos.x, pos.y, pos.z, 16, r, g, b);
		item->mesh_bits = 2;

		if (item->item_flags[0] < 0)
			item->item_flags[0] = 30;
	}
}
