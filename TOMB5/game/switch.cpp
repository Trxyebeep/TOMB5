#include "../tomb5/pch.h"
#include "switch.h"
#include "control.h"
#include "sound.h"
#include "items.h"
#include "collide.h"
#include "lara_states.h"
#include "objects.h"
#include "sphere.h"
#include "draw.h"

static PHD_VECTOR SwitchPos = { 0, 0, 0 };
static PHD_VECTOR CrowDovePos = { 0, 0, -400 };

static short PulleyBounds[12] =
{
	-256, 256, 0, 0, -512, 512, -1820, 1820, -5460, 5460, -1820, 1820
};

static short SwitchBounds[12] =
{
	0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820
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

	if (!(item->flags & IFL_INVISIBLE) && item->mesh_bits & 4 &&
		(input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP &&
		l->anim_number == ANIM_BREATH && !l->gravity_status || lara.IsMoving && lara.GeneralPtr == (void*)item_num))
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
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot = roty;
	}
	else if (l->current_anim_state != AS_CROWDOVE)
		ObjectCollision(item_num, l, coll);
}

void SwitchControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* switem;
	PHD_VECTOR pos;
	short TriggerItems[8];
	short NumTrigs, room_number;

	item = &items[item_number];

	if (item->object_number == AIRLOCK_SWITCH && item->trigger_flags >= 8)
	{
		if (item->anim_number == objects[AIRLOCK_SWITCH].anim_index + 1 && item->frame_number == anims[item->anim_number].frame_end - 1)
		{
			for (NumTrigs = GetSwitchTrigger(&items[item_number], TriggerItems, 1); NumTrigs > 0; NumTrigs--)
			{
				switem = &items[TriggerItems[NumTrigs - 1]];
				switem->flags ^= IFL_CODEBITS;

				if ((switem->flags & IFL_CODEBITS) == IFL_CODEBITS && switem->status != ITEM_ACTIVE)
				{
					AddActiveItem(TriggerItems[NumTrigs - 1]);
					switem->status = ITEM_ACTIVE;
				}
			}
		}

		if (item->trigger_flags == 8)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetJointAbsPosition(item, &pos, 0);
			room_number = item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_number);

			if (room_number != item->room_number)
				ItemNewRoom(item_number, room_number);
		}
	}
	else
	{
		item->flags |= IFL_CODEBITS;

		if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE))
		{
			if (item->object_number == JUMP_SWITCH)
				item->goal_anim_state = 0;
			else
				item->goal_anim_state = 1;

			item->timer = 0;
		}
	}

	AnimateItem(item);
}

void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;

	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS
		&& item->status == ITEM_INACTIVE && !(item->flags & IFL_INVISIBLE) && item->trigger_flags >= 0
		|| lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		bounds = GetBoundsAccurate(item);

		if ((item->trigger_flags == 3 && item->current_anim_state == 1) ||
			(item->trigger_flags >= 5 && item->trigger_flags <= 7 && item->current_anim_state == 0))
			return;

		SwitchBounds[0] = bounds[0] - 256;
		SwitchBounds[1] = bounds[1] + 256;

		if (!item->trigger_flags)
		{
			SwitchBounds[4] = bounds[4] - 200;
			SwitchBounds[5] = bounds[5] + 200;
			SwitchPos.z = bounds[4] - 64;
		}
		else
		{
			SwitchBounds[4] = bounds[4] - 512;
			SwitchBounds[5] = bounds[5] + 512;

			if (item->trigger_flags == 3)
				SwitchPos.z = bounds[4] - 256;
			else if (item->trigger_flags == 4)
				SwitchPos.z = bounds[4] - 88;
			else if (item->trigger_flags >= 5 && item->trigger_flags <= 7)
				SwitchPos.z = bounds[4] - 160;
			else if (item->trigger_flags >= 8)
				SwitchPos.z = bounds[4] - 96;
			else
				SwitchPos.z = bounds[4] - 128;
		}

		if (TestLaraPosition(SwitchBounds, item, l))
		{
			if (MoveLaraPosition(&SwitchPos, item, l))
			{
				if (item->current_anim_state == 1)
				{
					if (!item->trigger_flags)
					{
						l->anim_number = ANIM_SWITCHOFF;
						l->current_anim_state = AS_SWITCHOFF;
					}
					else if (item->trigger_flags < 3)
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
					}
					else if (item->trigger_flags == 4)
					{
						l->anim_number = ANIM_GENERATORSW_OFF;
						l->current_anim_state = AS_SWITCHOFF;
					}
					else if (item->trigger_flags >= 5 && item->trigger_flags <= 7)
					{
						if (item->trigger_flags == 6)
							bDisableLaraControl = 1;

						l->anim_number = ANIM_ONEHANDPUSHSW;
						l->current_anim_state = AS_SWITCHON;
					}

					item->goal_anim_state = 0;
				}
				else
				{
					if (!item->trigger_flags)
					{
						l->anim_number = ANIM_SWITCHON;
						l->current_anim_state = AS_SWITCHON;
					}
					else if (item->trigger_flags == 3)
					{
						l->anim_number = ANIM_SMALLSWITCH;
						l->current_anim_state = AS_SWITCHON;
					}
					else if (item->trigger_flags == 4)
					{
						l->anim_number = ANIM_GENERATORSW_ON;
						l->current_anim_state = AS_SWITCHON;
					}
					else if (item->trigger_flags < 8)
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
					}
					else
					{
						l->anim_number = ANIM_AIRLOCKSW;
						l->current_anim_state = AS_SWITCHON;
					}

					item->goal_anim_state = 1;
				}

				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				AddActiveItem(item_number);
				item->status = ITEM_ACTIVE;
				AnimateItem(item);
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (l->current_anim_state != AS_SWITCHON && l->current_anim_state != AS_SWITCHOFF)
		ObjectCollision(item_number, l, coll);
}

void inject_switch(bool replace)
{
	INJECT(0x0047FC80, CrowDoveSwitchControl, replace);
	INJECT(0x0047FD20, CrowDoveSwitchCollision, replace);
	INJECT(0x0047DA40, SwitchControl, replace);
	INJECT(0x0047DC70, SwitchCollision, replace);
}
