#pragma once
#include "../global/types.h"

void SetD3DMatrix(D3DMATRIX* mx, float* imx);
LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix);
void SaveD3DCameraMatrix();
void S_InitD3DMatrix();
D3DVECTOR* D3DNormalise(D3DVECTOR* vec);
LPD3DMATRIX D3DMultMatrix(LPD3DMATRIX d, LPD3DMATRIX a, LPD3DMATRIX b);

extern D3DMATRIX D3DMView;
extern D3DMATRIX D3DCameraMatrix;
extern D3DMATRIX D3DInvCameraMatrix;
