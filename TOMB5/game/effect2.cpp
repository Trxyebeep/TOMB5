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
#include "draw.h"
#include "camera.h"
#include "lara.h"

static long number_dynamics;
DYNAMIC dynamics[64];
SPLASH_SETUP splash_setup;
long wibble = 0;
long SplashCount = 0;
long KillEverythingFlag = 0;
long SmokeCountL;
long SmokeCountR;
long SmokeWeapon;
long SmokeWindX;
long SmokeWindZ;

long DeadlyBounds[6];

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv)
{
	SPARKS* sptr;
	long dx, dz, rand;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	rand = GetRandomDraw();
	sptr = &spark[GetFreeSpark()];
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = 255;
	sptr->dG = (rand & 0x7F) + 64;
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
	sptr->Xvel = short(((rand >> 2) & 0xFF) + xv - 128);
	sptr->Yvel = short(((rand >> 4) & 0xFF) + yv - 128);
	sptr->Zvel = short(((rand >> 6) & 0xFF) + zv - 128);
	sptr->Flags = SF_SCALE;
	sptr->Size = ((rand >> 9) & 3) + 4;
	sptr->sSize = sptr->Size;
	sptr->dSize = ((rand >> 12) & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
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
		sptr->Xvel = short((GetRandomControl() & 0xFF) - xv - 128);
		sptr->Yvel = short(-4 - (GetRandomControl() & 3));
		sptr->Zvel = short((GetRandomControl() & 0xFF) - zv - 128);
	}
	else
	{
		if (xv)
			sptr->Xvel = (short)-xv;
		else
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;

		sptr->Yvel = -4 - (GetRandomControl() & 3);

		if (zv)
			sptr->Zvel = (short)-zv;
		else
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	}

	sptr->Friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_DEF | SF_SCALE;

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
		sptr->Gravity = -4 - (GetRandomControl() & 3);
		sptr->dSize = (uchar)rand;
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
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
				sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
				sptr->Yvel = -16 - (GetRandomControl() & 0xF);
				sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
			}
			else
			{
				size = (GetRandomControl() & 0x7F) + 2048;
				sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot + 0x5000)) >> W2V_SHIFT);
				sptr->Yvel = -128 - (GetRandomControl() & 0xFF);
				sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot + 0x5000)) >> W2V_SHIFT);
			}

			sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;

			if (!(GlobalCounter & 3) && !(item->trigger_flags & 4))
				sptr->Flags = SF_DAMAGE | SF_ROTATE | SF_DEF | SF_SCALE;

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

		if (item->object_number == SMOKE_EMITTER_BLACK)
		{
			sptr->dR = 96;
			sptr->dG = 96;
			sptr->dB = 96;
		}

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

		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);

		if (room[item->room_number].flags & ROOM_OUTSIDE)
			sptr->Flags = SF_OUTSIDE | SF_ROTATE | SF_DEF | SF_SCALE;

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
		phd_GetVectorAngles(lara_item->pos.x_pos - fx->pos.x_pos, lara_item->pos.y_pos - fx->pos.y_pos - 256, lara_item->pos.z_pos - fx->pos.z_pos, angles);

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

			fx->pos.x_rot += (short)ox;
			fx->pos.y_rot += (short)oy;
		}

		fx->pos.z_rot += fx->speed << 4;

		if (!fx->flag1)
			fx->pos.z_rot += fx->speed << 4;
	}

	ox = fx->pos.x_pos;
	oy = fx->pos.y_pos;
	oz = fx->pos.z_pos;
	speed = fx->speed * phd_cos(fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.x_pos += speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
	fx->pos.y_pos += fx->speed * phd_sin(-fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.z_pos += speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
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
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 96;
		sptr->dG = 96;
		sptr->dB = 96;
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

	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
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
	sptr->Flags = SF_UNWATER | SF_DEF | SF_SCALE;
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
		xSize = rad * phd_sin(ang) >> W2V_SHIFT;
		zSize = rad * phd_cos(ang) >> W2V_SHIFT;
		sptr->x = xSize + (GetRandomControl() & 0xF) + x - 8;
		sptr->y = (GetRandomControl() & 0xF) + y - 8;
		sptr->z = zSize + (GetRandomControl() & 0xF) + z - 8;
		sptr->Xvel = 0;
		sptr->Yvel = (GetRandomControl() & 0x7F) + 128;
		sptr->Zvel = 0;

		sptr->Friction = 0;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
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
	xSize = rad * phd_sin(ang) >> W2V_SHIFT;
	zSize = rad * phd_cos(ang) >> W2V_SHIFT;
	sptr->x = xSize + (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = zSize + (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = 0;
	sptr->Yvel = (GetRandomControl() & 0x100) + (GetRandomControl() & 0x7F) + 128;
	sptr->Zvel = 0;
	sptr->Friction = 0;
	sptr->Flags = SF_DEF | SF_SCALE;
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

		if (n >= MAX_RIPPLES)
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

		if (num >= MAX_RIPPLES)
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

	for (int i = 0; i < MAX_RIPPLES; i++)
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

void TriggerRicochetSpark(GAME_VECTOR* pos, long ang, long num, long smoke_only)
{
	SPARKS* sptr;
	long rnd;

	if (!smoke_only)
	{
		for (int i = 0; i < num; i++)
		{
			sptr = &spark[GetFreeSpark()];
			rnd = GetRandomControl();
			sptr->On = 1;
			sptr->sR = 128;
			sptr->sG = (rnd & 0xF) + 16;
			sptr->sB = 0;
			sptr->dR = 96;
			sptr->dG = ((rnd >> 4) & 0x1F) + 48;
			sptr->dB = 0;
			sptr->ColFadeSpeed = 2;
			sptr->FadeToBlack = 4;
			sptr->Life = 9;
			sptr->sLife = 9;
			sptr->TransType = 2;
			sptr->x = pos->x;
			sptr->y = pos->y;
			sptr->z = pos->z;
			sptr->Yvel = (rnd & 0xFFF) - 2048;
			sptr->Gravity = (rnd >> 7) & 0x1F;
			rnd = (((rnd >> 3) & 0x7FF) + ang - 1024) & 0xFFF;
			sptr->Zvel = rcossin_tbl[(rnd << 1) + 1] >> 2;
			sptr->Xvel = -rcossin_tbl[rnd << 1] >> 2;
			sptr->Friction = 34;
			sptr->Flags = SF_NONE;
			sptr->MaxYvel = 0;
		}

		rnd = GetRandomControl();
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 48;
		sptr->sG = (rnd & 0xF) + 32;
		sptr->sB = 0;
		sptr->dR = 0;
		sptr->dG = 0;
		sptr->dB = 0;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 0;
		sptr->Life = 4;
		sptr->sLife = 4;
		sptr->TransType = 2;
		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;
		sptr->Xvel = 0;
		sptr->Yvel = 0;
		sptr->Zvel = 0;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = (rnd >> 2) & 0xFFF;

		if (rnd & 1)
			sptr->RotAdd = -64 - ((rnd >> 1) & 0x3F);
		else
			sptr->RotAdd = ((rnd >> 1) & 0x3F) + 64;

		sptr->Scalar = 3;
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 12;
		sptr->Size = ((rnd >> 10) & 7) + 8;
		sptr->sSize = sptr->Size;
		sptr->dSize = 1;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}

	for (int i = 0; i < 1 - smoke_only; i++)
	{
		rnd = GetRandomControl();
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 40;
		sptr->dG = 40;
		sptr->dB = 48;
		sptr->ColFadeSpeed = (rnd & 3) + 4;
		sptr->FadeToBlack = 8;
		sptr->Life = ((rnd >> 2) & 7) + 16;
		sptr->sLife = sptr->Life;
		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;

		if (smoke_only)
		{
			sptr->ColFadeSpeed >>= 1;
			sptr->FadeToBlack = 4;
			sptr->Life >>= 1;
			sptr->sLife >>= 1;
			sptr->Xvel = (rnd & 0x1FF) - 256;
			sptr->Yvel = ((rnd >> 2) & 0x1FF) - 256;
			sptr->Zvel = ((rnd >> 4) & 0x1FF) - 256;
		}
		else
		{
			sptr->Yvel = 0;
			sptr->Xvel = 0;
			sptr->Zvel = 0;
		}

		sptr->TransType = 2;
		sptr->Friction = 0;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = short(rnd >> 3);

		if (rnd & 1)
			sptr->RotAdd = -16 - (rnd & 0xF);
		else
			sptr->RotAdd = (rnd & 0xF) + 16;

		sptr->Scalar = 2;
		sptr->Gravity = -4 - ((rnd >> 9) & 3);
		sptr->MaxYvel = -4 - ((rnd >> 6) & 3);
		sptr->Size = ((rnd >> 5) & 7) + 4;
		sptr->sSize = sptr->Size;
		sptr->dSize = sptr->Size << 2;
	}
}

void TriggerExplosionSmoke(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (!uw)
	{
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 128;
		sptr->dG = 128;
		sptr->dB = 128;
	}
	else
	{
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 3;
	sptr->Life = (GetRandomControl() & 3) + 10;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
		sptr->Friction = 2;
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->Scalar = 1;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize >> 2;
}

void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number)
{
	SPARKS* sptr;
	SP_DYNAMIC* pDL;
	long scalar, mirror, i;
	uchar extras_table[4];
	uchar r, g, b;

	extras_table[0] = 0;
	extras_table[1] = 4;
	extras_table[2] = 7;
	extras_table[3] = 10;
	scalar = 0;
	mirror = 0;

	if (room_number < 0)
	{
		room_number = -room_number;
		scalar = 1;
	}

	if (room_number == gfMirrorRoom && gfLevelFlags & GF_MIRROR)
		mirror = 1;

	do
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 255;

		if (uw == 1)
		{
			sptr->sG = (GetRandomControl() & 0x3F) + 128;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = (GetRandomControl() & 0x1F) + 64;
			sptr->dB = 0;
			sptr->ColFadeSpeed = 7;
			sptr->FadeToBlack = 8;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 16;
			sptr->sLife = sptr->Life;
			sptr->RoomNumber = (uchar)room_number;
		}
		else
		{
			sptr->sG = (GetRandomControl() & 0xF) + 32;
			sptr->sB = 0;
			sptr->dR = (GetRandomControl() & 0x3F) + 192;
			sptr->dG = (GetRandomControl() & 0x3F) + 128;
			sptr->dB = 32;
			sptr->ColFadeSpeed = 8;
			sptr->FadeToBlack = 16;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 24;
			sptr->sLife = sptr->Life;
		}

		sptr->extras = uchar(extras | ((extras_table[extras] + (GetRandomControl() & 7) + 28) << 3));
		sptr->Dynamic = (char)dynamic;

		if (dynamic == -2)
		{
			for (i = 0; i < 8; i++)
			{
				pDL = &spark_dynamics[i];

				if (!pDL->On)
				{
					pDL->On = 1;
					pDL->Falloff = 4;

					if (uw == 1)
						pDL->Flags = 2;
					else
						pDL->Flags = 1;

					sptr->Dynamic = (char)i;
					break;
				}
			}

			if (i == 8)
				sptr->Dynamic = -1;
		}

		sptr->Xvel = (GetRandomControl() & 0xFFF) - 2048;
		sptr->Yvel = (GetRandomControl() & 0xFFF) - 2048;
		sptr->Zvel = (GetRandomControl() & 0xFFF) - 2048;

		if (dynamic != -2 || uw == 1)
		{
			sptr->x = (GetRandomControl() & 0x1F) + x - 16;
			sptr->y = (GetRandomControl() & 0x1F) + y - 16;
			sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		}
		else
		{
			sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
			sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
			sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
		}

		if (uw == 1)
			sptr->Friction = 17;
		else
			sptr->Friction = 51;

		if (GetRandomControl() & 1)
		{
			if (uw == 1)
				sptr->Flags = SF_UNWATER | SF_ROTATE | SF_DEF | SF_SCALE;
			else
				sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;

			sptr->RotAng = GetRandomControl() & 0xFFF;
			sptr->RotAdd = (GetRandomControl() & 0xFF) + 128;
		}
		else if (uw == 1)
			sptr->Flags = SF_UNWATER | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_DEF | SF_SCALE;

		sptr->Scalar = 3;
		sptr->Gravity = 0;
		sptr->Size = (GetRandomControl() & 0xF) + 40;
		sptr->sSize = sptr->Size << scalar;
		sptr->dSize = sptr->Size << (scalar + 1);
		sptr->Size <<= scalar;
		sptr->MaxYvel = 0;

		if (uw == 2)
		{
			r = sptr->sR;
			g = sptr->sG;
			b = sptr->sB;
			sptr->sR = b;
			sptr->sG = r;
			sptr->sB = g;

			r = sptr->dR;
			g = sptr->dG;
			b = sptr->dB;
			sptr->dR = b;
			sptr->dG = r;
			sptr->dB = g;

			sptr->Flags |= SF_GREEN;
		}
		else if (extras)
			TriggerExplosionSmoke(x, y, z, uw);
		else
			TriggerExplosionSmokeEnd(x, y, z, uw);

		z = 2 * gfMirrorZPlane - z;
		mirror--;
	}
	while (mirror >= 0);
}

void TriggerFireFlame(long x, long y, long z, long body_part, long type)
{
	SPARKS* sptr;
	long dx, dz, size;
	uchar swp;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (type == 2)
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = sptr->sR;
		sptr->sB = (GetRandomControl() & 0x3F) + 192;
	}
	else if (type == 254)
	{
		sptr->sR = 0;
		sptr->sB = 0;
		sptr->sG = 0;
		sptr->dR = (GetRandomControl() & 0xF) + 32;
		sptr->dB = sptr->dR;
		sptr->dG = sptr->dR;
	}
	else
	{
		sptr->sR = 255;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 48;

		if (lara.BurnBlue == 1)
		{
			swp = sptr->sR;
			sptr->sR = sptr->sB;
			sptr->sB = swp;
		}
		else if (lara.BurnBlue == 2)
		{
			sptr->sB = sptr->sG >> 1;
			sptr->sG = 255;
			sptr->sR = 0;
		}
	}

	if (type != 254)
	{
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;

		if (lara.BurnBlue == 1)
		{
			swp = sptr->dR;
			sptr->dR = sptr->dB;
			sptr->dB = swp;
		}
		else if (lara.BurnBlue == 2)
		{
			sptr->dB = sptr->dG >> 1;
			sptr->dG = 255;
			sptr->dR = 0;
		}
	}

	if (body_part != -1)
	{
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 3) + 28;
	}
	else if (type == 2 || type == 255 || type == 254)
	{
		sptr->FadeToBlack = 6;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 5;
		sptr->Life = (type < 254 ? 0 : 8) + (GetRandomControl() & 3) + 16;
	}
	else
	{
		sptr->FadeToBlack = 8;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 20;
		sptr->Life = (GetRandomControl() & 7) + 40;
	}

	sptr->sLife = sptr->Life;
	sptr->TransType = 2;

	if (body_part != -1)
	{
		sptr->x = (GetRandomControl() & 0x1F) - 16;
		sptr->y = 0;
		sptr->z = (GetRandomControl() & 0x1F) - 16;
	}
	else if (!type || type == 1)
	{
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	}
	else if (type < 254)
	{
		sptr->x = (GetRandomControl() & 0xF) + x - 8;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0xF) + z - 8;
	}
	else
	{
		sptr->x = (GetRandomControl() & 0x3F) + x - 32;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0x3F) + z - 32;
	}

	if (type == 2)
	{
		sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Yvel = -1024 - (GetRandomControl() & 0x1FF);
		sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Friction = 68;
	}
	else
	{
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

		if (type == 1)
			sptr->Friction = 51;
		else
			sptr->Friction = 5;
	}

	if (body_part == -1)
	{
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);

		if (type == 254)
			sptr->Gravity >>= 1;
	}
	else
	{
		sptr->Flags = SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->FxObj = (uchar)body_part;
		sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
		sptr->MaxYvel = -24 - (GetRandomControl() & 7);
	}

	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
	sptr->Scalar = 2;

	if (!type)
		size = (GetRandomControl() & 0x1F) + 128;
	else if (type == 1)
		size = (GetRandomControl() & 0x1F) + 64;
	else if (type < 254)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 0x1F) + 32;
	}
	else
		size = (GetRandomControl() & 0xF) + 48;

	sptr->Size = (uchar)size;
	sptr->sSize = sptr->Size;

	if (type == 2)
		sptr->dSize = sptr->Size >> 2;
	else
	{
		sptr->dSize = sptr->Size >> 4;

		if (type == 7)
		{
			sptr->ColFadeSpeed >>= 2;
			sptr->FadeToBlack >>= 2;
			sptr->Life >>= 2;
			sptr->sLife = sptr->Life;
		}
	}
}

