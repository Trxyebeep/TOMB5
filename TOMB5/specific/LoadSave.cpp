#include "../tomb5/pch.h"
#include "LoadSave.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "../game/sound.h"
#include "audio.h"
#include "dxsound.h"
#include "input.h"
#include "function_table.h"
#include "drawroom.h"
#include "polyinsert.h"
#include "winmain.h"
#include "output.h"
#include "dxshell.h"
#include "texture.h"
#include "function_stubs.h"
#include "../game/savegame.h"
#include "gamemain.h"
#include "specificfx.h"
#include "time.h"
#include "file.h"
#include "fmv.h"
#include "../game/newinv2.h"
#include "../game/control.h"
#include "3dmath.h"
#include "../tomb5/troyestuff.h"
#include "../tomb5/tomb5.h"
#include "drawbars.h"

long sfx_frequencies[3] = { 11025, 22050, 44100 };
long SoundQuality = 1;
long MusicVolume = 40;
long SFXVolume = 80;
long ControlMethod;

static DXTEXTURE MonoScreen;
char MonoScreenOn;

static long SpecialFeaturesNum = -1;
static long NumSpecialFeatures;

static LPDIRECTDRAWSURFACE4 screen_surface;
static SAVEFILE_INFO SaveGames[15];
static char SpecialFeaturesPage[5];

static const char* screen_paths[5] =
{
	"SCREENS\\STORY1.STR",
	"SCREENS\\NXG.STR",
	"SCREENS\\STORY2.STR",
	"SCREENS\\GALLERY.STR",
	"SCREENS\\SCREENS.STR"
};

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

void DisplayStatsUCunt()
{
	long ypos;
	short Days, Hours, Min, Sec;
	char buffer[40];

	ypos = phd_centery - 4 * font_height;
	PrintString(phd_centerx, ypos, 6, SCRIPT_TEXT(TXT_Statistics), FF_CENTER);
	PrintString(phd_centerx, ypos + 2 * font_height, 2, SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]), FF_CENTER);
	PrintString(phd_centerx >> 2, ypos + 3 * font_height, 2, SCRIPT_TEXT(TXT_Time_Taken), 0);
	PrintString(phd_centerx >> 2, ypos + 4 * font_height, 2, SCRIPT_TEXT(TXT_Distance_Travelled), 0);
	PrintString(phd_centerx >> 2, ypos + 5 * font_height, 2, SCRIPT_TEXT(TXT_Ammo_Used), 0);
	PrintString(phd_centerx >> 2, ypos + 6 * font_height, 2, SCRIPT_TEXT(TXT_Health_Packs_Used), 0);
	PrintString(phd_centerx >> 2, ypos + 7 * font_height, 2, SCRIPT_TEXT(TXT_Secrets_Found), 0);

	Sec = short(GameTimer / 30);
	Days = Sec / (24 * 60 * 60);
	Hours = (Sec % (24 * 60 * 60)) / (60 * 60);
	Min = (Sec / 60) % 60;
	Sec = (Sec % 60);

	sprintf(buffer, "%02d:%02d:%02d", (Days * 24) + Hours, Min, Sec);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 3 * font_height, 6, buffer, 0);
	sprintf(buffer, "%dm", savegame.Game.Distance / 419);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 4 * font_height, 6, buffer, 0);
	sprintf(buffer, "%d", savegame.Game.AmmoUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 5 * font_height, 6, buffer, 0);
	sprintf(buffer, "%d", savegame.Game.HealthUsed);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 6 * font_height, 6, buffer, 0);
	sprintf(buffer, "%d / 36", savegame.Game.Secrets);
	PrintString(phd_centerx + (phd_centerx >> 2), ypos + 7 * font_height, 6, buffer, 0);
}

long DoLoadSave(long LoadSave)
{
	SAVEFILE_INFO* pSave;
	static long selection;
	long txt, l, f;
	uchar color;
	char string[80];
	char name[41];

	if (LoadSave & IN_SAVE)
		txt = TXT_Save_Game;
	else
		txt = TXT_Load_Game;

	f = font_height;
	PrintString(phd_centerx, f, 6, SCRIPT_TEXT(txt), FF_CENTER);
	f += font_height;
	f += font_height >> 1;

	for (int i = 0; i < 15; i++)
	{
		pSave = &SaveGames[i];
		color = 2;

		if (i == selection)
			color = 1;

		memset(name, ' ', 40);
		l = strlen(pSave->name);

		if (l > 40)
			l = 40;

		strncpy(name, pSave->name, l);
		name[40] = 0;

		if (pSave->valid)
		{
			wsprintf(string, "%03d", pSave->num);
			PrintString(GetFixedScale(10), f, color, string, 0);
			PrintString(GetFixedScale(60), f, color, name, 0);
			wsprintf(string, "%d %s %02d:%02d:%02d", pSave->days, SCRIPT_TEXT(TXT_days), pSave->hours, pSave->minutes, pSave->seconds);
			PrintString(phd_centerx - long((float)phd_winwidth / 640.0F * -135.0), f, color, string, 0);
		}
		else
		{
			wsprintf(string, "%s", pSave->name);
			PrintString(phd_centerx, f, color, string, FF_CENTER);
		}

		f += font_height;
	}

	if (dbinput & IN_FORWARD)
	{
		selection--;
		SoundEffect(SFX_MENU_CHOOSE, 0, SFX_DEFAULT);
	}

	if (dbinput & IN_BACK)
	{
		selection++;
		SoundEffect(SFX_MENU_CHOOSE, 0, SFX_DEFAULT);
	}

	if (selection < 0)
		selection = 0;

	if (selection > 14)
		selection = 14;

	if (dbinput & IN_SELECT)
	{
		if (SaveGames[selection].valid || LoadSave == IN_SAVE)
			return selection;
		else
			SoundEffect(SFX_LARA_NO, 0, SFX_DEFAULT);
	}

	return -1;
}

