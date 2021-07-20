#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv);
void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit);

#define TriggerDynamic	( (void(__cdecl*)(long, long, long, int, int, int, int)) 0x00431240 )
#define TriggerDynamic_MIRROR	( (void(__cdecl*)(long, long, long, int, int, int, int)) 0x00431420 )
#define TriggerGunSmoke	( (void(__cdecl*)(long, long, long, long, long, long, int, int, int)) 0x004820A0 )
#define KillEverything	( (void(__cdecl*)()) 0x00431050 )
#define UpdateDebris	( (void(__cdecl*)()) 0x0041D500 )
#define UpdateSparks	( (void(__cdecl*)()) 0x0042E8B0 )
#define UpdateSplashes	( (void(__cdecl*)()) 0x00430710 )
#define TriggerWaterfallMist	( (void(__cdecl*)(long, long, long, long)) 0x00430A40 )
#define TriggerFireFlame	( (void(__cdecl*)(int, int, int, int, signed int)) 0x0042FE20 )
#define SetupRipple	( (void(__cdecl*)(long, long, long, long, long)) 0x00430910 )
