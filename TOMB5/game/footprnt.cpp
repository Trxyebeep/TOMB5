#include "../tomb5/pch.h"
#include "footprnt.h"
#include "delstuff.h"
#include "control.h"
#include "sound.h"
#include "gameflow.h"
#include "../specific/file.h"
#include "lara.h"
#include "../specific/gamemain.h"
#include "../specific/3dmath.h"
#include "../specific/specificfx.h"
#include "objects.h"
#include "../specific/function_table.h"
#include "../tomb5/tomb5.h"

#define PRINT_HEIGHT_CORRECTION 128 // The maximum difference between the footprint and the floor

static char footsounds[14] = { 0, 5, 3, 2, 1, 9, 9, 4, 6, 5, 3, 9, 4, 6 };

FOOTPRINT FootPrint[32];
long FootPrintNum;

void AddFootprint(ITEM_INFO* item)
{
	FOOTPRINT* print;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	short room_num;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetProperFootPos(&pos);

	room_num = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_num);

	if (floor->fx != 6 && floor->fx != 5 && floor->fx != 11)
		SoundEffect(footsounds[floor->fx] + SFX_FOOTSTEPS_MUD, &lara_item->pos, 0);

	if (tomb5.footprints && floor->fx < 3 && (gfCurrentLevel == LVL5_BASE || !OnObject))
	{
		print = &FootPrint[FootPrintNum];
		print->x = pos.x;
		print->y = GetHeight(floor, pos.x, pos.y, pos.z);
		print->z = pos.z;
		print->YRot = item->pos.y_rot;
		print->Active = 512;
		FootPrintNum = (FootPrintNum + 1) & 0x1F;
	}
}

static void ProjectTriPoints(FVECTOR pos, float& x, float& y, float& z)
{
	x = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
	y = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
	z = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];
}

void S_DrawFootPrints()
{
	FOOTPRINT* print;
	SPRITESTRUCT* sprite;
	FLOOR_INFO* floor;
	D3DTLVERTEX v[3];
	FVECTOR pos[3];
	TEXTURESTRUCT tex;
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, u1, v1, u2, v2;
	long x, z, col;
	short room_number;

	for (int i = 0; i < 32; i++)
	{
		print = &FootPrint[i];

		if (print->Active)
		{
			if (!tomb5.footprints)
			{
				print->Active = 0;
				continue;
			}

			print->Active--;

			if (print->Active < 29)
				col = print->Active << 2;
			else
				col = 112;

			pos[0].x = 0;
			pos[0].z = -64;
			pos[1].x = -128;
			pos[1].z = 64;
			pos[2].x = 128;
			pos[2].z = 64;
			phd_PushUnitMatrix();
			phd_TranslateRel(print->x, print->y, print->z);
			phd_RotY(print->YRot);
			
			for (int j = 0; j < 3; j++)
			{
				x = long(pos[j].x * aMXPtr[M00] + pos[j].z * aMXPtr[M02] + aMXPtr[M03]);
				z = long(pos[j].x * aMXPtr[M20] + pos[j].z * aMXPtr[M22] + aMXPtr[M23]);

				room_number = lara_item->room_number;
				floor = GetFloor(x, print->y, z, &room_number);
				pos[j].y = (float)GetHeight(floor, x, print->y, z) - print->y;

				if (abs(pos[j].y) > PRINT_HEIGHT_CORRECTION)
					pos[j].y = 0;
			}

			phd_PopMatrix();

			phd_PushMatrix();
			phd_TranslateAbs(print->x, print->y - 1, print->z);
			phd_RotY(print->YRot);

			ProjectTriPoints(pos[0], x1, y1, z1);
			ProjectTriPoints(pos[1], x2, y2, z2);
			ProjectTriPoints(pos[2], x3, y3, z3);

			phd_PopMatrix();
			setXYZ3(v, (long)x1, (long)y1, (long)z1, (long)x2, (long)y2, (long)z2, (long)x3, (long)y3, (long)z3, clipflags);

			for (int j = 0; j < 3; j++)
			{
				v[j].color = RGBA(col, col, col, 0xFF);
				v[j].specular = 0xFF000000;
			}

			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 10];
			tex.drawtype = 5;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			u1 = sprite->x1;
			u2 = sprite->x2;
			v1 = sprite->y1;
			v2 = sprite->y2;
			tex.u1 = u1;
			tex.v1 = v1;	//top left
			tex.u2 = u2;
			tex.v2 = v1;	//top right
			tex.u3 = u1;
			tex.v3 = v2;	//bottom left
			AddTriSorted(v, 0, 1, 2, &tex, 1);
		}
	}
}

