#pragma once
#include "../global/vars.h"

#define GetStringLength	( (int(__cdecl*)(char*, short*, short*)) 0x00480910 )
#define	PrintString	( (void(__cdecl*)(ushort, ushort, uchar, const char*, ushort)) 0x00480BC0 )
#define UpdatePulseColour	( (void(__cdecl*)()) 0x00480830 )
