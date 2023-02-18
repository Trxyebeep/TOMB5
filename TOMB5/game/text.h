#pragma once
#include "../global/types.h"

void DrawChar(long x, long y, ushort col, CHARDEF* def);
long GetStringLength(const char* string, long* top, long* bottom);
void PrintString(long x, long y, uchar col, const char* string, ushort flags);
void InitFont();
void UpdatePulseColour();

extern CVECTOR FontShades[10][32];
extern uchar ScaleFlag;
extern long font_height;
extern long default_font_height;
extern long GnFrameCounter;
extern CHARDEF CharDef[106];
