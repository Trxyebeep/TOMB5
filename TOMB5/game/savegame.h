#pragma once
#include "../global/types.h"

void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
void SaveLaraData();
void RestoreLaraData(long FullSave);
void SaveLevelData(long FullSave);
void RestoreLevelData(long FullSave);
void sgSaveGame();
void sgRestoreGame();

extern SAVEGAME_INFO savegame;
extern tomb5_save_info tomb5_save;
extern ulong tomb5_save_size;
