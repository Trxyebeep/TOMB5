#include "../tomb5/pch.h"
#include "drawlara.h"
#include "3dmath.h"
#include "specificfx.h"
#include "output.h"
#include "../game/draw.h"
#include "../game/hair.h"
#include "../game/objects.h"
#include "../game/gameflow.h"
#include "../game/control.h"
#include "../game/delstuff.h"
#include "lighting.h"
#include "input.h"
#include "../game/lara.h"
#include "../game/subsuit.h"
#include "../game/savegame.h"
#include "../game/lara2gun.h"
#include "../tomb5/tomb5.h"

static long lara_mesh_sweetness_table[15] =	//LM_ in LMX_ order for drawing
{
	LM_HIPS,
	LM_LTHIGH,
	LM_LSHIN,
	LM_LFOOT,
	LM_RTHIGH,
	LM_RSHIN,
	LM_RFOOT,
	LM_TORSO,
	LM_HEAD,
	LM_RINARM,
	LM_ROUTARM,
	LM_RHAND,
	LM_LINARM,
	LM_LOUTARM,
	LM_LHAND
};

static char lara_underwater_skin_sweetness_table[15] = { 0, 2, 3, 0, 5, 6, 7, 9, 10, 11, 12, 13, 14, 8, 0 };

static char NodesToStashToScratch[14][2] =
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

static char NodesToStashFromScratch[15][4] =
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

static uchar SkinUseMatrix[14][2] =
{
	{255, 255},
	{LMX_THIGH_L, LMX_CALF_L},
	{255, 255},
	{255, 255},
	{LMX_THIGH_R, LMX_CALF_R},
	{255, 255},
	{255, 255},
	{255, 255},
	{LMX_UARM_R, LMX_LARM_R},
	{255, 255},
	{255, 255},
	{LMX_UARM_L, LMX_LARM_L},
	{255, 255},
	{255, 255}
};

