#include "../tomb5/pch.h"
#include "dynamicshadows.h"
#include "../specific/3dmath.h"
#include "../game/draw.h"
#include "../game/control.h"
#include "../game/lara_states.h"
#include "../game/lara2gun.h"
#include "../game/gameflow.h"
#include "../specific/output.h"
#include "../game/objects.h"
#include "../specific/lighting.h"
#include "../specific/function_table.h"
#include "tomb5.h"

long shadow_matrices[180];
long shadow_joint_matrices[180];
float shadow_matricesF[180];
float shadow_joint_matricesF[180];

long	ShadowMeshSweetnessTable[15] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 14, 8, 9, 10, 11, 12, 13
};

char ShadowNodesToStashToScratch[14][2] =
{
	{1, 3},
	{4, 5},
	{6, 7},
	{2, 8},
	{9, 10},
	{11, 12},
	{0, 13},
	{14, 17},
	{18, 19},
	{20, 21},
	{15, 22},
	{23, 24},
	{25, 26},
	{16, 27}
};

char ShadowNodesToStashFromScratch[15][4] =
{
	{0, 1, 2, -1},
	{3, 4, -1, 0},
	{5, 6, -1, 0},
	{7, -1, 0, 0},
	{8, 9, -1, 0},
	{10, 11, -1, 0},
	{12, -1, 0, 0},
	{13, 16, 14, 15},
	{27, 28, 34, -1},
	{17, 18, -1, 0},
	{19, 20, -1, 0},
	{21, -1, 0, 0},
	{22, 23, -1, 0},
	{24, 25, -1, 0},
	{26, -1, 0, 0}
};

uchar ShadowUseMatrix[14][2] =
{
	{255, 255},
	{1, 2},
	{255, 255},
	{255, 255},
	{4, 5},
	{255, 255},
	{255, 255},
	{255, 255},
	{9, 10},
	{255, 255},
	{255, 255},
	{12, 13},
	{255, 255},
	{255, 255}
};

long ls_pitch;
long ls_yaw;

void CollectLights(ITEM_INFO* item)
{
	PCLIGHT* light;
	PHD_VECTOR best;
	long x, y, z, dist, bestdist;
	short angles[2];
	bool found;

	light = (PCLIGHT*)item->il.pCurrentLights;
	found = 0;
	best.x = 0;
	best.y = 0;
	best.z = 0;
	bestdist = 0x7FFFFFFF;

	for (int i = 0; i < item->il.nCurrentLights; i++)
	{
		if (!light[i].Active)
			continue;

		if (light[i].Type == LIGHT_SUN)	//if room has a sun, take it and exit
		{
			phd_GetVectorAngles(light[i].inx, light[i].iny, light[i].inz, angles);
			ls_pitch = angles[1];
			ls_yaw = angles[0];
			return;
		}

		if (light[i].Type == LIGHT_POINT || light[i].Type == LIGHT_SPOT)	//other lights?
		{
			x = item->pos.x_pos - light[i].ix;
			y = item->pos.y_pos - light[i].iy;
			z = item->pos.z_pos - light[i].iz;
			dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

			if (dist < bestdist)	//find the closest
			{
				bestdist = dist;
				best.x = x;
				best.y = y;
				best.z = z;
				found = 1;
			}
		}
	}

	if (found)	//found a close light?
	{
		phd_GetVectorAngles(best.x, best.y, best.z, angles);
		ls_pitch = angles[1];
		ls_yaw = angles[0];
	}
	else
	{
		ls_pitch = item->pos.x_rot - 0x2000;	//no lights, do overhead
		ls_yaw = item->pos.y_rot + 0x8000;
	}
}

