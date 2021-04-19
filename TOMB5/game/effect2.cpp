#include "../tomb5/pch.h"
#include "../global/types.h"
#include "effect2.h"
#include "effects.h"
#include "sound.h"
#include "../specific/DS.h"
#include "../specific/specific.h"
#include "control.h"
#include "delstuff.h"
#include "camera.h"
#include "objects.h"
#include "items.h"

char breath_pitch_shifter[12] =
{
	4, 5, 6, 8, 0xB, 0xE, 0x12, 0x17, 0, 0, 0, 0
};

void SoundEffects()
{
	OBJECT_VECTOR* sound;
	sound = &sound_effects[0];

	for (int i = number_sound_effects; i > 0; --i, sound++)
	{
		if (flip_stats[((sound->flags & 1)
			+ (sound->flags & 2)
			+ 3 * (((sound->flags & 0x1F) >> 2) & 1)
			+ 5 * (((sound->flags & 0x1F) >> 4) & 1)
			+ 4 * (((sound->flags & 0x1F) >> 3) & 1))])
		{
			if (sound->flags & 0x40)
			{
				SoundEffect(sound->data, (PHD_3DPOS*)sound, 0);
				continue;
			}
		}
		else if (sound->flags & 0x80)
		{
			SoundEffect(sound->data, (PHD_3DPOS*)sound, 0);
			continue;
		}
	}

	if (flipeffect != -1)
		effect_routines[flipeffect](0);

	if (!sound_active)
		return;

	SoundSlot* slot;
	int j;

	for (j = 0, slot = &LaSlot[j]; j < 32; j++, slot++)
	{
		if (slot->nSampleInfo >= 0)
		{
			if ((sample_infos[slot->nSampleInfo].flags & 3) != 3)
			{
				if (S_SoundSampleIsPlaying(j) == 0)
					slot->nSampleInfo = -1;
				else
				{
					GetPanVolume(slot);
					S_SoundSetPanAndVolume(j, slot->nPan, slot->nVolume);
				}
			}
			else
			{
				if (!slot->nVolume)
				{
					S_SoundStopSample(j);
					slot->nSampleInfo = -1;
				}
				else
				{
					S_SoundSetPanAndVolume(j, slot->nPan, slot->nVolume);
					S_SoundSetPitch(j, slot->nPitch);
					slot->nVolume = 0;
				}
			}
		}
	}
}

void DoSubsuitStuff()
{
	static short anxiety2;
	static short timer1sub;
	static short timer2sub;
	char anx;
	int anx_mod;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;

	if (CheckCutPlayed(40))
		TriggerAirBubbles();

	if (SubHitCount)
		SubHitCount -= 1;

	anx = lara.Anxiety;

	if (lara.Anxiety > 127)
		anx = 127;

	if (++timer1sub >= timer2sub)
	{
		anxiety2 = anx & 0x70;
		if (lara_item->hit_points > 0)
			SoundEffect(SFX_LARA_SUB_BREATHE, 0, ((((anxiety2 + 128) << 8) | breath_pitch_shifter[anxiety2 >> 4]) << 8) | SFX_ALWAYS | SFX_SETPITCH | SFX_SETVOL);

		timer1sub = -40 - (30 * (128 - anxiety2) >> 7);

		if (lara.Anxiety)
		{
			if (lara.Anxiety <= 128)
				anx = -4;
			else
				anx = -16;

			anx_mod = (lara.Anxiety & 0xFF) + anx;

			if (anx_mod < 0)
				anx_mod = 0;

			lara.Anxiety = lara.Anxiety & 0xffffff00 | anx_mod & 0xff;
			timer2sub = 0;
		}
		else if (timer2sub < 16)
			timer2sub += 2;
	}

	pos.x = 0;
	pos.y = -1024;
	pos.z = -128;
	GetLaraJointPos(&pos, 7);
	pos2.x = 0;
	pos.y = -20480;
	pos.z = -128;
	GetLaraJointPos(&pos2, 7);
	LaraTorch(&pos, &pos2, lara_item->pos.y_rot, 255);
	TriggerEngineEffects();

	if (lara.ChaffTimer)
		lara.ChaffTimer -= 1;

	if (dbinput & IN_SPRINT)
		if (!lara.ChaffTimer)
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

			for (int i = 8, b = 63, c = 127; i; --i)
			{
				pos.x = 0;
				pos.y = (GetRandomControl() & 0x1F) - 128;
				pos.z = -112;
				GetLaraJointPos(&pos, 7);
				pos2.x = (GetRandomControl() & 0xFF) - 128;
				pos2.y = c & GetRandomControl();
				pos2.z = -112 - (b & GetRandomControl());
				GetLaraJointPos(&pos2, 7);
				TriggerTorpedoSteam(&pos, &pos2, 1);
			}
		}
	}
}

void inject_effect2()
{
	INJECT(0x00432640, SoundEffects);
	INJECT(0x0047C6D0, FireChaff);
	INJECT(0x0047C950, DoSubsuitStuff);
}
