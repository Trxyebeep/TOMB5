#include "../tomb5/pch.h"
#include "tower2.h"
#include "control.h"
#include "sound.h"
#include "delstuff.h"
#include "objects.h"
#include "effect2.h"
#include "effects.h"
#include "items.h"
#include "tomb4fx.h"
#include "debris.h"
#include "switch.h"

void ControlGunship(short item_number)
{
	ITEM_INFO* item;
	MESH_INFO* StaticMesh;
	SPARKS* sptr;
	PHD_VECTOR v;
	GAME_VECTOR pos, pos1, pos2;
	long Target, ricochet, LaraOnLOS;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		SoundEffect(SFX_HELICOPTER_LOOP, &item->pos, 0);
		pos.x = (GetRandomControl() & 0x1FF) - 255;
		pos.y = (GetRandomControl() & 0x1FF) - 255;
		pos.z = (GetRandomControl() & 0x1FF) - 255;
		GetLaraJointPos((PHD_VECTOR*)&pos, LM_TORSO);
		pos1.x = pos.x;
		pos1.y = pos.y;
		pos1.z = pos.z;

		if (!item->item_flags[0] && !item->item_flags[1] && !item->item_flags[2])
		{
			item->item_flags[0] = (short)(pos.x >> 4);
			item->item_flags[1] = (short)(pos.y >> 4);
			item->item_flags[2] = (short)(pos.z >> 4);
		}

		pos.x = (pos.x + 80 * item->item_flags[0]) / 6;
		pos.y = (pos.y + 80 * item->item_flags[1]) / 6;
		pos.z = (pos.z + 80 * item->item_flags[2]) / 6;
		item->item_flags[0] = (short)(pos.x >> 4);
		item->item_flags[1] = (short)(pos.y >> 4);
		item->item_flags[2] = (short)(pos.z >> 4);

		if (item->trigger_flags == 1)
			item->pos.z_pos += (pos1.z - item->pos.z_pos) >> 5;
		else
			item->pos.x_pos += (pos1.x - item->pos.x_pos) >> 5;

		item->pos.y_pos += (pos1.y - item->pos.y_pos - 256) >> 5;
		pos2.x = item->pos.x_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.y = item->pos.y_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.z = item->pos.z_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.room_number = item->room_number;
		LaraOnLOS = LOS(&pos2, &pos1);
		pos1.x = 3 * pos.x - 2 * pos2.x;
		pos1.y = 3 * pos.y - 2 * pos2.y;
		pos1.z = 3 * pos.z - 2 * pos2.z;
		ricochet = !LOS(&pos2, &pos1);
		NotHitLaraCount = LaraOnLOS ? 1 : NotHitLaraCount + 1;

		if (NotHitLaraCount < 15)
			item->mesh_bits |= 0x100;
		else
			item->mesh_bits &= ~0x100;

		if (NotHitLaraCount < 15)
			SoundEffect(SFX_HK_FIRE, &item->pos, 0xC00000 | SFX_SETPITCH);

		if (GlobalCounter & 1)
		{
			GetLaraOnLOS = 1;
			Target = ObjectOnLOS2(&pos2, &pos1, &v, &StaticMesh);
			GetLaraOnLOS = 0;

			if (Target != 999 && Target >= 0)
			{
				if (items[Target].object_number == LARA)
				{
					TriggerDynamic(pos2.x, pos2.y, pos2.z, 16, (GetRandomControl() & 0x3F) + 96, (GetRandomControl() & 0x1F) + 64, 0);
					DoBloodSplat(v.x, v.y, v.z, (GetRandomControl() & 1) + 2, 2 * GetRandomControl(), lara_item->room_number);
					lara_item->hit_points -= 20;
				}
				else if (items[Target].object_number >= SMASH_OBJECT1 && items[Target].object_number <= SMASH_OBJECT8)
				{
					ExplodeItemNode(&items[Target], 0, 0, 0x80);
					SmashObject((short)Target);
					KillItem((short)Target);
				}
			}
			else if (NotHitLaraCount < 15)
			{
				TriggerDynamic(pos2.x, pos2.y, pos2.z, 16, (GetRandomControl() & 0x3F) + 96, (GetRandomControl() & 0x1F) + 64, 0);

				if (ricochet)
				{
					TriggerRicochetSpark(&pos1, 2 * GetRandomControl(), 3, 0);
					TriggerRicochetSpark(&pos1, 2 * GetRandomControl(), 3, 0);
				}

				if (Target < 0 && GetRandomControl() & 1)
				{
					if (StaticMesh->static_number >= 50 && StaticMesh->static_number < 59)
					{
						ShatterObject(NULL, StaticMesh, 64, pos1.room_number, 0);
						StaticMesh->Flags &= ~0x1;
						TestTriggersAtXYZ(StaticMesh->x, StaticMesh->y, StaticMesh->z, pos1.room_number, 1, 0);
						SoundEffect(ShatterSounds[gfCurrentLevel - 5][StaticMesh->static_number], (PHD_3DPOS*)StaticMesh, 0);
					}

					TriggerRicochetSpark((GAME_VECTOR*)&v, 2 * GetRandomControl(), 3, 0);
					TriggerRicochetSpark((GAME_VECTOR*)&v, 2 * GetRandomControl(), 3, 0);
				}
			}

			if (NotHitLaraCount < 15)
			{
				sptr = &spark[GetFreeSpark()];
				sptr->On = 1;
				sptr->sR = (GetRandomControl() & 0x7F) + 128;
				sptr->dR = sptr->sR;
				sptr->sG = (GetRandomControl() & 0x7F) + (sptr->sR >> 1);

				if (sptr->sG > sptr->sR)
					sptr->sG = sptr->sR;

				sptr->sB = 0;
				sptr->dR = 0;
				sptr->dG = 0;
				sptr->dB = 0;
				sptr->ColFadeSpeed = 12;
				sptr->TransType = 2;
				sptr->FadeToBlack = 0;
				sptr->Life = 12;
				sptr->sLife = 12;
				sptr->x = pos2.x;
				sptr->y = pos2.y;
				sptr->z = pos2.z;
				sptr->Xvel = (short)(4 * (pos1.x - pos2.x));
				sptr->Yvel = (short)(4 * (pos1.y - pos2.y));
				sptr->Zvel = (short)(4 * (pos1.z - pos2.z));
				sptr->Friction = 0;
				sptr->MaxYvel = 0;
				sptr->Gravity = 0;
				sptr->Flags = 0;
			}
		}

		AnimateItem(item);
	}
}

void inject_tower2(bool replace)
{
	INJECT(0x00487FF0, ControlGunship, replace);
}