static void DrawLara__1(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	FVECTOR v0;
	FVECTOR v1;
	short** meshpp;
	short* rot;
	long c, s, xRot, bone, top, bottom, left, right, stash;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];
	S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);

	if (!mirror)
		CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matrices[i * indices_count + M00];
		aMXPtr[M01] = lara_matrices[i * indices_count + M01];
		aMXPtr[M02] = lara_matrices[i * indices_count + M02];
		aMXPtr[M03] = lara_matrices[i * indices_count + M03];
		aMXPtr[M10] = lara_matrices[i * indices_count + M10];
		aMXPtr[M11] = lara_matrices[i * indices_count + M11];
		aMXPtr[M12] = lara_matrices[i * indices_count + M12];
		aMXPtr[M13] = lara_matrices[i * indices_count + M13];
		aMXPtr[M20] = lara_matrices[i * indices_count + M20];
		aMXPtr[M21] = lara_matrices[i * indices_count + M21];
		aMXPtr[M22] = lara_matrices[i * indices_count + M22];
		aMXPtr[M23] = lara_matrices[i * indices_count + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		if (lara_item->mesh_bits >> 0x10 & 1 << (15 - i))
		{
			phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);

			for (int j = 0; j < 4; j++)
			{
				stash = (uchar)NodesToStashFromScratch[i][j];

				if (stash == 255)
					break;

				StashSkinVertices(stash);
			}
		}
	}

	phd_PopMatrix();
	bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
	DrawHair();
	phd_PushMatrix();
	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 14; i++)//joints
	{
		SkinVerticesToScratch(NodesToStashToScratch[i][0]);
		SkinVerticesToScratch(NodesToStashToScratch[i][1]);

		if (LaraNodeUnderwater[lara_underwater_skin_sweetness_table[i]])
			bLaraUnderWater = lara_underwater_skin_sweetness_table[i];
		else
			bLaraUnderWater = -1;

		if (lara_item->mesh_bits & 1 << (14 - i))
		{
			if (SkinUseMatrix[i][0] >= 255)
				phd_PutPolygons(*meshpp, -1);
			else
			{
				aMXPtr[M00] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M00];
				aMXPtr[M01] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
				aMXPtr[M02] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M02];
				aMXPtr[M03] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M03];
				aMXPtr[M10] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M10];
				aMXPtr[M11] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
				aMXPtr[M12] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M12];
				aMXPtr[M13] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M13];
				aMXPtr[M20] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M20];
				aMXPtr[M21] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];
				aMXPtr[M22] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M22];
				aMXPtr[M23] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M23];
				phd_PushMatrix();

				v0.x = lara_matrices[SkinUseMatrix[i][0] * indices_count + M01];
				v0.y = lara_matrices[SkinUseMatrix[i][0] * indices_count + M11];
				v0.z = lara_matrices[SkinUseMatrix[i][0] * indices_count + M21];
				v1.x = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
				v1.y = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
				v1.z = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];

				v0.x *= 1 << W2V_SHIFT;
				v0.y *= 1 << W2V_SHIFT;
				v0.z *= 1 << W2V_SHIFT;
				v1.x *= 1 << W2V_SHIFT;
				v1.y *= 1 << W2V_SHIFT;
				v1.z *= 1 << W2V_SHIFT;

				c = long((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> W2V_SHIFT;
				s = phd_sqrt(0x1000000 - SQUARE(c));

				if (i == 1 || i == 4)
					xRot = -phd_atan(c, s);
				else
					xRot = phd_atan(c, s);

				phd_RotX(short(-xRot >> 1));
				phd_PutPolygons(*meshpp, -1);
				phd_PopMatrix();
			}
		}

		meshpp += 2;
	}

	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		obj = &objects[LHolster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 8;
		aMXPtr[M00] = lara_matrices[LMX_THIGH_L * indices_count + M00];
		aMXPtr[M01] = lara_matrices[LMX_THIGH_L * indices_count + M01];
		aMXPtr[M02] = lara_matrices[LMX_THIGH_L * indices_count + M02];
		aMXPtr[M03] = lara_matrices[LMX_THIGH_L * indices_count + M03];
		aMXPtr[M10] = lara_matrices[LMX_THIGH_L * indices_count + M10];
		aMXPtr[M11] = lara_matrices[LMX_THIGH_L * indices_count + M11];
		aMXPtr[M12] = lara_matrices[LMX_THIGH_L * indices_count + M12];
		aMXPtr[M13] = lara_matrices[LMX_THIGH_L * indices_count + M13];
		aMXPtr[M20] = lara_matrices[LMX_THIGH_L * indices_count + M20];
		aMXPtr[M21] = lara_matrices[LMX_THIGH_L * indices_count + M21];
		aMXPtr[M22] = lara_matrices[LMX_THIGH_L * indices_count + M22];
		aMXPtr[M23] = lara_matrices[LMX_THIGH_L * indices_count + M23];
		phd_PutPolygons(*meshpp, -1);

		obj = &objects[lara.holster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 16;
		aMXPtr[M00] = lara_matrices[LMX_THIGH_R * indices_count + M00];
		aMXPtr[M01] = lara_matrices[LMX_THIGH_R * indices_count + M01];
		aMXPtr[M02] = lara_matrices[LMX_THIGH_R * indices_count + M02];
		aMXPtr[M03] = lara_matrices[LMX_THIGH_R * indices_count + M03];
		aMXPtr[M10] = lara_matrices[LMX_THIGH_R * indices_count + M10];
		aMXPtr[M11] = lara_matrices[LMX_THIGH_R * indices_count + M11];
		aMXPtr[M12] = lara_matrices[LMX_THIGH_R * indices_count + M12];
		aMXPtr[M13] = lara_matrices[LMX_THIGH_R * indices_count + M13];
		aMXPtr[M20] = lara_matrices[LMX_THIGH_R * indices_count + M20];
		aMXPtr[M21] = lara_matrices[LMX_THIGH_R * indices_count + M21];
		aMXPtr[M22] = lara_matrices[LMX_THIGH_R * indices_count + M22];
		aMXPtr[M23] = lara_matrices[LMX_THIGH_R * indices_count + M23];
		phd_PutPolygons(*meshpp, -1);

		if (lara.back_gun)//back gun
		{
			phd_PushMatrix();
			aMXPtr[M00] = lara_matrices[LMX_TORSO * indices_count + M00];
			aMXPtr[M01] = lara_matrices[LMX_TORSO * indices_count + M01];
			aMXPtr[M02] = lara_matrices[LMX_TORSO * indices_count + M02];
			aMXPtr[M03] = lara_matrices[LMX_TORSO * indices_count + M03];
			aMXPtr[M10] = lara_matrices[LMX_TORSO * indices_count + M10];
			aMXPtr[M11] = lara_matrices[LMX_TORSO * indices_count + M11];
			aMXPtr[M12] = lara_matrices[LMX_TORSO * indices_count + M12];
			aMXPtr[M13] = lara_matrices[LMX_TORSO * indices_count + M13];
			aMXPtr[M20] = lara_matrices[LMX_TORSO * indices_count + M20];
			aMXPtr[M21] = lara_matrices[LMX_TORSO * indices_count + M21];
			aMXPtr[M22] = lara_matrices[LMX_TORSO * indices_count + M22];
			aMXPtr[M23] = lara_matrices[LMX_TORSO * indices_count + M23];
			bone = objects[lara.back_gun].bone_index;
			phd_TranslateRel(bones[bone + 53], bones[bone + 54], bones[bone + 55]);
			rot = objects[lara.back_gun].frame_base + 9;
			gar_RotYXZsuperpack(&rot, 14);
			phd_PutPolygons(meshes[objects[lara.back_gun].mesh_index + 28], -1);
			phd_PopMatrix();
		}
	}

	phd_PopMatrix();

	bLaraUnderWater = 0;
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}

