#pragma once
#include "../global/vars.h"

void inject_lasers(bool replace);

void DrawFloorLasers(ITEM_INFO* item);
void ControlLasers(short item_number);
void ControlSteamLasers(short item_number);
void ControlFloorLasers(short item_number);

#define CheckLaserBox	( (long(__cdecl*)(long*)) 0x00459EB0 )
#define GetFishTank	( (void(__cdecl*)(ITEM_INFO*)) 0x0045A4B0 )