long GetCampaignCheatValue()
{
	static long counter = 0;
	static long timer;
	long jump;

	if (timer)
		timer--;
	else
		counter = 0;

	jump = 0;

	switch (counter)
	{
	case 0:

		if (keymap[DIK_F])
		{
			timer = 450;
			counter = 1;
		}

		break;

	case 1:

		if (keymap[DIK_I])
			counter = 2;

		break;

	case 2:

		if (keymap[DIK_L])
			counter = 3;

		break;

	case 3:
		if (keymap[DIK_T])
			counter = 4;

		break;

	case 4:

		if (keymap[DIK_H])
			counter = 5;

		break;

	case 5:

		if (keymap[DIK_Y])
			counter = 6;

		break;

	case 6:

		if (keymap[DIK_1])
			jump = LVL5_STREETS_OF_ROME;

		if (keymap[DIK_2])
			jump = LVL5_BASE;

		if (keymap[DIK_3])
			jump = LVL5_GALLOWS_TREE;

		if (keymap[DIK_4])
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
	const char** keyboard_buttons;
	static long menu;	//0: options, 1: controls, 100: special features
	static ulong sel = 1;	//selection
	static ulong last_sel;
	static ulong sel2;		//selection for when mapping keys
	static long mSliderCol = 0x3F3F3F;
	static long sSliderCol = 0x3F3F3F;
	static long sfx_bak;
	static long sfx_quality_bak;
	static long sfx_breath_db = -1;
	ulong nMask;
	long f, y, i, SFMask, jread, jx, jy, lp, lp2;
	const char* text;
	char buf[256];
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

	f = font_height - 4;

	if (menu == 1)	//controls menu
	{
		if (Gameflow->Language == 2)
			keyboard_buttons = GermanKeyboard;
		else
			keyboard_buttons = KeyboardButtons;

		if (ControlMethod)
			nMask = 11;
		else
			nMask = 17;

		PrintString(phd_centerx >> 2, font_height, sel & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Control_Method), 0);

		font_height = GetFixedScale(27);

		if (!ControlMethod)
		{
			y = 2;
			i = 1;

			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, "\x18", 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, "\x1A", 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, "\x19", 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, "\x1B", 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Duck), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Dash), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Walk), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Jump), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Action), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Draw_Weapon), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Use_Flare), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Look), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Roll), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Inventory), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Step_Left), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Step_Right), 0);

			y = 2;
			i = 1;

			for (lp = 0; lp < 16; lp++)
			{
				text = (waiting_for_key && (sel2 & (1 << i))) ? SCRIPT_TEXT(TXT_Waiting) : keyboard_buttons[layout[1][lp]];
				PrintString(phd_centerx + (phd_centerx >> 2), y++ * font_height, sel2 & (1 << i++) ? 1 : 6, text, 0);
			}
		}

		if (ControlMethod == 1)
		{
			y = 3;
			i = 1;

			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Duck), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Dash), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Walk), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Jump), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Action), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Draw_Weapon), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Use_Flare), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Look), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Roll), 0);
			PrintString(phd_centerx >> 2, y++ * font_height, sel & (1 << i++) ? 1 : 2, SCRIPT_TEXT(TXT_Inventory), 0);

			for (lp = 0, f = 3; lp < 10; lp++, f++)
			{
				sprintf(buf, "(%s)", keyboard_buttons[layout[1][lp + 4]]);
				PrintString(phd_centerx + (phd_centerx >> 3) + (phd_centerx >> 1), f * font_height, 5, buf, 0);
			}

			y = 3;
			i = 1;

			for (lp = 0; lp < 10; lp++)
			{
				text = (waiting_for_key && (sel2 & (1 << i))) ? SCRIPT_TEXT(TXT_Waiting) : JoyStickButtons[jLayout[lp]];
				PrintString(phd_centerx + (phd_centerx >> 2), y++ * font_height, (waiting_for_key && (sel & (1 << i++))) ? 1 : 6, text, 0);
			}
		}

		font_height = default_font_height;

		if (!ControlMethod)
			PrintString(phd_centerx + (phd_centerx >> 2), font_height, sel2 & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Keyboard), 0);
		else if (ControlMethod == 1)
			PrintString(phd_centerx + (phd_centerx >> 2), font_height, sel2 & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Joystick), 0);
		else if (ControlMethod == 2)
			PrintString(phd_centerx + (phd_centerx >> 2), font_height, sel2 & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Reset), 0);

		if (ControlMethod < 2 && !waiting_for_key)
		{
			if (dbinput & IN_FORWARD)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				sel >>= 1;
			}

			if (dbinput & IN_BACK)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				sel <<= 1;
			}
		}

		if (waiting_for_key)
		{
			i = 0;

			if (keymap[DIK_ESCAPE])
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				sel2 = 0;
				dbinput = 0;
				waiting_for_key = 0;
				return;
			}

			if (!ControlMethod)
			{
				for (lp = 0; lp < 255; lp++)
				{
					if (keymap[lp] && keyboard_buttons[lp])
					{
						if (lp != DIK_RETURN && lp != DIK_LEFT && lp != DIK_RIGHT && lp != DIK_UP && lp != DIK_DOWN)
						{
							waiting_for_key = 0;

							sel2 >>= 2;

							while (sel2)
							{
								i++;
								sel2 >>= 1;
							}

							sel2 = 0;
							layout[1][i] = (short)lp;
						}
					}
				}
			}

			if (ControlMethod == 1)
			{
				jread = ReadJoystick(jx, jy);

				if (jread)
				{
					lp = sel >> 2;
					lp2 = 0;
					i = 0;

					while (lp)
					{
						lp >>= 1;
						i++;
					}

					lp = jread >> 1;

					while (lp)
					{
						lp >>= 1;
						lp2++;
					}

					jLayout[i] = lp2;
					waiting_for_key = 0;
				}
			}

			CheckKeyConflicts();
			dbinput = 0;
		}

		if (dbinput & IN_SELECT && sel > 1 && ControlMethod < 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			sel2 = sel;
			waiting_for_key = 1;
			memset(keymap, 0, sizeof(keymap));
		}

		if (dbinput & IN_SELECT && ControlMethod == 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, 2);
			memcpy(layout[1], layout, 72);
			ControlMethod = 0;
			memcpy(jLayout, defaultJLayout, 32);
		}

		if (sel & 1)
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
		}

		if (!sel)
			sel = 1;

		if (sel > ulong(1 << (nMask - 1)))
			sel = 1 << (nMask - 1);

		if (dbinput & IN_DESELECT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

			if (ControlMethod < 2)
				menu = 0;

			dbinput = 0;
			sel = 1;
		}
	}
	else if (menu == 100)	//special features
	{
		PrintString(phd_centerx, f + 3 * font_height, 6, SCRIPT_TEXT(TXT_Special_Features), FF_CENTER);
		PrintString(phd_centerx, f + 5 * font_height, SpecialFeaturesPage[0] ? (sel & 1 ? 1 : 2) : 3, SCRIPT_TEXT(TXT_Storyboards_Part_1), FF_CENTER);
		PrintString(phd_centerx, f + 6 * font_height, SpecialFeaturesPage[1] ? (sel & 2 ? 1 : 2) : 3, SCRIPT_TEXT(TXT_Next_Generation_Concept), FF_CENTER);
		PrintString(phd_centerx, f + 7 * font_height, SpecialFeaturesPage[2] ? (sel & 4 ? 1 : 2) : 3, SCRIPT_TEXT(TXT_Storyboards_Part_2), FF_CENTER);
		PrintString(phd_centerx, f + 8 * font_height, SpecialFeaturesPage[3] ? (sel & 8 ? 1 : 2) : 3, "Gallery", FF_CENTER);

		if (NumSpecialFeatures)
		{
			if (dbinput & IN_FORWARD)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
				sel = FindSFCursor(1, sel);
			}

			if (dbinput & IN_BACK)
			{
				SoundEffect(SFX_MENU_CHOOSE, 0, 2);
				sel = FindSFCursor(2, sel);
			}

			if (!sel)
				sel = 1;
			else if (sel > 8)
				sel = 8;

			if (dbinput & IN_SELECT)
			{
				if (sel & 1)
					SpecialFeaturesNum = 0;

				if (sel & 2)
					SpecialFeaturesNum = 1;

				if (sel & 4)
					SpecialFeaturesNum = 2;

				if (sel & 8)
					SpecialFeaturesNum = 3;

				if (sel & 0x10)
					SpecialFeaturesNum = 4;
			}
		}

		if (dbinput & IN_DESELECT)
		{
			menu = 0;
			sel = last_sel;
			dbinput &= ~IN_DESELECT;
		}
	}
	else if (menu == 0)	//main options menu
	{
		f= 3 * font_height;
		nMask = 6;
		PrintString(phd_centerx, 3 * font_height, 6, SCRIPT_TEXT(TXT_Options), FF_CENTER);
		PrintString(phd_centerx, f + font_height + (font_height >> 1), sel & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Control_Configuration), FF_CENTER);
		PrintString(phd_centerx >> 2, f + 3 * font_height, sel & 2 ? 1 : 2, SCRIPT_TEXT(TXT_Music_Volume), 0);
		PrintString(phd_centerx >> 2, f + 4 * font_height, sel & 4 ? 1 : 2, SCRIPT_TEXT(TXT_SFX_Volume), 0);
		PrintString(phd_centerx >> 2, f + 5 * font_height, sel & 8 ? 1 : 2, SCRIPT_TEXT(TXT_Sound_Quality), 0);
		PrintString(phd_centerx >> 2, f + 6 * font_height, sel & 0x10 ? 1 : 2, SCRIPT_TEXT(TXT_Targeting), 0);
		DoSlider(400, f + 3 * font_height + 4 - (font_height >> 1), 200, 16, MusicVolume, 0x1F1F1F, 0x3F3FFF, mSliderCol);
		DoSlider(400, f + 4 * font_height + 4 - (font_height >> 1), 200, 16, SFXVolume, 0x1F1F1F, 0x3F3FFF, sSliderCol);

		switch (SoundQuality)
		{
		case 0:
			strcpy(buf, SCRIPT_TEXT(TXT_Low));
			break;

		case 1:
			strcpy(buf, SCRIPT_TEXT(TXT_Medium));
			break;

		case 2:
			strcpy(buf, SCRIPT_TEXT(TXT_High));
			break;
		}

		PrintString(phd_centerx + (phd_centerx >> 2), f + 5 * font_height, sel & 8 ? 1 : 6, buf, 0);

		if (App.AutoTarget)
			strcpy(buf, SCRIPT_TEXT(TXT_Automatic));
		else
			strcpy(buf, SCRIPT_TEXT(TXT_Manual));

		PrintString(phd_centerx + (phd_centerx >> 2), f + 6 * font_height, sel & 0x10 ? 1 : 6, buf, 0);
		SFMask = 0x20;

		if (gfGameMode == 1)
		{
			nMask = 7;
			PrintString(phd_centerx, (font_height >> 1) + f + 7 * font_height, sel & 0x20 ? 1 : 2, SCRIPT_TEXT(TXT_Special_Features), FF_CENTER);
		}
		else
			SFMask = 0;

		if (SFMask)
			PrintString(phd_centerx, (font_height >> 1) + f + 8 * font_height, sel & 0x40 ? 1 : 2, "tomb5 options", FF_CENTER);
		else
			PrintString(phd_centerx, (font_height >> 1) + f + 7 * font_height, sel & 0x20 ? 1 : 2, "tomb5 options", FF_CENTER);

		if (dbinput & IN_FORWARD)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			sel >>= 1;
		}

		if (dbinput & IN_BACK)
		{
			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			sel <<= 1;
		}

		if (dbinput & IN_SELECT && sel & 1)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			menu = 1;
		}

		i = !SFMask ? 0x20 : 0x40;

		if (dbinput & IN_SELECT && sel & (ulong)i)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			last_sel = sel;
			sel = 1;
			menu = 200;
		}

		if (!sel)
			sel = 1;

		if (sel > ulong(1 << (nMask - 1)))
			sel = 1 << (nMask - 1);

		mSliderCol = 0x3F3F3F;
		sSliderCol = 0x3F3F3F;

		if (sel & 2)
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

			sSliderCol = 0x3F3F3F;
			mSliderCol = 0x7F7F7F;
			ACMSetVolume();
		}
		else if (sel & 4)
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
					DSChangeVolume(0, -100 * (long(100 - SFXVolume) >> 1));
				}
				else if (sfx_breath_db != -1 && DSIsChannelPlaying(0))
					DSChangeVolume(0, -100 * ((100 - SFXVolume) >> 1));
			}

			mSliderCol = 0x3F3F3F;
			sSliderCol = 0x7F7F7F;
		}
		else if (sel & 8)
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
		else if (sel & 0x10)
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
		else if (sel & SFMask && dbinput & IN_SELECT)
		{
			CalculateNumSpecialFeatures();
			last_sel = sel;
			sel = 1;
			menu = 100;
		}
	}
	else if (menu == 200)
		TroyeMenu(font_height - (font_height >> 1), menu, sel, last_sel);
}

