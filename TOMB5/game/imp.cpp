#include "../tomb5/pch.h"
#include "imp.h"
#include "sphere.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "items.h"
#include "objects.h"
#include "box.h"
#include "lara_states.h"
#include "effects.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "lara.h"
#include "effect2.h"
#include "flmtorch.h"

static BITE_INFO imp_hit = { 0, 100, 0, 9 };

void TriggerImpMissile(ITEM_INFO* item)
{
	FX_INFO* fx;
	PHD_VECTOR pos, pos2;
	long dx, dy, dz;
	short angles[2];
	short fx_number;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, 9);

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = 0;
	GetLaraJointPos(&pos2, LMX_HEAD);

	dx = pos.x - pos2.x;
	dy = pos.y - pos2.y;
	dz = pos.z - pos2.z;
	phd_GetVectorAngles(pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, angles);
	dz = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

	if (dz < 8)
		dz = 8;

	angles[0] += short(GetRandomControl() % (dz >> 2) - (dz >> 3));
	angles[1] += short(GetRandomControl() % (dz >> 1) - (dz >> 2));
	fx_number = CreateEffect(item->room_number);

	if (fx_number != NO_ITEM)
	{
		fx = &effects[fx_number];
		fx->pos.x_pos = pos.x;
		fx->pos.y_pos = pos.y;
		fx->pos.z_pos = pos.z;
		fx->room_number = item->room_number;
		fx->pos.x_rot = short(angles[1] + (dz >> 1));
		fx->pos.y_rot = angles[0];
		fx->pos.z_rot = 0;
		fx->speed = short(4 * phd_sqrt(dz));

		if (fx->speed < 256)
			fx->speed = 256;

		fx->fallspeed = 0;
		fx->object_number = BUBBLES;
		fx->shade = 0x4210;
		fx->counter = 0;
		fx->frame_number = objects[BUBBLES].mesh_index + 2 * (GetRandomControl() & 7);
		fx->flag1 = 2;
		fx->flag2 = 0x2000;
	}
}

void InitialiseImp(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 2 || item->trigger_flags == 12)
	{
		item->anim_number = objects[IMP].anim_index + 8;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
	}
	else if (item->trigger_flags == 1 || item->trigger_flags == 11)
	{
		item->anim_number = objects[IMP].anim_index + 7;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 7;
		item->goal_anim_state = 7;
	}
	else
	{
		item->anim_number = objects[IMP].anim_index + 1;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}
}

void ImpControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* imp;
	AI_INFO info;
	short torso_x, torso_y, angle, other_angle, head_x, head_y, elevation;

	if (!CreatureActive(item_number))
		return;

	torso_x = 0;
	torso_y = 0;
	angle = 0;
	other_angle = 0;
	head_x = 0;
	head_y = 0;
	elevation = 0;
	item = &items[item_number];
	imp = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 9)
		{
			item->anim_number = objects[IMP].anim_index + 45;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 9;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(imp);
		else if (imp->hurt_by_lara)
			imp->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (imp->enemy == lara_item)
			other_angle = info.angle;
		else
			other_angle = short(phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos) - item->pos.y_rot);

		elevation = short(item->pos.y_pos - lara_item->pos.y_pos + 384);

		if (lara_item->current_anim_state == AS_DUCK || lara_item->current_anim_state == AS_DUCKROLL ||
			(lara_item->current_anim_state > AS_MONKEY180 && lara_item->current_anim_state < AS_HANG2DUCK) ||
			lara_item->current_anim_state == AS_DUCKROTL || lara_item->current_anim_state == AS_DUCKROTR)
			elevation = short(item->pos.y_pos - lara_item->pos.y_pos);

		info.x_angle = (short)(phd_atan(phd_sqrt(info.distance), elevation));
		GetCreatureMood(item, &info, 1);

		if (item->current_anim_state == 6)
			imp->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, imp->maximum_turn);
		torso_y = info.angle >> 1;
		torso_x = info.x_angle >> 1;
		head_y = info.angle >> 1;
		head_x = info.x_angle >> 1;

		if (wibble & 0x10)
			item->meshswap_meshbits = 1024;
		else
			item->meshswap_meshbits = 0;

		switch (item->current_anim_state)
		{
		case 0:
			imp->maximum_turn = 1274;

			if (info.distance > 0x400000)
				item->goal_anim_state = 2;
			else if (info.distance < 0x40000)
				item->goal_anim_state = 1;

			break;

		case 1:
			imp->maximum_turn = -1;
			imp->flags = 0;

			if (info.bite && info.distance < 0x70E4 && item->trigger_flags < 10)
			{
				if (GetRandomControl() & 1)
					item->goal_anim_state = 3;
				else
					item->goal_anim_state = 5;
			}
			else if (item->ai_bits == FOLLOW)
				item->goal_anim_state = 0;
			else if (item->trigger_flags == 3)
				item->goal_anim_state = 11;
			else if (info.distance > 0x400000)
				item->goal_anim_state = 2;
			else if (info.distance > 0x40000 || item->trigger_flags < 10)
				item->goal_anim_state = 0;

			break;

		case 2:
			imp->maximum_turn = 1274;

			if (info.distance < 0x40000)
				item->goal_anim_state = 1;
			else if (info.distance < 0x400000)
				item->goal_anim_state = 0;

			break;

		case 3:
		case 5:
			imp->maximum_turn = -1;

			if (!imp->flags && item->touch_bits & 0x280)
			{
				lara_item->hit_points -= 3;
				lara_item->hit_status = 1;
				CreatureEffectT(item, &imp_hit, 10, item->pos.y_rot, DoBloodSplat);
			}

			break;

		case 6:
			imp->maximum_turn = 1274;

			if (torchroom != 11)
				item->goal_anim_state = 1;

			break;

		case 7:
		case 8:
			imp->maximum_turn = 0;
			break;

		case 11:
			imp->maximum_turn = -1;

			if (item->frame_number - anims[item->anim_number].frame_base == 40)
				TriggerImpMissile(item);

			break;
		}
	}

	if (imp->maximum_turn == -1)
	{
		imp->maximum_turn = 0;

		if (abs(other_angle) < 364)
			item->pos.y_rot += other_angle;
		else if (other_angle < 0)
			item->pos.y_rot -= 364;
		else
			item->pos.y_rot += 364;
	}

	if (torchroom == 11)
		item->goal_anim_state = 6;

	CreatureTilt(item, 0);
	CreatureJoint(item, 1, torso_y);
	CreatureJoint(item, 0, torso_x);
	CreatureJoint(item, 3, head_y);
	CreatureJoint(item, 2, head_x);
	CreatureAnimation(item_number, angle, 0);
}
