#pragma once
#include "../global/vars.h"

void inject_tower1(bool replace);

long TestBoundsCollideCamera(short* bounds, PHD_3DPOS* pos, long radius);
void CheckForRichesIllegalDiagonalWalls();
void ItemPushCamera(short* bounds, PHD_3DPOS* pos);
