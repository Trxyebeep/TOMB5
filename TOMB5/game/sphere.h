#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

void GetJointAbsPositionMatrix(ITEM_INFO* item, float* matrix, long node);
void InitInterpolate2(long frac, long rate);

#define GetSpheres	( (long(__cdecl*)(ITEM_INFO*, SPHERE*, long)) 0x00479380 )
#define TestCollision	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170 )
#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x00479780 )
