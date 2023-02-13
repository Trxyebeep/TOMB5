#pragma once
#include "../global/types.h"

void CheckKeyConflicts();
void DisplayStatsUCunt();
void S_DrawAirBar(long pos);
void S_DrawHealthBar(long pos);
void S_DrawHealthBar2(long pos);
void S_DrawDashBar(long pos);
long DoLoadSave(long LoadSave);
long GetCampaignCheatValue();
void DoOptions();
void S_DrawEnemyBar(long pos);
void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2);
void CreateMonoScreen();
void S_InitLoadBar(long max);
void S_UpdateLoadBar();
long S_DrawLoadBar();
void S_LoadBar();
void RGBM_Mono(uchar* r, uchar* g, uchar* b);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface);
void FreeMonoScreen();
void S_DrawTile(long x, long y, long w, long h, IDirect3DTexture2* t, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3);
void S_DisplayMonoScreen();
long S_LoadSave(long load_or_save, long mono, long inv_active);
void LoadScreen(long screen, long pathNum);
void ReleaseScreen();
void DrawLoadingScreen();
long GetSaveLoadFiles();
void DoSlider(long x, long y, long width, long height, long pos, long c1, long c2, long c3);
long S_DisplayPauseMenu(long reset);
long S_PauseMenu();
void DoFrontEndOneShotStuff();
long FindSFCursor(long in, long selection);
void CalculateNumSpecialFeatures();
void SpecialFeaturesDisplayScreens(long num);
void DoSpecialFeaturesServer();

extern long sfx_frequencies[3];
extern long SoundQuality;
extern long MusicVolume;
extern long SFXVolume;
extern long ControlMethod;
extern MONOSCREEN_STRUCT MonoScreen[5];
extern char MonoScreenOn;
extern long loadbar_on;
