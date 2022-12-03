#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

void SetD3DMatrixF(LPD3DMATRIX dest, float* src);
LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix);
void SaveD3DCameraMatrix();

#define SetD3DViewMatrix	( (void(__cdecl*)()) 0x00497320 )
#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00497650 )
#define D3DMultMatrix	( (LPD3DMATRIX(__cdecl*)(LPD3DMATRIX, LPD3DMATRIX, LPD3DMATRIX)) 0x00497CA0 )
#define S_InitD3DMatrix	( (void(__cdecl*)()) 0x004975D0 )
#define SetD3DMatrix	( (void(__cdecl*)(LPD3DMATRIX, long*)) 0x00497460 )
