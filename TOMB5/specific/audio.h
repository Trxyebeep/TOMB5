#pragma once
#include "../global/vars.h"

void inject_audio(bool replace);

void S_CDPlay(short track, long mode);

#define	S_CDFade	( (void(__cdecl*)(long)) 0x00492AA0 )
#define	S_CDStop	( (void(__cdecl*)()) 0x004929E0 )
#define S_StartSyncedAudio	( (void(__cdecl*)(long)) 0x00492AC0 )
#define ACMSetVolume	( (void(__cdecl*)()) 0x00492AF0 )
#define ACMEmulateCDPlay    ( (void(__cdecl*)(short, long)) 0x00493760 )
#define ACMClose	( (void(__cdecl*)()) 0x004931A0 )
