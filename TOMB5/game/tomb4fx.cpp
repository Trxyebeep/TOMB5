#include "../tomb5/pch.h"
#include "tomb4fx.h"
#include "control.h"

short DoBloodSplat(long x, long y, long z, short random, short y_rot, short room_number)
{
	GetFloor(x, y, z, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterBlood(x, y, z, random);
	else
		TriggerBlood(x, y, z, y_rot >> 4, random);

	return -1;
}

int GetFreeBlood()
{
	int count = 0;
	int min_life = 0;
	int min_life_num = 0;

	BLOOD_STRUCT* Blood = &blood[next_blood];
	int blood_num = next_blood;

	while (Blood->On)
	{
		if (Blood->Life < min_life)
		{
			min_life_num = blood_num;
			min_life = Blood->Life;
		}

		if (blood_num == 31)
		{
			Blood = &blood[0];
			blood_num = 0;
		}
		else
		{
			Blood++;
			blood_num++;
		}

		if (++count >= 32)
		{
			next_blood = (min_life_num + 1) & 31;
			return min_life_num;
		}
	}
	next_blood = (blood_num + 1) & 31;
	return blood_num;
}

void TriggerBlood(int x, int y, int z, int a4, int num)
{
	int i;
	struct BLOOD_STRUCT* bptr;
	int a;
	int b;
	int size;

	for (i = 0; i < num; i++)
	{
		bptr = &blood[GetFreeBlood()];
		bptr->On = 1;
		bptr->sShade = 0;
		bptr->ColFadeSpeed = 4;
		bptr->FadeToBlack = 8;
		bptr->dShade = (GetRandomControl() & 0x3F) + 48;
		bptr->Life = bptr->sLife = (GetRandomControl() & 7) + 24;
		bptr->x = (GetRandomControl() & 0x1F) + x - 16;
		bptr->y = (GetRandomControl() & 0x1F) + y - 16;
		bptr->z = (GetRandomControl() & 0x1F) + z - 16;
		a = (a4 == -1
			? GetRandomControl() & 0xFFFF
			: (GetRandomControl() & 0x1F) + a4 - 16) & 0xFFF;
		b = GetRandomControl() & 0xF;
		bptr->Zvel = b * rcossin_tbl[2 * a + 1] >> 7;
		bptr->Xvel = -(b * rcossin_tbl[2 * a]) >> 7;
		bptr->Friction = 4;
		bptr->Yvel = -((GetRandomControl() & 0xFF) + 128);
		bptr->RotAng = GetRandomControl() & 0xFFF;
		bptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
		if (GetRandomControl() & 1)
			bptr->RotAdd = -bptr->RotAdd;
		bptr->Gravity = (GetRandomControl() & 0x1F) + 31;
		size = (GetRandomControl() & 7) + 8;
		bptr->sSize = bptr->Size = size;
		bptr->dSize = size >> 2;
	}
}

void UpdateBlood()
{
	for (int i = 0; i < 32; i++)
	{
		BLOOD_STRUCT* Blood = &blood[i];

		if (Blood->On)
		{
			Blood->Life -= 1;

			if (Blood->Life < 0)
			{
				Blood->On = 0;
				continue;
			}
			else
			{
				if (Blood->sLife - Blood->Life < Blood->ColFadeSpeed)
					Blood->Shade = (((Blood->sLife - Blood->Life) << 16) / Blood->ColFadeSpeed * (Blood->dShade - Blood->sShade) >> 16) + Blood->sShade;
				else
				{
					if (Blood->Life < Blood->FadeToBlack)
					{
						Blood->Shade = (((Blood->Life - Blood->FadeToBlack) << 16) / Blood->FadeToBlack + 0x10000) * Blood->dShade >> 16;

						if (Blood->Shade < 8)
						{
							Blood->On = 0;
							continue;
						}

					}
					else
						Blood->Shade = Blood->dShade;

				}
			}


			Blood->RotAng = Blood->RotAdd + Blood->RotAng & 0xFFF;
			Blood->Yvel += Blood->Gravity;

			if (Blood->Friction & 0xF)
			{
				Blood->Xvel = Blood->Xvel - ((Blood->Xvel) >> (Blood->Friction & 0xF));
				Blood->Zvel = Blood->Zvel - ((Blood->Zvel) >> (Blood->Friction & 0xF));
			}


			Blood->x += (Blood->Xvel >> 5);
			Blood->y += (Blood->Yvel >> 5);
			Blood->z += (Blood->Zvel >> 5);

			Blood->Size = (((Blood->dSize - Blood->sSize) * (((Blood->sLife - Blood->Life) << 16) / Blood->sLife)) >> 16) + Blood->sSize;

		}
	}
}

long LSpline(long x, long* knots, int nk)
{
	int span;
	long* k;
	long c1, c2, c3, ret;

	x *= nk - 3;
	span = x >> 16;

	if (span >= nk - 3)
		span = nk - 4;

	x -= 65536 * span;
	k = &knots[3 * span];
	c1 = k[3] + (k[3] >> 1) - (k[6] >> 1) - k[6] + (k[9] >> 1) + ((-k[0] - 1) >> 1);
	ret = (long long) c1 * x >> 16;
	c2 = ret + 2 * k[6] - 2 * k[3] - (k[3] >> 1) - (k[9] >> 1) + k[0];
	ret = (long long) c2 * x >> 16;
	c3 = ret + (k[6] >> 1) + ((-k[0] - 1) >> 1);
	ret = (long long) c3 * x >> 16;
	return ret + k[3];
}

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments)
{
	LIGHTNING_STRUCT* lptr;
	char* vptr;

	for (int i = 0; i < 16; i++)
	{
		lptr = &Lightning[i];

		if (!lptr->Life)
		{
			lptr->Point[0].x = s->x;
			lptr->Point[0].y = s->y;
			lptr->Point[0].z = s->z;
			lptr->Point[1].x = ((s->x * 3) + d->x) >> 2;
			lptr->Point[1].y = ((s->y * 3) + d->y) >> 2;
			lptr->Point[1].z = ((s->z * 3) + d->z) >> 2;
			lptr->Point[2].x = ((d->x * 3) + s->x) >> 2;
			lptr->Point[2].y = ((d->y * 3) + s->y) >> 2;
			lptr->Point[2].z = ((d->z * 3) + s->z) >> 2;
			lptr->Point[3].x = d->x;
			lptr->Point[3].y = d->y;
			lptr->Point[3].z = d->z;
			vptr = &lptr->Xvel1;

			for (int j = 0; j < 6; j++)
				*vptr++ = (GetRandomControl() % variation) - (variation >> 1);

			for (int j = 0; j < 3; j++)
			{
				if (flags & 2)
					*vptr++ = (GetRandomControl() % variation) - (variation >> 1);
				else
					*vptr++ = 0;
			}

			lptr->Flags = flags;
			*(long*)&lptr->r = rgb;
			lptr->Segments = segments;
			lptr->Rand = variation;
			lptr->Size = size;
			return lptr;
		}
	}

	return 0;
}

void inject_tomb4fx(bool replace)
{
	INJECT(0x00432760, DoBloodSplat, replace);
	INJECT(0x00482580, GetFreeBlood, replace);
	INJECT(0x004827E0, TriggerBlood, replace);
	INJECT(0x00482610, UpdateBlood, replace);
	INJECT(0x00484D70, LSpline, replace);
	INJECT(0x00484B30, TriggerLightning, replace);
}
