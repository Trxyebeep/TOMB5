#include "../tomb5/pch.h"
#include "tomb4fx.h"
#include "control.h"
#include "sound.h"
#include "delstuff.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"

long GetFreeBlood()
{
	BLOOD_STRUCT* Blood;
	long min_life, min_life_num;

	Blood = &blood[next_blood];
	min_life = 4095;
	min_life_num = 0;

	for (int free = next_blood, i = 0; i < 32; i++)
	{
		if (Blood->On)
		{
			if (Blood->Life < min_life)
			{
				min_life_num = free;
				min_life = Blood->Life;
			}

			if (free == 31)
			{
				Blood = &blood[0];
				free = 0;
			}
			else
			{
				Blood++;
				free++;
			}
		}
		else
		{
			next_blood = (free + 1) & 31;
			return free;
		}
	}

	next_blood = (min_life_num + 1) & 31;
	return min_life_num;
}

void TriggerBlood(long x, long y, long z, long angle, long num)
{
	BLOOD_STRUCT* Blood;
	long ang, speed;
	uchar size;

	for (int i = 0; i < num; i++)
	{
		Blood = &blood[GetFreeBlood()];
		Blood->On = 1;
		Blood->sShade = 0;
		Blood->ColFadeSpeed = 4;
		Blood->FadeToBlack = 8;
		Blood->dShade = (GetRandomControl() & 0x3F) + 48;
		Blood->Life = (GetRandomControl() & 7) + 24;
		Blood->sLife = Blood->Life;
		Blood->x = (GetRandomControl() & 0x1F) + x - 16;
		Blood->y = (GetRandomControl() & 0x1F) + y - 16;
		Blood->z = (GetRandomControl() & 0x1F) + z - 16;

		if (angle == -1)
			ang = GetRandomControl();
		else
			ang = (GetRandomControl() & 0x1F) + angle - 16;

		ang &= 0xFFF;
		speed = GetRandomControl() & 0xF;
		Blood->Xvel = (short)(-(speed * rcossin_tbl[ang << 1]) >> 7);//sin
		Blood->Zvel = (short)(speed * rcossin_tbl[(ang << 1) + 1] >> 7);//cos
		Blood->Friction = 4;
		Blood->Yvel = -128 - (GetRandomControl() & 0xFF);
		Blood->RotAng = GetRandomControl() & 0xFFF;
		Blood->RotAdd = (GetRandomControl() & 0x3F) + 64;

		if (GetRandomControl() & 1)
			Blood->RotAdd = -Blood->RotAdd;

		Blood->Gravity = (GetRandomControl() & 0x1F) + 31;
		size = (uchar)((GetRandomControl() & 7) + 8);
		Blood->sSize = size;
		Blood->Size = size;
		Blood->dSize = size >> 2;
	}
}

void UpdateBlood()
{
	BLOOD_STRUCT* Blood;

	for (int i = 0; i < 32; i++)
	{
		Blood = &blood[i];

		if (Blood->On)
		{
			Blood->Life--;

			if (Blood->Life <= 0)
			{
				Blood->On = 0;
				continue;
			}

			if (Blood->sLife - Blood->Life < Blood->ColFadeSpeed)
				Blood->Shade += (((Blood->dShade - Blood->sShade) * (((Blood->sLife - Blood->Life) << 16) / Blood->ColFadeSpeed)) >> 16);
			else
			{
				if (Blood->Life < Blood->FadeToBlack)
				{
					Blood->Shade = (Blood->dShade * (((Blood->Life - Blood->FadeToBlack) << 16) / Blood->FadeToBlack + 0x10000)) >> 16;

					if (Blood->Shade < 8)
					{
						Blood->On = 0;
						continue;
					}
				}
				else
					Blood->Shade = Blood->dShade;
			}

			Blood->RotAng = Blood->RotAdd + Blood->RotAng & 0xFFF;
			Blood->Yvel += Blood->Gravity;

			if (Blood->Friction & 0xF)
			{
				Blood->Xvel -= Blood->Xvel >> (Blood->Friction & 0xF);
				Blood->Zvel -= Blood->Zvel >> (Blood->Friction & 0xF);
			}

			Blood->x += Blood->Xvel >> 5;
			Blood->y += Blood->Yvel >> 5;
			Blood->z += Blood->Zvel >> 5;
			Blood->Size = Blood->sSize + (((Blood->dSize - Blood->sSize) * (((Blood->sLife - Blood->Life) << 16) / Blood->sLife)) >> 16);
		}
	}
}