void GetProperFootPos(PHD_VECTOR* pos)
{
	FOOTPRINT* print;
	FLOOR_INFO* floor;
	PHD_VECTOR left_foot;
	PHD_VECTOR right_foot;
	long height;
	short room_number, frame, base;

	left_foot.x = 0;
	left_foot.y = 0;
	left_foot.z = 0;
	GetLaraJointPos(&left_foot, LMX_FOOT_L);

	right_foot.x = 0;
	right_foot.y = 0;
	right_foot.z = 0;
	GetLaraJointPos(&right_foot, LMX_FOOT_R);

	frame = lara_item->frame_number;
	base = anims[lara_item->anim_number].frame_base;

	switch (lara_item->anim_number)
	{
	case 7:	//run to walk right
		pos->x = right_foot.x;
		pos->y = right_foot.y;
		pos->z = right_foot.z;
		break;

	case 10:	//run -> stop right foot

		if (frame == base + 7)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}
		else if (frame == base + 12)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}

		break;

	case 12:	//turn right slow

		if (frame == base + 3)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}
		else if (frame == base + 17)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}

		break;

	case 13:	//turn left slow

		if (frame == base + 13)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}
		else if (frame == base + 27)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}

		break;

	case 24:	//hard land

		if (frame == base + 34)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 39)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 31:	//land from standing jump

		if (frame == base + 15)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 19)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 38:	//walk back end right
		pos->x = right_foot.x;	//right foot
		pos->y = right_foot.y;
		pos->z = right_foot.z;
		break;

	case 39:	//walk back end left
		pos->x = left_foot.x;	//left foot
		pos->y = left_foot.y;
		pos->z = left_foot.z;
		break;

	case 40:	//walk back

		if (frame == base + 14)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 44)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 44:	//fast turn right

		if (frame == base + 9)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 20)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 65:	//sidestep left

		if (frame == base + 1)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 17)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 67:	//sidestep right

		if (frame == base + 14)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else if (frame == base + 24)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	case 69:	//turn left fast

		if (frame == base + 12)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}
		else if (frame == base + 20)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}

		break;

	case 82:	//soft land, both feet
		pos->x = right_foot.x;
		pos->y = right_foot.y;
		pos->z = right_foot.z;
		room_number = lara_item->room_number;
		floor = GetFloor(pos->x, pos->y, pos->z, &room_number);

		if (floor->fx < 3 && !OnObject)	//add right foot print
		{
			print = &FootPrint[FootPrintNum];
			print->x = pos->x;
			print->y = GetHeight(floor, pos->x, pos->y, pos->z);
			print->z = pos->z;
			print->YRot = lara_item->pos.y_rot;
			print->Active = 512;
			FootPrintNum = FootPrintNum + 1 & 0x1F;
		}

		pos->x = left_foot.x;	//now left foot
		pos->y = left_foot.y;
		pos->z = left_foot.z;
		break;

	case 97:	//climb up

		if (frame == base + 52)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
			
		}
		else if (frame == base + 69)
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}
		else if (frame == base + 73)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}

		break;

	case 159:	//handstand

		if (frame == base + 157)
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;

	default:
		room_number = lara_item->room_number;
		floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
		height = GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

		if (abs(left_foot.y - height) < abs(right_foot.y - height))
		{
			pos->x = left_foot.x;	//left foot
			pos->y = left_foot.y;
			pos->z = left_foot.z;
		}
		else
		{
			pos->x = right_foot.x;	//right foot
			pos->y = right_foot.y;
			pos->z = right_foot.z;
		}

		break;
	}
}