void TriggerSuperJetFlame(ITEM_INFO* item, long yvel, long deadly)
{
	SPARKS* sptr;
	long dx, dy, dz;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	dy = (GetRandomControl() & 0x1FF) - yvel;

	if (dy < 512)
		dy = 512;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = uchar((dy >> 9) + (GetRandomControl() & 7) + 16);
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + item->pos.x_pos - 16;
	sptr->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 16;
	sptr->z = (GetRandomControl() & 0x1F) + item->pos.z_pos - 16;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;

	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;

	if (deadly)
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE | SF_FIRE;

	sptr->Scalar = 2;
	sptr->dSize = uchar((GetRandomControl() & 0xF) + (dy >> 6) + 16);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;

	if ((-item->trigger_flags & 7) == 1)
	{
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = (short)-dy;
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		sptr->dSize += sptr->dSize >> 2;
		return;
	}

	sptr->y -= 64;
	sptr->Gravity = short(-((dy >> 9) + GetRandomControl() % (dy >> 8)));
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	dy -= dy >> 2;

	if (!item->pos.y_rot)
		sptr->Zvel = (short)-dy;
	else if (item->pos.y_rot == 0x4000)
		sptr->Xvel = (short)-dy;
	else if (item->pos.y_rot == -0x8000)
		sptr->Zvel = (short)dy;
	else
		sptr->Xvel = (short)dy;
}

