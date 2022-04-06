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
#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#include "../tomb5/troyestuff.h"
#endif
#include "mmx.h"

static long MonoScreenX[4] = { 0, 256, 512, 640 };
static long MonoScreenY[3] = { 0, 256, 480 };

static const char* screen_paths[5] =
{
	"SCREENS\\STORY1.STR",
	"SCREENS\\NXG.STR",
	"SCREENS\\STORY2.STR",
	"SCREENS\\GALLERY.STR",
	"SCREENS\\SCREENS.STR"
};

#ifdef IMPROVED_BARS
static GouraudBarColourSet healthBarColourSet =
{
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 128, 192, 255, 192, 128 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet poisonBarColourSet =
{
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 128, 192, 255, 192, 128 }
};

static GouraudBarColourSet airBarColourSet =
{
	{ 0, 0, 0, 0, 0 },
	{ 113, 146, 113, 93, 74 },
	{ 123, 154, 123, 107, 91 },
	{ 0, 0, 0, 0, 0 },
	{ 113, 146, 113, 93, 74 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet dashBarColourSet =
{
	{ 144, 192, 240, 192, 144 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 144, 192, 240, 192, 144 },
	{ 144, 192, 240, 192, 144 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet loadBarColourSet =
{
	{ 48, 96, 127, 80, 32 },
	{ 0, 0, 0, 0, 0 },
	{ 48, 96, 127, 80, 32 },
	{ 0, 0, 0, 0, 0 },
	{ 48, 96, 127, 80, 32 },
	{ 48, 96, 127, 80, 32 }
};

static GouraudBarColourSet enemyBarColourSet =
{
	{ 128, 192, 255, 192, 128 },
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 123, 154, 123, 107, 91 },
	{ 0, 0, 0, 0, 0 }
};

static void S_DrawGouraudBar(int x, int y, int width, int height, int value, GouraudBarColourSet* colour)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float fx, fx2, fy, fy2, fvalue;
	long r, g, b;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;
	tex.drawtype = 0;
	tex.tpage = 0;
	fx = phd_winxmax * 0.0015625F;
	fy = phd_winymax * 0.0020833334F;
	fvalue = 0.0099999998F * value;
	fx2 = width * fvalue;
	fy2 = height * 0.1666666716F;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	v[0].sx = x * fx;
	v[1].sx = x * fx + fx2 * fx;
	v[2].sx = x * fx;
	v[3].sx = x * fx + fx2 * fx;
	v[0].sy = y * fy - fy2 * fy;
	v[1].sy = y * fy - fy2 * fy;
	v[2].sy = y * fy;
	v[3].sy = y * fy;
	v[0].sz = f_mznear;
	v[1].sz = f_mznear;
	v[2].sz = f_mznear;
	v[3].sz = f_mznear;
	v[0].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[1].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[2].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[3].rhw = f_mpersp / f_mznear * f_moneopersp;

	v[0].sy += fy2 * fy;
	v[1].sy += fy2 * fy;
	v[2].sy += fy2 * fy;
	v[3].sy += fy2 * fy;

	v[0].color = 0xFF000000;
	v[1].color = 0xFF000000;

	r = colour->abLeftRed[0];
	g = colour->abLeftGreen[0];
	b = colour->abLeftBlue[0];
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[2].color = RGBONLY(r, g, b);

	r = (long)((1 - fvalue) * colour->abLeftRed[0] + fvalue * colour->abRightRed[0]);
	g = (long)((1 - fvalue) * colour->abLeftGreen[0] + fvalue * colour->abRightGreen[0]);
	b = (long)((1 - fvalue) * colour->abLeftBlue[0] + fvalue * colour->abRightBlue[0]);
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[3].color = RGBONLY(r, g, b);

	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

	for (int i = 0; i < 4; i++)
	{
		v[0].sy += fy2 * fy;
		v[1].sy += fy2 * fy;
		v[2].sy += fy2 * fy;
		v[3].sy += fy2 * fy;
		v[0].color = RGBONLY(colour->abLeftRed[i], colour->abLeftGreen[i], colour->abLeftBlue[i]);
		r = (long)((1 - fvalue) * colour->abLeftRed[i] + fvalue * colour->abRightRed[i]);
		g = (long)((1 - fvalue) * colour->abLeftGreen[i] + fvalue * colour->abRightGreen[i]);
		b = (long)((1 - fvalue) * colour->abLeftBlue[i] + fvalue * colour->abRightBlue[i]);
		v[1].color = RGBONLY(r, g, b);
		v[2].color = RGBONLY(colour->abLeftRed[i + 1], colour->abLeftGreen[i + 1], colour->abLeftBlue[i + 1]);
		r = (long)((1 - fvalue) * colour->abLeftRed[i + 1] + fvalue * colour->abRightRed[i + 1]);
		g = (long)((1 - fvalue) * colour->abLeftGreen[i + 1] + fvalue * colour->abRightGreen[i + 1]);
		b = (long)((1 - fvalue) * colour->abLeftBlue[i + 1] + fvalue * colour->abRightBlue[i + 1]);
		v[3].color = RGBONLY(r, g, b);
		AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);
	}

	v[0].sy += fy2 * fy;
	v[1].sy += fy2 * fy;
	v[2].sy += fy2 * fy;
	v[3].sy += fy2 * fy;

	r = colour->abLeftRed[4];
	g = colour->abLeftGreen[4];
	b = colour->abLeftBlue[4];
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[0].color = RGBONLY(r, g, b);

	r = (long)((1 - fvalue) * colour->abLeftRed[4] + fvalue * colour->abRightRed[4]);
	g = (long)((1 - fvalue) * colour->abLeftGreen[4] + fvalue * colour->abRightGreen[4]);
	b = (long)((1 - fvalue) * colour->abLeftBlue[4] + fvalue * colour->abRightBlue[4]);
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[1].color = RGBONLY(r, g, b);

	v[2].color = 0xFF000000;
	v[3].color = 0xFF000000;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

	v[0].sx = x * fx - 2;
	v[1].sx = x * fx + width * fx + 2;
	v[2].sx = x * fx - 2;
	v[3].sx = x * fx + width * fx + 2;
	v[0].sy = y * fy;
	v[1].sy = y * fy;
	v[2].sy = y * fy + height * fy;
	v[3].sy = y * fy + height * fy;
	v[0].sz = f_mznear + 1;
	v[1].sz = f_mznear + 1;
	v[2].sz = f_mznear + 1;
	v[3].sz = f_mznear + 1;
	v[0].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[0].color = 0;
	v[1].color = 0;
	v[2].color = 0;
	v[3].color = 0;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//black background

	v[0].sx = x * fx - 3;
	v[1].sx = x * fx + width * fx + 3;
	v[2].sx = x * fx - 3;
	v[3].sx = x * fx + width * fx + 3;
	v[0].sy = y * fy - 1;
	v[1].sy = y * fy - 1;
	v[2].sy = y * fy + height * fy + 1;
	v[3].sy = y * fy + height * fy + 1;
	v[0].sz = f_mznear + 2;
	v[1].sz = f_mznear + 2;
	v[2].sz = f_mznear + 2;
	v[3].sz = f_mznear + 2;
	v[0].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[0].color = 0xFFFFFFFF;
	v[1].color = 0xFFFFFFFF;
	v[2].color = 0xFFFFFFFF;
	v[3].color = 0xFFFFFFFF;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//white border
}

void S_DoTR4Bar(long x, long y, long width, long height, long pos, long clr1, long clr2)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	static float wat = 0;
	long x2, sx, sy;

	nPolyType = 4;
	wat += 0.0099999998F;

	if (wat > 0.99000001F)
		wat = 0;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	x2 = (long)(x * phd_winxmax * 0.0015625F);
	sx = (long(width * phd_winxmax * 0.0015625F) * pos) / 100;
	sy = (long)((height >> 1) * phd_winymax * 0.0020833334F);

	v[0].sx = (float)x2;
	v[0].sy = (float)y;
	v[0].color = clr1;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)y;
	v[1].color = clr1;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + sy);
	v[2].color = clr2;

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + sy);
	v[3].color = clr2;

	for (int i = 0; i < 4; i++)
	{
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 6;
		v[i].rhw = f_moneoznear - 2;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	tex.drawtype = 0;
	tex.flag = 0;
	tex.tpage = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = (float)x2;
	v[0].sy = (float)(y + sy);
	v[0].color = clr2;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)(y + sy);
	v[1].color = clr2;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + 2 * sy);
	v[2].color = clr1;

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + 2 * sy);
	v[3].color = clr1;

	for (int i = 0; i < 4; i++)
	{
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 6;
		v[i].rhw = f_moneoznear - 2;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	x2 = (long)(x * phd_winxmax * 0.0015625F);
	sx = (long)(width * phd_winxmax * 0.0015625F);
	sy = (long)((height >> 1) * phd_winymax * 0.0020833334F);

	v[0].sx = (float)(x2 - 1);
	v[0].sy = (float)(y - 1);

	v[1].sx = (float)(x2 + sx + 1);
	v[1].sy = (float)(y - 1);

	v[2].sx = (float)(x2 + sx + 1);
	v[2].sy = (float)(y + 2 * sy + 1);

	v[3].sx = (float)(x2 - 1);
	v[3].sy = (float)(y + 2 * sy + 1);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFFFFFFFF;
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 3;
		v[i].rhw = f_moneoznear - 4;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = (float)x2;
	v[0].sy = (float)y;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)y;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + 2 * sy);

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + 2 * sy);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 4;
		v[i].rhw = f_moneoznear - 3;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}
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
	PrintString(phd_centerx, ypos, 6, SCRIPT_TEXT(STR_STATISTICS), FF_CENTER);
	PrintString(phd_centerx, ypos + 2 * font_height, 2, SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]), FF_CENTER);
	PrintString(phd_centerx >> 2, ypos + 3 * font_height, 2, SCRIPT_TEXT(STR_TIME_TAKEN), 0);
	PrintString(phd_centerx >> 2, ypos + 4 * font_height, 2, SCRIPT_TEXT(STR_DISTANCE_TRAVELLED), 0);
	PrintString(phd_centerx >> 2, ypos + 5 * font_height, 2, SCRIPT_TEXT(STR_AMMO_USED), 0);
	PrintString(phd_centerx >> 2, ypos + 6 * font_height, 2, SCRIPT_TEXT(STR_HEALTH_PACKS_USED), 0);
	PrintString(phd_centerx >> 2, ypos + 7 * font_height, 2, SCRIPT_TEXT(STR_SECRETS_FOUND), 0);

	seconds = GameTimer / 30;
	Days = seconds / (24 * 60 * 60);
	Hours = (seconds % (24 * 60 * 60)) / (60 * 60);
	Min = (seconds / 60) % 60;
	Sec = (seconds % 60);

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

