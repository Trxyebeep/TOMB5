#pragma once
#include "../global/types.h"

void S_DrawHealthBar(long pos);
void S_DrawAirBar(long pos);
void S_DrawDashBar(long pos);
void S_DrawEnemyBar(long pos);
void S_InitLoadBar(long max);
void S_UpdateLoadBar();
long S_DrawLoadBar();
void S_LoadBar();
void DoSlider(long x, long y, long width, long height, long pos, long c1, long c2, long c3);

extern long loadbar_on;
