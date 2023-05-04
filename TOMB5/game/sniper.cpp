#include "../tomb5/pch.h"
#include "sniper.h"
#include "box.h"
#include "objects.h"
#include "sphere.h"
#include "effect2.h"
#include "people.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "lara.h"

static BITE_INFO sniper_gun = { 0, 480, 110, 13 };

void InitialiseSniper(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[SNIPER].anim_index;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->pos.y_pos += 512;
	item->pos.x_pos += 256 * phd_sin(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
	item->pos.z_pos += 256 * phd_cos(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
}

void SniperControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* sniper;
	PHD_VECTOR pos;
	AI_INFO info;
	short angle, torso_y, torso_x, head, tilt;

	if (CreatureActive(item_number))
	{
		tilt = 0;
		angle = 0;
		torso_y = 0;
		head = 0;
		torso_x = 0;
		item = &items[item_number];
		sniper = (CREATURE_INFO*) item->data;

		if (item->fired_weapon)
		{
			pos.x = sniper_gun.x;
			pos.y = sniper_gun.y;
			pos.z = sniper_gun.z;
			GetJointAbsPosition(item, &pos, sniper_gun.mesh_num);
			TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 10, 192, 128, 32);
			item->fired_weapon--;
		}

		if (item->hit_points <= 0)
		{
			item->hit_points = 0;

			if (item->current_anim_state != 6)
			{
				item->anim_number = objects[SNIPER].anim_index + 5;
				item->current_anim_state = 6;
				item->frame_number = anims[item->anim_number].frame_base;
			}
		}
		else
		{
			if (item->ai_bits)
				GetAITarget(sniper);
			else if (sniper->hurt_by_lara)
				sniper->enemy = lara_item;

			CreatureAIInfo(item, &info);
			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
			angle = CreatureTurn(item, sniper->maximum_turn);

			if (info.ahead)
			{
				torso_y = info.angle >> 1;
				head = torso_y;
				torso_x = info.x_angle;
			}

			sniper->maximum_turn = 0;

			switch (item->current_anim_state)
			{
				case 1:
					item->mesh_bits = 0;

					if (TargetVisible(item, &info))
						item->goal_anim_state = 2;

					break;

				case 2:
					item->mesh_bits = -1;
					break;

				case 3:
					sniper->flags = 0;

					if (TargetVisible(item, &info) && (!item->hit_status || !(GetRandomControl() & 1)))
					{
						if (!(GetRandomControl() & 0x1F))
							item->goal_anim_state = 4;
					}
					else
						item->goal_anim_state = 5;

					break;

				case 4:

					if (!sniper->flags)
					{
						ShotLara(item, &info, &sniper_gun, torso_y, 100);
						sniper->flags = 1;
						item->fired_weapon = 2;
					}

					break;
			}
		}

		CreatureTilt(item, tilt);
		CreatureJoint(item, 0, torso_y);
		CreatureJoint(item, 1, torso_x);
		CreatureJoint(item, 2, head);
		CreatureAnimation(item_number, angle, tilt);
	}
}
