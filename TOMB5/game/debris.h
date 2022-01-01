#pragma once
#include "../global/vars.h"

void inject_debris(bool replace);

void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);
long GetFreeDebris();
void UpdateDebris();

#define ShatterObject	( (void(__cdecl*)(SHATTER_ITEM*, MESH_INFO*, short, short, int)) 0x0041D6B0 )
