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
#include "gameflow.h"
#include "camera.h"
#include "spotcam.h"
#include "lara.h"
#include "effects.h"

LIGHTNING_STRUCT Lightning[16];
SHOCKWAVE_STRUCT ShockWaves[16];
DRIP_STRUCT Drips[32];
BUBBLE_STRUCT Bubbles[40];
RIPPLE_STRUCT ripples[MAX_RIPPLES];
SPLASH_STRUCT splashes[4];
DEBRIS_STRUCT debris[256];
GUNFLASH_STRUCT Gunflashes[4];
GUNSHELL_STRUCT Gunshells[24];
FIRE_SPARKS fire_spark[20];
SPARKS spark[1024];
SP_DYNAMIC spark_dynamics[8];
FIRE_LIST fires[32];
SMOKE_SPARKS smoke_spark[32];
BLOOD_STRUCT blood[32];
long next_fire_spark = 1;
long next_smoke_spark;
long next_gunshell;
long next_bubble;
long next_drip;
long next_blood;
long next_debris = 0;
short FlashFadeR;
short FlashFadeG;
short FlashFadeB;
short FlashFader;
short ScreenFade;
short dScreenFade;
short ScreenFadeBack;
short ScreenFadedOut;
short ScreenFading;
short FadeScreenHeight;
short DestFadeScreenHeight;
short ScreenFadeSpeed = 8;
char tsv_buffer[16384];

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
		Blood->Xvel = short(-(speed * rcossin_tbl[ang << 1]) >> 7);
		Blood->Yvel = -128 - (GetRandomControl() & 0xFF);
		Blood->Zvel = short(speed * rcossin_tbl[(ang << 1) + 1] >> 7);
		Blood->Friction = 4;
		Blood->RotAng = GetRandomControl() & 0xFFF;
		Blood->RotAdd = (GetRandomControl() & 0x3F) + 64;

		if (GetRandomControl() & 1)
			Blood->RotAdd = -Blood->RotAdd;

		Blood->Gravity = (GetRandomControl() & 0x1F) + 31;
		size = uchar((GetRandomControl() & 7) + 8);
		Blood->Size = size;
		Blood->sSize = size;
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
		GetLaraJointPos(&pos, LMX_TORSO);
	}
	else
	{
		pos.y = -4;
		pos.z = 64;
		GetLaraJointPos(&pos, LMX_HEAD);
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
						SoundEffect(SFX_TELEPORT_CRACKLES, 0, SFX_DEFAULT);
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
						sptr->Flags = SF_DEF | SF_SCALE;
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
				SoundEffect(SFX_TELEPORT_FLASH, 0, 0x800000 | SFX_SETPITCH);
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
				SoundEffect(SFX_LIFT_HIT_FLOOR1, 0, SFX_DEFAULT);
				SoundEffect(SFX_LIFT_HIT_FLOOR2, 0, SFX_DEFAULT);
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
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rotation = frame + 9;
	gar_RotYXZsuperpack(&rotation, 0);

	if (!item->data)
		extra_rotation = no_rotation;
	else
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
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
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
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
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
	sptr->Flags = SF_DEF | SF_SCALE;
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
	xvel = speed * phd_sin(dir) >> (W2V_SHIFT - 4);
	zvel = speed * phd_cos(dir) >> (W2V_SHIFT - 4);

	if (GetRandomControl() & 1)
		dir += 0x4000;
	else
		dir -= 0x4000;

	speed = (GetRandomControl() & 0x1FF) - 256;
	x += speed * phd_sin(dir) >> W2V_SHIFT;
	z += speed * phd_cos(dir) >> W2V_SHIFT;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (short)xvel;
	sptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
	sptr->Zvel = (short)zvel;
	sptr->Friction = 3;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
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
		if (lara.wet[i])
		{
			if (!LaraNodeUnderwater[i] && (GetRandomControl() & 0x1FF) < lara.wet[i])
			{
				pos.x = (GetRandomControl() & 0x1F) - 16;
				pos.y = (GetRandomControl() & 0xF) + 16;
				pos.z = (GetRandomControl() & 0x1F) - 16;
				GetLaraJointPos(&pos, i);

				drip = &Drips[GetFreeDrip()];
				drip->On = 1;
				drip->x = pos.x;
				drip->y = pos.y;
				drip->z = pos.z;
				drip->R = (GetRandomControl() & 7) + 16;
				drip->G = (GetRandomControl() & 7) + 24;
				drip->B = (GetRandomControl() & 7) + 32;
				drip->Yvel = (GetRandomControl() & 0x1F) + 32;
				drip->Gravity = (GetRandomControl() & 0x1F) + 32;
				drip->Life = (GetRandomControl() & 0x1F) + 16;
				drip->RoomNumber = lara_item->room_number;
			}

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
				TriggerDripSplash(drip->x, h, drip->z, 1);

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

void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long biggest, long flags, short xv, short yv, short zv)
{
	BUBBLE_STRUCT* bubble;

	GetFloor(pos->x_pos, pos->y_pos, pos->z_pos, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		bubble = &Bubbles[GetFreeBubble()];
		bubble->pos.x = pos->x_pos;
		bubble->pos.y = pos->y_pos;
		bubble->pos.z = pos->z_pos;
		bubble->room_number = room_number;
		bubble->speed = (GetRandomControl() & 0xFF) + 64;
		bubble->shade = 0;
		bubble->size = short((size + (biggest & GetRandomControl())) << 1);
		bubble->dsize = bubble->size << 4;
		bubble->vel = (GetRandomControl() & 0x1F) + 32;
		bubble->Flags = (char)flags;
		bubble->Xvel = xv;
		bubble->Yvel = yv;
		bubble->Zvel = zv;
	}
}

void UpdateBubbles()
{
	BUBBLE_STRUCT* bubble;
	FLOOR_INFO* floor;
	long h, c;
	short room_number;

	for (int i = 0; i < 40; i++)
	{
		bubble = &Bubbles[i];

		if (!bubble->size)
			continue;

		bubble->y_rot += 6;
		bubble->speed += bubble->vel;
		bubble->pos.y -= bubble->speed >> 8;

		if (bubble->Flags & 1)
		{
			bubble->pos.x += bubble->Xvel >> 4;
			bubble->pos.y += bubble->Yvel >> 4;
			bubble->pos.z += bubble->Zvel >> 4;
			bubble->Xvel -= bubble->Xvel >> 3;
			bubble->Yvel -= bubble->Yvel >> 3;
			bubble->Zvel -= bubble->Zvel >> 3;
		}
		else
		{
			bubble->pos.x += (3 * phd_sin(bubble->y_rot << 8)) >> W2V_SHIFT;
			bubble->pos.z += phd_cos(bubble->y_rot << 8) >> W2V_SHIFT;
		}

		room_number = bubble->room_number;
		floor = GetFloor(bubble->pos.x, bubble->pos.y, bubble->pos.z, &room_number);
		h = GetHeight(floor, bubble->pos.x, bubble->pos.y, bubble->pos.z);

		if (bubble->pos.y > h || !floor)
		{
			bubble->size = 0;
			continue;
		}

		if (!(room[room_number].flags & ROOM_UNDERWATER))
		{
			SetupRipple(bubble->pos.x, room[bubble->room_number].maxceiling, bubble->pos.z, (GetRandomControl() & 0xF) + 48, 2);
			bubble->size = 0;
			continue;
		}

		c = GetCeiling(floor, bubble->pos.x, bubble->pos.y, bubble->pos.z);

		if (c == NO_HEIGHT || bubble->pos.y <= c)
		{
			bubble->size = 0;
			continue;
		}

		if (bubble->size < bubble->dsize)
			bubble->size++;

		if (bubble->Flags & 1)
		{
			if (bubble->shade < 96)
				bubble->shade += 16;
		}
		else
		{
			if (bubble->shade < 144)
				bubble->shade += 2;
		}

		bubble->room_number = room_number;
	}
}

void TriggerSmallSplash(long x, long y, long z, long num)
{
	SPARKS* sptr;
	short ang;

	while (num)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 64;
		sptr->sG = 64;
		sptr->sB = 64;
		sptr->dR = 32;
		sptr->dG = 32;
		sptr->dB = 32;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 2;
		ang = GetRandomControl() & 0xFFF;
		sptr->Xvel = -rcossin_tbl[ang << 1] >> 5;
		sptr->Yvel = -640 - (GetRandomControl() & 0xFF);
		sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 5;
		sptr->x = x + (sptr->Xvel >> 3);
		sptr->y = y - (sptr->Yvel >> 5);
		sptr->z = z + (sptr->Zvel >> 3);
		sptr->Friction = 5;
		sptr->Flags = SF_NONE;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0xF) + 64;
		num--;
	}
}

