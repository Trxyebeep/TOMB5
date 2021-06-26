#pragma once
#include "../global/vars.h"


void inject_LoadSave();

void CheckKeyConflicts();
void DoStatScreen();
void DisplayStatsUCunt();//actually in some other file
void S_DrawAirBar(int pos);
void S_DrawHealthBar(int pos);
void S_DrawHealthBar2(int pos);
void S_DrawDashBar(int pos);
int DoLoadSave(int LoadSave);
void S_MemSet(void* p, int val, size_t sz);

#define CreateMonoScreen	( (void(__cdecl*)()) 0x004AC430 )
#define S_DisplayMonoScreen	( (void(__cdecl*)()) 0x004AD010 )
#define FreeMonoScreen	( (void(__cdecl*)()) 0x004AC460 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x004AD290 )
#define DoOptions	( (void(__cdecl*)()) 0x004ADF90 )
#define S_PauseMenu	( (int(__cdecl*)()) 0x004B1030 )
#define S_LoadSave	( (int(__cdecl*)(int, int)) 0x004B1120 )
#define DoSpecialFeaturesServer	( (void(__cdecl*)()) 0x004B1D90 )
#define DoBar	( (void(__cdecl*)(int, int, int, int, int, long, long)) 0x004B1250 )