void DisplayStatsUCunt()
{
	DoStatScreen();
}

void S_DrawAirBar(int pos)
{
#ifdef GENERAL_FIXES
	long x, y;

	if (gfCurrentLevel != LVL5_TITLE)
	{
		if (tomb5.bars_pos == 1)//original
		{
			x = 490 - (font_height >> 2);

			if (tomb5.bar_mode == 2)
				y = (font_height >> 2) + (2 * font_height / 3);
			else
				y = (font_height >> 1) + (font_height >> 2) + 32;
		}
		else if (tomb5.bars_pos == 2)//improved
		{
			x = 490 - (font_height >> 2);
			y = font_height >> 2;
		}
		else//PSX
		{
			x = 470 - (font_height >> 2);

			if (tomb5.bar_mode == 2)
				y = (font_height >> 2) + (2 * font_height / 3);
			else
				y = (font_height >> 1) + (font_height >> 2);
		}

#ifdef IMPROVED_BARS
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(x, y, 150, 12, pos, &airBarColourSet);
		else if (tomb5.bar_mode == 2)
			S_DoTR4Bar(x, y, 150, 12, pos, 0xFF000000, 0xFF0000FF);
		else
#endif
			DoBar(x, y, 150, 12, pos, 0x0000A0, 0x0050A0);
	}
#else
	if (gfCurrentLevel != LVL5_TITLE)
		DoBar(490 - (font_height >> 2), (font_height >> 1) + (font_height >> 2) + 32, 150, 12, pos, 0x0000A0, 0x0050A0);//blue rgb 0, 0, 160/lighter blue rgb 0, 80, 160
#endif
}

