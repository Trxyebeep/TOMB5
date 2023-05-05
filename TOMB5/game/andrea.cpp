#include "../tomb5/pch.h"
#include "andrea.h"
#include "control.h"
#include "switch.h"
#include "draw.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "effect2.h"
#include "items.h"
#include "spotcam.h"
#include "sound.h"
#include "../specific/function_stubs.h"
#include "laramisc.h"
#include "sphere.h"
#include "delstuff.h"
#include "effects.h"
#include "../specific/function_table.h"
#include "../specific/specificfx.h"
#include "tomb4fx.h"
#include "../specific/file.h"
#include "camera.h"
#include "lara.h"
#include "../specific/gamemain.h"

void ControlPropeller(short item_number)
{
	ITEM_INFO* item;
	short* bptr;
	long b[6];
	long Xaxis, Zaxis, dx1, dx2, dy, dz1, dz2, frame;
	short tb[6];

	item = &items[item_number];
	AnimateItem(item);

	if (!TriggerActive(item))
	{
		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);

		if (item->current_anim_state != 1)
			item->goal_anim_state = 1;
		else if (item->frame_number == anims[item->anim_number].frame_end)
			return;
	}

	if (item->current_anim_state == 1)
		frame = anims[item->anim_number].frame_end - item->frame_number;
	else
		frame = 128;

	bptr = GetBoundsAccurate(item);
	b[2] = item->pos.y_pos + bptr[2];
	b[3] = item->pos.y_pos + bptr[3];

	//Vertical?
	if (item->object_number == PROPELLER_V)
	{
		phd_PushUnitMatrix();
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		phd_SetTrans(0, 0, 0);
		mRotBoundingBoxNoPersp(bptr, tb);
		phd_PopMatrix();
		b[0] = item->pos.x_pos + tb[0];
		b[1] = item->pos.x_pos + tb[1];
		b[4] = item->pos.z_pos + tb[4];
		b[5] = item->pos.z_pos + tb[5];
		TriggerFanEffects(b, 1, 0, frame);
		TriggerFanEffects(b, -1, 0, frame);

		if (lara_item->pos.x_pos >= b[0] && lara_item->pos.x_pos <= b[1] && lara_item->pos.z_pos >= b[4] && lara_item->pos.z_pos <= b[5])
		{
			if (lara_item->pos.y_pos > b[3])
			{
				if (lara_item->pos.y_pos - b[3] >= item->item_flags[0])
					return;

				dy = 96 * (item->item_flags[0] - (lara_item->pos.y_pos - b[3])) / item->item_flags[0];
			}
			else
			{
				if (b[2] - lara_item->pos.y_pos >= item->item_flags[0])
					return;

				dy = 96 * (b[2] - lara_item->pos.y_pos - item->item_flags[0]) / item->item_flags[0];
			}

			if (item->current_anim_state == 1)
				dy = frame * dy / 120;

			lara_item->pos.y_pos += dy;
		}

		return;
	}

	//Horizontal
	switch (item->pos.y_rot)
	{
	case 0x4000:
		b[0] = item->pos.x_pos + bptr[4];
		b[1] = item->pos.x_pos + bptr[5];
		b[4] = item->pos.z_pos - bptr[1];
		b[5] = item->pos.z_pos - bptr[0];
		Xaxis = 0;
		Zaxis = 1;
		break;

	case -0x4000:
		b[0] = item->pos.x_pos - bptr[5];
		b[1] = item->pos.x_pos - bptr[4];
		b[4] = item->pos.z_pos + bptr[0];
		b[5] = item->pos.z_pos + bptr[1];
		Xaxis = 0;
		Zaxis = 1;
		break;

	case -0x8000:
		b[0] = item->pos.x_pos - bptr[1];
		b[1] = item->pos.x_pos - bptr[0];
		b[4] = item->pos.z_pos - bptr[5];
		b[5] = item->pos.z_pos - bptr[4];
		Xaxis = 1;
		Zaxis = 0;
		break;

	default:
		b[0] = item->pos.x_pos + *bptr;
		b[1] = item->pos.x_pos + bptr[1];
		b[4] = item->pos.z_pos + bptr[4];
		b[5] = item->pos.z_pos + bptr[5];
		Xaxis = 1;
		Zaxis = 0;
		break;
	}

	TriggerFanEffects(b, 2, item->pos.y_rot, frame);
	TriggerFanEffects(b, -2, item->pos.y_rot, frame);

	if (lara_item->pos.y_pos < b[2] || lara_item->pos.y_pos > b[3])
		return;

	if (Zaxis)
	{
		if (lara_item->pos.x_pos >= b[0] && lara_item->pos.x_pos <= b[1])
		{
			dz1 = abs(lara_item->pos.z_pos - b[4]);
			dz2 = abs(lara_item->pos.z_pos - b[5]);

			if (dz2 >= dz1)
				Zaxis = -Zaxis;
			else
				dz1 = dz2;

			if (dz1 < item->item_flags[0])
			{
				dy = 96 * Zaxis * (item->item_flags[0] - dz1) / item->item_flags[0];

				if (item->current_anim_state == 1)
					dy = frame * dy / 120;

				lara_item->pos.z_pos += dy;
			}
		}

		return;
	}
	
	//Xaxis
	if (lara_item->pos.z_pos >= b[4] && lara_item->pos.z_pos <= b[5])
	{
		dx1 = abs(lara_item->pos.x_pos - b[0]);
		dx2 = abs(lara_item->pos.x_pos - b[1]);

		if (dx2 >= dx1)
			Xaxis = -Xaxis;
		else
			dx1 = dx2;

		if (dx1 < item->item_flags[0])
		{
			dy = 96 * Xaxis * (item->item_flags[0] - dx1) / item->item_flags[0];

			if (item->current_anim_state == 1)
				dy = frame * dy / 120;

			lara_item->pos.x_pos += dy;
		}
	}
}

