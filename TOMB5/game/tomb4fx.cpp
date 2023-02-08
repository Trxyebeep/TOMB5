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
#include "../specific/audio.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"
#include "draw.h"

static short FadeClipSpeed;

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
		Blood->Xvel = short(-(speed * rcossin_tbl[ang << 1]) >> 7);//sin
		Blood->Zvel = short(speed * rcossin_tbl[(ang << 1) + 1] >> 7);//cos
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
				S_CDPlay(90, 0);
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

void SetGunFlash(short weapon)
{
	GUNFLASH_STRUCT* flash;
	long num;
	short xrot, y, z;

	switch (weapon)
	{
	case WEAPON_FLARE:
	case WEAPON_SHOTGUN:
	case WEAPON_CROSSBOW:
		return;

	case WEAPON_REVOLVER:
		xrot = -0x38E0;
		y = 192;
		z = 68;
		break;

	case WEAPON_UZI:
		xrot = -0x3FFC;
		y = 190;
		z = 50;
		break;

	case WEAPON_HK:
		xrot = -0x38E0;
		y = 300;
		z = 92;
		break;

	default:
		xrot = -0x3FFC;
		y = 180;
		z = 40;
		break;
	}

	num = 0;
	flash = Gunflashes;

	while (flash->on)
	{
		flash++;
		num++;

		if (num >= 4)
			return;
	}
	
	flash->on = 1;
	phd_TranslateRel(0, y, z);
	phd_RotX(xrot);
	*(float*)(flash->mx + M00) = aMXPtr[M00];
	*(float*)(flash->mx + M01) = aMXPtr[M01];
	*(float*)(flash->mx + M02) = aMXPtr[M02];
	*(float*)(flash->mx + M03) = aMXPtr[M03];
	*(float*)(flash->mx + M10) = aMXPtr[M10];
	*(float*)(flash->mx + M11) = aMXPtr[M11];
	*(float*)(flash->mx + M12) = aMXPtr[M12];
	*(float*)(flash->mx + M13) = aMXPtr[M13];
	*(float*)(flash->mx + M20) = aMXPtr[M20];
	*(float*)(flash->mx + M21) = aMXPtr[M21];
	*(float*)(flash->mx + M22) = aMXPtr[M22];
	*(float*)(flash->mx + M23) = aMXPtr[M23];
}

void DrawGunflashes()
{
	GUNFLASH_STRUCT* flash;

	if (!Gunflashes[0].on)
		return;

	phd_top = 0;
	phd_left = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
	phd_PushMatrix();
	GetRandomDraw();
	GetRandomDraw();
	GetRandomDraw();
	GetRandomDraw();

	for (int i = 0; i < 4; i++)
	{
		flash = &Gunflashes[i];

		if (!flash->on)
			break;

		aMXPtr[M00] = *(float*)(flash->mx + M00);
		aMXPtr[M01] = *(float*)(flash->mx + M01);
		aMXPtr[M02] = *(float*)(flash->mx + M02);
		aMXPtr[M03] = *(float*)(flash->mx + M03);
		aMXPtr[M10] = *(float*)(flash->mx + M10);
		aMXPtr[M11] = *(float*)(flash->mx + M11);
		aMXPtr[M12] = *(float*)(flash->mx + M12);
		aMXPtr[M13] = *(float*)(flash->mx + M13);
		aMXPtr[M20] = *(float*)(flash->mx + M20);
		aMXPtr[M21] = *(float*)(flash->mx + M21);
		aMXPtr[M22] = *(float*)(flash->mx + M22);
		aMXPtr[M23] = *(float*)(flash->mx + M23);
		phd_RotZ(short(GetRandomDraw() << 1));
		GlobalAmbient = 0xFF2F2F00;
		phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], -1);
		flash->on = 0;
	}

	phd_PopMatrix();
}

