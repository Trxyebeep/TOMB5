#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

void DrawChar(short x, short y, ushort col, CHARDEF* def);
long GetStringLength(const char* string, short* top, short* bottom);
void PrintString(ushort x, ushort y, uchar col, const char* string, ushort flags);

#define UpdatePulseColour	( (void(__cdecl*)()) 0x00480830 )
#define InitFont	( (void(__cdecl*)()) 0x00480F00 )
