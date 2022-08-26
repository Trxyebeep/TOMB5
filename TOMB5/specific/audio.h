#pragma once
#include "../global/vars.h"

void inject_audio(bool replace);

void S_CDPlay(long track, long mode);
void S_CDStop();
void S_CDFade(long n);
void S_StartSyncedAudio(long track);
void ACMSetVolume();
void OpenStreamFile(char* name);
void GetADPCMData();
void ACMEmulateCDPlay(long track, long mode);
BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);

#define ACMClose	( (void(__cdecl*)()) 0x004931A0 )
