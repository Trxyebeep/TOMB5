#include "../tomb5/pch.h"
#include "chef.h"
#include "../global/types.h"
#include "box.h"
#include "objects.h"
#include "people.h"
#include "../specific/maths.h"
#include "sound.h"
#include "tomb4fx.h"

BITE_INFO knife = {0, 200, 0, 13};

void InitialiseChef(short item_number)
{
	ITEM_INFO* chef;

	chef = &items[item_number];
	InitialiseCreature(item_number);
	chef->anim_number = objects[CHEF].anim_index;
	chef->frame_number = anims[objects[CHEF].anim_index].frame_base;
	chef->current_anim_state = 1;
	chef->goal_anim_state = 1;
	chef->pos.x_pos += (192 * SIN(chef->pos.y_rot)) >> 14;
	chef->pos.z_pos += (192 * COS(chef->pos.y_rot)) >> 14;
}

void ChefControl(short item_number)
{
	ITEM_INFO* chef;
	creature_info* creature;
	AI_info ai;
	AI_info laraAI;
	int dx, dz;
	short angle;
	short joints[3] = {0, 0, 0};

	if (!CreatureActive(item_number))
		return;

	chef = &items[item_number];
	creature = (creature_info*)chef->data;

	if (chef->ai_bits)
		GetAITarget(creature);
	else if (creature->hurt_by_lara)
		creature->enemy = lara_item;

	CreatureAIInfo(chef, &ai);

	if (creature->enemy == lara_item)
	{
		laraAI.angle = ai.angle;
		laraAI.distance = ai.distance;
	}
	else
	{
		dx = lara_item->pos.x_pos - chef->pos.x_pos;
		dz = lara_item->pos.z_pos - chef->pos.z_pos;
		laraAI.angle = phd_atan(dz, dx) - chef->pos.y_rot;
		laraAI.ahead = 1;

		if (laraAI.angle <= -16384 || laraAI.angle >= 16384)
			laraAI.ahead = 0;

		laraAI.distance = SQUARE(dz) + SQUARE(dx);
	}

	GetCreatureMood(chef, &ai, 1);
	CreatureMood(chef, &ai, 1);

	angle = CreatureTurn(chef, creature->maximum_turn);

	if (ai.ahead)
	{
		joints[0] = ai.angle >> 1;
		joints[2] = ai.angle >> 1;
		joints[1] = ai.x_angle;
	}

	creature->maximum_turn = 0;

	switch (chef->current_anim_state)
	{
	case 1:
		if (ABS(lara_item->pos.y_pos - chef->pos.y_pos) < 1024 && ai.distance < 2359296 && 
			(chef->touch_bits || lara_item->speed > 15 || chef->hit_status || TargetVisible(chef, &laraAI)))
		{
			chef->goal_anim_state = 2;
			creature->alerted = 1;
			chef->ai_bits = 0;
		}

		break;

	case 2:
		creature->maximum_turn = 0;

		if (ai.angle >= 0)
			chef->pos.y_rot -= 364;
		else
			chef->pos.y_rot += 364;

		if (chef->frame_number == anims[chef->anim_number].frame_end)
			chef->pos.y_rot += 32768;

		break;

	case 3:
		creature->maximum_turn = 0;

		if (ABS(ai.angle) >= 364)
		{
			if (ai.angle >= 0)
				chef->pos.y_rot += 364;
			else
				chef->pos.y_rot -= 364;
		}
		else
			chef->pos.y_rot += ai.angle;

		if (!creature->flags && chef->touch_bits & 0x2000 && chef->frame_number > anims[chef->anim_number].frame_base + 10)
		{
			lara_item->hit_points -= 80;
			lara_item->hit_status = 1;
			CreatureEffectT(chef, &knife, 20, chef->pos.y_rot, DoBloodSplat);
			SoundEffect(70, &chef->pos, 0);
			creature->flags = 1;
		}

		break;

	case 4:
		creature->flags = 0;
		creature->maximum_turn = 364;

		if (ai.distance >= 465124)
		{
			if (ai.angle > 20480 || ai.angle < -20480)
				chef->goal_anim_state = 2;
			else if (creature->mood == ATTACK_MOOD)
				chef->goal_anim_state = 5;
		}
		else if (ai.bite)
			chef->goal_anim_state = 3;

		break;

	case 5:
		creature->maximum_turn = 1274;

		if (ai.distance < 465124 || ai.angle > 20480 || ai.angle < -20480 || creature->mood != ATTACK_MOOD)
			chef->goal_anim_state = 4;

		break;
	}

	CreatureTilt(chef, 0);
	CreatureJoint(chef, 0, joints[0]);
	CreatureJoint(chef, 1, joints[1]);
	CreatureJoint(chef, 2, joints[2]);
	CreatureAnimation(item_number, angle, 0);
}

void inject_chef()
{
	INJECT(0x00410990, InitialiseChef);
	INJECT(0x00410A60, ChefControl);
}