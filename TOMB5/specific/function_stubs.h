#pragma once
#include "../global/vars.h"

void inject_funcStubs();

void* game_malloc(int size, int type);

#define Log	( (void(__cdecl*)(unsigned long, const char*, ...)) 0x004DEB10 )
