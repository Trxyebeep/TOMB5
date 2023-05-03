#pragma once
#include "../global/types.h"

void init_game_malloc();
void* game_malloc(long size);
long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void Log(const char* s, ...);

extern FILE* logF;
extern char* malloc_buffer;
extern char* malloc_ptr;
extern long malloc_size;
extern long malloc_free;
extern long nPolyType;
