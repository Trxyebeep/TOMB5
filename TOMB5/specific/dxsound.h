#pragma once
#include "../global/vars.h"

#define S_SoundSampleIsPlaying	( (int(__cdecl*)(int)) 0x004A3B10 )
#define S_SoundSetPanAndVolume	( (void(__cdecl*)(int, short, ushort)) 0x004A3B40 )
#define S_SoundSetPitch	( (void(__cdecl*)(int, int)) 0x004A3B90 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_SoundStopSample	( (void(__cdecl*)(int)) 0x004A3A00 )
