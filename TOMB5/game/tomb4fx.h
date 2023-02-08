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
void TriggerLightningGlow(long x, long y, long z, long rgb);
void CalcLightningSpline(PHD_VECTOR* pos, SVECTOR* dest, LIGHTNING_STRUCT* lptr);
void UpdateLightning();
void TriggerShockwaveHitEffect(long x, long y, long z, long rgb, short dir, long speed);
long GetFreeShockwave();
void TriggerShockwave(PHD_VECTOR* pos, long InnerOuterRads, long speed, long bgrl, long XRotFlags);
void UpdateShockwaves();
long GetFreeDrip();
void TriggerLaraDrips();
void UpdateDrips();
void Fade();
void SetScreenFadeOut(short speed, short back);
void SetScreenFadeIn(short speed);
void UpdateFadeClip();
void SetFadeClip(short height, short speed);

#define UpdateBubbles	( (void(__cdecl*)()) 0x00483540 )
#define UpdateGunShells	( (void(__cdecl*)()) 0x00482D80 )
#define UpdateFireSparks	( (void(__cdecl*)()) 0x004813B0 )
#define UpdateSmokeSparks	( (void(__cdecl*)()) 0x00481DD0 )
#define	CreateBubble	( (void(__cdecl*)(PHD_3DPOS*, short, long, long, long, long, long , long)) 0x00483350 )
#define	TriggerGunShell	( (void(__cdecl*)(short, short, long)) 0x00482A60 )
#define GetFreeSmokeSpark	( (long(__cdecl*)()) 0x00481D40 )
#define TriggerSmallSplash	( (void(__cdecl*)(long, long, long, long)) 0x00483180 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, long, long, long)) 0x004820A0 )
#define ClearFires	( (void(__cdecl*)()) 0x00481B10 )
#define TriggerFenceSparks	( (void(__cdecl*)(long, long, long, long, long)) 0x00485D80 )
#define AddFire	( (void(__cdecl*)(long, long, long, long, short, short)) 0x00481B40 )
#define DrawGunshells	( (void(__cdecl*)()) 0x00483090 )
#define S_DrawFires	( (void(__cdecl*)()) 0x00481BB0 )
#define TriggerShatterSmoke	( (void(__cdecl*)(long, long, long)) 0x004823A0 )
