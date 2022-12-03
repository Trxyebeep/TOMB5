#pragma once
#include "../global/vars.h"

void inject_specificfx(bool replace);

void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void DrawLaserSightSprite();
void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype);
void S_DrawDarts(ITEM_INFO* item);
void DrawMoon();
void DrawGasCloud(ITEM_INFO* item);
void DrawStarField();
void setXYZ3(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip);
void setXYZ4(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip);
void setXY3(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long z, short* clip);
void setXY4(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip);
void S_DrawDrawSparksNEW(SPARKS* sptr, long smallest_size, float* xyz);
void DoRain();
void OutputSky();
void DoScreenFade();
void ClipCheckPoint(D3DTLVERTEX* v, float x, float y, float z, short* clip);
void aTransformPerspSV(SVECTOR* vec, D3DTLVERTEX* v, short* c, long nVtx, long col);
void DrawBinoculars();
void aDrawWreckingBall(ITEM_INFO* item, long shade);
void ClearFX();
void AddPolyLine(D3DTLVERTEX* vtx, TEXTURESTRUCT* tex);
void DoSnow();
void aInitFX();
void DoWeather();
void aSetXY4(D3DTLVERTEX* v, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, short* clip);
void InitTarget();
void InitBinoculars();

#define SuperShowLogo	( (void(__cdecl*)()) 0x004C9190 )
#define SetFade	( (void(__cdecl*)(long, long)) 0x004CA720 )
#define DrawRope	( (void(__cdecl*)(ROPE_STRUCT*)) 0x004C6E00)
#define SetUpLensFlare	( (void(__cdecl*)(long, long, long, GAME_VECTOR*)) 0x004C9F70 )
#define ClipLine	( (bool(__cdecl*)(long&, long&, long, long&, long&, long, long, long, long, long)) 0x004C5B10 )
#define DrawLasers	( (void(__cdecl*)(ITEM_INFO*)) 0x004CD960 )
#define DrawSteamLasers	( (void(__cdecl*)(ITEM_INFO*)) 0x004CE610 )
#define S_DrawSparks	( (void(__cdecl*)()) 0x004C4C60 )
#define DoUwEffect	( (void(__cdecl*)()) 0x004C8650 )
#define S_DrawSmokeSparks	( (void(__cdecl*)()) 0x004C2980 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x004C1790 )
#define DrawBubbles	( (void(__cdecl*)()) 0x004C1340 )
#define DrawDebris	( (void(__cdecl*)()) 0x004C7620 )
#define DrawBlood	( (void(__cdecl*)()) 0x004C7CB0 )
#define DrawDrips	( (void(__cdecl*)()) 0x004C8110 )
#define DrawShockwaves	( (void(__cdecl*)()) 0x004CB280 )
#define DrawLightning	( (void(__cdecl*)()) 0x004CC0B0 )
#define OldDrawLightning	( (void(__cdecl*)()) 0x004CCBA0 )
#define DrawPsxTile	( (void(__cdecl*)(long, long, long, long, long)) 0x004CA7F0 )
#define DrawFlash	( (void(__cdecl*)()) 0x004CAC90 )
