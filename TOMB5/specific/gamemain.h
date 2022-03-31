#pragma once
#include "../global/vars.h"

void inject_gamemain(bool replace);

ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
void GameClose();

#define GameInitialise	( (bool(__cdecl*)()) 0x004A8610 )
