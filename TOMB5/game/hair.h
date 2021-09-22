#pragma once
#include "../global/vars.h"

void inject_hair(bool replace);

void DrawHair();
void HairControl(int in_cutscene, int pigtail, short* cutscenething);
void InitialiseHair();
void GetCorrectStashPoints(long pigtail, long hair_nose, long skin_node);
