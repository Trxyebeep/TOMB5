#pragma once
#include "../global/vars.h"

void CalcShadowMatrices_Normal(short* frame, long* bone, long flag);
void CalcShadowMatrices(long flag);
void DrawDynamicShadow(ITEM_INFO* item);
void DrawAnimatingItemShadow(ITEM_INFO* item);
bool DoIDynamicShadow(ITEM_INFO* item);
