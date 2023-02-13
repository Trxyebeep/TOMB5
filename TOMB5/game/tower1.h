#pragma once
#include "../global/types.h"

long TestBoundsCollideCamera(short* bounds, PHD_3DPOS* pos, long radius);
void CheckForRichesIllegalDiagonalWalls();
void ItemPushCamera(short* bounds, PHD_3DPOS* pos);