void TriggerFanEffects(long* b, long y, short angle, long rate)
{
	SPARKS* sptr;
	long dx, dy, dz;

	if (abs(y) == 1)
	{
		dx = (b[0] + b[1]) >> 1;
		dy = y < 0 ? b[2] : b[3];
		dz = (b[4] + b[5]) >> 1;
	}
	else
	{
		dy = (b[2] + b[3]) >> 1;

		if (angle & 0x7FFF)
		{
			dx = (b[0] + b[1]) >> 1;
			dz = y < 0 ? b[4] : b[5];
		}
		else
		{
			dx = y < 0 ? b[0] : b[1];
			dz = (b[4] + b[5]) >> 1;
		}
	}

	if (abs(camera.pos.x - dx) > 7168 || abs(camera.pos.y - dy) > 7168 || abs(camera.pos.z - dz) > 7168)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = uchar((48 * rate) >> 7);
	sptr->dG = sptr->dR;
	sptr->dB = uchar((rate * ((GetRandomControl() & 8) + 48)) >> 7);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;

	if (abs(y) == 1)
	{
		dx = GetRandomControl() % ((3 * (b[1] - b[0])) >> 3);
		dz = GetRandomControl() << 1;
		sptr->x = ((b[0] + b[1]) >> 1) + (dx * phd_sin(dz) >> W2V_SHIFT);
		sptr->y = y < 0 ? b[2] : b[3];
		sptr->z = ((b[4] + b[5]) >> 1) + (dx * phd_cos(dz) >> W2V_SHIFT);
		sptr->Xvel = 0;
		sptr->Yvel = short(32 * y * ((GetRandomControl() & 0x1F) + 224));
		sptr->Zvel = 0;
	}
	else
	{
		dx = GetRandomControl() % ((3 * (b[3] - b[2])) >> 3);
		dz = GetRandomControl() << 1;

		if (angle & 0x7FFF)
		{
			sptr->x = ((b[0] + b[1]) >> 1) + (dx * phd_cos(dz) >> W2V_SHIFT);
			sptr->y = ((b[3] + b[2]) >> 1) + (dx * phd_sin(dz) >> W2V_SHIFT);
			sptr->z = y < 0 ? b[4] : b[5];
			sptr->Xvel = 0;
			sptr->Zvel = short(y * ((GetRandomControl() & 0x1F) + 224) << 4);
		}
		else
		{
			sptr->x = y < 0 ? b[0] : b[1];
			sptr->y = ((b[3] + b[2]) >> 1) + (dx * phd_sin(dz) >> W2V_SHIFT);
			sptr->z = ((b[4] + b[5]) >> 1) + (dx * phd_cos(dz) >> W2V_SHIFT);
			sptr->Xvel = short(y * ((GetRandomControl() & 0x1F) + 224) << 4);
			sptr->Zvel = 0;
		}

		sptr->Yvel = 0;
	}

	sptr->Friction = 85;
	sptr->MaxYvel = 0;
	sptr->Xvel = short(rate * sptr->Xvel >> 7);
	sptr->Yvel = short(rate * sptr->Yvel >> 7);
	sptr->Zvel = short(rate * sptr->Zvel >> 7);
	sptr->Gravity = 0;
	sptr->Flags = SF_NONE;
}

