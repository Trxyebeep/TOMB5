#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

#ifdef GENERAL_FIXES
extern tomb5_save_info tomb5_save;
extern ulong tomb5_save_size;
#endif

void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
long CheckSumValid(char* buffer);
void SaveLaraData();
void RestoreLaraData(long FullSave);
void SaveLevelData(long FullSave);
void RestoreLevelData(long FullSave);
void sgSaveGame();
void sgRestoreGame();