void CreateMonoScreen()
{
	MonoScreenOn = 1;
	ConvertSurfaceToTextures(App.dx.lpBackBuffer);
}

void RGBM_Mono(uchar* r, uchar* g, uchar* b)
{
	uchar c;

	if (MonoScreenOn == 2)
		return;

	if (tomb5.inv_bg_mode == 3)
		return;

	c = (*r + *b) >> 1;
	*r = c;
	*g = c;
	*b = c;
}

static void BitMaskGetNumberOfBits(ulong bitMask, ulong* bitDepth, ulong* bitOffset)
{
	long i;

	if (!bitMask)
	{
		*bitOffset = 0;
		*bitDepth = 0;
		return;
	}

	for (i = 0; !(bitMask & 1); i++)
		bitMask >>= 1;

	*bitOffset = i;

	for (i = 0; bitMask != 0; i++)
		bitMask >>= 1;

	*bitDepth = i;
}

static void WinVidGetColorBitMasks(COLOR_BIT_MASKS* bm, LPDDPIXELFORMAT pixelFormat)
{
	bm->dwRBitMask = pixelFormat->dwRBitMask;
	bm->dwGBitMask = pixelFormat->dwGBitMask;
	bm->dwBBitMask = pixelFormat->dwBBitMask;
	bm->dwRGBAlphaBitMask = pixelFormat->dwRGBAlphaBitMask;

	BitMaskGetNumberOfBits(bm->dwRBitMask, &bm->dwRBitDepth, &bm->dwRBitOffset);
	BitMaskGetNumberOfBits(bm->dwGBitMask, &bm->dwGBitDepth, &bm->dwGBitOffset);
	BitMaskGetNumberOfBits(bm->dwBBitMask, &bm->dwBBitDepth, &bm->dwBBitOffset);
	BitMaskGetNumberOfBits(bm->dwRGBAlphaBitMask, &bm->dwRGBAlphaBitDepth, &bm->dwRGBAlphaBitOffset);
}

