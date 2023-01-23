#include "../tomb5/pch.h"
#include "text.h"
#include "../specific/specificfx.h"
#include "../specific/polyinsert.h"

void DrawChar(short x, short y, ushort col, CHARDEF* def)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float u1, v1, u2, v2;
	long x1, y1, x2, y2, top, bottom;

	x1 = short(x + phd_winxmin);
	x2 = x1 + def->w;
	y1 = short(y + phd_winymin) + def->YOffset;
	y2 = short(y + phd_winymin) + def->h + def->YOffset;
	setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, (long)f_mznear, clipflags);

	top = *(long*)&FontShades[col][2 * def->TopShade];
	bottom = *(long*)&FontShades[col][2 * def->BottomShade];
	v[0].color = top;
	v[1].color = top;
	v[2].color = bottom;
	v[3].color = bottom;

	top = *(long*)&FontShades[col][(2 * def->TopShade) + 1];
	bottom = *(long*)&FontShades[col][(2 * def->BottomShade) + 1];
	v[0].specular = top;
	v[1].specular = top;
	v[2].specular = bottom;
	v[3].specular = bottom;

	u1 = def->u + (1.0F / 512.0F);
	v1 = def->v + (1.0F / 512.0F);
	u2 = 512.0F / float(phd_winxmax + 1) * (float)def->w * (1.0F / 256.0F) + def->u - (1.0F / 512.0F);
	v2 = 240.0F / float(phd_winymax + 1) * (float)def->h * (1.0F / 256.0F) + def->v - (1.0F / 512.0F);
	tex.u1 = u1;
	tex.v1 = v1;
	tex.u2 = u2;
	tex.v2 = v1;
	tex.u3 = u2;
	tex.v3 = v2;
	tex.u4 = u1;
	tex.v4 = v2;

	tex.drawtype = 1;
	tex.tpage = ushort(nTextures - 2);
	tex.flag = 0;
	nPolyType = 4;
	AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}

long GetStringLength(const char* string, short* top, short* bottom)
{
	CHARDEF* def;
	long s, accent, length;
	short lowest, highest, y;

	s = *string++;
	length = 0;
	accent = 0;
	lowest = -1024;
	highest = 1024;

	while (s)
	{
		if (s == '\n')
			break;

		if (s == ' ')
			length += long((float(phd_winxmax + 1) / 640.0F) * 8.0F);
		else if (s == '\t')
		{
			length += 40;

			if (top && highest > -12)
				highest = -12;

			if (bottom && lowest < 2)
				lowest = 2;
		}
		else if (s >= 20)
		{
			if (s < ' ')
				def = &CharDef[s + 74];
			else
			{
				/*
				if (s >= 128 && s <= 173)
				{
					accent = 1;
					s = AccentTable[s - 128][0];
				}
				*/
				def = &CharDef[s - '!'];
			}

			if (ScaleFlag)
				length += def->w - def->w / 4;
			else
				length += def->w;

			y = def->YOffset;

			if (top && y < highest)
				highest = def->YOffset;

			if (bottom && def->h + y > lowest)
				lowest = def->h + y;
		}

		s = *string++;
	}

	if (top)
	{
		/*
		if (accent)
			highest -= 4;
		*/

		*top = highest;
	}

	if (bottom)
		*bottom = lowest;

	return length;
}

void PrintString(ushort x, ushort y, uchar col, const char* string, ushort flags)
{
	CHARDEF* def;
	CHARDEF* accent;
	short x2, bottom, l, top, bottom2;
	uchar s;

	if (flags & FF_BLINK && GnFrameCounter & 0x10)
		return;

	ScaleFlag = (flags & FF_SMALL) != 0;
	x2 = (short)GetStringLength(string, 0, &bottom);

	if (flags & FF_CENTER)
		x2 = x - (x2 >> 1);
	else if (flags & FF_RJUSTIFY)
		x2 = x - x2;
	else
		x2 = x;

	s = *string++;

	while (s)
	{
		if (s == '\n')
		{
			if (*string == '\n')
			{
				bottom = 0;
				y += 16;
			}
			else
			{
				l = (short)GetStringLength(string, &top, &bottom2);

				if (flags & FF_CENTER)
					x2 = x - (l >> 1);
				else if (flags & FF_RJUSTIFY)
					x2 = x - l;
				else
					x2 = x;

				y += bottom - top + 2;
				bottom = bottom2;
			}

			s = *string++;
			continue;
		}

		if (s == ' ')
		{
			if (ScaleFlag)
				x2 += 6;
			else
				x2 += short(float(phd_winxmax + 1) / 640.0F * 8.0F);

			s = *string++;
			continue;
		}

		if (s == '\t')
		{
			x2 += 40;
			s = *string++;
			continue;
		}

		if (s < 20)
		{
			col = s - 1;
			s = *string++;
			continue;
		}

		/*
		if (s >= 128 && s <= 173)
		{
			def = &CharDef[AccentTable[s - 128][0] - '!'];
			accent = &CharDef[AccentTable[s - 128][1] - '!'];
			DrawChar(x2, y, col, def);

			if (AccentTable[s - 128][1] != ' ')
				DrawChar(def->w / 2 + x2 - 3, y + def->YOffset, col, accent);
		}
		else
		*/
		{
			if (s < ' ')
				def = &CharDef[s + 74];
			else
				def = &CharDef[s - '!'];

			DrawChar(x2, y, col, def);
		}

		if (ScaleFlag)
			x2 += def->w - def->w / 4;
		else
			x2 += def->w;

		s = *string++;
	}

	ScaleFlag = 0;
}

void inject_text(bool replace)
{
	INJECT(0x004805D0, DrawChar, replace);
	INJECT(0x00480910, GetStringLength, replace);
	INJECT(0x00480BC0, PrintString, replace);
}
