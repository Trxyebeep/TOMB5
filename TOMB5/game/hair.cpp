#include "../tomb5/pch.h"
#include "hair.h"
#include "../global/types.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "gameflow.h"
#include "objects.h"

//0x00439A40 DrawHair
//HairControl	00438C80

void _DrawHair()//fix later
{
	return;
	HAIR_STRUCT* hair;
	short** meshptr;
	short** meshptr2;

	hair = &hairs[0][1];

	int a, b, c;
	for (a = 0, b = 0, c = 0; a > -12; a -= 6, ++b, c += 6, hair++)
	{
		HAIR_STRUCT* hair1 = hair;
		for (int i = 1; i < 6; i += 2, meshptr += 3, hair1 += 2)
		{
			meshptr = &meshes[objects[HAIR].mesh_index + 2 * i];
			phd_PushMatrix();
			phd_TranslateAbs(hair->pos.x_pos, hair->pos.y_pos, hair->pos.z_pos);
			phd_RotY(hair->pos.y_rot);
			phd_RotX(hair->pos.x_rot);
			phd_PutPolygons(*meshptr, -1);
			if (i == 5)
				StashSkinVertices(c + 33);
			else
			{
				StashSkinVertices(c + i + 28);
				StashSkinVertices(c + i + 29);
			}
			phd_mxptr -= 12;
			phd_dxptr -= 12;
		}

		meshptr2 = &meshes[objects[30].mesh_index];
		int z = c + 29;
		int l = a + c - 29;
		for (; a + z - 29 < 6; z += 2, meshptr2 += 4)
		{
			SkinVerticesToScratch(z - 1);
			GetCorrectStashPoints(b, a + z - 29, z);
			SkinVerticesToScratch(z);
			phd_PutPolygons(*meshptr2, -1);
		}
		if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA))
			break;
	}
}

void inject_hair()
{
//	INJECT(0x00439A40, DrawHair)
}
