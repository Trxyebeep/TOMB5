#include "../tomb5/pch.h"
#include "specific.h"
#include "../global/types.h"
#include "../game/gameflow.h"
#include <stdio.h>
#include "display.h"

void DoStatScreen()
{
	const unsigned short y = phd_centery - 4 * font_height;/*unsigned short instead of dword to remove warnings*/
	PrintString(
		phd_centerx,
		y,
		6u,
		&gfStringWad[gfStringOffset[STR_STATISTICS]],
		FF_CENTER);
	PrintString(
		phd_centerx,
		y + 2 * font_height,
		2u,
		&gfStringWad[gfStringOffset[gfLevelNames[gfCurrentLevel]]],
		FF_CENTER);
	PrintString(
		phd_centerx >> 2,
		y + 2 * font_height + font_height,
		2u,
		&gfStringWad[gfStringOffset[STR_TIME_TAKEN]],
		FF_NONE);
	PrintString(
		phd_centerx >> 2,
		y + 4 * font_height,
		2u,
		&gfStringWad[gfStringOffset[STR_DISTANCE_TRAVELLED]],
		FF_NONE);
	PrintString(
		phd_centerx >> 2,
		y + 5 * font_height,
		2u,
		&gfStringWad[gfStringOffset[STR_AMMO_USED]],
		FF_NONE);
	PrintString(
		phd_centerx >> 2,
		y + 6 * font_height,
		2u,
		&gfStringWad[gfStringOffset[STR_HEALTH_PACKS_USED]],
		FF_NONE);
	PrintString(
		phd_centerx >> 2,
		y + 7 * font_height,
		2u,
		&gfStringWad[gfStringOffset[STR_SECRETS_FOUND]],
		FF_NONE);
	char string[40];
	const auto total_sec = GameTimer / 30;
	const auto seconds = total_sec % 60;
	const auto minutes = (total_sec / 60) % 60;
	const auto hours = total_sec / 3600;
	sprintf(
		string,
		"%02d:%02d:%02d", hours, minutes, seconds);
	PrintString(phd_centerx + (phd_centerx >> 2), y + 2 * font_height + font_height, 6u, string, FF_NONE);
	sprintf(string, "%dm", savegame.Game.Distance / 419);
	PrintString(phd_centerx + (phd_centerx >> 2), y + 4 * font_height, 6u, string, FF_NONE);
	sprintf(string, "%d", savegame.Game.AmmoUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), y + 4 * font_height + font_height, 6u, string, FF_NONE);
	sprintf(string, "%d", savegame.Game.HealthUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), y + 6 * font_height, 6u, string, FF_NONE);
	sprintf(string, "%d / 36", savegame.Game.Secrets);
	PrintString(phd_centerx + (phd_centerx >> 2), y + 7 * font_height, 6u, string, FF_NONE);
}

void DisplayStatsUCunt()
{
	DoStatScreen();
}

void inject_specific()
{
	INJECT(0x004B0910, DoStatScreen);
	INJECT(0x004B1E70, DisplayStatsUCunt);
}
