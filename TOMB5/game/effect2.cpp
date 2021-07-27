#include "../tomb5/pch.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "../specific/function_stubs.h"

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv)
{
	SPARKS* sptr;
	long dx, dz, rand;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx >= -0x4000 && dx <= 0x4000 && dz >= -0x4000 && dz <= 0x4000)
	{
		rand = GetRandomDraw();
		sptr = &spark[GetFreeSpark()];
		sptr->sR = -1;
		sptr->sG = -1;
		sptr->sB = -1;
		sptr->dR = -1;
		sptr->dG = (rand & 127) + 64;
		sptr->dB = 192 - sptr->dG;
		sptr->On = 1;
		sptr->ColFadeSpeed = 3;
		sptr->FadeToBlack = 5;
		sptr->Life = 10;
		sptr->sLife = 10;
		sptr->TransType = 2;
		sptr->Friction = 34;
		sptr->Scalar = 1;
		sptr->x = (rand & 7) + x - 3;
		sptr->y = ((rand >> 3) & 7) + y - 3;
		sptr->z = ((rand >> 6) & 7) + z - 3;
		sptr->Xvel = (short)(((rand >> 2) & 0xFF) + xv - 128);
		sptr->Yvel = (short)(((rand >> 4) & 0xFF) + yv - 128);
		sptr->Zvel = (short)(((rand >> 6) & 0xFF) + zv - 128);
		sptr->Flags = 2;
		sptr->Size = ((rand >> 9) & 3) + 4;
		sptr->sSize = ((rand >> 9) & 3) + 4;
		sptr->dSize = ((rand >> 12) & 1) + 1;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
}

void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit)
{
	SPARKS* sptr;
	long dx, dz, rand;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 16;
	sptr->sG = 8;
	sptr->sB = 4;
	sptr->dR = 64;
	sptr->dG = 48;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (hit)
	{
		sptr->Xvel = (short)((GetRandomControl() & 0xFF) - xv - 128);
		sptr->Yvel = (short)(-4 - (GetRandomControl() & 3));
		sptr->Zvel = (short)((GetRandomControl() & 0xFF) - zv - 128);
	}
	else
	{
		if (xv)
			sptr->Xvel = (short)-xv;
		else
			sptr->Xvel = (short)((GetRandomControl() & 0xFF) - 128);

		sptr->Yvel = (short)(-4 - (GetRandomControl() & 3));

		if (zv)
			sptr->Zvel = (short)-zv;
		else
			sptr->Zvel = (short)((GetRandomControl() & 0xFF) - 128);
	}

	sptr->Friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = (short)(GetRandomControl() & 0xFFF);

		if (GetRandomControl() & 1)
			sptr->RotAdd = (char)(-16 - (GetRandomControl() & 0xF));
		else
			sptr->RotAdd = (char)((GetRandomControl() & 0xF) + 16);
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 1;
	rand = (GetRandomControl() & 0x3F) + 72;

	if (hit)
	{
		sptr->MaxYvel = 0;
		sptr->sSize = (uchar)(rand >> 3);
		sptr->Size = sptr->sSize;
		sptr->Gravity = 0;
		sptr->dSize = (uchar)(rand >> 1);
	}
	else
	{
		sptr->sSize = (uchar)(rand >> 4);
		sptr->Size = sptr->sSize;
		sptr->Gravity = (short)(-4 - (GetRandomControl() & 3));
		sptr->dSize = (uchar)rand;
		sptr->MaxYvel = (char)(-4 - (GetRandomControl() & 3));
	}
}

void inject_effect2(bool replace)
{
	INJECT(0x0042F460, TriggerFlareSparks, replace);
	INJECT(0x00430D90, TriggerDartSmoke, replace);
}