void ProjectLightToFloorMatrix(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long x, y, z, h, dy;
	long pitch, yaw, c, t, rs, rc;
	short rn;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	rn = item->room_number;
	floor = GetFloor(x, y, z, &rn);
	h = GetHeight(floor, x, y, z);
	dy = y - h;
	x -= (dy * phd_sin(ls_yaw)) >> 15;
	y = h - 1;
	z -= (dy * phd_cos(ls_yaw)) >> 15;
	phd_TranslateAbs(x, y, z);

	pitch = ls_pitch - 16384;
	yaw = ls_yaw - 32768;
	c = phd_cos(pitch);

	if (c)
	{
		if (1024 * phd_sin(pitch) / c < 0)
			t = -(1024 * phd_sin(pitch) / c);
		else
			t = 1024 * phd_sin(pitch) / c;

		if (t > 1024)
			t = 1024;
	}
	else
		t = 1024;

	rs = t * phd_sin(yaw);

	if (rs < 0)
		rs -= 1025;

	rs >>= 10;
	rc = t * phd_cos(yaw);

	if (rc < 0)
		rc -= 1025;

	rc >>= 10;
	phd_mxptr[M01] = (phd_mxptr[M02] * rc + phd_mxptr[M00] * rs) >> 14;
	phd_mxptr[M11] = (phd_mxptr[M12] * rc + phd_mxptr[M10] * rs) >> 14;
	phd_mxptr[M21] = (phd_mxptr[M22] * rc + phd_mxptr[M20] * rs) >> 14;

	aMXPtr[M00] = (float)phd_mxptr[M00] / 16384;
	aMXPtr[M01] = (float)phd_mxptr[M01] / 16384;
	aMXPtr[M02] = (float)phd_mxptr[M02] / 16384;
	aMXPtr[M10] = (float)phd_mxptr[M10] / 16384;
	aMXPtr[M11] = (float)phd_mxptr[M11] / 16384;
	aMXPtr[M12] = (float)phd_mxptr[M12] / 16384;
	aMXPtr[M20] = (float)phd_mxptr[M20] / 16384;
	aMXPtr[M21] = (float)phd_mxptr[M21] / 16384;
	aMXPtr[M22] = (float)phd_mxptr[M22] / 16384;
}

void CalcShadowMatrices_Normal(short* frame, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* matricesF;
	float* Fmatrix;
	long* matrices;
	long* matrix;
	short* rot;
	short* rot2;
	short gun;

	if (flag == 1)
	{
		matrices = shadow_joint_matrices;
		matricesF = shadow_joint_matricesF;
	}
	else
	{
		matrices = shadow_matrices;
		matricesF = shadow_matricesF;
	}

	matrix = (long*)matrices;
	Fmatrix = (float*)matricesF;
	phd_PushMatrix();

	if (!flag || flag == 2)
		ProjectLightToFloorMatrix(lara_item);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.z = -16384;
		vec.y = -16384;
		vec.x = -16384;
		ScaleCurrentMatrix(&vec);
	}

	phd_PushMatrix();
	rot = &frame[9];
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);

#ifdef GENERAL_FIXES//rotate her head and torso when she's HK'ing
	if (lara.weapon_item != NO_ITEM && lara.gun_type == WEAPON_HK &&
		(items[lara.weapon_item].current_anim_state == 0 || items[lara.weapon_item].current_anim_state == 1 ||
			items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 3 ||
			items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 7);
	}
	else
#endif
		gar_RotYXZsuperpack(&rot, 0);

	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	rot2 = rot;
	gar_RotYXZsuperpack(&rot2, 6);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);//this must be left arm I guess

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];

			gar_RotYXZsuperpack(&rot, 11);
		}
		else
			gar_RotYXZsuperpack(&rot, 0);

		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];//going back 2 matrices
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_HK:
	case WEAPON_CROSSBOW:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number *
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
	GLaraShadowframe = frame;
}

void CalcShadowMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* matricesF;
	float* Fmatrix;
	float* armsF;
	long* matrices;
	long* matrix;
	long* arms;
	short* rot;
	short* rot2;
	short* rotcopy;
	short* rot2copy;
	short gun;

	if (flag == 1)
	{
		matrices = shadow_joint_matrices;
		matricesF = shadow_joint_matricesF;
	}
	else
	{
		matrices = shadow_matrices;
		matricesF = shadow_matricesF;
	}

	matrix = (long*)matrices;
	Fmatrix = (float*)matricesF;
	phd_PushMatrix();
	arms = phd_mxptr;
	armsF = aMXPtr;

	if (!flag || flag == 2)
		ProjectLightToFloorMatrix(lara_item);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.z = -16384;
		vec.y = -16384;
		vec.x = -16384;
		ScaleCurrentMatrix(&vec);
	}

	phd_PushMatrix();
	rot = frame1 + 9;
	rot2 = frame2 + 9;
	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);