void S_DrawHealthBar(int pos)
{
	long color;
#ifdef GENERAL_FIXES
	long x, y;

	if (gfCurrentLevel != LVL5_TITLE)
	{
		if (lara.poisoned || lara.Gassed)
			color = 0xA0A000;//yellowish poison, rgb 160, 160, 0
		else
			color = 0x00A000;//green, rgb 0, 160, 0

		if (tomb5.bars_pos == 1)//original
		{
			x = font_height >> 2;

			if (tomb5.bar_mode == 2)
				y = font_height >> 2;
			else
				y = (font_height >> 2) + 32;
}
		else if (tomb5.bars_pos == 2)//improved
		{
			x = font_height >> 2;
			y = font_height >> 2;
		}
		else//PSX
		{
			x = 470 - (font_height >> 2);
			y = font_height >> 2;
		}

#ifdef IMPROVED_BARS
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(x, y, 150, 12, pos, lara.poisoned || lara.Gassed ? &poisonBarColourSet : &healthBarColourSet);
		else if (tomb5.bar_mode == 2)
		{
			if (lara.poisoned || lara.Gassed)
				S_DoTR4Bar(x, y, 150, 12, pos, 0xFF000000, 0xFFFFFF00);
			else
				S_DoTR4Bar(x, y, 150, 12, pos, 0xFF000000, 0xFFFF0000);
		}
		else
#endif
			DoBar(x, y, 150, 12, pos, 0xA00000, color);
	}
#else
	if (gfCurrentLevel != LVL5_TITLE)
	{
		if (lara.poisoned || lara.Gassed)
			color = 0xA0A000;//yellowish poison, rgb 160, 160, 0
		else
			color = 0x00A000;//green, rgb 0, 160, 0

		DoBar(font_height >> 2, (font_height >> 2) + 32, 150, 12, pos, 0xA00000, color);//red rgb 160, 0, 0/color
	}
#endif
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

#ifdef IMPROVED_BARS
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(245, (font_height >> 1) + 32, 150, 12, pos, lara.poisoned || lara.Gassed ? &poisonBarColourSet : &healthBarColourSet);
		else if (tomb5.bar_mode == 2)
		{
			if (lara.poisoned || lara.Gassed)
				S_DoTR4Bar(245, font_height + 48, 150, 12, pos, 0xFF000000, 0xFFFFFF00);
			else
				S_DoTR4Bar(245, font_height + 48, 150, 12, pos, 0xFF000000, 0xFFFF0000);
		}
		else
#endif
			DoBar(245, (font_height >> 1) + 32, 150, 12, pos, 0xA00000, color);
	}
}

void S_DrawDashBar(int pos)
{
#ifdef GENERAL_FIXES
	long x, y;

	if (tomb5.bars_pos == 1)//original
	{
		x = 490 - (font_height >> 2);

		if (tomb5.bar_mode == 2)
			y = font_height >> 2;
		else
			y = (font_height >> 2) + 32;
}
	else if (tomb5.bars_pos == 2)//improved
	{
		x = 490 - (font_height >> 2);

		if (tomb5.bar_mode == 2)
			y = (font_height >> 2) + (2 * font_height / 3);
		else
			y = (font_height >> 1) + (font_height >> 2);
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);

		if (tomb5.bar_mode == 2)
			y = (font_height >> 2) + (2 * font_height / 3) + (2 * font_height / 3);
		else
			y = (font_height >> 2) + (font_height >> 2) + 32;
	}

	if (gfCurrentLevel != LVL5_TITLE)
	{
#ifdef IMPROVED_BARS
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(x, y, 150, 12, pos, &dashBarColourSet);
		else if (tomb5.bar_mode == 2)
			S_DoTR4Bar(x, y, 150, 12, pos, 0xFF000000, 0xFF00FF00);
		else
#endif	//IMPROVED_BARS
			DoBar(x, y, 150, 12, pos, 0xA0A000, 0x00A000);
	}
#else	//GENERAL_FIXES
	if (gfCurrentLevel != LVL5_TITLE)
		DoBar(490 - (font_height >> 2), (font_height >> 2) + 32, 150, 12, pos, 0xA0A000, 0x00A000);//yellow rgb 160, 160, 0 / green rgb 0, 160, 0
#endif	//GENERAL_FIXES
}

