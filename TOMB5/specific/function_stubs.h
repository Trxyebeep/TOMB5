#pragma once
#include "../global/types.h"

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
void Log(ulong type, const char* s, ...);

extern FILE* logF;
extern char* malloc_buffer;
extern char* malloc_ptr;
extern long malloc_size;
extern long malloc_free;
extern long nPolyType;
