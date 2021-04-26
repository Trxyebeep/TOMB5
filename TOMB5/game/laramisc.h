#pragma once
#include "../global/types.h"

void inject_laramisc();

void GetLaraDeadlyBounds();
void InitialiseLaraAnims(ITEM_INFO* item);
void InitialiseLaraLoad(short item_num);
void LaraCheat(ITEM_INFO* item, COLL_INFO* coll);
void LaraInitialiseMeshes();
void InitialiseLara(int restore);
void LaraCheatyBits();
#ifdef VER_JP
void _cheats();
#endif

#define AnimateLara ( (void(__cdecl*)(ITEM_INFO*)) 0x004563F0 )
#define LaraControl	( (void(__cdecl*)(short)) 0x00455830 )
#define DelAlignLaraToRope	( (void(__cdecl*)(ITEM_INFO*)) 0x0046D510 )//in rope.cpp i think
