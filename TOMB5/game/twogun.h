#pragma once
#include "../global/vars.h"

void inject_twogun(bool replace);

void ControlZipController(short item_number);
void InitialiseTwogun(short item_number);
void TwogunControl(short item_number);
void UpdateTwogunLasers();
void DrawTwogunLasers();
void TriggerTwogunPlasma(PHD_VECTOR* pos, short* angles, long life);
void FireTwogunWeapon(ITEM_INFO* item, long lr, long plasma);

#define ControlGunTestStation	( (void(__cdecl*)(ITEM_INFO*)) 0x0048D940 )
