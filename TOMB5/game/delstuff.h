#pragma once
#include "../global/vars.h"

void inject_delshit(bool replace);

void GetLaraJointPos(PHD_VECTOR* pos, long node);
void CalcLaraMatrices(long flag);
void Rich_CalcLaraMatrices_Normal(short* frame, long* bone, long flag);
void Rich_CalcLaraMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag);
