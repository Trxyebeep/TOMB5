#pragma once
#include "../global/vars.h"

#define SuperResetLights	( (void(__cdecl*)()) 0x004AAF00 )
#define CalcAmbientLight	( (void(__cdecl*)(ITEM_INFO*)) 0x004A9E60 )
#define CreateLightList		( (void(__cdecl*)(ITEM_INFO*)) 0x004AA5A0 )
#define ClearObjectLighting	( (void(__cdecl*)()) 0x004AB910 )//empty
#define ClearDynamicLighting	( (void(__cdecl*)()) 0x004AB9D0 )//empty
#define InitObjectLighting	( (void(__cdecl*)(ITEM_INFO*)) 0x004AB7A0 )
#define InitDynamicLighting	( (void(__cdecl*)(ITEM_INFO*)) 0x004AB950 )
#define InitObjectFogBulbs	( (void(__cdecl*)()) 0x004AB580 )
