#pragma once
#include "../global/types.h"

void inject_drawlara();

void DrawLara(ITEM_INFO* item, int mirror);
void DrawLara__1(ITEM_INFO* item, int mirror);
void DrawLara__4(ITEM_INFO* item, int mirror);
void DrawLara__5(ITEM_INFO* item, int mirror);

#define DrawLara__6	( (void(__cdecl*)(ITEM_INFO*, int)) 0x00499BA0 )
#define SetLaraUnderwaterNodes	( (void(__cdecl*)()) 0x0049A210 )