void trig_actor_gunflash(long* mx, PHD_VECTOR* pos)
{
	GUNFLASH_STRUCT* p;
	long lp;

	p = Gunflashes;
	lp = 0;

	while (p->on)
	{
		p++;
		lp++;

		if (lp >= 4)
			return;
	}

	p->on = 1;

	phd_PushMatrix();
	aMXPtr[M00] = *(float*)(mx + M00);
	aMXPtr[M01] = *(float*)(mx + M01);
	aMXPtr[M02] = *(float*)(mx + M02);
	aMXPtr[M03] = *(float*)(mx + M03);
	aMXPtr[M10] = *(float*)(mx + M10);
	aMXPtr[M11] = *(float*)(mx + M11);
	aMXPtr[M12] = *(float*)(mx + M12);
	aMXPtr[M13] = *(float*)(mx + M13);
	aMXPtr[M20] = *(float*)(mx + M20);
	aMXPtr[M21] = *(float*)(mx + M21);
	aMXPtr[M22] = *(float*)(mx + M22);
	aMXPtr[M23] = *(float*)(mx + M23);
	phd_TranslateRel(pos->x, pos->y, pos->z);
	phd_RotX(-0x4000);
	*(float*)(p->mx + M00) = aMXPtr[M00];
	*(float*)(p->mx + M01) = aMXPtr[M01];
	*(float*)(p->mx + M02) = aMXPtr[M02];
	*(float*)(p->mx + M03) = aMXPtr[M03];
	*(float*)(p->mx + M10) = aMXPtr[M10];
	*(float*)(p->mx + M11) = aMXPtr[M11];
	*(float*)(p->mx + M12) = aMXPtr[M12];
	*(float*)(p->mx + M13) = aMXPtr[M13];
	*(float*)(p->mx + M20) = aMXPtr[M20];
	*(float*)(p->mx + M21) = aMXPtr[M21];
	*(float*)(p->mx + M22) = aMXPtr[M22];
	*(float*)(p->mx + M23) = aMXPtr[M23];
	phd_PopMatrix();
}

void TriggerLightningGlow(long x, long y, long z, long rgb)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = CLRR(rgb);
	sptr->sG = CLRG(rgb);
	sptr->sB = CLRB(rgb);
	sptr->dR = sptr->sR;
	sptr->dG = sptr->sG;
	sptr->dB = sptr->sB;
	sptr->Life = 4;
	sptr->sLife = 4;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 0;
	sptr->TransType = 2;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = 10;
	sptr->Scalar = 3;
	sptr->MaxYvel = 0;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
	sptr->Gravity = 0;
	sptr->Size = (rgb >> 24) + (GetRandomControl() & 3);
	sptr->dSize = sptr->Size;
	sptr->sSize = sptr->Size;
}

void CalcLightningSpline(PHD_VECTOR* pos, SVECTOR* dest, LIGHTNING_STRUCT* lptr)
{
	long segments, x, y, z, xadd, yadd, zadd;

	dest->x = (short)pos->x;
	dest->y = (short)pos->y;
	dest->z = (short)pos->z;
	dest++;
	segments = lptr->Segments * 3;

	if (lptr->Flags & 1)
	{
		xadd = 0x10000 / (segments - 1);
		x = xadd;

		for (int i = 0; i < segments - 2; i++)
		{
			dest->x = short(LSpline(x, &pos->x, 6) + (GetRandomControl() & 0xF) - 8);
			dest->y = short(LSpline(x, &pos->y, 6) + (GetRandomControl() & 0xF) - 8);
			dest->z = short(LSpline(x, &pos->z, 6) + (GetRandomControl() & 0xF) - 8);
			dest++;
			x += xadd;
		}
	}
	else
	{
		xadd = (pos[5].x - pos->x) / (segments - 1);
		yadd = (pos[5].y - pos->y) / (segments - 1);
		zadd = (pos[5].z - pos->z) / (segments - 1);
		x = xadd + pos->x + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		y = yadd + pos->y + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		z = zadd + pos->z + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;

		for (int i = 0; i < segments - 2; i++)
		{
			dest->x = (short)x;
			dest->y = (short)y;
			dest->z = (short)z;
			dest++;
			x += xadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
			y += yadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
			z += zadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		}
	}

	dest->x = (short)pos[5].x;
	dest->y = (short)pos[5].y;
	dest->z = (short)pos[5].z;
}