static void CustomBlt(DDSURFACEDESC2* dst, ulong dstX, ulong dstY, DDSURFACEDESC2* src, LPRECT srcRect)
{
	COLOR_BIT_MASKS srcMask, dstMask;
	uchar* srcLine;
	uchar* dstLine;
	uchar* srcPtr;
	uchar* dstPtr;
	ulong srcX, srcY, width, height, srcBpp, dstBpp, color, high, low, r, g, b;

	srcX = srcRect->left;
	srcY = srcRect->top;
	width = srcRect->right - srcRect->left;
	height = srcRect->bottom - srcRect->top;
	srcBpp = src->ddpfPixelFormat.dwRGBBitCount / 8;
	dstBpp = dst->ddpfPixelFormat.dwRGBBitCount / 8;
	WinVidGetColorBitMasks(&srcMask, &src->ddpfPixelFormat);
	WinVidGetColorBitMasks(&dstMask, &dst->ddpfPixelFormat);
	srcLine = (uchar*)src->lpSurface + srcY * src->lPitch + srcX * srcBpp;
	dstLine = (uchar*)dst->lpSurface + dstY * dst->lPitch + dstX * dstBpp;

	for (ulong j = 0; j < height; j++)
	{
		srcPtr = srcLine;
		dstPtr = dstLine;

		for (ulong i = 0; i < width; i++)
		{
			color = 0;
			memcpy(&color, srcPtr, srcBpp);
			r = ((color & srcMask.dwRBitMask) >> srcMask.dwRBitOffset);
			g = ((color & srcMask.dwGBitMask) >> srcMask.dwGBitOffset);
			b = ((color & srcMask.dwBBitMask) >> srcMask.dwBBitOffset);

			if (srcMask.dwRBitDepth < dstMask.dwRBitDepth)
			{
				high = dstMask.dwRBitDepth - srcMask.dwRBitDepth;
				low = (srcMask.dwRBitDepth > high) ? srcMask.dwRBitDepth - high : 0;
				r = (r << high) | (r >> low);
			}
			else if (srcMask.dwRBitDepth > dstMask.dwRBitDepth)
				r >>= srcMask.dwRBitDepth - dstMask.dwRBitDepth;

			if (srcMask.dwGBitDepth < dstMask.dwGBitDepth)
			{
				high = dstMask.dwGBitDepth - srcMask.dwGBitDepth;
				low = (srcMask.dwGBitDepth > high) ? srcMask.dwGBitDepth - high : 0;
				g = (g << high) | (g >> low);
			}
			else if (srcMask.dwGBitDepth > dstMask.dwGBitDepth)
				g >>= srcMask.dwGBitDepth - dstMask.dwGBitDepth;


			if (srcMask.dwBBitDepth < dstMask.dwBBitDepth)
			{
				high = dstMask.dwBBitDepth - srcMask.dwBBitDepth;
				low = (srcMask.dwBBitDepth > high) ? srcMask.dwBBitDepth - high : 0;
				b = (b << high) | (b >> low);
			}
			else if (srcMask.dwBBitDepth > dstMask.dwBBitDepth)
				b >>= srcMask.dwBBitDepth - dstMask.dwBBitDepth;

			RGBM_Mono((uchar*)&r, (uchar*)&g, (uchar*)&b);
			color = dst->ddpfPixelFormat.dwRGBAlphaBitMask; // destination is opaque
			color |= r << dstMask.dwRBitOffset;
			color |= g << dstMask.dwGBitOffset;
			color |= b << dstMask.dwBBitOffset;
			memcpy(dstPtr, &color, dstBpp);
			srcPtr += srcBpp;
			dstPtr += dstBpp;
		}

		srcLine += src->lPitch;
		dstLine += dst->lPitch;
	}
}

