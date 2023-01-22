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
#ifdef GENERAL_FIXES
void LaraCheatyBits();
#endif
void LaraControl(short item_number);
