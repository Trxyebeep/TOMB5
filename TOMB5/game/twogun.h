#pragma once
#include "../global/types.h"

void ControlZipController(short item_number);
void InitialiseTwogun(short item_number);
void TwogunControl(short item_number);
void UpdateTwogunLasers();
void DrawTwogunLasers();
void TriggerTwogunPlasma(PHD_VECTOR* pos, short* angles, long life);
void FireTwogunWeapon(ITEM_INFO* item, long lr, long plasma);
void ControlGunTestStation(ITEM_INFO* item);

extern TWOGUN_INFO twogun[4];
