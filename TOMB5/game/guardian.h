#pragma once
#include "../global/vars.h"

void inject_guardian(bool replace);

void InitialiseGuardian(short item_number);
void TriggerGuardianSparks(GAME_VECTOR* pos, long size, long rgb, long power);
void TriggerBaseLightning(ITEM_INFO *item);
void GuardianControl(short item_number);
