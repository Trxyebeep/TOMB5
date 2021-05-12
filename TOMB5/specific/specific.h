#pragma once
#include "../global/vars.h"


void inject_specific();

#define S_LoadGame	( (int(__cdecl*)(int)) 0x004A8E10 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_CDLoop	( (void(__cdecl*)()) 0x004A7BD0 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define	S_CDFade	( (void(__cdecl*)(int)) 0x00492AA0 )
#define	S_CDStop	( (void(__cdecl*)()) 0x004929E0 )
#define	S_CDPlay	( (void(__cdecl*)(short, int)) 0x00492990 )
#define S_SetReverbType	( (void(__cdecl*)(int)) 0x004A7B90 )//empty
#define S_StartSyncedAudio	( (void(__cdecl*)(int)) 0x00492AC0 )
#define S_SoundStopSample	( (void(__cdecl*)(int)) 0x004A3A00 )
#define IsVolumetric	( (char(__cdecl*)()) 0x004BA1B0 )
#define	SetFogColor	( (void(__cdecl*)(char, char, char)) 0x004A84A0 )//r g b