void ControlRaisingCog(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* triggered;
	short TriggerItems[4];
	short NumTrigs, vol;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->item_flags[0] >= 3)
	{
		AnimateItem(item);
		return;
	}

	if (item->item_flags[2] >= 256)
	{
		item->item_flags[2] = 0;
		item->item_flags[0]++;

		if (item->item_flags[0] == 3)
		{
			for (NumTrigs = (short)GetSwitchTrigger(item, TriggerItems, 1); NumTrigs; NumTrigs--)
			{
				triggered = &items[TriggerItems[NumTrigs - 1]];

				if (triggered->object_number == PULLEY)
				{
					if (triggered->room_number == item->item_flags[1])
					{
						triggered->item_flags[1] = 0;
						triggered->flags |= IFL_TRIGGERED;
					}
					else
						triggered->item_flags[1] = 1;
				}
				else if (triggered->object_number == TRIGGER_TRIGGERER)
				{
					AddActiveItem(TriggerItems[NumTrigs - 1]);
					triggered->status = ITEM_ACTIVE;
					triggered->flags |= IFL_CODEBITS;
				}
			}
		}

		RemoveActiveItem(item_number);
		item->status = ITEM_INACTIVE;
		item->flags &= ~IFL_CODEBITS;
	}
	else
	{
		if (!item->item_flags[2])
		{
			InitialiseSpotCam(9);
			bUseSpotCam = 1;
		}

		if (item->item_flags[2] < 31)
			vol = item->item_flags[2];
		else if (item->item_flags[2] > 224)
			vol = 255 - item->item_flags[2];
		else
			vol = 31;

		SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, (vol << 8) | SFX_SETVOL);
		item->item_flags[2] += 2;
		item->pos.y_pos -= 2;
	}
}

void ControlPortalDoor(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long x;

	item = &items[item_number];
	SoundEffect(SFX_PORTAL_LOOP, &item->pos, SFX_DEFAULT);

	if (!item->item_flags[0])
	{
		SoundEffect(SFX_GOD_HEAD_BLAST, &item->pos, SFX_SETPITCH | 0x800000);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_SETPITCH | 0x1400000);
		SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_SETPITCH | 0x400000);
		item->item_flags[0]++;
	}
	else if (item->item_flags[0] < 12)
	{
		if (FlashFader > 2)
			FlashFader = (FlashFader >> 1) & ~1;

		if (GlobalCounter & 1)
		{
			FlashFadeR = 192;
			FlashFadeG = (GetRandomControl() & 0x1F) + 160;
			FlashFadeB = GetRandomControl() & 0x7F;
			FlashFader = 32;
			item->item_flags[0]++;
		}
	}
	else if (item->item_flags[0] == 12)
	{
		SoundEffect(SFX_GOD_HEAD_BLAST, &item->pos, SFX_SETPITCH | 0x400000);
		FlashFadeR = 192;
		FlashFadeG = (GetRandomControl() & 0x1F) + 160;
		FlashFadeB = 0;
		FlashFader = 32;
		item->item_flags[0]++;
	}

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x7F) + 64;
	sptr->dG = (GetRandomControl() & 0x7F) + 64;
	sptr->dB = (GetRandomControl() & 0x7F) + 64;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	x = (GetRandomControl() & 0x3F) - 32;
	sptr->x = item->pos.x_pos + x;
	sptr->y = item->pos.y_pos - (GetRandomControl() & 0x3FF);
	sptr->z = item->pos.z_pos + ((GetRandomControl() & 0x3FF) - 512);
	sptr->Xvel = short(x << 3);
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 6;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Flags = SF_DEF | SF_SCALE;
	sptr->Size = (GetRandomControl() & 3) + 8;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size;
	sptr->Scalar = 1;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
}

