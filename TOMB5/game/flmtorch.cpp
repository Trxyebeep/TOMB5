#include "../tomb5/pch.h"
#include "flmtorch.h"
#include "lara_states.h"
#include "objects.h"
#include "collide.h"
#include "switch.h"
#include "items.h"

static short FireBounds[12] =
{
	0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820
};

void FireCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short rot;

	item = &items[item_num];

	if (lara.gun_type == WEAPON_TORCH && lara.gun_status == LG_READY && !lara.left_arm.lock && (item->status & 1) != lara.LitTorch && item->timer != -1 && input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status)
	{
		rot = item->pos.y_rot;

		if (item->object_number == FLAME_EMITTER)
		{
			FireBounds[0] = -256;
			FireBounds[1] = 256;
			FireBounds[2] = 0;
			FireBounds[3] = 1024;
			FireBounds[4] = -800;
			FireBounds[5] = 800;
		}
		else if (item->object_number == FLAME_EMITTER2)
		{
			FireBounds[0] = -256;
			FireBounds[1] = 256;
			FireBounds[2] = 0;
			FireBounds[3] = 1024;
			FireBounds[4] = -600;
			FireBounds[5] = 600;
		}
		else if (item->object_number == BURNING_ROOTS)
		{
			FireBounds[0] = -384;
			FireBounds[1] = 384;
			FireBounds[2] = 0;
			FireBounds[3] = 2048;
			FireBounds[4] = -384;
			FireBounds[5] = 384;
		}

		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(FireBounds, item, l))
		{
			if (item->object_number != BURNING_ROOTS)
			{
				l->item_flags[3] = 1;
				l->anim_number = (short) (ANIM_LIGHT_TORCH1 + (ABS(l->pos.y_pos - item->pos.y_pos) >> 8));
			}
			else
				l->anim_number = ANIM_LIGHT_TORCH5;

			l->current_anim_state = AS_CONTROLLED;
			l->frame_number = anims[l->anim_number].frame_base;
			lara.flare_control_left = 0;
			lara.left_arm.lock = 3;
			lara.GeneralPtr = (void*) item_num;
		}

		item->pos.y_rot = rot;
	}
	else if (item->object_number == BURNING_ROOTS)
		ObjectCollision(item_num, l, coll);

	if (lara.GeneralPtr == (void*) item_num && item->status != ITEM_ACTIVE && l->current_anim_state == AS_CONTROLLED && l->anim_number >= ANIM_LIGHT_TORCH1 && l->anim_number <= ANIM_LIGHT_TORCH5 && l->frame_number - anims[l->anim_number].frame_base == 40)
	{
		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, item->flags & IFL_CODEBITS);
		item->flags |= IFL_CODEBITS;
		item->item_flags[3] = 0;
		item->status = ITEM_ACTIVE;
		AddActiveItem(item_num);
	}
}

void inject_flmtorch(bool replace)
{
	INJECT(0x00433B40, FireCollision, replace);
}
