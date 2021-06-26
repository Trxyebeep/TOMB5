#pragma once
#include "../global/vars.h"

void inject_bats();

long GetFreeBat();
void TriggerBat(ITEM_INFO* item);
void InitialiseBatEmitter(short item_number);
void ControlBatEmitter(short item_number);
void UpdateBats();
void DrawBats();
