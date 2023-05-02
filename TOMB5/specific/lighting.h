#pragma once
#include "../global/types.h"

void InitObjectLighting(ITEM_INFO* item);
void SuperSetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient);
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void SuperResetLights();
void CalcAmbientLight(ITEM_INFO* item);
void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r);
void SuperSetupDynamicLight(DYNAMIC* light, ITEM_INFO* item);
void InitDynamicLighting(ITEM_INFO* item);
void InitObjectFogBulbs();

extern long NumSunLights;
extern long NumPointLights;
extern long NumShadowLights;
extern long NumSpotLights;
extern long NumFogBulbs;
extern long TotalNumLights;
extern D3DMATRIX aLightMatrix;
extern ITEM_INFO* current_item;
extern FVECTOR lGlobalMeshPos;
extern long aAmbientR;
extern long aAmbientG;
extern long aAmbientB;
extern long bLaraUnderWater;
extern char bLaraInWater;
extern SUNLIGHT_STRUCT SunLights[16];
extern POINTLIGHT_STRUCT PointLights[64];
extern SPOTLIGHT_STRUCT SpotLights[64];
extern long StaticMeshShade;
