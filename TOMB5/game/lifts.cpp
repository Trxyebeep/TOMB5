#include "../tomb5/pch.h"
#include "lifts.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"

void DrawLiftDoors(ITEM_INFO* item)
{
	VECTOR v;
	short** meshpp;
	short* frmptr[2];
	long clip, rate;

	if (item->item_flags[0])
	{
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_bottom = phd_winheight;
		GetFrames(item, frmptr, &rate);
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		clip = S_GetObjectBounds(frmptr[0]);

		if (clip)
		{
			meshpp = &meshes[objects[item->object_number].mesh_index];
			phd_TranslateRel(frmptr[0][6], frmptr[0][7], frmptr[0][8]);
			v.vx = item->item_flags[0] << 2;
			v.vy = 16384;
			v.vz = 16384;
			ScaleCurrentMatrix(&v);
			CalculateObjectLighting(item, frmptr[0]);
			phd_PutPolygons(*meshpp, clip);
		}

		phd_bottom = phd_winheight;
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_PopMatrix();
	}
}

void inject_lifts(bool replace)
{
	INJECT(0x0045AAF0, DrawLiftDoors, replace);
}
