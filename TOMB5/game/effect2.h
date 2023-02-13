#pragma once
#include "../global/types.h"

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
void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number);
void TriggerFireFlame(long x, long y, long z, long body_part, long type);
void TriggerSuperJetFlame(ITEM_INFO* item, long yvel, long deadly);
long GetFreeSpark();
void DetatchSpark(long num, long type);
void UpdateSparks();

extern DYNAMIC dynamics[64];
extern SPLASH_SETUP splash_setup;
extern long wibble;
extern long SplashCount;
extern long KillEverythingFlag;
extern long SmokeCountL;
extern long SmokeCountR;
extern long SmokeWeapon;
extern long SmokeWindX;
extern long SmokeWindZ;
extern long DeadlyBounds[6];