#ifdef ENEMY_BARS
void S_DrawEnemyBar(long pos)
{
	long x, y;

	if (tomb5.bars_pos == 1)//original
	{
		x = font_height >> 2;

		if (tomb5.bar_mode == 2)
			y = (font_height >> 2) + (2 * font_height / 3);
		else
			y = (font_height >> 1) + (font_height >> 2) + 32;
	}
	else if (tomb5.bars_pos == 2)//improved
	{
		x = font_height >> 2;

		if (tomb5.bar_mode == 2)
			y = (font_height >> 2) + (2 * font_height / 3);
		else
			y = (font_height >> 1) + (font_height >> 2);
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);

		if (tomb5.bar_mode == 2)
			y = (font_height >> 2) + (2 * font_height / 3) + (2 * font_height / 3) + (2 * font_height / 3);
		else
			y = (font_height >> 1) + (font_height >> 2) + (font_height >> 2) + 32;
	}

	if (tomb5.bar_mode == 3)
		S_DrawGouraudBar(x, y, 150, 12, pos, &enemyBarColourSet);
	else if (tomb5.bar_mode == 2)
		S_DoTR4Bar(x, y, 150, 12, pos, 0xFF000000, 0xFFFFA000);
	else
		DoBar(x, y, 150, 12, pos, 0xA00000, 0xA0A000);
}
#endif

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
			wsprintf(SaveInfo, "%d %s %02d:%02d:%02d", SaveGames[i].Day, SCRIPT_TEXT_bis(STR_DAYS), SaveGames[i].Hour, SaveGames[i].Min, SaveGames[i].Sec);
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

		PrintString(phd_centerx >> 2, font_height, selection & 1 ? 1 : 2, SCRIPT_TEXT_bis(STR_CONTROL_METHOD), 0);
		textY = font_height;
		font_height = (long)((float)phd_winymax * 0.050000001F);
		big_char_height = 10;
		textY2 = font_height + (font_height + (font_height >> 1));

		if (!ControlMethod)
		{
			PrintString(phd_centerx >> 2, (ushort)(textY2 + font_height), selection & 2 ? 1 : 2, "\x18", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 2 * font_height), selection & 4 ? 1 : 2, "\x1A", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 3 * font_height), selection & 8 ? 1 : 2, "\x19", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 4 * font_height), selection & 0x10 ? 1 : 2, "\x1B", 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 5 * font_height),  selection & 0x20 ? 1 : 2, SCRIPT_TEXT_bis(STR_DUCK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 6 * font_height), selection & 0x40 ? 1 : 2, SCRIPT_TEXT_bis(STR_DASH), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 7 * font_height), selection & 0x80 ? 1 : 2, SCRIPT_TEXT_bis(STR_WALK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height), selection & 0x100 ? 1 : 2, SCRIPT_TEXT_bis(STR_JUMP), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 9 * font_height), selection & 0x200 ? 1 : 2, SCRIPT_TEXT_bis(STR_ACTION), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 10 * font_height), selection & 0x400 ? 1 : 2, SCRIPT_TEXT_bis(STR_DRAW_WEAPON), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 11 * font_height), selection & 0x800 ? 1 : 2, SCRIPT_TEXT_bis(STR_USE_FLARE), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 12 * font_height), selection & 0x1000 ? 1 : 2, SCRIPT_TEXT_bis(STR_LOOK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 13 * font_height), selection & 0x2000 ? 1 : 2, SCRIPT_TEXT_bis(STR_ROLL), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 14 * font_height), selection & 0x4000 ? 1 : 2, SCRIPT_TEXT_bis(STR_INVENTORY), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 15 * font_height), selection & 0x8000 ? 1 : 2, SCRIPT_TEXT_bis(STR_STEP_LEFT), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 16 * font_height), selection & 0x10000 ? 1 : 2, SCRIPT_TEXT_bis(STR_STEP_RIGHT), 0);
			text = (waiting_for_key && (controls_selection & 2)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][0]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + font_height), controls_selection & 2 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 4)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][1]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 2 * font_height), controls_selection & 4 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 8)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][2]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 3 * font_height), (controls_selection & 8) != 0 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][3]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 4 * font_height), controls_selection & 0x10 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x20)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][4]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 5 * font_height), controls_selection & 0x20 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x40)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][5]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 6 * font_height), controls_selection & 0x40 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x80)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][6]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 7 * font_height), controls_selection & 0x80 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x100)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][7]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height), controls_selection & 0x100 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x200)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][8]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 9 * font_height), controls_selection & 0x200 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x400)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][9]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 10 * font_height), controls_selection & 0x400 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x800)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][10]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 11 * font_height), controls_selection & 0x800 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x1000)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][11]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 12 * font_height), controls_selection & 0x1000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x2000)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][12]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 13 * font_height), controls_selection & 0x2000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x4000)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][13]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 14 * font_height), controls_selection & 0x4000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x8000)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][14]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 15 * font_height), controls_selection & 0x8000 ? 1 : 6, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10000)) ? SCRIPT_TEXT_bis(STR_WAITING) : keyboard_buttons[layout[1][15]];
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 16 * font_height), controls_selection & 0x10000 ? 1 : 6, text, 0);
		}

		if (ControlMethod == 1)
		{
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 5 * font_height), selection & 2 ? 1 : 2, SCRIPT_TEXT_bis(STR_DUCK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 6 * font_height), selection & 4 ? 1 : 2, SCRIPT_TEXT_bis(STR_DASH), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 7 * font_height), selection & 8 ? 1 : 2, SCRIPT_TEXT_bis(STR_WALK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 8 * font_height), selection & 0x10 ? 1 : 2, SCRIPT_TEXT_bis(STR_JUMP), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 9 * font_height), selection & 0x20 ? 1 : 2, SCRIPT_TEXT_bis(STR_ACTION), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 10 * font_height), selection & 0x40 ? 1 : 2, SCRIPT_TEXT_bis(STR_DRAW_WEAPON), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 11 * font_height), selection & 0x80 ? 1 : 2, SCRIPT_TEXT_bis(STR_USE_FLARE), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 12 * font_height), selection & 0x100 ? 1 : 2, SCRIPT_TEXT_bis(STR_LOOK), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 13 * font_height), selection & 0x200 ? 1 : 2, SCRIPT_TEXT_bis(STR_ROLL), 0);
			PrintString(phd_centerx >> 2, (ushort)(textY2 + 14 * font_height), selection & 0x400 ? 1 : 2, SCRIPT_TEXT_bis(STR_INVENTORY), 0);

			for (int i = 0; i < 10; i++)
			{
				sprintf(quality_buffer, "(%s)", keyboard_buttons[layout[1][i + 4]]);
				PrintString((phd_centerx >> 3) + phd_centerx + (phd_centerx >> 1), (ushort)(textY2 + font_height * (i + 5)), 5, quality_buffer, 0);
			}

			text = (waiting_for_key && (controls_selection & 2)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[0]];
			clr = (waiting_for_key && (selection & 2)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 5 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 4)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[1]];
			clr = (waiting_for_key && (selection & 4)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 6 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 8)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[2]];
			clr = (waiting_for_key && (selection & 8)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 7 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x10)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[3]];
			clr = (waiting_for_key && (selection & 0x10)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 8 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x20)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[4]];
			clr = (waiting_for_key && (selection & 0x20)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 9 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x40)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[5]];
			clr = (waiting_for_key && (selection & 0x40)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 10 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x80)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[6]];
			clr = (waiting_for_key && (selection & 0x80)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 11 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x100)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[7]];
			clr = (waiting_for_key && (selection & 0x100)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 12 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x200)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[8]];
			clr = (waiting_for_key && (selection & 0x200)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 13 * font_height), clr, text, 0);
			text = (waiting_for_key && (controls_selection & 0x400)) ? SCRIPT_TEXT_bis(STR_WAITING) : JoyStickButtons[MappedControls[9]];
			clr = (waiting_for_key && (selection & 0x400)) ? 1 : 6;
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY2 + 14 * font_height), clr, text, 0);
		}

		font_height = default_font_height;
		big_char_height = 6;

		if (!ControlMethod)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT_bis(STR_KEYBOARD), 0);
		else if (ControlMethod == 1)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT_bis(STR_JOYSTICK), 0);
		else if (ControlMethod == 2)
			PrintString(phd_centerx + (phd_centerx >> 2), (ushort)textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT_bis(STR_RESET), 0);

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
		PrintString(phd_centerx, (ushort)(textY + 3 * font_height), 6, SCRIPT_TEXT_bis(STR_SPECIAL_FEATURES), FF_CENTER);

		if (SpecialFeaturesPage[0])
			clr = selection & 1 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 5 * font_height), clr, SCRIPT_TEXT_bis(STR_STORYBOARDS_PART_1), FF_CENTER);

		if (SpecialFeaturesPage[1])
			clr = selection & 2 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 6 * font_height), clr, SCRIPT_TEXT_bis(STR_NEXT_GENERATION_CONCEPT_ART), FF_CENTER);

		if (SpecialFeaturesPage[2])
			clr = selection & 4 ? 1 : 2;
		else
			clr = 3;

		PrintString(phd_centerx, (ushort)(textY + 7 * font_height), clr, SCRIPT_TEXT_bis(STR_STORYBOARDS_PART_2), FF_CENTER);

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
		PrintString(phd_centerx, 3 * font_height, 6, SCRIPT_TEXT_bis(STR_OPTIONS), FF_CENTER);
		PrintString(phd_centerx, (ushort)(textY + font_height + (font_height >> 1)), selection & 1 ? 1 : 2, SCRIPT_TEXT_bis(STR_CONTROL_CONFIGURATION), FF_CENTER);
		PrintString(phd_centerx >> 2, (ushort)(textY + 3 * font_height), selection & 2 ? 1 : 2, SCRIPT_TEXT_bis(STR_MUSIC_VOLUME_BIS), 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 4 * font_height), selection & 4 ? 1 : 2, SCRIPT_TEXT_bis(STR_SFX_VOLUME_BIS), 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 5 * font_height), selection & 8 ? 1 : 2, SCRIPT_TEXT_bis(STR_SOUND_QUALITY), 0);
		PrintString(phd_centerx >> 2, (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 2, SCRIPT_TEXT_bis(STR_TARGETING), 0);
		DoSlider(400, 3 * font_height - (font_height >> 1) + textY + 4, 200, 16, MusicVolume, 0xFF1F1F1F, 0xFF3F3FFF, music_volume_bar_shade);
		DoSlider(400, textY + 4 * font_height + 4 - (font_height >> 1), 200, 16, SFXVolume, 0xFF1F1F1F, 0xFF3F3FFF, sfx_volume_bar_shade);

		switch (SoundQuality)
		{
		case 0:
			strcpy(quality_text, SCRIPT_TEXT_bis(STR_LOW));
			break;

		case 1:
			strcpy(quality_text, SCRIPT_TEXT_bis(STR_MEDIUM));
			break;

		case 2:
			strcpy(quality_text, SCRIPT_TEXT_bis(STR_HIGH));
			break;
		}

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 5 * font_height), selection & 8 ? 1 : 6, quality_text, 0);

		if (App.AutoTarget)
			strcpy(quality_text, SCRIPT_TEXT_bis(STR_AUTOMATIC));
		else
			strcpy(quality_text, SCRIPT_TEXT_bis(STR_MANUAL));

		PrintString(phd_centerx + (phd_centerx >> 2), (ushort)(textY + 6 * font_height), selection & 0x10 ? 1 : 6, quality_text, 0);
		special_features_available = 0x20;	//not the most accurate name

		if (gfGameMode == 1)
		{
#ifdef GENERAL_FIXES
			num = 7;
#else
			num = 6;
#endif
			PrintString(phd_centerx, (ushort)((font_height >> 1) + textY + 7 * font_height), selection & 0x20 ? 1 : 2, SCRIPT_TEXT_bis(STR_SPECIAL_FEATURES), FF_CENTER);
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
		TroyeMenu(textY, menu, selection, selection_bak);
#endif
}

