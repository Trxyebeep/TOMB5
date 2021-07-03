#pragma once
#include "../global/vars.h"

#define CDDA_SetMasterVolume	( (void(__cdecl*)(int)) 0x004A7BB0 )
#define S_SoundSampleIsPlaying	( (int(__cdecl*)(int)) 0x004A3B10 )
#define S_SoundSetPanAndVolume	( (void(__cdecl*)(int, short, ushort)) 0x004A3B40 )
#define S_SoundSetPitch	( (void(__cdecl*)(int, int)) 0x004A3B90 )
#define GetPanVolume	( (void(__cdecl*)(SoundSlot*)) 0x00478D30 )