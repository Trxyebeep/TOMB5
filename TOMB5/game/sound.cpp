#include "../tomb5/pch.h"
#include "sound.h"
#include "../specific/function_stubs.h"

void SoundEffectCS(long sfx, PHD_3DPOS* pos, long flags)
{
	long bak;

	bak = GLOBAL_playing_cutseq;
	GLOBAL_playing_cutseq = 0;
	SoundEffect(sfx, pos, flags);
	GLOBAL_playing_cutseq = bak;
}

void SayNo()
{
	long fx;

	fx = SFX_LARA_NO;

	if (Gameflow->Language == 1)
		fx = SFX_LARA_NO_FRENCH;
	else if (Gameflow->Language == 6)
		fx = SFX_LARA_NO_JAPANESE;

	SoundEffect(fx, 0, SFX_ALWAYS);
}

void SOUND_Init()
{
	S_SoundSetMasterVolume();

	for (int i = 0; i < 32; i++)
		LaSlot[i].nSampleInfo = -1;

	sound_active = 1;
}

void inject_sound(bool replace)
{
	INJECT(0x00479130, SoundEffectCS, replace);
	INJECT(0x004790E0, SayNo, replace);
	INJECT(0x004790A0, SOUND_Init, replace);
}
