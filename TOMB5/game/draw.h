#pragma once
#include "../global/vars.h"

void inject_draw(bool replace);


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);
void InitInterpolate(int frac, int rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(int x, int y, int z);
void phd_TranslateRel_ID(int x, int y, int z, int x2, int y2, int z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, int skip);
void gar_RotYXZsuperpack(short** pprot, int skip);
void phd_PutPolygons_I(short* ptr, int clip);
void aInterpolateMatrix();
long DrawPhaseGame();
void SkyDrawPhase();
void UpdateSkyLightning();

#define InterpolateMatrix	( (void(__cdecl*)()) 0x0042C8F0 )
#define	mRotBoundingBoxNoPersp	( (void(__cdecl*)(short*, short*)) 0x0042E240 )
#define GetFrames	( (long(__cdecl*)(ITEM_INFO*, short*[], int*)) 0x0042CEB0 )
#define DrawRooms	( (void(__cdecl*)(short)) 0x0042A7A0 )
#define CalculateObjectLightingLara	( (void(__cdecl*)()) 0x0042A1B0 )
#define CalculateObjectLighting	( (void(__cdecl*)(ITEM_INFO*, short*)) 0x0042CD50 )
