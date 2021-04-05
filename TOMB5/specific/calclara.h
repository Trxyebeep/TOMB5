#pragma once
#include "../global/types.h"

void inject_calclara();

short* GetBoundsAccurate(ITEM_INFO* item);
#define GetLaraJointPos	( (void(__cdecl*)(PHD_VECTOR*, long)) 0x0041E2A0 )
//void GetLaraJointPos(struct PHD_VECTOR* pos, long joint);
