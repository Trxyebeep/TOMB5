#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

bool LoadTextureInfos();
bool LoadRooms();
FILE* FileOpen(const char* Filename);
bool FindCDDrive();
void FileClose(FILE* fp);
int FileSize(FILE* fp);

#define S_LoadLevelFile	( (int(__cdecl*)(uchar)) 0x004A72B0 )
#define	AdjustUV	( (void(__cdecl*)(int)) 0x004A5430 )
#define DoMonitorScreen	( (void(__cdecl*)()) 0x004A7020 )
