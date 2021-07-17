#include "../tomb5/pch.h"
#include "skeleton.h"
#include "box.h"
#include "objects.h"
#include "control.h"
#include "sound.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"

static BITE_INFO skeleton_hit = {0, 0, 0, 17};

void InitialiseSkeleton(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[SKELETON].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
	item->pos.y_pos += 512;
}

void SkeletonControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* skeleton;
	AI_INFO info;
	short angle, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	torso_y = 0;
	head = 0;
	torso_x = 0;
	item = &items[item_number];
	skeleton = (CREATURE_INFO*)item->data;

	if (item->ai_bits)
		GetAITarget(skeleton);
	else if (skeleton->hurt_by_lara)
		skeleton->enemy = lara_item;

	CreatureAIInfo(item, &info);
	angle = CreatureTurn(item, skeleton->maximum_turn);

	if (ABS(info.angle) < 546)
		item->pos.y_rot += info.angle;
	else if (info.angle >= 0)
		item->pos.y_rot += 546;
	else
		item->pos.y_rot -= 546;

	if (info.ahead)
	{
		head = info.angle >> 1;
		torso_x = info.x_angle;
		torso_y = info.angle >> 1;
	}

	skeleton->maximum_turn = 0;

	switch (item->current_anim_state)
	{
	case 1:
		skeleton->flags = 0;

		if (info.distance < 376996)
		{
			if (GetRandomControl() & 1)
				item->goal_anim_state = 2;
			else
				item->goal_anim_state = 3;
		}

		break;

	case 2:
	case 3:

		if (!skeleton->flags && item->touch_bits & 0x9470 && item->frame_number > anims[item->anim_number].frame_base + 18)
		{
			lara_item->hit_points -= 400;
			lara_item->hit_status = 1;
			CreatureEffectT(item, &skeleton_hit, 10, item->pos.y_rot, DoBloodSplat);
			skeleton->flags = 1;
		}

		break;
	}

	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (info.distance >= 2359296)
	{
		item->after_death = 125;
		item->item_flags[0] = 0;
	}
	else
	{
		item->after_death = (short)(phd_sqrt(info.distance) >> 4);

		if (item->item_flags[0] == 0)
		{
			item->item_flags[0] = 1;
			SoundEffect(186, &item->pos, 0);
		}
	}

	CreatureAnimation(item_number, angle, 0);
}

void inject_skelly(bool replace)
{
	INJECT(0x00477A20, InitialiseSkeleton, replace);
	INJECT(0x00477AB0, SkeletonControl, replace);
}
