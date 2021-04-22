#pragma once

//void PrintString(unsigned short x, unsigned short y, unsigned char colourFlag, const char* string, unsigned short flag)
#define	PrintString	( (void(__cdecl*)(unsigned short, unsigned short, unsigned char, const char*, unsigned short)) 0x00480BC0 )
#define IsHardware	( (int(__cdecl*)()) 0x004B1E90 )
#define IsSuperLowRes	( (int(__cdecl*)()) 0x004B1EB0 )