void TriggerDripSplash(long x, long y, long z, long num)	//new func; same as above but more fitting color to the new drips!
{
	SPARKS* sptr;
	short ang;

	while (num)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = (GetRandomControl() & 0x1F) + 64;
		sptr->sG = (GetRandomControl() & 0x1F) + 96;
		sptr->sB = (GetRandomControl() & 0x1F) + 128;
		sptr->dR = sptr->sR >> 1;
		sptr->dG = sptr->sG >> 1;
		sptr->dB = sptr->sB >> 1;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 2;
		ang = GetRandomControl() & 0xFFF;
		sptr->Xvel = -rcossin_tbl[ang << 1] >> 5;
		sptr->Yvel = -640 - (GetRandomControl() & 0xFF);
		sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 5;
		sptr->x = x + (sptr->Xvel >> 3);
		sptr->y = y - (sptr->Yvel >> 5);
		sptr->z = z + (sptr->Zvel >> 3);
		sptr->Friction = 5;
		sptr->Flags = SF_NONE;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0xF) + 64;
		num--;
	}
}

long GetFreeGunshell()
{
	GUNSHELL_STRUCT* shell;
	long free, min_life, min_life_num;

	shell = &Gunshells[next_gunshell];
	min_life = 4095;
	min_life_num = 0;
	free = next_gunshell;

	for (int i = 0; i < 24; i++)
	{
		if (shell->counter)
		{
			if (shell->counter < min_life)
			{
				min_life_num = free;
				min_life = shell->counter;
			}

			if (free == 23)
			{
				shell = &Gunshells[0];
				free = 0;
			}
			else
			{
				free++;
				shell++;
			}
		}
		else
		{
			next_gunshell = free + 1;

			if (next_gunshell >= 24)
				next_gunshell = 0;

			return free;
		}
	}

	next_gunshell = min_life_num + 1;

	if (next_gunshell >= 24)
		next_gunshell = 0;

	return min_life_num;
}

