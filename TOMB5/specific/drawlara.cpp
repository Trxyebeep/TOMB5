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
#ifdef GENERAL_FIXES
#include "../game/savegame.h"
#include "../tomb5/tomb5.h"
#include "../game/lara2gun.h"
#endif

static long lara_mesh_sweetness_table[15] = { 0, 1, 2, 3, 4, 5, 6, 7, 14, 8, 9, 10, 11, 12, 13 };
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

void DrawLara(ITEM_INFO* item, long mirror)
{
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
}

void DrawLara__1(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	PHD_VECTOR v0;
	PHD_VECTOR v1;
	short** meshpp;
	short* rot;
	long dx, dy, dz, dist, cos, sin, xRot, bone, top, bottom, left, right, stash;
	static long trans_lara = 255;

	aGlobalSkinMesh = 1;
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

#ifdef GENERAL_FIXES
	if (tomb5.look_transparency)
#endif
	{
		if (input & IN_LOOK)
		{
			dx = lara_item->pos.x_pos - CamPos.x;
			dy = lara_item->pos.y_pos - CamPos.y - 512;
			dz = lara_item->pos.z_pos - CamPos.z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
			trans_lara = dist >> 2;

			if (trans_lara < 0)
				trans_lara = 0;

			if (trans_lara > 255)
				trans_lara = 255;

			GlobalAlpha = trans_lara << 24;
		}
		else
		{
			if (trans_lara < 255)
			{
				trans_lara += 8;

				if (trans_lara > 255)
					trans_lara = 255;
			}

			GlobalAlpha = trans_lara << 24;
		}
	}

	if (!mirror)
		CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matricesF[i * 12 + M00];
		aMXPtr[M01] = lara_matricesF[i * 12 + M01];
		aMXPtr[M02] = lara_matricesF[i * 12 + M02];
		aMXPtr[M03] = lara_matricesF[i * 12 + M03];
		aMXPtr[M10] = lara_matricesF[i * 12 + M10];
		aMXPtr[M11] = lara_matricesF[i * 12 + M11];
		aMXPtr[M12] = lara_matricesF[i * 12 + M12];
		aMXPtr[M13] = lara_matricesF[i * 12 + M13];
		aMXPtr[M20] = lara_matricesF[i * 12 + M20];
		aMXPtr[M21] = lara_matricesF[i * 12 + M21];
		aMXPtr[M22] = lara_matricesF[i * 12 + M22];
		aMXPtr[M23] = lara_matricesF[i * 12 + M23];

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
				phd_PushMatrix();
				aMXPtr[M00] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M00];
				aMXPtr[M01] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M01];
				aMXPtr[M02] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M02];
				aMXPtr[M03] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M03];
				aMXPtr[M10] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M10];
				aMXPtr[M11] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M11];
				aMXPtr[M12] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M12];
				aMXPtr[M13] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M13];
				aMXPtr[M20] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M20];
				aMXPtr[M21] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M21];
				aMXPtr[M22] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M22];
				aMXPtr[M23] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M23];
				v0.x = lara_matrices[12 * SkinUseMatrix[i][0] + M01];
				v0.y = lara_matrices[12 * SkinUseMatrix[i][0] + M11];
				v0.z = lara_matrices[12 * SkinUseMatrix[i][0] + M21];
				v1.x = lara_matrices[12 * SkinUseMatrix[i][1] + M01];
				v1.y = lara_matrices[12 * SkinUseMatrix[i][1] + M11];
				v1.z = lara_matrices[12 * SkinUseMatrix[i][1] + M21];
				cos = ((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> 14;
				sin = phd_sqrt(0x1000000 - SQUARE(cos));

				if (i == 1 || i == 4)
					xRot = -phd_atan(cos, sin);
				else
					xRot = phd_atan(cos, sin);

				phd_RotX((short)(-xRot >> 1));
				phd_PutPolygons(*meshpp, -1);
				phd_PopMatrix();
			}
		}

		meshpp += 2;
	}

	aGlobalSkinMesh = 0;
	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
