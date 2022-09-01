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
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * aMXPtr[M01] + sin * aMXPtr[M02];
		mx2 = cos * aMXPtr[M02] - sin * aMXPtr[M01];
		aMXPtr[M01] = mx1;
		aMXPtr[M02] = mx2;

		mx1 = cos * aMXPtr[M11] + sin * aMXPtr[M12];
		mx2 = cos * aMXPtr[M12] - sin * aMXPtr[M11];
		aMXPtr[M11] = mx1;
		aMXPtr[M12] = mx2;

		mx1 = cos * aMXPtr[M21] + sin * aMXPtr[M22];
		mx2 = cos * aMXPtr[M22] - sin * aMXPtr[M21];
		aMXPtr[M21] = mx1;
		aMXPtr[M22] = mx2;
	}
}

void aRotY(short angle)
{
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * aMXPtr[M00] - sin * aMXPtr[M02];
		mx2 = cos * aMXPtr[M02] + sin * aMXPtr[M00];
		aMXPtr[M00] = mx1;
		aMXPtr[M02] = mx2;

		mx1 = cos * aMXPtr[M10] - sin * aMXPtr[M12];
		mx2 = cos * aMXPtr[M12] + sin * aMXPtr[M10];
		aMXPtr[M10] = mx1;
		aMXPtr[M12] = mx2;

		mx1 = cos * aMXPtr[M20] - sin * aMXPtr[M22];
		mx2 = cos * aMXPtr[M22] + sin * aMXPtr[M20];
		aMXPtr[M20] = mx1;
		aMXPtr[M22] = mx2;
	}
}

void aRotZ(short angle)
{
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * aMXPtr[M00] + sin * aMXPtr[M01];
		mx2 = cos * aMXPtr[M01] - sin * aMXPtr[M00];
		aMXPtr[M00] = mx1;
		aMXPtr[M01] = mx2;

		mx1 = cos * aMXPtr[M10] + sin * aMXPtr[M11];
		mx2 = cos * aMXPtr[M11] - sin * aMXPtr[M10];
		aMXPtr[M10] = mx1;
		aMXPtr[M11] = mx2;

		mx1= cos * aMXPtr[M20] + sin * aMXPtr[M21];
		mx2 = cos * aMXPtr[M21] - sin * aMXPtr[M20];;
		aMXPtr[M20] = mx1;
		aMXPtr[M21] = mx2;
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

void phd_PushMatrix()
{
	phd_mxptr[indices_count + M00] = phd_mxptr[M00];
	phd_mxptr[indices_count + M01] = phd_mxptr[M01];
	phd_mxptr[indices_count + M02] = phd_mxptr[M02];
	phd_mxptr[indices_count + M03] = phd_mxptr[M03];
	phd_mxptr[indices_count + M10] = phd_mxptr[M10];
	phd_mxptr[indices_count + M11] = phd_mxptr[M11];
	phd_mxptr[indices_count + M12] = phd_mxptr[M12];
	phd_mxptr[indices_count + M13] = phd_mxptr[M13];
	phd_mxptr[indices_count + M20] = phd_mxptr[M20];
	phd_mxptr[indices_count + M21] = phd_mxptr[M21];
	phd_mxptr[indices_count + M22] = phd_mxptr[M22];
	phd_mxptr[indices_count + M23] = phd_mxptr[M23];
	phd_mxptr += 12;

	aMXPtr[indices_count + M00] = aMXPtr[M00];
	aMXPtr[indices_count + M01] = aMXPtr[M01];
	aMXPtr[indices_count + M02] = aMXPtr[M02];
	aMXPtr[indices_count + M03] = aMXPtr[M03];
	aMXPtr[indices_count + M10] = aMXPtr[M10];
	aMXPtr[indices_count + M11] = aMXPtr[M11];
	aMXPtr[indices_count + M12] = aMXPtr[M12];
	aMXPtr[indices_count + M13] = aMXPtr[M13];
	aMXPtr[indices_count + M20] = aMXPtr[M20];
	aMXPtr[indices_count + M21] = aMXPtr[M21];
	aMXPtr[indices_count + M22] = aMXPtr[M22];
	aMXPtr[indices_count + M23] = aMXPtr[M23];
	aMXPtr += 12;
}

void phd_SetTrans(long x, long y, long z)
{
	phd_mxptr[M03] = x << 14;
	phd_mxptr[M13] = y << 14;
	phd_mxptr[M23] = z << 14;

#ifdef GENERAL_FIXES // Fixes wrong translation
	aSetTrans(x, y, z);
#else
	aSetTrans(x << 14, y << 14, z << 14);
#endif
}

void phd_PushUnitMatrix()
{
	aMXPtr += 12;
	phd_mxptr += 12;
	phd_mxptr[M00] = 16384;
	phd_mxptr[M01] = 0;
	phd_mxptr[M02] = 0;
	phd_mxptr[M03] = 0;
	phd_mxptr[M10] = 0;
	phd_mxptr[M11] = 16384;
	phd_mxptr[M12] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M20] = 0;
	phd_mxptr[M21] = 0;
	phd_mxptr[M22] = 16384;
	phd_mxptr[M23] = 0;
	aMXPtr[M00] = 1;
	aMXPtr[M01] = 0;
	aMXPtr[M02] = 0;
	aMXPtr[M03] = 0;
	aMXPtr[M10] = 0;
	aMXPtr[M11] = 1;
	aMXPtr[M12] = 0;
	aMXPtr[M13] = 0;
	aMXPtr[M20] = 0;
	aMXPtr[M21] = 0;
	aMXPtr[M22] = 1;
	aMXPtr[M23] = 0;
}

