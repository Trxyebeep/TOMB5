#include "../tomb5/pch.h"
#include "subsuit.h"
#include "control.h"
#include "minisub.h"
#include "sound.h"
#include "delstuff.h"
#include "camera.h"
#include "items.h"
#include "objects.h"
#include "effect2.h"

char BVols[8] =
{
	4, 5, 6, 8, 11, 14, 18, 23
};

short BreathCount = 0;
short BreathDelay = 16;
short ZeroStressCount = 0;

void DoSubsuitStuff()
{
	PHD_VECTOR src, target;
	long pitch, anx, vol;

	if (CheckCutPlayed(40))
		TriggerAirBubbles();

	if (SubHitCount)
		SubHitCount -= 1;

	anx = lara.Anxiety;

	if (lara.Anxiety > 127)
		anx = 127;

	if (++BreathCount >= BreathDelay)
	{
		pitch = ((anx & 0x70) << 8) + 0x8000;
		anx &= 0x70;
		vol = BVols[anx >> 4];
		ZeroStressCount = (short)anx;

		if (lara_item->hit_points > 0)
			SoundEffect(SFX_LARA_SUB_BREATHE, 0, (pitch << 8) | (vol << 8) | SFX_ALWAYS | SFX_SETPITCH | SFX_SETVOL);

		BreathCount = (short)(-40 - (30 * (128 - anx) >> 7));

		if (lara.Anxiety)
		{
			anx = lara.Anxiety;

			if (anx > 128)

				anx -= 16;
			else
				anx -= 4;

			if (anx < 0)
				anx = 0;

			lara.Anxiety = (uchar)anx;
			BreathDelay = 0;
		}
		else if (BreathDelay < 16)
			BreathDelay += 2;
	}

	src.x = 0;
	src.y = -1024;
	src.z = -128;
	GetLaraJointPos(&src, 7);
	target.x = 0;
	target.y = -20480;
	target.z = -128;
	GetLaraJointPos(&target, 7);
	LaraTorch(&src, &target, lara_item->pos.y_rot, 255);
	TriggerEngineEffects();

	if (lara.ChaffTimer)
		lara.ChaffTimer--;

	if (dbinput & IN_SPRINT && !lara.ChaffTimer)
		FireChaff();
}

void FireChaff()
{
	short item_number;
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	int height;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;

	if (lara.puzzleitems[0])
	{
		item_number = CreateItem();

		if (item_number != NO_ITEM)
		{
			item = &items[item_number];
			SoundEffect(SFX_UNDERWATER_CHAFF, &lara_item->pos, SFX_ALWAYS);
			item->object_number = CHAFF;
			item->shade = -15856;
			--lara.puzzleitems[0];
			pos.x = 0;
			pos.y = -112;
			pos.z = -112;
			GetLaraJointPos(&pos, 7);
			item->room_number = lara_item->room_number;
			floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
			height = GetHeight(floor, pos.x, pos.y, pos.z);

			if (height >= pos.y)
			{
				item->pos.x_pos = pos.x;
				item->pos.y_pos = pos.y;
				item->pos.z_pos = pos.z;
			}
			else
			{
				item->pos.x_pos = lara_item->pos.x_pos;
				item->pos.y_pos = pos.y;
				item->pos.z_pos = lara_item->pos.z_pos;
				item->room_number = lara_item->room_number;
			}

			InitialiseItem(item_number);
			item->pos.x_rot = 0;
			item->pos.y_rot = lara_item->pos.y_rot - 0x8000;
			item->pos.z_rot = 0;
			item->speed = 32;
			item->fallspeed = -128;
			AddActiveItem(item_number);
			lara.ChaffTimer = -116;

			for (int i = 0; i < 8; i++)
			{
				pos.x = 0;
				pos.y = (GetRandomControl() & 0x1F) - 128;
				pos.z = -112;
				GetLaraJointPos(&pos, 7);
				pos2.x = (GetRandomControl() & 0xFF) - 128;
				pos2.y = GetRandomControl() & (((i + 1) << 7) - 1);
				pos2.z = -112 - (GetRandomControl() & (((i + 1) << 6) - 1));
				GetLaraJointPos(&pos2, 7);
				TriggerTorpedoSteam(&pos, &pos2, 1);
			}
		}
	}
}

void TriggerAirBubbles()
{
	SPARKS* sptr;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	long size;

	pos1.x = 0;
	pos1.y = -192;
	pos1.z = -160;
	GetLaraJointPos(&pos1, 7);
	pos2.x = 0;
	pos2.y = -192;
	pos2.z = -512 - (GetRandomControl() & 0x7F);
	GetLaraJointPos(&pos2, 7);
	sptr = &spark[GetFreeSpark()];
	sptr->sR = 32;
	sptr->sG = 32;
	sptr->sB = 32;
	sptr->On = 1;
	sptr->dR = -96;
	sptr->dG = -96;
	sptr->dB = -96;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 6;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 16;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + pos1.x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos1.y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos1.z - 16;
	sptr->Xvel = (short)(pos2.x - pos1.x + ((GetRandomControl() & 127) - 64));
	sptr->Yvel = (short)(pos2.y - pos1.y + ((GetRandomControl() & 127) - 64));
	sptr->Zvel = (short)(pos2.z - pos1.z + ((GetRandomControl() & 127) - 64));
	sptr->Friction = 0; 
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 17;
	sptr->MaxYvel = 0;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->Scalar = 1;
	sptr->Flags = 26;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) - 8;
	size = 16 + (GetRandomControl() & 15);
	sptr->Size = (uchar)size;
	sptr->sSize = (uchar)size;
	sptr->dSize = (uchar)(size << 1);
}

void inject_subsuit(bool replace)
{
	INJECT(0x0047C6D0, FireChaff, replace);
	INJECT(0x0047C950, DoSubsuitStuff, replace);
	INJECT(0x0047C4D0, TriggerAirBubbles, replace);
}
