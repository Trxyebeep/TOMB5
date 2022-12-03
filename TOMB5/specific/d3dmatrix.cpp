#include "../tomb5/pch.h"
#include "d3dmatrix.h"
#include "dxshell.h"

static D3DMATRIX D3DMWorld;
static D3DMATRIX D3DMProjection;

void SetD3DMatrixF(LPD3DMATRIX dest, float* src)
{
	D3DIdentityMatrix(dest);
	dest->_11 = src[M00];
	dest->_12 = src[M10];
	dest->_13 = src[M20];
	dest->_21 = src[M01];
	dest->_22 = src[M11];
	dest->_23 = src[M21];
	dest->_31 = src[M02];
	dest->_32 = src[M12];
	dest->_33 = src[M22];
	dest->_41 = src[M03];
	dest->_42 = src[M13];
	dest->_43 = src[M23];
}

LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix)
{
	matrix->_11 = 1;
	matrix->_12 = 0;
	matrix->_13 = 0;
	matrix->_14 = 0;
	matrix->_21 = 0;
	matrix->_22 = 1;
	matrix->_23 = 0;
	matrix->_24 = 0;
	matrix->_31 = 0;
	matrix->_32 = 0;
	matrix->_33 = 1;
	matrix->_34 = 0;
	matrix->_41 = 0;
	matrix->_42 = 0;
	matrix->_43 = 0;
	matrix->_44 = 1;
	return matrix;
}

void SaveD3DCameraMatrix()
{
	D3DIdentityMatrix(&D3DCameraMatrix);
	D3DCameraMatrix._11 = aMXPtr[M00];
	D3DCameraMatrix._12 = aMXPtr[M10];
	D3DCameraMatrix._13 = aMXPtr[M20];
	D3DCameraMatrix._21 = aMXPtr[M01];
	D3DCameraMatrix._22 = aMXPtr[M11];
	D3DCameraMatrix._23 = aMXPtr[M21];
	D3DCameraMatrix._31 = aMXPtr[M02];
	D3DCameraMatrix._32 = aMXPtr[M12];
	D3DCameraMatrix._33 = aMXPtr[M22];
	D3DCameraMatrix._41 = aMXPtr[M03];
	D3DCameraMatrix._42 = aMXPtr[M13];
	D3DCameraMatrix._43 = aMXPtr[M23];
}

void SetD3DViewMatrix()
{
	D3DIdentityMatrix(&D3DMView);
	D3DMView._11 = (float)phd_mxptr[M00] / 16384;
	D3DMView._12 = (float)phd_mxptr[M10] / 16384;
	D3DMView._13 = (float)phd_mxptr[M20] / 16384;
	D3DMView._21 = (float)phd_mxptr[M01] / 16384;
	D3DMView._22 = (float)phd_mxptr[M11] / 16384;
	D3DMView._23 = (float)phd_mxptr[M21] / 16384;
	D3DMView._31 = (float)phd_mxptr[M02] / 16384;
	D3DMView._32 = (float)phd_mxptr[M12] / 16384;
	D3DMView._33 = (float)phd_mxptr[M22] / 16384;
	D3DMView._41 = float(phd_mxptr[M03] >> 14);
	D3DMView._42 = float(phd_mxptr[M13] >> 14);
	D3DMView._43 = float(phd_mxptr[M23] >> 14);
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &D3DMView));
}

void SetD3DMatrix(D3DMATRIX* mx, long* imx)
{
	D3DIdentityMatrix(mx);
	mx->_11 = (float)imx[M00] / 16384;
	mx->_12 = (float)imx[M10] / 16384;
	mx->_13 = (float)imx[M20] / 16384;
	mx->_21 = (float)imx[M01] / 16384;
	mx->_22 = (float)imx[M11] / 16384;
	mx->_23 = (float)imx[M21] / 16384;
	mx->_31 = (float)imx[M02] / 16384;
	mx->_32 = (float)imx[M12] / 16384;
	mx->_33 = (float)imx[M22] / 16384;
	mx->_41 = float(imx[M03] >> 14);
	mx->_42 = float(imx[M13] >> 14);
	mx->_43 = float(imx[M23] >> 14);
}

void S_InitD3DMatrix()
{
	D3DIdentityMatrix(&D3DMWorld);
	D3DIdentityMatrix(&D3DMProjection);
	D3DMProjection._22 = -1;
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &D3DMWorld));
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &D3DMProjection));
}

D3DVECTOR* D3DNormalise(D3DVECTOR* vec)
{
	float val;

	if (vec->x != 0 || vec->y != 0 || vec->z != 0)
	{
		val = 1.0F / sqrt(SQUARE(vec->x) + SQUARE(vec->y) + SQUARE(vec->z));
		vec->x = val * vec->x;
		vec->y = val * vec->y;
		vec->z = val * vec->z;
	}

	return vec;
}

D3DVECTOR* D3DVSubtract(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
	return out;
}

D3DVECTOR* D3DVAdd(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b)
{
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
	return out;
}

D3DVECTOR* D3DCrossProduct(D3DVECTOR* out, D3DVECTOR* a, D3DVECTOR* b)
{
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
	return out;
}

float D3DDotProduct(D3DVECTOR* a, D3DVECTOR* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

D3DMATRIX* D3DSetTranslate(D3DMATRIX* mx, float x, float y, float z)
{
	mx->_41 = x;
	mx->_42 = y;
	mx->_43 = z;
	return mx;
}

D3DMATRIX* D3DSetRotateX(D3DMATRIX* mx, float ang)
{
	float s, c;

	s = sin(ang);
	c = cos(ang);
	mx->_22 = c;
	mx->_23 = -s;
	mx->_32 = s;
	mx->_33 = c;
	return mx;
}

D3DMATRIX* D3DSetRotateY(D3DMATRIX* mx, float ang)
{
	float s, c;

	s = sin(ang);
	c = cos(ang);
	mx->_11 = c;
	mx->_13 = s;
	mx->_31 = -s;
	mx->_33 = c;
	return mx;
}

D3DMATRIX* D3DSetRotateZ(D3DMATRIX* mx, float ang)
{
	float s, c;

	s = sin(ang);
	c = cos(ang);
	mx->_11 = c;
	mx->_12 = -s;
	mx->_21 = s;
	mx->_22 = c;
	return mx;
}

D3DMATRIX* D3DSetScale(D3DMATRIX* mx, float scale)
{
	mx->_11 = scale;
	mx->_22 = scale;
	mx->_33 = scale;
	return mx;
}

void inject_d3dmatrix(bool replace)
{
	INJECT(0x00497550, SetD3DMatrixF, replace);
	INJECT(0x00497910, D3DIdentityMatrix, replace);
	INJECT(0x00497280, SaveD3DCameraMatrix, replace);
	INJECT(0x00497320, SetD3DViewMatrix, replace);
	INJECT(0x00497460, SetD3DMatrix, replace);
	INJECT(0x004975D0, S_InitD3DMatrix, replace);
	INJECT(0x00497650, D3DNormalise, replace);
	INJECT(0x004976F0, D3DVSubtract, replace);
	INJECT(0x00497730, D3DVAdd, replace);
	INJECT(0x00497770, D3DCrossProduct, replace);
	INJECT(0x004977C0, D3DDotProduct, replace);
	INJECT(0x004977F0, D3DSetTranslate, replace);
	INJECT(0x00497820, D3DSetRotateX, replace);
	INJECT(0x00497860, D3DSetRotateY, replace);
	INJECT(0x004978A0, D3DSetRotateZ, replace);
	INJECT(0x004978E0, D3DSetScale, replace);
}
