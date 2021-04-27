#include "../tomb5/pch.h"
#include "gameflow.h"
#include "../global/types.h"
#include "../specific/game.h"
#include "../specific/display.h"
#include "../specific/specific.h"
#include "sound.h"

void DoGameflow()
{
	unsigned char* gf;
	unsigned char n;
	unsigned char* m;

	do_boot_screen(Gameflow->Language);
	num_fmvs = 0;
	fmv_to_play[1] = 0;
	fmv_to_play[0] = 0;
	gfCurrentLevel = Gameflow->TitleEnabled == 0;
	gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

	while (1)
	{
		switch (n = *gf++)
		{
		case GF_FMV:
			fmv_to_play[num_fmvs++] = *gf++;
			continue;

		case GF_LEVEL:
			gfLevelFlags = gf[1] | (gf[2] << 8);

			if (!(gfLevelFlags & GF_LVOP_NO_LEVEL))
				DoLevel(gf[3], gf[4]);
			else
			{
				gfStatus = 999;
				gfCurrentLevel++;
			}

			gfLegendTime = 0;
			LaserSight = 0;
			BinocularRange = 0;
			*(int*)gfResidentCut = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfNumPickups = 0;
			gfMirrorRoom = -1;
			gfFogColour.r = 0;
			gfFogColour.g = 0;
			gfFogColour.b = 0;
			gfFogColour.cd = 0;

			if (gfStatus != 2)
			{
				if (gfStatus < 3)
				{
					if (gfStatus == 1)
					{
						gfInitialiseGame = 1;
						gfCurrentLevel = Gameflow->TitleEnabled == 0;
					}
				}
				else
				{
					if (gfStatus == 3)
					{
						if (!Gameflow->DemoDisc && Gameflow->nLevels != 2 && gfLevelComplete <= Gameflow->nLevels)
							gfCurrentLevel = gfLevelComplete;
						else
							gfCurrentLevel = 0;
					}
					else if (gfStatus == 4)
						return;
				}
			}
			else
			{
				gfGameMode = 4;
				gfCurrentLevel = savegame.CurrentLevel;
			}

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

			break;
		case GF_TITLE_LEVEL:
			gfLevelFlags = gf[0] | (gf[1] << 8);
			DoTitle(gf[2], gf[3]);
			gfMirrorRoom = -1;
			*(int*)gfResidentCut = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfNumPickups = 0;

			if (gfStatus == 3)
			{
				gfGameMode = 0;
				gfCurrentLevel = gfLevelComplete;
				gfInitialiseGame = 1;
			}
			else if (gfStatus < 4)
			{
				if (gfStatus == 2)
				{
					gfGameMode = 4;
					gfCurrentLevel = savegame.CurrentLevel;
				}
			}
			else if (gfStatus == 4)
				return;

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

			break;
		case GF_LEVEL_DATA_END:
			continue;

		case GF_CUT:
			gfCutNumber = *gf++;
			continue;

		case GF_RESIDENTCUT1:
			gfResidentCut[0] = *gf++;
			continue;

		case GF_RESIDENTCUT2:
			gfResidentCut[1] = *gf++;
			continue;

		case GF_RESIDENTCUT3:
			gfResidentCut[2] = *gf++;
			continue;

		case GF_RESIDENTCUT4:
			gfResidentCut[3] = *gf++;
			continue;

		case GF_LAYER1:
			LightningRGB[0] = *gf;
			LightningRGBs[0] = *gf;
			gfLayer1Col.r = *gf++;
			LightningRGB[1] = *gf;
			LightningRGBs[1] = *gf;
			gfLayer1Col.g = *gf++;
			LightningRGB[2] = *gf;
			LightningRGBs[2] = *gf;
			gfLayer1Col.b = *gf++;
			gfLayer1Vel = *gf++;
			continue;

		case GF_LAYER2:
			LightningRGB[0] = *gf;
			LightningRGBs[0] = *gf;
			gfLayer2Col.r = *gf++;
			LightningRGB[1] = *gf;
			LightningRGBs[1] = *gf;
			gfLayer2Col.g = *gf++;
			LightningRGB[2] = *gf;
			LightningRGBs[2] = *gf;
			gfLayer2Col.b = *gf++;
			gfLayer2Vel = *gf++;
			continue;

		case GF_UV_ROTATE:
			gfUVRotate = *gf++;
			continue;

		case GF_LEGEND:
			gfLegend = *gf++;

			if (gfGameMode != 4)
				gfLegendTime = 150;

			continue;

		case GF_LENS_FLARE:
			gfLensFlare.x = ((gf[1] << 8) | gf[0]) << 8;
			gfLensFlare.y = ((gf[2] | (gf[3] << 8)) << 16) >> 8;
			gfLensFlare.z = ((gf[5] << 8) | gf[4]) << 8;
			gfLensFlareColour.r = gf[6];
			gfLensFlareColour.g = gf[7];
			gfLensFlareColour.b = gf[8];
			gf += 9;
			continue;

		case GF_MIRROR:
			gfMirrorRoom = *gf;
			gfMirrorZPlane = (gf[4] << 24) | (gf[3] << 16) | (gf[2] << 8) | gf[1];
			gf += 5;
			continue;

		case GF_FOG:
			m = gf++;
			gfFogColour.b = *gf;
			gfFogColour.g = *(char*)(m)++;
			gfFogColour.r = *(char*)(gf++);
			gf = (unsigned __int8*)((char*)(gf++) + 1);
			continue;

		case GF_ANIMATING_MIP:
			gfMips[gfNumMips++] = *gf++;
			continue;

		case GF_RESET_HUB:
			gfResetHubDest = *gf++;
			continue;

		case GF_GIVE_ITEM_AT_STARTUP:
			gfPickups[gfNumPickups++] = *gf;
			gf += 2;
			continue;

		case GF_LOSE_ITEM_AT_STARTUP:
			gfTakeaways[gfNumTakeaways++] = *gf;
			gf += 2;
			continue;

		default:

			if (n >= GF_KEY_ITEM1 && n <= GF_KEY_ITEM8)
				n -= 93;
			else if (n >= GF_PUZZLE_ITEM1 && n <= GF_PUZZLE_ITEM8)
				n += 127;
			else if (n >= GF_PICKUP_ITEM1 && n <= GF_PICKUP_ITEM4)
				n -= 93;
			else if (n >= GF_EXAMINE1 && n <= GF_EXAMINE3)
				n -= 83;
			else if (n >= GF_KEY_ITEM1 && n <= GF_KEY_ITEM8_COMBO2)
				n -= 116;
			else if (n >= GF_PUZZLE_ITEM1_COMBO1 && n <= GF_PUZZLE_ITEM8_COMBO2)
				n += 100;
			else if (n >= GF_PICKUP_ITEM1_COMBO1 && n <= GF_PICKUP_ITEM4_COMBO2)
				n -= 128;

			inventry_objects_list[n].objname = gf[0] | (gf[1] << 8);
			inventry_objects_list[n].yoff = gf[2] | (gf[3] << 8);
			inventry_objects_list[n].scale1 = gf[4] | (gf[5] << 8);
			inventry_objects_list[n].yrot = gf[6] | (gf[7] << 8);
			inventry_objects_list[n].xrot = gf[8] | (gf[9] << 8);
			inventry_objects_list[n].zrot = gf[10] | (gf[11] << 8);
			inventry_objects_list[n].flags = gf[12] | (gf[13] << 8);
			gf += 14;
			continue;
		}
	}
}

