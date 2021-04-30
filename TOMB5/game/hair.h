#pragma once

void inject_hair();

#define	DrawHair	( (void(__cdecl*)()) 0x00439A40 )
#define HairControl	( (void(__cdecl*)(int, int, short*)) 0x00438C80 )
#define	InitialiseHair	( (void(__cdecl*)()) 0x00438BE0 )
#define GetCorrectStashPoints	( (void(__cdecl*)(int, int, int)) 0x00439950 )
