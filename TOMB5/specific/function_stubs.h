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
void DBG_Print(const char* string, ...);
void exit_message();
void S_ExitSystem(const char* string);
void S_InitialiseScreen();
void S_CalculateStaticLight();
void S_CalculateLight(long x, long y, long z, short room_number, ITEM_LIGHT* il);
void S_SetReverbType(short type);
void S_CDVolume(long vol);
void S_CDLoop();
void PrintDbug();
void init_game_malloc();
void game_free(long size);

#define Log_Init	( (void(__cdecl*)(ulong)) 0x004DEC40 )
#define Log_DefType	( (void(__cdecl*)(const char*, ulong, bool)) 0x004DEA50 )
#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x004DEB10 )//wrong place, idk where it goes
#define CreateFakeD3D	( (void(__cdecl*)()) 0x004D3A90 )
#define ClearFakeDevice	( (HRESULT(__cdecl*)(LPDIRECT3DDEVICE3, long, D3DRECT*, long, long, float, long)) 0x004D3D20 )
