#pragma once
#include "../global/vars.h"

void inject_footprnt(bool replace);

void AddFootprint(ITEM_INFO* item);

#ifdef FOOTPRINTS
void S_DrawFootPrints();
void GetProperFootPos(PHD_VECTOR* pos);
#endif
