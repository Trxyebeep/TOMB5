#include "../tomb5/pch.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "objects.h"
#include "sound.h"
#include "gameflow.h"
#include "../specific/3dmath.h"
#include "missile.h"
#include "items.h"
#include "effects.h"
#include "traps.h"
#include "hydra.h"
#include "romangod.h"

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

void KillAllCurrentItems(short item_number)
{
	KillEverythingFlag = 1;
}

void KillEverything()
{
	KillEverythingFlag = 0;
}

void ControlSmokeEmitter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	PHD_3DPOS pos;
	long size, dx, dz, normal;

	item = &items[item_number];
	normal = 0;

	if (!TriggerActive(item))
		return;

	if (item->object_number == STEAM_EMITTER && room[item->room_number].flags & ROOM_UNDERWATER)
	{
		if (item->item_flags[0] || !(GetRandomControl() & 0x1F) || item->trigger_flags == 1)
		{
			if (!(GetRandomControl() & 3) || item->item_flags[1])
			{
				pos.x_pos = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
				pos.y_pos = item->pos.y_pos - (GetRandomControl() & 0x1F) - 16;
				pos.z_pos = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
				
				if (item->trigger_flags == 1)
					CreateBubble(&pos, item->room_number, 15, 15, 0, 0, 0, 0);
				else
					CreateBubble(&pos, item->room_number, 8, 7, 0, 0, 0, 0);

				if (item->item_flags[0])
				{
					item->item_flags[0]--;

					if (!item->item_flags[0])
						item->item_flags[1] = 0;
				}
			}
		}
		else if (!(GetRandomControl() & 0x1F))
			item->item_flags[0] = (GetRandomControl() & 3) + 4;

		return;
	}

	if (item->object_number == STEAM_EMITTER && item->trigger_flags & 8 && item->trigger_flags != 111)
	{
		if (gfCurrentLevel == LVL5_RED_ALERT && item->trigger_flags > 0)
			SoundEffect(SFX_HISS_LOOP_BIG, &item->pos, SFX_DEFAULT);

		if (item->trigger_flags & 4)
		{
			if (GlobalCounter & 1)
				return;
		}
		else
		{
			if (item->item_flags[0])
			{
				item->item_flags[0]--;

				if (!item->item_flags[0])
					item->item_flags[1] = (GetRandomControl() & 0x3F) + 30;

				normal = 1;

				if (item->item_flags[2])
					item->item_flags[2] -= 256;
			}
			else if (item->item_flags[2] < 4096)
				item->item_flags[2] += 256;
		}

		if (item->item_flags[2])
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;

			if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
				return;

			sptr = &spark[GetFreeSpark()];
			sptr->On = 1;
			sptr->dR = 48;
			sptr->dG = 48;
			sptr->dB = 48;
			sptr->TransType = 2;
			sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
			sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
			sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;

			if (item->item_flags[2] == 4096)
			{
				if (item->trigger_flags & 4)
					size = (GetRandomControl() & 0xFFF) + 256;
				else
					size = (GetRandomControl() & 0x7FF) + 2048;
			}

			if (item->trigger_flags >= 0)
			{
				sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot - 32768)) >> 14);
				sptr->Yvel = -16 - (GetRandomControl() & 0xF);
				sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot - 32768)) >> 14);
			}
			else
			{
				size = (GetRandomControl() & 0x7F) + 2048;
				sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot + 20480)) >> 14);
				sptr->Yvel = -128 - (GetRandomControl() & 0xFF);
				sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot + 20480)) >> 14);
			}

			sptr->Flags = 538;

			if (!(GlobalCounter & 3) && !(item->trigger_flags & 4))
				sptr->Flags = 1562;

			sptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				sptr->RotAdd = -8 - (GetRandomControl() & 7);
			else
				sptr->RotAdd = (GetRandomControl() & 7) + 8;

			sptr->Gravity = -8 - (GetRandomControl() & 0xF);
			sptr->MaxYvel = -8 - (GetRandomControl() & 7);
			dz = (GetRandomControl() & 0x1F) + 128;

			if (item->trigger_flags & 4)
			{
				sptr->sB = 0;
				sptr->sG = 0;
				sptr->sR = 0;
				sptr->ColFadeSpeed = 2;
				sptr->FadeToBlack = 2;
				sptr->Life = (GetRandomControl() & 3) + 16;
				sptr->sLife = sptr->Life;

				if (item->trigger_flags >= 0)
				{
					sptr->Xvel <<= 2;
					sptr->Zvel <<= 2;
					sptr->Scalar = 3;
					sptr->Friction = 4;
				}
				else
				{
					sptr->Scalar = 1;
					sptr->Friction = 51;
					dz >>= 1;
				}

				dx = (size * dz) >> 10;

				if (dx > 255)
					dx = 255;

				sptr->dSize = (uchar)dx;
				sptr->sSize = (uchar)(dx >> 2);
				sptr->Size = (uchar)(dx >> 2);
			}
			else
			{
				sptr->sR = 96;
				sptr->sG = 96;
				sptr->sB = 96;
				sptr->FadeToBlack = 6;
				sptr->ColFadeSpeed = (GetRandomControl() & 3) + 6;
				sptr->Life = (GetRandomControl() & 7) + 8;
				sptr->sLife = sptr->Life;
				sptr->Friction = 4 - (item->trigger_flags & 4);
				sptr->dSize = (uchar)dz;
				sptr->Scalar = (item->trigger_flags & 4) + 2;
				sptr->sSize = (uchar)(dz >> 1);
				sptr->Size = (uchar)(dz >> 1);

				if (item->item_flags[1])
					item->item_flags[1]--;
				else
					item->item_flags[0] = item->trigger_flags >> 4;
			}
		}

		if (!normal)
			return;
	}

	if (!(wibble & 0xF) && (item->object_number != STEAM_EMITTER || wibble & 0x1F))
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 7) + 28;

