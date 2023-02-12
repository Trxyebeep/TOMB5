#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

void DrawChar(short x, short y, ushort col, CHARDEF* def);
long GetStringLength(const char* string, short* top, short* bottom);
void PrintString(ushort x, ushort y, uchar col, const char* string, ushort flags);
void InitFont();
void UpdatePulseColour();
void GetStringDimensions(const char* string, ushort* w, ushort* h);

extern CVECTOR FontShades[10][32];
extern uchar ScaleFlag;
extern long small_font;
extern long font_height;
extern long big_char_height;
extern long default_font_height;
extern long GnFrameCounter;
extern CHARDEF CharDef[106];
