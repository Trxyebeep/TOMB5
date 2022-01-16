#pragma once
#include "../global/vars.h"

void inject_funcStubs(bool replace);

void* game_malloc(int size, int type);
int GetRandomControl();
void SeedRandomControl(long seed);
int GetRandomDraw();
void SeedRandomDraw(long seed);
void S_SoundSetMasterVolume();
void deadLog(const char* string, ...);

#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x004DEB10 )//wrong place, idk where it goes
#define S_CDVolume	( (void(__cdecl*)(int)) 0x004A7BB0 )
#define S_CDLoop	( (void(__cdecl*)()) 0x004A7BD0 )
#define S_SetReverbType	( (void(__cdecl*)(int)) 0x004A7B90 )
#define S_ExitSystem	( (void(__cdecl*)(const char*)) 0x004A7AE0 )
#define S_InitialiseScreen	( (void(__cdecl*)()) 0x004A7B10 )
