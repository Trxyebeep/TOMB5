#pragma once
#include "../global/vars.h"

void inject_specificfx(bool replace);

void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void DrawLaserSightSprite();
void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype);
void S_DrawDarts(ITEM_INFO* item);

#define setXY4	( (void(__cdecl*)(D3DTLVERTEX*, int, int, int, int, int, int, int, int, int, short*)) 0x004C05B0 )
#define setXYZ3	( (void(__cdecl*)(D3DTLVERTEX*, long, long, long, long, long, long, long, long, long, short*)) 0x004C0F30 )
#define OutputSky	( (void(__cdecl*)()) 0x004C6D10 )
#define aDrawWreckingBall	( (void(__cdecl*)(ITEM_INFO*, long)) 0x004CF1B0 )
#define SuperShowLogo	( (void(__cdecl*)()) 0x004C9190 )
#define SetFade	( (void(__cdecl*)(int, int)) 0x004CA720 )
#define DrawRope	( (void(__cdecl*)(ROPE_STRUCT*)) 0x004C6E00)
#define _0x004C6BA0	( (void(__cdecl*)(D3DTLVERTEX*, float, float, float, short*)) 0x004C6BA0 )
#define SetUpLensFlare	( (void(__cdecl*)(long, long, long, GAME_VECTOR*)) 0x004C9F70 )
#define ClipLine	( (bool(__cdecl*)(long&, long&, long, long&, long&, long, long, long, long, long)) 0x004C5B10 )
