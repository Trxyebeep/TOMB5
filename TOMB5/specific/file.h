#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

bool LoadTextureInfos();
bool LoadRooms();
FILE* FileOpen(const char* Filename);
bool FindCDDrive();
void FileClose(FILE* fp);
long FileSize(FILE* fp);
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
bool LoadTextures(long RTPages, long OTPages, long BTPages);
void S_GetUVRotateTextures();
void DoMonitorScreen();
void S_LoadLevel();
void LoadMapFile();

#define	AdjustUV	( (void(__cdecl*)(long)) 0x004A5430 )
#define FreeLevel	( (void(__cdecl*)()) 0x004A7130 )
#define Decompress	( (bool(__cdecl*)(char*, char*, long, long)) 0x004A3EF0 )
#define LoadFile	( (long(__cdecl*)(const char*, char**)) 0x004A3E10 )
