#pragma once
#include "../global/vars.h"

void inject_twogun(bool replace);

void ControlZipController(short item_number);
void InitialiseTwogun(short item_number);
void TwogunControl(short item_number);

#define ControlGunTestStation	( (void(__cdecl*)(ITEM_INFO*)) 0x0048D940 )
#define UpdateTwogunLasers	( (void(__cdecl*)()) 0x0048D7D0 )
#define FireTwogunWeapon	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x0048DF60 )
#define DrawTwogunLasers	( (void(__cdecl*)()) 0x0048D900 )
