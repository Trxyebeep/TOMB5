#pragma once
#include "../global/vars.h"

void inject_lasers(bool replace);

void DrawFloorLasers(ITEM_INFO* item);
void ControlLasers(short item_number);
void ControlSteamLasers(short item_number);
void ControlFloorLasers(short item_number);
long IsSteamOn(ITEM_INFO* item);
long GetSteamMultiplier(ITEM_INFO* item, long y, long z);
long CheckLaserBox(long* bounds);

#define GetFishTank	( (void(__cdecl*)(ITEM_INFO*)) 0x0045A4B0 )

extern char SteamLasers[8][5];
