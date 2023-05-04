#include "../tomb5/pch.h"
#include "sas.h"
#include "box.h"
#include "objects.h"
#include "control.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "people.h"
#include "draw.h"
#include "gameflow.h"
#include "lara.h"

static BITE_INFO sas_gun = { 15, 80, 200, 13 };

void SasControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy_bak;
	ITEM_INFO* item2;
	FLOOR_INFO* floor;
	CREATURE_INFO* sas;
	PHD_VECTOR pos;
	GAME_VECTOR start;
	GAME_VECTOR target;
	AI_INFO sasAI;
	AI_INFO lara_info;
	short* bounds;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz;
	short index, room_number, jump_ahead, long_jump_ahead, angle, head, torso_x, torso_y, los, frame, base;

	if (!CreatureActive(item_number))
		return;

	if (objects[SWAT].loaded)
		index = objects[SWAT].anim_index;
	else
		index = objects[BLUE_GUARD].anim_index;

	item = &items[item_number];
	sas = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;
	room_number = item->room_number;
	Xoffset = 870 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 870 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
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
		pos.x = sas_gun.x;
		pos.y = sas_gun.y;
		pos.z = sas_gun.z;
		GetJointAbsPosition(item, &pos, sas_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 10, 192, 128, 32);
		item->fired_weapon--;
	}

	if (item->ai_bits)
		GetAITarget(sas);
	else
		sas->enemy = lara_item;

	CreatureAIInfo(item, &sasAI);

	if (sas->enemy == lara_item)
	{
		lara_info.angle = sasAI.angle;
		lara_info.distance = sasAI.distance;
	}
	else
	{
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;
		lara_info.angle = (short)(phd_atan(dz, dx) - item->pos.y_rot);
		lara_info.distance = SQUARE(dz) + SQUARE(dx);
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 8 && item->current_anim_state != 6)
		{
			if (lara_info.angle >= 12288 || lara_info.angle <= -12288)
			{
				item->current_anim_state = 8;
				item->anim_number = index + 16;
				item->pos.y_rot += lara_info.angle + 32768;
			}
			else
			{
				item->current_anim_state = 6;
				item->anim_number = index + 11;
				item->pos.y_rot += lara_info.angle;
			}

			item->frame_number = anims[item->anim_number].frame_base;
		}
	}
	else
	{
		GetCreatureMood(item, &sasAI, sas->enemy != lara_item);

		if (item->object_number == SCIENTIST)
		{
			if (item->hit_points < objects[SCIENTIST].hit_points)
				sas->mood = ESCAPE_MOOD;
			else if (sas->enemy == lara_item)
				sas->mood = BORED_MOOD;
		}

		if (room[item->room_number].flags & ROOM_NO_LENSFLARE)
		{
			if (item->object_number == SWAT_PLUS)
			{
				item->item_flags[0]++;

				if (item->item_flags[0] > 60 && !(GetRandomControl() & 0xF))
				{
					SoundEffect(SFX_BIO_BREATHE_OUT, &item->pos, SFX_DEFAULT);
					item->item_flags[0] = 0;
				}
			}
			else
			{
				if (!(GlobalCounter & 7))
					item->hit_points--;

				sas->mood = ESCAPE_MOOD;

				if (item->hit_points <= 0)
				{
					item->current_anim_state = 8;
					item->anim_number = index + 16;
					item->frame_number = anims[item->anim_number].frame_base;
				}
			}
		}

		CreatureMood(item, &sasAI, sas->enemy != lara_item);
		angle = CreatureTurn(item, sas->maximum_turn);
		enemy_bak = sas->enemy;
		sas->enemy = lara_item;

		if ((lara_info.distance < 0x400000 && lara_item->speed > 20) || item->hit_status || TargetVisible(item, &lara_info))
		{
			if (!(item->ai_bits & FOLLOW) && item->object_number != SCIENTIST)
			{
				if (abs(item->pos.y_pos - lara_item->pos.y_pos) < 1280)
				{
					sas->enemy = lara_item;
					AlertAllGuards(item_number);
				}
			}
		}

		sas->enemy = enemy_bak;
		bounds = GetBestFrame(lara_item);
		start.x = item->pos.x_pos;
		start.y = item->pos.y_pos - 384;
		start.z = item->pos.z_pos;
		start.room_number = item->room_number;
		target.x = lara_item->pos.x_pos;
		target.y = lara_item->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);;
		target.z = lara_item->pos.z_pos;
		los = !LOS(&start, &target) && item->trigger_flags != 10;
		sas->maximum_turn = 0;

		switch (item->current_anim_state)
		{
		case 1:
			head = lara_info.angle;
			sas->LOT.is_jumping = 0;
			sas->flags = 0;

			if (sasAI.ahead && !(item->ai_bits & GUARD))
			{
				torso_y = sasAI.angle >> 1;
				torso_x = sasAI.x_angle;
			}

			if (item->object_number == SCIENTIST && item == lara.target)
				item->goal_anim_state = 39;
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (item->ai_bits & GUARD)
			{
				if (item->ai_bits & MODIFY)
					head = 0;
				else
					head = AIGuard(sas);

				if (item->ai_bits & PATROL1)
				{
					item->trigger_flags--;

					if (item->trigger_flags < 1)
						item->ai_bits &= ~GUARD;
				}
			}
			else if (sas->enemy == lara_item && (lara_info.angle > 20480 || lara_info.angle < -20480) && item->object_number != SCIENTIST)
				item->goal_anim_state = 2;
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 5;
			else if (item->ai_bits & AMBUSH)
				item->goal_anim_state = 7;
			else if (!Targetable(item, &sasAI) || item->object_number == SCIENTIST)
			{
				if (jump_ahead || long_jump_ahead)
				{
					sas->maximum_turn = 0;
					item->anim_number = index + 41;
					item->frame_number = anims[item->anim_number].frame_base;
					item->current_anim_state = 26;

					if (!long_jump_ahead)
						item->goal_anim_state = 27;
					else
						item->goal_anim_state = 28;

					sas->LOT.is_jumping = 1;
				}
				else if (los)
					item->goal_anim_state = 31;
				else if (sas->mood == BORED_MOOD)
					item->goal_anim_state = 1;
				else if (sasAI.distance < 0x900000 || item->ai_bits & FOLLOW)
					item->goal_anim_state = 5;
				else
					item->goal_anim_state = 7;
			}
			else if (sasAI.distance < 0x1000000 || sasAI.zone_number != sasAI.enemy_zone)
				item->goal_anim_state = 4;
			else if (item->ai_bits != MODIFY)
				item->goal_anim_state = 5;

			if (item->trigger_flags == 11)
				item->trigger_flags = 0;

			break;

		case 2:
			sas->flags = 0;

			if (sasAI.angle >= 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			if (item->frame_number == anims[item->anim_number].frame_end)
				item->pos.y_rot += 32768;

			break;

		case 3:
		case 35:
			torso_y = lara_info.angle >> 1;
			head = lara_info.angle >> 1;

			if (abs(sasAI.angle) >= 364)
			{
				if (sasAI.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}
			else
				item->pos.y_rot += sasAI.angle;

			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (item->current_anim_state == 35 && sas->flags && frame < base + 10 && (frame - base) & 1)
				sas->flags = 0;

			if (!sas->flags)
			{
				sas->flags = 1;
				item->fired_weapon = 2;

				if (item->current_anim_state == 3)
					ShotLara(item, &sasAI, &sas_gun, lara_info.angle >> 1, 30 * ((gfCurrentLevel > LVL5_OLD_MILL) + 1));
				else
					ShotLara(item, &sasAI, &sas_gun, lara_info.angle >> 1, 10);
			}

			break;

		case 4:
			sas->flags = 0;
			torso_y = lara_info.angle >> 1;
			head = lara_info.angle >> 1;

			if (sasAI.ahead)
				torso_x = sasAI.x_angle;

			if (abs(sasAI.angle) >= 364)
			{
				if (sasAI.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}
			else
				item->pos.y_rot += sasAI.angle;

			if (!Targetable(item, &sasAI))
				item->goal_anim_state = 1;
			else if (item->object_number == BLUE_GUARD || item->object_number == CRANE_GUY)
				item->goal_anim_state = 3;
			else
				item->goal_anim_state = 35;

			break;

		case 5:
			sas->LOT.is_jumping = 0;
			sas->maximum_turn = 910;

			if (!Targetable(item, &sasAI) || sasAI.distance >= 0x1000000 && sasAI.zone_number == sasAI.enemy_zone ||
				item->object_number == SCIENTIST || item->ai_bits & (AMBUSH | PATROL1))
			{
				if (jump_ahead || long_jump_ahead)
				{
					sas->maximum_turn = 0;
					item->anim_number = index + 41;
					item->frame_number = anims[item->anim_number].frame_base;
					item->current_anim_state = 26;

					if (!long_jump_ahead)
						item->goal_anim_state = 27;
					else
						item->goal_anim_state = 26;

					sas->LOT.is_jumping = 1;
				}
				else if (sasAI.distance < 0x100000)
					item->goal_anim_state = 1;
				else if (los && !item->ai_bits)
					item->goal_anim_state = 1;
				else if (sasAI.distance > 0x900000 && !(item->ai_bits & PATROL1))
					item->goal_anim_state = 7;
			}
			else
				item->goal_anim_state = 4;

			break;

		case 7:
			sas->LOT.is_jumping = 0;
			sas->maximum_turn = 1820;

			if (!Targetable(item, &sasAI) || sasAI.distance >= 0x1000000 && sasAI.zone_number == sasAI.enemy_zone || item->object_number == SCIENTIST)
			{
				if (jump_ahead || long_jump_ahead)
				{
					sas->maximum_turn = 0;
					item->anim_number = index + 50;
					item->frame_number = anims[item->anim_number].frame_base;
					item->current_anim_state = 26;

					if (!long_jump_ahead)
						item->goal_anim_state = 27;
					else
						item->goal_anim_state = 28;

					sas->LOT.is_jumping = 1;
				}
				else if (los)
					item->goal_anim_state = 1;
				else if (sasAI.distance < 0x900000)
					item->goal_anim_state = 5;
			}
			else
				item->goal_anim_state = 4;

			if (item->trigger_flags == 11)
			{
				sas->LOT.is_jumping = 1;
				sas->maximum_turn = 0;
			}

			break;

		case 14:
			head = lara_info.angle;

			if (item->pos.y_pos > item->floor - 2048 && item->trigger_flags == 5)
			{
				item->trigger_flags = 0;
				room_number = item->room_number;
				GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
					item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				SoundEffect(SFX_LARA_ROPEDOWN_LOOP, &item->pos, 0);
			}
			else if (item->pos.y_pos >= item->floor - 512)
				item->goal_anim_state = 4;

			if (abs(sasAI.angle) >= 364)
			{
				if (sasAI.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}
			else
				item->pos.y_rot += sasAI.angle;

			break;

		case 15:
			head = AIGuard(sas);

			if (sas->alerted)
				item->goal_anim_state = 16;

			break;

		case 16:
		case 18:
			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (frame == base)
			{
				room_number = item->room_number;
				GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
					item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
			}
			else if (frame == base + 44)
			{
				item->meshswap_meshbits = 0;

				for (int i = room[item->room_number].item_number; i != NO_ITEM; i = item2->next_item)
				{
					item2 = &items[i];

					if (item2->object_number >= ANIMATING1 && item2->object_number < ANIMATING15 && item2->room_number == item->room_number)
					{
						if (item2->trigger_flags > 2 && item2->trigger_flags < 5)
						{
							item2->mesh_bits = -3;
							break;
						}
					}
				}
			}
			else if (frame == anims[item->anim_number].frame_end)
				item->pos.y_rot -= 16384;

			break;

		case 17:
			head = 0;

			if (!item->hit_status && lara_item->speed < 40 && !lara.has_fired)
				sas->alerted = 0;

			if (sas->alerted)
				item->goal_anim_state = 18;

			break;

		case 19:
			head = AIGuard(sas);

			if (sas->alerted)
				item->goal_anim_state = 1;

			break;

		case 31:

			if (item->trigger_flags != 8 || !los || item->hit_status)
				item->goal_anim_state = 30;

			//same thing as 39 and 36

		case 30:
			sas->LOT.is_jumping = 0;
			sas->maximum_turn = 910;

			if (jump_ahead || long_jump_ahead || sasAI.distance < 0x100000 || !los || item->hit_status)
				item->goal_anim_state = 1;

			break;

		case 37:
			item2 = 0;

			for (int i = room[item->room_number].item_number; i != -1; i = item2->next_item)
			{
				item2 = &items[i];

				if (item2->object_number == PUZZLE_HOLE8)
					break;
			}

			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (frame == base)
			{
				item2->mesh_bits = 0x1FFF;
				item->pos.y_rot = item2->pos.y_rot;
				item->pos.x_pos = item2->pos.x_pos - 256;
				item->pos.z_pos = item2->pos.z_pos + 128;
				item->meshswap_meshbits = 1024;
			}
			else if (frame == base + 32)
				item2->mesh_bits = 0x3FFD;
			else if (frame == base + 74)
				item2->mesh_bits = 0x43FBD;
			else if (frame == base + 120)
				item2->mesh_bits = 0xC3F3D;
			else if (frame == base + 157)
				item2->mesh_bits = 0xC7F39;
			else if (frame == base + 190)
				item2->mesh_bits = 0x10C6F39;
			else if (frame == anims[item->anim_number].frame_end)
			{
				item2->mesh_bits = 0x1FFF;
				room_number = item->room_number;
				GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
					item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				item->required_anim_state = 5;
				item->meshswap_meshbits = 0;
			}

			break;

		case 38:

			if ((item->object_number != SCIENTIST || item != lara.target) &&
				((GetRandomControl() & 0x7F || item->trigger_flags >= 10) || item->trigger_flags == 9))
			{
				if (item->ai_bits & GUARD)
				{
					head = AIGuard(sas);

					if (item->ai_bits & PATROL1)
					{
						item->trigger_flags--;

						if (item->trigger_flags < 1)
							item->ai_bits = PATROL1;
					}
				}
			}
			else
				item->goal_anim_state = 1;

			break;

		case 39:

			if (item != lara.target && !(GetRandomControl() & 0x3F))
			{
				if (item->trigger_flags == 7 || item->trigger_flags == 9)
					item->required_anim_state = 38;

				item->goal_anim_state = 1;
			}

			//LABEL_253: TestTriggers code

		case 36:	
			/*
			moving 36 down here and getting rid of the break at 39 because I don't wanna use a goto. otherwise 36 is literally just
			a goto LABEL_253 (see above)
			*/

			if (item->frame_number == anims[item->anim_number].frame_base + 39)
			{
				room_number = item->room_number;
				GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
					item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
			}

			break;
		}
	}

	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (sas->reached_goal && sas->enemy)
	{
		if (sas->enemy->flags == 4)
		{
			item->goal_anim_state = 1;
			item->required_anim_state = 37;
		}
		else if (sas->enemy->flags & 16)
		{
			item->goal_anim_state = 1;
			item->required_anim_state = 38;
			item->trigger_flags = 300;
			item->ai_bits = GUARD | PATROL1;
		}
		else if (sas->enemy->flags & 32)
		{
			item->goal_anim_state = 1;
			item->required_anim_state = 36;
			item->ai_bits = PATROL1;
		}
		else
		{
			room_number = sas->enemy->room_number;
			floor = GetFloor(sas->enemy->pos.x_pos, sas->enemy->pos.y_pos, sas->enemy->pos.z_pos, &room_number);
			GetHeight(floor, sas->enemy->pos.x_pos, sas->enemy->pos.y_pos, sas->enemy->pos.z_pos);
			TestTriggers(trigger_index, 1, 0);
			item->required_anim_state = 5;

			if (sas->enemy->flags & 2)
				item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

			if (sas->enemy->flags & 8)
			{
				item->required_anim_state = 1;
				item->trigger_flags = 300;
				item->ai_bits |= GUARD | PATROL1;
			}
		}

		item->item_flags[3]++;
		sas->reached_goal = 0;
		sas->enemy = 0;
	}

	if ((item->current_anim_state >= 20 || item->current_anim_state == 6 || item->current_anim_state == 8) && item->current_anim_state != 30)
		CreatureAnimation(item_number, angle, 0);
	else
	{
		switch (CreatureVault(item_number, angle, 2, 256))
		{
		case -4:
			sas->maximum_turn = 0;
			item->anim_number = index + 38;
			item->current_anim_state = 23;
			item->frame_number = anims[item->anim_number].frame_base;
			break;

		case -3:
			sas->maximum_turn = 0;
			item->anim_number = index + 39;
			item->current_anim_state = 24;
			item->frame_number = anims[item->anim_number].frame_base;
			break;

		case -2:
			sas->maximum_turn = 0;
			item->anim_number = index + 40;
			item->current_anim_state = 25;
			item->frame_number = anims[item->anim_number].frame_base;
			break;

		case 2:
			sas->maximum_turn = 0;
			item->anim_number = index + 35;
			item->current_anim_state = 20;
			item->frame_number = anims[item->anim_number].frame_base;
			break;

		case 3:
			sas->maximum_turn = 0;
			item->anim_number = index + 36;
			item->current_anim_state = 21;
			item->frame_number = anims[item->anim_number].frame_base;
			break;

		case 4:
			sas->maximum_turn = 0;
			item->anim_number = index + 37;
			item->current_anim_state = 22;
			item->frame_number = anims[item->anim_number].frame_base;
			break;
		}
	}
}