void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface)
{
	DDSURFACEDESC2 tSurf;
	DDSURFACEDESC2 uSurf;
	RECT r;
	ushort* pTexture;
	ushort* pSrc;

	memset(&tSurf, 0, sizeof(tSurf));
	tSurf.dwSize = sizeof(DDSURFACEDESC2);
	surface->Lock(0, &tSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	pSrc = (ushort*)tSurf.lpSurface;
	MonoScreen.surface = CreateTexturePage(tSurf.dwWidth, tSurf.dwHeight, 0, NULL, RGBM_Mono, -1);

	memset(&uSurf, 0, sizeof(uSurf));
	uSurf.dwSize = sizeof(DDSURFACEDESC2);
	MonoScreen.surface->Lock(0, &uSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	pTexture = (ushort*)uSurf.lpSurface;

	r.left = 0;
	r.top = 0;
	r.right = tSurf.dwWidth;
	r.bottom = tSurf.dwHeight;
	CustomBlt(&uSurf, 0, 0, &tSurf, &r);

	MonoScreen.surface->Unlock(0);
	DXAttempt(MonoScreen.surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen.tex));
	surface->Unlock(0);
}

void FreeMonoScreen()
{
	if (MonoScreenOn == 1)
	{
		if (MonoScreen.surface)
		{
			Log("Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen.surface, MonoScreen.surface->Release());
			MonoScreen.surface = 0;
		}
		else
			Log("%s Attempt To Release NULL Ptr", "Mono Screen Surface");

		if (MonoScreen.tex)
		{
			Log("Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen.tex, MonoScreen.tex->Release());
			MonoScreen.tex = 0;
		}
		else
			Log("%s Attempt To Release NULL Ptr", "Mono Screen Texture");
	}

	MonoScreenOn = 0;
}

void S_DrawTile(long x, long y, long w, long h, IDirect3DTexture2* t, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3)
{
	D3DTLBUMPVERTEX v[4];
	D3DTLBUMPVERTEX tri[3];
	float u1, v1, u2, v2;

	u1 = float(tU * (1.0F / 256.0F));
	v1 = float(tV * (1.0F / 256.0F));
	u2 = float((tW + tU) * (1.0F / 256.0F));
	v2 = float((tH + tV) * (1.0F / 256.0F));

	v[0].sx = (float)x;
	v[0].sy = (float)y;
	v[0].sz = 0.995F;
	v[0].tu = u1;
	v[0].tv = v1;
	v[0].rhw = 1;
	v[0].color = c0;
	v[0].specular = 0xFF000000;

	v[1].sx = float(w + x);
	v[1].sy = (float)y;
	v[1].sz = 0.995F;
	v[1].tu = u2;
	v[1].tv = v1;
	v[1].rhw = 1;
	v[1].color = c1;
	v[1].specular = 0xFF000000;

	v[2].sx = float(w + x);
	v[2].sy = float(h + y);
	v[2].sz = 0.995F;
	v[2].tu = u2;
	v[2].tv = v2;
	v[2].rhw = 1;
	v[2].color = c3;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)x;
	v[3].sy = float(h + y);
	v[3].sz = 0.995F;
	v[3].tu = u1;
	v[3].tv = v2;
	v[3].rhw = 1;
	v[3].color = c2;
	v[3].specular = 0xFF000000;

	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);
	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, t));
	tri[0] = v[0];
	tri[1] = v[2];
	tri[2] = v[3];
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, v, 3, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, tri, 3, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);

	if (App.Filtering)
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
	}
}

