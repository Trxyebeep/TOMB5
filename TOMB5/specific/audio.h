#pragma once
#include "../global/vars.h"

void inject_audio(bool replace);

void S_CDPlay(long track, long mode);
void S_CDStop();
void S_CDFade(long n);
void S_StartSyncedAudio(long track);
void ACMSetVolume();

#define ACMEmulateCDPlay    ( (void(__cdecl*)(long, long)) 0x00493760 )
#define ACMClose	( (void(__cdecl*)()) 0x004931A0 )
