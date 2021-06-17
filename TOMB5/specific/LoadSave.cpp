#include "../tomb5/pch.h"
#include "LoadSave.h"
#include "display.h"
#include "../game/gameflow.h"

void CheckKeyConflicts()
{
	short key;

	for (int i = 0; i < 18; i++)
	{
		key = layout[0][i];

		conflict[i] = 0;

		for (int j = 0; j < 18; j++)
		{
			if (key == layout[1][j])
			{
				conflict[i] = 1;
				break;
			}
		}
	}
}

void DoStatScreen()
{
	unsigned short ypos;
	short Days, Hours, Min, Sec;
	char buffer[40];
	int seconds;

	ypos = phd_centery - 4 * font_height;
	PrintString(phd_centerx, ypos, 6, &gfStringWad[gfStringOffset[STR_STATISTICS]], FF_CENTER);
	PrintString(phd_centerx, ypos + 2 * font_height, 2, &gfStringWad[gfStringOffset[gfLevelNames[gfCurrentLevel]]], FF_CENTER);
	PrintString(phd_centerx >> 2, ypos + 2 * font_height + font_height, 2, &gfStringWad[gfStringOffset[STR_TIME_TAKEN]], 0);
	PrintString(phd_centerx >> 2, ypos + 4 * font_height, 2, &gfStringWad[gfStringOffset[STR_DISTANCE_TRAVELLED]], 0);
	PrintString(phd_centerx >> 2, ypos + 5 * font_height, 2, &gfStringWad[gfStringOffset[STR_AMMO_USED]], 0);
	PrintString(phd_centerx >> 2, ypos + 6 * font_height, 2, &gfStringWad[gfStringOffset[STR_HEALTH_PACKS_USED]], 0);
	PrintString(phd_centerx >> 2, ypos + 7 * font_height, 2, &gfStringWad[gfStringOffset[STR_SECRETS_FOUND]],0);

	seconds = GameTimer / 30;
	Days = seconds / (24 * 60 * 60);
	Hours = (seconds % (24 * 60 * 60)) / (60 * 60);
	Min = (seconds / 60) % 60;
	Sec = (seconds % 60);

	sprintf(buffer, "%02d:%02d:%02d", (Days * 24) + Hours, Min, Sec);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 2 * font_height + font_height, 6, buffer, 0);
	sprintf(buffer, "%dm", savegame.Game.Distance / 419);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 4 * font_height, 6, buffer, 0);
	sprintf(buffer, "%d", savegame.Game.AmmoUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 4 * font_height + font_height, 6, buffer, 0);
	sprintf(buffer, "%d", savegame.Game.HealthUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 6 * font_height, 6, buffer, 0);
	sprintf(buffer, "%d / 36", savegame.Game.Secrets);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 7 * font_height, 6, buffer, 0);
}

void DisplayStatsUCunt()
{
	DoStatScreen();
}

void inject_LoadSave()
{
	INJECT(0x004ADF40, CheckKeyConflicts);
	INJECT(0x004B0910, DoStatScreen);
	INJECT(0x004B1E70, DisplayStatsUCunt);//actually in some other file
}
