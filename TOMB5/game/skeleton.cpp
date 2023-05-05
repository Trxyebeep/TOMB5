#include "../tomb5/pch.h"
#include "skeleton.h"
#include "box.h"
#include "objects.h"
#include "sound.h"
#include "effects.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "effect2.h"
#include "sphere.h"
#include "tomb4fx.h"
#include "control.h"
#include "lara.h"

static BITE_INFO skeleton_hit = { 0, 0, 0, 17 };

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

	if (abs(info.angle) < 546)
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

void TriggerSkeletonFire(ITEM_INFO* item)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = (GetRandomControl() & 0xF) + 64;
	sptr->dG = sptr->dR - (sptr->dR >> 2);
	sptr->dB = 0;
	sptr->sR = 0;
	sptr->sB = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sB >> 1;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 16;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x3F) - 32;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0x3F) - 32;
	sptr->Xvel = (GetRandomControl() & 0x1FF) - 256;
	sptr->Yvel = 0;
	sptr->Zvel = (GetRandomControl() & 0x1FF) - 256;
	sptr->Friction = 4;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->NodeNumber = 8;
	sptr->FxObj = item - items;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->MaxYvel = 0;
	sptr->Gravity = -16 - (GetRandomControl() & 0xF);
	sptr->Scalar = 3;
	sptr->dSize = 4;
	sptr->Size = (GetRandomControl() & 0x1F) + 32;
	sptr->sSize = sptr->Size;
}

void TriggerFontFire(ITEM_INFO* item, long num, long loop)
{
	SPARKS* sptr;
	PHD_VECTOR pos;
	long numbak, color, falloff;

	numbak = num;

	if (!num || num > 32)
		num = 32;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	for (; loop > 0; loop--)
	{
		pos.x = (num * ((GetRandomControl() & 0x7F) - 64)) >> 5;
		pos.y = -80;
		pos.z = (num * ((GetRandomControl() & 0x7F) - 64)) >> 5;
		GetJointAbsPosition(item, &pos, 0);
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->dB = 0;
		sptr->sR = 0;
		sptr->dR = uchar((num * ((GetRandomControl() & 0xF) + 64)) >> 5);
		sptr->dG = sptr->dR - (sptr->dR >> 2);
		sptr->sB = uchar((num * ((GetRandomControl() & 0x1F) + 48)) >> 5);
		sptr->sG = sptr->sB >> 1;
		sptr->FadeToBlack = 4;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 16;
		sptr->TransType = 2;
		sptr->Dynamic = -1;
		sptr->Life = (GetRandomControl() & 3) + 32;
		sptr->sLife = sptr->Life;
		sptr->x = pos.x;
		sptr->y = pos.y;
		sptr->z = pos.z;
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = 0;
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Friction = 3;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
		sptr->MaxYvel = 0;
		sptr->Gravity = -8 - (GetRandomControl() & 7);
		sptr->Scalar = 2;
		sptr->dSize = 4;
		sptr->Size = (uchar)((num * ((GetRandomControl() & 0x1F) + 64)) >> 5);
		sptr->sSize = sptr->Size;
	}

	if (numbak > 111)
	{
		falloff = 8;
		color = 127 - numbak;
	}
	else if (numbak < 32)
	{
		falloff = 24 - (numbak >> 1);
		color = 32 - (numbak >> 1);
	}
	else
	{
		falloff = 8;
		color = 16;
	}

	color = ((64 + (GetRandomControl() & 15)) * color) >> 4;
	TriggerDynamic(pos.x, pos.y, pos.z, falloff, color, color - (color >> 2), color >> 1);
}