void S_DisplayMonoScreen()
{
	ulong col;

	if (MonoScreenOn == 1 || MonoScreenOn == 2)
	{
		if (MonoScreenOn == 2)	//pictures always the same!!
			col = 0xFFFFFFFF;
		else
		{
			if (tomb5.inv_bg_mode == 2 || tomb5.inv_bg_mode == 3)
				col = 0xFFFFFFFF;
			else
				col = 0xFFFFFF80;
		}

		S_DrawTile(0, 0, phd_winxmax, phd_winymax, MonoScreen.tex, 0, 0, 256, 256, col, col, col, col);
	}
}

long S_LoadSave(long load_or_save, long mono, long inv_active)
{
	long fade, ret;

	fade = 0;

	if (!mono)
		CreateMonoScreen();

	GetSaveLoadFiles();

	if (!inv_active)
		InventoryActive = 1;

	while (1)
	{
		S_InitialisePolyList();

		if (fade)
			dbinput = 0;
		else
			S_UpdateInput();

		SetDebounce = 1;
		S_DisplayMonoScreen();
		ret = DoLoadSave(load_or_save);
		UpdatePulseColour();
		S_OutputPolyList();
		S_DumpScreen();

		if (ret >= 0)
		{
			if (load_or_save & IN_SAVE)
			{
				sgSaveGame();
				S_SaveGame(ret);
				GetSaveLoadFiles();
				break;
			}

			fade = ret + 1;
			S_LoadGame(ret);
			SetFade(0, 255);
			ret = -1;
		}

		if (fade && DoFade == 2)
		{
			ret = fade - 1;
			break;
		}

		if (input & IN_OPTION)
		{
			ret = -1;
			break;
		}

		if (MainThread.ended)
			break;
	}

	TIME_Init();

	if (!mono)
		FreeMonoScreen();

	if (!inv_active)
		InventoryActive = 0;

	return ret;
}