int GetCampaignCheatValue()//somewhere in specific 
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

int TitleOptions()
{
	int ret, ret2, i, n, n2, load, cheat_jump, colorFlag, flag;

	static int load_or_new;
	static int always0 = 0;//leftover debug thing? if it's ever 1, the menu and logo don't show.
	static int gfLevelComplete_bak;
	static int selected_option_bak = 0;

	ret = 0;

	if (load_or_new)
	{
		ret2 = load_or_new;

		if (_00C87BF0 == 2)
		{
			gfLevelComplete = gfLevelComplete_bak;
			gfLevelComplete_bak = 0;
			load_or_new = 0;
			always0 = 0;
			menu_to_display = 0;
			selected_option = 1;		
			return ret2;
		}

		input = 0;
		dbinput = 0;
	}

	if (bDoCredits)
	{
		if (DoCredits())
			return 0;

		bDoCredits = 0;
		DestFadeScreenHeight = 0;
	}

	if (dels_cutseq_selector_flag)
	{
		menu_to_display = 0;
		selected_option = 1;
		do_dels_cutseq_selector();
		return ret2;
	}

	if (always0 == 0)
	{
		switch (menu_to_display)
		{
		case 0://main menu
			SuperShowLogo();
			Chris_Menu = 0;

			if (selected_option & 1)
				PrintString(phd_centerx, phd_winymax - 4 * font_height, 1, &gfStringWad[gfStringOffset_bis[STR_SAVE_GAME_BIS]], 0x8000);
			else
				PrintString(phd_centerx, phd_winymax - 4 * font_height, 2, &gfStringWad[gfStringOffset_bis[STR_SAVE_GAME_BIS]], 0x8000);

			if (selected_option & 2)
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + font_height, 1, &gfStringWad[gfStringOffset_bis[STR_LOAD_GAME_BIS]], 0x8000);
			else
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + font_height, 2, &gfStringWad[gfStringOffset_bis[STR_LOAD_GAME_BIS]], 0x8000);

			if (selected_option & 4)
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height, 1, &gfStringWad[gfStringOffset_bis[STR_OPTIONS]], 0x8000);
			else
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height, 2, &gfStringWad[gfStringOffset_bis[STR_OPTIONS]], 0x8000);

			if (selected_option & 8)
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height + font_height, 1, &gfStringWad[gfStringOffset_bis[STR_EXIT]], 0x8000);
			else
				PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height + font_height, 2, &gfStringWad[gfStringOffset_bis[STR_EXIT]], 0x8000);

			flag = 8;

			break;

		case 1://select level menu

			PrintString(phd_centerx, font_height + phd_winymin, 6, &gfStringWad[gfStringOffset_bis[STR_SELECT_LEVEL]], 0x8000);

			if (Gameflow->nLevels >= 10)
			{
				i = selected_option;
				n = 0;

				for (colorFlag = 10; i; ++n)
					i >>= 1;

				n2 = n - 9;

				if (n2 >= 1)
				{
					if (n2 > 1)
					{
						PrintString(32, font_height + font_height + phd_winymin + font_height, 6, &title_string[4], 0);
						PrintString(phd_winxmax - 48, font_height + font_height + phd_winymin + font_height, 6, &title_string[4], 0);
					}
				}
				else
					n2 = 1;

				if (n != Gameflow->nLevels - 1)
				{
					PrintString(32, font_height + phd_winymin + font_height + (10 * font_height), 6, &title_string[0], 0);
					PrintString(phd_winxmax - 48, font_height + phd_winymin + font_height + (10 * font_height), 6, &title_string[0], 0);
				}
			}
			else
			{
				n2 = 1;
				colorFlag = Gameflow->nLevels - 1;
			}

			i = n2;

			if (n2 < colorFlag + n2)
			{
				n = n2 - 1;

				int height = font_height + font_height + phd_winymin;

				while (1)
				{
					height += font_height;

					if (selected_option & (1 << n))
						PrintString(phd_centerx, height, 1, &gfStringWad[gfStringOffset_bis[gfLevelNames[n + 1]]], 0x8000);
					else
						PrintString(phd_centerx, height, 3 - (*((char*)&weirdo + i + 3) != 0), &gfStringWad[gfStringOffset_bis[gfLevelNames[n + 1]]], 0x8000);

					if (selected_option & (1 << n))
						selected_level = n;

					n++;

					if (++i >= colorFlag + n2)
						break;
				}
			}

			ret = 0;
			flag = 1 << (Gameflow->nLevels - 2);
			break;

		case 2://loading menu

			if (Gameflow->LoadSaveEnabled)
			{
				load = DoLoadSave(0x800000);

				if (load >= 0)
				{
					S_LoadGame(load);
					ret = 2;
				}
			}
			else
			{
				SoundEffect(SFX_LARA_NO, 0, SFX_ALWAYS);
				menu_to_display = 0;
				SuperShowLogo();
				Chris_Menu = 0;

				if (selected_option & 1)
					PrintString(phd_centerx, phd_winymax - 4 * font_height, 1, &gfStringWad[gfStringOffset_bis[STR_SAVE_GAME_BIS]], 0x8000);
				else
					PrintString(phd_centerx, phd_winymax - 4 * font_height, 2, &gfStringWad[gfStringOffset_bis[STR_SAVE_GAME_BIS]], 0x8000);

				if (selected_option & 2)
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + font_height, 1, &gfStringWad[gfStringOffset_bis[STR_LOAD_GAME_BIS]], 0x8000);
				else
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + font_height, 2, &gfStringWad[gfStringOffset_bis[STR_LOAD_GAME_BIS]], 0x8000);

				if (selected_option & 4)
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height, 1, &gfStringWad[gfStringOffset_bis[STR_OPTIONS]], 0x8000);
				else
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height, 2, &gfStringWad[gfStringOffset_bis[STR_OPTIONS]], 0x8000);

				if (selected_option & 8)
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height + font_height, 1, &gfStringWad[gfStringOffset_bis[STR_EXIT]], 0x8000);
				else
					PrintString(phd_centerx, (phd_winymax - 4 * font_height) + 2 * font_height + font_height, 2, &gfStringWad[gfStringOffset_bis[STR_EXIT]], 0x8000);

				flag = 8;
			}

			break;

		case 3://options menu
			DoOptions();
			break;
		}

		if (menu_to_display < 2)
		{
			if (dbinput & IN_FORWARD)
			{
				if (selected_option > 1)
					selected_option >>= 1;

				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			}

			if (dbinput & IN_BACK)
			{
				if (selected_option < flag)
					selected_option <<= 1;

				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
			}
		}

		if (dbinput & IN_DESELECT && menu_to_display > 0)
		{
			menu_to_display = 0;
			selected_option = selected_option_bak;
			S_SoundStopAllSamples();
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		}

		if (menu_to_display == 0)
		{
			cheat_jump = GetCampaignCheatValue();

			if (cheat_jump)
			{
				gfLevelComplete = cheat_jump;
				ret = 3;
			}
		}

		if (dbinput & IN_SELECT && !keymap[56] && menu_to_display < 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

			if (menu_to_display != 0)
			{
				if (menu_to_display == 1 && level_select_names[selected_level])
				{
					gfLevelComplete = 0;

					n = 0;
					n2 = selected_option;

					if (n2)
					{
						do
						{
							n2 >>= 1;
							n++;

						} while (n2);

						gfLevelComplete = n;
					}

					ret = 3;
				}
			}
			else if (selected_option > 0 && selected_option <= 8)
			{
				switch (selected_option)
				{
				case 1:
					if (Gameflow->PlayAnyLevel)
					{
						selected_option_bak = selected_option;
						menu_to_display = 1;
					}
					else
					{
						gfLevelComplete = 1;
						ret = 3;
					}

					break;

				case 2:
					GetSaveLoadFiles();
					selected_option_bak = selected_option;
					menu_to_display = 2;
					break;

				case 3:
				case 5:
				case 6:
				case 7:

					break;

				case 4:
					selected_option_bak = selected_option;
					menu_to_display = 3;
					break;

				case 8:
					ret = 4;
					break;
				}
			}
		}
	}

	if (thread_started)
		return 4;

	if (ret)
	{
		load_or_new = ret;
		gfLevelComplete_bak = gfLevelComplete;
		gfLevelComplete = 0;
		ret = 0;
		SetFade(0, 255);
	}

	return ret;
}

void inject_gameflow()
{
	INJECT(0x00434B60, TitleOptions);
	INJECT(0x004354B0, DoGameflow);
	INJECT(0x004B1F00, GetCampaignCheatValue);
}
