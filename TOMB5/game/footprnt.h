#pragma once
#include "../global/types.h"

void AddFootprint(ITEM_INFO* item);
void S_DrawFootPrints();
void GetProperFootPos(PHD_VECTOR* pos);

extern FOOTPRINT FootPrint[32];
extern long FootPrintNum;