void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float fx, fx2, fy, fw, fh, r1, g1, b1, r2, g2, b2, r, g, b, mul;
	long lr, lg, lb, clr_11, clr_12, clr_21, clr_22;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;
	tex.drawtype = 0;
	tex.tpage = 0;
	fx = (float)phd_winxmax * 0.0015625F;
	fy = (float)phd_winymax * 0.0020833334F;
	fw = (float)width;
	fh = (float)(height >> 1);
	fx2 = (fw * fx) * 0.0099999998F * (float)pos;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	v[0].sx = (float)x * fx;
	v[1].sx = ((float)x * fx) + fx2;
	v[2].sx = (float)x * fx;
	v[3].sx = ((float)x * fx) + fx2;
	v[0].sy = (float)y * fy;
	v[1].sy = (float)y * fy;
	v[2].sy = ((float)y * fy) + (fh * fy);
	v[3].sy = ((float)y * fy) + (fh * fy);
	v[0].sz = f_mznear;
	v[1].sz = f_mznear;
	v[2].sz = f_mznear;
	v[3].sz = f_mznear;
	v[0].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[1].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[2].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[3].rhw = f_mpersp / f_mznear * f_moneopersp;

	r1 = (float)CLRR(clr1);		//get rgbs
	g1 = (float)CLRG(clr1);
	b1 = (float)CLRB(clr1);
	r2 = (float)CLRR(clr2);
	g2 = (float)CLRG(clr2);
	b2 = (float)CLRB(clr2);

	mul = fx2 / (fw * fx);		//mix
	r = r1 + ((r2 - r1) * mul);
	g = g1 + ((g2 - g1) * mul);
	b = b1 + ((b2 - b1) * mul);

	lr = (long)r1;
	lg = (long)g1;
	lb = (long)b1;
	clr_11 = RGBONLY(lr >> 1, lg >> 1, lb >> 1);	//clr1 is taken as is
	clr_12 = RGBONLY(lr, lg, lb);

	lr = (long)r;
	lg = (long)g;
	lb = (long)b;
	clr_21 = RGBONLY(lr >> 1, lg >> 1, lb >> 1);	//clr2 is the mix
	clr_22 = RGBONLY(lr, lg, lb);

	v[0].color = clr_11;
	v[1].color = clr_21;
	v[2].color = clr_12;
	v[3].color = clr_22;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//top half

	v[0].color = clr_12;
	v[1].color = clr_22;
	v[2].color = clr_11;
	v[3].color = clr_21;
	v[0].sy = ((float)y * fy) + (fh * fy);
	v[1].sy = ((float)y * fy) + (fh * fy);
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);		//bottom half

	v[0].sx = (float)x * fx;
	v[1].sx = (fw * fx) + ((float)x * fx);
	v[2].sx = (float)x * fx;
	v[3].sx = (fw * fx) + ((float)x * fx);
	v[0].sy = (float)y * fy;
	v[1].sy = (float)y * fy;
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[0].sz = f_mznear + 1;
	v[1].sz = f_mznear + 1;
	v[2].sz = f_mznear + 1;
	v[3].sz = f_mznear + 1;
	v[0].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[0].color = 0;
	v[1].color = 0;
	v[2].color = 0;
	v[3].color = 0;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//black background

	v[0].sx = ((float)x * fx) - 1;
	v[1].sx = (fw * fx) + ((float)x * fx) + 1;
	v[2].sx = ((float)x * fx) - 1;
	v[3].sx = (fw * fx) + ((float)x * fx) + 1;
	v[0].sy = ((float)y * fy) - 1;
	v[1].sy = ((float)y * fy) - 1;
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy) + 1;
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy) + 1;
	v[0].sz = f_mznear + 2;
	v[1].sz = f_mznear + 2;
	v[2].sz = f_mznear + 2;
	v[3].sz = f_mznear + 2;
	v[0].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[0].color = 0xFFFFFFFF;
	v[1].color = 0xFFFFFFFF;
	v[2].color = 0xFFFFFFFF;
	v[3].color = 0xFFFFFFFF;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//white border
}

