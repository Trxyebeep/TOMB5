#include "../tomb5/pch.h"
#include "gladiatr.h"
#include "box.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "sphere.h"
#include "debris.h"
#include "sound.h"
#include "control.h"
#include "effects.h"
#include "gameflow.h"
#include "lara.h"

static BITE_INFO gladiator_hit = { 0, 0, 0, 16 };

void InitialiseGladiator(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[GLADIATOR].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;

	if (item->trigger_flags == 1)
		item->meshswap_meshbits = -1;
}

void GladiatorControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* gladiator;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	AI_INFO info;
	PHD_VECTOR pos;
	long distance, dx, dz, front;
	short tilt, angle, head_x, head, torso_y, da;

	if (!CreatureActive(item_number))
		return;

	tilt = 0;
	angle = 0;
	head = 0;
	head_x = 0;
	torso_y = 0;
	item = &items[item_number];
	gladiator = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 6)
		{
			item->anim_number = objects[GLADIATOR].anim_index + 16;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 6;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(gladiator);
		else if (gladiator->hurt_by_lara)
			gladiator->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (gladiator->enemy == lara_item)
		{
			distance = info.distance;
			da = info.angle;
			front = 1;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			da = short(phd_atan(dz, dx) - item->pos.y_rot);
			front = abs(da) < 0x4000;
			distance = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, gladiator->maximum_turn);

		if (info.ahead)
		{
			torso_y = info.angle >> 1;
			head = info.angle >> 1;
			head_x = info.x_angle;
		}

		switch (item->current_anim_state)
		{
		case 1:
			gladiator->flags = 0;
			gladiator->maximum_turn = gladiator->mood == BORED_MOOD ? 0 : 364;
			head = da;

			if (item->ai_bits & GUARD || (!(GetRandomControl() & 0x1F) && (info.distance > 0x100000 || gladiator->mood != ATTACK_MOOD)))
				head = AIGuard(gladiator);
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (gladiator->mood == ESCAPE_MOOD)
				item->goal_anim_state = lara.target != item && info.ahead && !item->hit_status ? 1 : 2;
			else if (gladiator->mood != BORED_MOOD && (!(item->ai_bits & FOLLOW) || !gladiator->reached_goal && distance <= 0x400000))
			{
				if (lara.target == item && front && distance < 0x240000 && GetRandomControl() & 1 && (lara.gun_type == WEAPON_SHOTGUN || !(GetRandomControl() & 0xF)) && item->mesh_bits == -1)
					item->goal_anim_state = 4;
				else if (info.bite && info.distance < 0xA3C29)
					item->goal_anim_state = GetRandomControl() & 1 ? 8 : 9;
				else
					item->goal_anim_state = 2;
			}
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (!(GetRandomControl() & 0x3F))
				item->goal_anim_state = 2;

			break;

		case 2:
			gladiator->flags = 0;
			head = da;
			gladiator->maximum_turn = gladiator->mood == BORED_MOOD ? 364 : 1274;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = 2;
				head = 0;
			}
			else if (gladiator->mood == ESCAPE_MOOD)
				item->goal_anim_state = 3;
			else if (gladiator->mood == BORED_MOOD)
			{
				if (!(GetRandomControl() & 0x3F))
					item->goal_anim_state = 1;
			}
			else if (info.distance < 0x100000)
				item->goal_anim_state = 1;
			else if (info.bite && info.distance < 0x400000)
				item->goal_anim_state = 11;
			else if (!info.ahead || info.distance > 0x240000)
				item->goal_anim_state = 3;

			break;

		case 3:
			gladiator->LOT.is_jumping = 0;

			if (info.ahead)
				head = info.angle;

			gladiator->maximum_turn = 2002;
			tilt = angle / 2;

			if (item->ai_bits & GUARD)
			{
				gladiator->maximum_turn = 0;
				item->goal_anim_state = 1;
			}
			else if (gladiator->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = 1;
			}
			else if (item->ai_bits & FOLLOW)
			{
				if (gladiator->reached_goal || distance > 0x400000)
					item->goal_anim_state = 1;
			}
			else if (gladiator->mood == BORED_MOOD)
				item->goal_anim_state = 2;
			else if (info.distance < 0x240000)
				item->goal_anim_state = info.bite ? 10 : 2;

			break;

		case 4:

			if (item->hit_status)
			{
				if (!front)
					item->goal_anim_state = 1;
			}
			else if (lara.target != item || !(GetRandomControl() & 0x7F))
				item->goal_anim_state = 1;

			break;

		case 5:

			if (lara.target != item)
				item->goal_anim_state = 1;

			break;

		case 8:
		case 9:
		case 10:
		case 11:
			gladiator->maximum_turn = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (item->frame_number > anims[item->anim_number].frame_base + 10)
			{
				r = &room[item->room_number];
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(item, &pos, 16);
				floor = &r->floor[((pos.x - r->x) >> 10) * r->x_size + ((pos.z - r->z) >> 10)];

				if (floor->stopper)
				{
					for (int i = 0; i < r->num_meshes; i++)
					{
						if (!((r->mesh[i].z ^ pos.z) & ~1023) && !((r->mesh[i].x ^ pos.x) & ~1023) && r->mesh[i].static_number >= 50 && r->mesh[i].static_number <= 59)
						{
							ShatterObject(0, &r->mesh[i], -64, lara_item->room_number, 0);
							SoundEffect(ShatterSounds[gfCurrentLevel][r->mesh[i].static_number - 50], (PHD_3DPOS*)&r->mesh[i].x, SFX_DEFAULT);
							r->mesh[i].Flags &= ~1;
							floor->stopper = 0;
							GetHeight(floor, pos.x, pos.y, pos.z);
							TestTriggers(trigger_index, 1, 0);
						}
					}
				}

				if (!gladiator->flags && item->touch_bits & 0x6000)
				{
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &gladiator_hit, 10, item->pos.y_rot, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					gladiator->flags = 1;
				}
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, head_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
