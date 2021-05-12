#include "../tomb5/pch.h"
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

void inject_effect2()
{
	INJECT(0x00432640, SoundEffects);
}
