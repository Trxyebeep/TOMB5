#pragma once
#include "../global/types.h"

#define GetSpheres	( (int(__cdecl*)(ITEM_INFO*, SPHERE*, int)) 0x00479380 )
#define TestCollision	( (int(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170 )
#define IsRoomOutside	( (int(__cdecl*)(long, long, long)) 0x00418E90 )//this feels out of place
#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, int)) 0x00479780 )