void TriggerGunShell(short leftright, short objnum, long weapon)
{
	GUNSHELL_STRUCT* shell;
	PHD_VECTOR pos;
	long shade;

	if (leftright)
	{
		switch (weapon)
		{
		case WEAPON_PISTOLS:
			pos.x = 8;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = 8;
			pos.y = 35;
			pos.z = 48;
			break;

		case WEAPON_SHOTGUN:
			pos.x = 16;
			pos.y = 114;
			pos.z = 32;
			break;
		}

		GetLaraJointPos(&pos, LMX_HAND_R);
	}
	else
	{
		switch (weapon)
		{
		case WEAPON_PISTOLS:
			pos.x = -12;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = -16;
			pos.y = 35;
			pos.z = 48;
			break;
		}

		GetLaraJointPos(&pos, LMX_HAND_L);
	}

	shell = &Gunshells[GetFreeGunshell()];
	shell->pos.x_pos = pos.x;
	shell->pos.y_pos = pos.y;
	shell->pos.z_pos = pos.z;
	shell->room_number = lara_item->room_number;
	shell->pos.x_rot = 0;
	shell->pos.y_rot = 0;
	shell->pos.z_rot = (short)GetRandomControl();
	shell->speed = (GetRandomControl() & 0x1F) + 16;
	shell->object_number = objnum;
	shell->fallspeed = -48 - (GetRandomControl() & 7);
	shell->counter = (GetRandomControl() & 0x1F) + 60;

	if (leftright)
	{
		if (weapon == WEAPON_SHOTGUN)
		{
			shell->DirXrot = lara.torso_y_rot + lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x2800;
			shell->pos.y_rot += lara.left_arm.y_rot + lara.torso_y_rot + lara_item->pos.y_rot;

			if (shell->speed < 24)
				shell->speed += 24;
		}
		else
			shell->DirXrot = lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x4800;
	}
	else
		shell->DirXrot = lara_item->pos.y_rot + (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot - 0x4800;

	if (lara_item->mesh_bits)
	{
		if (weapon == WEAPON_SHOTGUN)
			shade = 24;
		else
			shade = 16;

		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, weapon, shade);
	}
}

