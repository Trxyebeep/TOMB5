#include "../tomb5/pch.h"
#include "tomb4fx.h"
#include "control.h"
#include "sound.h"
#include "delstuff.h"
#include "../specific/function_stubs.h"
#include "lara_states.h"
#include "items.h"
#include "effect2.h"
#include "objects.h"
#include "xatracks.h"
#include "../specific/audio.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"
#include "draw.h"

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
	long* k;
	long c1, c2, c3, ret, span;

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

void ControlTeleporter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	PHD_VECTOR s, d;
	long r, g, b, size;
	short room_number;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->trigger_flags == 512)
		{
			if (!item->item_flags[2])
			{
				item->item_flags[0] += 2;

				if (item->item_flags[0] <= 255)
				{
					size = item->item_flags[0] >> 3;

					if (size < 4)
						size = 4;
					else if (size > 31)
						size = 31;

					SoundEffect(SFX_RICH_TELEPORT, &items[item->item_flags[1]].pos, size << 8 | SFX_SETVOL);

					if (FlashFader > 4)
						FlashFader = FlashFader >> 1 & 0xFE;

					if (GlobalCounter & 1)
					{
						s.x = items[item->item_flags[1]].pos.x_pos;
						s.y = items[item->item_flags[1]].pos.y_pos - 496;
						s.z = items[item->item_flags[1]].pos.z_pos + 472;
						size = 4 * item->item_flags[0] + 256;
						d.x = s.x + GetRandomControl() % size - (size >> 1);
						d.y = s.y + GetRandomControl() % size - (size >> 1);
						d.z = s.z + GetRandomControl() % size - (size >> 1);
						r = item->item_flags[0];
						g = item->item_flags[0] - GetRandomControl() % (item->item_flags[0] >> 1);
						b = item->item_flags[0] >> 2;
						TriggerLightning(&s, &d, (GetRandomControl() & 0x1F) + 16, RGBA(r, g, b, 24), 15, 40, 5);
						TriggerLightningGlow(s.x, s.y, s.z, RGBA(r >> 1, g >> 1, b >> 1, 64));
						TriggerDynamic(s.x, s.y, s.z, (item->item_flags[0] >> 5) + (GetRandomControl() & 3) + 8, r, g, b);
					}

					if (GetRandomControl() & 1)
					{
						FlashFadeR = item->item_flags[0];
						FlashFadeG = item->item_flags[0] - GetRandomControl() % (item->item_flags[0] >> 1);
						FlashFadeB = item->item_flags[0] >> 2;
						FlashFader = 32;
						SoundEffect(SFX_TELEPORT_CRACKLES, NULL, 0);
					}

					if (!(GlobalCounter & 3))
					{
						size = GetRandomControl() & 3;
						s.x = 0;
						s.z = 0;

						if (!size)
							s.x = -512;
						else if (size == 1)
							s.x = 512;
						else
							s.z = size - 2 ? 512 : -512;

						s.x += items[item->item_flags[1]].pos.x_pos;
						s.y = items[item->item_flags[1]].pos.y_pos - 2328;
						s.z += items[item->item_flags[1]].pos.z_pos;
						d.x = items[item->item_flags[1]].pos.x_pos;
						d.y = items[item->item_flags[1]].pos.y_pos - 496;
						d.z = items[item->item_flags[1]].pos.z_pos + 472;
						r = item->item_flags[0];
						g = item->item_flags[0] - GetRandomControl() % (item->item_flags[0] >> 1);
						b = item->item_flags[0] >> 2;
						TriggerLightning(&s, &d, (GetRandomControl() & 0xF) + 16, RGBA(r, g, b, 36), 13, 56, 5);
						sptr = &spark[GetFreeSpark()];
						sptr->On = 1;
						sptr->dR = (uchar)item->item_flags[0];
						sptr->sR = (uchar)item->item_flags[0];
						sptr->dG = item->item_flags[0] >> 1;
						sptr->sG = item->item_flags[0] >> 1;
						sptr->ColFadeSpeed = 20;
						sptr->dB = item->item_flags[0] >> 2;
						sptr->sB = item->item_flags[0] >> 2;
						sptr->FadeToBlack = 4;
						sptr->Life = 24;
						sptr->sLife = 24;
						sptr->TransType = 2;
						sptr->x = s.x;
						sptr->y = s.y;
						sptr->z = s.z;
						sptr->Xvel = 0;
						sptr->Yvel = 0;
						sptr->Zvel = 0;
						sptr->Flags = 10;
						sptr->Scalar = 3;
						sptr->MaxYvel = 0;
						sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
						sptr->Gravity = 0;
						size = (GetRandomControl() & 3) + 24;
						sptr->dSize = (uchar)size;
						sptr->sSize = (uchar)size;
						sptr->Size = (uchar)size;
					}

					return;
				}

				FlashFadeR = 255;
				FlashFadeG = 255;
				FlashFadeB = 64;
				FlashFader = 32;
				item->item_flags[2] = 1;
				SoundEffect(SFX_TELEPORT_FLASH, NULL, 0x800000 | SFX_SETPITCH);
			}
			else
			{
				lara.puzzleitems[1] = 1;
				RemoveActiveItem(item_number);
				item->flags &= ~IFL_CODEBITS;
			}
		}

		bDisableLaraControl = 0;

		if (item->trigger_flags == 666)
		{
			if (item->item_flags[0] == 15)
			{
				IsAtmospherePlaying = 0;
				S_CDPlay(CDA_XA12_Z_10, 0);
			}
			else if (item->item_flags[0] == 70)
			{
				SoundEffect(SFX_LIFT_HIT_FLOOR1, NULL, 0);
				SoundEffect(SFX_LIFT_HIT_FLOOR2, NULL, 0);
			}

			lara_item->anim_number = ANIM_ELEVATOR_RECOVER;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->goal_anim_state = AS_CONTROLLED;
			lara_item->current_anim_state = AS_CONTROLLED;
			item->item_flags[0]++;

			if (item->item_flags[0] >= 150)
				KillItem(item_number);
		}
		else
		{
			camera.fixed_camera = 1;
			lara_item->pos.x_pos = item->pos.x_pos;
			lara_item->pos.z_pos = item->pos.z_pos;
			lara_item->pos.y_rot = item->pos.y_rot - 32768;
			room_number = item->room_number;
			lara_item->pos.y_pos = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (lara_item->room_number != room_number)
				ItemNewRoom(lara.item_number, room_number);

			if (item->flags & IFL_INVISIBLE)
				KillItem(item_number);
			else if (item->trigger_flags != 512)
			{
				RemoveActiveItem(item_number);
				item->flags &= ~IFL_CODEBITS;
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

void DrawLensFlares(ITEM_INFO* item)
{
	GAME_VECTOR pos;

	pos.x = item->pos.x_pos;
	pos.y = item->pos.y_pos;
	pos.z = item->pos.z_pos;
	pos.room_number = item->room_number;
	SetUpLensFlare(0, 0, 0, &pos);
}

long ExplodingDeath2(short item_number, long mesh_bits, short Flags)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FX_INFO* fx;
	long* bone;
	short* rotation;
	short* frame;
	short* extra_rotation;
	long bit, poppush;
	short fx_number;
	
	item = &items[item_number];
	obj = &objects[item->object_number];
	frame = GetBestFrame(item);
	phd_PushUnitMatrix();
#ifdef GENERAL_FIXES
	phd_SetTrans(0, 0, 0);
#else
	phd_mxptr[M03] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M23] = 0;
#endif
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rotation = frame + 9;
	gar_RotYXZsuperpack(&rotation, 0);

#ifdef GENERAL_FIXES	//fix crash if exploding inactive enemies..
	if (!item->data)
		extra_rotation = no_rotation;
	else
#endif
		extra_rotation = (short*)item->data;

	bone = &bones[obj->bone_index];
	bit = 1;

	if (mesh_bits & 1 && item->mesh_bits & 1)
	{
		if (Flags & 0x100 || !(GetRandomControl() & 3))
		{
			fx_number = CreateEffect(item->room_number);

			if (fx_number != NO_ITEM)
			{
				fx = &effects[fx_number];
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> 14);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> 14);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> 14);
				fx->room_number = item->room_number;
				fx->pos.x_rot = 0;
				fx->pos.y_rot = 0;
				fx->pos.z_rot = 0;

				if (Flags & 0x10)
					fx->speed = 0;
				else if (Flags & 0x20)
					fx->speed = short(GetRandomControl() >> 12);
				else
					fx->speed = short(GetRandomControl() >> 8);

				if (Flags & 0x40)
					fx->fallspeed = 0;
				else if (Flags & 0x80)
					fx->fallspeed = (short)-(GetRandomControl() >> 12);
				else
					fx->fallspeed = (short)-(GetRandomControl() >> 8);

				fx->frame_number = obj->mesh_index;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;

				if (item->object_number == CRUMBLING_FLOOR)
				{
					fx->speed = 0;
					fx->fallspeed = 0;
					fx->counter = 61;
				}
				else
					fx->counter = 0;

				fx->flag1 = 0;
			}

			item->mesh_bits -= bit;
		}
	}

	for (int i = 1; i < obj->nmeshes; i++, bone += 4)
	{
		poppush = bone[0];

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rotation, 0);

		if (poppush & 28)
		{
			if (poppush & 8)
			{
				phd_RotY(*extra_rotation);
				extra_rotation++;
			}

			if (poppush & 4)
			{
				phd_RotX(*extra_rotation);
				extra_rotation++;
			}

			if (poppush & 16)
			{
				phd_RotZ(*extra_rotation);
				extra_rotation++;
			}
		}

		bit <<= 1;

		if (bit & mesh_bits && bit & item->mesh_bits && (Flags & 0x100 || !(GetRandomControl() & 3)))
		{
			fx_number = CreateEffect(item->room_number);

			if (fx_number != NO_ITEM)
			{
				fx = &effects[fx_number];
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> 14);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> 14);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> 14);
				fx->room_number = item->room_number;
				fx->pos.x_rot = 0;
				fx->pos.y_rot = 0;
				fx->pos.z_rot = 0;

				if (Flags & 0x10)
					fx->speed = 0;
				else if (Flags & 0x20)
					fx->speed = short(GetRandomControl() >> 12);
				else
					fx->speed = short(GetRandomControl() >> 8);

				if (Flags & 0x40)
					fx->fallspeed = 0;
				else if (Flags & 0x80)
					fx->fallspeed = (short)-(GetRandomControl() >> 12);
				else
					fx->fallspeed = (short)-(GetRandomControl() >> 8);

				fx->frame_number = obj->mesh_index + 2 * i;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;

				if (item->object_number == CRUMBLING_FLOOR)
				{
					fx->speed = 0;
					fx->fallspeed = 0;
					fx->counter = 61;
				}
				else
					fx->counter = 0;

				fx->flag1 = 0;
			}

			item->mesh_bits -= bit;
		}
	}

	phd_PopMatrix();
	return !item->mesh_bits;
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
	INJECT(0x00485380, ControlTeleporter, replace);
	INJECT(0x004852E0, DrawWeaponMissile, replace);
	INJECT(0x00485290, DrawLensFlares, replace);
	INJECT(0x00484080, ExplodingDeath2, replace);
}
