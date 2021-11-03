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

#ifdef FOOTPRINTS
	GetProperFootPos(&pos);
#else
	if (FXType == SFX_LANDONLY)
		GetLaraJointPos(&pos, LM_LFOOT);
	else
		GetLaraJointPos(&pos, LM_RFOOT);
#endif

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
	D3DTLVERTEX v[3];
	FVECTOR pos;
	TEXTURESTRUCT tex;
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, u1, v1, u2, v2;
	long a;

	for (int i = 0; i < 32; i++)
	{
		print = &FootPrint[i];

		if (print->Active)
		{
			print->Active--;
			a = print->Active >> 2;
			phd_PushMatrix();
			phd_TranslateAbs(print->x, print->y, print->z);
			phd_RotY(print->YRot - 8192);
			pos.x = -128;
			pos.y = 0;	//top left
			pos.z = 64;
			x1 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
			y1 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
			z1 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

			pos.x = -128;
			pos.y = 0;	//bottom left
			pos.z = -64;
			x2 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
			y2 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
			z2 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

			pos.x = 128;
			pos.y = 0;	//top right
			pos.z = 64;
			x3 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
			y3 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
			z3 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

			phd_PopMatrix();
			setXYZ3(v, (long)x1, (long)y1, (long)z1, (long)x2, (long)y2, (long)z2, (long)x3, (long)y3, (long)z3, clipflags);

			for (int i = 0; i < 3; i++)
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
			tex.u1 = u2;
			tex.v1 = v1;	//bottom left
			tex.u2 = u2;
			tex.v2 = v2;	//top left
			tex.u3 = u1;
			tex.v3 = v2;	//top right
			AddTriSorted(v, 0, 1, 2, &tex, 1);	//a tri instead of a quad is needed to avoid the snow sprite
		}
	}
}

void GetProperFootPos(PHD_VECTOR* pos)	//more hacks to be added later
{
	PHD_VECTOR left_foot;
	PHD_VECTOR right_foot;
	FLOOR_INFO* floor;
	long height;
	short room_number;

	left_foot.x = 0;
	left_foot.y = 0;
	left_foot.z = 0;
	GetLaraJointPos(&left_foot, LM_LFOOT);

	right_foot.x = 0;
	right_foot.y = 0;
	right_foot.z = 0;
	GetLaraJointPos(&right_foot, LM_RFOOT);

	room_number = lara_item->room_number;
	floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
	height = GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	if (ABS(left_foot.y - height) < ABS(right_foot.y - height))
	{
		pos->x = left_foot.x;	//left foot winer!
		pos->y = left_foot.y;
		pos->z = left_foot.z;
	}
	else
	{
		pos->x = right_foot.x;	//right foot winer!
		pos->y = right_foot.y;
		pos->z = right_foot.z;
	}
}
#endif

void inject_footprnt(bool replace)
{
	INJECT(0x004346A0, AddFootprint, replace);
}
