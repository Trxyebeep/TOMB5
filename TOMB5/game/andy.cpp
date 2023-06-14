#include "../tomb5/pch.h"
#include "andy.h"
#include "control.h"
#include "effect2.h"
#include "items.h"
#include "sphere.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"

void TriggerRopeFlame(PHD_VECTOR* pos, long size)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
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
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;

	if (!(GetRandomControl() & 3))
		sptr->Flags |= SF_NOKILL;

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
	sptr->Size = (uchar)size;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;
}

void ControlBurningRoots(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* heart;
	PHD_VECTOR pos;
	long size, s, o, odd, lp;

	item = &items[item_number];

	if (item->item_flags[0] < 256)
	{
		item->item_flags[0]++;

		if (item->item_flags[0] == 96 && item->item_flags[1] != NO_ITEM)
		{
			AddActiveItem(item->item_flags[1]);
			heart = &items[item->item_flags[1]];
			heart->status = ITEM_ACTIVE;
			heart->flags |= IFL_CODEBITS;
			heart->pos.y_pos = item->pos.y_pos;
		}

		if (GlobalCounter & 1)
			return;

		odd = (GlobalCounter & 2) >> 1;

		if (item->item_flags[0] <= 127)
		{
			size = 512 * rcossin_tbl[item->item_flags[0] >> 3] >> W2V_SHIFT;

			for (int i = odd; i < 4; i += 2)
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
				item->mesh_bits &= ~0xE;
			}
		}

		s = item->item_flags[0] - 32;
		o = (odd * 3) + 4;
		lp = 0;

		while (s >= 0 && lp < 3)
		{
			if (s <= 127)
			{
				size = 128 * phd_sin(s << 8) >> W2V_SHIFT;

				for (int i = o; i < 28; i += 6)
				{
					pos.x = 0;
					pos.y = 0;
					pos.z = 0;
					GetJointAbsPosition(item, &pos, i);
					TriggerRopeFlame(&pos, size);
				}
			}

			if (s == 64 || s == 65)
			{
				for (int i = lp + 4; i < 28; i += 3)
					item->meshswap_meshbits |= (1 << i);
			}

			s -= 32;
			o++;
			lp++;
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

	if (item->item_flags[2] >= 15)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 16;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
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
	sptr->Size = (GetRandomControl() & 0xF) + 32;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;

	if (GetRandomControl() & 3)
	{
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->Scalar = 3;
		sptr->Gravity = (GetRandomControl() & 0x3F) + 32;
	}
	else
	{
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
		sptr->Scalar = 1;
		sptr->Gravity = (GetRandomControl() & 0xF) + 64;
	}
}

void TriggerCoinGlow(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	short shade, ang;

	item = &items[item_number];
	ang = CamRot.y + 1256;

	if ((ang & 0x7FF) > 1024)
		return;

	shade = phd_sin((ang & 0x7FF) << 5);
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 224;
	sptr->sG = (GetRandomControl() & 0xF) + 192;
	sptr->sB = GetRandomControl() & 0x7F;
	sptr->dR = 0;
	sptr->dG = 0;
	sptr->dB = 0;
	sptr->sR = shade * sptr->sR >> W2V_SHIFT;
	sptr->sG = shade * sptr->sG >> W2V_SHIFT;
	sptr->sB = shade * sptr->sB >> W2V_SHIFT;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 0;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->x = item->pos.x_pos + (32 * phd_sin((ang & 0xFFF) << 4) >> W2V_SHIFT);
	sptr->y = item->pos.y_pos - 16;
	sptr->z = item->pos.z_pos + (32 * phd_cos((ang & 0xFFF) << 4) >> W2V_SHIFT);
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = SF_NOKILL | SF_DEF | SF_SCALE;
	sptr->Scalar = 4;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Size = 24;
	sptr->sSize = 24;
	sptr->dSize = 24;
}
