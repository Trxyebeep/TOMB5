#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

void SetD3DMatrixF(LPD3DMATRIX dest, float* src);
LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix);
void SaveD3DCameraMatrix();
void SetD3DViewMatrix();
void SetD3DMatrix(D3DMATRIX* mx, long* imx);
void S_InitD3DMatrix();

#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00497650 )
#define D3DMultMatrix	( (LPD3DMATRIX(__cdecl*)(LPD3DMATRIX, LPD3DMATRIX, LPD3DMATRIX)) 0x00497CA0 )
