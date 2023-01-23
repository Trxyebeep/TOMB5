#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

void DrawChar(short x, short y, ushort col, CHARDEF* def);
long GetStringLength(const char* string, short* top, short* bottom);

#define	PrintString	( (void(__cdecl*)(ushort, ushort, uchar, const char*, ushort)) 0x00480BC0 )
#define UpdatePulseColour	( (void(__cdecl*)()) 0x00480830 )
#define InitFont	( (void(__cdecl*)()) 0x00480F00 )
