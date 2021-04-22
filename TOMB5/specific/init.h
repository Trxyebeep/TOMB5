#pragma once


void inject_init();

void* game_malloc(int size);


#define Log	( (void(__cdecl*)(char, const char*)) 0x004DEB10 )