void UpdateGunShells()
{
	GUNSHELL_STRUCT* shell;
	FLOOR_INFO* floor;
	long ox, oy, oz, c, h;
	short oroom;

	for (int i = 0; i < 24; i++)
	{
		shell = &Gunshells[i];

		if (!shell->counter)
			continue;

		ox = shell->pos.x_pos;
		oy = shell->pos.y_pos;
		oz = shell->pos.z_pos;
		oroom = shell->room_number;
		shell->counter--;

		if (room[oroom].flags & ROOM_UNDERWATER)
		{
			shell->fallspeed++;

			if (shell->fallspeed > 8)
				shell->fallspeed = 8;
			else if (shell->fallspeed < 0)
				shell->fallspeed >>= 1;

			shell->speed -= shell->speed >> 1;
		}
		else
			shell->fallspeed += 6;

		shell->pos.x_rot += 182 * ((shell->speed >> 1) + 7);
		shell->pos.y_rot += 182 * shell->speed;
		shell->pos.z_rot += 4186;
		shell->pos.x_pos += shell->speed * phd_sin(shell->DirXrot) >> (W2V_SHIFT + 1);
		shell->pos.y_pos += shell->fallspeed;
		shell->pos.z_pos += shell->speed * phd_cos(shell->DirXrot) >> (W2V_SHIFT + 1);
		floor = GetFloor(shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos, &shell->room_number);

		if (room[shell->room_number].flags & ROOM_UNDERWATER && !(room[oroom].flags & ROOM_UNDERWATER))
		{
			TriggerSmallSplash(shell->pos.x_pos, room[shell->room_number].maxceiling, shell->pos.z_pos, 8);
			SetupRipple(shell->pos.x_pos, room[shell->room_number].maxceiling, shell->pos.z_pos, (GetRandomControl() & 3) + 8, 2);
			shell->fallspeed >>= 5;
			continue;
		}

		c = GetCeiling(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if (shell->pos.y_pos < c)
		{
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 4;

			if (shell->speed < 8)
			{
				shell->counter = 0;
				continue;
			}

			shell->pos.y_pos = c;
			shell->fallspeed = -shell->fallspeed;
		}

		h = GetHeight(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if (shell->pos.y_pos >= h)
		{
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 8;

			if (shell->speed < 8)
			{
				shell->counter = 0;
				continue;
			}

			if (oy <= h)
				shell->fallspeed = -shell->fallspeed >> 1;
			else
			{
				shell->DirXrot += 0x8000;
				shell->pos.x_pos = ox;
				shell->pos.z_pos = oz;
			}

			shell->pos.y_pos = oy;
		}
	}
}

void DrawGunshells()
{
	GUNSHELL_STRUCT* p;
	OBJECT_INFO* obj;

	phd_left = 0;
	phd_right = phd_winwidth;
	phd_top = 0;
	phd_bottom = phd_winheight;

	for (int i = 0; i < 24; i++)
	{
		p = &Gunshells[i];

		if (p->counter)
		{
			obj = &objects[p->object_number];
			phd_PushMatrix();
			phd_TranslateAbs(p->pos.x_pos, p->pos.y_pos, p->pos.z_pos);
			phd_RotYXZ(p->pos.y_rot, p->pos.x_rot, p->pos.z_rot);
			phd_PutPolygons(meshes[obj->mesh_index], -1);
			phd_PopMatrix();
		}
	}
}

void TriggerShatterSmoke(long x, long y, long z)
{
	SMOKE_SPARKS* sptr;

	sptr = &smoke_spark[GetFreeSmokeSpark()];
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = (GetRandomControl() & 0x1F) + 64;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 24 - (GetRandomControl() & 7);
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 48;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Friction = 7;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
		else
			sptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
	}
	else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = SF_OUTSIDE;
	else
		sptr->Flags = SF_NONE;

	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x3F) + 64;
	sptr->sSize = sptr->dSize >> 3;
	sptr->Size = sptr->dSize >> 3;
}

