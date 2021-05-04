#pragma once


void inject_LoadSave();

void CheckKeyConflicts();
void DoStatScreen();
void DisplayStatsUCunt();//actually in some other file

#define CreateMonoScreen	( (void(__cdecl*)()) 0x004AC430 )
#define S_DisplayMonoScreen	( (void(__cdecl*)()) 0x004AD010 )
#define FreeMonoScreen	( (void(__cdecl*)()) 0x004AC460 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x004AD290 )
#define DoLoadSave	( (int(__cdecl*)(int)) 0x004AD460 )
#define S_DrawAirBar	( (void(__cdecl*)(int)) 0x004B18E0 )
#define S_DrawHealthBar	( (void(__cdecl*)(int)) 0x004B1950 )
#define S_DrawHealthBar2	( (void(__cdecl*)(int)) 0x004B19C0 )
#define S_DrawDashBar	( (void(__cdecl*)(int)) 0x004B1890 )
#define DoOptions	( (void(__cdecl*)()) 0x004ADF90 )
#define S_PauseMenu	( (int(__cdecl*)()) 0x004B1030 )
#define S_LoadSave	( (int(__cdecl*)(int, int)) 0x004B1120 )
#define DoSpecialFeaturesServer	( (void(__cdecl*)()) 0x004B1D90 )

