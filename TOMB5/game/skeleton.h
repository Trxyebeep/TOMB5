#pragma once
#include "../global/types.h"

void inject_skelly(bool replace);

void InitialiseSkeleton(short item_number);
void SkeletonControl(short item_number);
void TriggerSkeletonFire(ITEM_INFO* item);
void TriggerFontFire(ITEM_INFO* item, long num, long loop);