#ifdef GENERAL_FIXES//rotate her head and torso when she's HK'ing
	if (lara.weapon_item != NO_ITEM && lara.gun_type == WEAPON_HK &&
		(items[lara.weapon_item].current_anim_state == 0 || items[lara.weapon_item].current_anim_state == 1 ||
			items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 3 ||
			items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot2 = rot;
		gar_RotYXZsuperpack_I(&rot, &rot2, 7);
	}
	else
#endif
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	rotcopy = rot;
	rot2copy = rot2;
	gar_RotYXZsuperpack_I(&rotcopy, &rot2copy, 6);
	phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(matrix, phd_mxptr, 48);
	memcpy(Fmatrix, aMXPtr, 48);
	matrix += 12;
	Fmatrix += 12;
	phd_PopMatrix();

	phd_PopMatrix_I();
	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot2 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			rot = rot2;
			gar_RotYXZsuperpack_I(&rot, &rot2, 11);
		}
		else
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		InterpolateArmMatrix(arms);
		aInterpolateArmMatrix(armsF);
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix(arms);
		aInterpolateArmMatrix(armsF);
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		InterpolateArmMatrix(arms);
		aInterpolateArmMatrix(armsF);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix(arms);
		aInterpolateArmMatrix(armsF);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_HK:
	case WEAPON_CROSSBOW:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		rot2 = &lara.right_arm.frame_base[lara.right_arm.frame_number *
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot = rot2;
		gar_RotYXZsuperpack_I(&rot, &rot2, 8);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(matrix, phd_mxptr, 48);
		memcpy(Fmatrix, aMXPtr, 48);
		matrix += 12;
		Fmatrix += 12;
		phd_PopMatrix();
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}

void CalcShadowMatrices(long flag)
{
	long* bone;
	short* frame;
	short* frmptr[2];
	long rate, frac;
	short jerk;

	CollectLights(lara_item);
	bone = &bones[objects[lara_item->object_number].bone_index];
	frac = GetFrames(lara_item, frmptr, &rate);

	if (lara.hit_direction < 0)
	{
		if (frac)
		{
			CalcShadowMatrices_Interpolated(frmptr[0], frmptr[1], frac, rate, bone, flag);
			return;
		}
	}

	if (lara.hit_direction < 0)
		frame = *frmptr;
	else
	{
		if (lara.hit_direction == 0)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKF : ANIM_JERK_FORWARD;
		else if (lara.hit_direction == 1)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKR : ANIM_JERK_RIGHT;
		else if (lara.hit_direction == 2)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKB : ANIM_JERK_BACK;
		else
			jerk = lara.IsDucked ? ANIM_JERK_DUCKL : ANIM_JERK_LEFT;

		frame = &anims[jerk].frame_ptr[lara.hit_frame * (anims[jerk].interpolation >> 8)];
	}

	CalcShadowMatrices_Normal(frame, bone, flag);
}

void TransformShadowMesh(MESH_DATA* mesh)
{
	FVECTOR vec;
	short* clip;
	float zv;
	short c;

	clip = clipflags;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		vec.x = mesh->aVtx[i].x * D3DMView._11 + mesh->aVtx[i].y * D3DMView._21 + mesh->aVtx[i].z * D3DMView._31 + D3DMView._41;
		vec.y = mesh->aVtx[i].x * D3DMView._12 + mesh->aVtx[i].y * D3DMView._22 + mesh->aVtx[i].z * D3DMView._32 + D3DMView._42;
		vec.z = mesh->aVtx[i].x * D3DMView._13 + mesh->aVtx[i].y * D3DMView._23 + mesh->aVtx[i].z * D3DMView._33 + D3DMView._43;
		aVertexBuffer[i].tu = vec.x;
		aVertexBuffer[i].tv = vec.y;
		c = 0;

		if (vec.z < f_mznear)
			c = -128;
		else
		{
			zv = f_mpersp / vec.z;
			vec.x = vec.x * zv + f_centerx;
			vec.y = vec.y * zv + f_centery;
			aVertexBuffer[i].rhw = f_moneopersp * zv;

			if (vec.x < f_left)
				c = 1;
			else if (vec.x > f_right)
				c = 2;

			if (vec.y < f_top)
				c += 4;
			else if (vec.y > f_bottom)
				c += 8;
		}

		*clip++ = c;
		aVertexBuffer[i].sx = vec.x;
		aVertexBuffer[i].sy = vec.y;
		aVertexBuffer[i].sz = vec.z;
		aVertexBuffer[i].color = 0x28000000;
		aVertexBuffer[i].specular = 0xFF000000;
	}
}

