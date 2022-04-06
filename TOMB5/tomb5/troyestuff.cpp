#include "../tomb5/pch.h"
#include "troyestuff.h"
#include "../game/text.h"
#include "../game/sound.h"
#include "tomb5.h"

#define PAGE0_NUM	14
#define PAGE1_NUM	6

void TroyeMenu(long textY, long& menu, ulong& selection, ulong selection_bak)
{
	long num;
	static long page = 0;
	bool changed;

	num = 0;
	changed = 0;

	if (dbinput & IN_DESELECT)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		menu = 0;
		dbinput &= ~IN_DESELECT;
		selection = selection_bak;
		page = 0;
		return;
	}

	switch (page)
	{
	case 0:
		changed = Page0(num, textY, selection);
		break;

	case 1:
		changed = Page1(num, textY, selection);
		break;
	}
	
	PrintString(phd_centerx - (phd_centerx >> 3), (ushort)(textY + (num + 2) * font_height), selection & (1 << num) ? 1 : 6, "\x19", 0);
	PrintString(phd_centerx + (phd_centerx >> 3), (ushort)(textY + (num + 2) * font_height), selection & (1 << num) ? 1 : 6, "\x1B", 0);

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

	if (selection > ulong(1 << num))
		selection = 1 << num;

	if (selection & (1 << num))
	{
		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			
			if (page)
			{
				page = 0;
				num = PAGE0_NUM;
			}
			else
			{
				page = 1;
				num = PAGE1_NUM;
			}

			selection = 1 << num;
		}
	}

	if (changed)
		save_new_tomb5_settings();
}

bool Page0(long& num, long textY, ulong selection)
{
	char buffer[80];
	bool changed;

	changed = 0;
	num = PAGE0_NUM;
	PrintString(phd_centerx, 2 * font_height, 6, "New tomb5 options", FF_CENTER);
	PrintString(phd_centerx >> 2, (ushort)(textY + 2 * font_height), selection & 1 ? 1 : 2, "FootPrints", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 3 * font_height), selection & 2 ? 1 : 2, "Point light shadows", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 4 * font_height), selection & 4 ? 1 : 2, "Shadow mode", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 5 * font_height), selection & 8 ? 1 : 2, "Fix climb up delay", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 2, "Flexible crawling", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 7 * font_height), selection & 0x20 ? 1 : 2, "Cutscene skipper", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 8 * font_height), selection & 0x40 ? 1 : 2, "Cheats", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 9 * font_height), selection & 0x80 ? 1 : 2, "Bar positions", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 10 * font_height), selection & 0x100 ? 1 : 2, "Enemy bars", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 11 * font_height), selection & 0x200 ? 1 : 2, "Ammo counter", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 12 * font_height), selection & 0x400 ? 1 : 2, "Gameover menu", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 13 * font_height), selection & 0x800 ? 1 : 2, "Fog", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 14 * font_height), selection & 0x1000 ? 1 : 2, "Camera", 0);
	PrintString(phd_centerx >> 2, (ushort)(textY + 15 * font_height), selection & 0x2000 ? 1 : 2, "Bar mode", 0);

	strcpy(buffer, tomb5.footprints ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 2 * font_height), selection & 1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_point_lights ? "TR4" : "TR5");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 3 * font_height), selection & 2 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.shadow_mode == 1 ? "original" : tomb5.shadow_mode == 2 ? "circle" : tomb5.shadow_mode == 3 ? "improved PSX" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 4 * font_height), selection & 4 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.fix_climb_up_delay ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 5 * font_height), selection & 8 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.flexible_crawling ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.cutseq_skipper ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 7 * font_height), selection & 0x20 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.enable_cheats ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 8 * font_height), selection & 0x40 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.bars_pos == 1 ? "original" : tomb5.bars_pos == 2 ? "improved" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 9 * font_height), selection & 0x80 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.enemy_bars ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 10 * font_height), selection & 0x100 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.ammo_counter ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 11 * font_height), selection & 0x200 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.gameover ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 12 * font_height), selection & 0x400 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.fog ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 13 * font_height), selection & 0x800 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_camera ? "TR4" : "TR5");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 14 * font_height), selection & 0x1000 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.bar_mode == 1 ? "original" : tomb5.bar_mode == 2 ? "TR4" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 15 * font_height), selection & 0x2000 ? 1 : 6, buffer, 0);

	switch (selection)
	{
	case 1 << 0:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.footprints = !tomb5.footprints;
			changed = 1;
		}

		break;

	case 1 << 1:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.tr4_point_lights = !tomb5.tr4_point_lights;
			changed = 1;
		}

		break;

	case 1 << 2:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.shadow_mode++;

			if (tomb5.shadow_mode > 4)
				tomb5.shadow_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.shadow_mode--;

			if (tomb5.shadow_mode < 1)
				tomb5.shadow_mode = 4;

			changed = 1;
		}

		break;

	case 1 << 3:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.fix_climb_up_delay = !tomb5.fix_climb_up_delay;
			changed = 1;
		}

		break;

	case 1 << 4:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.flexible_crawling = !tomb5.flexible_crawling;
			changed = 1;
		}

		break;

	case 1 << 5:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.cutseq_skipper = !tomb5.cutseq_skipper;
			changed = 1;
		}

		break;

	case 1 << 6:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.enable_cheats = !tomb5.enable_cheats;
			changed = 1;
		}

		break;

	case 1 << 7:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.bars_pos++;

			if (tomb5.bars_pos > 3)
				tomb5.bars_pos = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.bars_pos--;

			if (tomb5.bars_pos < 1)
				tomb5.bars_pos = 3;

			changed = 1;
		}

		break;

	case 1 << 8:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.enemy_bars = !tomb5.enemy_bars;
			changed = 1;
		}

		break;

	case 1 << 9:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.ammo_counter = !tomb5.ammo_counter;
			changed = 1;
		}

		break;

	case 1 << 10:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.gameover = !tomb5.gameover;
			changed = 1;
		}

		break;

	case 1 << 11:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.fog = !tomb5.fog;
			changed = 1;
		}

		break;

	case 1 << 12:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.tr4_camera = !tomb5.tr4_camera;
			changed = 1;
		}

		break;

	case 1 << 13:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.bar_mode++;

			if (tomb5.bar_mode > 3)
				tomb5.bar_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.bar_mode--;

			if (tomb5.bar_mode < 1)
				tomb5.bar_mode = 3;

			changed = 1;
		}

		break;
	}

	return changed;
}

