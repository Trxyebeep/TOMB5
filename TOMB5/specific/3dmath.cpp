#include "../tomb5/pch.h"
#include "3dmath.h"
#include "d3dmatrix.h"
#include "dxshell.h"

void AlterFOV(short fov)
{
	static short old_fov = 0;

	if (fov)
		old_fov = fov;
	else
		fov = old_fov;

	CurrentFov = fov;
	fov /= 2;

#ifdef TR2MAIN_WIDESCREEN	//by Arsunt
	long fov_width;

	fov_width = phd_winheight * 320 / 240;
	LfAspectCorrection = 1.0F; // must always be 1.0 for unstretched view
	phd_persp = (fov_width / 2) * phd_cos(fov) / phd_sin(fov);
#else
	phd_persp = ((phd_winwidth / 2) * phd_cos(fov)) / phd_sin(fov);
#endif

	f_persp = float(phd_persp);
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;

#ifndef TR2MAIN_WIDESCREEN
	LfAspectCorrection = (4.0F / 3.0F) / (float(phd_winwidth) / float(phd_winheight));
#endif

	f_mpersp = f_persp;
	f_moneopersp = mone / f_persp;
	f_mperspoznear = f_persp / f_mznear;
}

void gte_sttr(PHD_VECTOR* vec)
{
	vec->x = phd_mxptr[M03] >> 14;
	vec->y = phd_mxptr[M13] >> 14;
	vec->z = phd_mxptr[M23] >> 14;
}

void aInitMatrix()
{
	float* ptr;
	float ang;

	for (int i = 0; i < 65536; i++)
	{
		ptr = &fcossin_tbl[i];
		ang = i * 0.000095873802F;
		*ptr = sin(ang);
	}

	aMXPtr = aFMatrixStack;
}

void aSetViewMatrix()
{
	SetD3DMatrixF(&D3DMView, aMXPtr);
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &D3DMView));
}

void aSetTrans(long x, long y, long z)
{
	aMXPtr[M03] = (float)x;
	aMXPtr[M13] = (float)y;
	aMXPtr[M23] = (float)z;
}

void aTranslateAbs(long x, long y, long z)
{
	float fx, fy, fz;

	fx = x - aCamera.matrix[M03];
	fy = y - aCamera.matrix[M13];
	fz = z - aCamera.matrix[M23];
	aMXPtr[M03] = fx * aMXPtr[M00] + fy * aMXPtr[M01] + fz * aMXPtr[M02];
	aMXPtr[M13] = fx * aMXPtr[M10] + fy * aMXPtr[M11] + fz * aMXPtr[M12];
	aMXPtr[M23] = fx * aMXPtr[M20] + fy * aMXPtr[M21] + fz * aMXPtr[M22];
}

void aUnitMatrixByMat(float* matrix)
{
	matrix[M00] = 1;
	matrix[M01] = 0;
	matrix[M02] = 0;
	matrix[M03] = 0;
	matrix[M10] = 0;
	matrix[M11] = 1;
	matrix[M12] = 0;
	matrix[M13] = 0;
	matrix[M20] = 0;
	matrix[M21] = 0;
	matrix[M22] = 1;
	matrix[M23] = 0;
}

void aPushUnitMatrix()
{
	aMXPtr += 12;
	aUnitMatrixByMat(aMXPtr);
}

long aTranslateRel(long x, long y, long z)
{
	aMXPtr[M03] += x * aMXPtr[M00] + y * aMXPtr[M01] + z * aMXPtr[M02];
	aMXPtr[M13] += x * aMXPtr[M10] + y * aMXPtr[M11] + z * aMXPtr[M12];
	aMXPtr[M23] += x * aMXPtr[M20] + y * aMXPtr[M21] + z * aMXPtr[M22];
	return 1;
}

void aRotX(short angle)
{
	float sin, cos, trash;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);
		trash = aMXPtr[M01];
		aMXPtr[M01] = cos * aMXPtr[M01] + sin * aMXPtr[M02];
		aMXPtr[M02] = cos * aMXPtr[M02] - sin * trash;
		trash = aMXPtr[M11];
		aMXPtr[M11] = cos * aMXPtr[M11] + sin * aMXPtr[M12];
		aMXPtr[M12] = cos * aMXPtr[M12] - sin * trash;
		trash = aMXPtr[M21];
		aMXPtr[M21] = cos * aMXPtr[M21] + sin * aMXPtr[M22];
		aMXPtr[M22] = cos * aMXPtr[M22] - sin * trash;
	}
}

void aRotY(short angle)
{
	float sin, cos, trash;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);
		trash = aMXPtr[M00];
		aMXPtr[M00] = cos * aMXPtr[M00] - sin * aMXPtr[M02];
		aMXPtr[M02] = cos * aMXPtr[M02] + sin * trash;
		trash = aMXPtr[M10];
		aMXPtr[M10] = cos * aMXPtr[M10] - sin * aMXPtr[M12];
		aMXPtr[M12] = cos * aMXPtr[M12] + sin * trash;
		trash = aMXPtr[M20];
		aMXPtr[M20] = cos * aMXPtr[M20] - sin * aMXPtr[M22];
		aMXPtr[M22] = cos * aMXPtr[M22] + sin * trash;
	}
}

void aRotZ(short angle)
{
	float sin, cos, trash;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);
		trash = aMXPtr[M00];
		aMXPtr[M00] = cos * aMXPtr[M00] + sin * aMXPtr[M01];
		aMXPtr[M01] = cos * aMXPtr[M01] - sin * trash;
		trash = aMXPtr[M10];
		aMXPtr[M10] = cos * aMXPtr[M10] + sin * aMXPtr[M11];
		aMXPtr[M11] = cos * aMXPtr[M11] - sin * trash;
		trash = aMXPtr[M20];
		aMXPtr[M20] = cos * aMXPtr[M20] + sin * aMXPtr[M21];
		aMXPtr[M21] = cos * aMXPtr[M21] - sin * trash;
	}
}

void aRotYXZPack(long angles)//angles is XYZ, not YXZ as the name suggests, ty core
{
	short angle;

	angle = (angles >> 10) & 0x3FF;//second ten bits, Y
	angle <<= 6;//* 64

	if (angle)
		aRotY(angle);

	angle = (angles >> 20) & 0x3FF;//firrst ten bits, X
	angle <<= 6;//* 64

	if (angle)
		aRotX(angle);

	angle = angles & 0x3FF;//last ten, Z
	angle <<= 6;//* 64

	if (angle)
		aRotZ(angle);
}

void aRotYXZ(short y, short x, short z)
{
	if (y)
		aRotY(y);

	if (x)
		aRotX(x);

	if (z)
		aRotZ(z);
}

void inject_3dmath(bool replace)
{
	INJECT(0x0048EDC0, AlterFOV, replace);
	INJECT(0x00491320, gte_sttr, replace);
	INJECT(0x00490590, aInitMatrix, replace);
	INJECT(0x00490BE0, aSetViewMatrix, replace);
	INJECT(0x004905E0, aSetTrans, replace);
	INJECT(0x00490610, aTranslateAbs, replace);
	INJECT(0x004912E0, aUnitMatrixByMat, replace);
	INJECT(0x00490720, aPushUnitMatrix, replace);
	INJECT(0x00490790, aTranslateRel, replace);
	INJECT(0x00490810, aRotX, replace);
	INJECT(0x004908E0, aRotY, replace);
	INJECT(0x004909B0, aRotZ, replace);
	INJECT(0x00490A80, aRotYXZPack, replace);
	INJECT(0x00490AF0, aRotYXZ, replace);
}