void CreateMonoScreen()
{
	MonoScreenOn = 1;

#ifdef GENERAL_FIXES
	ConvertSurfaceToTextures(App.dx.lpBackBuffer);
#else
	ConvertSurfaceToTextures(App.dx.lpPrimaryBuffer);
#endif
}

void S_InitLoadBar(long max)
{
	loadbar_steps = 0;
	loadbar_maxpos = max;
	loadbar_pos = 0;
	loadbar_on = 1;
}

void S_UpdateLoadBar()
{
	loadbar_steps = 100.0F / loadbar_maxpos + loadbar_steps;
}

long S_DrawLoadBar()
{
	_BeginScene();
	InitBuckets();
	InitialiseSortList();
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

#ifdef GENERAL_FIXES
	if (tomb5.tr4_loadbar)
	{
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(20, 480 - (font_height >> 1), 600, 15, (long)loadbar_pos, &loadBarColourSet);
		else if (tomb5.bar_mode == 2)
			S_DoTR4Bar(20, phd_winymax - font_height, 600, 15, (long)loadbar_pos, 0xFF000000, 0xFF9F1F80);
		else
			DoBar(20, 480 - (font_height >> 1), 600, 15, (long)loadbar_pos, 0xFF7F007F, 0xFF007F7F);
	}
	else
	{
		if (tomb5.bar_mode == 3)
			S_DrawGouraudBar(170, 480 - font_height, 300, 10, (long)loadbar_pos, &loadBarColourSet);
		else if (tomb5.bar_mode == 2)
			S_DoTR4Bar(170, phd_winymax- (font_height << 1), 300, 10, (long)loadbar_pos, 0xFF000000, 0xFF9F1F80);
		else
			DoBar(170, 480 - font_height, 300, 10, (long)loadbar_pos, 0xA0, 0xF0);
	}
#else
		DoBar(170, 480 - font_height, 300, 10, (long)loadbar_pos, 0xA0, 0xF0);
#endif

	SortPolyList(SortCount, SortList);
	RestoreFPCW(FPCW);
	DrawSortList();
	MungeFPCW(&FPCW);
	S_DumpScreenFrame();

	if (loadbar_pos >= loadbar_steps)
		return loadbar_maxpos <= loadbar_steps;

	loadbar_pos += 2;
	return 0;
}

void S_LoadBar()
{
	S_UpdateLoadBar();
}

