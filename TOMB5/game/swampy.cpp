#include "../tomb5/pch.h"
#include "swampy.h"
#include "box.h"
#include "objects.h"
#include "newinv2.h"
#include "../specific/3dmath.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO swampy_hitright = { 0, 0, 0, 7 };

void InitialiseSwampy(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[GREEN_TEETH].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 2;
	item->goal_anim_state = 2;
	item->pos.y_pos += 512;
}

void SwampyControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* swampy;
	AI_INFO info;
	short angle, torso_y, torso_x, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	swampy = (CREATURE_INFO*)item->data;
	torso_x = 0;
	torso_y = 0;
	head = 0;

	if (have_i_got_object(PUZZLE_ITEM2))
	{
		item->ai_bits = 0;
		swampy->enemy = lara_item;
	}

	if (item->ai_bits)
		GetAITarget(swampy);
	else if (swampy->hurt_by_lara)
		swampy->enemy = lara_item;

	CreatureAIInfo(item, &info);

	if (swampy->enemy != lara_item)
		phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);

	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, swampy->maximum_turn);

	if (info.ahead)
	{
		torso_y = info.angle >> 1;
		head = info.angle >> 1;
		torso_x = info.x_angle;
	}

	swampy->maximum_turn = 0;

	switch (item->current_anim_state)
	{
	case 1:
		swampy->maximum_turn = 728;

		if (info.distance < 0x100000)
			item->goal_anim_state = 2;

		break;

	case 2:
		swampy->flags = 0;
		swampy->maximum_turn = 364;

		if (info.distance < 0x90000)
			item->goal_anim_state = 3;
		else if (info.distance > 0x100000)
			item->goal_anim_state = 1;
		else
			item->goal_anim_state = 2;

		break;

	case 3:
		swampy->maximum_turn = 364;

		if (!swampy->flags && (item->touch_bits & 0x3C3C0) && item->frame_number > anims[item->anim_number].frame_base + 29)
		{
			lara_item->hit_points -= 100;
			lara_item->hit_status = 1;
			CreatureEffectT(item, &swampy_hitright, 10, item->pos.y_rot, DoBloodSplat);
			swampy->flags = 1;
		}

		break;
	}

	if (swampy->reached_goal)
	{
		if (swampy->enemy)
		{
			if (swampy->enemy->flags & 2)
				item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

			item->item_flags[3]++;
			swampy->reached_goal = 0;
			swampy->enemy = 0;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
	CreatureUnderwater(item, 341);
}