static void DrawLara__4(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	FVECTOR v0;
	FVECTOR v1;
	short** meshpp;
	short* rot;
	long c, s, xRot, bone, top, bottom, left, right, stash;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];
	S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);

	if (!mirror)
		CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matrices[i * indices_count + M00];
		aMXPtr[M01] = lara_matrices[i * indices_count + M01];
		aMXPtr[M02] = lara_matrices[i * indices_count + M02];
		aMXPtr[M03] = lara_matrices[i * indices_count + M03];
		aMXPtr[M10] = lara_matrices[i * indices_count + M10];
		aMXPtr[M11] = lara_matrices[i * indices_count + M11];
		aMXPtr[M12] = lara_matrices[i * indices_count + M12];
		aMXPtr[M13] = lara_matrices[i * indices_count + M13];
		aMXPtr[M20] = lara_matrices[i * indices_count + M20];
		aMXPtr[M21] = lara_matrices[i * indices_count + M21];
		aMXPtr[M22] = lara_matrices[i * indices_count + M22];
		aMXPtr[M23] = lara_matrices[i * indices_count + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		aSetViewMatrix();
		phd_PutPolygonsSpcEnvmap(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);

		for (int j = 0; j < 4; j++)
		{
			stash = (uchar)NodesToStashFromScratch[i][j];

			if (stash == 255)
				break;

			StashSkinVertices(stash);
			StashSkinNormals(stash);
		}
	}

	phd_PopMatrix();
	bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
	DrawHair();
	phd_PushMatrix();
	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 14; i++)//joints
	{
		SkinVerticesToScratch(NodesToStashToScratch[i][0]);
		SkinVerticesToScratch(NodesToStashToScratch[i][1]);
		SkinNormalsToScratch(NodesToStashToScratch[i][0]);
		SkinNormalsToScratch(NodesToStashToScratch[i][1]);

		if (LaraNodeUnderwater[lara_underwater_skin_sweetness_table[i]])
			bLaraUnderWater = lara_underwater_skin_sweetness_table[i];
		else
			bLaraUnderWater = -1;

		if (SkinUseMatrix[i][0] >= 255)
		{
			aSetViewMatrix();
			phd_PutPolygonsSpcEnvmap(*meshpp, -1);
		}
		else
		{
			phd_PushMatrix();
			aMXPtr[M00] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M00];
			aMXPtr[M01] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
			aMXPtr[M02] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M02];
			aMXPtr[M03] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M03];
			aMXPtr[M10] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M10];
			aMXPtr[M11] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
			aMXPtr[M12] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M12];
			aMXPtr[M13] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M13];
			aMXPtr[M20] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M20];
			aMXPtr[M21] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];
			aMXPtr[M22] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M22];
			aMXPtr[M23] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M23];

			v0.x = lara_matrices[SkinUseMatrix[i][0] * indices_count + M01];
			v0.y = lara_matrices[SkinUseMatrix[i][0] * indices_count + M11];
			v0.z = lara_matrices[SkinUseMatrix[i][0] * indices_count + M21];
			v1.x = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
			v1.y = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
			v1.z = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];

			v0.x *= 1 << W2V_SHIFT;
			v0.y *= 1 << W2V_SHIFT;
			v0.z *= 1 << W2V_SHIFT;
			v1.x *= 1 << W2V_SHIFT;
			v1.y *= 1 << W2V_SHIFT;
			v1.z *= 1 << W2V_SHIFT;

			c = long((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> W2V_SHIFT;
			s = phd_sqrt(0x1000000 - SQUARE(c));

			if (i == 1 || i == 4)
				xRot = -phd_atan(c, s);
			else
				xRot = phd_atan(c, s);

			phd_RotX(short(-xRot >> 1));
			aSetViewMatrix();
			phd_PutPolygonsSpcEnvmap(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	if (!(gfLevelFlags & GF_YOUNGLARA) && lara.back_gun)
	{
		phd_PushMatrix();
		aMXPtr[M00] = lara_matrices[LMX_TORSO * indices_count + M00];
		aMXPtr[M01] = lara_matrices[LMX_TORSO * indices_count + M01];
		aMXPtr[M02] = lara_matrices[LMX_TORSO * indices_count + M02];
		aMXPtr[M03] = lara_matrices[LMX_TORSO * indices_count + M03];
		aMXPtr[M10] = lara_matrices[LMX_TORSO * indices_count + M10];
		aMXPtr[M11] = lara_matrices[LMX_TORSO * indices_count + M11];
		aMXPtr[M12] = lara_matrices[LMX_TORSO * indices_count + M12];
		aMXPtr[M13] = lara_matrices[LMX_TORSO * indices_count + M13];
		aMXPtr[M20] = lara_matrices[LMX_TORSO * indices_count + M20];
		aMXPtr[M21] = lara_matrices[LMX_TORSO * indices_count + M21];
		aMXPtr[M22] = lara_matrices[LMX_TORSO * indices_count + M22];
		aMXPtr[M23] = lara_matrices[LMX_TORSO * indices_count + M23];
		bone = objects[lara.back_gun].bone_index;
		phd_TranslateRel(bones[bone + 53], bones[bone + 54], bones[bone + 55]);
		rot = objects[lara.back_gun].frame_base + 9;
		gar_RotYXZsuperpack(&rot, 14);
		aSetViewMatrix();
		phd_PutPolygons(meshes[objects[lara.back_gun].mesh_index + 28], -1);
		phd_PopMatrix();
	}

	phd_PopMatrix();

	bLaraUnderWater = 0;
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}

static void DrawLara__5(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long top, bottom, left, right;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;

	phd_PushMatrix();
	obj = &objects[item->object_number];
	S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);
	CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matrices[i * indices_count + M00];
		aMXPtr[M01] = lara_matrices[i * indices_count + M01];
		aMXPtr[M02] = lara_matrices[i * indices_count + M02];
		aMXPtr[M03] = lara_matrices[i * indices_count + M03];
		aMXPtr[M10] = lara_matrices[i * indices_count + M10];
		aMXPtr[M11] = lara_matrices[i * indices_count + M11];
		aMXPtr[M12] = lara_matrices[i * indices_count + M12];
		aMXPtr[M13] = lara_matrices[i * indices_count + M13];
		aMXPtr[M20] = lara_matrices[i * indices_count + M20];
		aMXPtr[M21] = lara_matrices[i * indices_count + M21];
		aMXPtr[M22] = lara_matrices[i * indices_count + M22];
		aMXPtr[M23] = lara_matrices[i * indices_count + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);
	}

	phd_PopMatrix();

	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	obj = &objects[LARA_EXTRA_MESH1];
	meshpp = &meshes[obj->mesh_index];

	phd_PushMatrix();
	aMXPtr[M00] = lara_matrices[LMX_TORSO * indices_count + M00];
	aMXPtr[M01] = lara_matrices[LMX_TORSO * indices_count + M01];
	aMXPtr[M02] = lara_matrices[LMX_TORSO * indices_count + M02];
	aMXPtr[M03] = lara_matrices[LMX_TORSO * indices_count + M03];
	aMXPtr[M10] = lara_matrices[LMX_TORSO * indices_count + M10];
	aMXPtr[M11] = lara_matrices[LMX_TORSO * indices_count + M11];
	aMXPtr[M12] = lara_matrices[LMX_TORSO * indices_count + M12];
	aMXPtr[M13] = lara_matrices[LMX_TORSO * indices_count + M13];
	aMXPtr[M20] = lara_matrices[LMX_TORSO * indices_count + M20];
	aMXPtr[M21] = lara_matrices[LMX_TORSO * indices_count + M21];
	aMXPtr[M22] = lara_matrices[LMX_TORSO * indices_count + M22];
	aMXPtr[M23] = lara_matrices[LMX_TORSO * indices_count + M23];
	phd_TranslateRel(-80, -192, -160);
	phd_RotX(subsuit.XRot);
	phd_PutPolygons(*meshpp, -1);
	phd_PopMatrix();

	phd_PushMatrix();
	aMXPtr[M00] = lara_matrices[LMX_TORSO * indices_count + M00];
	aMXPtr[M01] = lara_matrices[LMX_TORSO * indices_count + M01];
	aMXPtr[M02] = lara_matrices[LMX_TORSO * indices_count + M02];
	aMXPtr[M03] = lara_matrices[LMX_TORSO * indices_count + M03];
	aMXPtr[M10] = lara_matrices[LMX_TORSO * indices_count + M10];
	aMXPtr[M11] = lara_matrices[LMX_TORSO * indices_count + M11];
	aMXPtr[M12] = lara_matrices[LMX_TORSO * indices_count + M12];
	aMXPtr[M13] = lara_matrices[LMX_TORSO * indices_count + M13];
	aMXPtr[M20] = lara_matrices[LMX_TORSO * indices_count + M20];
	aMXPtr[M21] = lara_matrices[LMX_TORSO * indices_count + M21];
	aMXPtr[M22] = lara_matrices[LMX_TORSO * indices_count + M22];
	aMXPtr[M23] = lara_matrices[LMX_TORSO * indices_count + M23];
	phd_TranslateRel(80, -192, -160);
	phd_RotX(subsuit.XRot);
	phd_PutPolygons(*meshpp, -1);
	phd_PopMatrix();

	bLaraUnderWater = 0;
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}

