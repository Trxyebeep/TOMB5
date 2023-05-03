#pragma once
#include "../global/types.h"

long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
bool GameInitialise();
unsigned int __stdcall GameMain(void* ptr);
void GameClose();

extern WATERTAB WaterTable[22][64];
extern THREAD MainThread;
extern float vert_wibble_table[32];
extern short* clipflags;
extern long SaveCounter;
