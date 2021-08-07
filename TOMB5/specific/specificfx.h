#pragma once
#include "../global/vars.h"

void inject_specificfx(bool replace);

void S_PrintShadow(short size, short* box, ITEM_INFO* item, short unknown);
void DrawLaserSightSprite();

#define setXY4	( (void(__cdecl*)(D3DTLVERTEX*, int, int, int, int, int, int, int, int, int, short*)) 0x004C05B0 )
#define setXYZ3	( (void(__cdecl*)(D3DTLVERTEX*, long, long, long, long, long, long, long, long, long, short*)) 0x004C0F30 )
#define DrawFlatSky	( (void(__cdecl*)(ulong, int, int, int)) 0x004C5EA0 )
#define OutputSky	( (void(__cdecl*)()) 0x004C6D10 )
#define aDrawWreckingBall	( (void(__cdecl*)(ITEM_INFO*, long)) 0x004CF1B0 )
#define SuperShowLogo	( (void(__cdecl*)()) 0x004C9190 )
#define SetFade	( (void(__cdecl*)(int, int)) 0x004CA720 )
