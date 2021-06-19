#pragma once
#include "../global/vars.h"

#define GetSpheres	( (int(__cdecl*)(ITEM_INFO*, SPHERE*, int)) 0x00479380 )
#define TestCollision	( (int(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170 )
#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, int)) 0x00479780 )
