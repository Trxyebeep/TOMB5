#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

void GetJointAbsPositionMatrix(ITEM_INFO* item, float* matrix, long node);

#define GetSpheres	( (int(__cdecl*)(ITEM_INFO*, SPHERE*, int)) 0x00479380 )
#define TestCollision	( (int(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170 )
#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, int)) 0x00479780 )
#define InitInterpolate2	( (void(__cdecl*)(long, long)) 0x00479BB0 )
