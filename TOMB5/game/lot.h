#pragma once
#include "../global/vars.h"

void inject_lot(bool replace);

void InitialiseLOTarray(long allocmem);

#define EnableBaddieAI	( (int(__cdecl*)(short, int)) 0x0045B1A0 )
#define	DisableBaddieAI	( (void(__cdecl*)(short)) 0x0045B150 )
