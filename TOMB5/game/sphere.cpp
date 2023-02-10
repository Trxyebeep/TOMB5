#include "../tomb5/pch.h"
#include "sphere.h"
#include "draw.h"
#include "../specific/3dmath.h"

void GetJointAbsPositionMatrix(ITEM_INFO* item, float* matrix, long node)
{
	float* aMXPtr_bak;
	float* aIMXPtr_bak;
	long* bone;
	long* phd_mxptr_bak;
	long* IMptr_bak;
	short* frmptr[2];
	short* data;
	short* rot;
	short* rot2;
	long frac, rate;

	phd_mxptr_bak = phd_mxptr;
	IMptr_bak = IMptr;
	aMXPtr_bak = aMXPtr;
	aIMXPtr_bak = aIMXPtr;
	frac = GetFrames(item, frmptr, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	if (!item->data)
		data = no_rotation;
	else
		data = (short*)item->data;

	bone = &bones[objects[item->object_number].bone_index];

	if (frac)
	{
		InitInterpolate2(frac, rate);
		rot = frmptr[0] + 9;
		rot2 = frmptr[1] + 9;
		phd_TranslateRel_ID(frmptr[0][6], frmptr[0][7], frmptr[0][8], frmptr[1][6], frmptr[1][7], frmptr[1][8]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		for (int i = 0; i < node; i++, bone += 4)
		{
			if (bone[0] & 1)
				phd_PopMatrix_I();

			if (bone[0] & 2)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (bone[0] & 0x1C)
			{
				if (bone[0] & 8)
					phd_RotY_I(*data++);

				if (bone[0] & 4)
					phd_RotX_I(*data++);

				if (bone[0] & 16)
					phd_RotZ_I(*data++);
			}
		}

		InterpolateMatrix();
		matrix[M00] = aMXPtr[M00];
		matrix[M01] = aMXPtr[M01];
		matrix[M02] = aMXPtr[M02];
		matrix[M03] = aMXPtr[M03];
		matrix[M10] = aMXPtr[M10];
		matrix[M11] = aMXPtr[M11];
		matrix[M12] = aMXPtr[M12];
		matrix[M13] = aMXPtr[M13];
		matrix[M20] = aMXPtr[M20];
		matrix[M21] = aMXPtr[M21];
		matrix[M22] = aMXPtr[M22];
		matrix[M23] = aMXPtr[M23];
	}
	else
	{
		phd_TranslateRel(frmptr[0][6], frmptr[0][7], frmptr[0][8]);
		rot = frmptr[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);

		for (int i = 0; i < node; i++, bone += 4)
		{
			if (bone[0] & 1)
				phd_PopMatrix();

			if (bone[0] & 2)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot, 0);

			if (bone[0] & 0x1C)
			{
				if (bone[0] & 8)
					phd_RotY(*data++);

				if (bone[0] & 4)
					phd_RotX(*data++);

				if (bone[0] & 16)
					phd_RotZ(*data++);
			}
		}

		matrix[M00] = aMXPtr[M00];
		matrix[M01] = aMXPtr[M01];
		matrix[M02] = aMXPtr[M02];
		matrix[M03] = aMXPtr[M03];
		matrix[M10] = aMXPtr[M10];
		matrix[M11] = aMXPtr[M11];
		matrix[M12] = aMXPtr[M12];
		matrix[M13] = aMXPtr[M13];
		matrix[M20] = aMXPtr[M20];
		matrix[M21] = aMXPtr[M21];
		matrix[M22] = aMXPtr[M22];
		matrix[M23] = aMXPtr[M23];
	}

	phd_mxptr = phd_mxptr_bak;
	IMptr = IMptr_bak;
	aMXPtr = aMXPtr_bak;
	aIMXPtr = aIMXPtr_bak;
}

void InitInterpolate2(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;

	IMptr = &IMstack[384];
	memcpy(&IMstack[384], phd_mxptr, 48u);

	aIMXPtr = &aIFMStack[384];
	memcpy(&aIFMStack[384], aMXPtr, 48u);
}

void inject_sphere(bool replace)
{
	INJECT(0x00479C20, GetJointAbsPositionMatrix, replace);
	INJECT(0x00479BB0, InitInterpolate2, replace);
}
