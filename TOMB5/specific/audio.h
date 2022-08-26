#pragma once
#include "../global/vars.h"

void inject_audio(bool replace);

void S_CDPlay(short track, long mode);
void S_CDStop();
void S_CDFade();
void S_StartSyncedAudio(long track);

#define ACMSetVolume	( (void(__cdecl*)()) 0x00492AF0 )
#define ACMEmulateCDPlay    ( (void(__cdecl*)(short, long)) 0x00493760 )
#define ACMClose	( (void(__cdecl*)()) 0x004931A0 )
