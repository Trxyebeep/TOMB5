#include "../tomb5/pch.h"
#include "dog.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "lara.h"

static BITE_INFO dog_bite = { 0, 30, 141, 20 };

void InitialiseDog(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags)
	{
		item->current_anim_state = 5;
		item->anim_number = objects[item->object_number].anim_index + 6;
		item->status -= ITEM_INVISIBLE;
	}
	else
	{
		item->current_anim_state = 6;
		item->anim_number = objects[item->object_number].anim_index + 10;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void DogControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* dog;
	AI_INFO info;
	short angle, head, head_x, torso_y, tilt, random;

	if (!CreatureActive(item_number))
		return;

	angle = 0;
	tilt = 0;
	head_x = 0;
	head = 0;
	torso_y = 0;
	item = &items[item_number];
	dog = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 10)
		{
			item->anim_number = objects[DOG].anim_index + 13;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 10;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, 0);
		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, dog->maximum_turn);

		switch (item->current_anim_state)
		{
		case 1:
			dog->maximum_turn = 546;

			if (dog->mood == BORED_MOOD)
			{
				random = (short)GetRandomControl();

				if (random < 768)
				{
					item->goal_anim_state = 3;
					item->required_anim_state = 4;
				}
				else if (random < 1536)
				{
					item->goal_anim_state = 3;
					item->required_anim_state = 5;
				}
				else if (random < 2816)
					item->goal_anim_state = 3;
			}
			else
				item->goal_anim_state = 2;

			break;

		case 2:
			tilt = angle;
			dog->maximum_turn = 1092;

			if (dog->mood == BORED_MOOD)
				item->goal_anim_state = 3;
			else if (info.distance < 0x90000)
				item->goal_anim_state = 8;

			break;

		case 3:
			dog->maximum_turn = 0;
			dog->flags = 0;

			if (dog->mood == BORED_MOOD)
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else
				{
					random = (short)GetRandomControl();

					if (random < 768)
						item->goal_anim_state = 4;
					else if (random < 1536)
						item->goal_anim_state = 5;
					else if (random < 9728)
						item->goal_anim_state = 1;
				}
			}
			else if (dog->mood == ESCAPE_MOOD || info.distance >= 0x1C639 || !info.ahead)
				item->goal_anim_state = 2;
			else
				item->goal_anim_state = 7;

			break;

		case 4:

			if (dog->mood != BORED_MOOD || GetRandomControl() < 1280)
				item->goal_anim_state = 3;

			break;

		case 5:

			if (dog->mood != BORED_MOOD || GetRandomControl() < 256)
				item->goal_anim_state = 3;

			break;

		case 6:

			if (dog->mood != BORED_MOOD || GetRandomControl() < 512)
				item->goal_anim_state = 3;

			break;

		case 7:
			dog->maximum_turn = 91;

			if (dog->flags != 1 && info.ahead && item->touch_bits & 0x122000)
			{
				CreatureEffect(item, &dog_bite, DoBloodSplat);
				lara_item->hit_points -= 30;
				lara_item->hit_status = 1;
				dog->flags = 1;
			}

			if (info.distance > 0x1C639 && info.distance < 0x718E4)
				item->goal_anim_state = 9;
			else
				item->goal_anim_state = 3;

			break;

		case 8:

			if (dog->flags != 2 && item->touch_bits & 0x122000)
			{
				CreatureEffect(item, &dog_bite, DoBloodSplat);
				lara_item->hit_points -= 80;
				lara_item->hit_status = 1;
				dog->flags = 2;
			}

			if (info.distance < 0x1C639)
				item->goal_anim_state = 7;
			else if (info.distance < 0x718E4)
				item->goal_anim_state = 9;

			break;

		case 9:
			dog->maximum_turn = 1092;

			if (dog->flags != 3 && item->touch_bits & 0x122000)
			{
				CreatureEffect(item, &dog_bite, DoBloodSplat);
				lara_item->hit_points -= 50;
				lara_item->hit_status = 1;
				dog->flags = 3;
			}

			if (info.distance < 0x1C639)
				item->goal_anim_state = 7;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, head);
	CreatureJoint(item, 2, head_x);
	CreatureAnimation(item_number, angle, tilt);
}
