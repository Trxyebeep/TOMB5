#include "../tomb5/pch.h"
#include "footprnt.h"
#include "delstuff.h"
#include "control.h"
#include "sound.h"
#ifdef FOOTPRINTS
#include "../specific/3dmath.h"
#include "../specific/specificfx.h"
#include "objects.h"
#include "../specific/function_table.h"
#endif

static char footsounds[14] =
{
	0, 5, 3, 2, 1, 9, 9, 4, 6, 5, 3, 9, 4, 6
};

void AddFootprint(ITEM_INFO* item)
{
	FOOTPRINT* print;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	short room_num;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (FXType == SFX_LANDONLY)
		GetLaraJointPos(&pos, LM_LFOOT);
	else
		GetLaraJointPos(&pos, LM_RFOOT);

	room_num = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_num);

	if (floor->fx != 6 && floor->fx != 5 && floor->fx != 11)
		SoundEffect(footsounds[floor->fx] + 288, &lara_item->pos, 0);

	if (floor->fx < 3 && !OnObject)
	{
		print = &FootPrint[FootPrintNum];
		print->x = pos.x;
		print->y = GetHeight(floor, pos.x, pos.y, pos.z);
		print->z = pos.z;
		print->YRot = item->pos.y_rot;
		print->Active = 512;
		FootPrintNum = FootPrintNum + 1 & 0x1F;
	}
}

#ifdef FOOTPRINTS
void S_DrawFootPrints()
{
    FOOTPRINT* print;
    SPRITESTRUCT* sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, u1, v1, u2, v2;
    long a;

    for (int i = 0; i < 32; i++)
    {
        print = &FootPrint[i];

        if (print->Active)
        {
            print->Active--;
            a = print->Active >> 1;
            phd_PushMatrix();
            phd_TranslateAbs(print->x, print->y, print->z);
            phd_RotY(print->YRot);
            x1 = -128;
            y1 = -16;
            z1 = 0;
            x1 = aMXPtr[M00] * x1 + aMXPtr[M01] * y1 + aMXPtr[M02] * z1 + aMXPtr[M03];
            y1 = aMXPtr[M10] * x1 + aMXPtr[M11] * y1 + aMXPtr[M12] * z1 + aMXPtr[M13];
            z1 = aMXPtr[M20] * x1 + aMXPtr[M21] * y1 + aMXPtr[M22] * z1 + aMXPtr[M23];

            x2 = 128;
            y2 = -16;
            z2 = 0;
            x2 = aMXPtr[M00] * x2 + aMXPtr[M01] * y2 + aMXPtr[M02] * z2 + aMXPtr[M03];
            y2 = aMXPtr[M10] * x2 + aMXPtr[M11] * y2 + aMXPtr[M12] * z2 + aMXPtr[M13];
            z2 = aMXPtr[M20] * x2 + aMXPtr[M21] * y2 + aMXPtr[M22] * z2 + aMXPtr[M23];

            x3 = 0;
            y3 = -16;
            z3 = -64;
            x3 = aMXPtr[M00] * x3 + aMXPtr[M01] * y3 + aMXPtr[M02] * z3 + aMXPtr[M03];
            y3 = aMXPtr[M10] * x3 + aMXPtr[M11] * y3 + aMXPtr[M12] * z3 + aMXPtr[M13];
            z3 = aMXPtr[M20] * x3 + aMXPtr[M21] * y3 + aMXPtr[M22] * z3 + aMXPtr[M23];

            x4 = 0;
            y4 = -16;
            z4 = 64;
            x4 = aMXPtr[M00] * x4 + aMXPtr[M01] * y4 + aMXPtr[M02] * z4 + aMXPtr[M03];
            y4 = aMXPtr[M10] * x4 + aMXPtr[M11] * y4 + aMXPtr[M12] * z4 + aMXPtr[M13];
            z4 = aMXPtr[M20] * x4 + aMXPtr[M21] * y4 + aMXPtr[M22] * z4 + aMXPtr[M23];

            phd_PopMatrix();
            setXYZ4(v, (long)x1, (long)y1, (long)z1, (long)x2, (long)y2, (long)z2, (long)x3, (long)y3, (long)z3, (long)x4, (long)y4, (long)z4, clipflags);

            for (int i = 0; i < 4; i++)
            {
                v[i].color = RGBA(64, 64, 64, a);
                v[i].specular = 0xFF000000;
            }

            sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 10];
            tex.drawtype = 3;
            tex.flag = 0;
            tex.tpage = sprite->tpage;
            u1 = sprite->x2;
            u2 = sprite->x1;
            v1 = sprite->y2;
            v2 = sprite->y1;
            tex.u1 = u1;
            tex.v1 = v1;
            tex.u2 = u2;
            tex.v2 = v1;
            tex.u3 = u2;
            tex.v3 = v2;
            tex.u4 = u1;
            tex.v4 = v2;
            AddQuadSorted(v, 3, 2, 1, 0, &tex, 1);
        }
    }
}
#endif

void inject_footprnt(bool replace)
{
	INJECT(0x004346A0, AddFootprint, replace);
}
