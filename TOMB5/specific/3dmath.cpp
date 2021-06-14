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
	phd_persp = ((phd_winwidth / 2) * phd_cos(fov)) / phd_sin(fov);
	f_persp = float(phd_persp);
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;
	LfAspectCorrection = (4.0f / 3.0f) / (float(phd_winwidth) / float(phd_winheight));
	f_mpersp = f_persp;
	f_moneopersp = mone / f_persp;
	f_mperspoznear = f_persp / f_mznear;
}

void gte_sttr(PHD_VECTOR* vec)
{
	vec->x = phd_mxptr[3] >> 14;
	vec->y = phd_mxptr[7] >> 14;
	vec->z = phd_mxptr[11] >> 14;
}

void inject_3dmath()
{
	INJECT(0x0048EDC0, AlterFOV);
	INJECT(0x00491320, gte_sttr);
}