#ifdef GENERAL_FIXES
		if (item->object_number == SMOKE_EMITTER_BLACK)
		{
			sptr->dR = 96;
			sptr->dG = 96;
			sptr->dB = 96;
		}
#endif

		sptr->sLife = sptr->Life;

		if (item->object_number == SMOKE_EMITTER_BLACK)
			sptr->TransType = 3;
		else
			sptr->TransType = 2;

		sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
		sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
		sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;

		if (item->trigger_flags == 111)
		{
			sptr->Xvel = (phd_sin(item->pos.y_rot - 32768)) >> 5;
			sptr->Zvel = (phd_cos(item->pos.y_rot - 32768)) >> 5;
			sptr->Friction = 5;
		}
		else
		{
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Friction = 3;
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		}

		sptr->Flags = 538;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);

		if (room[item->room_number].flags & ROOM_OUTSIDE)
			sptr->Flags = 794;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -8 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 8;

		sptr->Scalar = 2;
		sptr->Gravity = -8 - (GetRandomControl() & 0xF);
		sptr->MaxYvel = -8 - (GetRandomControl() & 7);
		size = (GetRandomControl() & 0x1F) + 128;
		sptr->dSize = (uchar)size;
		sptr->sSize = (uchar)(size >> 2);
		sptr->Size = (uchar)(size >> 2);

		if (item->object_number == STEAM_EMITTER)
		{
			sptr->Gravity >>= 1;
			sptr->Yvel >>= 1;
			sptr->MaxYvel >>= 1;
			sptr->Life += 16;
			sptr->sLife += 16;
			sptr->dR = 32;
			sptr->dG = 32;
			sptr->dB = 32;
		}
	}
}

