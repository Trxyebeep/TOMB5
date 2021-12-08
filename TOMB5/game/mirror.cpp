#include "../tomb5/pch.h"
#include "mirror.h"
#include "delstuff.h"
#include "../specific/function_table.h"
#include "../specific/drawlara.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"

void Draw_Mirror_Lara()
{
	HAIR_STRUCT* hair;
	GUNSHELL_STRUCT* gunshell;
	long zplane;
	
	zplane = gfMirrorZPlane << 1;
	hair = &hairs[0][0];

	for (int i = 0; i < 6; i++)
	{
		hair->pos.z_pos = zplane - hair->pos.z_pos;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}

	for (int i = 0; i < 24; i++)
	{
		gunshell = &Gunshells[i];

		if (gunshell->counter)
			gunshell->pos.z_pos = zplane - gunshell->pos.z_pos;
	}

	lara_item->pos.z_pos = zplane - lara_item->pos.z_pos;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	CalcLaraMatrices(2);
	SetCullCW();
	DrawLara(lara_item, 1);
	DrawGunshells();
	SetCullCCW();
	lara_item->pos.z_pos = zplane - lara_item->pos.z_pos;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	phd_PushMatrix();

	if (lara.right_arm.flash_gun)
	{
		phd_mxptr[M00] = lara_matrices[11].m00;
		phd_mxptr[M01] = lara_matrices[11].m01;
		phd_mxptr[M02] = lara_matrices[11].m02;
		phd_mxptr[M03] = lara_matrices[11].m03;
		phd_mxptr[M10] = lara_matrices[11].m10;
		phd_mxptr[M11] = lara_matrices[11].m11;
		phd_mxptr[M12] = lara_matrices[11].m12;
		phd_mxptr[M13] = lara_matrices[11].m13;
		phd_mxptr[M20] = lara_matrices[11].m20;
		phd_mxptr[M21] = lara_matrices[11].m21;
		phd_mxptr[M22] = lara_matrices[11].m22;
		phd_mxptr[M23] = lara_matrices[11].m23;
		SetGunFlash(lara.gun_type);
	}

	if (lara.left_arm.flash_gun)
	{
		phd_mxptr[M00] = lara_matrices[14].m00;
		phd_mxptr[M01] = lara_matrices[14].m01;
		phd_mxptr[M02] = lara_matrices[14].m02;
		phd_mxptr[M03] = lara_matrices[14].m03;
		phd_mxptr[M10] = lara_matrices[14].m10;
		phd_mxptr[M11] = lara_matrices[14].m11;
		phd_mxptr[M12] = lara_matrices[14].m12;
		phd_mxptr[M13] = lara_matrices[14].m13;
		phd_mxptr[M20] = lara_matrices[14].m20;
		phd_mxptr[M21] = lara_matrices[14].m21;
		phd_mxptr[M22] = lara_matrices[14].m22;
		phd_mxptr[M23] = lara_matrices[14].m23;
		SetGunFlash(lara.gun_type);
	}

	phd_PopMatrix();

	for (int i = 0; i < 24; i++)
	{
		gunshell = &Gunshells[i];

		if (gunshell->counter)
			gunshell->pos.z_pos = zplane - gunshell->pos.z_pos;
	}

	hair = &hairs[0][0];

	for (int i = 0; i < 6; i++)
	{
		hair->pos.z_pos = zplane - hair->pos.z_pos;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}
}

void inject_mirror(bool replace)
{
	INJECT(0x0045DC50, Draw_Mirror_Lara, replace);
}