long LSpline(long x, long* knots, long nk)
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

void LaraBubbles(ITEM_INFO* item)
{
	PHD_VECTOR pos;
	int i;

	SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
	pos.x = 0;

	if (LaraDrawType == LARA_DIVESUIT)
	{
		pos.y = -192;
		pos.z = -160;
		GetLaraJointPos(&pos, 7);
	}
	else
	{
		pos.y = -4;
		pos.z = 64;
		GetLaraJointPos(&pos, 8);
	}

	i = (GetRandomControl() & 1) + 2;

	for (; i > 0; i--)
		CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 7, 0, 0, 0, 0);
}

void ControlElectricFence(short item_number)
{
	ITEM_INFO* item;
	long tx, ty, tz, xand, zand, cnt;
	//long x, lp;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags == -1)
		{
			ty = item->pos.y_pos - 128;
			tx = item->pos.x_pos + (GetRandomControl() & 0xFFF) + (GetRandomControl() & 0x1FFF);
			tz = item->pos.z_pos + (GetRandomControl() & 0x3FFF);
			xand = 255;
			zand = 255;
		}
		else
		{
			ty = 0;

			if (item->pos.y_rot <= 0)
			{
				if (item->pos.y_rot)
				{
					if (item->pos.y_rot != -32768)
					{
						if (item->pos.y_rot == -16384)
						{
							tx = item->pos.x_pos - 256;
							tz = item->pos.z_pos - 256;
							zand = 2047;
						}
						else
						{
							tx = 0;
							tz = 0;
							zand = 0;
						}

						xand = 0;
					}
					else
					{
						tx = item->pos.x_pos - 1504;
						tz = item->pos.z_pos - 256;
						xand = 2047;
						zand = 0;
					}
				}
				else
				{
					tx = item->pos.x_pos - 480;
					tz = item->pos.z_pos + 256;
					xand = 2047;
					zand = 0;
				}
			}
			else
			{
				if (item->pos.y_rot != 16384)
				{
					tx = 0;
					tz = 0;
					zand = 0;
				}
				else
				{
					tx = item->pos.x_pos + 256;
					tz = item->pos.z_pos - 1504;
					zand = 2047;
				}

				xand = 0;
			}
		}

		if (!(GetRandomControl() & 0x3F))
		{
			cnt = (GetRandomControl() & 3) + 3;

			if (item->trigger_flags != -1)
				ty = item->pos.y_pos - (GetRandomControl() & 0x7FF) - (GetRandomControl() & 0x3FF);

			for (int i = 0; i < cnt; i++)
			{
				if (item->trigger_flags == -1)
					TriggerFenceSparks(tx + (GetRandomControl() & xand) - (xand >> 1), ty, tz + (GetRandomControl() & zand) - (zand >> 1), 0, 1);
				else
				{
					if (xand)
						tx += (GetRandomControl() & xand & 7) - 4;
					else
						tz += (GetRandomControl() & zand & 7) - 4;

					ty += (GetRandomControl() & 7) - 4;

					TriggerFenceSparks(tx, ty, tz, 0, 0);
				}
			}
		}
	}
}

void DrawWeaponMissile(ITEM_INFO* item)
{
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.z_rot, item->pos.x_rot);
	phd_PutPolygons_train(meshes[objects[item->object_number].mesh_index], -1);
	phd_PopMatrix();
}

void inject_tomb4fx(bool replace)
{
	INJECT(0x00482580, GetFreeBlood, replace);
	INJECT(0x004827E0, TriggerBlood, replace);
	INJECT(0x00482610, UpdateBlood, replace);
	INJECT(0x00484D70, LSpline, replace);
	INJECT(0x00484B30, TriggerLightning, replace);
	INJECT(0x00483470, LaraBubbles, replace);
	INJECT(0x00485AD0, ControlElectricFence, replace);
	INJECT(0x004852E0, DrawWeaponMissile, replace);
}