void ControlEnemyMissile(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long speed, ox, oy, oz, h, c;
	short angles[2];
	short room_number, max_turn;

	fx = &effects[fx_number];

	if (fx->flag1 == 2)
	{
		fx->pos.z_rot += fx->speed << 4;

		if (fx->speed > 64)
			fx->speed -= 4;

		if (fx->pos.x_rot > -12288)
		{
			if (fx->fallspeed < 512)
				fx->fallspeed += 36;

			fx->pos.x_rot -= fx->fallspeed;
		}
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - fx->pos.x_pos, lara_item->pos.y_pos - fx->pos.y_pos - 256,
			lara_item->pos.z_pos - fx->pos.z_pos, &angles[0]);

		if (!fx->flag1)
		{
			if (fx->counter)
				fx->counter--;

			max_turn = 256;
		}
		else if (fx->flag1 == 1)
			max_turn = 384;
		else
			max_turn = 768;

		if (fx->speed < 192)
		{
			if (!fx->flag1 || fx->flag1 == 1)
				fx->speed++;

			oy = (ushort)angles[0] - (ushort)fx->pos.y_rot;

			if (abs(oy) > 0x8000)
				oy = (ushort)fx->pos.y_rot - (ushort)angles[0];

			ox = (ushort)angles[1] - (ushort)fx->pos.x_rot;

			if (abs(ox) > 0x8000)
				ox = (ushort)fx->pos.x_rot - (ushort)angles[1];

			oy >>= 3;
			ox >>= 3;

			if (oy > max_turn)
				oy = max_turn;
			else if (oy < -max_turn)
				oy = -max_turn;

			if (ox > max_turn)
				ox = max_turn;
			else if (ox < -max_turn)
				ox = -max_turn;

			fx->pos.y_rot += (short)oy;
			fx->pos.x_rot += (short)ox;
		}

		fx->pos.z_rot += fx->speed << 4;

		if (!fx->flag1)
			fx->pos.z_rot += fx->speed << 4;
	}

	ox = fx->pos.x_pos;
	oy = fx->pos.y_pos;
	oz = fx->pos.z_pos;
	speed = fx->speed * phd_cos(fx->pos.x_rot) >> 14;
	fx->pos.x_pos += speed * phd_sin(fx->pos.y_rot) >> 14;
	fx->pos.y_pos += fx->speed * phd_sin(-fx->pos.x_rot) >> 14;
	fx->pos.z_pos += speed * phd_cos(fx->pos.y_rot) >> 14;
	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos <= c)
	{
		fx->pos.x_pos = ox;
		fx->pos.y_pos = oy;
		fx->pos.z_pos = oz;

		if (!fx->flag1)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x18806000, 0x20000);
		}
		else if (fx->flag1 == 1)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
			fx->pos.y_pos -= 64;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0x1000030, 64, 0x18008040, 0x10000);
			fx->pos.y_pos -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x18008040, 0x10000);
		}
		else if (fx->flag1 == 2)
		{
			ExplodeFX(fx, 0, 32);
			SoundEffect(SFX_IMP_STONE_HIT, &fx->pos, SFX_DEFAULT);
		}

		KillEffect(fx_number);
		return;
	}

	if (ItemNearLara(&fx->pos, 200))
	{
		lara_item->hit_status = 1;

		if (!fx->flag1)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x18806000, 0);

			if (lara_item->hit_points < 500)
				LaraBurn();
			else
				lara_item->hit_points -= 300;
		}
		else if (fx->flag1 == 1)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
			fx->pos.y_pos -= 64;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0x1000030, 64, 0x18008040, 0x10000);
			fx->pos.y_pos -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x18008040, 0x10000);
			lara_item->hit_points -= 200;
		}
		else if (fx->flag1 == 2)
		{
			ExplodeFX(fx, 0, 32);
			lara_item->hit_points -= 50;
			DoBloodSplat(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, (GetRandomControl() & 3) + 2, lara_item->pos.y_rot, lara_item->room_number);
			SoundEffect(SFX_IMP_STONE_HIT, &fx->pos, SFX_DEFAULT);
			SoundEffect(SFX_LARA_INJURY_RND, &lara_item->pos, SFX_DEFAULT);
		}

		KillEffect(fx_number);
		return;
	}

	if (room_number != fx->room_number)
		EffectNewRoom(fx_number, room_number);

	if (GlobalCounter & 1)
	{
		pos.x = ox;
		pos.y = oy;
		pos.z = oz;
		ox -= fx->pos.x_pos;
		oy -= fx->pos.y_pos;
		oz -= fx->pos.z_pos;

		if (!fx->flag1)
		{
			TriggerHydraMissileFlame(&pos, ox << 2, oy << 2, oz << 2);
			TriggerHydraMissileFlame((PHD_VECTOR*)&fx->pos, ox << 2, oy << 2, oz << 2);
		}
		else if (fx->flag1 == 1)
			TriggerRomanGodMissileFlame(&pos, fx_number);
	}
}

