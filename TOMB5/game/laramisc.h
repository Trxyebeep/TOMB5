#pragma once
#include "../global/types.h"

void GetLaraDeadlyBounds();
void InitialiseLaraAnims(ITEM_INFO* item);
void InitialiseLaraLoad(short item_num);
void LaraCheat(ITEM_INFO* item, COLL_INFO* coll);
void LaraInitialiseMeshes();
void LaraCheatGetStuff();
void AnimateLara(ITEM_INFO* item);
void LaraCheatyBits();
void LaraControl(short item_number);

extern COLL_INFO mycoll;