void UpdateLightning()
{
	LIGHTNING_STRUCT* lptr;
	long* pPoint;
	char* pVel;

	for (int i = 0; i < 16; i++)
	{
		lptr = &Lightning[i];

		if (!lptr->Life)
			continue;

		lptr->Life -= 2;

		if (!lptr->Life)
			continue;

		pPoint = &lptr->Point[1].x;
		pVel = &lptr->Xvel1;

		for (int j = 0; j < 9; j++)
		{
			*pPoint++ += *pVel << 1;
			*pVel -= *pVel >> 4;
			pVel++;
		}

		if (GlobalCounter & 2 && lptr->Flags & 0x10 && lptr->Life > 18)
			TriggerRicochetSpark((GAME_VECTOR*)&lptr->Point[3], GetRandomControl() << 1, 2, 0);
	}
}

void TriggerShockwaveHitEffect(long x, long y, long z, long rgb, short dir, long speed)
{
	SPARKS* sptr;
	long dx, dz, xvel, zvel;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = CLRR(rgb);
	sptr->dG = CLRG(rgb);
	sptr->dB = CLRB(rgb);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	speed += GetRandomControl() & 0xF;
	xvel = speed * phd_sin(dir) >> 10;
	zvel = speed * phd_cos(dir) >> 10;

	if (GetRandomControl() & 1)
		dir += 0x4000;
	else
		dir -= 0x4000;

	speed = (GetRandomControl() & 0x1FF) - 256;
	x += speed * phd_sin(dir) >> 14;
	z += speed * phd_cos(dir) >> 14;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (short)xvel;
	sptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
	sptr->Zvel = (short)zvel;
	sptr->Friction = 3;
	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Scalar = 1;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0x3F) + 64;
	sptr->Size = (GetRandomControl() & 0x1F) + 32;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;
}

long GetFreeShockwave()
{
	for (int i = 0; i < 16; i++)
	{
		if (!ShockWaves[i].life)
			return i;
	}

	return -1;
}

void TriggerShockwave(PHD_VECTOR* pos, long InnerOuterRads, long speed, long bgrl, long XRotFlags)
{
	SHOCKWAVE_STRUCT* sw;
	long swn;

	swn = GetFreeShockwave();

	if (swn != -1)
	{
		sw = &ShockWaves[swn];
		sw->x = pos->x;
		sw->y = pos->y;
		sw->z = pos->z;
		sw->InnerRad = InnerOuterRads & 0xFFFF;
		sw->OuterRad = InnerOuterRads >> 16;
		sw->XRot = XRotFlags & 0xFFFF;
		sw->Flags = XRotFlags >> 16;
		sw->Speed = (short)speed;
		sw->r = CLRB(bgrl);
		sw->g = CLRG(bgrl);
		sw->b = CLRR(bgrl);
		sw->life = CLRA(bgrl);
		SoundEffect(SFX_IMP_STONE_HIT, (PHD_3DPOS*)pos, SFX_DEFAULT);
	}
}

void UpdateShockwaves()
{
	SHOCKWAVE_STRUCT* sw;
	short* bounds;
	long dx, dz, dist;
	short dir;

	for (int i = 0; i < 16; i++)
	{
		sw = &ShockWaves[i];

		if (!sw->life)
			continue;

		sw->life--;

		if (!sw->life)
			continue;

		sw->OuterRad += sw->Speed;
		sw->InnerRad += sw->Speed >> 1;
		sw->Speed -= sw->Speed >> 4;

		if (lara_item->hit_points >= 0 && sw->Flags & 3)
		{
			bounds = GetBestFrame(lara_item);
			dx = lara_item->pos.x_pos - sw->x;
			dz = lara_item->pos.z_pos - sw->z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dz));

			if (sw->y > lara_item->pos.y_pos + bounds[2] && sw->y < bounds[3] + lara_item->pos.y_pos + 256 &&
				dist > sw->InnerRad && dist < sw->OuterRad)
			{
				dir = (short)phd_atan(dz, dx);
				TriggerShockwaveHitEffect(lara_item->pos.x_pos, sw->y, lara_item->pos.z_pos, *(long*)&sw->r, dir, sw->Speed);
				lara_item->hit_points -= sw->Speed >> (((sw->Flags & 2) != 0) + 2);
			}
			else
				sw->Temp = 0;
		}
	}
}

