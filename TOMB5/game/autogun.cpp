#include "../tomb5/pch.h"
#include "autogun.h"
#include "sound.h"
#include "effect2.h"
#include "items.h"
#include "control.h"
#include "lara_states.h"
#include "switch.h"
#include "../specific/3dmath.h"

void ControlMotionSensors(short item_number)
{
	ITEM_INFO* item;
	short angles[2];
	short angdiff, destangle;

	item = &items[item_number];

	if (!(item->mesh_bits & 1))
	{
		SoundEffect(SFX_EXPLOSION2, &item->pos, 0);
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 0, item->room_number);
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, 0, 0, item->room_number);
		KillItem(item_number);
	}
	else if (!item->item_flags[2])
	{
		InterpolateAngle(-2048, &item->pos.x_rot, NULL, 3);
		destangle = item->item_flags[3];

		if (item->item_flags[0])
			destangle += 20480;

		InterpolateAngle(destangle, &item->pos.y_rot, &angdiff, 5);

		if (ABS(angdiff) < 256)
			item->item_flags[0] ^= 1;

		if (lara_item->current_anim_state != AS_WALK &&
			lara_item->current_anim_state != AS_STOP &&
			lara_item->current_anim_state != AS_STEPLEFT &&
			lara_item->current_anim_state != AS_STEPRIGHT &&
			lara_item->current_anim_state != AS_BACK &&
			lara_item->current_anim_state != AS_TURN_R &&
			lara_item->current_anim_state != AS_TURN_L &&
			lara_item->current_anim_state != AS_POSE &&
			lara_item->current_anim_state != AS_INTO_ZIP &&
			lara_item->current_anim_state != AS_ZIP &&
			lara_item->current_anim_state != AS_OUT_ZIP &&
			lara_item->current_anim_state != AS_SWITCHON &&
			lara_item->current_anim_state != AS_SWITCHOFF)
		{
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			item->item_flags[2] = 1;
		}
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - item->pos.x_pos, lara_item->pos.y_pos - item->pos.y_pos - 384, lara_item->pos.z_pos - item->pos.z_pos, angles);
		InterpolateAngle(angles[0], &item->pos.y_rot, NULL, 3);
		InterpolateAngle(angles[1], &item->pos.x_rot, NULL, 3);
	}
}

void inject_autogun(bool replace)
{
	INJECT(0x00407510, ControlMotionSensors, replace);
}