void phd_PutPolygons_ShadowMesh(short* objptr, long clipstatus)
{
	MESH_DATA* mesh;
	TEXTURESTRUCT tex;
	short* quad;
	short* tri;

	aSetViewMatrix();
	mesh = (MESH_DATA*)objptr;

	if (!objptr)
		return;

	TransformShadowMesh(mesh);

	tex.flag = 0;
	tex.tpage = 0;
	tex.drawtype = 3;
	tex.u1 = 0;
	tex.v1 = 0;
	tex.u2 = 0;
	tex.v2 = 0;
	tex.u3 = 0;
	tex.v3 = 0;
	tex.u4 = 0;
	tex.v4 = 0;

	quad = mesh->gt4;

	for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], &tex, 1);

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		AddTriSorted(aVertexBuffer, tri[0], tri[1], tri[2], &tex, 1);
}

void DrawDynamicShadow()
{
	OBJECT_INFO* obj;
	VECTOR v0;
	VECTOR v1;
	short** meshpp;
	short* rot;
	long cos, sin, xRot, bone, top, bottom, left, right, stash;

	if (lara.skelebob)
		return;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[lara_item->object_number];

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = shadow_matricesF[i * 12 + M00];
		aMXPtr[M01] = shadow_matricesF[i * 12 + M01];
		aMXPtr[M02] = shadow_matricesF[i * 12 + M02];
		aMXPtr[M03] = shadow_matricesF[i * 12 + M03];
		aMXPtr[M10] = shadow_matricesF[i * 12 + M10];
		aMXPtr[M11] = shadow_matricesF[i * 12 + M11];
		aMXPtr[M12] = shadow_matricesF[i * 12 + M12];
		aMXPtr[M13] = shadow_matricesF[i * 12 + M13];
		aMXPtr[M20] = shadow_matricesF[i * 12 + M20];
		aMXPtr[M21] = shadow_matricesF[i * 12 + M21];
		aMXPtr[M22] = shadow_matricesF[i * 12 + M22];
		aMXPtr[M23] = shadow_matricesF[i * 12 + M23];

		if (lara_item->mesh_bits >> 0x10 & 1 << (15 - i))
		{
			phd_PutPolygons_ShadowMesh(lara.mesh_ptrs[ShadowMeshSweetnessTable[i]], -1);

			for (int j = 0; j < 4; j++)
			{
				stash = (uchar)ShadowNodesToStashFromScratch[i][j];

				if (stash == 255)
					break;

				StashSkinVertices(stash);
			}
		}
	}

	phd_PopMatrix();

	if (LaraDrawType == LARA_DIVESUIT)
	{
		phd_top = top;
		phd_bottom = bottom;
		phd_left = left;
		phd_right = right;
		return;
	}

	phd_PushMatrix();
	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 14; i++)//joints
	{
		SkinVerticesToScratch(ShadowNodesToStashToScratch[i][0]);
		SkinVerticesToScratch(ShadowNodesToStashToScratch[i][1]);

		if (lara_item->mesh_bits & 1 << (14 - i))
		{
			if (ShadowUseMatrix[i][0] >= 255)
				phd_PutPolygons_ShadowMesh(*meshpp, -1);
			else
			{
				phd_PushMatrix();
				aMXPtr[M00] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M00];
				aMXPtr[M01] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M01];
				aMXPtr[M02] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M02];
				aMXPtr[M03] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M03];
				aMXPtr[M10] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M10];
				aMXPtr[M11] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M11];
				aMXPtr[M12] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M12];
				aMXPtr[M13] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M13];
				aMXPtr[M20] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M20];
				aMXPtr[M21] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M21];
				aMXPtr[M22] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M22];
				aMXPtr[M23] = shadow_matricesF[ShadowUseMatrix[i][1] * 12 + M23];
				v0.vx = shadow_matrices[12 * ShadowUseMatrix[i][0] + M01];
				v0.vy = shadow_matrices[12 * ShadowUseMatrix[i][0] + M11];
				v0.vz = shadow_matrices[12 * ShadowUseMatrix[i][0] + M21];
				v1.vx = shadow_matrices[12 * ShadowUseMatrix[i][1] + M01];
				v1.vy = shadow_matrices[12 * ShadowUseMatrix[i][1] + M11];
				v1.vz = shadow_matrices[12 * ShadowUseMatrix[i][1] + M21];

				cos = ((v0.vx * v1.vx) + (v0.vy * v1.vy) + (v0.vz * v1.vz)) >> 14;
				sin = phd_sqrt(16777216 - SQUARE(cos));

				if (i == 1 || i == 4)
					xRot = -phd_atan(cos, sin);
				else
					xRot = phd_atan(cos, sin);

				phd_RotX(short(-xRot >> 1));
				phd_PutPolygons_ShadowMesh(*meshpp, -1);
				phd_PopMatrix();
			}
		}

		meshpp += 2;
	}

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		if (LaraDrawType != 4)
		{
			obj = &objects[LHolster];
			meshpp = &meshes[obj->mesh_index];
			meshpp += 8;
			aMXPtr[M00] = shadow_matricesF[1 * 12 + M00];
			aMXPtr[M01] = shadow_matricesF[1 * 12 + M01];
			aMXPtr[M02] = shadow_matricesF[1 * 12 + M02];
			aMXPtr[M03] = shadow_matricesF[1 * 12 + M03];
			aMXPtr[M10] = shadow_matricesF[1 * 12 + M10];
			aMXPtr[M11] = shadow_matricesF[1 * 12 + M11];
			aMXPtr[M12] = shadow_matricesF[1 * 12 + M12];
			aMXPtr[M13] = shadow_matricesF[1 * 12 + M13];
			aMXPtr[M20] = shadow_matricesF[1 * 12 + M20];
			aMXPtr[M21] = shadow_matricesF[1 * 12 + M21];
			aMXPtr[M22] = shadow_matricesF[1 * 12 + M22];
			aMXPtr[M23] = shadow_matricesF[1 * 12 + M23];
			phd_PutPolygons_ShadowMesh(*meshpp, -1);

			obj = &objects[lara.holster];
			meshpp = &meshes[obj->mesh_index];
			meshpp += 16;
			aMXPtr[M00] = shadow_matricesF[4 * 12 + M00];
			aMXPtr[M01] = shadow_matricesF[4 * 12 + M01];
			aMXPtr[M02] = shadow_matricesF[4 * 12 + M02];
			aMXPtr[M03] = shadow_matricesF[4 * 12 + M03];
			aMXPtr[M10] = shadow_matricesF[4 * 12 + M10];
			aMXPtr[M11] = shadow_matricesF[4 * 12 + M11];
			aMXPtr[M12] = shadow_matricesF[4 * 12 + M12];
			aMXPtr[M13] = shadow_matricesF[4 * 12 + M13];
			aMXPtr[M20] = shadow_matricesF[4 * 12 + M20];
			aMXPtr[M21] = shadow_matricesF[4 * 12 + M21];
			aMXPtr[M22] = shadow_matricesF[4 * 12 + M22];
			aMXPtr[M23] = shadow_matricesF[4 * 12 + M23];
			phd_PutPolygons_ShadowMesh(*meshpp, -1);
		}

		if (lara.back_gun)
		{
			phd_PushMatrix();
			aMXPtr[M00] = shadow_matricesF[84 + M00];
			aMXPtr[M01] = shadow_matricesF[84 + M01];
			aMXPtr[M02] = shadow_matricesF[84 + M02];
			aMXPtr[M03] = shadow_matricesF[84 + M03];
			aMXPtr[M10] = shadow_matricesF[84 + M10];
			aMXPtr[M11] = shadow_matricesF[84 + M11];
			aMXPtr[M12] = shadow_matricesF[84 + M12];
			aMXPtr[M13] = shadow_matricesF[84 + M13];
			aMXPtr[M20] = shadow_matricesF[84 + M20];
			aMXPtr[M21] = shadow_matricesF[84 + M21];
			aMXPtr[M22] = shadow_matricesF[84 + M22];
			aMXPtr[M23] = shadow_matricesF[84 + M23];
			bone = objects[lara.back_gun].bone_index;
			phd_TranslateRel(bones[bone + 53], bones[bone + 54], bones[bone + 55]);
			rot = objects[lara.back_gun].frame_base + 9;
			gar_RotYXZsuperpack(&rot, 14);
			phd_PutPolygons_ShadowMesh(meshes[objects[lara.back_gun].mesh_index + 28], -1);
			phd_PopMatrix();
		}
	}

	phd_PopMatrix();
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}