long GetFreeSpark()
{
	SPARKS* sptr;
	static long next_spark = 0;
	long free, min_life, lp;

	free = next_spark;
	sptr = &spark[next_spark];

	for (lp = 0; lp < 1024; lp++)
	{
		if (sptr->On)
		{
			if (free == 1023)
			{
				sptr = &spark[0];
				free = 0;
			}
			else
			{
				free++;
				sptr++;
			}
		}
		else
		{
			next_spark = (free + 1) & 0x3FF;
			spark[free].extras = 0;
			spark[free].Dynamic = -1;
			spark[free].Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;
			return free;
		}
	}

	free = 0;
	min_life = 4095;

	for (lp = 0; lp < 1024; lp++)
	{
		sptr = &spark[lp];

		if (sptr->Life < min_life && sptr->Dynamic == -1 && !(sptr->Flags & SF_NOKILL))
		{
			free = lp;
			min_life = sptr->Life;
		}
	}

	next_spark = (free + 1) & 0x3FF;
	spark[free].extras = 0;
	spark[free].Dynamic = -1;
	spark[free].Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;
	return free;
}

void DetatchSpark(long num, long type)
{
	SPARKS* sptr;
	FX_INFO* fx;
	ITEM_INFO* item;

	for (int i = 0; i < 1024; i++)
	{
		sptr = &spark[i];

		if (sptr->On && sptr->Flags & type && sptr->FxObj == num)
		{
			if (type == SF_FX)
			{
				fx = &effects[num];
				sptr->x += fx->pos.x_pos;
				sptr->y += fx->pos.y_pos;
				sptr->z += fx->pos.z_pos;
				sptr->Flags &= ~SF_FX;
			}
			else if (type == SF_ITEM)
			{
				item = &items[num];
				sptr->x += item->pos.x_pos;
				sptr->y += item->pos.y_pos;
				sptr->z += item->pos.z_pos;
				sptr->Flags &= ~SF_ITEM;
			}
		}
	}
}

