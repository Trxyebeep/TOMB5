#pragma once
#include "../global/vars.h"

void inject_tomb4fx(bool replace);

int GetFreeBlood();
void UpdateBlood();
void TriggerBlood(int x, int y, int z, int a4, int num);
long LSpline(long x, long* knots, int nk);
LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments);
void LaraBubbles(ITEM_INFO* item);

#define UpdateBubbles	( (void(__cdecl*)()) 0x00483540 )
#define	TriggerRicochetSpark	( (void(__cdecl*)(GAME_VECTOR*, int, int, int)) 0x0042F060 )//pos, angle, size, unk
#define TriggerLaraDrips	( (void(__cdecl*)()) 0x00483F00 )
#define UpdateFadeClip	( (void(__cdecl*)()) 0x00483AC0 )
#define UpdateDrips	( (void(__cdecl*)()) 0x00483D90 )
#define UpdateGunShells	( (void(__cdecl*)()) 0x00482D80 )
#define UpdateFireSparks	( (void(__cdecl*)()) 0x004813B0 )
#define UpdateSmokeSparks	( (void(__cdecl*)()) 0x00481DD0 )
#define UpdateShockwaves	( (void(__cdecl*)()) 0x004849A0 )
#define UpdateLightning	( (void(__cdecl*)()) 0x00484CB0 )
#define	SetFadeClip	( (void(__cdecl*)(short, short)) 0x00483A90 )
#define	SetScreenFadeOut	( (void(__cdecl*)(long, long)) 0x00483B30 )
#define	SetScreenFadeIn	( (void(__cdecl*)(long)) 0x00483BA0 )
#define	CreateBubble	( (void(__cdecl*)(PHD_3DPOS*, short, int, int, int, int, int , int)) 0x00483350 )
#define	TriggerGunShell	( (void(__cdecl*)(short, short, int)) 0x482A60 )
#define GetFreeSmokeSpark	( (int(__cdecl*)()) 0x00481D40 )
#define TriggerLightningGlow	( (void(__cdecl*)(long, long, long, long)) 0x004851B0 )
#define ExplodingDeath2	( (void(__cdecl*)(short, long, short)) 0x00484080 )
#define Richochet	( (void(__cdecl*)(GAME_VECTOR*)) 0x00432710 )
#define trig_actor_gunflash	( (void(__cdecl*)(MATRIX3D*, PHD_VECTOR*)) 0x00485EC0 )
#define TriggerSmallSplash	( (void(__cdecl*)(long, long, long, long)) 0x00483180 )
#define TriggerShockwave	( (void(__cdecl*)(PHD_VECTOR*, long, long, long, long)) 0x00484670 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, int, int, int)) 0x004820A0 )
#define ClearFires	( (void(__cdecl*)()) 0x00481B10 )
