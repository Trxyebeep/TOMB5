#pragma once
#include "../global/vars.h"

void inject_lighting(bool replace);

void InitObjectLighting(ITEM_INFO* item);
void SuperSetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient);
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void SuperResetLights();
void CalcAmbientLight(ITEM_INFO* item);
void S_CalculateStaticMeshLight(int x, int y, int z, int shade, ROOM_INFO* r);
void SuperSetupDynamicLight(DYNAMIC* light, ITEM_INFO* item);

#ifdef GENERAL_FIXES
extern SPOTLIGHT_STRUCT SpotLights[64];
#endif

#define ClearObjectLighting	( (void(__cdecl*)()) 0x004AB910 )//empty
#define ClearDynamicLighting	( (void(__cdecl*)()) 0x004AB9D0 )//empty
#define InitDynamicLighting_noparams	( (void(__cdecl*)()) 0x004AB930 )
#define InitDynamicLighting	( (void(__cdecl*)(ITEM_INFO*)) 0x004AB950 )
#define InitObjectFogBulbs	( (void(__cdecl*)()) 0x004AB580 )
#define ShowOmni	( (void(__cdecl*)(long, long, long, long)) 0x004AA0C0 )
