#include "../tomb5/pch.h"
#include "sphere.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "objects.h"
#include "lara.h"

SPHERE Slist[34];
char GotLaraSpheres;
static SPHERE LaraSpheres[15];
static long NumLaraSpheres;

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

void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint)
{
	OBJECT_INFO* obj;
	float* fmx;
	float* fimx;
	long* mx;
	long* imx;
	long* bone;
	short* frm[2];
	short* extra_rotation;
	short* rot;
	short* rot2;
	long frac, rate, poppush;

	mx = phd_mxptr;
	imx = IMptr;
	fmx = aMXPtr;
	fimx = aIMXPtr;
	obj = &objects[item->object_number];
	frac = GetFrames(item, frm, &rate);

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	extra_rotation = (short*)item->data;

	if (!extra_rotation)
		extra_rotation = no_rotation;

	bone = &bones[obj->bone_index];

	if (frac)
	{
		InitInterpolate2(frac, rate);
		rot = frm[0] + 9;
		rot2 = frm[1] + 9;
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & 1)
				phd_PopMatrix_I();

			if (poppush & 2)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (poppush & 0x1C)
			{
				if (poppush & 8)
					phd_RotY_I(*extra_rotation++);

				if (poppush & 4)
					phd_RotX_I(*extra_rotation++);

				if (poppush & 0x10)
					phd_RotZ_I(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		InterpolateMatrix();
	}
	else
	{
		phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
		rot = frm[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & 1)
				phd_PopMatrix();

			if (poppush & 2)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot, 0);

			if (poppush & 0x1C)
			{
				if (poppush & 8)
					phd_RotY(*extra_rotation++);

				if (poppush & 4)
					phd_RotX(*extra_rotation++);

				if (poppush & 0x10)
					phd_RotZ(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel(pos->x, pos->y, pos->z);
	}

	pos->x = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
	pos->y = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
	pos->z = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
	phd_mxptr = mx;
	IMptr = imx;
	aMXPtr = fmx;
	aIMXPtr = fimx;
}

long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* meshp;
	short* frame;
	short* rot;
	short* extra_rot;
	long x, y, z, poppush;

	if (!item)
		return 0;

	if (WorldSpace & 1)
	{
		x = item->pos.x_pos;
		y = item->pos.y_pos;
		z = item->pos.z_pos;
		phd_PushUnitMatrix();
		phd_mxptr[M03] = 0;
		phd_mxptr[M13] = 0;
		phd_mxptr[M23] = 0;
	}
	else
	{
		x = 0;
		y = 0;
		z = 0;
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	}

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	frame = GetBestFrame(item);
	phd_TranslateRel(frame[6], frame[7], frame[8]);

	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);

	obj = &objects[item->object_number];
	meshpp = &meshes[obj->mesh_index];
	meshp = *meshpp;
	meshpp += 2;
	bone = &bones[obj->bone_index];

	phd_PushMatrix();

	if (!(WorldSpace & 2))
		phd_TranslateRel(meshp[0], meshp[1], meshp[2]);

	ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
	ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
	ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
	ptr->r = meshp[3];
	ptr++;
	phd_PopMatrix();

	extra_rot = (short*)item->data;

	for (int i = 0; i < obj->nmeshes - 1; i++)
	{
		poppush = *bone++;

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[0], bone[1], bone[2]);
		gar_RotYXZsuperpack(&rot, 0);

		if (poppush & 0x1C && extra_rot)
		{
			if (poppush & 8)
				phd_RotY(*extra_rot++);

			if (poppush & 4)
				phd_RotX(*extra_rot++);

			if (poppush & 0x10)
				phd_RotZ(*extra_rot++);
		}

		meshp = *meshpp;
		phd_PushMatrix();

		if (!(WorldSpace & 2))
			phd_TranslateRel(meshp[0], meshp[1], meshp[2]);

		ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
		ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
		ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
		ptr->r = meshp[3];
		ptr++;
		phd_PopMatrix();

		bone += 3;
		meshpp += 2;
	}

	phd_PopMatrix();
	return obj->nmeshes;
}

long TestCollision(ITEM_INFO* item, ITEM_INFO* l)
{
	SPHERE* itemSpheres;
	SPHERE* laraSpheres;
	PHD_VECTOR ip;
	PHD_VECTOR lp;
	ulong touch_bits;
	long nItemSpheres, nLaraSpheres, ir, lr;

	touch_bits = 0;
	nItemSpheres = GetSpheres(item, Slist, 1);

	if (l != lara_item)
		GotLaraSpheres = 0;

	if (GotLaraSpheres)
		nLaraSpheres = NumLaraSpheres;
	else
	{
		nLaraSpheres = GetSpheres(l, LaraSpheres, 1);
		NumLaraSpheres = nLaraSpheres;

		if (l == lara_item)
			GotLaraSpheres = 1;
	}

	l->touch_bits = 0;

	for (int i = 0; i < nItemSpheres; i++)
	{
		itemSpheres = &Slist[i];
		ir = itemSpheres->r;

		if (ir > 0)
		{
			ip.x = itemSpheres->x;
			ip.y = itemSpheres->y;
			ip.z = itemSpheres->z;

			for (int j = 0; j < nLaraSpheres; j++)
			{
				laraSpheres = &LaraSpheres[j];
				lr = laraSpheres->r;

				if (lr > 0)
				{
					lp.x = laraSpheres->x - ip.x;
					lp.y = laraSpheres->y - ip.y;
					lp.z = laraSpheres->z - ip.z;
					lr += ir;

					if (SQUARE(lp.x) + SQUARE(lp.y) + SQUARE(lp.z) < SQUARE(lr))
					{
						l->touch_bits |= 1 << j;
						touch_bits |= 1 << i;
						break;
					}
				}
			}
		}
	}

	item->touch_bits = touch_bits;
	return touch_bits;
}
