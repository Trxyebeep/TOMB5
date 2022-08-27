#pragma once
#include "../global/vars.h"

void inject_funcStubs(bool replace);

void* game_malloc(long size, long type);
long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void S_SoundSetMasterVolume();
void deadLog(const char* string, ...);

#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x004DEB10 )//wrong place, idk where it goes
#define S_CDVolume	( (void(__cdecl*)(long)) 0x004A7BB0 )
#define S_CDLoop	( (void(__cdecl*)()) 0x004A7BD0 )
#define S_SetReverbType	( (void(__cdecl*)(long)) 0x004A7B90 )
#define S_ExitSystem	( (void(__cdecl*)(const char*)) 0x004A7AE0 )
#define S_InitialiseScreen	( (void(__cdecl*)()) 0x004A7B10 )
#define CreateFakeD3D	( (void(__cdecl*)()) 0x004D3A90 )
#define ClearFakeDevice	( (HRESULT(__cdecl*)(LPDIRECT3DDEVICE3, long, D3DRECT*, long, long, float, long)) 0x004D3D20 )
