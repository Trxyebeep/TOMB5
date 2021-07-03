#pragma once
#include "../global/vars.h"

#define GetStringLength	( (int(__cdecl*)(char*, short*, short*)) 0x00480910 )
#define	PrintString	( (void(__cdecl*)(unsigned short, unsigned short, unsigned char, const char*, unsigned short)) 0x00480BC0 )