void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade)
{
	SMOKE_SPARKS* sptr;
	uchar size;

	sptr = &smoke_spark[GetFreeSmokeSpark()];
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = uchar(4 * shade);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = uchar(32 - 16 * notLara);
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;

	if ((weaponType == WEAPON_PISTOLS || weaponType == WEAPON_REVOLVER || weaponType == WEAPON_UZI) && sptr->dShade > 64)
		sptr->dShade = 64;

	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (notLara)
	{
		sptr->Xvel = short((GetRandomControl() & 0x3FF) + xVel - 512);
		sptr->Yvel = short((GetRandomControl() & 0x3FF) + yVel - 512);
		sptr->Zvel = short((GetRandomControl() & 0x3FF) + zVel - 512);
	}
	else
	{
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
	}

	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
			sptr->Flags = SF_OUTSIDE | SF_ROTATE;
		else
			sptr->Flags = SF_ROTATE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = SF_OUTSIDE;
	else
		sptr->Flags = SF_NONE;

	sptr->Gravity = -2 - (GetRandomControl() & 1);
	sptr->MaxYvel = -2 - (GetRandomControl() & 1);
	size = (GetRandomControl() & 0xF) - (weaponType != WEAPON_HK ? 24 : 0) + 48;

	if (notLara)
	{
		sptr->Size = size >> 1;
		sptr->sSize = size >> 1;
		sptr->dSize = (size + 4) << 1;
	}
	else
	{
		sptr->sSize = size >> 2;
		sptr->Size = size >> 2;
		sptr->dSize = size;
	}

	sptr->mirror = gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom;
}

long GetFreeSmokeSpark()
{
	SMOKE_SPARKS* sptr;
	long min_life, min_life_num;

	sptr = &smoke_spark[next_smoke_spark];
	min_life = 4095;
	min_life_num = 0;

	for (int free = next_smoke_spark, i = 0; i < 32; i++)
	{
		if (sptr->On)
		{
			if (sptr->Life < min_life)
			{
				min_life_num = free;
				min_life = sptr->Life;
			}

			if (free == 31)
			{
				sptr = &smoke_spark[0];
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
			next_smoke_spark = (free + 1) & 0x1F;
			return free;
		}
	}

	next_smoke_spark = (min_life_num + 1) & 0x1F;
	return min_life_num;
}

void UpdateSmokeSparks()
{
	SMOKE_SPARKS* sptr;
	long fade;

	for (int i = 0; i < 32; i++)
	{
		sptr = &smoke_spark[i];

		if (!sptr->On)
			continue;

		sptr->Life -= 2;

		if (sptr->Life <= 0)
		{
			sptr->On = 0;
			continue;
		}

		if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed)
		{
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->Shade = uchar(sptr->sShade + (((sptr->dShade - sptr->sShade) * fade) >> 16));
		}
		else if (sptr->Life < sptr->FadeToBlack)
		{
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->Shade = uchar((sptr->dShade * fade) >> 16);

			if (sptr->Shade < 8)
			{
				sptr->On = 0;
				continue;
			}
		}
		else
			sptr->Shade = sptr->dShade;

		if (sptr->Shade < 24)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 2);
		else if (sptr->Shade < 80)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 1);
		else
			sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;

		if (sptr->Flags & SF_ROTATE)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

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
	}
}

void S_DrawFires()
{
	FIRE_LIST* fire;
	ROOM_INFO* r;
	short bounds[6];
	short size;

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];

		if (!fire->on)
			continue;

		if (fire->size == 2)
			size = 256;
		else
			size = 384;

		bounds[0] = -size;
		bounds[1] = size;
		bounds[2] = -size * 6;
		bounds[3] = size;
		bounds[4] = -size;
		bounds[5] = size;

		r = &room[fire->room_number];
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;

		phd_PushMatrix();
		phd_TranslateAbs(fire->x, fire->y, fire->z);

		if (S_GetObjectBounds(bounds))
		{
			if (fire->on == 1)
				S_DrawFireSparks((uchar)fire->size, 255);
			else
				S_DrawFireSparks((uchar)fire->size, fire->on & 0xFF);
		}

		phd_PopMatrix();
	}

	phd_top = 0;
	phd_left = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
}

void AddFire(long x, long y, long z, long size, short room_number, short fade)
{
	FIRE_LIST* fire;

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];

		if (fire->on)
			continue;

		if (fade)
			fire->on = (char)fade;
		else
			fire->on = 1;

		fire->x = x;
		fire->y = y;
		fire->z = z;
		fire->size = (char)size;
		fire->room_number = room_number;
		break;
	}
}

