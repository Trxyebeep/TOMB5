#include "../tomb5/pch.h"
#include "audio.h"

void S_CDPlay(short track, long mode)
{
    if (acm_ready)
    {
        audio_counter = 0;
        IsAtmospherePlaying = track == CurrentAtmosphere;
        S_CDStop();
        ACMEmulateCDPlay(track, mode);
    }
}

void inject_audio(bool replace)
{
    INJECT(0x00492990, S_CDPlay, replace);
}
