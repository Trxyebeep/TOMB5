#include "../tomb5/pch.h"
#include "sound.h"

void SoundEffectCS(long sfx, PHD_3DPOS* pos, long flags)
{
	long bak;

	bak = GLOBAL_playing_cutseq;
	GLOBAL_playing_cutseq = 0;
	SoundEffect(sfx, pos, flags);
	GLOBAL_playing_cutseq = bak;
}

void inject_sound(bool replace)
{
	INJECT(0x00479130, SoundEffectCS, replace);
}