long GetFreeFireSpark()
{
	FIRE_SPARKS* sptr;
	long free, min_life, min_life_num;

	sptr = &fire_spark[next_fire_spark];
	min_life = 4095;
	min_life_num = 0;
	free = next_fire_spark;

	for (int i = 0; i < 20; i++)
	{
		if (sptr->On)
		{
			if (sptr->Life < min_life)
			{
				min_life_num = free;
				min_life = sptr->Life;
			}

			if (free == 19)
			{
				sptr = &fire_spark[1];
				free = 1;
			}
			else
			{
				free++;
				sptr++;
			}
		}
		else
		{
			next_fire_spark = free + 1;

			if (next_fire_spark >= 20)
				next_fire_spark = 1;

			return free;
		}
	}

	next_fire_spark = min_life_num + 1;

	if (next_fire_spark >= 20)
		next_fire_spark = 1;

	return min_life_num;
}

void TriggerGlobalFireSmoke()
{
	FIRE_SPARKS* sptr;

	sptr = &fire_spark[GetFreeFireSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;
	sptr->FadeToBlack = 16;
	sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
	sptr->Life = (GetRandomControl() & 0xF) + 57;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = -256 - (GetRandomControl() & 0x7F);
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_NONE;

	sptr->Gravity = -16 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = (GetRandomControl() & 0x7F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->dSize >> 2;
}

void TriggerGlobalFireFlame()
{
	FIRE_SPARKS* sptr;

	sptr = &fire_spark[GetFreeFireSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->y = 0;
	sptr->z = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->Xvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Friction = 5;
	sptr->Gravity = -32 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_NONE;

	sptr->Size = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;
}

void TriggerGlobalStaticFlame()
{
	FIRE_SPARKS* sptr;

	sptr = &fire_spark[0];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = (GetRandomControl() & 0x3F) + 96;
	sptr->sB = 64;
	sptr->dR = sptr->sR;
	sptr->dG = sptr->sG;
	sptr->dB = sptr->sB;
	sptr->ColFadeSpeed = 1;
	sptr->FadeToBlack = 0;
	sptr->Life = 8;
	sptr->sLife = 8;
	sptr->x = (GetRandomControl() & 7) - 4;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 7) - 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Friction = 0;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = SF_NONE;
	sptr->dSize = (GetRandomControl() & 0x1F) + 0x80;
	sptr->sSize = sptr->dSize;
	sptr->Size = sptr->dSize;
}

void ClearFires()
{
	FIRE_LIST* fire;

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];
		fire->on = 0;
	}
}

void keep_those_fires_burning()
{
	TriggerGlobalStaticFlame();

	if (!(wibble & 0xF))
	{
		TriggerGlobalFireFlame();

		if (!(wibble & 0x1F))
			TriggerGlobalFireSmoke();
	}
}

void UpdateFireSparks()
{
	FIRE_SPARKS* sptr;
	long fade;

	keep_those_fires_burning();

	for (int i = 0; i < 20; i++)
	{
		sptr = &fire_spark[i];

		if (!sptr->On)
			continue;

		sptr->Life--;

		if (!sptr->Life)
		{
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
			sptr->R = uchar((fade * sptr->dR) >> 16);
			sptr->G = uchar((fade * sptr->dG) >> 16);
			sptr->B = uchar((fade * sptr->dB) >> 16);

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

		if (sptr->Flags & SF_ROTATE)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		if (sptr->R < 24 && sptr->G < 24 && sptr->B < 24)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 2);
		else if (sptr->R < 80 && sptr->G < 80 && sptr->B < 80)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 1);
		else
			sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if (sptr->MaxYvel)
		{
			if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if (sptr->Friction)
		{
			sptr->Xvel -= sptr->Xvel >> sptr->Friction;
			sptr->Zvel -= sptr->Zvel >> sptr->Friction;
		}

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;
		sptr->Size = uchar(sptr->sSize + ((fade * (sptr->dSize - sptr->sSize)) >> 16));
	}
}

void TriggerFenceSparks(long x, long y, long z, long yv, long fric)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 192;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sR;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = sptr->sR >> 1;
	sptr->dB = sptr->sR >> 2;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 4 * (GetRandomControl() & 0xFF) - 512;
	sptr->Yvel = short((GetRandomControl() & 0xF) + (fric << 4) - (yv << 5) - 8);
	sptr->Zvel = 4 * (GetRandomControl() & 0xFF) - 512;

	if (fric)
		sptr->Friction = 5;
	else
		sptr->Friction = 4;

	sptr->Flags = SF_NONE;
	sptr->MaxYvel = 0;
	sptr->Gravity = short(((fric + 1) << 4) + (GetRandomControl() & 0xF));
}
