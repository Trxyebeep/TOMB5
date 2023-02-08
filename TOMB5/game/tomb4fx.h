#pragma once
#include "../global/vars.h"

void inject_tomb4fx(bool replace);

long GetFreeBlood();
void UpdateBlood();
void TriggerBlood(long x, long y, long z, long angle, long num);
long LSpline(long x, long* knots, long nk);
LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments);
void LaraBubbles(ITEM_INFO* item);
void ControlElectricFence(short item_number);
void ControlTeleporter(short item_number);
void DrawWeaponMissile(ITEM_INFO* item);
void DrawLensFlares(ITEM_INFO* item);
long ExplodingDeath2(short item_number, long mesh_bits, short Flags);
void SetGunFlash(short weapon);
void DrawGunflashes();
void trig_actor_gunflash(long* mx, PHD_VECTOR* pos);

#define UpdateBubbles	( (void(__cdecl*)()) 0x00483540 )
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
#define	CreateBubble	( (void(__cdecl*)(PHD_3DPOS*, short, long, long, long, long, long , long)) 0x00483350 )
#define	TriggerGunShell	( (void(__cdecl*)(short, short, long)) 0x00482A60 )
#define GetFreeSmokeSpark	( (long(__cdecl*)()) 0x00481D40 )
#define TriggerLightningGlow	( (void(__cdecl*)(long, long, long, long)) 0x004851B0 )
#define TriggerSmallSplash	( (void(__cdecl*)(long, long, long, long)) 0x00483180 )
#define TriggerShockwave	( (void(__cdecl*)(PHD_VECTOR*, long, long, long, long)) 0x00484670 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, long, long, long)) 0x004820A0 )
#define ClearFires	( (void(__cdecl*)()) 0x00481B10 )
#define TriggerFenceSparks	( (void(__cdecl*)(long, long, long, long, long)) 0x00485D80 )
#define AddFire	( (void(__cdecl*)(long, long, long, long, short, short)) 0x00481B40 )
#define DrawGunshells	( (void(__cdecl*)()) 0x00483090 )
#define S_DrawFires	( (void(__cdecl*)()) 0x00481BB0 )
#define TriggerShatterSmoke	( (void(__cdecl*)(long, long, long)) 0x004823A0 )
#define Fade	( (void(__cdecl*)()) 0x00483BF0 )
#define CalcLightningSpline	( (void(__cdecl*)(PHD_VECTOR*, SVECTOR*, LIGHTNING_STRUCT*)) 0x00484EB0 )
