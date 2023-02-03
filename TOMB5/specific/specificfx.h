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
void SetFade(long start, long end);
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
void SuperDrawBox(long* box);
void Draw2DSprite(long x, long y, long slot, long unused, long unused2);
void DrawBikeSpeedo(long ux, long uy, long vel, long maxVel, long turboVel, long size, long unk);
void DrawShockwaves();
void DrawPsxTile(long x_y, long height_width, long color, long u0, long u1);
void DrawRedTile(long x_y, long height_width);
void DrawFlash();
void SuperShowLogo();
void DrawDebris();
void DrawBlood();
void DrawDrips();
void DoUwEffect();
void DrawWraithTrail(ITEM_INFO* item);
void DrawTrainFloorStrip(long x, long z, TEXTURESTRUCT* tex, long y_and_flags);
void DrawTrainStrips();
void DrawBubbles();
void DrawSprite(long x, long y, long slot, long col, long size, long z);
void SetUpLensFlare(long x, long y, long z, GAME_VECTOR* lfobj);
bool ClipLine(long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xMin, long yMin, long w, long h);
void S_DrawSparks();
void DrawLasers(ITEM_INFO* item);
void DrawSteamLasers(ITEM_INFO* item);
#ifdef GENERAL_FIXES
void S_DrawFloorLasers(ITEM_INFO* item);
#endif
void DrawLightning();
void OldDrawLightning();
void DrawTwogunLaser(TWOGUN_INFO* info);

#define DrawRope	( (void(__cdecl*)(ROPE_STRUCT*)) 0x004C6E00)
#define S_DrawSmokeSparks	( (void(__cdecl*)()) 0x004C2980 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x004C1790 )
