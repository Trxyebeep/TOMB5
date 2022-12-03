#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

void SetD3DMatrixF(LPD3DMATRIX dest, float* src);
LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix);
void SaveD3DCameraMatrix();
void SetD3DViewMatrix();
void SetD3DMatrix(D3DMATRIX* mx, long* imx);
void S_InitD3DMatrix();
D3DVECTOR* D3DNormalise(D3DVECTOR* vec);
D3DVECTOR* D3DVSubtract(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b);
D3DVECTOR* D3DVAdd(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b);
D3DVECTOR* D3DCrossProduct(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b);
float D3DDotProduct(D3DVECTOR* a, D3DVECTOR* b);
D3DMATRIX* D3DSetTranslate(D3DMATRIX* mx, float x, float y, float z);
D3DMATRIX* D3DSetRotateX(D3DMATRIX* mx, float ang);
D3DMATRIX* D3DSetRotateY(D3DMATRIX* mx, float ang);
D3DMATRIX* D3DSetRotateZ(D3DMATRIX* mx, float ang);
D3DMATRIX* D3DSetScale(D3DMATRIX* mx, float scale);

#define D3DMultMatrix	( (LPD3DMATRIX(__cdecl*)(LPD3DMATRIX, LPD3DMATRIX, LPD3DMATRIX)) 0x00497CA0 )
