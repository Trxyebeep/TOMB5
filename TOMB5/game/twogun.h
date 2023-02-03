#pragma once
#include "../global/vars.h"

void inject_twogun(bool replace);

void ControlZipController(short item_number);
void InitialiseTwogun(short item_number);
void TwogunControl(short item_number);
void UpdateTwogunLasers();
void DrawTwogunLasers();

#define ControlGunTestStation	( (void(__cdecl*)(ITEM_INFO*)) 0x0048D940 )
#define FireTwogunWeapon	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x0048DF60 )