void ControlGenSlot1(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	long hit;
	short frame;

	item = &items[item_number];

	if (!TriggerActive(item) || !item->trigger_flags)
		return;

	frame = item->frame_number - anims[item->anim_number].frame_base;

	if (frame == 10 || frame == 11)
	{
		GetLaraDeadlyBounds();
		DeadlyBounds[0] -= 350;
		DeadlyBounds[1] += 350;
		DeadlyBounds[4] -= 350;
		DeadlyBounds[5] += 350;
		hit = 0;

		for (int i = 1; i < 7; i++)
		{
			pos.x = 0;
			pos.y = -350;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, i);

			if (pos.x > DeadlyBounds[0] && pos.x < DeadlyBounds[1] && pos.y > DeadlyBounds[2] &&
				pos.y < DeadlyBounds[3] && pos.z > DeadlyBounds[4] && pos.z < DeadlyBounds[5])
				hit = 1;
		}

		if (hit)
		{
			for (int i = 7; i < 15; i++)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetLaraJointPos(&pos, i);

				for (int j = 0; j < 5; j++)
					DoBloodSplat(GetRandomControl() + pos.x - 128, (GetRandomControl() & 0xFF) + pos.y - 128, (GetRandomControl() & 0xFF) + pos.z - 128, 1, -1, lara_item->room_number);
			}

			lara_item->hit_points = 0;
		}
	}

	AnimateItem(item);
}

void ControlRaisingPlinth(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	short item_number2, vol;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->item_flags[0] == 4)
		{
			item->item_flags[1]--;

			if (!item->item_flags[1])
			{
				item2 = &items[item->item_flags[3] & 0xFF];
				item2->flags = (item2->flags & ~IFL_CODEBITS) | IFL_TRIGGERED;
				item->item_flags[0] = 6;
				item->item_flags[1] = 768;
				TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			}
		}
		else if (item->item_flags[0] == 6)
		{
			item->item_flags[1] -= 8;
			vol = item->item_flags[1];

			if (item->item_flags[1] >= 0)
			{
				if (item->item_flags[1] < 256)
					vol = vol >> 3;
				else
				{
					if (item->item_flags[1] < 513)
						vol = 31;
					else
						vol = (768 - vol) >> 3;
				}

				SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, (vol << 8) | SFX_SETVOL);
				item->pos.y_pos += 8;
				item2 = &items[item->item_flags[3] >> 8];
				item2->flags |= IFL_TRIGGERED;
				item2->pos.y_pos = item->pos.y_pos - 560;
			}
			else if (item->item_flags[1] < -60)
			{
				item2 = &items[item->item_flags[2] & 0xFF];
				item2->item_flags[1] = 0;
				item2->flags |= IFL_TRIGGERED;
				item->item_flags[0] = 0;
				item->item_flags[1] = 0;
				RemoveActiveItem(item_number);
				item->flags &= ~IFL_CODEBITS;
				item->status = 0;
			}
		}
	}
	else if (item->item_flags[0] < 3)
	{
		vol = item->item_flags[1];

		if (vol < 256)
		{
			if (vol > 30)
			{
				if (vol < 225)
					vol = 31;
				else
					vol = 255 - vol;
			}

			SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, (vol << 8) | SFX_SETVOL);
			item->item_flags[1] += 16;
			item->pos.y_pos -= 16;
			item2 = &items[item->item_flags[3] >> 8];
			item2->flags |= IFL_TRIGGERED;
			item2->pos.y_pos = item->pos.y_pos - 560;
		}
		else
		{
			item->item_flags[1] = 0;
			item->item_flags[0]++;

			if (item->item_flags[0] == 3)
			{
				item->item_flags[1] = item->trigger_flags * 0x1E;
				item->item_flags[0] = 4;
				item_number2 = item->item_flags[3] & 0xFF;
				item2 = &items[item_number2];
				AddActiveItem(item_number2);
				item2->flags |= (IFL_CODEBITS | IFL_TRIGGERED);
				item2->status = 1;
				item2 = &items[item->item_flags[2] & 0xFF];
				item2->item_flags[1] = 1;
				item2->flags |= IFL_TRIGGERED;
				item->flags &= ~IFL_CODEBITS;
			}
			else
			{
				RemoveActiveItem(item_number);
				item->flags &= ~IFL_CODEBITS;
				item->status = 0;
			}
		}
	}
	else if (item->item_flags[0] == 4)
	{
		item->item_flags[0] = 5;
		item->item_flags[1] = 0;
	}
	else if (item->item_flags[0] == 5 && !item->item_flags[1] && (items[item->item_flags[3] >> 8].flags & IFL_CLEARBODY) != 0)
	{
		FlipMap(3);
		flipmap[3] ^= IFL_CODEBITS;
		item->item_flags[1] = 1;
	}
}