long GetFreeDrip()
{
	DRIP_STRUCT* drip;
	long min_life, min_life_num, free;

	free = next_drip;
	drip = &Drips[next_drip];
	min_life = 4095;
	min_life_num = 0;

	for (int i = 0; i < 32; i++)
	{
		if (drip->On)
		{
			if (drip->Life < min_life)
			{
				min_life_num = free;
				min_life = drip->Life;
			}

			if (free == 31)
			{
				drip = &Drips[0];
				free = 0;
			}
			else
			{
				free++;
				drip++;
			}
		}
		else
		{
			next_drip = (free + 1) & 0x1F;
			return free;
		}
	}

	next_drip = (min_life_num + 1) & 0x1F;
	return min_life_num;
}

void TriggerLaraDrips()
{
	DRIP_STRUCT* drip;
	PHD_VECTOR pos;

	if (wibble & 0xF)
		return;

	for (int i = 14; i > 0; i--)
	{
		if (lara.wet[i] && !LaraNodeUnderwater[i] && (GetRandomControl() & 0x1FF) < lara.wet[i])
		{
			pos.x = (GetRandomControl() & 0x1F) - 16;
			pos.y = (GetRandomControl() & 0xF) + 16;
			pos.z = (GetRandomControl() & 0x1F) - 16;
			GetLaraJointPos(&pos, i);

			drip = &Drips[GetFreeDrip()];
			drip->x = pos.x;
			drip->y = pos.y;
			drip->z = pos.z;
			drip->On = 1;
			drip->R = (GetRandomControl() & 7) + 16;
			drip->G = (GetRandomControl() & 7) + 24;
			drip->B = (GetRandomControl() & 7) + 32;
			drip->Yvel = (GetRandomControl() & 0x1F) + 32;
			drip->Gravity = (GetRandomControl() & 0x1F) + 32;
			drip->Life = (GetRandomControl() & 0x1F) + 16;
			drip->RoomNumber = lara_item->room_number;
			lara.wet[i] -= 4;
		}
	}
}

void UpdateDrips()
{
	DRIP_STRUCT* drip;
	FLOOR_INFO* floor;
	long h;

	for (int i = 0; i < 32; i++)
	{
		drip = &Drips[i];

		if (!drip->On)
			continue;

		drip->Life--;

		if (!drip->Life)
		{
			drip->On = 0;
			continue;
		}

		if (drip->Life < 16)
		{
			drip->R -= drip->R >> 3;
			drip->G -= drip->G >> 3;
			drip->B -= drip->B >> 3;
		}

		drip->Yvel += drip->Gravity;

		if (room[drip->RoomNumber].flags & ROOM_NOT_INSIDE)
		{
			drip->x += SmokeWindX >> 1;
			drip->z += SmokeWindZ >> 1;
		}

		drip->y += drip->Yvel >> 5;
		floor = GetFloor(drip->x, drip->y, drip->z, &drip->RoomNumber);

		if (room[drip->RoomNumber].flags & ROOM_UNDERWATER)
			drip->On = 0;

		h = GetHeight(floor, drip->x, drip->y, drip->z);

		if (drip->y > h)
		{
			if (!(i & 1))
				TriggerSmallSplash(drip->x, h, drip->z, 1);

			drip->On = 0;
		}
	}
}

