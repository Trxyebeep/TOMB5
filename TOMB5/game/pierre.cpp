#include "../tomb5/pch.h"
#include "pierre.h"
#include "box.h"
#include "effect2.h"
#include "sphere.h"
#include "gameflow.h"
#include "objects.h"
#include "control.h"
#include "lot.h"
#include "items.h"
#include "people.h"
#include "../specific/function_stubs.h"
#include "lara.h"

static BITE_INFO larson_gun = { -55, 200, 5, 14 };
static BITE_INFO pierre_gun1 = { 60, 200, 0, 11 };
static BITE_INFO pierre_gun2 = { -57, 200, 0, 14 };

void InitialisePierre(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;

	if (item->trigger_flags)
	{
		item->item_flags[3] = item->trigger_flags;

		if (item->pos.y_rot > 0x1000 && item->pos.y_rot < 0x7000)
			item->pos.x_pos += 384;
		else if (item->pos.y_rot < -0x1000 && item->pos.y_rot > -0x7000)
			item->pos.x_pos -= 384;

		if (item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x2000)
			item->pos.z_pos += 384;
		else if (item->pos.y_rot < -0x5000 || item->pos.y_rot > 0x5000)
			item->pos.z_pos -= 384;
	}
}

void PierreControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* pierre;
	FLOOR_INFO* floor;
	ROOM_INFO* r;
	AI_INFO info;
	PHD_VECTOR pos;
	long x, y, z;
	short angle, tilt, head, torso_x, torso_y, room_number;

	if (!CreatureActive(item_number))
		return;

	torso_y = 0;
	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	item = &items[item_number];
	pierre = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 40 && !(item->flags & IFL_INVISIBLE))
	{
		item->hit_points = 40;
		pierre->flags++;
	}

	if (item->fired_weapon)
	{
		pos.x = larson_gun.x;
		pos.y = larson_gun.y;
		pos.z = larson_gun.z;
		GetJointAbsPosition(item, &pos, larson_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 10, 192, 128, 32);
		item->fired_weapon--;
	}

	if (item->trigger_flags)
	{
		if (gfCurrentLevel == LVL5_TRAJAN_MARKETS)
		{
			item->item_flags[3] = 1;
			item->ai_bits = AMBUSH;
		}
		else
			item->ai_bits = GUARD;

		item->trigger_flags = 0;
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 5)
		{
			if (item->object_number == PIERRE)
				item->anim_number = objects[PIERRE].anim_index + 12;
			else
				item->anim_number = objects[LARSON].anim_index + 15;

			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 5;
		}
		else if (item->object_number == LARSON && item->frame_number == anims[item->anim_number].frame_end)
		{
			room_number = item->item_flags[2] & 0xFF;
			r = &room[room_number];
			x = r->x + ((item->draw_room & 0xFFFFFF00) << 2) + 512;
			y = r->minfloor + (item->item_flags[2] & 0xFFFFFF00);
			z = ((item->draw_room & 0xFF) << 10) + r->z + 512;
			floor = GetFloor(x, y, z, &room_number);
			GetHeight(floor, x, y, z);
			TestTriggers(trigger_index, 1, 0);
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(pierre);
		else
			pierre->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		if (pierre->flags)
		{
			item->hit_points = 60;
			item->ai_bits = AMBUSH;
			pierre->flags = 0;
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (info.distance < 0x400000 && lara_item->speed > 20 || item->hit_status || TargetVisible(item, &info))
		{
			item->ai_bits &= ~GUARD;
			pierre->alerted = 1;
		}

		angle = CreatureTurn(item, pierre->maximum_turn);

		switch (item->current_anim_state)
		{
		case 1:
			torso_y = info.angle >> 1;
			head = info.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (item->ai_bits == AMBUSH)
				item->goal_anim_state = 3;
			else if (Targetable(item, &info))
				item->goal_anim_state = 4;
			else if (item->ai_bits & GUARD || gfCurrentLevel == LVL5_TRAJAN_MARKETS || item->item_flags[3])
			{
				pierre->maximum_turn = 0;
				item->goal_anim_state = 1;

				if (abs(info.angle) < 364)
					item->pos.y_rot += info.angle;
				else if (info.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}
			else if (pierre->mood == BORED_MOOD)
				item->goal_anim_state = GetRandomControl() >= 96 ? 2 : 6;
			else if (pierre->mood == ESCAPE_MOOD)
				item->goal_anim_state = 3;
			else
				item->goal_anim_state = 2;

			break;

		case 2:

			if (info.ahead)
				head = info.angle;

			pierre->maximum_turn = 1274;

			if (pierre->mood == BORED_MOOD && GetRandomControl() < 96)
			{
				item->required_anim_state = 6;
				item->goal_anim_state = 1;
			}
			else if (pierre->mood == ESCAPE_MOOD || item->ai_bits == AMBUSH)
			{
				item->required_anim_state = 3;
				item->goal_anim_state = 1;
			}
			else if (Targetable(item, &info))
			{
				item->required_anim_state = 4;
				item->goal_anim_state = 1;
			}
			else if (!info.ahead || info.distance > 0x900000)
			{
				item->required_anim_state = 3;
				item->goal_anim_state = 1;
			}

			break;

		case 3:

			if (info.ahead)
				head = info.angle;

			pierre->maximum_turn = 2002;
			tilt = angle >> 1;

			if (pierre->reached_goal)
				item->goal_anim_state = 1;
			else if (item->ai_bits == AMBUSH)
				item->goal_anim_state = 3;
			else if (pierre->mood == BORED_MOOD && GetRandomControl() < 96)
			{
				item->required_anim_state = 6;
				item->goal_anim_state = 1;
			}
			else if (Targetable(item, &info))
			{
				item->required_anim_state = 4;
				item->goal_anim_state = 1;
			}
			else if (info.ahead && info.distance < 0x900000)
			{
				item->required_anim_state = 2;
				item->goal_anim_state = 1;
			}

			break;

		case 4:
			torso_y = info.angle >> 1;
			head = info.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			pierre->maximum_turn = 0;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle >= 0)
				item->pos.y_rot += 364;
			else
				item->pos.y_rot -= 364;

			if (Targetable(item, &info))
				item->goal_anim_state = 7;
			else
				item->goal_anim_state = 1;

			break;

		case 6:
			torso_y = info.angle >> 1;
			head = info.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			if (pierre->mood != BORED_MOOD)
				item->goal_anim_state = 1;
			else if (GetRandomControl() < 96)
			{
				item->required_anim_state = 2;
				item->goal_anim_state = 1;
			}

			break;

		case 7:
			torso_y = info.angle >> 1;
			head = info.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			pierre->maximum_turn = 0;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle >= 0)
				item->pos.y_rot += 364;
			else
				item->pos.y_rot -= 364;

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				if (item->object_number == PIERRE)
				{
					ShotLara(item, &info, &pierre_gun1, torso_y, 20);
					ShotLara(item, &info, &pierre_gun2, torso_y, 20);
				}
				else
					ShotLara(item, &info, &larson_gun, torso_y, 20);

				item->fired_weapon = 2;
			}

			if (pierre->mood == ESCAPE_MOOD && GetRandomControl() > 0x2000)
				item->required_anim_state = 1;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);

	if (pierre->reached_goal)
	{
		if (gfCurrentLevel == LVL5_TRAJAN_MARKETS)
		{
			item->goal_anim_state = 1;
			item->required_anim_state = 1;
			item->trigger_flags = 0;
			item->ai_bits = 0;
			pierre->reached_goal = 0;
		}
		else
		{
			item->hit_points = -16384;
			DisableBaddieAI(item_number);
			KillItem(item_number);
		}
	}
}
