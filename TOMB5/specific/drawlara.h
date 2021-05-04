#pragma once
#include "../global/types.h"

void inject_drawlara();

void DrawLara(ITEM_INFO* item, int mirror);
void DrawLara__1(ITEM_INFO* item, int mirror);

#define DrawLara__4	( (void(__cdecl*)(ITEM_INFO*, int)) 0x00498C70 )
#define DrawLara__5	( (void(__cdecl*)(ITEM_INFO*, int)) 0x004995C0 )
#define DrawLara__6	( (void(__cdecl*)(ITEM_INFO*, int)) 0x00499BA0 )
#define SetLaraUnderwaterNodes	( (void(__cdecl*)()) 0x0049A210 )
