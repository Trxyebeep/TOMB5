#include "../tomb5/pch.h"
#include "3dmath.h"

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

void inject_3dmath(bool replace)
{
	INJECT(0x0048EDC0, AlterFOV, replace);
	INJECT(0x00491320, gte_sttr, replace);
	INJECT(0x00490590, aInitMatrix, replace);
}
