#pragma once
#include "../global/types.h"

void DrawFloorLasers(ITEM_INFO* item);
void ControlLasers(short item_number);
void ControlSteamLasers(short item_number);
void ControlFloorLasers(short item_number);
long IsSteamOn(ITEM_INFO* item);
long GetSteamMultiplier(ITEM_INFO* item, long y, long z);
long CheckLaserBox(long* bounds);
void GetFishTank(ITEM_INFO* item);

extern char SteamLasers[8][5];
