#pragma once
#include "../global/types.h"

void InitialiseGuardian(short item_number);
void TriggerGuardianSparks(GAME_VECTOR* pos, long size, long rgb, long power);
void TriggerBaseLightning(ITEM_INFO *item);
void GuardianControl(short item_number);
