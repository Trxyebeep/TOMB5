#pragma once
#include "../global/vars.h"

void inject_twogun(bool replace);

void ControlZipController(short item_number);

#define ControlGunTestStation	( (void(__cdecl*)(ITEM_INFO*)) 0x0048D940 )
#define UpdateTwogunLasers	( (void(__cdecl*)()) 0x0048D7D0 )
