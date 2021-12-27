#pragma once
#include "../global/vars.h"

void inject_gamemain(bool replace);

ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();

#define S_LoadGame	( (int(__cdecl*)(int)) 0x004A8E10 )