void Fade()
{
	long oldfucker;

	oldfucker = ScreenFade;

	if (dScreenFade && dScreenFade >= ScreenFade)
	{
		ScreenFade += ScreenFadeSpeed;

		if (ScreenFade > dScreenFade)
		{
			ScreenFade = dScreenFade;

			if (oldfucker >= dScreenFade)
			{
				ScreenFadedOut = 1;

				if (ScreenFadeBack)
				{
					dScreenFade = 0;
					ScreenFadeBack = 0;
				}
				else
					ScreenFading = 0;
			}
		}
	}
	else if (dScreenFade < ScreenFade)
	{
		ScreenFade -= ScreenFadeSpeed;

		if (ScreenFade < dScreenFade)
		{
			ScreenFade = dScreenFade;
			ScreenFading = 0;
		}
	}

	if (ScreenFade || dScreenFade)
		DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(ScreenFade, ScreenFade, ScreenFade, 98), 2, 0);
}

void SetScreenFadeOut(short speed, short back)
{
	if (!ScreenFading)
	{
		ScreenFading = 1;
		ScreenFade = 0;
		dScreenFade = 255;
		ScreenFadeSpeed = speed;
		ScreenFadeBack = back;
		ScreenFadedOut = 0;
	}
}

void SetScreenFadeIn(short speed)
{
	if (!ScreenFading)
	{
		ScreenFading = 1;
		ScreenFade = 255;
		dScreenFade = 0;
		ScreenFadeSpeed = speed;
		ScreenFadedOut = 0;
	}
}

void UpdateFadeClip()
{
	if (DestFadeScreenHeight < FadeScreenHeight)
	{
		FadeScreenHeight -= FadeClipSpeed;

		if (DestFadeScreenHeight > FadeScreenHeight)
			FadeScreenHeight = DestFadeScreenHeight;
	}
	else if (DestFadeScreenHeight > FadeScreenHeight)
	{
		FadeScreenHeight += FadeClipSpeed;

		if (DestFadeScreenHeight < FadeScreenHeight)
			FadeScreenHeight = DestFadeScreenHeight;
	}
}

void SetFadeClip(short height, short speed)
{
	DestFadeScreenHeight = height;
	FadeClipSpeed = speed;
}

long GetFreeBubble()
{
	BUBBLE_STRUCT* bubble;
	long free;

	free = next_bubble;
	bubble = &Bubbles[next_bubble];

	for (int i = 0; i < 40; i++)
	{
		if (bubble->size)
		{
			if (free == 39)
			{
				bubble = &Bubbles[0];
				free = 0;
			}
			else
			{
				free++;
				bubble++;
			}
		}
		else
		{
			next_bubble = free + 1;

			if (next_bubble >= 40)
				next_bubble = 0;

			return free;
		}
	}

	next_bubble = free + 1;

	if (next_bubble >= 40)
		next_bubble = 0;

	return free;
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
	INJECT(0x004837B0, SetGunFlash, replace);
	INJECT(0x004838E0, DrawGunflashes, replace);
	INJECT(0x00485EC0, trig_actor_gunflash, replace);
	INJECT(0x004851B0, TriggerLightningGlow, replace);
	INJECT(0x00484EB0, CalcLightningSpline, replace);
	INJECT(0x00484CB0, UpdateLightning, replace);
	INJECT(0x00484700, TriggerShockwaveHitEffect, replace);
	INJECT(0x00484640, GetFreeShockwave, replace);
	INJECT(0x00484670, TriggerShockwave, replace);
	INJECT(0x004849A0, UpdateShockwaves, replace);
	INJECT(0x00483D00, GetFreeDrip, replace);
	INJECT(0x00483F00, TriggerLaraDrips, replace);
	INJECT(0x00483D90, UpdateDrips, replace);
	INJECT(0x00483BF0, Fade, replace);
	INJECT(0x00483B30, SetScreenFadeOut, replace);
	INJECT(0x00483BA0, SetScreenFadeIn, replace);
	INJECT(0x00483AC0, UpdateFadeClip, replace);
	INJECT(0x00483A90, SetFadeClip, replace);
	INJECT(0x004832C0, GetFreeBubble, replace);
}
