#include "../tomb5/pch.h"
#include "andy.h"
#include "control.h"
#include "effect2.h"
#include "items.h"
#include "sphere.h"
#include "objects.h"

void TriggerRopeFlame(PHD_VECTOR* pos, long size)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = -1;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 24;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) + pos->x - 8;
	sptr->y = pos->y;
	sptr->z = (GetRandomControl() & 0xF) + pos->z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	sptr->Flags = 538;

	if (!(GetRandomControl() & 3))
		sptr->Flags |= 32;

	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	if (GetRandomControl() & 0xF)
	{
		sptr->Yvel = -24 - (GetRandomControl() & 0xF);
		sptr->Gravity = -24 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}
	else
	{
		sptr->Yvel = (GetRandomControl() & 0xF) + 24;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 24;
	}

	sptr->Scalar = 2;
	size += (GetRandomControl() & 15);
	sptr->sSize = (uchar)size;
	sptr->Size = sptr->sSize;
	sptr->dSize = (uchar)(size >> 2);
}

void ControlBurningRoots(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* inside_item;
	PHD_VECTOR pos;
	long size, Sine, Sn, Loop, Odd;

	item = &items[item_number];

	if (item->item_flags[0] < 256)
	{
		item->item_flags[0]++;

		if (item->item_flags[0] == 96)
		{
			if (item->item_flags[1] != NO_ITEM)
			{
				inside_item = &items[item->item_flags[1]];
				AddActiveItem(item->item_flags[1]);
				inside_item->status = ITEM_ACTIVE;
				inside_item->flags |= IFL_CODEBITS;
				inside_item->pos.y_pos = item->pos.y_pos;
			}
		}

		if (GlobalCounter & 1)
			return;

		Odd = (GlobalCounter & 2) >> 1;

		if (item->item_flags[0] <= 127)
		{
			size = rcossin_tbl[item->item_flags[0] >> 3] << 9 >> 14;

			for (int i = Odd; i < 4; i += 2)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(item, &pos, i);
				TriggerRopeFlame(&pos, size);
			}

			if (item->item_flags[0] == 64 || item->item_flags[0] == 65)
			{
				item->meshswap_meshbits |= 1;
				item->mesh_bits &= 0xFFFFFFF1;
			}
		}

		Sn = 4 + (Odd * 3);
		Sine = item->item_flags[0] - 32;
		Loop = 0;

		while (Sine >= 0 && Loop < 3)
		{
			if (Sine <= 127)
			{
				size = (phd_sin(Sine << 8) << 7) >> 14;

				for (int i = Sn; i < 28; i += 6)
				{
					pos.x = 0;
					pos.y = 0;
					pos.z = 0;
					GetJointAbsPosition(item, &pos, i);
					TriggerRopeFlame(&pos, size);
				}
			}

			if (Sine == 64 || Sine == 65)
			{
				for (int i = 4 + Loop; i < 28; i += 3)
					item->meshswap_meshbits |= (1 << i);
			}

			Sine -= 32;
			Loop++;
			Sn++;
		}
	}
}

void ControlLavaEffect(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (!item->item_flags[2])
	{
		item->item_flags[0] = GetRandomControl() % ((item->trigger_flags % 10) << 10);
		item->item_flags[1] = GetRandomControl() % ((item->trigger_flags / 10) << 10);
		item->item_flags[2] = (GetRandomControl() & 0xF) + 15;
	}

	item->item_flags[2]--;

	if (item->item_flags[2] < 15)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = -1;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 16;
		sptr->dR = (GetRandomControl() & 0x3F) - 64;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 0;
		sptr->FadeToBlack = 4;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 24;
		sptr->sLife = sptr->Life;
		sptr->x = item->item_flags[1] + (GetRandomControl() & 0x3F) + item->pos.x_pos - 544;
		sptr->y = item->pos.y_pos;
		sptr->z = item->item_flags[0] + (GetRandomControl() & 0x3F) + item->pos.z_pos - 544;
		sptr->Xvel = (GetRandomControl() & 0x1FF) - 256;
		sptr->Yvel = -512 - (GetRandomControl() & 0x3FF);
		sptr->Zvel = (GetRandomControl() & 0x1FF) - 256;
		sptr->Friction = 6;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
		sptr->MaxYvel = 0;
		sptr->sSize = (GetRandomControl() & 0xF) + 32;
		sptr->Size = sptr->sSize;
		sptr->dSize = sptr->sSize >> 2;

		if (GetRandomControl() & 3)
		{
			sptr->Flags = 538;
			sptr->Scalar = 3;
			sptr->Gravity = (GetRandomControl() & 0x3F) + 32;
		}
		else
		{
			sptr->Flags = 26;
			sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
			sptr->Scalar = 1;
			sptr->Gravity = (GetRandomControl() & 0xF) + 64;
		}
	}
}

void TriggerCoinGlow(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	short shade;

	item = &items[item_number];

	if (((CamRot.vy + 1256) & 0x7FF) > 1024)
		return;

	shade = phd_sin(((CamRot.vy + 1256) & 2047) << 5);
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) - 32;
	sptr->sG = (GetRandomControl() & 0xF) - 64;
	sptr->sB = GetRandomControl() & 0x7F;
	sptr->dR = 0;
	sptr->dG = 0;
	sptr->dB = 0;
	sptr->sR = shade * sptr->sR >> 14;
	sptr->sG = shade * sptr->sG >> 14;
	sptr->sB = shade * sptr->sB >> 14;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 0;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->x = item->pos.x_pos + ((phd_sin(((CamRot.vy + 1256) & 0xFFF) << 4) << 5) >> 14);
	sptr->y = item->pos.y_pos - 16;
	sptr->z = item->pos.z_pos + ((phd_cos(((CamRot.vy + 1256) & 0xFFF) << 4) << 5) >> 14);
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = 42;
	sptr->Scalar = 4;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->dSize = 24;
	sptr->sSize = 24;
	sptr->Size = 24;
}

void inject_andy(bool replace)
{
	INJECT(0x00406C10, TriggerRopeFlame, replace);
	INJECT(0x00406E10, ControlBurningRoots, replace);
	INJECT(0x004070D0, ControlLavaEffect, replace);
	INJECT(0x00407350, TriggerCoinGlow, replace);
}