#ifdef GENERAL_FIXES
		obj = &objects[LHolster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 8;
		aMXPtr[M00] = lara_matricesF[1 * 12 + M00];
		aMXPtr[M01] = lara_matricesF[1 * 12 + M01];
		aMXPtr[M02] = lara_matricesF[1 * 12 + M02];
		aMXPtr[M03] = lara_matricesF[1 * 12 + M03];
		aMXPtr[M10] = lara_matricesF[1 * 12 + M10];
		aMXPtr[M11] = lara_matricesF[1 * 12 + M11];
		aMXPtr[M12] = lara_matricesF[1 * 12 + M12];
		aMXPtr[M13] = lara_matricesF[1 * 12 + M13];
		aMXPtr[M20] = lara_matricesF[1 * 12 + M20];
		aMXPtr[M21] = lara_matricesF[1 * 12 + M21];
		aMXPtr[M22] = lara_matricesF[1 * 12 + M22];
		aMXPtr[M23] = lara_matricesF[1 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);

		obj = &objects[lara.holster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 16;
		aMXPtr[M00] = lara_matricesF[4 * 12 + M00];
		aMXPtr[M01] = lara_matricesF[4 * 12 + M01];
		aMXPtr[M02] = lara_matricesF[4 * 12 + M02];
		aMXPtr[M03] = lara_matricesF[4 * 12 + M03];
		aMXPtr[M10] = lara_matricesF[4 * 12 + M10];
		aMXPtr[M11] = lara_matricesF[4 * 12 + M11];
		aMXPtr[M12] = lara_matricesF[4 * 12 + M12];
		aMXPtr[M13] = lara_matricesF[4 * 12 + M13];
		aMXPtr[M20] = lara_matricesF[4 * 12 + M20];
		aMXPtr[M21] = lara_matricesF[4 * 12 + M21];
		aMXPtr[M22] = lara_matricesF[4 * 12 + M22];
		aMXPtr[M23] = lara_matricesF[4 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);
#else
		obj = &objects[lara.holster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 8;
		aMXPtr[M00] = lara_matricesF[1 * 12 + M00];//holsters
		aMXPtr[M01] = lara_matricesF[1 * 12 + M01];
		aMXPtr[M02] = lara_matricesF[1 * 12 + M02];
		aMXPtr[M03] = lara_matricesF[1 * 12 + M03];
		aMXPtr[M10] = lara_matricesF[1 * 12 + M10];
		aMXPtr[M11] = lara_matricesF[1 * 12 + M11];
		aMXPtr[M12] = lara_matricesF[1 * 12 + M12];
		aMXPtr[M13] = lara_matricesF[1 * 12 + M13];
		aMXPtr[M20] = lara_matricesF[1 * 12 + M20];
		aMXPtr[M21] = lara_matricesF[1 * 12 + M21];
		aMXPtr[M22] = lara_matricesF[1 * 12 + M22];
		aMXPtr[M23] = lara_matricesF[1 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);
		meshpp += 8;
		aMXPtr[M00] = lara_matricesF[4 * 12 + M00];
		aMXPtr[M01] = lara_matricesF[4 * 12 + M01];
		aMXPtr[M02] = lara_matricesF[4 * 12 + M02];
		aMXPtr[M03] = lara_matricesF[4 * 12 + M03];
		aMXPtr[M10] = lara_matricesF[4 * 12 + M10];
		aMXPtr[M11] = lara_matricesF[4 * 12 + M11];
		aMXPtr[M12] = lara_matricesF[4 * 12 + M12];
		aMXPtr[M13] = lara_matricesF[4 * 12 + M13];
		aMXPtr[M20] = lara_matricesF[4 * 12 + M20];
		aMXPtr[M21] = lara_matricesF[4 * 12 + M21];
		aMXPtr[M22] = lara_matricesF[4 * 12 + M22];
		aMXPtr[M23] = lara_matricesF[4 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);
#endif

		if (lara.back_gun)//back gun
		{
			phd_PushMatrix();

#ifdef GENERAL_FIXES	//fixes back weapons not drawing
			aMXPtr[M00] = lara_matricesF[7 * indices_count + M00];
			aMXPtr[M01] = lara_matricesF[7 * indices_count + M01];
			aMXPtr[M02] = lara_matricesF[7 * indices_count + M02];
			aMXPtr[M03] = lara_matricesF[7 * indices_count + M03];
			aMXPtr[M10] = lara_matricesF[7 * indices_count + M10];
			aMXPtr[M11] = lara_matricesF[7 * indices_count + M11];
			aMXPtr[M12] = lara_matricesF[7 * indices_count + M12];
			aMXPtr[M13] = lara_matricesF[7 * indices_count + M13];
			aMXPtr[M20] = lara_matricesF[7 * indices_count + M20];
			aMXPtr[M21] = lara_matricesF[7 * indices_count + M21];
			aMXPtr[M22] = lara_matricesF[7 * indices_count + M22];
			aMXPtr[M23] = lara_matricesF[7 * indices_count + M23];
#else
			aMXPtr[0] = *((float*)(&lara_matrices[84]) + 0);
			aMXPtr[1] = *((float*)(&lara_matrices[84]) + 1);
			aMXPtr[2] = *((float*)(&lara_matrices[84]) + 2);
			aMXPtr[3] = *((float*)(&lara_matrices[84]) + 3);
			aMXPtr[4] = *((float*)(&lara_matrices[84]) + 4);
			aMXPtr[5] = *((float*)(&lara_matrices[84]) + 5);
			aMXPtr[6] = *((float*)(&lara_matrices[84]) + 6);
			aMXPtr[7] = *((float*)(&lara_matrices[84]) + 7);
			aMXPtr[8] = *((float*)(&lara_matrices[84]) + 8);
			aMXPtr[9] = *((float*)(&lara_matrices[84]) + 9);
			aMXPtr[10] = *((float*)(&lara_matrices[84]) + 10);
			aMXPtr[11] = *((float*)(&lara_matrices[84]) + 11);
#endif
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
	GlobalAlpha = 0xFF000000;
}

void DrawLara__4(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	PHD_VECTOR v0;
	PHD_VECTOR v1;
	short** meshpp;
	short* rot;
	long dx, dy, dz, dist, cos, sin, xRot, bone, top, bottom, left, right, stash;
	static long trans_lara = 255;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	aGlobalSkinMesh = 1;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];
	S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);

#ifdef GENERAL_FIXES
	if (tomb5.look_transparency)
#endif
	{
		if (input & IN_LOOK)
		{
			dx = lara_item->pos.x_pos - CamPos.x;
			dy = lara_item->pos.y_pos - CamPos.y - 512;
			dz = lara_item->pos.z_pos - CamPos.z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
			trans_lara = dist >> 2;

			if (trans_lara < 0)
				trans_lara = 0;

			if (trans_lara > 255)
				trans_lara = 255;

			GlobalAlpha = trans_lara << 24;
		}
		else
		{
			if (trans_lara < 255)
			{
				trans_lara += 8;

				if (trans_lara > 255)
					trans_lara = 255;
			}

			GlobalAlpha = trans_lara << 24;
		}
	}

	if (!mirror)
		CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matricesF[i * 12 + M00];
		aMXPtr[M01] = lara_matricesF[i * 12 + M01];
		aMXPtr[M02] = lara_matricesF[i * 12 + M02];
		aMXPtr[M03] = lara_matricesF[i * 12 + M03];
		aMXPtr[M10] = lara_matricesF[i * 12 + M10];
		aMXPtr[M11] = lara_matricesF[i * 12 + M11];
		aMXPtr[M12] = lara_matricesF[i * 12 + M12];
		aMXPtr[M13] = lara_matricesF[i * 12 + M13];
		aMXPtr[M20] = lara_matricesF[i * 12 + M20];
		aMXPtr[M21] = lara_matricesF[i * 12 + M21];
		aMXPtr[M22] = lara_matricesF[i * 12 + M22];
		aMXPtr[M23] = lara_matricesF[i * 12 + M23];

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
			aMXPtr[M00] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M00];
			aMXPtr[M01] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M01];
			aMXPtr[M02] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M02];
			aMXPtr[M03] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M03];
			aMXPtr[M10] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M10];
			aMXPtr[M11] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M11];
			aMXPtr[M12] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M12];
			aMXPtr[M13] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M13];
			aMXPtr[M20] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M20];
			aMXPtr[M21] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M21];
			aMXPtr[M22] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M22];
			aMXPtr[M23] = lara_matricesF[SkinUseMatrix[i][1] * 12 + M23];
			v0.x = lara_matrices[12 * SkinUseMatrix[i][0] + M01];
			v0.y = lara_matrices[12 * SkinUseMatrix[i][0] + M11];
			v0.z = lara_matrices[12 * SkinUseMatrix[i][0] + M21];
			v1.x = lara_matrices[12 * SkinUseMatrix[i][1] + M01];
			v1.y = lara_matrices[12 * SkinUseMatrix[i][1] + M11];
			v1.z = lara_matrices[12 * SkinUseMatrix[i][1] + M21];
			cos = ((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> 14;
			sin = phd_sqrt(0x1000000 - SQUARE(cos));

			if (i == 1 || i == 4)
				xRot = -phd_atan(cos, sin);
			else
				xRot = phd_atan(cos, sin);

			phd_RotX((short)(-xRot >> 1));
			aSetViewMatrix();
			phd_PutPolygonsSpcEnvmap(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

	aGlobalSkinMesh = 0;
	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	if (!(gfLevelFlags & GF_YOUNGLARA) && lara.back_gun)
	{
		phd_PushMatrix();
		aMXPtr[M00] = lara_matricesF[7 * indices_count + M00];
		aMXPtr[M01] = lara_matricesF[7 * indices_count + M01];
		aMXPtr[M02] = lara_matricesF[7 * indices_count + M02];
		aMXPtr[M03] = lara_matricesF[7 * indices_count + M03];
		aMXPtr[M10] = lara_matricesF[7 * indices_count + M10];
		aMXPtr[M11] = lara_matricesF[7 * indices_count + M11];
		aMXPtr[M12] = lara_matricesF[7 * indices_count + M12];
		aMXPtr[M13] = lara_matricesF[7 * indices_count + M13];
		aMXPtr[M20] = lara_matricesF[7 * indices_count + M20];
		aMXPtr[M21] = lara_matricesF[7 * indices_count + M21];
		aMXPtr[M22] = lara_matricesF[7 * indices_count + M22];
		aMXPtr[M23] = lara_matricesF[7 * indices_count + M23];
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
	GlobalAlpha = 0xFF000000;
}

void DrawLara__5(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long dx, dy, dz, dist, top, bottom, left, right;
	static long trans_lara = 255;

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

#ifdef GENERAL_FIXES
	if (tomb5.look_transparency)
#endif
	{
		if (input & IN_LOOK)
		{
			dx = lara_item->pos.x_pos - CamPos.x;
			dy = lara_item->pos.y_pos - CamPos.y - 512;
			dz = lara_item->pos.z_pos - CamPos.z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
			trans_lara = dist >> 2;

			if (trans_lara < 0)
				trans_lara = 0;

			if (trans_lara > 255)
				trans_lara = 255;

			GlobalAlpha = trans_lara << 24;
		}
		else
		{
			if (trans_lara < 255)
			{
				trans_lara += 8;

				if (trans_lara > 255)
					trans_lara = 255;
			}

			GlobalAlpha = trans_lara << 24;
		}
	}

	CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		aMXPtr[M00] = lara_matricesF[i * indices_count + M00];
		aMXPtr[M01] = lara_matricesF[i * indices_count + M01];
		aMXPtr[M02] = lara_matricesF[i * indices_count + M02];
		aMXPtr[M03] = lara_matricesF[i * indices_count + M03];
		aMXPtr[M10] = lara_matricesF[i * indices_count + M10];
		aMXPtr[M11] = lara_matricesF[i * indices_count + M11];
		aMXPtr[M12] = lara_matricesF[i * indices_count + M12];
		aMXPtr[M13] = lara_matricesF[i * indices_count + M13];
		aMXPtr[M20] = lara_matricesF[i * indices_count + M20];
		aMXPtr[M21] = lara_matricesF[i * indices_count + M21];
		aMXPtr[M22] = lara_matricesF[i * indices_count + M22];
		aMXPtr[M23] = lara_matricesF[i * indices_count + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);
	}

	phd_PopMatrix();
	bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
	phd_PushMatrix();
	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;
	phd_PopMatrix();
	phd_left = left;
	phd_right = right;
	phd_top = top;
	phd_bottom = bottom;
	GlobalAlpha = 0xFF000000;
	obj = &objects[LARA_EXTRA_MESH1];
	meshpp = &meshes[obj->mesh_index];

	phd_PushMatrix();
	aMXPtr[M00] = lara_matricesF[7 * indices_count + M00];
	aMXPtr[M01] = lara_matricesF[7 * indices_count + M01];
	aMXPtr[M02] = lara_matricesF[7 * indices_count + M02];
	aMXPtr[M03] = lara_matricesF[7 * indices_count + M03];
	aMXPtr[M10] = lara_matricesF[7 * indices_count + M10];
	aMXPtr[M11] = lara_matricesF[7 * indices_count + M11];
	aMXPtr[M12] = lara_matricesF[7 * indices_count + M12];
	aMXPtr[M13] = lara_matricesF[7 * indices_count + M13];
	aMXPtr[M20] = lara_matricesF[7 * indices_count + M20];
	aMXPtr[M21] = lara_matricesF[7 * indices_count + M21];
	aMXPtr[M22] = lara_matricesF[7 * indices_count + M22];
	aMXPtr[M23] = lara_matricesF[7 * indices_count + M23];
	aTranslateRel(-80, -192, -160);
	aRotX(subsuit.XRot);
	phd_PutPolygons(*meshpp, -1);
	phd_PopMatrix();

	phd_PushMatrix();
	aMXPtr[M00] = lara_matricesF[7 * indices_count + M00];
	aMXPtr[M01] = lara_matricesF[7 * indices_count + M01];
	aMXPtr[M02] = lara_matricesF[7 * indices_count + M02];
	aMXPtr[M03] = lara_matricesF[7 * indices_count + M03];
	aMXPtr[M10] = lara_matricesF[7 * indices_count + M10];
	aMXPtr[M11] = lara_matricesF[7 * indices_count + M11];
	aMXPtr[M12] = lara_matricesF[7 * indices_count + M12];
	aMXPtr[M13] = lara_matricesF[7 * indices_count + M13];
	aMXPtr[M20] = lara_matricesF[7 * indices_count + M20];
	aMXPtr[M21] = lara_matricesF[7 * indices_count + M21];
	aMXPtr[M22] = lara_matricesF[7 * indices_count + M22];
	aMXPtr[M23] = lara_matricesF[7 * indices_count + M23];
	aTranslateRel(80, -192, -160);
	aRotX(subsuit.XRot);
	phd_PutPolygons(*meshpp, -1);
	phd_PopMatrix();
	bLaraUnderWater = 0;
}

void DrawLara__6(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	PHD_VECTOR v0;
	PHD_VECTOR v1;
	short** meshpp;
#ifdef GENERAL_FIXES
	short* rot;
	long bone;
#endif
	long cos, sin, xRot, top, bottom, left, right, stash;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	aGlobalSkinMesh = 1;
	phd_top = 0;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];

#ifndef GENERAL_FIXES
	S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);
#endif

	if (!mirror)
		CalculateObjectLightingLara();

	meshpp = &meshes[objects[LARA_EXTRA_MESH1].mesh_index];

	for (int i = 0; i < 15; i++)
	{
		aMXPtr[M00] = lara_matricesF[i * indices_count + M00];
		aMXPtr[M01] = lara_matricesF[i * indices_count + M01];
		aMXPtr[M02] = lara_matricesF[i * indices_count + M02];
		aMXPtr[M03] = lara_matricesF[i * indices_count + M03];
		aMXPtr[M10] = lara_matricesF[i * indices_count + M10];
		aMXPtr[M11] = lara_matricesF[i * indices_count + M11];
		aMXPtr[M12] = lara_matricesF[i * indices_count + M12];
		aMXPtr[M13] = lara_matricesF[i * indices_count + M13];
		aMXPtr[M20] = lara_matricesF[i * indices_count + M20];
		aMXPtr[M21] = lara_matricesF[i * indices_count + M21];
		aMXPtr[M22] = lara_matricesF[i * indices_count + M22];
		aMXPtr[M23] = lara_matricesF[i * indices_count + M23];

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
			aMXPtr[M00] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M00];
			aMXPtr[M01] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M01];
			aMXPtr[M02] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M02];
			aMXPtr[M03] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M03];
			aMXPtr[M10] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M10];
			aMXPtr[M11] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M11];
			aMXPtr[M12] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M12];
			aMXPtr[M13] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M13];
			aMXPtr[M20] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M20];
			aMXPtr[M21] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M21];
			aMXPtr[M22] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M22];
			aMXPtr[M23] = lara_matricesF[SkinUseMatrix[i][1] * indices_count + M23];
			v0.x = lara_matrices[indices_count * SkinUseMatrix[i][0] + M01];
			v0.y = lara_matrices[indices_count * SkinUseMatrix[i][0] + M11];
			v0.z = lara_matrices[indices_count * SkinUseMatrix[i][0] + M21];
			v1.x = lara_matrices[indices_count * SkinUseMatrix[i][1] + M01];
			v1.y = lara_matrices[indices_count * SkinUseMatrix[i][1] + M11];
			v1.z = lara_matrices[indices_count * SkinUseMatrix[i][1] + M21];
			cos = ((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> 14;
			sin = phd_sqrt(0x1000000 - SQUARE(cos));

			if (i == 1 || i == 4)
				xRot = -phd_atan(cos, sin);
			else
				xRot = phd_atan(cos, sin);

			phd_RotX((short)(-xRot >> 1));
			phd_PutPolygonsSpcXLU(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

#ifdef GENERAL_FIXES
	if (lara.back_gun)
	{
		phd_PushMatrix();
		aMXPtr[M00] = lara_matricesF[7 * indices_count + M00];
		aMXPtr[M01] = lara_matricesF[7 * indices_count + M01];
		aMXPtr[M02] = lara_matricesF[7 * indices_count + M02];
		aMXPtr[M03] = lara_matricesF[7 * indices_count + M03];
		aMXPtr[M10] = lara_matricesF[7 * indices_count + M10];
		aMXPtr[M11] = lara_matricesF[7 * indices_count + M11];
		aMXPtr[M12] = lara_matricesF[7 * indices_count + M12];
		aMXPtr[M13] = lara_matricesF[7 * indices_count + M13];
		aMXPtr[M20] = lara_matricesF[7 * indices_count + M20];
		aMXPtr[M21] = lara_matricesF[7 * indices_count + M21];
		aMXPtr[M22] = lara_matricesF[7 * indices_count + M22];
		aMXPtr[M23] = lara_matricesF[7 * indices_count + M23];
		bone = objects[lara.back_gun].bone_index;
		phd_TranslateRel(bones[bone + 53], bones[bone + 54], bones[bone + 55]);
		rot = objects[lara.back_gun].frame_base + 9;
		gar_RotYXZsuperpack(&rot, 14);
		phd_PutPolygonsSpcXLU(meshes[objects[lara.back_gun].mesh_index + 28], -1);
		phd_PopMatrix();
	}
#endif

	phd_PopMatrix();
	phd_left = left;
	phd_right = right;
	phd_top = top;
	phd_bottom = bottom;
	GlobalAlpha = 0xFF000000;
	aGlobalSkinMesh = 0;
	bLaraUnderWater = 0;
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

		if (lara_mesh_sweetness_table[i] == 7)
			pos.y -= 120;

		if (lara_mesh_sweetness_table[i] == 14)
			pos.y -= 60;

		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);
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

void inject_drawlara(bool replace)
{
	INJECT(0x00498030, DrawLara, replace);
	INJECT(0x00498100, DrawLara__1, replace);
	INJECT(0x00498C70, DrawLara__4, replace);
	INJECT(0x004995C0, DrawLara__5, replace);
	INJECT(0x00499BA0, DrawLara__6, replace);
	INJECT(0x0049A210, SetLaraUnderwaterNodes, replace);
}
