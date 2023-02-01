#include "../tomb5/pch.h"
#include "lasers.h"
#include "control.h"
#include "sound.h"
#include "traps.h"
#include "switch.h"
#include "../specific/function_stubs.h"
#include "items.h"
#include "effect2.h"
#include "../specific/3dmath.h"
#include "objects.h"

static char SteamLasers[8][5] =
{
	{ 1, 0, 1, 0, 0 },
	{ 0, 0, 1, 0, 1 },
	{ 0, 1, 0, 0, 1 },
	{ 0, 1, 1, 1, 0 },
	{ 1, 0, 1, 1, 1 },
	{ 1, 1, 0, 0, 1 },
	{ 0, 1, 0, 1, 0 },
	{ 1, 0, 0, 1, 0 }
};

void DrawFloorLasers(ITEM_INFO* item)
{
	
}

void ControlLasers(short item_number)
{
	ITEM_INFO* item;
	LASER_STRUCT* laser;
	long bbox[6];

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	SoundEffect(SFX_RICH_DOOR_BEAM, &item->pos, SFX_DEFAULT);

	if (item->item_flags[3])
		item->item_flags[3] -= 2;

	if (item->room_number == lara_item->room_number)
	{
		laser = (LASER_STRUCT*)item->data;
		bbox[0] = item->pos.x_pos + laser->v1[0].x;
		bbox[1] = item->pos.x_pos + laser->v4[0].x;
		bbox[2] = item->pos.y_pos - item->item_flags[0];
		bbox[3] = item->pos.y_pos;
		bbox[4] = item->pos.z_pos + laser->v1[0].z;
		bbox[5] = item->pos.z_pos + laser->v4[0].z;

		if (CheckLaserBox(bbox))
		{
			if (!lara.burn && item->trigger_flags & 2)
			{
				LaraBurn();
				lara.BurnCount = 24;

				if (lara_item->hit_points > 0)
					lara_item->hit_points = 0;
			}

			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			item->item_flags[3] = (GetRandomControl() & 0x1E) + 32;
		}
	}
}

void ControlSteamLasers(short item_number)
{
	ITEM_INFO* item;
	STEAMLASER_STRUCT* laser;
	long bbox[6];

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	SoundEffect(SFX_RICH_DOOR_BEAM, &item->pos, SFX_DEFAULT);

	if (item->item_flags[3])
		item->item_flags[3]--;

	item->item_flags[0] = (phd_sin((GlobalCounter + (item->trigger_flags << 5)) << 8) >> 6) - ((item->trigger_flags & 1) << 8);

	if (item->room_number == lara_item->room_number)
	{
		laser = (STEAMLASER_STRUCT*)item->data;
		bbox[0] = item->pos.x_pos + laser->v1[0].x;
		bbox[1] = item->pos.x_pos + laser->v4[0].x;
		bbox[2] = item->pos.y_pos - 2048;
		bbox[3] = item->pos.y_pos;
		bbox[4] = item->pos.z_pos + laser->v1[0].z;
		bbox[5] = item->pos.z_pos + laser->v4[0].z;

		if (SteamLasers[(GlobalCounter >> 5) & 7][item->trigger_flags])
		{
			if (CheckLaserBox(bbox) && !lara.burn)
			{
				LaraBurn();
				lara.BurnCount = 24;

				if (lara_item->hit_points > 0)
					lara_item->hit_points = 0;

				item->item_flags[3] = 64;
			}
		}
	}
}

void ControlFloorLasers(short item_number)
{
	ITEM_INFO* item;
	FLOORLASER_STRUCT* laser;
	GAME_VECTOR sparkPos;
	long bbox[6];

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	SoundEffect(SFX_RICH_DOOR_BEAM, &item->pos, SFX_DEFAULT);
	laser = (FLOORLASER_STRUCT*)item->data;

	if (item->trigger_flags)
	{
		item->trigger_flags--;

		if (!item->trigger_flags)
			KillItem(item_number);

		if (item->trigger_flags > 8)
		{
			sparkPos.y = item->pos.y_pos + laser->v1.y;

			if (GlobalCounter & 1)
			{
				sparkPos.z = laser->v1.z + item->pos.z_pos;

				if (GetRandomControl() & 1)
					sparkPos.x = item->pos.x_pos + laser->v1.x;
				else
					sparkPos.x = item->pos.x_pos + laser->v4.x;

				sparkPos.z += GetRandomControl() % (laser->v4.z - laser->v1.z);
			}
			else
			{
				sparkPos.x = item->pos.x_pos + laser->v1.x;

				if (GetRandomControl() & 1)
					sparkPos.z = item->pos.z_pos + laser->v1.z;
				else
					sparkPos.z = item->pos.z_pos + laser->v4.z;

				sparkPos.x += GetRandomControl() % (laser->v4.x - laser->v1.x);
			}

			TriggerRicochetSpark(&sparkPos, GetRandomControl() << 1, 2, 0);
		}
	}
	else
	{
		if (item->item_flags[3])
			item->item_flags[3]--;

		if (item->item_flags[2] <= 0)
		{
			if (item->item_flags[2] >= 0)
			{
				if (!(GetRandomControl() & 0xFF))
					item->item_flags[2] = (item->item_flags[0] << 10) + 2560;
			}
			else
				item->item_flags[2]++;
		}
		else
		{
			item->item_flags[2] -= 512;

			if (!item->item_flags[2])
				item->item_flags[2] = (GetRandomControl() & 0x7F) + 1024;
		}

		if (item->room_number == lara_item->room_number)
		{
			bbox[0] = item->pos.x_pos + laser->v1.x;
			bbox[1] = item->pos.x_pos + laser->v4.x;
			bbox[2] = item->pos.y_pos;
			bbox[3] = item->pos.y_pos;
			bbox[4] = item->pos.z_pos + laser->v1.z;
			bbox[5] = item->pos.z_pos + laser->v4.z;

			if (CheckLaserBox(bbox) && !lara.burn)
			{
				LaraBurn();
				lara.BurnCount = 24;

				if (lara_item->hit_points > 0)
					lara_item->hit_points = 0;

				item->item_flags[3] = (GetRandomControl() & 0xF) + 48;
			}
		}

		GetFishTank(item);
	}
}

long IsSteamOn(ITEM_INFO* item)
{
	ITEM_INFO* steam;
	short item_number;

	item_number = room[item->room_number].item_number;

	while (item_number != NO_ITEM)
	{
		steam = &items[item_number];

		if (steam->object_number == STEAM_EMITTER && steam->trigger_flags > 0)
		{
			if (steam->status == ITEM_ACTIVE)
				return 1;

			break;
		}

		item_number = steam->next_item;
	}

	return 0;
}

void inject_lasers(bool replace)
{
	INJECT(0x0045A540, DrawFloorLasers, replace);
	INJECT(0x00459D30, ControlLasers, replace);
	INJECT(0x0045A030, ControlSteamLasers, replace);
	INJECT(0x0045A1E0, ControlFloorLasers, replace);
	INJECT(0x00459C10, IsSteamOn, replace);
}