static void DrawLara__6(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	FVECTOR v0;
	FVECTOR v1;
	short** meshpp;
	short* rot;
	long c, s, xRot, top, bottom, left, right, stash, bone;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];

	if (!mirror)
		CalculateObjectLightingLara();

	meshpp = &meshes[objects[LARA_EXTRA_MESH1].mesh_index];

	for (int i = 0; i < 15; i++)
	{
		aMXPtr[M00] = lara_matrices[i * indices_count + M00];
		aMXPtr[M01] = lara_matrices[i * indices_count + M01];
		aMXPtr[M02] = lara_matrices[i * indices_count + M02];
		aMXPtr[M03] = lara_matrices[i * indices_count + M03];
		aMXPtr[M10] = lara_matrices[i * indices_count + M10];
		aMXPtr[M11] = lara_matrices[i * indices_count + M11];
		aMXPtr[M12] = lara_matrices[i * indices_count + M12];
		aMXPtr[M13] = lara_matrices[i * indices_count + M13];
		aMXPtr[M20] = lara_matrices[i * indices_count + M20];
		aMXPtr[M21] = lara_matrices[i * indices_count + M21];
		aMXPtr[M22] = lara_matrices[i * indices_count + M22];
		aMXPtr[M23] = lara_matrices[i * indices_count + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		phd_PutPolygonsSpcXLU(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);

		for (int j = 0; j < 4; j++)
		{
			stash = (uchar)NodesToStashFromScratch[i][j];

			if (stash == 255)
				break;

			StashSkinVertices(stash);
		}

		phd_PutPolygons(meshpp[2 * lara_mesh_sweetness_table[i]], -1);
	}

	phd_PopMatrix();
	bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
	phd_PushMatrix();
	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 13; i++)
	{
		SkinVerticesToScratch(NodesToStashToScratch[i][0]);
		SkinVerticesToScratch(NodesToStashToScratch[i][1]);

		if (LaraNodeUnderwater[lara_underwater_skin_sweetness_table[i]])
			bLaraUnderWater = lara_underwater_skin_sweetness_table[i];
		else
			bLaraUnderWater = -1;

		if (SkinUseMatrix[i][0] >= 255)
			phd_PutPolygonsSpcXLU(*meshpp, -1);
		else
		{
			phd_PushMatrix();
			aMXPtr[M00] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M00];
			aMXPtr[M01] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
			aMXPtr[M02] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M02];
			aMXPtr[M03] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M03];
			aMXPtr[M10] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M10];
			aMXPtr[M11] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
			aMXPtr[M12] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M12];
			aMXPtr[M13] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M13];
			aMXPtr[M20] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M20];
			aMXPtr[M21] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];
			aMXPtr[M22] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M22];
			aMXPtr[M23] = lara_matrices[SkinUseMatrix[i][1] * indices_count + M23];

			v0.x = lara_matrices[SkinUseMatrix[i][0] * indices_count + M01];
			v0.y = lara_matrices[SkinUseMatrix[i][0] * indices_count + M11];
			v0.z = lara_matrices[SkinUseMatrix[i][0] * indices_count + M21];
			v1.x = lara_matrices[SkinUseMatrix[i][1] * indices_count + M01];
			v1.y = lara_matrices[SkinUseMatrix[i][1] * indices_count + M11];
			v1.z = lara_matrices[SkinUseMatrix[i][1] * indices_count + M21];

			v0.x *= 1 << W2V_SHIFT;
			v0.y *= 1 << W2V_SHIFT;
			v0.z *= 1 << W2V_SHIFT;
			v1.x *= 1 << W2V_SHIFT;
			v1.y *= 1 << W2V_SHIFT;
			v1.z *= 1 << W2V_SHIFT;

			c = long((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> W2V_SHIFT;
			s = phd_sqrt(0x1000000 - SQUARE(c));

			if (i == 1 || i == 4)
				xRot = -phd_atan(c, s);
			else
				xRot = phd_atan(c, s);

			phd_RotX(short(-xRot >> 1));
			phd_PutPolygonsSpcXLU(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

	if (lara.back_gun)
	{
		phd_PushMatrix();
		aMXPtr[M00] = lara_matrices[LMX_TORSO * indices_count + M00];
		aMXPtr[M01] = lara_matrices[LMX_TORSO * indices_count + M01];
		aMXPtr[M02] = lara_matrices[LMX_TORSO * indices_count + M02];
		aMXPtr[M03] = lara_matrices[LMX_TORSO * indices_count + M03];
		aMXPtr[M10] = lara_matrices[LMX_TORSO * indices_count + M10];
		aMXPtr[M11] = lara_matrices[LMX_TORSO * indices_count + M11];
		aMXPtr[M12] = lara_matrices[LMX_TORSO * indices_count + M12];
		aMXPtr[M13] = lara_matrices[LMX_TORSO * indices_count + M13];
		aMXPtr[M20] = lara_matrices[LMX_TORSO * indices_count + M20];
		aMXPtr[M21] = lara_matrices[LMX_TORSO * indices_count + M21];
		aMXPtr[M22] = lara_matrices[LMX_TORSO * indices_count + M22];
		aMXPtr[M23] = lara_matrices[LMX_TORSO * indices_count + M23];
		bone = objects[lara.back_gun].bone_index;
		phd_TranslateRel(bones[bone + 53], bones[bone + 54], bones[bone + 55]);
		rot = objects[lara.back_gun].frame_base + 9;
		gar_RotYXZsuperpack(&rot, 14);
		phd_PutPolygonsSpcXLU(meshes[objects[lara.back_gun].mesh_index + 28], -1);
		phd_PopMatrix();
	}

	phd_PopMatrix();

	bLaraUnderWater = 0;
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}

void DrawLara(ITEM_INFO* item, long mirror)
{
	long x, y, z, d;
	static long alpha = 255;

	if (tomb5.look_transparency)
	{
		if (input & IN_LOOK)
		{
			x = lara_item->pos.x_pos - CamPos.x;
			y = lara_item->pos.y_pos - CamPos.y - 512;
			z = lara_item->pos.z_pos - CamPos.z;
			d = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
			alpha = d >> 2;

			if (alpha < 0)
				alpha = 0;

			if (alpha > 255)
				alpha = 255;
		}
		else if (alpha < 255)
		{
			alpha += 8;

			if (alpha > 255)
				alpha = 255;
		}

		GlobalAlpha = alpha << 24;
	}

	aGlobalSkinMesh = 1;

	if (lara.skelebob)
		DrawLara__6(item, mirror);
	else
	{
		switch (LaraDrawType)
		{
		case LARA_NORMAL:
		case LARA_YOUNG:
		case LARA_BUNHEAD:
			DrawLara__1(item, mirror);
			break;

		case LARA_CATSUIT:
			DrawLara__4(item, mirror);
			break;

		case LARA_DIVESUIT:
			DrawLara__5(item, mirror);
			break;
		}
	}

	aGlobalSkinMesh = 0;
	GlobalAlpha = 0xFF000000;
}

void SetLaraUnderwaterNodes()
{
	PHD_VECTOR pos;
	long bit;
	short room_num;

	pos.x = lara_item->pos.x_pos;
	pos.y = lara_item->pos.y_pos;
	pos.z = lara_item->pos.z_pos;
	room_num = lara_item->room_number;
	GetFloor(pos.x, pos.y, pos.z, &room_num);
	bLaraInWater = room[room_num].flags & ROOM_UNDERWATER ? 1 : 0;
	bit = 0;

	for (int i = 14; i >= 0; i--)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, i);

		if (lara_mesh_sweetness_table[i] == LM_TORSO)
			pos.y -= 120;

		if (lara_mesh_sweetness_table[i] == LM_HEAD)
			pos.y -= 60;

		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);

		if (lara_mesh_sweetness_table[i] == LM_RINARM || lara_mesh_sweetness_table[i] == LM_LINARM)
			LaraNodeUnderwater[i] = LaraNodeUnderwater[LMX_TORSO];
		else
			LaraNodeUnderwater[i] = room[room_num].flags & ROOM_UNDERWATER;

		if (room[room_num].flags & ROOM_UNDERWATER)
		{
			lara.wet[i] = 252;

			if (!(bit & 1))
			{
				LaraNodeAmbient[1] = room[room_num].ambient;
				bit |= 1;
			}
		}
		else if (!(bit & 2))
		{
			LaraNodeAmbient[0] = room[room_num].ambient;
			bit |= 2;
		}
	}
}
