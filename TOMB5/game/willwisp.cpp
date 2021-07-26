#include "../tomb5/pch.h"
#include "willwisp.h"
#include "box.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "sound.h"
#include "effect2.h"
#include "control.h"
#include "sphere.h"
#include "tomb4fx.h"
#include "lot.h"
#include "items.h"
#include "../specific/function_stubs.h"

void InitialiseWillOWisp(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[WILLOWISP].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
}

void WillOWispControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* willowisp;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	AI_INFO info, lara_info;
	long lara_dx, lara_dz, height, ceiling;
	short angle, tilt, room_number, lightlevel, x;
	//short torso_y, torso_x, head;

	if (CreatureActive(item_number))
	{
		tilt = 0;
		item = &items[item_number];
		willowisp = (CREATURE_INFO*) item->data;
		item->ai_bits |= 0x10;

		if (willowisp->reached_goal)
		{
			item->item_flags[3]++;
			willowisp->reached_goal = 0;
			willowisp->enemy = NULL;
		}

		if (item->ai_bits)
			GetAITarget(willowisp);
		else if (willowisp->hurt_by_lara)
			willowisp->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (willowisp->enemy == lara_item)
			lara_info.distance = info.distance;
		else
		{
			lara_dx = lara_item->pos.x_pos - item->pos.x_pos;
			lara_dz = lara_item->pos.z_pos - item->pos.z_pos;
			phd_atan(lara_dz, lara_dx);
			lara_info.distance = SQUARE(lara_dx) + SQUARE(lara_dz);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, willowisp->maximum_turn);
		willowisp->maximum_turn = 1274;

		if (lara_info.distance <= 1863225 && ABS((int) (item->pos.y_pos - lara_item->pos.y_pos <= 1024)) && !willowisp->reached_goal)
			item->goal_anim_state = 2;
		else
			item->goal_anim_state = 1;

		if (item->current_anim_state == 1)
			willowisp->reached_goal = 0;

		SoundEffect(239, &item->pos, 0);
		lightlevel = (GetRandomControl() & 0x1F) - (item->item_flags[0] >> 2) + 48;

		for (int i = 0; i < 3; i++)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 200;
			GetJointAbsPosition(item, &pos, i + 1);
			TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(48, 48, 64, lightlevel));
		}

		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 15, lightlevel + 12, lightlevel + 12, lightlevel + 16);
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (item->pos.y_pos > height - 512)
			item->pos.y_pos = height - 512;
		else if (item->pos.y_pos < ceiling + 768)
			item->pos.y_pos = ceiling + 768;

		CreatureAnimation(item_number, angle, tilt);

		if (willowisp->enemy == lara_item)
		{
			x = item->item_flags[0];
			item->item_flags[0]++;

			if (x > 191)
			{
				item->hit_points = -16384;
				DisableBaddieAI(item_number);
				KillItem(item_number);
			}
		}
	}
}

void inject_wisp(bool replace)
{
	INJECT(0x0048E500, InitialiseWillOWisp, replace);
	INJECT(0x0048E580, WillOWispControl, replace);
}