void TriggerExplosionSmokeEnd(long x, long y, long z, long uw)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (uw)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	}
	else
	{
#ifdef GENERAL_FIXES
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 96;
		sptr->dG = 96;
		sptr->dB = 96;
#else
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
#endif
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if (uw)
		sptr->TransType = 2;
	else
		sptr->TransType = 3;

	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
	{
		sptr->Friction = 20;
		sptr->Yvel >>= 4;
		sptr->y += 32;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Scalar = 3;

	if (uw)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize;
}

void ClearDynamics()
{
	number_dynamics = 0;

	for (int i = 0; i < 32; i++)
		dynamics[i].on = 0;
}

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;
	long dist, max, best, dx, dy, dz;

	if (abs(camera.pos.x - x) > 0x5000 || abs(camera.pos.y - y) > 0x5000 || abs(camera.pos.z - z) > 0x5000 || x < 0 || z < 0 || !falloff)
		return;

	max = 0;
	best = 0;

	if (number_dynamics == 32)
	{
		for (int i = 0; i < 32; i++)
		{
			dl = &dynamics[i];
			dx = camera.pos.x - dl->x;
			dy = camera.pos.y - dl->y;
			dz = camera.pos.z - dl->z;
			dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

			if (dist > max)
			{
				max = dist;
				best = i;
			}
		}

		dl = &dynamics[best];
		number_dynamics--;
	}
	else
		dl = &dynamics[number_dynamics];

	dl->on = 1;
	dl->x = x;
	dl->y = y;
	dl->z = z;
	dl->falloff = ushort(falloff << 8);

	if (falloff < 8)
	{
		dl->r = uchar((r * falloff) >> 3);
		dl->g = uchar((g * falloff) >> 3);
		dl->b = uchar((b * falloff) >> 3);
	}
	else
	{
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
	}

	dl->FalloffScale = 0x200000 / (falloff << 8);
	number_dynamics++;
}

void TriggerDynamic_MIRROR(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	for (int i = 0; i < 2; i++)
	{
		if (number_dynamics == 32 || !falloff)
			break;

		dl = &dynamics[number_dynamics];
		dl->on = 1;
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);

		if (falloff < 8)
		{
			dl->r = uchar((r * falloff) >> 3);
			dl->g = uchar((g * falloff) >> 3);
			dl->b = uchar((b * falloff) >> 3);
		}
		else
		{
			dl->r = (uchar)r;
			dl->g = (uchar)g;
			dl->b = (uchar)b;
		}

		dl->FalloffScale = 0x200000 / (falloff << 8);
		number_dynamics++;
		z = 2 * gfMirrorZPlane - z;
	}
}

void TriggerExplosionBubble(long x, long y, long z, short room_number)
{
	SPARKS* sptr;
	PHD_3DPOS pos;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 128;
	sptr->sG = 64;
	sptr->sB = 0;
	sptr->dR = 128;
	sptr->dG = 128;
	sptr->dB = 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 12;
	sptr->Life = 24;
	sptr->sLife = 24;
	sptr->TransType = 2;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 0;
	sptr->Flags = 2058;
	sptr->Scalar = 3;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 13;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	size = (GetRandomControl() & 7) + 63;
	sptr->Size = size >> 1;
	sptr->sSize = size >> 1;
	sptr->dSize = size << 1;

	for (int i = 0; i < 7; i++)
	{
		pos.x_pos = (GetRandomControl() & 0x1FF) + x - 256;
		pos.y_pos = (GetRandomControl() & 0x7F) + y - 64;
		pos.z_pos = (GetRandomControl() & 0x1FF) + z - 256;
		CreateBubble(&pos, room_number, 6, 15, 0, 0, 0, 0);
	}
}