void MemBltSurf(void* dest, long x, long y, long w, long h, long dadd, void* source, long x2, long y2, DDSURFACEDESC2 surface, float xsize, float ysize)
{
	ulong* pDest;
	short* psSrc;
	char* pSrc;
	long xadd, yadd, rx2, ry2, xoff, yoff, curY;
	short andVal;
	uchar r, g, b, rshift, gshift, bshift, rcount, gcount, bcount;

	xadd = long(((float)App.dx.dwRenderWidth / 640.0F) * xsize * 65536.0);
	yadd = long(((float)App.dx.dwRenderHeight / 480.0F) * ysize * 65536.0);
	rx2 = long(x2 * ((float)App.dx.dwRenderWidth / 639.0F));
	ry2 = long(y2 * ((float)App.dx.dwRenderHeight / 479.0F));

	if (App.dx.Flags & 2)
	{
		rx2 += App.dx.rScreen.left;
		ry2 += App.dx.rScreen.top;
	}

	DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwRBitMask, &rshift, &rcount);
	DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwGBitMask, &gshift, &gcount);
	DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwBBitMask, &bshift, &bcount);
	pDest = (ulong*)dest + 4 * h * y + x;
	pSrc = (char*)source + rx2 * (surface.ddpfPixelFormat.dwRGBBitCount >> 3) + (ry2 * surface.lPitch);
	psSrc = (short*)pSrc;
	curY = 0;
	yoff = 0;

	if (surface.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		for (int i = 0; i < h; i++)
		{
			xoff = 0;

			for (int j = 0; j < w; j++)
			{
				andVal = psSrc[curY + (xoff >> 16)];
				r = uchar(((surface.ddpfPixelFormat.dwRBitMask & andVal) >> rshift) << (8 - rcount));
				g = uchar(((surface.ddpfPixelFormat.dwGBitMask & andVal) >> gshift) << (8 - gcount));
				b = uchar(((surface.ddpfPixelFormat.dwBBitMask & andVal) >> bshift) << (8 - bcount));
				*pDest = RGBA(r, g, b, 0xFF);
				pDest++;
				xoff += xadd;
			}

			yoff += yadd;
			curY = (surface.lPitch >> 1) * (yoff >> 16);
			pDest += dadd - w;
		}
	}
	else if (surface.ddpfPixelFormat.dwRGBBitCount == 24)
	{
		for (int i = 0; i < h; i++)
		{
			xoff = 0;

			for (int j = 0; j < w; j++)
			{
				r = pSrc[curY + (xoff >> 16)];
				g = pSrc[curY + 1 + (xoff >> 16)];
				b = pSrc[curY + 2 + (xoff >> 16)];
				*pDest = RGBA(r, g, b, 0xFF);
				pDest++;
				xoff += 3 * xadd;
			}

			yoff += yadd;
			curY = surface.lPitch * (yoff >> 16);
			pDest += dadd - w;
		}
	}
	else if (surface.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		for (int i = 0; i < h; i++)
		{
			xoff = 0;

			for (int j = 0; j < w; j++)
			{
				r = pSrc[curY + (xoff >> 16)];
				g = pSrc[curY + 1 + (xoff >> 16)];
				b = pSrc[curY + 2 + (xoff >> 16)];
				*pDest = RGBA(r, g, b, 0xFF);
				pDest++;
				xoff += xadd << 2;
			}

			yoff += yadd;
			curY = surface.lPitch * (yoff >> 16);
			pDest += dadd - w;
		}
	}
}

void RGBM_Mono(uchar* r, uchar* g, uchar* b)
{
	uchar c;

#ifdef GENERAL_FIXES
	if (MonoScreenOn == 2)
		return;
#endif

	c = (*r + *b) >> 1;
	*r = c;
	*g = c;
	*b = c;
}

#ifdef GENERAL_FIXES
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
#endif

void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface)
{
	DDSURFACEDESC2 tSurf;
#ifdef GENERAL_FIXES
	DDSURFACEDESC2 uSurf;
	RECT r;
#endif
	ushort* pTexture;
	ushort* pSrc;

	memset(&tSurf, 0, sizeof(tSurf));
	tSurf.dwSize = sizeof(DDSURFACEDESC2);
	surface->Lock(0, &tSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	pSrc = (ushort*)tSurf.lpSurface;
#ifdef GENERAL_FIXES
	MonoScreen[0].surface = CreateTexturePage(tSurf.dwWidth, tSurf.dwHeight, 0, NULL, RGBM_Mono, -1);

	memset(&uSurf, 0, sizeof(uSurf));
	uSurf.dwSize = sizeof(DDSURFACEDESC2);
	MonoScreen[0].surface->Lock(0, &uSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	pTexture = (ushort*)uSurf.lpSurface;

	r.left = 0;
	r.top = 0;
	r.right = tSurf.dwWidth;
	r.bottom = tSurf.dwHeight;
	CustomBlt(&uSurf, 0, 0, &tSurf, &r);

	MonoScreen[0].surface->Unlock(0);
	DXAttempt(MonoScreen[0].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[0].tex));
	surface->Unlock(0);
#else
	pTexture = (ushort*)malloc(0x40000);

	MemBltSurf(pTexture, 0, 0, 256, 256, 256, pSrc, 0, 0, tSurf, 1.0F, 1.0F);
	MonoScreen[0].surface = CreateTexturePage(256, 256, 0, (long*)pTexture, RGBM_Mono, 0);
	DXAttempt(MonoScreen[0].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[0].tex));

	MemBltSurf(pTexture, 0, 0, 256, 256, 256, pSrc, 256, 0, tSurf, 1.0F, 1.0F);
	MonoScreen[1].surface = CreateTexturePage(256, 256, 0, (long*)pTexture, RGBM_Mono, 0);
	DXAttempt(MonoScreen[1].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[1].tex));

	MemBltSurf(pTexture, 0, 0, 128, 256, 256, pSrc, 512, 0, tSurf, 1.0F, 1.0F);
	MemBltSurf(pTexture, 128, 0, 128, 224, 256, pSrc, 512, 256, tSurf, 1.0F, 1.0F);
	MonoScreen[2].surface = CreateTexturePage(256, 256, 0, (long*)pTexture, RGBM_Mono, 0);
	DXAttempt(MonoScreen[2].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[2].tex));

	MemBltSurf(pTexture, 0, 0, 256, 224, 256, pSrc, 0, 256, tSurf, 1.0F, 1.0F);
	MonoScreen[3].surface = CreateTexturePage(256, 256, 0, (long*)pTexture, RGBM_Mono, 0);
	DXAttempt(MonoScreen[3].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[3].tex));

	MemBltSurf(pTexture, 0, 0, 256, 224, 256, pSrc, 256, 256, tSurf, 1.0F, 1.0F);
	MonoScreen[4].surface = CreateTexturePage(256, 256, 0, (long*)pTexture, RGBM_Mono, 0);
	DXAttempt(MonoScreen[4].surface->QueryInterface(IID_IDirect3DTexture2, (void**)&MonoScreen[4].tex));

	surface->Unlock(0);
	free(pTexture);
#endif
}

