#include "../tomb5/pch.h"
#include "LoadSave.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "../game/sound.h"

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
	ushort ypos;
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

void S_DrawAirBar(int pos)
{
	if (gfCurrentLevel != LVL5_TITLE)
		DoBar(490 - (font_height >> 2), (font_height >> 1) + (font_height >> 2) + 32, 150, 12, pos, 0x0000A0, 0x0050A0);//blue rgb 0, 0, 160/lighter blue rgb 0, 80, 160
}

void S_DrawHealthBar(int pos)
{
	long color;

	if (gfCurrentLevel != LVL5_TITLE)
	{
		if (lara.poisoned || lara.Gassed)
			color = 0xA0A000;//yellowish poison, rgb 160, 160, 0
		else
			color = 0x00A000;//green, rgb 0, 160, 0

		DoBar(font_height >> 2, (font_height >> 2) + 32, 150, 12, pos, 0xA00000, color);//red rgb 160, 0, 0/color
	}
}

void S_DrawHealthBar2(int pos)//same as above just different screen position
{
	long color;

	if (gfCurrentLevel != LVL5_TITLE)
	{
		if (lara.poisoned || lara.Gassed)
			color = 0xA0A000;
		else
			color = 0xA000;

		DoBar(245, (font_height >> 1) + 32, 150, 12, pos, 0xA00000, color);
	}
}

void S_DrawDashBar(int pos)
{
	if (gfCurrentLevel != LVL5_TITLE)
		DoBar(490 - (font_height >> 2), (font_height >> 2) + 32, 150, 12, pos, 0xA0A000, 0x00A000);//yellow rgb 160, 160, 0 / green rgb 0, 160, 0
}

int DoLoadSave(int LoadSave)
{
	int String, color, n;
	char SaveInfo[80];
	char string[41];

	if (LoadSave & IN_SAVE)
		String = gfStringOffset_bis[STR_SAVE_GAME_BIS_BIS];
	else
		String = gfStringOffset_bis[STR_LOAD_GAME_BIS];

	PrintString(phd_centerx, font_height, 6, &gfStringWad[String], FF_CENTER);

	for (int i = 0; i < 15; i++)
	{
		color = 2;

		if (i == cSaveGameSelect)
			color = 1;

		memset(string, 32, 40);
		n = strlen(SaveGames[i].SaveName);

		if (n > 40)
			n = 40;

		strncpy(string, SaveGames[i].SaveName, n);
		string[40] = 0;
		tqFontHeight = 1;

		if (SaveGames[i].bValid)
		{
			wsprintf(SaveInfo, "%03d", SaveGames[i].Count);
			PrintString(phd_centerx - int((310.0f * (phd_winwidth / 640.0f))), font_height + ((i + 2) * font_height), color, SaveInfo, 0);
			PrintString(phd_centerx - int((270.0f * (phd_winwidth / 640.0f))), font_height + ((i + 2) * font_height), color, string, 0);
			wsprintf(SaveInfo, "%d %s %02d:%02d:%02d", SaveGames[i].Day, &gfStringWad[gfStringOffset_bis[STR_DAYS]], SaveGames[i].Hour, SaveGames[i].Min, SaveGames[i].Sec);
			PrintString(phd_centerx + int((135.0f * (phd_winwidth / 640.0f))), font_height + ((i + 2) * font_height), color, SaveInfo, 0);
		}
		else
		{
			wsprintf(SaveInfo, "%s", SaveGames[i].SaveName);
			PrintString(phd_centerx, font_height + ((i + 2) * font_height), color, SaveInfo, FF_CENTER);
		}

		tqFontHeight = 0;
	}

	if (dbinput & IN_FORWARD)
	{
		cSaveGameSelect--;
		SoundEffect(SFX_MENU_CHOOSE, 0, 0);
	}

	if (dbinput & IN_BACK)
	{
		cSaveGameSelect++;
		SoundEffect(SFX_MENU_CHOOSE, 0, 0);
	}

	if (cSaveGameSelect < 0)
		cSaveGameSelect = 0;

	if (cSaveGameSelect > 14)
		cSaveGameSelect = 14;

	if (dbinput & IN_SELECT)
	{
		if (SaveGames[cSaveGameSelect].bValid || LoadSave == IN_SAVE)
			return cSaveGameSelect;
		else
			SoundEffect(SFX_LARA_NO, 0, 0);
	}

	return -1;
}

void S_MemSet(void* p, int val, size_t sz)
{
	memset(p, val, sz);
}

int GetCampaignCheatValue()
{
	static int counter = 0;
	static int timer;
	int jump;

	if (timer)
		timer--;
	else
		counter = 0;

	jump = 0;

	switch (counter)
	{
	case 0:

		if (keymap[33])//F
		{
			timer = 450;
			counter = 1;
		}

		break;

	case 1:
		if (keymap[23])//I
			counter = 2;

		break;

	case 2:
		if (keymap[38])//L
			counter = 3;

		break;

	case 3:
		if (keymap[20])//T
			counter = 4;

		break;

	case 4:
		if (keymap[35])//H
			counter = 5;

		break;

	case 5:
		if (keymap[21])//Y
			counter = 6;

		break;

	case 6:
		if (keymap[2])//1, not the numpad
			jump = LVL5_STREETS_OF_ROME;

		if (keymap[3])//2, not the numpad
			jump = LVL5_BASE;

		if (keymap[4])//3, not the numpad
			jump = LVL5_GALLOWS_TREE;

		if (keymap[5])//4, not the numpad
			jump = LVL5_THIRTEENTH_FLOOR;

		if (jump)
		{
			counter = 0;
			timer = 0;
		}

		break;
	}

	return jump;
}

void inject_LoadSave(bool replace)
{
	INJECT(0x004ADF40, CheckKeyConflicts, replace);
	INJECT(0x004B0910, DoStatScreen, replace);
	INJECT(0x004B1E70, DisplayStatsUCunt, replace);//actually in some other file
	INJECT(0x004B18E0, S_DrawAirBar, replace);
	INJECT(0x004B1950, S_DrawHealthBar, replace);
	INJECT(0x004B19C0, S_DrawHealthBar2, replace);
	INJECT(0x004B1890, S_DrawDashBar, replace);
	INJECT(0x004AD460, DoLoadSave, replace);
	INJECT(0x004B1E30, S_MemSet, replace);
	INJECT(0x004B1F00, GetCampaignCheatValue, replace);
}
