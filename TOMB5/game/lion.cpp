#include "../tomb5/pch.h"
#include "lion.h"
#include "box.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO lion_pounce = { -2, -10, 250, 21 };
static BITE_INFO lion_bite = { -2, -10, 132, 21 };

void InitialiseLion(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[LION].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void LionControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* lion;
	AI_INFO info;
	short torso_y, angle, tilt, head;
	
	if (!CreatureActive(item_number))
		return;

	torso_y = 0;
	angle = 0;
	tilt = 0;
	head = 0;
	item = &items[item_number];
	lion = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 5)
		{
			item->anim_number = objects[LION].anim_index + (GetRandomControl() & 1) + 7;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 5;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, lion->maximum_turn);
		torso_y = -angle << 4;

		switch (item->current_anim_state)
		{
		case 1:
			lion->maximum_turn = 0;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (lion->mood == BORED_MOOD)
			{
				if (!(GetRandomControl() & 0x3F))
					item->goal_anim_state = 2;
			}
			else if (info.ahead && item->touch_bits & 0x200048)
				item->goal_anim_state = 7;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = 4;
			else
				item->goal_anim_state = 3;

			break;

		case 2:
			lion->maximum_turn = 364;

			if (lion->mood != BORED_MOOD)
				item->goal_anim_state = 1;
			else if (GetRandomControl() < 128)
			{
				item->required_anim_state = 6;
				item->goal_anim_state = 1;
			}

			break;

		case 3:
			lion->maximum_turn = 910;
			tilt = angle;

			if (lion->mood == BORED_MOOD)
				item->goal_anim_state = 1;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = 1;
			else if (item->touch_bits & 0x200048 && info.ahead)
				item->goal_anim_state = 1;
			else if (lion->mood != ESCAPE_MOOD && GetRandomControl() < 128)
			{
				item->required_anim_state = 6;
				item->goal_anim_state = 1;
			}

			break;

		case 4:

			if (!item->required_anim_state && item->touch_bits & 0x200048)
			{
				lara_item->hit_points -= 200;
				lara_item->hit_status = 1;
				CreatureEffectT(item, &lion_pounce, 10, item->pos.y_rot, DoBloodSplat);
				item->required_anim_state = 1;
			}
			
			break;

		case 7:
			lion->maximum_turn = 182;

			if (!item->required_anim_state && item->touch_bits & 0x200048)
			{
				CreatureEffectT(item, &lion_bite, 10, item->pos.y_rot, DoBloodSplat);
				lara_item->hit_points -= 60;
				lara_item->hit_status = 1;
				item->required_anim_state = 1;
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, head);
	CreatureAnimation(item_number, angle, 0);
}
