#pragma once
#include "../global/vars.h"


void inject_LoadSave(bool replace);

void CheckKeyConflicts();
void DoStatScreen();
void DisplayStatsUCunt();
void S_DrawAirBar(int pos);
void S_DrawHealthBar(int pos);
void S_DrawHealthBar2(int pos);
void S_DrawDashBar(int pos);
int DoLoadSave(int LoadSave);
void S_MemSet(void* p, int val, size_t sz);
int GetCampaignCheatValue();
void DoOptions();

#define CreateMonoScreen	( (void(__cdecl*)()) 0x004AC430 )
#define S_DisplayMonoScreen	( (void(__cdecl*)()) 0x004AD010 )
#define FreeMonoScreen	( (void(__cdecl*)()) 0x004AC460 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x004AD290 )
#define S_PauseMenu	( (int(__cdecl*)()) 0x004B1030 )
#define S_LoadSave	( (int(__cdecl*)(int, int)) 0x004B1120 )
#define DoSpecialFeaturesServer	( (void(__cdecl*)()) 0x004B1D90 )
#define DoBar	( (void(__cdecl*)(int, int, int, int, int, long, long)) 0x004B1250 )
#define DoFrontEndOneShotStuff	( (void(__cdecl*)()) 0x004B2090 )
#define IsHardware	( (int(__cdecl*)()) 0x004B1E90 )
#define IsSuperLowRes	( (int(__cdecl*)()) 0x004B1EB0 )
#define FindSFCursor	( (long(__cdecl*)(long, long)) 0x004ABA60 )
#define DoSlider	( (void(__cdecl*)(long, long, long, long, long, long, long, long)) 0x004AD820 )
#define CalculateNumSpecialFeatures	( (void(__cdecl*)()) 0x004AB9F0 )
