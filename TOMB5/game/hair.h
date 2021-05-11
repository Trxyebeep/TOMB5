#pragma once

void inject_hair();

void DrawHair();
void HairControl(int in_cutscene, int pigtail, short* cutscenething);

//#define HairControl	( (void(__cdecl*)(int, int, short*)) 0x00438C80 )
#define	InitialiseHair	( (void(__cdecl*)()) 0x00438BE0 )
#define GetCorrectStashPoints	( (void(__cdecl*)(int, int, int)) 0x00439950 )