void LoadScreen(long screen, long pathNum)
{
	FILE* file;
	DDPIXELFORMAT pixel_format;
	DDSURFACEDESC2 surf;
	void* pic;
	ushort* pSrc;
	ushort* pDst;
	ushort col, r, g, b;

	memset(&surf, 0, sizeof(surf));
	memset(&pixel_format, 0, sizeof(pixel_format));
	surf.dwSize = sizeof(DDSURFACEDESC2);
	surf.dwWidth = 640;
	surf.dwHeight = 480;
	pixel_format.dwSize = sizeof(DDPIXELFORMAT);
	pixel_format.dwFlags = DDPF_RGB;
	pixel_format.dwRGBBitCount = 16;
	pixel_format.dwRBitMask = 0xF800;
	pixel_format.dwGBitMask = 0x7E0;
	pixel_format.dwBBitMask = 0x1F;
	pixel_format.dwRGBAlphaBitMask = 0;
	memcpy(&surf.ddpfPixelFormat, &pixel_format, sizeof(surf.ddpfPixelFormat));
	surf.dwFlags = DDSD_PIXELFORMAT | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS;
	surf.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	file = FileOpen(screen_paths[pathNum]);

	if (DXCreateSurface(G_dxptr->lpDD, &surf, &screen_surface) && file)
	{
		pic = malloc(0x96000);
		fseek(file, 0x96000 * screen, SEEK_SET);
		fread(pic, 0x96000, 1, file);
		fclose(file);
		memset(&surf, 0, sizeof(surf));
		surf.dwSize = sizeof(DDSURFACEDESC2);
		screen_surface->Lock(0, &surf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
		pDst = (ushort*)surf.lpSurface;
		pSrc = (ushort*)pic;
		
		for (int i = 0; i < 0x4B000; i++, pSrc++, pDst++)
		{
			col = *pSrc;
			r = (col >> 10) & 0x1F;
			g = (col >> 5) & 0x1F;
			b = col & 0x1F;
			*pDst = (r << 11) | (g << 6) | b;
		}

		screen_surface->Unlock(0);
		free(pic);
		MonoScreenOn = 2;
		ConvertSurfaceToTextures(screen_surface);
	}
	else
		Log("WHORE!");
}

void ReleaseScreen()
{
	MonoScreenOn = 0;

	if (screen_surface)
	{
		Log("Released %s @ %x - RefCnt = %d", "Picture Surface", screen_surface, screen_surface->Release());
		screen_surface = 0;
	}
	else
		Log("%s Attempt To Release NULL Ptr", "Picture Surface");

	FreeMonoScreen();
}

void DrawLoadingScreen()
{
	S_DisplayMonoScreen();
}

long GetSaveLoadFiles()
{
	FILE* file;
	SAVEFILE_INFO* pSave;
	SAVEGAME_INFO save_info;
	static long nSaves;
	char name[75];

	SaveCounter = 0;

	for (int i = 0; i < 15; i++)
	{
		pSave = &SaveGames[i];
		wsprintf(name, "savegame.%d", i);
		file = fopen(name, "rb");
		Log("Attempting to open %s", name);

		if (!file)
		{
			pSave->valid = 0;
			strcpy(pSave->name, SCRIPT_TEXT(TXT_Empty_Slot));
			continue;
		}

		Log("Opened OK");
		fread(&pSave->name, sizeof(char), 75, file);
		fread(&pSave->num, sizeof(long), 1, file);
		fread(&pSave->days, sizeof(short), 1, file);
		fread(&pSave->hours, sizeof(short), 1, file);
		fread(&pSave->minutes, sizeof(short), 1, file);
		fread(&pSave->seconds, sizeof(short), 1, file);
		fread(&save_info, 1, sizeof(SAVEGAME_INFO), file);
		fclose(file);

		if (pSave->num > SaveCounter)
			SaveCounter = pSave->num;

		pSave->valid = 1;
		nSaves++;
		Log("Validated savegame");
	}

	SaveCounter++;
	return nSaves;
}

long S_DisplayPauseMenu(long reset)
{
	static long menu, selection = 1;
	long y;

	if (!menu)
	{
		if (reset)
		{
			selection = reset;
			menu = 0;
		}
		else
		{
			y = phd_centery - font_height;
			PrintString(phd_centerx, y - ((3 * font_height) >> 1), 6, SCRIPT_TEXT(TXT_Paused), FF_CENTER);
			PrintString(phd_centerx, y, selection & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Statistics), FF_CENTER);
			PrintString(phd_centerx, y + font_height, selection & 2 ? 1 : 2, SCRIPT_TEXT(TXT_Options), FF_CENTER);
			PrintString(phd_centerx, y + 2 * font_height, selection & 4 ? 1 : 2, SCRIPT_TEXT(TXT_Exit_to_Title), FF_CENTER);

			if (dbinput & IN_FORWARD)
			{
				if (selection > 1)
					selection >>= 1;

				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			}

			if (dbinput & IN_BACK)
			{
				if (selection < 4)
					selection <<= 1;

				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			}

			if (dbinput & IN_DESELECT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				return 1;
			}

			if (dbinput & IN_SELECT && !keymap[DIK_LALT])
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_DEFAULT);

				if (selection & 1)
					menu = 2;
				else if (selection & 2)
					menu = 1;
				else if (selection & 4)
					return 8;
			}
		}
	}
	else if (menu == 1)
	{
		DoOptions();

		if (dbinput & IN_DESELECT)
		{
			menu = 0;
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		}
	}
	else if (menu == 2)
	{
		DisplayStatsUCunt();

		if (dbinput & IN_DESELECT)
		{
			menu = 0;
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		}
	}

	return 0;
}