bool Page1(long& num, long textY, ulong selection)
{
	char buffer[80];
	bool changed;

	changed = 0;
	num = PAGE1_NUM;
	PrintString(phd_centerx, 2 * font_height, 6, "New tomb5 options", FF_CENTER);
	PrintString(phd_centerx >> 2, ushort(textY + 2 * font_height), selection & 1 ? 1 : 2, "Crawl Tilting", 0);
	PrintString(phd_centerx >> 2, ushort(textY + 3 * font_height), selection & 2 ? 1 : 2, "PSX skies", 0);
	PrintString(phd_centerx >> 2, ushort(textY + 4 * font_height), selection & 4 ? 1 : 2, "TR4 loadscreens", 0);
	PrintString(phd_centerx >> 2, ushort(textY + 5 * font_height), selection & 8 ? 1 : 2, "loadbar style", 0);
	PrintString(phd_centerx >> 2, ushort(textY + 6 * font_height), selection & 0x10 ? 1 : 2, "mono screen style", 0);
	PrintString(phd_centerx >> 2, ushort(textY + 7 * font_height), selection & 0x20 ? 1 : 2, "Loading text", 0);

	strcpy(buffer, tomb5.crawltilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 2 * font_height), selection & 1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.PSX_skies ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 3 * font_height), selection & 2 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_loadscreens ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 4 * font_height), selection & 4 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_loadbar ? "TR4" : "TR5");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 5 * font_height), selection & 8 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.inv_bg_mode == 1 ? "original" : tomb5.inv_bg_mode == 2 ? "TR4" : "clear");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 6 * font_height), selection & 0x10 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.loadingtxt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), ushort(textY + 7 * font_height), selection & 0x20 ? 1 : 6, buffer, 0);

	switch (selection)
	{
	case 1 << 0:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.crawltilt = !tomb5.crawltilt;
			changed = 1;
		}

		break;

	case 1 << 1:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.PSX_skies = !tomb5.PSX_skies;
			changed = 1;
		}

		break;

	case 1 << 2:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.tr4_loadscreens = !tomb5.tr4_loadscreens;
			changed = 1;
		}

		break;

	case 1 << 3:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.tr4_loadbar = !tomb5.tr4_loadbar;
			changed = 1;
		}

		break;

	case 1 << 4:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.inv_bg_mode++;

			if (tomb5.inv_bg_mode > 3)
				tomb5.inv_bg_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.inv_bg_mode--;

			if (tomb5.inv_bg_mode < 1)
				tomb5.inv_bg_mode = 3;

			changed = 1;
		}

		break;

	case 1 << 5:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.loadingtxt = !tomb5.loadingtxt;
			changed = 1;
		}

		break;
	}

	return changed;
}
