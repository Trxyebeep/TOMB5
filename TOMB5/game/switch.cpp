#include "../tomb5/pch.h"
#include "switch.h"
#include "control.h"
#include "sound.h"
#include "items.h"
#include "collide.h"
#include "lara_states.h"

static short PulleyBounds[12] =
{
	-256, 256, 0, 0, -512, 512, -1820, 1820, -5460, 5460, -1820, 1820
};

static PHD_VECTOR CrowDovePos =
{
	0, 0, -400
};

void CrowDoveSwitchControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->mesh_bits & 2)
	{
		ExplodeItemNode(item, 1, 0, 256);
		SoundEffect(SFX_RAVENSWITCH_EXP, &item->pos, 0);
		item->mesh_bits = 5;
		RemoveActiveItem(item_number);
	}
	else
	{
		if (!item->current_anim_state)
			item->goal_anim_state = 1;

		AnimateItem(item);

		if (!item->current_anim_state)
			item->pos.y_rot += 16384;
	}
}

void CrowDoveSwitchCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short roty;

	item = &items[item_num];

	if (!(item->flags & IFL_INVISIBLE) &&
		item->mesh_bits & 4 &&
		(input & IN_ACTION &&
		lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP &&
		l->anim_number == ANIM_BREATH &&
		!l->gravity_status ||
		lara.IsMoving &&
		lara.GeneralPtr == (void *) item_num))
	{
		roty = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(PulleyBounds, item, l))
		{
			if (MoveLaraPosition(&CrowDovePos, item, l))
			{
				l->anim_number = ANIM_CROWDOVE;
				l->current_anim_state = AS_CROWDOVE;
				l->frame_number = anims[l->anim_number].frame_base;
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.GeneralPtr = item;
			}
			else
				lara.GeneralPtr = (void *) item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void *) item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot = roty;
	}
	else if (l->current_anim_state != AS_CROWDOVE)
		ObjectCollision(item_num, l, coll);
}

void inject_switch(bool replace)
{
	INJECT(0x0047FC80, CrowDoveSwitchControl, replace);
	INJECT(0x0047FD20, CrowDoveSwitchCollision, replace);
}
