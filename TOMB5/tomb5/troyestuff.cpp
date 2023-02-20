#include "../tomb5/pch.h"
#include "troyestuff.h"
#include "../game/text.h"
#include "../game/sound.h"
#include "../specific/3dmath.h"
#include "tomb5.h"
#include "../specific/input.h"

#define PAGE0_NUM	12
#define PAGE1_NUM	12

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
	
	PrintString(phd_centerx - (phd_centerx >> 3), textY + (num + 2) * font_height, selection & (1 << num) ? 1 : 6, "\x19", 0);
	PrintString(phd_centerx + (phd_centerx >> 3), textY + (num + 2) * font_height, selection & (1 << num) ? 1 : 6, "\x1B", 0);

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
	long y, i;
	char buffer[80];
	bool changed;

	changed = 0;
	num = PAGE0_NUM;
	PrintString(phd_centerx, font_height, 6, "New tomb5 options", FF_CENTER);

	y = 2;
	i = 0;

	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Footprints", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Shadow mode", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Fix climb up delay", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Flexible crawling", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Cutscene skipper", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Cheats", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Bar mode", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Bar positions", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Enemy bars", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Ammo counter", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Gameover menu", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Fog", 0);

	y = 2;
	i = 0;

	strcpy(buffer, tomb5.footprints ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.shadow_mode == 1 ? "Original" : tomb5.shadow_mode == 2 ? "Circle" : tomb5.shadow_mode == 3 ? "Faded circle" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.fix_climb_up_delay ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.flexible_crawling ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.cutseq_skipper ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.enable_cheats ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.bar_mode == 1 ? "Original" : tomb5.bar_mode == 2 ? "TR4" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.bars_pos == 1 ? "Original" : tomb5.bars_pos == 2 ? "Improved" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.enemy_bars ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.ammo_counter ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.gameover ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.fog ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

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

	case 1 << 2:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.fix_climb_up_delay = !tomb5.fix_climb_up_delay;
			changed = 1;
		}

		break;

	case 1 << 3:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.flexible_crawling = !tomb5.flexible_crawling;
			changed = 1;
		}

		break;

	case 1 << 4:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.cutseq_skipper = !tomb5.cutseq_skipper;
			changed = 1;
		}

		break;

	case 1 << 5:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.enable_cheats = !tomb5.enable_cheats;
			changed = 1;
		}

		break;

	case 1 << 6:

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
	}

	return changed;
}

bool Page1(long& num, long textY, ulong selection)
{
	long y, i;
	char buffer[80];
	bool changed;

	changed = 0;
	num = PAGE1_NUM;
	PrintString(phd_centerx, font_height, 6, "New tomb5 options", FF_CENTER);

	y = 2;
	i = 0;

	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Crawl Tilting", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "PSX skies", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "TR4 loadscreens", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Loadbar style", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Mono screen style", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Loading text", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Shimmer", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Distance fog", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Ammotype hotkeys", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Look transparency", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "Static lighting", 0);
	PrintString(phd_centerx >> 2, textY + y++ * font_height, selection & (1 << i++) ? 1 : 2, "UW Effect", 0);

	y = 2;
	i = 0;

	strcpy(buffer, tomb5.crawltilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.PSX_skies ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_loadscreens ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.tr4_loadbar ? "TR4" : "TR5");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.inv_bg_mode == 1 ? "Original" : tomb5.inv_bg_mode == 2 ? "TR4" : "Clear");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.loadingtxt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.shimmer ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	sprintf(buffer, "%i", tomb5.distance_fog);
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.ammotype_hotkeys ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.look_transparency ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.static_lighting ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb5.uw_dust == 1 ? "off" : tomb5.uw_dust == 2 ? "Original" : "TR4");
	PrintString(phd_centerx + (phd_centerx >> 2), textY + y++ * font_height, selection & (1 << i++) ? 1 : 6, buffer, 0);

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

	case 1 << 6:

		if (dbinput & IN_RIGHT || dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.shimmer = !tomb5.shimmer;
			changed = 1;
		}

		break;

	case 1 << 7:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.distance_fog++;

			if (tomb5.distance_fog > 30)
				tomb5.distance_fog = 30;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.distance_fog--;

			if (tomb5.distance_fog < 3)
				tomb5.distance_fog = 3;

			changed = 1;
		}

		break;

	case 1 << 8:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.ammotype_hotkeys = !tomb5.ammotype_hotkeys;
			changed = 1;
		}

		break;

	case 1 << 9:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.look_transparency = !tomb5.look_transparency;
			changed = 1;
		}

		break;

	case 1 << 10:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.static_lighting = !tomb5.static_lighting;
			changed = 1;
		}

		break;

	case 1 << 11:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.uw_dust++;

			if (tomb5.uw_dust > 3)
				tomb5.uw_dust = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb5.uw_dust--;

			if (tomb5.uw_dust < 1)
				tomb5.uw_dust = 3;

			changed = 1;
		}

		break;
	}

	return changed;
}
