#pragma once
#include "../global/vars.h"

void inject_lighting(bool replace);

void InitObjectLighting(ITEM_INFO* item);
void SuperSetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient);
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void SuperResetLights();
void CalcAmbientLight(ITEM_INFO* item);
void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r);
void SuperSetupDynamicLight(DYNAMIC* light, ITEM_INFO* item);
void InitDynamicLighting_noparams();
void InitDynamicLighting(ITEM_INFO* item);
void ClearObjectLighting();
void ClearDynamicLighting();
void ApplyMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest);
void ApplyTransposeMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest);
void CreateD3DLights();
void FreeD3DLights();
void MallocD3DLights();
void InitObjectFogBulbs();
void SetupLight(D3DLIGHT_STRUCT* d3dlight, PCLIGHT* light, long* ambient);
void ShowOmni(long x, long y, long z, long rad);

#ifdef GENERAL_FIXES
extern SPOTLIGHT_STRUCT SpotLights[64];
#endif
