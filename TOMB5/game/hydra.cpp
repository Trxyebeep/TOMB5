#include "../tomb5/pch.h"
#include "hydra.h"
#include "objects.h"
#include "box.h"
#include "control.h"
#include "sound.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"
#include "sphere.h"

static BITE_INFO hydra_hit = {0, 0, 0, 11};

void InitialiseHydra(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[HYDRA].anim_index;
	item->frame_number = 30 * item->trigger_flags + anims[item->anim_number].frame_base;
	item->goal_anim_state = 0;
	item->current_anim_state = 0;

	if (item->trigger_flags == 1)
		item->pos.z_pos += 384;
	else if (item->trigger_flags == 2)
		item->pos.z_pos -= 384;

	item->pos.y_rot = 16384;
	item->pos.x_pos -= 256;
}

void HydraControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* hydra;
	PHD_VECTOR pos;
	PHD_3DPOS pos2;
	AI_INFO info;
	short angles[2];
	short tilt, head_z, head_x, head_y, neck_y, shade, damage, room_number;

	if (!CreatureActive(item_number))
		return;

	tilt = 0;
	head_z = 0;
	head_x = 0;
	head_y = 0;
	neck_y = 0;
	item = &items[item_number];
	hydra = (CREATURE_INFO*)item->data;

	if (item->hit_points > 0)
	{
		if (item->ai_bits)
			GetAITarget(hydra);
		else if (hydra->hurt_by_lara)
			hydra->enemy = lara_item;

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (item->current_anim_state != 5 && item->current_anim_state != 10 && item->current_anim_state != 11)
		{
			if (ABS(info.angle) < 182)
				item->pos.y_rot += info.angle;
			else if (info.angle >= 0)
				item->pos.y_rot += 182;
			else
				item->pos.y_rot -= 182;

			if (item->trigger_flags == 1)
				tilt = -512;
			else if (item->trigger_flags == 2)
				tilt = 512;
		}

		if (item->current_anim_state != 12)
		{
			head_y = info.angle >> 1;
			head_z = info.angle >> 1;
			head_x = -info.x_angle;
		}

		neck_y = -head_y;

		switch (item->current_anim_state)
		{
		case 0:
			hydra->maximum_turn = 182;
			hydra->flags = 0;

			if (item->trigger_flags == 1)
				tilt = -512;
			else if (item->trigger_flags == 2)
				tilt = 512;

			if (info.distance < 0x310000 || !(GetRandomControl() & 0x1F))
				item->goal_anim_state = 6;
			else
			{
				if (info.distance < 0x400000 || !(GetRandomControl() & 0xF))
					item->goal_anim_state = 1;
				else if (!(GetRandomControl() & 0xF))
					item->goal_anim_state = 2;
			}

			break;

		case 1:
		case 7:
		case 8:
		case 9:
			hydra->maximum_turn = 0;

			if (!hydra->flags)
			{
				if (item->touch_bits & 0x400)
				{
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &hydra_hit, 10, item->pos.y_rot, DoBloodSplat);
					hydra->flags = 1;
				}

				if (item->hit_status && info.distance < 0x310000)
				{
					damage = (short)(5 - phd_sqrt(info.distance) / 1024);

					if (lara.gun_type == WEAPON_SHOTGUN)
						damage *= 3;

					if (damage > 0)
					{
						item->goal_anim_state = 4;
						item->hit_points -= damage;
						CreatureEffectT(item, &hydra_hit, damage * 10, item->pos.y_rot, DoBloodSplat);
					}
				}
			}

			break;

		case 2:
			hydra->maximum_turn = 0;

			if (item->hit_status)
			{
				damage = (short)(6 - phd_sqrt(info.distance) / 1024);

				if (lara.gun_type == WEAPON_SHOTGUN)
					damage *= 3;

				if ((GetRandomControl() & 0xF) < damage && info.distance < 0x6400000 && damage > 0)
				{
					item->hit_points -= damage;
					item->goal_anim_state = 4;
					CreatureEffectT(item, &hydra_hit, 10 * damage, item->pos.y_rot, DoBloodSplat);
				}
			}

			if (item->trigger_flags == 1)
				tilt = -512;
			else if (item->trigger_flags == 2)
				tilt = 512;

			if (!(GlobalCounter & 3))
			{
				shade = ((anims[item->anim_number].frame_base - item->frame_number) >> 3) + 1;

				if (shade > 16)
					shade = 16;

				TriggerHydraPowerupFlames(item_number, shade);
			}

			break;

		case 3:

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				pos.x = 0;
				pos.y = 1024;
				pos.z = 40;
				GetJointAbsPosition(item, &pos, 10);
				pos2.x_pos = 0;
				pos2.y_pos = 144;
				pos2.z_pos = 40;
				GetJointAbsPosition(item, (PHD_VECTOR*)&pos2, 10);
				phd_GetVectorAngles(pos.x - pos2.x_pos, pos.y - pos2.y_pos, pos.z - pos2.z_pos, &angles[0]);
				pos2.x_rot = angles[1];
				pos2.y_rot = angles[0];
				room_number = item->room_number;
				GetFloor(pos2.x_pos, pos2.y_pos, pos2.z_pos, &room_number);
				TriggerHydraMissile(&pos2, room_number, 1);
			}
			break;

		case 6:
			hydra->maximum_turn = 182;
			hydra->flags = 0;

			if (item->trigger_flags == 1)
				tilt = -512;
			else if (item->trigger_flags == 2)
				tilt = 512;

			if (info.distance < 0x90000)
				item->goal_anim_state = 7;
			else if (info.distance < 0x190000)
				item->goal_anim_state = 8;
			else if (info.distance < 0x310000)
				item->goal_anim_state = 9;
			else
				item->goal_anim_state = 0;

			break;
		}
	}
	else
	{
		item->hit_points = 0;

		if (item->current_anim_state != 11)
		{
			item->anim_number = objects[HYDRA].anim_index + 15;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 11;
		}
		
		if (!((item->frame_number - anims[item->anim_number].frame_base) & 7))
		{
			if (item->item_flags[3] < 12)
			{
				ExplodeItemNode(item, 11 - item->item_flags[3], 0, 64);
				SoundEffect(SFX_SMASH_ROCK, &item->pos, 0);
				item->item_flags[3]++;
			}
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, neck_y);
	CreatureJoint(item, 1, head_y);
	CreatureJoint(item, 2, head_x);
	CreatureJoint(item, 3, head_z);
	CreatureAnimation(item_number, 0, 0);
}

void inject_hydra()
{
	INJECT(0x0043BEA0, InitialiseHydra);
	INJECT(0x0043BF70, HydraControl);
}