void DrawPortalDoor(ITEM_INFO* item)
{
	PORTAL_STRUCT* portal;
	FVECTOR* p;
	CVECTOR* rgb;
	CVECTOR* orgb;
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT tex;
	D3DTLVERTEX v[64];
	D3DTLVERTEX vtx[4];
	FVECTOR points[64];
	short* rand;
	float fx, fy, fz, zv, y, z, s;
	long r, g, b;
	short clip[64];
	short c;

	if (!TriggerActive(item))
		return;

	portal = (PORTAL_STRUCT*)item->data;
	rand = portal->Rand;
	rgb = portal->rgb;
	orgb = portal->orgb;
	p = points;

	for (int i = 0; i < 8; i++)
	{
		y = float(i * 146 - 1024);

		for (int j = 0; j < 8; j++)
		{
			s = fSin((rand[0] << 11) + (GlobalCounter << 11)) * 32;
			z = float(j * 146 - 512);

			if (!i || i == 7 || !j || j == 7)
			{
				//edge vertices
				p->x = 0;
				p->y = y;
				p->z = z;
				r = 0;
				g = 0;
				b = 0;
			}
			else
			{
				p->x = s;

				if ((i & 1) != (j & 1))
				{
					p->y = y - (s / 2);
					p->z = z + s;
				}
				else
				{
					p->y = y + (s / 2);
					p->z = z - s;
				}

				r = orgb->r - (long)s;
				g = orgb->g;
				b = orgb->b + (long)s;

				r -= r >> 2;
				g -= g >> 2;
				b -= b >> 2;

				if (r < 0)
					r = 0;

				if (g < 0)
					g = 0;

				if (b < 0)
					b = 0;
			}

			rgb->r = (char)r;
			rgb->g = (char)g;
			rgb->b = (char)b;

			p++;
			rand++;
			rgb++;
			orgb++;
		}
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	for (int i = 0; i < 64; i++)
	{
		p = &points[i];
		rgb = &portal->rgb[i];

		fx = aMXPtr[M00] * p->x + aMXPtr[M01] * p->y + aMXPtr[M02] * p->z + aMXPtr[M03];
		fy = aMXPtr[M10] * p->x + aMXPtr[M11] * p->y + aMXPtr[M12] * p->z + aMXPtr[M13];
		fz = aMXPtr[M20] * p->x + aMXPtr[M21] * p->y + aMXPtr[M22] * p->z + aMXPtr[M23];

		zv = f_persp / fz;
		v[i].sx = fx * zv + f_centerx;
		v[i].sy = fy * zv + f_centery;
		v[i].tu = fx;
		v[i].tv = fy;
		v[i].sz = fz;
		v[i].rhw = f_mpersp / fz * f_moneopersp;
		v[i].color = RGBA(rgb->r, rgb->g, rgb->b, 0xFF);
		v[i].specular = 0xFF000000;

		if (v[i].sz < f_mznear)
			c = -128;
		else
		{
			c = 0;

			if (v[i].sx < f_left)
				c++;
			else if (v[i].sx > f_right)
				c += 2;

			if (v[i].sy < f_top)
				c += 4;
			else if (v[i].sy > f_bottom)
				c += 8;
		}

		clip[i] = c;
	}

	phd_PopMatrix();

	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 1];
	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;

	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;

	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;

	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;

	for (int i = 0; i < 64 - 9; i++)
	{
		if ((i & 7) == 7)
			continue;

		for (int j = 0; j < 4; j++)
		{
			if (!j)
				c = 0;
			else if (j == 1)
				c = 1;
			else if (j == 2)
				c = 9;
			else if (j == 3)
				c = 8;

			vtx[j] = v[i + c];
			clipflags[j] = clip[i + c];
		}

		AddQuadSorted(vtx, 0, 1, 2, 3, &tex, 1);
	}
}