void DrawAnimatingItemShadow(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* data;
	short* rot;
	short* rot2;
	long frac, rate, clip, bit;

	CollectLights(item);
	frac = GetFrames(item, frm, &rate);
	obj = &objects[item->object_number];

	phd_PushMatrix();
	ProjectLightToFloorMatrix(item);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	calc_animating_item_clip_window(item, frm[0]);
	clip = S_GetObjectBounds(frm[0]);

	if (clip)
	{
		if (!item->data)
			data = no_rotation;
		else
			data = (short*)item->data;

		bit = 1;
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];

		if (frac)
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
			rot = frm[0] + 9;
			rot2 = frm[1] + 9;
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (item->mesh_bits & 1)
			{
				phd_PushMatrix();
				InterpolateMatrix();
				
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons_ShadowMesh(meshpp[1], clip);
				else
					phd_PutPolygons_ShadowMesh(meshpp[0], clip);

				phd_PopMatrix();
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
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

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					phd_PushMatrix();
					InterpolateMatrix();

					if (bit & item->meshswap_meshbits)
						phd_PutPolygons_ShadowMesh(meshpp[1], clip);
					else
						phd_PutPolygons_ShadowMesh(meshpp[0], clip);

					phd_PopMatrix();
				}
			}
		}
		else
		{
			phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
			rot = frm[0] + 9;
			gar_RotYXZsuperpack(&rot, 0);

			if (item->mesh_bits & 1)
			{
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons_ShadowMesh(meshpp[1], clip);
				else
					phd_PutPolygons_ShadowMesh(meshpp[0], clip);
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
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

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					if (bit & item->meshswap_meshbits)
						phd_PutPolygons_ShadowMesh(meshpp[1], clip);
					else
						phd_PutPolygons_ShadowMesh(meshpp[0], clip);
				}
			}
		}
	}

	phd_right = phd_winwidth;
	phd_left = 0;
	phd_top = 0;
	phd_bottom = phd_winheight;
	phd_PopMatrix();
}

void DrawDynamicShadow(ITEM_INFO* item)
{
	if (item == lara_item)
		DrawDynamicShadow();
	else
		DrawAnimatingItemShadow(item);
}

bool DoIDynamicShadow(ITEM_INFO* item)
{
	short objnum;

	if (tomb5.shadow_mode != 5)
		return 0;

	objnum = item->object_number;

	if (objnum >= DOOR_TYPE1 && objnum <= SEQUENCE_DOOR1)
		return 0;

	if (objnum >= TRAPDOOR1 && objnum <= TRAPDOOR3)
		return 0;

	if (objnum >= BRIDGE_FLAT && objnum <= BRIDGE_TILT2_MIP)
		return 0;

	if (objnum >= WATERFALL1 && objnum <= WATERFALLSS2)
		return 0;

	if (objnum >= ANIMATING13 && objnum <= ANIMATING16_MIP)	//light rays and whatnot
		return 0;

	return 1;
}