long phd_TranslateRel(long x, long y, long z)
{
	phd_mxptr[M03] += x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02];
	phd_mxptr[M13] += x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12];
	phd_mxptr[M23] += x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22];
	aTranslateRel(x, y, z);
	return 1;
}

void phd_RotX(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	aRotX(angle);
}

void phd_RotY(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	aRotY(angle);
}

void phd_RotZ(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}

	aRotZ(angle);
}

void phd_RotYXZpack(long angles)//angles is XYZ, not YXZ as the name suggests, ty core
{
	long sin, cos, mx1, mx2;
	short angle;

	aRotYXZPack(angles);
	angle = (angles >> 10) & 0x3FF;//second ten bits, Y
	angle <<= 6;//* 64

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	angle = (angles >> 20) & 0x3FF;//firrst ten bits, X
	angle <<= 6;//* 64

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	angle = angles & 0x3FF;//last ten, Z
	angle <<= 6;//* 64

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}
}

void phd_RotYXZ(short y, short x, short z)
{
	long sin, cos, mx1, mx2;

	aRotYXZ(y, x, z);

	if (y)
	{
		sin = phd_sin(y);
		cos = phd_cos(y);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	if (x)
	{
		sin = phd_sin(x);
		cos = phd_cos(x);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	if (z)
	{
		sin = phd_sin(z);
		cos = phd_cos(z);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}
}

void phd_TranslateAbs(long x, long y, long z)
{
	long fx, fy, fz;

	aTranslateAbs(x, y, z);
	fx = x - w2v_matrix[M03];
	fy = y - w2v_matrix[M13];
	fz = z - w2v_matrix[M23];
	phd_mxptr[M03] = fx * phd_mxptr[M00] + fy * phd_mxptr[M01] + fz * phd_mxptr[M02];
	phd_mxptr[M13] = fx * phd_mxptr[M10] + fy * phd_mxptr[M11] + fz * phd_mxptr[M12];
	phd_mxptr[M23] = fx * phd_mxptr[M20] + fy * phd_mxptr[M21] + fz * phd_mxptr[M22];
}

void phd_GetVectorAngles(long x, long y, long z, short* angles)
{
	short atan;

	angles[0] = (short)phd_atan(z, x);

	while ((short)x != x || (short)y != y || (short)z != z)
	{
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}

	atan = (short)phd_atan(phd_sqrt(SQUARE(z) + SQUARE(x)), y);

	if ((y > 0 && atan > 0) || (y < 0 && atan < 0))
		atan = -atan;

	angles[1] = atan;
}

void phd_TransposeMatrix()
{
	long bak;

	bak = phd_mxptr[M01];
	phd_mxptr[M01] = phd_mxptr[M10];
	phd_mxptr[M10] = bak;

	bak = phd_mxptr[M12];
	phd_mxptr[M12] = phd_mxptr[M21];
	phd_mxptr[M21] = bak;

	bak = phd_mxptr[M20];
	phd_mxptr[M20] = phd_mxptr[M21];
	phd_mxptr[M21] = bak;
}

void phd_LookAt(long xsrc, long ysrc, long zsrc, long xtar, long ytar, long ztar, short roll)
{
	PHD_3DPOS viewPos;
	long dx, dy, dz;
	short angles[2];

	phd_GetVectorAngles(xtar - xsrc, ytar - ysrc, ztar - zsrc, angles);
	viewPos.x_pos = xsrc;
	viewPos.y_pos = ysrc;
	viewPos.z_pos = zsrc;
	viewPos.x_rot = angles[1];
	viewPos.y_rot = angles[0];
	viewPos.z_rot = roll;
	dx = xsrc - xtar;
	dy = ysrc - ytar;
	dz = zsrc - ztar;
#ifdef GENERAL_FIXES
	CamRot.vx = (mGetAngle(0, 0, (long)phd_sqrt(SQUARE(dx) + SQUARE(dz)), dy) >> 4) & 0xFFF;
#else
	CamRot.vx = (mGetAngle(0, 0, (long)sqrt(SQUARE(dx) + SQUARE(dz)), dy) >> 4) & 0xFFF;
#endif
	CamRot.vy = (mGetAngle(zsrc, xsrc, ztar, xtar) >> 4) & 0xFFF;
	CamRot.vz = 0;
	CamPos.x = xsrc;
	CamPos.y = ysrc;
	CamPos.z = zsrc;
	phd_GenerateW2V(&viewPos);
	S_InitD3DMatrix();
#ifdef GENERAL_FIXES
	aLookAt((float)xsrc, (float)ysrc, (float)zsrc, (float)xtar, (float)ytar, (float)ztar, roll);
#endif
}

void aLookAt(float xsrc, float ysrc, float zsrc, float xtar, float ytar, float ztar, long roll)
{
	D3DMATRIX mx;

	aCamera.pos.x = xsrc;
	aCamera.pos.y = ysrc;
	aCamera.pos.z = zsrc;
	aMXPtr = aFMatrixStack;
	aCamera.tar.x = xtar;
	aCamera.tar.y = ytar;
	aCamera.tar.z = ztar;
#ifdef GENERAL_FIXES
	aCamera.matrix[M00] = (float)phd_mxptr[M00] / 16384;
	aCamera.matrix[M01] = (float)phd_mxptr[M01] / 16384;
	aCamera.matrix[M02] = (float)phd_mxptr[M02] / 16384;
	aCamera.matrix[M10] = (float)phd_mxptr[M10] / 16384;
	aCamera.matrix[M11] = (float)phd_mxptr[M11] / 16384;
	aCamera.matrix[M12] = (float)phd_mxptr[M12] / 16384;
	aCamera.matrix[M20] = (float)phd_mxptr[M20] / 16384;
	aCamera.matrix[M21] = (float)phd_mxptr[M21] / 16384;
	aCamera.matrix[M22] = (float)phd_mxptr[M22] / 16384;
#else
	aPointCamera(&aCamera);
#endif
	aCamera.matrix[M03] = xsrc;
	aCamera.matrix[M13] = ysrc;
	aCamera.matrix[M23] = zsrc;
	aMXPtr[M00] = aCamera.matrix[M00];
	aMXPtr[M01] = aCamera.matrix[M01];
	aMXPtr[M02] = aCamera.matrix[M02];
	aMXPtr[M03] = aCamera.matrix[M03];
	aMXPtr[M10] = aCamera.matrix[M10];
	aMXPtr[M11] = aCamera.matrix[M11];
	aMXPtr[M12] = aCamera.matrix[M12];
	aMXPtr[M13] = aCamera.matrix[M13];
	aMXPtr[M20] = aCamera.matrix[M20];
	aMXPtr[M21] = aCamera.matrix[M21];
	aMXPtr[M22] = aCamera.matrix[M22];
	aMXPtr[M23] = aCamera.matrix[M23];

#ifndef GENERAL_FIXES
	if (roll)
	{
		aRotZ((short)roll);
		aCamera.matrix[M00] = aMXPtr[M00];
		aCamera.matrix[M01] = aMXPtr[M01];
		aCamera.matrix[M02] = aMXPtr[M02];
		aCamera.matrix[M10] = aMXPtr[M10];
		aCamera.matrix[M11] = aMXPtr[M11];
		aCamera.matrix[M12] = aMXPtr[M12];
		aCamera.matrix[M20] = aMXPtr[M20];
		aCamera.matrix[M21] = aMXPtr[M21];
		aCamera.matrix[M22] = aMXPtr[M22];
	}

	aCamera.matrix[M10] *= LfAspectCorrection;
	aCamera.matrix[M11] *= LfAspectCorrection;
	aCamera.matrix[M12] *= LfAspectCorrection;
#endif
	SetD3DMatrixF(&mx, aCamera.matrix);
	D3DInvCameraMatrix._11 = mx._11;
	D3DInvCameraMatrix._12 = mx._21;
	D3DInvCameraMatrix._13 = mx._31;
	D3DInvCameraMatrix._14 = mx._41;
	D3DInvCameraMatrix._21 = mx._12;
	D3DInvCameraMatrix._22 = mx._22;
	D3DInvCameraMatrix._23 = mx._32;
	D3DInvCameraMatrix._24 = mx._42;
	D3DInvCameraMatrix._31 = mx._13;
	D3DInvCameraMatrix._32 = mx._23;
	D3DInvCameraMatrix._33 = mx._33;
	D3DInvCameraMatrix._34 = mx._43;
	D3DInvCameraMatrix._41 = mx._14;
	D3DInvCameraMatrix._42 = mx._24;
	D3DInvCameraMatrix._43 = mx._34;
	D3DInvCameraMatrix._44 = mx._44;
}

void aOuterProduct(FVECTOR* v1, FVECTOR* v2, FVECTOR* dest)
{
	dest->x = v1->y * v2->z - v1->z * v2->y;
	dest->y = v1->z * v2->x - v1->x * v2->z;
	dest->z = v1->x * v2->y - v1->y * v2->x;
}

void aVectorNormal(FVECTOR* v, FVECTOR* a)
{
	float m;

	m = sqrt(SQUARE(v->x) + SQUARE(v->y) + SQUARE(v->z));
	v->x = 1.0F / m * v->x;
	v->y = 1.0F / m * v->y;
	v->z = 1.0F / m * v->z;
}

void aPerpVectors(FVECTOR* a, FVECTOR* b, FVECTOR* c)
{
	FVECTOR vA, vB, vC;

	vA.x = a->x;
	vA.y = a->y;
	vA.z = a->z;
	vC.x = c->x;
	vC.y = c->y;
	vC.z = c->z;
	aVectorNormal(&vA, &vA);

	if (!vA.x && !vA.y)
	{
		vB.x = b->x;
		vB.y = b->y;
		vB.z = b->z;
	}
	else
		aOuterProduct(&vC, &vA, &vB);

	aOuterProduct(&vA, &vB, &vC);
	aVectorNormal(&vB, &vB);
	aVectorNormal(&vC, &vC);

	a->x = vA.x;
	a->y = vA.y;
	a->z = vA.z;
	b->x = vB.x;
	b->y = vB.y;
	b->z = vB.z;
	c->x = vC.x;
	c->y = vC.y;
	c->z = vC.z;
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
	INJECT(0x0048F9C0, phd_PushMatrix, replace);
	INJECT(0x0048FA40, phd_SetTrans, replace);
	INJECT(0x0048FA90, phd_PushUnitMatrix, replace);
	INJECT(0x0048FB20, phd_TranslateRel, replace);
	INJECT(0x0048FBE0, phd_RotX, replace);
	INJECT(0x0048FCD0, phd_RotY, replace);
	INJECT(0x0048FDC0, phd_RotZ, replace);
	INJECT(0x0048FEB0, phd_RotYXZpack, replace);
	INJECT(0x00490150, phd_RotYXZ, replace);
	INJECT(0x004903F0, phd_TranslateAbs, replace);
	INJECT(0x004904B0, phd_GetVectorAngles, replace);
	INJECT(0x00490550, phd_TransposeMatrix, replace);
	INJECT(0x0048F760, phd_LookAt, replace);
	INJECT(0x00490C20, aLookAt, replace);
	INJECT(0x00491120, aOuterProduct, replace);
	INJECT(0x004910D0, aVectorNormal, replace);
	INJECT(0x00491170, aPerpVectors, replace);
}
