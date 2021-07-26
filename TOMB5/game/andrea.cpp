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

		if (item->current_anim_state == 1)
		{
			if (item->frame_number == anims[item->anim_number].frame_end)
				return;
		}
		else
			item->goal_anim_state = 1;
	}

	if (item->current_anim_state == 1)
		frame = anims[item->anim_number].frame_end - item->frame_number;
	else
		frame = 128;

	bptr = GetBoundsAccurate(item);
	b[2] = item->pos.y_pos + bptr[2];
	b[3] = item->pos.y_pos + bptr[3];

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
			return;
		}
	}
	else
	{
		switch (item->pos.y_rot)
		{
		case 16384:
			b[0] = item->pos.x_pos + bptr[4];
			b[1] = item->pos.x_pos + bptr[5];
			b[4] = item->pos.z_pos - bptr[1];
			b[5] = item->pos.z_pos - bptr[0];
			Xaxis = 0;
			Zaxis = 1;
			break;

		case -16384:
			b[0] = item->pos.x_pos - bptr[5];
			b[1] = item->pos.x_pos - bptr[4];
			b[4] = item->pos.z_pos + bptr[0];
			b[5] = item->pos.z_pos + bptr[1];
			Xaxis = 0;
			Zaxis = 1;
			break;

		case -32768:
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
			Xaxis = 1;//some [redacted] [redacted] decided to set this to 1 at the beginning of the function. moved here where it [redacted] belongs.. :)
			Zaxis = 0;
			break;
		}

		TriggerFanEffects(b, 2, item->pos.y_rot, frame);
		TriggerFanEffects(b, -2, item->pos.y_rot, frame);

		if (lara_item->pos.y_pos >= b[2] && lara_item->pos.y_pos <= b[3])
		{
			if (Zaxis)
			{
				if (lara_item->pos.x_pos >= b[0] && lara_item->pos.x_pos <= b[1])
				{
					dz1 = ABS(lara_item->pos.z_pos - b[4]);
					dz2 = ABS(lara_item->pos.z_pos - b[5]);

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
			}
			else
			{
				if (lara_item->pos.z_pos >= b[4] && lara_item->pos.z_pos <= b[5])
				{
					dx1 = ABS(lara_item->pos.x_pos - b[0]);
					dx2 = ABS(lara_item->pos.x_pos - b[1]);

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
		}
	}
}

void TriggerFanEffects(long* b, long y, short angle, long rate)
{
	SPARKS* sptr;
	long dx, dy, dz;

	if (ABS(y) == 1)
	{
		dx = (b[0] + b[1]) >> 1;

		if (y >= 0)
			dy = b[3];
		else
			dy = b[2];

		dz = (b[4] + b[5]) >> 1;
	}
	else
	{
		dy = (b[2] + b[3]) >> 1;

		if (angle & 0x7FFF)
		{

			if (y >= 0)
				dz = b[5];
			else
				dz = b[4];

			dx = (b[0] + b[1]) >> 1;
		}
		else
		{
			if (y >= 0)
				dx = b[1];
			else
				dx = b[0];

			dz = (b[4] + b[5]) >> 1;
		}
	}

	if (ABS(camera.pos.x - dx) <= 7168 && ABS(camera.pos.y - dy) <= 7168 && ABS(camera.pos.z - dz) <= 7168)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = (uchar)((48 * rate) >> 7);
		sptr->dG = (uchar)((48 * rate) >> 7);
		sptr->dB = (uchar)((rate * ((GetRandomControl() & 8) + 48)) >> 7);
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 20;
		sptr->sLife = sptr->Life;

		if (ABS(y) == 1)
		{
			dx = (3 * (b[1] - b[0])) >> 3;
			dx = GetRandomControl() % dx;
			dz = GetRandomControl() << 1;
			sptr->x = ((b[0] + b[1]) >> 1) + ((phd_sin(dz) * dx) >> 14);
			sptr->z = ((phd_cos(dz) * dx) >> 14) + ((b[4] + b[5]) >> 1);

			if (y >= 0)
				sptr->y = b[3];
			else
				sptr->y = b[2];

			sptr->Zvel = 0;
			sptr->Xvel = 0;
			sptr->Yvel = (short)(32 * y * ((GetRandomControl() & 0x1F) + 224));
		}
		else
		{
			dx = (3 * (b[3] - b[2])) >> 3;
			dx = GetRandomControl() % dx;
			dz = GetRandomControl() << 1;
			sptr->y = (b[3] + b[2]) >> 1;

			if (angle & 0x7FFF)
			{
				if (y >= 0)
					sptr->z = b[5];
				else
					sptr->z = b[4];

				sptr->x = (b[0] + b[1]) >> 1;
				sptr->y += (dx * phd_sin(dz)) >> 14;
				sptr->x += (dx * phd_cos(dz)) >> 14;
				sptr->Xvel = 0;
				sptr->Zvel = (short)(y * ((GetRandomControl() & 0x1F) + 224) << 4);
			}
			else
			{
				if (y >= 0)
					sptr->x = b[1];
				else
					sptr->x = b[0];

				sptr->z = (b[4] + b[5]) >> 1;
				sptr->y += (dx * phd_sin(dz)) >> 14;
				sptr->z += (dx * phd_cos(dz)) >> 14;
				sptr->Zvel = 0;
				sptr->Xvel = (short)(y * ((GetRandomControl() & 0x1F) + 224) << 4);
			}

			sptr->Yvel = 0;
		}

		sptr->Friction = 85;
		sptr->MaxYvel = 0;
		sptr->Xvel = (short)((rate * sptr->Xvel) >> 7);
		sptr->Yvel = (short)((rate * sptr->Yvel) >> 7);
		sptr->Zvel = (short)((rate * sptr->Zvel) >> 7);
		sptr->Gravity = 0;
		sptr->Flags = 0;
	}
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
		AnimateItem(item);
	else
	{
		if (item->item_flags[2] >= 256)
		{
			item->item_flags[2] = 0;
			item->item_flags[0]++;

			if (item->item_flags[0] == 3)
			{
				for (NumTrigs = GetSwitchTrigger(item, TriggerItems, 1); NumTrigs; NumTrigs--)
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

			if (item->item_flags[2] >= 31)
			{
				if (item->item_flags[2] <= 224)
					vol = 31;
				else
					vol = 255 - item->item_flags[2];
			}
			else
				vol = item->item_flags[2];

			SoundEffect(SFX_BLK_PLAT_RAISE_LOW, &item->pos, (vol << 8) | SFX_SETVOL);
			item->item_flags[2] += 2;
			item->pos.y_pos -= 2;
		}
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
	else
	{
		if (item->item_flags[0] < 12)
		{
			if (FlashFader > 2)
			{
				FlashFader >>= 1;
				FlashFader &= ~1;
			}

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
	}

	sptr = &spark[GetFreeSpark()];
	x = (GetRandomControl() & 0x3F) - 32;
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
	sptr->x = x + item->pos.x_pos;
	sptr->y = -(GetRandomControl() & 0x3FF) + item->pos.y_pos;
	sptr->z = ((GetRandomControl() & 0x3FF) - 512) + item->pos.z_pos;
	sptr->Xvel = (short)(x << 3);
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 6;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Flags = 10;
	sptr->dSize = (GetRandomControl() & 3) + 8;
	sptr->sSize = sptr->dSize;
	sptr->Size = sptr->dSize;
	sptr->Scalar = 1;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
}

void inject_andrea(bool replace)
{
	INJECT(0x00405610, ControlPropeller, replace);
	INJECT(0x00405C00, TriggerFanEffects, replace);
	INJECT(0x00406040, ControlRaisingCog, replace);
	INJECT(0x004062B0, ControlPortalDoor, replace);
}
