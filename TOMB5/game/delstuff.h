#pragma once
#include "../global/types.h"

void GetLaraJointPos(PHD_VECTOR* pos, long node);
void CalcLaraMatrices(long flag);
void Rich_CalcLaraMatrices_Normal(short* frame, long* bone, long flag);
void Rich_CalcLaraMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag);

extern short* GLaraShadowframe;
extern float lara_matrices[180];
extern float lara_joint_matrices[180];
extern long LaraNodeAmbient[2];
extern uchar LaraNodeUnderwater[15];
extern char SkinVertNums[40][12];
extern char ScratchVertNums[40][12];
