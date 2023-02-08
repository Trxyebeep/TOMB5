#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv);
void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit);
void KillAllCurrentItems(short item_number);
void KillEverything();
void ControlSmokeEmitter(short item_number);
void ControlEnemyMissile(short fx_number);
void TriggerExplosionSmokeEnd(long x, long y, long z, long uw);
void ClearDynamics();
void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b);
void TriggerDynamic_MIRROR(long x, long y, long z, long falloff, long r, long g, long b);
void TriggerExplosionBubble(long x, long y, long z, short room_number);
void TriggerWaterfallMist(long x, long y, long z, long ang);
void TriggerUnderwaterBlood(long x, long y, long z, long size);
void SetupRipple(long x, long y, long z, long size, long flags);
void SetupSplash(SPLASH_SETUP* setup);
void UpdateSplashes();
void TriggerRicochetSpark(GAME_VECTOR* pos, long ang, long num, long smoke_only);
void TriggerExplosionSmoke(long x, long y, long z, long uw);

#define UpdateSparks	( (void(__cdecl*)()) 0x0042E8B0 )
#define TriggerFireFlame	( (void(__cdecl*)(long, long, long, long, signed long)) 0x0042FE20 )
#define TriggerExplosionSparks	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x0042F610 )
#define GetFreeSpark	( (long(__cdecl*)()) 0x0042E790 )
#define TriggerSuperJetFlame	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x00430350 )
