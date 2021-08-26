#pragma once
#include "../global/vars.h"

void inject_delshit(bool replace);

void GetLaraJointPos(PHD_VECTOR* pos, long node);
void CalcLaraMatrices(long flag);
#define Rich_CalcLaraMatrices_Interpolated	( (void(__cdecl*)(short*, short*, long, long, long*, long)) 0x0041F9C0 )
#define Rich_CalcLaraMatrices_Normal	( (void(__cdecl*)(short*, long*, int)) 0x0041E460 )