void TriggerWaterfallMist(long x, long y, long z, long ang)
{
	SPARKS* sptr;
	long rad, xSize, zSize;

	for (int i = 0; i < 1536; i += 256)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 64;
		sptr->sG = 64;
		sptr->sB = 64;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
		sptr->ColFadeSpeed = 1;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 6;
		sptr->sLife = sptr->Life;
		sptr->FadeToBlack = sptr->Life - 4;

		rad = ((i + (GlobalCounter << 6)) % 1536) + (GetRandomControl() & 0x3F) - 32;
		xSize = rad * phd_sin(ang) >> 14;
		zSize = rad * phd_cos(ang) >> 14;
		sptr->x = xSize + (GetRandomControl() & 0xF) + x - 8;
		sptr->y = (GetRandomControl() & 0xF) + y - 8;
		sptr->z = zSize + (GetRandomControl() & 0xF) + z - 8;
		sptr->Xvel = 0;
		sptr->Yvel = (GetRandomControl() & 0x7F) + 128;
		sptr->Zvel = 0;

		sptr->Friction = 0;
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
		sptr->Scalar = 3;
		sptr->MaxYvel = 0;
		sptr->Gravity = -sptr->Yvel >> 2;
		sptr->Size = (GetRandomControl() & 3) + 16;
		sptr->sSize = sptr->Size;
		sptr->dSize = sptr->Size << 1;
	}

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 96;
	sptr->sG = 96;
	sptr->sB = 96;
	sptr->dR = 96;
	sptr->dG = 96;
	sptr->dB = 96;
	sptr->ColFadeSpeed = 1;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 6;
	sptr->sLife = sptr->Life;
	sptr->FadeToBlack = sptr->Life - 1;
	rad = GetRandomControl() % 1408 + 64;
	xSize = rad * phd_sin(ang) >> 14;
	zSize = rad * phd_cos(ang) >> 14;
	sptr->x = xSize + (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = zSize + (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = 0;
	sptr->Yvel = (GetRandomControl() & 0x100) + (GetRandomControl() & 0x7F) + 128;
	sptr->Zvel = 0;
	
	sptr->Friction = 0;
	sptr->Flags = 10;
	sptr->Scalar = 2;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 17;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Size = (GetRandomControl() & 7) + 8;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size << 1;
}

void TriggerUnderwaterBlood(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 32)
			return;
	}

	ripple->flags = 49;
	ripple->init = 1;
	ripple->life = (GetRandomControl() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = x + (GetRandomControl() & 0x3F) - 32;
	ripple->y = y;
	ripple->z = z + (GetRandomControl() & 0x3F) - 32;
}

void SetupRipple(long x, long y, long z, long size, long flags)
{
	RIPPLE_STRUCT* ripple;
	long num;

	num = 0;

	while (ripples[num].flags & 1)
	{
		num++;

		if (num >= 32)
			return;
	}

	ripple = &ripples[num];
	ripple->flags = (char)flags | 1;
	ripple->size = (uchar)size;
	ripple->life = (GetRandomControl() & 0xF) + 48;
	ripple->init = 1;
	ripple->x = x;
	ripple->y = y;
	ripple->z = z;

	if (flags & 0x40)
	{
		ripple->x += (GetRandomControl() & 0x7F) - 64;
		ripple->z += (GetRandomControl() & 0x7F) - 64;
	}
}

void SetupSplash(SPLASH_SETUP* setup)
{
	SPLASH_STRUCT* splash;
	long n;

	splash = splashes;
	n = 0;

	while (splash->flags & 1)
	{
		splash++;
		n++;

		if (n >= 4)
		{
			SoundEffect(SFX_LARA_SPLASH, (PHD_3DPOS*)setup, SFX_DEFAULT);
			return;
		}
	}

	splash->flags = 1;
	splash->x = setup->x;
	splash->y = setup->y;
	splash->z = setup->z;
	splash->life = 62;
	splash->InnerRad = setup->InnerRad;
	splash->InnerSize = setup->InnerSize;
	splash->InnerRadVel = setup->InnerRadVel;
	splash->InnerYVel = setup->InnerYVel;
	splash->InnerY = setup->InnerYVel >> 2;
	splash->MiddleRad = setup->MiddleRad;
	splash->MiddleSize = setup->MiddleSize;
	splash->MiddleRadVel = setup->MiddleRadVel;
	splash->MiddleYVel = setup->MiddleYVel;
	splash->MiddleY = setup->MiddleYVel >> 2;
	splash->OuterRad = setup->OuterRad;
	splash->OuterSize = setup->OuterSize;
	splash->OuterRadVel = setup->OuterRadVel;
	SoundEffect(SFX_LARA_SPLASH, (PHD_3DPOS*)setup, SFX_DEFAULT);
}

