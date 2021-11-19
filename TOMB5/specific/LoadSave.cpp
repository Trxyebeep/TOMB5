#include "../tomb5/pch.h"
#include "LoadSave.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "../game/sound.h"
#include "audio.h"
#include "dxsound.h"
#include "input.h"
#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#endif

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

void DoOptions()
{
	char** keyboard_buttons;
	static long menu;	//0: options, 1: controls, 100: special features
	static ulong selection = 1;	//selection
	static ulong selection_bak;
	static ulong controls_selection;	//selection for when mapping keys
	static long music_volume_bar_shade = 0xFF3F3F3F;
	static long sfx_volume_bar_shade = 0xFF3F3F3F;
	static long sfx_bak;	//backup sfx volume
	static long sfx_quality_bak;	//backup sfx quality
	static long sfx_breath_db = -1;
	long textY, textY2, special_features_available, joystick, joystick_x, joystick_y, joy1, joy2, joy3;
	char* text;
	uchar clr, num, num2;
	char quality_buffer[256];
	char quality_text[80];
	static char sfx_backup_flag;	//have we backed sfx stuff up?
	static bool waiting_for_key = 0;

	if (!(sfx_backup_flag & 1))
	{
		sfx_backup_flag |= 1;
		sfx_bak = SFXVolume;
	}

	if (!(sfx_backup_flag & 2))
	{
		sfx_backup_flag |= 2;
		sfx_quality_bak = SoundQuality;
	}

	textY = font_height - 4;

	if (menu == 1)	//controls menu
	{
		if (Gameflow->Language == 2)
			keyboard_buttons = GermanKeyboard;
		else
			keyboard_buttons = KeyboardButtons;

		load_save_options_unk = 1;

		if (ControlMethod)
			num = 11;
		else
			num = 17;

		PrintString(phd_centerx >> 2, font_height, selection & 1 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_CONTROL_METHOD]], 0);
		textY = font_height;
		font_height = (long)((float)phd_winymax * 0.050000001F);
		big_char_height = 10;
		textY2 = font_height + (font_height + (font_height >> 1));

		if (!ControlMethod)
		{
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height), selection & 2 ? 1 : 2, "\x18", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 2 * font_height), selection & 4 ? 1 : 2, "\x1A", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 2 * font_height + font_height), selection & 8 ? 1 : 2, "\x19", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 4 * font_height), selection & 0x10 ? 1 : 2, "\x1B", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 4 * font_height + font_height),  selection & 0x20 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DUCK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 6 * font_height), selection & 0x40 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DASH]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 7 * font_height), selection & 0x80 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_WALK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height), selection & 0x100 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_JUMP]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height + font_height), selection & 0x200 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_ACTION]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 10 * font_height), selection & 0x400 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DRAW_WEAPON]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height + 10 * font_height), selection & 0x800 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_USE_FLARE]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 12 * font_height), selection & 0x1000 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_LOOK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height + 12 * font_height), selection & 0x2000 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_ROLL]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 14 * font_height), selection & 0x4000 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_INVENTORY]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 12 * font_height + 3 * font_height), selection & 0x8000 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_STEP_LEFT]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 16 * font_height), selection & 0x10000 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_STEP_RIGHT]], 0);
			text = (waiting_for_key && (controls_selection & 2)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][0]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height), controls_selection & 2 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 4)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][1]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 2 * font_height), controls_selection & 4 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 8)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][2]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 2 * font_height + font_height), (controls_selection & 8) != 0 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][3]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 4 * font_height), controls_selection & 0x10 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x20)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][4]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 4 * font_height + font_height), controls_selection & 0x20 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x40)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][5]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 6 * font_height), controls_selection & 0x40 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x80)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][6]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 7 * font_height), controls_selection & 0x80 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x100)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][7]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height), controls_selection & 0x100 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x200)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][8]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height + font_height), controls_selection & 0x200 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x400)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][9]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 10 * font_height), controls_selection & 0x400 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x800)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][10]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height + 10 * font_height), controls_selection & 0x800 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x1000)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][11]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 12 * font_height), controls_selection & 0x1000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x2000)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][12]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height + 12 * font_height), controls_selection & 0x2000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x4000)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][13]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 14 * font_height), controls_selection & 0x4000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x8000)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][14]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 12 * font_height + 3 * font_height), controls_selection & 0x8000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10000)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : keyboard_buttons[layout[1][15]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 16 * font_height), controls_selection & 0x10000 ? 1 : 6, text, 0);
		}

		if (ControlMethod == 1)
		{
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 4 * font_height + font_height), selection & 2 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DUCK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 6 * font_height), selection & 4 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DASH]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 7 * font_height), selection & 8 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_WALK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height), selection & 0x10 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_JUMP]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height + font_height), selection & 0x20 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_ACTION]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 10 * font_height), selection & 0x40 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_DRAW_WEAPON]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height + 10 * font_height), selection & 0x80 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_USE_FLARE]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 12 * font_height), selection & 0x100 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_LOOK]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height + 12 * font_height), selection & 0x200 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_ROLL]], 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 14 * font_height), selection & 0x400 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_INVENTORY]], 0);

			for (int i = 0; i < 10; i++)
			{
				sprintf(quality_buffer, "(%s)", keyboard_buttons[layout[1][i + 4]]);
				PrintString((phd_centerx >> 3) + phd_centerx + (phd_centerx >> 1), (ushort)(textY2 + font_height * (i + 5)), 5, quality_buffer, 0);
			}

			text = (waiting_for_key && (controls_selection & 2)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[0]];
			clr = (waiting_for_key && (selection & 2)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 4 * font_height + font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 4)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[1]];
			clr = (waiting_for_key && (selection & 4)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 6 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 8)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[2]];
			clr = (waiting_for_key && (selection & 8)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 7 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[3]];
			clr = (waiting_for_key && (selection & 0x10)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x20)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[4]];
			clr = (waiting_for_key && (selection & 0x20)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height + font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x40)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[5]];
			clr = (waiting_for_key && (selection & 0x40)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 10 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x80)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[6]];
			clr = (waiting_for_key && (selection & 0x80)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height + 10 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x100)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[7]];
			clr = (waiting_for_key && (selection & 0x100)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 12 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x200)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[8]];
			clr = (waiting_for_key && (selection & 0x200)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height + 12 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x400)) ? &gfStringWad[gfStringOffset_bis[STR_WAITING]] : JoyStickButtons[MappedControls[8]];
			clr = (waiting_for_key && (selection & 0x400)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 14 * font_height), clr, text, 0);
		}

		font_height = default_font_height;
		big_char_height = 6;

		if (!ControlMethod)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, &gfStringWad[gfStringOffset_bis[STR_KEYBOARD]], 0);
		else if (ControlMethod == 1)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, &gfStringWad[gfStringOffset_bis[STR_JOYSTICK]], 0);
		else if (ControlMethod == 2)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, &gfStringWad[gfStringOffset_bis[STR_RESET]], 0);

		load_save_options_unk = 0;

		if (ControlMethod < 2 && !waiting_for_key)
		{
			if (dbinput & IN_FORWARD)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				selection >>= 1;
			}

			if (dbinput & IN_BACK)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				selection <<= 1;
			}
		}

		if (waiting_for_key)
		{
			num2 = 0;

			if (keymap[DIK_ESCAPE])
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				controls_selection = 0;
				dbinput = 0;
				waiting_for_key = 0;
				return;
			}

			if (!ControlMethod)
			{
				for (int i = 0; i < 255; i++)
				{
					if (keymap[i] && keyboard_buttons[i])
					{
						if (i != DIK_RETURN && i != DIK_LEFT && i != DIK_RIGHT && i != DIK_UP && i != DIK_DOWN)
						{
							waiting_for_key = 0;

							for (int j = controls_selection >> 2; j; num2++)
								j >>= 1;

							controls_selection = 0;
							layout[1][num2] = i;
						}
					}
				}
			}

			if (ControlMethod == 1)
			{
				joystick = ReadJoystick(joystick_x, joystick_y);

				if (joystick)
				{
					joy1 = selection >> 2;
					joy2 = 0;
					joy3 = 0;

					while (joy1)
					{
						joy1 >>= 1;
						joy2++;
					}

					joy1 = joystick >> 1;

					while (joy1)
					{
						joy1 >>= 1;
						joy3++;
					}

					MappedControls[joy2] = joy3;
					waiting_for_key = 0;
				}
			}

			CheckKeyConflicts();
			dbinput = 0;
		}

		if (dbinput & IN_SELECT && selection > 1 && ControlMethod < 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			controls_selection = selection;
			waiting_for_key = 1;
			memset(keymap, 0, sizeof(keymap));
		}

		if (dbinput & IN_SELECT && ControlMethod == 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, 2);
			memcpy(layout[1], layout, 72);
			ControlMethod = 0;
			memcpy(MappedControls, DefaultControls, 32);
		}

		if (selection & 1)
		{
			if (dbinput & IN_LEFT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				ControlMethod--;
			}

			if (dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				ControlMethod++;
			}

			if (ControlMethod > 2)
				ControlMethod = 2;

			if (ControlMethod < 0)
				ControlMethod = 0;

			if (ControlMethod == 1 && !joystick_read)
			{
				if (dbinput & IN_LEFT)
					ControlMethod = 0;

				if (dbinput & IN_RIGHT)
					ControlMethod = 2;
			}
		}

		if (!selection)
			selection = 1;

		if (selection > (ulong)(1 << (num - 1)))
			selection = 1 << (num - 1);

		if (dbinput & IN_DESELECT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

			if (ControlMethod < 2)
				menu = 0;

			dbinput = 0;
			selection = 1;
		}
	}
	else if (menu == 100)	//special features
	{
		PrintString(phd_centerx, (ushort)(textY + 2 * font_height + font_height), 6, &gfStringWad[gfStringOffset_bis[STR_SPECIAL_FEATURES]], FF_CENTER);

		if (SpecialFeaturesPage[0])
			clr = selection & 1 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 4 * font_height + font_height), clr, &gfStringWad[gfStringOffset_bis[STR_STORYBOARDS_PART_1]], FF_CENTER);

		if (SpecialFeaturesPage[1])
			clr = selection & 2 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 6 * font_height), clr, &gfStringWad[gfStringOffset_bis[STR_NEXT_GENERATION_CONCEPT_ART]], FF_CENTER);

		if (SpecialFeaturesPage[2])
			clr = selection & 4 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 7 * font_height), clr, &gfStringWad[gfStringOffset_bis[STR_STORYBOARDS_PART_2]], FF_CENTER);

		if (SpecialFeaturesPage[3])
			clr = selection & 8 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 8 * font_height), clr, "Gallery", FF_CENTER);

		if (NumSpecialFeatures)
		{
			if (dbinput & IN_FORWARD)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				selection = FindSFCursor(1, selection);
			}

			if (dbinput & IN_BACK)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, 2);
				selection = FindSFCursor(2, selection);
			}

			if (!selection)
				selection = 1;
			else if (selection > 8)
				selection = 8;

			if (dbinput & IN_SELECT)
			{
				if (selection & 1)
					SpecialFeaturesNum = 0;

				if (selection & 2)
					SpecialFeaturesNum = 1;

				if (selection & 4)
					SpecialFeaturesNum = 2;

				if (selection & 8)
					SpecialFeaturesNum = 3;

				if (selection & 16)
					SpecialFeaturesNum = 4;
			}
		}

		if (dbinput & IN_DESELECT)
		{
			menu = 0;	//go back to main options menu
			selection = selection_bak;	//go back to selection
			dbinput &= ~IN_DESELECT;	//don't deselect twice
		}
	}
	else if (menu == 0)	//main options menu
	{
		textY= 3 * font_height;
#ifdef GENERAL_FIXES	//1 more option
		num = 6;
#else
		num = 5;
#endif
		PrintString(phd_centerx, 3 * font_height, 6, &gfStringWad[gfStringOffset_bis[STR_OPTIONS]], FF_CENTER);
		PrintString(phd_centerx, (ushort)(textY + font_height + (font_height >> 1)), selection & 1 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_CONTROL_CONFIGURATION]], FF_CENTER);
		PrintString(phd_centerx >> 2, (ushort)(textY + 2 * font_height + font_height), selection & 2 ? 1 : 2, &gfStringWad[gfStringOffset_bis[STR_MUSIC_VOLUME_BIS]], 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 4 * font_height), selection & 4 ? 1 : 2, & gfStringWad[gfStringOffset_bis[STR_SFX_VOLUME_BIS]], 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 4 * font_height + font_height), selection & 8 ? 1 : 2, & gfStringWad[gfStringOffset_bis[STR_SOUND_QUALITY]], 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 2, & gfStringWad[gfStringOffset_bis[STR_TARGETING]], 0);
		DoSlider(400, 3 * font_height - (font_height >> 1) + textY + 4, 200, 16, MusicVolume, 0xFF1F1F1F, 0xFF3F3FFF, music_volume_bar_shade);
		DoSlider(400, textY + 4 * font_height + 4 - (font_height >> 1), 200, 16, SFXVolume, 0xFF1F1F1F, 0xFF3F3FFF, sfx_volume_bar_shade);

		switch (SoundQuality)
		{
		case 0:
			strcpy(quality_text, &gfStringWad[gfStringOffset_bis[STR_LOW]]);
			break;

		case 1:
			strcpy(quality_text, &gfStringWad[gfStringOffset_bis[STR_MEDIUM]]);
			break;

		case 2:
			strcpy(quality_text, &gfStringWad[gfStringOffset_bis[STR_HIGH]]);
			break;
		}

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 4 * font_height + font_height), selection & 8 ? 1 : 6, quality_text, 0);

		if (App.AutoTarget)
			strcpy(quality_text, &gfStringWad[gfStringOffset_bis[STR_AUTOMATIC]]);
		else
			strcpy(quality_text, &gfStringWad[gfStringOffset_bis[STR_MANUAL]]);

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 6, quality_text, 0);
		special_features_available = 0x20;	//not the most accurate name

		if (gfGameMode == 1)
		{
#ifdef GENERAL_FIXES
			num = 7;
#else
			num = 6;
#endif
			PrintString(phd_centerx, (ushort)((font_height >> 1) + textY + 7 * font_height), selection & 0x20 ? 1 : 2, & gfStringWad[gfStringOffset_bis[STR_SPECIAL_FEATURES]], FF_CENTER);
		}
		else
			special_features_available = 0;

