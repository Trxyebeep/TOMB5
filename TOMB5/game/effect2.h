#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv);
void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit);
void KillAllCurrentItems(short item_number);
void KillEverything();

#define TriggerDynamic	( (void(__cdecl*)(long, long, long, int, int, int, int)) 0x00431240 )
#define TriggerDynamic_MIRROR	( (void(__cdecl*)(long, long, long, int, int, int, int)) 0x00431420 )
#define ClearDynamics	( (void(__cdecl*)()) 0x00431530 )
#define UpdateSparks	( (void(__cdecl*)()) 0x0042E8B0 )
#define UpdateSplashes	( (void(__cdecl*)()) 0x00430710 )
#define TriggerWaterfallMist	( (void(__cdecl*)(long, long, long, long)) 0x00430A40 )
#define TriggerFireFlame	( (void(__cdecl*)(int, int, int, int, signed int)) 0x0042FE20 )
#define SetupRipple	( (void(__cdecl*)(long, long, long, long, long)) 0x00430910 )
#define TriggerUnderwaterBlood	( (void(__cdecl*)(int, int, int, int)) 0x004309B0 )
#define TriggerExplosionSparks	( (void(__cdecl*)(int, int, int, int, int, int, short)) 0x0042F610 )
#define TriggerExplosionBubble	( (void(__cdecl*)(int, int, int, short)) 0x00431070 )
#define GetFreeSpark	( (int(__cdecl*)()) 0x0042E790 )
#define TriggerSuperJetFlame	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x00430350 )
