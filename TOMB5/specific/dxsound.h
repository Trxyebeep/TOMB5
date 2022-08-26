#pragma once
#include "../global/vars.h"

void inject_dxsound(bool replace);

bool DXChangeOutputFormat(long nSamplesPerSec, bool force);
void DSChangeVolume(long num, long volume);
void DSAdjustPitch(long num, long pitch);
void DSAdjustPan(long num, long pan);
bool DXSetOutputFormat();
bool DXDSCreate();
bool InitSampleDecompress();
bool FreeSampleDecompress();
bool DXCreateSampleADPCM(char* data, long comp_size, long uncomp_size, long num);

#define S_SoundSampleIsPlaying	( (long(__cdecl*)(long)) 0x004A3B10 )
#define S_SoundSetPanAndVolume	( (void(__cdecl*)(long, short, ushort)) 0x004A3B40 )
#define S_SoundSetPitch	( (void(__cdecl*)(long, long)) 0x004A3B90 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_SoundStopSample	( (void(__cdecl*)(long)) 0x004A3A00 )
#define DSIsChannelPlaying	( (bool(__cdecl*)(long)) 0x004A3790 )
#define S_SoundPlaySample	( (long(__cdecl*)(long, ushort, long, short)) 0x004A3A20 )
#define S_SoundPlaySampleLooped	( (long(__cdecl*)(long, ushort, long, short)) 0x004A3A60 )
