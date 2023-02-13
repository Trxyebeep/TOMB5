#pragma once
#include "../global/types.h"

ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
void GameClose();
unsigned int __stdcall GameMain(void* ptr);
bool GameInitialise();

extern WATERTAB WaterTable[22][64];
extern THREAD MainThread;
extern float vert_wibble_table[32];
extern short* clipflags;
extern long SaveCounter;
