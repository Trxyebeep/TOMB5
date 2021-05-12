#pragma once
#include "../global/vars.h"

void inject_laramisc();

void GetLaraDeadlyBounds();
void InitialiseLaraAnims(ITEM_INFO* item);
void InitialiseLaraLoad(short item_num);
void LaraCheat(ITEM_INFO* item, COLL_INFO* coll);
void LaraInitialiseMeshes();
void InitialiseLara(int restore);
void LaraCheatGetStuff();
void AnimateLara(ITEM_INFO* item);
#ifdef VER_JP
void LaraCheatyBits();
#endif

#define LaraControl	( (void(__cdecl*)(short)) 0x00455830 )
