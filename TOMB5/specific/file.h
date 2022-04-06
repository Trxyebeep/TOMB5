#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

bool LoadTextureInfos();
bool LoadRooms();
FILE* FileOpen(const char* Filename);
bool FindCDDrive();
void FileClose(FILE* fp);
int FileSize(FILE* fp);
bool LoadItems();
bool LoadSprites();
bool LoadCameras();
bool LoadSoundEffects();
bool LoadAnimatedTextures();
bool LoadObjects();
bool LoadCinematic();
bool LoadSamples();
bool LoadAIInfo();
long S_LoadLevelFile(long num);

#define	AdjustUV	( (void(__cdecl*)(int)) 0x004A5430 )
#define DoMonitorScreen	( (void(__cdecl*)()) 0x004A7020 )
#define FreeLevel	( (void(__cdecl*)()) 0x004A7130 )