long S_PauseMenu()
{
	long fade, ret;

	fade = 0;
	CreateMonoScreen();
	S_DisplayPauseMenu(1);
	InventoryActive = 1;

	do
	{
		S_InitialisePolyList();

		if (fade)
			dbinput = 0;
		else
			S_UpdateInput();

		SetDebounce = 1;
		S_DisplayMonoScreen();
		ret = S_DisplayPauseMenu(0);
		UpdatePulseColour();
		S_OutputPolyList();
		S_DumpScreen();

		if (ret == 1)
			break;

		if (ret == 8)
		{
			fade = 8;
			ret = 0;
			SetFade(0, 255);
		}

		if (fade && DoFade == 2)
		{
			ret = fade;
			break;
		}

	} while (!MainThread.ended);

	TIME_Init();
	FreeMonoScreen();
	InventoryActive = 0;
	return ret;
}

void DoFrontEndOneShotStuff()
{
	static long done;

	if (!done)
	{
		PlayFmvNow(0, 0);
		PlayFmvNow(1, 0);
		done = 1;
	}
}

long FindSFCursor(long in, long selection)
{
	long num, bak;

	num = 0;

	while (selection != 1)
	{
		selection >>= 1;
		num++;
	}

	bak = num;

	if (in & IN_FORWARD && num)
		do num--; while (num && !SpecialFeaturesPage[num]);

	if (in & IN_BACK && num < 4)
		do num++; while (num < 4 && !SpecialFeaturesPage[num]);

	if (!SpecialFeaturesPage[num])
		num = bak;

	return 1 << num;
}

void CalculateNumSpecialFeatures()
{
	SpecialFeaturesPage[0] = 0;
	SpecialFeaturesPage[1] = 0;
	SpecialFeaturesPage[2] = 0;
	SpecialFeaturesPage[3] = 0;
	SpecialFeaturesPage[4] = 0;
	NumSpecialFeatures = 0;

	for (int i = 0; i < 4; i++)
	{
		if (savegame.CampaignSecrets[i] >= 9)
		{
			SpecialFeaturesPage[i] = 1;
			NumSpecialFeatures++;
		}
	}
}

void SpecialFeaturesDisplayScreens(long num)
{
	static long start[4] = { 0, 0, 0, 0 };
	static long nPics[4] = { 12, 15, 12, 23 };
	long first, max, pos, count;

	first = start[num];
	max = nPics[num];
	pos = 0;
	count = 0;
	LoadScreen(first, num);

	while (!MainThread.ended && !(dbinput & IN_DESELECT))
	{
		_BeginScene();
		InitBuckets();
		InitialiseSortList();
		S_UpdateInput();
		SetDebounce = 1;

		if (count < 2)
		{
			count++;
			DrawLoadingScreen();
		}
		else if (count == 2)
		{
			count = 3;
			ReleaseScreen();
		}

		if (!pos)
			PrintString(font_height, phd_winymax - font_height, 6, "Next \x1B", 0);
		else if (pos < max)
			PrintString(font_height, phd_winymax - font_height, 6, "\x19 Previous / Next \x1b", 0);
		else
			PrintString(font_height, phd_winymax - font_height, 6, "\x19 Previous", 0);

		UpdatePulseColour();
		S_OutputPolyList();
		S_DumpScreen();

		if (dbinput & IN_LEFT && pos)
		{
			pos--;
			LoadScreen(pos + first, num);
			count = 0;
		}

		if (dbinput & IN_RIGHT && pos < max)
		{
			pos++;
			LoadScreen(pos + first, num);
			count = 0;
		}
	}

	dbinput &= ~IN_DESELECT;
	ReleaseScreen();
}

void DoSpecialFeaturesServer()
{
	switch (SpecialFeaturesNum)
	{
	case 0:
		SpecialFeaturesDisplayScreens(0);
		break;

	case 1:
		SpecialFeaturesDisplayScreens(1);
		break;

	case 2:
		SpecialFeaturesDisplayScreens(2);
		break;

	case 3:
		SpecialFeaturesDisplayScreens(3);
		break;
	}

	SpecialFeaturesNum = -1;
}