#ifdef GENERAL_FIXES	//if special features are available, print it below them
		if (special_features_available)
			PrintString(phd_centerx, (ushort)((font_height >> 1) + textY + 8 * font_height), selection & 0x40 ? 1 : 2, "tomb5 options", FF_CENTER);
		else
			PrintString(phd_centerx, (ushort)((font_height >> 1) + textY + 7 * font_height), selection & 0x20 ? 1 : 2, "tomb5 options", FF_CENTER);
#endif

		if (dbinput & IN_FORWARD)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			selection >>= 1;
		}

		if (dbinput & IN_BACK)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			selection <<= 1;
		}

		if (dbinput & IN_SELECT && selection & 1)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			menu = 1;
		}

#ifdef GENERAL_FIXES	//time to change some options
		num2 = !special_features_available ? 0x20 : 0x40;

		if (dbinput & IN_SELECT && selection & num2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			selection_bak = selection;
			selection = 1;
			menu = 200;
		}
#endif

		if (!selection)
			selection = 1;

		if (selection > (ulong)(1 << (num - 1)))
			selection = 1 << (num - 1);

		music_volume_bar_shade = 0xFF3F3F3F;
		sfx_volume_bar_shade = 0xFF3F3F3F;

		if (selection & 2)
		{
			sfx_bak = SFXVolume;

			if (input & IN_LEFT || keymap[DIK_LEFT])
				MusicVolume--;

			if (input & IN_RIGHT || keymap[DIK_RIGHT])
				MusicVolume++;

			if (MusicVolume > 100)
				MusicVolume = 100;

			if (MusicVolume < 0)
				MusicVolume = 0;

			sfx_volume_bar_shade = 0xFF3F3F3F;
			music_volume_bar_shade = 0xFF7F7F7F;
			ACMSetVolume();
		}
		else if (selection & 4)
		{
			if (input & IN_LEFT || keymap[DIK_LEFT])
				SFXVolume--;

			if (input & IN_RIGHT || keymap[DIK_RIGHT])
				SFXVolume++;

			if (SFXVolume > 100)
				SFXVolume = 100;

			if (SFXVolume < 0)
				SFXVolume = 0;

			if (SFXVolume != sfx_bak)
			{
				if (sfx_breath_db == -1 || !DSIsChannelPlaying(0))
				{
					S_SoundStopAllSamples();
					sfx_bak = SFXVolume;
					sfx_breath_db = SoundEffect(SFX_LARA_BREATH, 0, SFX_DEFAULT);
					DSChangeVolume(0, -100 * ((int)(100 - SFXVolume) >> 1));
				}
				else if (sfx_breath_db != -1 && DSIsChannelPlaying(0))
					DSChangeVolume(0, -100 * ((100 - SFXVolume) >> 1));
			}

			music_volume_bar_shade = 0xFF3F3F3F;
			sfx_volume_bar_shade = 0xFF7F7F7F;
		}
		else if (selection & 8)
		{
			sfx_bak = SFXVolume;
			
			if (dbinput & IN_LEFT)
				SoundQuality--;

			if (dbinput & IN_RIGHT)
				SoundQuality++;

			if (SoundQuality > 2)
				SoundQuality = 2;

			if (SoundQuality < 0)
				SoundQuality = 0;

			if (SoundQuality != sfx_quality_bak)
			{
				S_SoundStopAllSamples();
				DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
				sfx_quality_bak = SoundQuality;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}
		}
		else if (selection & 16)
		{
			if (dbinput & IN_LEFT)
			{
				if (App.AutoTarget)
					App.AutoTarget = 0;

				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			if (dbinput & IN_RIGHT)
			{
				if (!App.AutoTarget)
					App.AutoTarget = 1;

				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			savegame.AutoTarget = (uchar)App.AutoTarget;
		}
		else if (selection & special_features_available && dbinput & IN_SELECT)
		{
			CalculateNumSpecialFeatures();
			selection_bak = selection;
			selection = 1;
			menu = 100;
		}
	}
#ifdef GENERAL_FIXES	//new menu
	else if (menu == 200)
	{
		num = 5;
		PrintString(phd_centerx, 2 * font_height, 6, "New tomb5 options", FF_CENTER);
		PrintString(phd_centerx >> 2, (ushort)(textY + 3 * font_height), selection & 1 ? 1 : 2, "FootPrints", 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 4 * font_height), selection & 2 ? 1 : 2, "Point light shadows", 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 5 * font_height), selection & 4 ? 1 : 2, "Shadow mode", 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 6 * font_height), selection & 8 ? 1 : 2, "Fix climb up delay", 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 7 * font_height), selection & 0x10 ? 1 : 2, "Flexible crawling", 0);

		if (dbinput & IN_FORWARD)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			selection >>= 1;
		}

		if (dbinput & IN_BACK)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			selection <<= 1;
		}

		if (!selection)
			selection = 1;

		if (selection > (ulong)(1 << (num - 1)))
			selection = 1 << (num - 1);

		if (dbinput & IN_DESELECT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			menu = 0;
			dbinput &= ~IN_DESELECT;
			selection = selection_bak;
		}

		if (tomb5.footprints)
			strcpy(quality_text, "on");
		else
			strcpy(quality_text, "off");

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 3 * font_height), selection & 1 ? 1 : 6, quality_text, 0);

		if (tomb5.tr4_point_lights)
			strcpy(quality_text, "TR4: stronger shadows");
		else
			strcpy(quality_text, "TR5: more coverage");

		PrintString(phd_centerx + (phd_centerx >> 1), (ushort)(textY + 4 * font_height), selection & 2 ? 1 : 6, quality_text, FF_CENTER);

		if (!tomb5.shadow_mode)
			strcpy(quality_text, "original");
		else if (tomb5.shadow_mode == 1)
			strcpy(quality_text, "circle");
		else if (tomb5.shadow_mode == 2)
			strcpy(quality_text, "PSX-like");

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 5 * font_height), selection & 4 ? 1 : 6, quality_text, 0);

		if (tomb5.fix_climb_up_delay)
			strcpy(quality_text, "on");
		else
			strcpy(quality_text, "off");

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 6 * font_height), selection & 8 ? 1 : 6, quality_text, 0);

		if (tomb5.flexible_crawling)
			strcpy(quality_text, "on");
		else
			strcpy(quality_text, "off");

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 7 * font_height), selection & 0x10 ? 1 : 6, quality_text, 0);

		if (selection & 1)
		{
			if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.footprints = !tomb5.footprints;
				save_new_tomb5_settings();
			}
		}
		else if (selection & 2)
		{
			if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.tr4_point_lights = !tomb5.tr4_point_lights;
				save_new_tomb5_settings();
			}
		}
		else if (selection & 4)
		{
			if (dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.shadow_mode++;

				if (tomb5.shadow_mode > 2)
					tomb5.shadow_mode = 0;

				save_new_tomb5_settings();
			}

			if (dbinput & IN_LEFT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.shadow_mode--;

				if (tomb5.shadow_mode < 0)
					tomb5.shadow_mode = 2;

				save_new_tomb5_settings();
			}
		}
		else if (selection & 8)
		{
			if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.fix_climb_up_delay = !tomb5.fix_climb_up_delay;
				save_new_tomb5_settings();
			}
		}
		else if (selection & 0x10)
		{
			if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				tomb5.flexible_crawling = !tomb5.flexible_crawling;
				save_new_tomb5_settings();
			}
		}
	}
#endif
}

void inject_LoadSave(bool replace)
{
	INJECT(0x004ADF40, CheckKeyConflicts, replace);
	INJECT(0x004B0910, DoStatScreen, replace);
	INJECT(0x004B1E70, DisplayStatsUCunt, replace);
	INJECT(0x004B18E0, S_DrawAirBar, replace);
	INJECT(0x004B1950, S_DrawHealthBar, replace);
	INJECT(0x004B19C0, S_DrawHealthBar2, replace);
	INJECT(0x004B1890, S_DrawDashBar, replace);
	INJECT(0x004AD460, DoLoadSave, replace);
	INJECT(0x004B1E30, S_MemSet, replace);
	INJECT(0x004B1F00, GetCampaignCheatValue, replace);
	INJECT(0x004ADF90, DoOptions, replace);
}
