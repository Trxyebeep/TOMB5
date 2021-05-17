#pragma once
#include "../global/vars.h"

void inject_hair();

void DrawHair();
void HairControl(int in_cutscene, int pigtail, short* cutscenething);
void InitialiseHair();

#define GetCorrectStashPoints	( (void(__cdecl*)(int, int, int)) 0x00439950 )
