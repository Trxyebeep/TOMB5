#include "../tomb5/pch.h"
#include "autogun.h"
#include "sound.h"
#include "effect2.h"
#include "items.h"
#include "control.h"
#include "lara_states.h"
#include "switch.h"
#include "../specific/3dmath.h"
#include "sphere.h"
#include "delstuff.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "tomb4fx.h"

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

void AutogunControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* gun;
	GAME_VECTOR pos1;
	GAME_VECTOR pos2;
	GAME_VECTOR pos3;
	long dx, dy, dz;
	short angles[2];
	short los, x, y;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->frame_number >= anims[item->anim_number].frame_end)
	{
		gun = (CREATURE_INFO*)item->data;
		item->mesh_bits = 1664;
		pos1.y = 0;
		pos1.x = 0;
		pos1.z = -64;
		GetJointAbsPosition(item, (PHD_VECTOR*)&pos1, 8);
		pos2.z = 0;
		pos2.y = 0;
		pos2.x = 0;
		GetLaraJointPos((PHD_VECTOR*)&pos2, 0);
		pos1.room_number = item->room_number;
		los = (short)LOS(&pos1, &pos2);

		if (los)
		{
			phd_GetVectorAngles(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z, angles);
			angles[0] -= item->pos.y_rot;
		}
		else
		{
			angles[0] = item->item_flags[0];
			angles[1] = item->item_flags[1];
		}

		InterpolateAngle(angles[0], &item->item_flags[0], &y, 4);
		InterpolateAngle(angles[1], &item->item_flags[1], &x, 4);
		gun->joint_rotation[0] = item->item_flags[0];
		gun->joint_rotation[1] = item->item_flags[1];
		gun->joint_rotation[2] += item->item_flags[2];

		if (ABS(y) < 1024 && ABS(x) < 1024 && los)
		{
			SoundEffect(SFX_HK_FIRE, &item->pos, SFX_SETPITCH | 0xC00000);

			if (GlobalCounter & 1)
			{
				item->mesh_bits |= 256;
				TriggerDynamic(pos1.x, pos1.y, pos1.z, 10, (GetRandomControl() & 0x1F) + 192, (GetRandomControl() & 0x1F) + 128, 0);

				if (GetRandomControl() & 3)
				{
					pos2.x = 0;
					pos2.y = 0;
					pos2.z = 0;
					GetLaraJointPos((PHD_VECTOR*)&pos2, GetRandomControl() % 15);
					DoBloodSplat(pos2.x, pos2.y, pos2.z, (GetRandomControl() & 3) + 3, short(GetRandomControl() << 1), lara_item->room_number);
					lara_item->hit_points -= 20;
				}
				else
				{
					pos3.x = pos2.x;
					pos3.y = pos2.y;
					pos3.z = pos2.z;
					dx = pos2.x - pos1.x;
					dy = pos2.y - pos1.y;
					dz = pos2.z - pos1.z;

					do
					{
						dx <<= 1;
						dy <<= 1;
						dz <<= 1;

					} while (ABS(dx) < 12288 && ABS(dy) < 12288 && ABS(dz) < 12288);

					pos3.x += dx + (GetRandomControl() & 0xFF) - 128;
					pos3.y += dy + (GetRandomControl() & 0xFF) - 128;
					pos3.z += dz + (GetRandomControl() & 0xFF) - 128;

					if (!LOS(&pos1, &pos3))
						TriggerRicochetSpark(&pos3, GetRandomControl() << 1, 3, 0);
				}
			}
			else
				item->mesh_bits &= ~256;

			if (item->item_flags[2] < 1024)
				item->item_flags[2] += 64;
		}
		else
		{
			if (item->item_flags[2])
				item->item_flags[2] -= 64;

			item->mesh_bits &= ~256;
		}

		if (item->item_flags[2])
			TriggerAutoGunSmoke(&pos1, item->item_flags[2] >> 4);
	}
	else
	{
		item->mesh_bits = -1281;
		AnimateItem(item);
	}
}

void TriggerAutoGunSmoke(GAME_VECTOR* pos, long shade)
{
	SMOKE_SPARKS* sptr;

	sptr = &smoke_spark[GetFreeSmokeSpark()];
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = (uchar)shade;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 32;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + pos->x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos->y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos->z - 16;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 4;
	sptr->Flags = 16;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 31;
	sptr->MaxYvel = 0;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->mirror = 0;
	sptr->dSize = (GetRandomControl() & 0xF) + 24;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->dSize >> 2;
}

void inject_autogun(bool replace)
{
	INJECT(0x00407510, ControlMotionSensors, replace);
	INJECT(0x004078A0, AutogunControl, replace);
	INJECT(0x00407780, TriggerAutoGunSmoke, replace);
}
