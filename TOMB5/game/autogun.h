#pragma once
#include "../global/vars.h"

void inject_autogun(bool replace);

void ControlMotionSensors(short item_number);
void AutogunControl(short item_number);
void TriggerAutoGunSmoke(GAME_VECTOR* pos, long shade);
