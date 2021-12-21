#pragma once
#include "../global/vars.h"

void inject_debris(bool replace);

void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);

#define ShatterObject	( (void(__cdecl*)(SHATTER_ITEM*, MESH_INFO*, short, short, int)) 0x0041D6B0 )
#define UpdateDebris	( (void(__cdecl*)()) 0x0041D500 )
#define GetFreeDebris	( (long(__cdecl*)()) 0x0041D170 )
