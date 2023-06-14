#include "../tomb5/pch.h"
#include "mafia2.h"
#include "box.h"
#include "control.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/3dmath.h"
#include "people.h"
#include "objects.h"
#include "lara.h"

static BITE_INFO mafia_gun = { -50, 220, 60, 13 };

void InitialiseMafia(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
	item->meshswap_meshbits = 0x2400;
}

void MafiaControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* mafia;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO lara_info;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight;
	short angle, head, torso_x, torso_y, room_number, jump_ahead, long_jump_ahead, mood;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	mafia = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;
	
	Xoffset = 870 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 870 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

	room_number = item->room_number;
	x = item->pos.x_pos + Xoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z);

	if (item->box_number == lara_item->box_number || y >= nearheight - 384 || y >= midheight + 256 || y <= midheight - 256)
		jump_ahead = 0;
	else
		jump_ahead = 1;

	if (item->box_number == lara_item->box_number || y >= nearheight - 384 || y >= midheight - 384 || y >= farheight + 256 || y <= farheight - 256)
		long_jump_ahead = 0;
	else
		long_jump_ahead = 1;

	if (item->fired_weapon)
	{
		pos.x = mafia_gun.x;
		pos.y = mafia_gun.y;
		pos.z = mafia_gun.z;
		GetJointAbsPosition(item, &pos, mafia_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 10, 192, 128, 32);
		item->fired_weapon--;
	}

	CreatureAIInfo(item, &info);

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 8 && item->current_anim_state != 6)
		{
			if (info.angle >= 0x3000 || info.angle <= -0x3000)
			{
				item->anim_number = objects[item->object_number].anim_index + 16;
				item->current_anim_state = 8;
				item->pos.y_rot += info.angle + 0x8000;
			}
			else
			{
				item->anim_number = objects[item->object_number].anim_index + 11;
				item->current_anim_state = 6;
				item->pos.y_rot += info.angle;
			}

			item->frame_number = anims[item->anim_number].frame_base;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(mafia);
		else
			mafia->enemy = lara_item;

		if (mafia->enemy == lara_item)
		{
			lara_info.angle = info.angle;
			lara_info.distance = info.distance;
		}
		else
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			lara_info.angle = short(phd_atan(z, x) - item->pos.y_rot);
			lara_info.distance = SQUARE(z) + SQUARE(x);
		}

		mood = mafia->enemy != lara_item;

		GetCreatureMood(item, &info, mood);
		CreatureMood(item, &info, mood);
		angle = CreatureTurn(item, mafia->maximum_turn);

		if ((lara_info.distance < 0x400000 && lara_item->speed > 20 || item->hit_status || TargetVisible(item, &lara_info)) && !(item->ai_bits & FOLLOW))
		{
			mafia->enemy = lara_item;
			AlertAllGuards(item_number);
		}

		switch (item->current_anim_state)
		{
		case 1:
			head = lara_info.angle;
			mafia->flags = 0;
			mafia->LOT.is_jumping = 0;
			mafia->maximum_turn = 0;

			if (info.ahead && item->ai_bits != GUARD)
			{
				torso_y = info.angle >> 1;
				torso_x = info.x_angle;
			}

			if (item->ai_bits & GUARD)
				head = AIGuard(mafia);
			else if (item->meshswap_meshbits == 0x2400)
			{
				if (lara_info.angle > 0x5000 || lara_info.angle < -0x5000)
					item->goal_anim_state = 2;
				else
					item->goal_anim_state = 37;
			}
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x1000000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = 4;
				else if (item->ai_bits != MODIFY)
					item->goal_anim_state = 5;
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 5;
			else if (jump_ahead || long_jump_ahead)
			{
				mafia->maximum_turn = 0;
				item->anim_number = objects[item->object_number].anim_index + 41;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 26;

				if (long_jump_ahead)
					item->goal_anim_state = 28;
				else
					item->goal_anim_state = 27;

				mafia->LOT.is_jumping = 1;
			}
			else if (mafia->mood == BORED_MOOD)
				item->goal_anim_state = 1;
			else if (info.distance < 0x900000)
				item->goal_anim_state = 5;
			else
				item->goal_anim_state = 7;

			break;

		case 2:
		case 32:
			mafia->maximum_turn = 0;

			if (info.angle >= 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			if (item->frame_number == anims[item->anim_number].frame_base + 16 && item->meshswap_meshbits == 0x2400)
				item->meshswap_meshbits = 0x80;
			else if (item->frame_number == anims[item->anim_number].frame_end)
				item->pos.y_rot += 0x8000;

			break;

		case 3:
			torso_y = lara_info.angle >> 1;
			head = lara_info.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			mafia->maximum_turn = 0;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			if (!mafia->flags)
			{
				ShotLara(item, &info, &mafia_gun, head, 35);
				mafia->flags = 1;
				item->fired_weapon = 2;
			}

			break;

		case 4:
			torso_y = lara_info.angle >> 1;
			head = lara_info.angle >> 1;
			mafia->flags = 0;
			mafia->maximum_turn = 0;

			if (info.ahead)
				torso_x = info.x_angle;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			if (Targetable(item, &info))
				item->goal_anim_state = 3;
			else if (lara_info.angle > 0x5000 || lara_info.angle < -0x5000)
				item->goal_anim_state = 32;
			else
				item->goal_anim_state = 1;

			break;

		case 5:
			mafia->LOT.is_jumping = 0;
			mafia->maximum_turn = 910;

			if (Targetable(item, &info) && (info.distance < 0x1000000 || info.zone_number != info.enemy_zone))
				item->goal_anim_state = 4;
			else if (jump_ahead || long_jump_ahead)
			{
				mafia->maximum_turn = 0;
				item->anim_number = objects[item->object_number].anim_index + 41;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 26;

				if (long_jump_ahead)
					item->goal_anim_state = 28;
				else
					item->goal_anim_state = 27;

				mafia->LOT.is_jumping = 1;
			}
			else if (info.distance < 0x100000)
				item->goal_anim_state = 1;
			else if (info.distance > 0x900000)
				item->goal_anim_state = 7;

			break;

		case 7:
			mafia->LOT.is_jumping = 0;
			mafia->maximum_turn = 1820;

			if (Targetable(item, &info) && (info.distance < 0x1000000 || info.zone_number != info.enemy_zone))
				item->goal_anim_state = 4;
			else if (jump_ahead || long_jump_ahead)
			{
				mafia->maximum_turn = 0;
				item->anim_number = objects[item->object_number].anim_index + 50;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 26;

				if (long_jump_ahead)
					item->goal_anim_state = 28;
				else
					item->goal_anim_state = 27;

				mafia->LOT.is_jumping = 1;
			}
			else if (info.distance < 0x900000)
				item->goal_anim_state = 5;

			break;

		case 37:
			mafia->maximum_turn = 0;

			if (info.angle >= 0)
				item->pos.y_rot += 364;
			else
				item->pos.y_rot -= 364;

			if (item->frame_number == anims[item->anim_number].frame_base + 16 && item->meshswap_meshbits == 0x2400)
				item->meshswap_meshbits = 0x80;

			break;
		}
	}

	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= 20 || item->current_anim_state == 6 || item->current_anim_state == 8)
		CreatureAnimation(item_number, angle, 0);
	else
	{
		switch (CreatureVault(item_number, angle, 2, 256))
		{
		case -4:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 38;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 23;
			break;

		case -3:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 39;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 24;
			break;

		case -2:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 40;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 25;
			break;

		case 2:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 35;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 20;
			break;

		case 3:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 36;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 21;
			break;

		case 4:
			mafia->maximum_turn = 0;
			item->anim_number = objects[item->object_number].anim_index + 37;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 22;
			break;
		}
	}
}
