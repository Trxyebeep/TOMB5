#pragma once
#include "../global/vars.h"

void inject_laramisc(bool replace);

void GetLaraDeadlyBounds();
void InitialiseLaraAnims(ITEM_INFO* item);
void InitialiseLaraLoad(short item_num);
void LaraCheat(ITEM_INFO* item, COLL_INFO* coll);
void LaraInitialiseMeshes();
void LaraCheatGetStuff();
void AnimateLara(ITEM_INFO* item);
#ifndef TRF
#ifdef ENABLE_CHEATS
void LaraCheatyBits();
#endif
#endif
void LaraControl(short item_number);
