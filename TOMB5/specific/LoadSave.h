#pragma once
#include "../global/types.h"

void CheckKeyConflicts();
void DisplayStatsUCunt();
long DoLoadSave(long LoadSave);
long GetCampaignCheatValue();
void DoOptions();
void CreateMonoScreen();
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
extern char MonoScreenOn;
