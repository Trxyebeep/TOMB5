#pragma once


void inject_specific();

void DoStatScreen();
void DisplayStatsUCunt();
#define DoLoadSave	( (int(__cdecl*)(int)) 0x004AD460 )
#define S_LoadGame	( (int(__cdecl*)(int)) 0x004A8E10 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x004AD290 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_CDLoop	( (void(__cdecl*)()) 0x004A7BD0 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x004A39D0 )
#define S_LoadSave	( (int(__cdecl*)(int, int)) 0x004B1120 )
#define S_PauseMenu	( (int(__cdecl*)()) 0x004B1030 )
#define	S_CDFade	( (void(__cdecl*)(int)) 0x00492AA0 )
#define	S_CDStop	( (void(__cdecl*)()) 0x004929E0 )
#define	S_CDPlay	( (void(__cdecl*)(short, int)) 0x00492990 )
#define S_CDLoop	( (void(__cdecl*)()) 0x004A7BD0 )
#define S_StartSyncedAudio	( (void(__cdecl*)(int)) 0x00492AC0 )
#define S_SoundStopSample	( (void(__cdecl*)(int)) 0x004A3A00 )
#define IsVolumetric	( (char(__cdecl*)()) 0x004BA1B0 )
#define	SetFogColor	( (void(__cdecl*)(char, char, char)) 0x004A84A0 )//r g b
#define S_DrawAirBar	( (void(__cdecl*)(int)) 0x004B18E0 )
#define S_DrawHealthBar	( (void(__cdecl*)(int)) 0x004B1950 )
#define S_DrawHealthBar2	( (void(__cdecl*)(int)) 0x004B19C0 )
#define S_DrawDashBar	( (void(__cdecl*)(int)) 0x004B1890 )
