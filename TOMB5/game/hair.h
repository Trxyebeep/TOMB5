#pragma once
#include "../global/types.h"

void DrawHair();
void HairControl(long in_cutscene, long pigtail, short* cutscenething);
void InitialiseHair();
void GetCorrectStashPoints(long pigtail, long hair_nose, long skin_node);

extern HAIR_STRUCT hairs[2][7];
