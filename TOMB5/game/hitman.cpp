#include "../tomb5/pch.h"
#include "hitman.h"
#include "objects.h"
#include "box.h"
#include "control.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "sound.h"
#include "delstuff.h"
#include "traps.h"
#include "../specific/3dmath.h"
#include "people.h"
#include "lara.h"

static BITE_INFO hitman_gun = { 0,300,64, 7 };
static char hitman_nodes[16] = { 15, 14, 13, 6, 5, 12, 7, 4, 10, 11, 19, 0, 0, 0, 0, 0 };

void InitialiseHitman(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[HITMAN].anim_index + 4;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void HitmanControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* hitman;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	AI_INFO info;
	AI_INFO larainfo;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, c, h;
	short angle, head, torso_x, torso_y, room_number, jump_ahead, long_jump_ahead, node, left_foot_room, right_foot_room, frame, base;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	hitman = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	room_number = item->room_number;
	Xoffset = 808 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 808 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

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
		pos.x = hitman_gun.x;
		pos.y = hitman_gun.y;
		pos.z = hitman_gun.z;
		GetJointAbsPosition(item, &pos, hitman_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 10, 192, 128, 32);
		item->fired_weapon--;
	}

	if (item->ai_bits)
		GetAITarget(hitman);
	else
		hitman->enemy = lara_item;

	CreatureAIInfo(item, &info);

	if (item->hit_status && !(GetRandomControl() & 7) && item->item_flags[0] < 11)
	{
		item->meshswap_meshbits |= 1 << hitman_nodes[item->item_flags[0]];
		item->item_flags[0]++;
	}

	node = GetRandomControl() & 0xFF;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
		node &= 0x1F;

	if (node < item->item_flags[0])
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 50;
		GetJointAbsPosition(item, &pos, hitman_nodes[node]);

		TriggerLightningGlow(pos.x, pos.y, pos.z, 0x30202040);
		TriggerFlareSparks(pos.x, pos.y, pos.z, -1, -1, -1);
		TriggerDynamic(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 16, 31, 63, 127);
		SoundEffect(SFX_HITMAN_ELEC_SHORT, &item->pos, SFX_DEFAULT);

		if (node == 5 || node == 7 || node == 10)
		{
			pos2.x = 0;
			pos2.y = 0;
			pos2.z = 50;

			if (node == 5)
				GetJointAbsPosition(item, &pos2, 15);
			else if (node == 7)
			{
				GetJointAbsPosition(item, &pos2, 6);

				if (room[item->room_number].flags & ROOM_UNDERWATER && item->hit_points > 0)
				{
					item->current_anim_state = 43;
					item->anim_number = objects[HITMAN].anim_index + 69;
					item->frame_number = anims[item->anim_number].frame_base;
					item->hit_points = 0;
					DropBaddyPickups(item);
				}
			}
			else
				GetJointAbsPosition(item, &pos2, 12);

			TriggerLightning(&pos, &pos2, (GetRandomControl() & 7) + 8, 0x181F3F7F, 13, 64, 3);
		}
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state == 43 && !lara.burn)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraJointPos(&pos, LMX_FOOT_L);
			left_foot_room = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &left_foot_room);

			pos2.x = 0;
			pos2.y = 0;
			pos2.z = 0;
			GetLaraJointPos(&pos2, LMX_FOOT_R);
			right_foot_room = lara_item->room_number;
			GetFloor(pos2.x, pos2.y, pos2.z, &right_foot_room);

			if ((room[left_foot_room].flags & ROOM_UNDERWATER || room[right_foot_room].flags & ROOM_UNDERWATER) &&
				(room[left_foot_room].FlipNumber == room[item->room_number].FlipNumber || room[right_foot_room].FlipNumber == room[item->room_number].FlipNumber))
			{
				LaraBurn();
				lara.BurnBlue = 1;
				lara.BurnCount = 48;
				lara_item->hit_points = 0;
			}
		}
	}
	else
	{
		if (hitman->enemy == lara_item)
		{
			larainfo.angle = info.angle;
			larainfo.distance = info.distance;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			larainfo.angle = short(phd_atan(dz, dx) - item->pos.y_rot);
			larainfo.distance = SQUARE(dz) + SQUARE(dx);
		}

		GetCreatureMood(item, &info, hitman->enemy != lara_item);

		if (room[item->room_number].flags & ROOM_NO_LENSFLARE)
		{
			if (!(GlobalCounter & 7))
				item->hit_points--;

			hitman->mood = ESCAPE_MOOD;

			if (item->hit_points <= 0)
			{
				item->current_anim_state = 42;
				item->anim_number = objects[HITMAN].anim_index + 68;
				item->frame_number = anims[item->anim_number].frame_base;
			}
		}

		CreatureMood(item, &info, hitman->enemy != lara_item);
		angle = CreatureTurn(item, hitman->maximum_turn);

		if ((larainfo.distance < 0x400000 && lara_item->speed > 20 || item->hit_status || TargetVisible(item, &larainfo)) && !(item->ai_bits & FOLLOW))
		{
			hitman->enemy = lara_item;
			AlertAllGuards(item_number);
		}

		switch (item->current_anim_state)
		{
		case 1:
			head = larainfo.angle;
			hitman->flags = 0;
			hitman->LOT.is_jumping = 0;
			hitman->maximum_turn = 0;

			if (info.ahead && item->ai_bits != GUARD)
			{
				torso_y = info.angle >> 1;
				torso_x = info.x_angle;
			}

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (item->ai_bits & GUARD)
			{
				head = AIGuard(hitman);

				if (item->ai_bits & PATROL1)
				{
					item->trigger_flags--;

					if (item->trigger_flags < 1)
						item->ai_bits = PATROL1;
				}
			}
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x1000000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = 38;
				else if (item->ai_bits != PATROL1)
					item->goal_anim_state = 2;
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (jump_ahead || long_jump_ahead)
			{
				hitman->maximum_turn = 0;
				item->anim_number = objects[HITMAN].anim_index + 22;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 15;

				if (long_jump_ahead)
					item->goal_anim_state = 16;

				hitman->LOT.is_jumping = 1;
			}
			else if (!hitman->monkey_ahead)
			{
				if (hitman->mood == BORED_MOOD)
					item->goal_anim_state = 1;
				else if (info.distance < 0x900000 || item->ai_bits & FOLLOW)
					item->goal_anim_state = 2;
				else
					item->goal_anim_state = 3;
			}
			else
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
					item->goal_anim_state = 4;
				else
					item->goal_anim_state = 2;
			}

			break;

		case 2:
			hitman->LOT.is_jumping = 0;
			hitman->maximum_turn = 910;

			if (Targetable(item, &info) && (info.distance < 0x1000000 || info.zone_number != info.enemy_zone))
			{
				item->goal_anim_state = 1;
				item->required_anim_state = 38;
			}
			else if (jump_ahead || long_jump_ahead)
			{
				hitman->maximum_turn = 0;
				item->anim_number = objects[HITMAN].anim_index + 22;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 15;

				if (long_jump_ahead)
					item->goal_anim_state = 16;

				hitman->LOT.is_jumping = 1;
			}
			else if (hitman->monkey_ahead)
				item->goal_anim_state = 1;
			else if (info.distance < 0x100000)
				item->goal_anim_state = 1;
			else if (info.distance > 0x900000 && !item->ai_bits)
				item->goal_anim_state = 3;

			break;

		case 3:
			hitman->maximum_turn = 1820;
			hitman->LOT.is_jumping = 0;

			if (Targetable(item, &info) && (info.distance < 0x1000000 || info.zone_number != info.enemy_zone))
			{
				item->goal_anim_state = 1;
				item->required_anim_state = 38;
			}
			else if (jump_ahead || long_jump_ahead)
			{
				hitman->maximum_turn = 0;
				item->anim_number = objects[HITMAN].anim_index + 22;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 15;

				if (long_jump_ahead)
					item->goal_anim_state = 16;

				hitman->LOT.is_jumping = 1;
			}
			else if (hitman->monkey_ahead)
				item->goal_anim_state = 1;
			else if (info.distance < 0x900000)
				item->goal_anim_state = 2;

			break;

		case 4:
			hitman->maximum_turn = 0;

			if (item->box_number == hitman->LOT.target_box || !hitman->monkey_ahead)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
					item->goal_anim_state = 1;
			}
			else
				item->goal_anim_state = 5;

			break;

		case 5:
			hitman->LOT.is_jumping = 1;
			hitman->LOT.is_monkeying = 1;
			hitman->maximum_turn = 910;

			if (item->box_number == hitman->LOT.target_box || !hitman->monkey_ahead)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
					item->goal_anim_state = 4;
			}

			break;

		case 38:
			torso_y = larainfo.angle >> 1;
			head = larainfo.angle >> 1;
			hitman->flags = 0;
			hitman->maximum_turn = 0;

			if (info.ahead)
				torso_x = info.x_angle;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;
				
			if (!Targetable(item, &info) || info.distance >= 0x1000000 && info.zone_number == info.enemy_zone)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 39;

			break;

		case 39:
			torso_y = larainfo.angle >> 1;
			head = larainfo.angle >> 1;

			if (info.ahead)
				torso_x = info.x_angle;

			hitman->maximum_turn = 0;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (frame > base + 6 && frame < base + 16 && ((frame - base) & 1))
			{
				item->fired_weapon = 1;
				ShotLara(item, &info, &hitman_gun, larainfo.angle >> 1, 12);
			}

			break;
		}
	}

	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (hitman->reached_goal && hitman->enemy)
	{
		room_number = hitman->enemy->room_number;
		floor = GetFloor(hitman->enemy->pos.x_pos, hitman->enemy->pos.y_pos, hitman->enemy->pos.z_pos, &room_number);
		GetHeight(floor, hitman->enemy->pos.x_pos, hitman->enemy->pos.y_pos, hitman->enemy->pos.z_pos);
		TestTriggers(trigger_index, 1, 0);
		item->required_anim_state = 2;

		if (hitman->enemy->flags & 2)
			item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

		if (hitman->enemy->flags & 8)
		{
			item->required_anim_state = 1;
			item->trigger_flags = 300;
			item->ai_bits = GUARD | PATROL1;
		}

		item->item_flags[3]++;
		hitman->reached_goal = 0;
		hitman->enemy = 0;
	}

	if (item->current_anim_state >= 15 || item->current_anim_state == 5)
		CreatureAnimation(item_number, angle, 0);
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 35;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 25;
			break;

		case -3:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 41;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 24;
			break;

		case -2:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 42;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 23;
			break;

		case 2:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 19;
			break;

		case 3:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 18;
			break;

		case 4:
			hitman->maximum_turn = 0;
			item->anim_number = objects[HITMAN].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 17;
			break;
		}
	}
}
