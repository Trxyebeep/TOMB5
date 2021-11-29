#pragma once
#include "../global/vars.h"

#define S_SoundSampleIsPlaying	( (int(__cdecl*)(int)) 0x004A3B10 )
#define S_SoundSetPanAndVolume	( (void(__cdecl*)(int, short, ushort)) 0x004A3B40 )
#define S_SoundSetPitch	( (void(__cdecl*)(int, int)) 0x004A3B90 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_SoundStopSample	( (void(__cdecl*)(int)) 0x004A3A00 )
#define DSIsChannelPlaying	( (bool(__cdecl*)(long)) 0x004A3790 )
#define DSChangeVolume	( (void(__cdecl*)(long, long)) 0x004A2F10 )
#define DXChangeOutputFormat	( (bool(__cdecl*)(long, bool)) 0x004A2E30 )
#define S_SoundPlaySample	( (long(__cdecl*)(long, ushort, long, short)) 0x004A3A20 )
#define S_SoundPlaySampleLooped	( (long(__cdecl*)(long, ushort, long, short)) 0x004A3A60 )