void FreeMonoScreen()
{
	if (MonoScreenOn == 1)
	{
		if (MonoScreen[0].surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[0].surface, MonoScreen[0].surface->Release());
			MonoScreen[0].surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");

#ifndef GENERAL_FIXES
		if (MonoScreen[1].surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[1].surface, MonoScreen[1].surface->Release());
			MonoScreen[1].surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");

		if (MonoScreen[2].surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[2].surface, MonoScreen[2].surface->Release());
			MonoScreen[2].surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");

		if (MonoScreen[3].surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[3].surface, MonoScreen[3].surface->Release());
			MonoScreen[3].surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");

		if (MonoScreen[4].surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[4].surface, MonoScreen[4].surface->Release());
			MonoScreen[4].surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
#endif

		if (MonoScreen[0].tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[0].tex, MonoScreen[0].tex->Release());
			MonoScreen[0].tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");

#ifndef GENERAL_FIXES

		if (MonoScreen[1].tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[1].tex, MonoScreen[1].tex->Release());
			MonoScreen[1].tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");

		if (MonoScreen[2].tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[2].tex, MonoScreen[2].tex->Release());
			MonoScreen[2].tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");

		if (MonoScreen[3].tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[3].tex, MonoScreen[3].tex->Release());
			MonoScreen[3].tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");

		if (MonoScreen[4].tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[4].tex, MonoScreen[4].tex->Release());
			MonoScreen[4].tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
#endif
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
	long x[4];
	long y[4];

#ifndef GENERAL_FIXES
	if (MonoScreenOn == 1)
#endif
	{
#ifdef GENERAL_FIXES
		x[0] = phd_winxmin;
		y[0] = phd_winymin;
		x[1] = phd_winxmin + phd_winwidth;
		y[1] = phd_winymin + phd_winheight;
#else
		for (int i = 0; i < 3; i++)
		{
			x[i] = phd_winxmin + phd_winwidth * MonoScreenX[i] / 640;
			y[i] = phd_winymin + phd_winheight * MonoScreenY[i] / 480;
		}

		x[3] = phd_winxmin + phd_winwidth * MonoScreenX[3] / 640;
#endif
		RestoreFPCW(FPCW);

#ifdef GENERAL_FIXES
		if (MonoScreenOn == 2)
			S_DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], MonoScreen[0].tex, 0, 0, 256, 256, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
		else
#endif
			S_DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], MonoScreen[0].tex, 0, 0, 256, 256, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);

#ifndef GENERAL_FIXES
		S_DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], MonoScreen[1].tex, 0, 0, 256, 256, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);
		S_DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], MonoScreen[2].tex, 0, 0, 128, 256, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);
		S_DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], MonoScreen[3].tex, 0, 0, 256, 224, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);
		S_DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], MonoScreen[4].tex, 0, 0, 256, 224, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);
		S_DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], MonoScreen[2].tex, 128, 0, 128, 224, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80, 0xFFFFFF80);
#endif
		MungeFPCW(&FPCW);
	}
}

long S_LoadSave(long load_or_save, long mono)
{
	long fade, ret;

	fade = 0;

	if (!mono)
		CreateMonoScreen();

	GetSaveLoadFiles();
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
		SEEK(file, 0x96000 * screen, SEEK_SET);//fseek(file, 0x96000 * screen, SEEK_SET);
		READ(pic, 0x96000, 1, file);//fread(pic, 0x96000, 1, file);
		CLOSE(file);//fclose(file);
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
		
#ifdef GENERAL_FIXES
		MonoScreenOn = 2;
		ConvertSurfaceToTextures(screen_surface);
#else
		MonoScreenOn = 1;
#endif
	}
	else
		Log(0, "WHORE!");
}

void ReleaseScreen()
{
	MonoScreenOn = 0;

	if (screen_surface)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Picture Surface", screen_surface, screen_surface->Release());
		screen_surface = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Picture Surface");

#ifdef GENERAL_FIXES
	FreeMonoScreen();
#endif
}

void DrawLoadingScreen()
{
	DDSURFACEDESC2 surf;
	ushort* pSrc;
	uchar* pDest;
	float xoff, yoff, xadd, yadd;
	long w, h, val;
	ushort sVal;

	if (!(App.dx.Flags & 0x80))	//software mode bs
	{
		memset(&surf, 0, sizeof(surf));
		surf.dwSize = sizeof(DDSURFACEDESC2);
		screen_surface->Lock(0, &surf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
		pSrc = (ushort*)surf.lpSurface;
		pDest = (uchar*)MMXGetDeviceViewPort(App.dx.lpD3DDevice);
		MMXGetBackSurfWH(w, h);
		xadd = 640.0F / (float)w;
		yadd = 480.0F / (float)h;
		xoff = 0;
		yoff = 0;

		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				val = long(640 * yoff + xoff);
				xoff += xadd;
				sVal = pSrc[val];
				pDest[0] = sVal << 3;			//b
				pDest[1] = (sVal >> 6) << 3;	//g
				pDest[2] = (sVal >> 11) << 3;	//r
				pDest[3] = 0xFF;				//a
				pDest += 4;
			}

			xoff = 0;
			yoff += yadd;
		}

		screen_surface->Unlock(0);
	}
	else
#ifdef GENERAL_FIXES
		S_DisplayMonoScreen();
#else
		G_dxptr->lpBackBuffer->Blt(0, screen_surface, 0, DDBLT_WAIT, 0);
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
	INJECT(0x004B1250, DoBar, replace);
	INJECT(0x004AC430, CreateMonoScreen, replace);
	INJECT(0x004B1A40, S_InitLoadBar, replace);
	INJECT(0x004B1A80, S_UpdateLoadBar, replace);
	INJECT(0x004B1AB0, S_DrawLoadBar, replace);
	INJECT(0x004B1BE0, S_LoadBar, replace);
	INJECT(0x004ABAE0, MemBltSurf, replace);
	INJECT(0x004AC010, RGBM_Mono, replace);
	INJECT(0x004AC050, ConvertSurfaceToTextures, replace);
	INJECT(0x004AC460, FreeMonoScreen, replace);
	INJECT(0x004ACC70, S_DrawTile, replace);
	INJECT(0x004AD010, S_DisplayMonoScreen, replace);
	INJECT(0x004B1120, S_LoadSave, replace);
	INJECT(0x004AC810, LoadScreen, replace);
	INJECT(0x004ACA30, ReleaseScreen, replace);
	INJECT(0x004ACAB0, DrawLoadingScreen, replace);
}
