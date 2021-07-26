#pragma once
#include "../global/vars.h"

void inject_funcStubs(bool replace);

void* game_malloc(int size, int type);
int GetRandomControl();
void SeedRandomControl(long seed);
int GetRandomDraw();
void SeedRandomDraw(long seed);

#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x004DEB10 )
