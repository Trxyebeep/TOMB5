#pragma once
#include "../global/types.h"

long GetFreeBat();
void TriggerBat(ITEM_INFO* item);
void InitialiseBatEmitter(short item_number);
void ControlBatEmitter(short item_number);
void UpdateBats();
void DrawBats();

extern BAT_STRUCT* Bats;
