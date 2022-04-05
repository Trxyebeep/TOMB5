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
#ifdef ENEMY_BARS
void S_DrawEnemyBar(long pos);
#endif
void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2);
void CreateMonoScreen();
void S_InitLoadBar(long max);
void S_UpdateLoadBar();
long S_DrawLoadBar();
void S_LoadBar();
void MemBltSurf(void* dest, long x, long y, long w, long h, long dadd, void* source, long x2, long y2, DDSURFACEDESC2 surface, float xsize, float ysize);
void RGBM_Mono(uchar* r, uchar* g, uchar* b);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface);
void FreeMonoScreen();
void S_DrawTile(long x, long y, long w, long h, IDirect3DTexture2* t, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3);
void S_DisplayMonoScreen();
long S_LoadSave(long load_or_save, long mono);
void LoadScreen(long screen, long pathNum);
void ReleaseScreen();
void DrawLoadingScreen();

#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x004AD290 )
#define S_PauseMenu	( (int(__cdecl*)()) 0x004B1030 )
#define DoSpecialFeaturesServer	( (void(__cdecl*)()) 0x004B1D90 )
#define DoFrontEndOneShotStuff	( (void(__cdecl*)()) 0x004B2090 )
#define IsHardware	( (int(__cdecl*)()) 0x004B1E90 )
#define IsSuperLowRes	( (int(__cdecl*)()) 0x004B1EB0 )
#define FindSFCursor	( (long(__cdecl*)(long, long)) 0x004ABA60 )
#define DoSlider	( (void(__cdecl*)(long, long, long, long, long, long, long, long)) 0x004AD820 )
#define CalculateNumSpecialFeatures	( (void(__cdecl*)()) 0x004AB9F0 )
