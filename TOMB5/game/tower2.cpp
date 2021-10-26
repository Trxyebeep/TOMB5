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
#include "../specific/function_stubs.h"
#include "traps.h"
#include "collide.h"

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
						SoundEffect(ShatterSounds[gfCurrentLevel][StaticMesh->static_number - 50], (PHD_3DPOS*)StaticMesh, 0);
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

void DrawSteelDoorLensFlare(ITEM_INFO* item)
{

}

void ControlIris(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos, pos2;
	long r, g, b, rot;

	item = &items[item_number];

	if (item->timer)
	{
		item->timer--;

		if (!item->timer)
			item->flags |= IFL_CODEBITS;
	}
	else if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
	{
		SoundEffect(SFX_RICH_IRIS_ELEC, &item->pos, SFX_DEFAULT);

		if (!lara.burn)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, 0);

			if (ABS(pos.y - item->pos.y_pos) >= 1024 || ABS(pos.x - item->pos.x_pos) >= 2048 || ABS(pos.z - item->pos.z_pos) >= 2048)
				item->item_flags[3] = 0;
			else
			{
				if (!item->item_flags[3])
					SoundEffect(SFX_LARA_INJURY_NONRND, &lara_item->pos, SFX_DEFAULT);

				if (lara_item->hit_points <= 72 || item->item_flags[3] >= 45)
				{
					LaraBurn();
					lara_item->hit_points = 0;
					lara.BurnCount = 24;
				}
				else
				{
					lara_item->hit_points -= 12;
					item->item_flags[3]++;
				}
			}
		}

		if (!(GlobalCounter & 1))
		{
			r = (GetRandomControl() & 0x3F) + 128;
			g = (GetRandomControl() & 0x7F) + 64;
			b = GetRandomControl() & 0x1F;
			pos.x = item->pos.x_pos;
			pos.y = item->pos.y_pos - 128;
			pos.z = item->pos.z_pos;

			if ((GlobalCounter & 3) == 2)
			{
				rot = (GetRandomControl() & 0xFF) + ((GlobalCounter & 0x7F) << 9) - 128;

				if (GlobalCounter & 4)
					rot += 32768;

				pos2.x = pos.x - ((2240 * phd_sin(rot)) >> 14);
				pos2.y = pos.y;
				pos2.z = pos.z - ((2240 * phd_cos(rot)) >> 14);
				TriggerLightning(&pos, &pos2, (GetRandomControl() & 0x3F) + 64, RGBA(r, g, b, 50), 21, 48, 5);
				TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(r >> 1, g >> 1, b >> 1, 32));
			}
			else
			{
				pos2.x = pos.x + ((GetRandomControl() & 0x1FF) << 1) - 512;
				pos2.y = pos.y + ((GetRandomControl() & 0x1FF) << 1) - 512;
				pos2.z = pos.z + ((GetRandomControl() & 0x1FF) << 1) - 512;
				TriggerLightning(&pos, &pos2, (GetRandomControl() & 0xF) + 16, RGBA(r, g >> 2, b >> 2, 24), 7, 48, 3);
			}

			TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(r >> 1, g >> 1, b >> 1, 96));
		}
	}
}

void ControlArea51Laser(short item_number)
{
	ITEM_INFO* item;
	long x, z, dx, dz;
	short num, room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->current_anim_state = 0;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos,
		(GetRandomControl() & 1) + 8, (GetRandomControl() & 3) + 24, GetRandomControl() & 3, GetRandomControl() & 1);
	item->mesh_bits = -1 - (GetRandomControl() & 0x14);
	dx = ABS(((item->item_flags[1] & 0xFF) << 9) - item->pos.x_pos);

	if (dx < 768)
	{
		dz = ABS(((item->item_flags[1] & 0xFF00) << 1) - item->pos.z_pos);

		if (dz < 768)
		{
			item->trigger_flags = 32;
			x = ((((item->item_flags[1] & 0xFF) << 9) + ((-2560 * (item->item_flags[2] * phd_sin(item->pos.y_rot))) >> 14)) >> 9) & 0xFF;
			z = (((((-2560 * (item->item_flags[2] * phd_cos(item->pos.y_rot))) >> 14) + ((item->item_flags[0] & 0xFF00) << 1)) >> 9) & 0xFF) << 8;
			item->item_flags[1] = (short)(x | z);
		}
	}

	if (item->item_flags[2] == 1)
	{
		if (item->trigger_flags)
		{
			if (item->item_flags[3])
			{
				if (item->item_flags[3] > 4)
					item->item_flags[3] -= item->item_flags[3] >> 2;
				else
					item->item_flags[3] = 0;
			}
			else
			{
				item->trigger_flags--;

				if (item->trigger_flags == 1)
					item->item_flags[2] = -1;
			}
		}
		else
		{
			item->item_flags[3] += 5;

			if (item->item_flags[3] > 512)
				item->item_flags[3] = 512;
		}
	}
	else
	{
		if (item->trigger_flags)
		{
			if (item->item_flags[3])
			{
				if (item->item_flags[3] < -4)
					item->item_flags[3] -= item->item_flags[3] >> 2;
				else
					item->item_flags[3] = 0;
			}
			else
			{
				item->trigger_flags--;

				if (item->trigger_flags == 1)
					item->item_flags[2] = -item->item_flags[2];
			}
		}
		else
		{
			item->item_flags[3] -= 5;

			if (item->item_flags[3] < -512)
				item->item_flags[3] = -512;
		}
	}

	if (item->item_flags[3])
	{
		num = ABS(item->item_flags[3] >> 4);

		if (num > 31)
			num = 31;

		SoundEffect(SFX_ZOOM_VIEW_WHIRR, &item->pos, 0x800000 | SFX_ALWAYS | SFX_SETPITCH | SFX_SETVOL | (num << 8));
	}

	item->pos.z_pos += (item->item_flags[3] * phd_cos(item->pos.y_rot)) >> 16;
	item->pos.x_pos += (item->item_flags[3] * phd_sin(item->pos.y_rot)) >> 16;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	if (TestBoundsCollide(item, lara_item, 64))
	{
		if (!item->draw_room)
		{
			SoundEffect(SFX_LARA_INJURY_NONRND, &lara_item->pos, SFX_DEFAULT);
			item->draw_room = 1;
		}

		lara_item->hit_points -= 100;
		DoBloodSplat(lara_item->pos.x_pos, item->pos.y_pos - GetRandomControl() - 32, lara_item->pos.z_pos,
			(GetRandomControl() & 3) + 4, GetRandomControl() << 1, lara_item->room_number);
		AnimateItem(item);
	}
	else
	{
		item->draw_room = 0;
		AnimateItem(item);
	}
}

void inject_tower2(bool replace)
{
	INJECT(0x00487FF0, ControlGunship, replace);
	INJECT(0x00487AB0, DrawSteelDoorLensFlare, replace);
	INJECT(0x00486050, ControlIris, replace);
	INJECT(0x00486450, ControlArea51Laser, replace);
}