void UpdateSplashes()
{
	SPLASH_STRUCT* splash;
	RIPPLE_STRUCT* ripple;

	for (int i = 0; i < 4; i++)
	{
		splash = &splashes[i];

		if (!(splash->flags & 1))
			continue;

		splash->InnerRad += splash->InnerRadVel >> 5;
		splash->InnerSize += splash->InnerRadVel >> 6;
		splash->InnerRadVel -= splash->InnerRadVel >> 6;
		splash->MiddleRad += splash->MiddleRadVel >> 5;
		splash->MiddleSize += splash->MiddleRadVel >> 6;
		splash->MiddleRadVel -= splash->MiddleRadVel >> 6;
		splash->OuterRad += splash->OuterRadVel >> 5;
		splash->OuterSize += splash->OuterRadVel >> 6;
		splash->OuterRadVel -= splash->OuterRadVel >> 6;
		splash->InnerY += splash->InnerYVel >> 4;
		splash->InnerYVel += 0x400;

		if (splash->InnerYVel > 0x4000)
			splash->InnerYVel = 0x4000;

		if (splash->InnerY < 0)
		{
			if (splash->InnerY < -0x7000)
				splash->InnerY = -0x7000;
		}
		else
		{
			splash->InnerY = 0;
			splash->flags |= 4;
			splash->life -= 2;

			if (!splash->life)
				splash->flags = 0;
		}

		splash->MiddleY += splash->MiddleYVel >> 4;
		splash->MiddleYVel += 0x380;

		if (splash->MiddleYVel > 0x4000)
			splash->MiddleYVel = 0x4000;

		if (splash->MiddleY < 0)
		{
			if (splash->MiddleY < -0x7000)
				splash->MiddleY = -0x7000;
		}
		else
		{
			splash->MiddleY = 0;
			splash->flags |= 8;
		}
	}

	for (int i = 0; i < 32; i++)
	{
		ripple = &ripples[i];

		if (!(ripple->flags & 1))
			continue;

		if (ripple->size < 252)
		{
			if (ripple->flags & 2)
				ripple->size += 2;
			else
				ripple->size += 4;
		}

		if (ripple->init)
		{
			if (ripple->init < ripple->life)
			{
				if (ripple->flags & 2)
					ripple->init += 8;
				else
					ripple->init += 4;

				if (ripple->init >= ripple->life)
					ripple->init = 0;
			}
		}
		else
		{
			ripple->life -= 3;

			if (ripple->life > 250)
				ripple->flags = 0;
		}
	}
}

void inject_effect2(bool replace)
{
	INJECT(0x0042F460, TriggerFlareSparks, replace);
	INJECT(0x00430D90, TriggerDartSmoke, replace);
	INJECT(0x00431030, KillAllCurrentItems, replace);
	INJECT(0x00431050, KillEverything, replace);
	INJECT(0x00431560, ControlSmokeEmitter, replace);
	INJECT(0x00431E70, ControlEnemyMissile, replace);
	INJECT(0x0042FA10, TriggerExplosionSmokeEnd, replace);
	INJECT(0x00431530, ClearDynamics, replace);
	INJECT(0x00431240, TriggerDynamic, replace);
	INJECT(0x00431420, TriggerDynamic_MIRROR, replace);
	INJECT(0x00431070, TriggerExplosionBubble, replace);
	INJECT(0x00430A40, TriggerWaterfallMist, replace);
	INJECT(0x004309B0, TriggerUnderwaterBlood, replace);
	INJECT(0x00430910, SetupRipple, replace);
	INJECT(0x00430620, SetupSplash, replace);
	INJECT(0x00430710, UpdateSplashes, replace);
}
