#pragma once
#include "../global/vars.h"

void inject_funcStubs();

void* game_malloc(int size);

#define Log	( (void(__cdecl*)(char, const char*)) 0x004DEB10 )
