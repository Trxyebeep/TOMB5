#include "../tomb5/pch.h"
#include "effect2.h"
#include "control.h"
#include "tomb4fx.h"

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

void inject_effect2(bool replace)
{
	INJECT(0x0042F460, TriggerFlareSparks, replace);
}