void UpdateSparks()
{
	SPARKS* sptr;
	SP_DYNAMIC* dynamic;
	short* bounds;
	long fade, uw, rad, rnd, x, y, z, r, g, b, falloff;

	bounds = GetBoundsAccurate(lara_item);
	DeadlyBounds[0] = lara_item->pos.x_pos + bounds[0];
	DeadlyBounds[1] = lara_item->pos.x_pos + bounds[1];
	DeadlyBounds[2] = lara_item->pos.y_pos + bounds[2];
	DeadlyBounds[3] = lara_item->pos.y_pos + bounds[3];
	DeadlyBounds[4] = lara_item->pos.z_pos + bounds[4];
	DeadlyBounds[5] = lara_item->pos.z_pos + bounds[5];

	for (int i = 0; i < 1024; i++)
	{
		sptr = &spark[i];

		if (!sptr->On)
			continue;

		sptr->Life--;

		if (!sptr->Life)
		{
			if (sptr->Dynamic != -1)
				spark_dynamics[sptr->Dynamic].On = 0;

			sptr->On = 0;
			continue;
		}

		if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed)
		{
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->R = uchar(sptr->sR + ((fade * (sptr->dR - sptr->sR)) >> 16));
			sptr->G = uchar(sptr->sG + ((fade * (sptr->dG - sptr->sG)) >> 16));
			sptr->B = uchar(sptr->sB + ((fade * (sptr->dB - sptr->sB)) >> 16));
		}
		else if (sptr->Life < sptr->FadeToBlack)
		{
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->R = uchar((sptr->dR * fade) >> 16);
			sptr->G = uchar((sptr->dG * fade) >> 16);
			sptr->B = uchar((sptr->dB * fade) >> 16);

			if (sptr->R < 8 && sptr->G < 8 && sptr->B < 8)
			{
				sptr->On = 0;
				continue;
			}
		}
		else
		{
			sptr->R = sptr->dR;
			sptr->G = sptr->dG;
			sptr->B = sptr->dB;
		}

		if (sptr->Life == sptr->FadeToBlack && sptr->Flags & SF_UNWATER)
			sptr->dSize >>= 2;

		if (sptr->Flags & SF_ROTATE)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		if (sptr->sLife - sptr->Life == sptr->extras >> 3 && sptr->extras & 7)
		{
			if (sptr->Flags & SF_UNWATER)
				uw = 1;
			else if (sptr->Flags & SF_GREEN)
				uw = 2;
			else
				uw = 0;

			for (int j = 0; j < (sptr->extras & 7); j++)
			{
				TriggerExplosionSparks(sptr->x, sptr->y, sptr->z, (sptr->extras & 7) - 1, sptr->Dynamic, uw, sptr->RoomNumber);
				sptr->Dynamic = -1;
			}

			if (uw == 1)
				TriggerExplosionBubble(sptr->x, sptr->y, sptr->z, sptr->RoomNumber);

			sptr->extras = 0;
		}

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if (sptr->MaxYvel)
		{
			if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if (sptr->Friction & 0xF)
		{
			sptr->Xvel -= sptr->Xvel >> (sptr->Friction & 0xF);
			sptr->Zvel -= sptr->Zvel >> (sptr->Friction & 0xF);
		}

		if (sptr->Friction & 0xF0)
			sptr->Yvel -= sptr->Yvel >> (sptr->Friction >> 4);

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;

		if (sptr->Flags & SF_OUTSIDE)
		{
			sptr->x += SmokeWindX >> 1;
			sptr->z += SmokeWindZ >> 1;
		}

		sptr->Size = uchar(sptr->sSize + ((fade * (sptr->dSize - sptr->sSize)) >> 16));

		if (sptr->Flags & SF_FIRE && !lara.burn || sptr->Flags & SF_DAMAGE)
		{
			rad = sptr->Size << sptr->Scalar >> 1;

			if (sptr->x + rad > DeadlyBounds[0] && sptr->x - rad < DeadlyBounds[1] &&
				sptr->y + rad > DeadlyBounds[2] && sptr->y - rad < DeadlyBounds[3] &&
				sptr->z + rad > DeadlyBounds[4] && sptr->z - rad < DeadlyBounds[5])
			{
				if (sptr->Flags & SF_FIRE)
					LaraBurn();
				else
					lara_item->hit_points -= 2;
			}
		}
	}

	for (int i = 0; i < 1024; i++)
	{
		sptr = &spark[i];

		if (!sptr->On || sptr->Dynamic == -1)
			continue;

		dynamic = &spark_dynamics[sptr->Dynamic];

		if (dynamic->Flags & 3)
		{
			rnd = GetRandomControl();
			x = sptr->x + 16 * (rnd & 0xF);
			y = sptr->y + (rnd & 0xF0);
			z = sptr->z + ((rnd >> 4) & 0xF0);
			falloff = sptr->sLife - sptr->Life - 1;

			if (falloff < 2)
			{
				if (dynamic->Falloff < 28)
					dynamic->Falloff += 6;

				r = 255 - (falloff << 3) - (rnd & 0x1F);
				g = 255 - (falloff << 4) - (rnd & 0x1F);
				b = 255 - (falloff << 6) - (rnd & 0x1F);
			}
			else if (falloff < 4)
			{
				if (dynamic->Falloff < 28)
					dynamic->Falloff += 6;

				r = 255 - (falloff << 3) - (rnd & 0x1F);
				g = 128 - (falloff << 3);
				b = 128 - (falloff << 5);

				if (b < 0)
					b = 0;
			}
			else
			{
				if (dynamic->Falloff > 0)
					dynamic->Falloff--;

				r = 224 + (rnd & 0x1F);
				g = 128 + ((rnd >> 4) & 0x1F);
				b = (rnd >> 8) & 0x3F;
			}

			if (sptr->Flags & SF_GREEN)
				TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, b, r, g);
			else
				TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, r, g, b);
		}
	}
}
