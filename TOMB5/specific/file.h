#pragma once
#include "../global/vars.h"

void inject_file();

bool LoadTextureInfos();
bool LoadRooms();
FILE* FileOpen(const char* Filename);

#define S_LoadLevelFile	( (int(__cdecl*)(unsigned char)) 0x004A72B0 )
#define PlayFmvNow	( (void(__cdecl*)(int)) 0x004A79A0 )
#define	AdjustUV	( (void(__cdecl*)(int)) 0x004A5430 )
