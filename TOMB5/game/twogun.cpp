#include "../tomb5/pch.h"
#include "twogun.h"
#include "lara_states.h"
#include "items.h"
#include "../specific/audio.h"
#include "box.h"
#include "sphere.h"
#include "effect2.h"
#include "control.h"
#include "effects.h"
#include "../specific/3dmath.h"
#include "people.h"

static BITE_INFO Guns[2] =
{
	{ 0, 230, 40, 8 },
	{ 8, 230, 40, 5 }
};

static BITE_INFO twogun_head = { 0,-200, 0, 2 };

void ControlZipController(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->item_flags[0])
	{
		bDisableLaraControl = 1;
		item->item_flags[0]++;
	}
	else if (item->item_flags[0] == 1)
	{
		if (lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH)
		{
			lara_item->goal_anim_state = AS_INTO_ZIP;
			lara_item->current_anim_state = AS_INTO_ZIP;
			lara_item->anim_number = ANIM_ZIP_IN;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			item->item_flags[0]++;
			IsAtmospherePlaying = 0;
			S_CDPlay(item->trigger_flags, 0);
		}
	}
	else if (item->item_flags[0] == 2)
	{
		if (!item->item_flags[1])
		{
			if (XATrack == item->trigger_flags)
				item->item_flags[1] = 1;
		}
		else if (XATrack != item->trigger_flags)
		{
			lara_item->goal_anim_state = AS_OUT_ZIP;
			bDisableLaraControl = 0;
			KillItem(item_number);
		}
	}
}

void InitialiseTwogun(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
}

void TwogunControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* _2gun;
	BITE_INFO* bite;
	PHD_VECTOR pos;
	AI_INFO twogunInfo;
	AI_INFO lara_info;
	long dx, dz;
	short angle, head, torso_x, torso_y, frame, base, room_number;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	_2gun = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->item_flags[0] || item->item_flags[1])
	{
		bite = Guns;

		for (int i = 0; i < 2; i++)
		{
			if (item->item_flags[i])
			{
				pos.x = bite->x;
				pos.y = bite->y;
				pos.z = bite->z;
				GetJointAbsPosition(item, &pos, bite->mesh_num);
				FireTwogunWeapon(item, i, 1);

				if (item->item_flags[i] <= 0)
					item->item_flags[i]++;
				else
				{
					TriggerDynamic(pos.x, pos.y, pos.z, item->item_flags[i] + 8, 0, (item->item_flags[i] << 4) >> 2, item->item_flags[i] << 4);
					item->item_flags[i]--;
				}
			}
		}
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 7 && item->trigger_flags != 1)
		{
			item->anim_number = objects[item->object_number].anim_index + 3;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 7;
		}
		else if (item->trigger_flags == 1)
		{
			switch (item->current_anim_state)
			{
			case 11:
				frame = item->frame_number;
				base = anims[item->anim_number].frame_base;

				if (frame == base + 48 || frame == base + 15)
				{
					room_number = item->room_number;
					GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
						item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
					TestTriggers(trigger_index, 1, 0);
				}

				break;

			case 12:
				item->gravity_status = 1;

				if (item->pos.y_pos >= item->floor)
				{
					item->pos.y_pos = item->floor;
					item->gravity_status = 0;
					item->fallspeed = 0;
					item->goal_anim_state = 13;
					CreatureEffectT(item, &twogun_head, 20, item->pos.y_rot, DoBloodSplat);
				}

				break;

			case 13:
				item->pos.y_pos = item->floor;
				break;

			default:
				item->anim_number = objects[item->object_number].anim_index + 14;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 11;
				_2gun->LOT.is_jumping = 1;
				item->pos.x_pos = room[item->room_number].x + ((item->draw_room & 0xFFFFFF00) << 2) + 512;
				item->pos.y_pos = room[item->room_number].minfloor + (item->item_flags[2] & 0xFFFFFF00);
				item->pos.z_pos = ((item->draw_room & 0xFF) << 10) + room[item->room_number].z + 512;
				item->pos.y_rot = item->TOSSPAD & 57344;
				break;
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(_2gun);
		else
			_2gun->enemy = lara_item;

		CreatureAIInfo(item, &twogunInfo);

		if (_2gun->enemy == lara_item)
		{
			lara_info.angle = twogunInfo.angle;
			lara_info.distance = twogunInfo.distance;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			lara_info.angle = (short)(phd_atan(dz, dx) - item->pos.y_rot);
			lara_info.distance = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &twogunInfo, _2gun->enemy != lara_item);
		CreatureMood(item, &twogunInfo, _2gun->enemy != lara_item);
		angle = CreatureTurn(item, _2gun->maximum_turn);

		if (((lara_info.distance < 0x400000 && (lara_info.angle < 0x4000 && lara_info.angle > -16384 || lara_item->speed > 20)) ||
			item->hit_status || TargetVisible(item, &lara_info)) && ABS(item->pos.y_pos - lara_item->pos.y_pos) < 1536)
		{
			_2gun->enemy = lara_item;
			AlertAllGuards(item_number);
		}

		switch (item->current_anim_state)
		{
		case 1:
			_2gun->flags = 0;
			_2gun->maximum_turn = 0;

			if (!(item->ai_bits & 1))
			{
				if (ABS(twogunInfo.angle) < 364)
					item->pos.y_rot += twogunInfo.angle;
				else
				{
					if (twogunInfo.angle >= 0)
						item->pos.y_rot += 364;
					else
						item->pos.y_rot -= 364;
				}

				head = lara_info.angle >> 1;
				torso_y = lara_info.angle >> 1;
				torso_x = twogunInfo.x_angle >> 1;
			}

			if (item->ai_bits & 1)
				head = AIGuard(_2gun);
			else
			{
				if (lara_info.angle > 20480 || lara_info.angle < -20480)
					item->goal_anim_state = 8;
				else if (!Targetable(item, &twogunInfo))
				{
					if (item->trigger_flags != 1)
						item->goal_anim_state = 2;
				}
				else if (twogunInfo.distance < 0x900000 || twogunInfo.zone_number != twogunInfo.enemy_zone)
					item->goal_anim_state = 5;
				else if (item->ai_bits != 8)
				{
					if (item->trigger_flags != 1)
						item->goal_anim_state = 2;
				}
			}

			break;

		case 2:
			_2gun->maximum_turn = 910;

			if (Targetable(item, &twogunInfo) && lara_info.angle < 6144 && lara_info.angle > -6144)
			{
				if (item->frame_number >= anims[item->anim_number].frame_base + 29)
					item->goal_anim_state = 3;
				else
					item->goal_anim_state = 4;
			}
			else
			{
				if (lara_info.angle > 20480 || lara_info.angle < -20480)
					item->goal_anim_state = 1;
				else
					item->goal_anim_state = 2;
			}

			break;

		case 3:
		case 4:
			head = lara_info.angle;

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				if (item->current_anim_state == 4)
					FireTwogunWeapon(item, 0, 0);
				else
					FireTwogunWeapon(item, 1, 0);
			}

			break;

		case 5:
			_2gun->flags = 0;
			_2gun->maximum_turn = 0;
			head = lara_info.angle >> 1;
			torso_y = lara_info.angle >> 1;
			torso_x = twogunInfo.x_angle >> 1;

			if (ABS(twogunInfo.angle) < 364)
				item->pos.y_rot += twogunInfo.angle;
			else if (twogunInfo.angle >= 0)
				item->pos.y_rot += 364;
			else
				item->pos.y_rot -= 364;

			if (item->trigger_flags == 2)
				item->goal_anim_state = 9;
			else if (item->trigger_flags == 3)
				item->goal_anim_state = 10;
			else if (Targetable(item, &twogunInfo))
				item->goal_anim_state = 6;
			else
				item->goal_anim_state = 1;

			break;

		case 6:
			head = lara_info.angle >> 1;
			torso_y = lara_info.angle >> 1;
			torso_x = twogunInfo.x_angle;

			if (ABS(twogunInfo.angle) < 364)
				item->pos.y_rot += twogunInfo.angle;
			else
			{
				if (twogunInfo.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}

			if (item->frame_number == anims[item->anim_number].frame_base + 17)
			{
				FireTwogunWeapon(item, 0, 0);
				FireTwogunWeapon(item, 1, 0);
			}

			break;

		case 8:
			_2gun->flags = 0;
			_2gun->maximum_turn = 0;

			if (twogunInfo.angle < 0)
				item->pos.y_rot += 364;
			else
				item->pos.y_rot -= 364;

			if (item->frame_number == anims[item->anim_number].frame_end)
				item->pos.y_rot += 32768;

			break;

		case 9:
			torso_x = twogunInfo.x_angle >> 1;
			head = lara_info.angle >> 1;
			torso_y = lara_info.angle >> 1;

			if (item->frame_number == anims[item->anim_number].frame_base + 18)
			{
				FireTwogunWeapon(item, 0, 0);
				item->item_flags[1] = -16;
				item->trigger_flags = 3;
			}

			break;

		case 10:
			item->trigger_flags = 0;
			break;
		}
	}

	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}

void inject_twogun(bool replace)
{
	INJECT(0x0048E3C0, ControlZipController, replace);
	INJECT(0x0048CD40, InitialiseTwogun, replace);
	INJECT(0x0048CDD0, TwogunControl, replace);
}
