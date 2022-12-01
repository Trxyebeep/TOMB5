#include "../tomb5/pch.h"
#include "specificfx.h"
#include "3dmath.h"
#include "../game/delstuff.h"
#include "../game/control.h"
#include "../specific/function_table.h"
#include "../game/objects.h"
#include "polyinsert.h"
#include "drawroom.h"
#include "d3dmatrix.h"
#include "function_stubs.h"
#include "../game/tomb4fx.h"
#include "winmain.h"
#include "mmx.h"

#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#include "../tomb5/dynamicshadows.h"

#define CIRCUMFERENCE_POINTS 32 // Number of points in the circumference
#endif
#include "profiler.h"

#define LINE_POINTS	4	//number of points in each grid line
#define POINT_HEIGHT_CORRECTION	196	//if the difference between the floor below Lara and the floor height below the point is greater than this value, point height is corrected to lara's floor level.
#define NUM_TRIS	14	//number of triangles needed to create the shadow (this depends on what shape you're doing)
#define GRID_POINTS	(LINE_POINTS * LINE_POINTS)	//number of points in the whole grid

long ShadowTable[NUM_TRIS * 3] =	//num of triangles * 3 points
{
	//shadow is split in 3 parts. top, middle, bottom, each part made of triangles. first 4 tris are the top part,
	//the following 6 are the middle part, and the last 4 are the bottom part.

	//tris for each part go left to right. i.e the first line for the top part is the leftmost tri, 4th one is the rightmost, and so on
	//but this isn't a hard rule at all, it's just how Core did it
/*
	the default shadow grid is 4 x 4 points
	0	1	2	3

	4	5	6	7

	8	9	10	11

	12	13	14	15

	the values here are which grid points the tri points are at.
	for example, the first tri, 4, 1, 5. connect the dots. 4 -> 1 -> 5
	which makes the top left tri.
	and so on.
*/
4, 1, 5,
5, 1, 6,	//top part
6, 1, 2,
6, 2, 7,
//
8, 4, 9,
9, 4, 5,
9, 5, 10,	//middle part
10, 5, 6,
10, 6, 11,
11, 6, 7,
//
13, 8, 9,
13, 9, 14,	//bottom part
14, 9, 10,
14, 10, 11
};

uchar TargetGraphColTab[48] =
{
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	0, 0, 255,
	0, 0, 255,
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	255, 255, 0,
	255, 255, 0
};

float SnowSizes[32]
{
	-24.0F, -24.0F, -24.0F, 24.0F, 24.0F, -24.0F, 24.0F, 24.0F, -12.0F, -12.0F, -12.0F, 12.0F, 12.0F, -12.0F, 12.0F, 12.0F,
	-8.0F, -8.0F, -8.0F, 8.0F, 8.0F, -8.0F, 8.0F, 8.0F, -6.0F, -6.0F, -6.0F, 6.0F, 6.0F, -6.0F, 6.0F, 6.0F
};

#ifdef GENERAL_FIXES
static void S_PrintCircleShadow(short size, short* box, ITEM_INFO* item)
{
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	FVECTOR cv[CIRCUMFERENCE_POINTS];
	PHD_VECTOR cp[CIRCUMFERENCE_POINTS];
	FVECTOR ccv;
	PHD_VECTOR ccp;
	float fx, fy, fz;
	long x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number;

	xSize = size * (box[1] - box[0]) / 192;	//x size of grid
	zSize = size * (box[5] - box[4]) / 192;	//z size of grid
	xDist = xSize / LINE_POINTS;			//distance between each point of the grid on X
	zDist = zSize / LINE_POINTS;			//distance between each point of the grid on Z
	x = xDist + (xDist >> 1);
	z = zDist + (zDist >> 1);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		cp[i].x = x * phd_sin(65536 * i / CIRCUMFERENCE_POINTS) >> 14;
		cp[i].z = z * phd_cos(65536 * i / CIRCUMFERENCE_POINTS) >> 14;
		cv[i].x = (float)cp[i].x;
		cv[i].z = (float)cp[i].z;
	}

	phd_PushUnitMatrix();

	if (item == lara_item)	//position the grid
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LM_TORSO);
		room_number = lara_item->room_number;
		y = GetHeight(GetFloor(pos.x, pos.y, pos.z, &room_number), pos.x, pos.y, pos.z);

		if (y == NO_HEIGHT)
			y = item->floor;
	}
	else
	{
		pos.x = item->pos.x_pos;
		y = item->floor;
		pos.z = item->pos.z_pos;
	}

	y -= 16;
	phd_TranslateRel(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);	//rot the grid to correct Y

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		x = cp[i].x;
		z = cp[i].z;
		cp[i].x = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> 14;
		cp[i].z = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> 14;
	}

	ccp.x = phd_mxptr[M03] >> 14;
	ccp.z = phd_mxptr[M23] >> 14;
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		room_number = item->room_number;
		cp[i].y = GetHeight(GetFloor(cp[i].x, item->floor, cp[i].z, &room_number), cp[i].x, item->floor, cp[i].z);

		if (ABS(cp[i].y - item->floor) > POINT_HEIGHT_CORRECTION)
			cp[i].y = item->floor;
	}

	room_number = item->room_number;
	ccp.y = GetHeight(GetFloor(ccp.x, item->floor, ccp.z, &room_number), ccp.x, item->floor, ccp.z);

	if (ABS(ccp.y - item->floor) > POINT_HEIGHT_CORRECTION)
		ccp.y = item->floor;

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		fx = cv[i].x;
		fy = (float)(cp[i].y - item->floor);
		fz = cv[i].z;
		cv[i].x = aMXPtr[M00] * fx + aMXPtr[M01] * fy + aMXPtr[M02] * fz + aMXPtr[M03];
		cv[i].y = aMXPtr[M10] * fx + aMXPtr[M11] * fy + aMXPtr[M12] * fz + aMXPtr[M13];
		cv[i].z = aMXPtr[M20] * fx + aMXPtr[M21] * fy + aMXPtr[M22] * fz + aMXPtr[M23];
	}

	fy = (float)(ccp.y - item->floor);
	ccv.x = aMXPtr[M01] * fy + aMXPtr[M03];
	ccv.y = aMXPtr[M11] * fy + aMXPtr[M13];
	ccv.z = aMXPtr[M21] * fy + aMXPtr[M23];
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++) // Draw the pizza
	{
		x1 = (long)cv[i].x;
		y1 = (long)cv[i].y;
		z1 = (long)cv[i].z;
		x2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].x;
		y2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].y;
		z2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].z;
		x3 = (long)ccv.x;
		y3 = (long)ccv.y;
		z3 = (long)ccv.z;
		setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);

		if (tomb5.shadow_mode == 3)	//psx like?
		{
			v[0].color = 0x00000000;
			v[1].color = 0x00000000;
			v[2].color = 0xFF000000;
		}
		else
		{
			v[0].color = 0x4F000000;
			v[1].color = 0x4F000000;
			v[2].color = 0x4F000000;
		}

		if (item->after_death)
		{
			if (tomb5.shadow_mode == 3)	//to stop PSX shadow from turning to a flat circle when entities are dying
				v[2].color = 0x80000000 - (item->after_death << 24);
			else
			{
				v[0].color = 0x80000000 - (item->after_death << 24);
				v[1].color = v[0].color;
				v[2].color = v[0].color;
			}
		}

		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		Tex.flag = 0;
		Tex.tpage = 0;
		Tex.drawtype = 3;
		Tex.u1 = 0;
		Tex.v1 = 0;
		Tex.u2 = 0;
		Tex.v2 = 0;
		Tex.u3 = 0;
		Tex.v3 = 0;
		Tex.u4 = 0;
		Tex.v4 = 0;
		AddTriSorted(v, 0, 1, 2, &Tex, 1);
	}
}

static void S_PrintSpriteShadow(short size, short* box, ITEM_INFO* item)
{
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[4];
	FVECTOR pos;
	float xSize, zSize, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
	long opt;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
	xSize = float(size * (box[1] - box[0]) / 160) / 2;
	zSize = float(size * (box[5] - box[4]) / 160) / 2;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->floor, item->pos.z_pos);
	phd_RotY(item->pos.y_rot);

	pos.x = -xSize;
	pos.y = -16;
	pos.z = zSize;
	x1 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
	y1 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
	z1 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

	pos.x = xSize;
	pos.y = -16;
	pos.z = zSize;
	x2 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
	y2 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
	z2 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

	pos.x = xSize;
	pos.y = -16;
	pos.z = -zSize;
	x3 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
	y3 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
	z3 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];

	pos.x = -xSize;
	pos.y = -16;
	pos.z = -zSize;
	x4 = aMXPtr[M00] * pos.x + aMXPtr[M01] * pos.y + aMXPtr[M02] * pos.z + aMXPtr[M03];
	y4 = aMXPtr[M10] * pos.x + aMXPtr[M11] * pos.y + aMXPtr[M12] * pos.z + aMXPtr[M13];
	z4 = aMXPtr[M20] * pos.x + aMXPtr[M21] * pos.y + aMXPtr[M22] * pos.z + aMXPtr[M23];
	phd_PopMatrix();

	setXYZ4(v, (long)x1, (long)y1, (long)z1, (long)x2, (long)y2, (long)z2, (long)x3, (long)y3, (long)z3, (long)x4, (long)y4, (long)z4, clipflags);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFF3C3C3C;
		v[i].specular = 0xFF000000;
	}

	Tex.drawtype = 5;
	Tex.flag = 0;
	Tex.tpage = sprite->tpage;
	Tex.u1 = sprite->x2;
	Tex.v1 = sprite->y2;
	Tex.u2 = sprite->x1;
	Tex.v2 = sprite->y2;
	Tex.u3 = sprite->x1;
	Tex.v3 = sprite->y1;
	Tex.u4 = sprite->x2;
	Tex.v4 = sprite->y1;
	opt = nPolyType;
	nPolyType = 6;
	AddQuadSorted(v, 0, 1, 2, 3, &Tex, 0);
	nPolyType = opt;
}
#endif

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	float* sXYZ;
	long* hXZ;
	long* hY;
	float sxyz[GRID_POINTS * 3];
	long hxz[GRID_POINTS * 2];
	long hy[GRID_POINTS];
	long triA, triB, triC;
	float fx, fy, fz;
	long x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number;

#ifdef GENERAL_FIXES
	if (tomb5.shadow_mode != 1)
	{
		if (tomb5.shadow_mode == 5)
			DrawDynamicShadow(item);
		else if (tomb5.shadow_mode == 4)
			S_PrintSpriteShadow(size, box, item);
		else
			S_PrintCircleShadow(size, box, item);

		return;
	}
#endif

	xSize = size * (box[1] - box[0]) / 192;	//x size of grid
	zSize = size * (box[5] - box[4]) / 192;	//z size of grid
	xDist = xSize / LINE_POINTS;			//distance between each point of the grid on X
	zDist = zSize / LINE_POINTS;			//distance between each point of the grid on Z
	x = -xDist - (xDist >> 1);				//idfk
	z = zDist + (zDist >> 1);
	sXYZ = sxyz;
	hXZ = hxz;

	for (int i = 0; i < LINE_POINTS; i++, z -= zDist)
	{
		for (int j = 0; j < LINE_POINTS; j++, sXYZ += 3, hXZ += 2, x += xDist)
		{
			sXYZ[0] = (float)x;		//fill shadow XYZ array with the points of the grid
			sXYZ[2] = (float)z;
			hXZ[0] = x;				//fill height XZ array with the points of the grid
			hXZ[1] = z;
		}

		x = -xDist - (xDist >> 1);
	}

	phd_PushUnitMatrix();

	if (item == lara_item)	//position the grid
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LM_TORSO);
		room_number = lara_item->room_number;
		y = GetHeight(GetFloor(pos.x, pos.y, pos.z, &room_number), pos.x, pos.y, pos.z);

		if (y == NO_HEIGHT)
			y = item->floor;
	}
	else
	{
		pos.x = item->pos.x_pos;
		y = item->floor;
		pos.z = item->pos.z_pos;
	}

	y -= 16;
	phd_TranslateRel(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);	//rot the grid to correct Y
	hXZ = hxz;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2)
	{
		x = hXZ[0];
		z = hXZ[1];
		hXZ[0] = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> 14;
		hXZ[1] = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> 14;
	}

	phd_PopMatrix();

	hXZ = hxz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2, hY++)	//Get height on each grid point and store it in hy array
	{
		room_number = item->room_number;
		*hY = GetHeight(GetFloor(hXZ[0], item->floor, hXZ[1], &room_number), hXZ[0], item->floor, hXZ[1]);

		if (ABS(*hY - item->floor) > POINT_HEIGHT_CORRECTION)
			*hY = item->floor;
	}

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);
	sXYZ = sxyz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, sXYZ += 3, hY++)
	{
		fx = sXYZ[0];
		fy = (float)(*hY - item->floor);
		fz = sXYZ[2];
		sXYZ[0] = aMXPtr[M00] * fx + aMXPtr[M01] * fy + aMXPtr[M02] * fz + aMXPtr[M03];
		sXYZ[1] = aMXPtr[M10] * fx + aMXPtr[M11] * fy + aMXPtr[M12] * fz + aMXPtr[M13];
		sXYZ[2] = aMXPtr[M20] * fx + aMXPtr[M21] * fy + aMXPtr[M22] * fz + aMXPtr[M23];
	}

	phd_PopMatrix();
	sXYZ = sxyz;

	for (int i = 0; i < NUM_TRIS; i++)	//draw triangles
	{
		triA = 3 * ShadowTable[(i * 3) + 0];	//get tri points
		triB = 3 * ShadowTable[(i * 3) + 1];
		triC = 3 * ShadowTable[(i * 3) + 2];
		x1 = (long)sXYZ[triA + 0];
		y1 = (long)sXYZ[triA + 1];
		z1 = (long)sXYZ[triA + 2];
		x2 = (long)sXYZ[triB + 0];
		y2 = (long)sXYZ[triB + 1];
		z2 = (long)sXYZ[triB + 2];
		x3 = (long)sXYZ[triC + 0];
		y3 = (long)sXYZ[triC + 1];
		z3 = (long)sXYZ[triC + 2];
		setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);
		v[0].color = 0x4F000000;
		v[1].color = 0x4F000000;
		v[2].color = 0x4F000000;

		if (item->after_death)
		{
			v[0].color = 0x80000000 - (item->after_death << 24);
			v[1].color = v[0].color;
			v[2].color = v[0].color;
		}

		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		Tex.flag = 0;
		Tex.tpage = 0;
		Tex.drawtype = 3;
		Tex.u1 = 0;
		Tex.v1 = 0;
		Tex.u2 = 0;
		Tex.v2 = 0;
		Tex.u3 = 0;
		Tex.v3 = 0;
		Tex.u4 = 0;
		Tex.v4 = 0;
		AddTriSorted(v, 0, 1, 2, &Tex, 1);
	}
}

void DrawLaserSightSprite()
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
#ifdef GENERAL_FIXES
	FVECTOR vec;
#else
	PHD_VECTOR vec;
#endif
	long* Z;
	short* pos;
	short* XY;
	float zv;
	short size;

	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	pos = (short*)&scratchpad[512];
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	pos[0] = short(LaserSightX - lara_item->pos.x_pos);
	pos[1] = short(LaserSightY - lara_item->pos.y_pos);
	pos[2] = short(LaserSightZ - lara_item->pos.z_pos);

#ifdef GENERAL_FIXES
	vec.x = aMXPtr[M00] * pos[0] + aMXPtr[M01] * pos[1] + aMXPtr[M02] * pos[2] + aMXPtr[M03];
	vec.y = aMXPtr[M10] * pos[0] + aMXPtr[M11] * pos[1] + aMXPtr[M12] * pos[2] + aMXPtr[M13];
	vec.z = aMXPtr[M20] * pos[0] + aMXPtr[M21] * pos[1] + aMXPtr[M22] * pos[2] + aMXPtr[M23];
	zv = f_persp / vec.z;
	XY[0] = short(float(vec.x * zv + f_centerx));
	XY[1] = short(float(vec.y * zv + f_centery));
	Z[0] = (long)vec.z;
#else
	vec.x = phd_mxptr[M00] * pos[0] + phd_mxptr[M01] * pos[1] + phd_mxptr[M02] * pos[2] + phd_mxptr[M03];
	vec.y = phd_mxptr[M10] * pos[0] + phd_mxptr[M11] * pos[1] + phd_mxptr[M12] * pos[2] + phd_mxptr[M13];
	vec.z = phd_mxptr[M20] * pos[0] + phd_mxptr[M21] * pos[1] + phd_mxptr[M22] * pos[2] + phd_mxptr[M23];
	zv = f_persp / (float)vec.z;
	XY[0] = short(float(vec.x * zv + f_centerx));
	XY[1] = short(float(vec.y * zv + f_centery));
	Z[0] = vec.z >> 14;
#endif

	phd_PopMatrix();

#ifdef GENERAL_FIXES	//restore the target sprite
	if (LaserSightCol)
	{
		size = (GlobalCounter & 7) + 16;
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 18];
	}
	else
	{
		size = 4;
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	}
#else
	size = 2;
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
#endif

	setXY4(v, XY[0] - size, XY[1] - size, XY[0] + size, XY[1] - size, XY[0] - size,
		XY[1] + size, XY[0] + size, XY[1] + size, (long)f_mznear, clipflags);

	v[0].color = LaserSightCol ? 0x0000FF00 : 0x00FF0000;
	v[1].color = v[0].color;
	v[2].color = v[0].color;
	v[3].color = v[0].color;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;

	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x2;
	tex.v1 = sprite->y2;
	tex.u2 = sprite->x1;
	tex.v2 = sprite->y2;
	tex.u3 = sprite->x1;
	tex.v3 = sprite->y1;
	tex.u4 = sprite->x2;
	tex.v4 = sprite->y1;
#ifdef GENERAL_FIXES
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 0);
#else
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
#endif
	LaserSightCol = 0;
	LaserSightActive = 0;
}

#ifdef USE_SKY_SPRITE
void SetSkyCoords(FVECTOR* vec, long segment, long def)
{
	if (segment == 1)	//bottom left
	{
		switch (def)
		{
		case 0:	//bottom left
			SetVecXYZ(0, -4864, 0, -2432);
			SetVecXYZ(1, -2432, 0, -2432);
			SetVecXYZ(2, -2432, 0, -4864);
			SetVecXYZ(3, -4864, 0, -4864);
			break;

		case 1:	//bottom right
			SetVecXYZ(0, -2432, 0, -2432);
			SetVecXYZ(1, 0, 0, -2432);
			SetVecXYZ(2, 0, 0, -4864);
			SetVecXYZ(3, -2432, 0, -4864);
			break;

		case 2:	//top left
			SetVecXYZ(0, -4864, 0, 0);
			SetVecXYZ(1, -2432, 0, 0);
			SetVecXYZ(2, -2432, 0, -2432);
			SetVecXYZ(3, -4864, 0, -2432);
			break;

		case 3:	//top right
			SetVecXYZ(0, -2432, 0, 0);
			SetVecXYZ(1, 0, 0, 0);
			SetVecXYZ(2, 0, 0, -2432);
			SetVecXYZ(3, -2432, 0, -2432);
			break;
		}
	}
	else if (segment == 2)	//bottom right
	{
		switch (def)
		{
		case 0:	//bottom left
			SetVecXYZ(0, 0, 0, -2432);
			SetVecXYZ(1, 2432, 0, -2432);
			SetVecXYZ(2, 2432, 0, -4864);
			SetVecXYZ(3, 0, 0, -4864);
			break;

		case 1:	//bottom right
			SetVecXYZ(0, 2432, 0, -2432);
			SetVecXYZ(1, 4864, 0, -2432);
			SetVecXYZ(2, 4864, 0, -4864);
			SetVecXYZ(3, 2432, 0, -4864);
			break;

		case 2:	//top left
			SetVecXYZ(0, 0, 0, 0);
			SetVecXYZ(1, 2432, 0, 0);
			SetVecXYZ(2, 2432, 0, -2432);
			SetVecXYZ(3, 0, 0, -2432);
			break;

		case 3:	//top right
			SetVecXYZ(0, 2432, 0, 0);
			SetVecXYZ(1, 4864, 0, 0);
			SetVecXYZ(2, 4864, 0, -2432);
			SetVecXYZ(3, 2432, 0, -2432);
			break;
		}
	}
	else if (segment == 3)	//top right
	{
		switch (def)
		{
		case 0:	//bottom left
			SetVecXYZ(0, 0, 0, 2432);
			SetVecXYZ(1, 2432, 0, 2432);
			SetVecXYZ(2, 2432, 0, 0);
			SetVecXYZ(3, 0, 0, 0);
			break;

		case 1:	//bottom right
			SetVecXYZ(0, 2432, 0, 2432);
			SetVecXYZ(1, 4864, 0, 2432);
			SetVecXYZ(2, 4864, 0, 0);
			SetVecXYZ(3, 2432, 0, 0);
			break;

		case 2:	//top left
			SetVecXYZ(0, 0, 0, 4864);
			SetVecXYZ(1, 2432, 0, 4864);
			SetVecXYZ(2, 2432, 0, 2432);
			SetVecXYZ(3, 0, 0, 2432);
			break;

		case 3:	//top right
			SetVecXYZ(0, 2432, 0, 4864);
			SetVecXYZ(1, 4864, 0, 4864);
			SetVecXYZ(2, 4864, 0, 2432);
			SetVecXYZ(3, 2432, 0, 2432);
			break;
		}
	}
	else if (segment == 4)	//top left
	{
		switch (def)
		{
		case 0:	//bottom left
			SetVecXYZ(0, -4864, 0, 2432);
			SetVecXYZ(1, -2432, 0, 2432);
			SetVecXYZ(2, -2432, 0, 0);
			SetVecXYZ(3, -4864, 0, 0);
			break;

		case 1:	//bottom right
			SetVecXYZ(0, -2432, 0, 2432);
			SetVecXYZ(1, 0, 0, 2432);
			SetVecXYZ(2, 0, 0, 0);
			SetVecXYZ(3, -2432, 0, 0);
			break;

		case 2:	//top left
			SetVecXYZ(0, -4864, 0, 4864);
			SetVecXYZ(1, -2432, 0, 4864);
			SetVecXYZ(2, -2432, 0, 2432);
			SetVecXYZ(3, -4864, 0, 2432);
			break;

		case 3:	//top right
			SetVecXYZ(0, -2432, 0, 4864);
			SetVecXYZ(1, 0, 0, 4864);
			SetVecXYZ(2, 0, 0, 2432);
			SetVecXYZ(3, -2432, 0, 2432);
			break;
		}
	}
}

void DrawSkySegment(ulong color, long drawtype, long def, long seg, long zpos, long ypos)
{
	SPRITESTRUCT* sprite;
	FVECTOR vec[4];
	D3DTLVERTEX v[4];
	TEXTURESTRUCT Tex;
	short* clip;
	float perspz, u1, v1, u2, v2, x, y, z;
	short clipdistance;

	phd_PushMatrix();

	if (gfCurrentLevel)
		phd_TranslateRel(zpos, ypos, 0);
	else
		phd_TranslateRel(0, ypos, 0);

	SetSkyCoords(vec, seg, def);

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
		vec[i].y = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
		vec[i].z = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
		v[i].color = color | 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;

	for (int i = 0; i < 4; i++, clip++)
	{
		v[i].tu = (float)vec[i].x;
		v[i].tv = (float)vec[i].y;
		v[i].sz = (float)vec[i].z;
		clipdistance = 0;

		if (v[i].sz < f_mznear)
			clipdistance = -128;
		else
		{
			perspz = f_mpersp / v[i].sz;

			if (v[i].sz > FogEnd)
			{
				v[i].sz = f_zfar;
				clipdistance = 256;
			}

			v[i].sx = perspz * v[i].tu + f_centerx;
			v[i].sy = perspz * v[i].tv + f_centery;
			v[i].rhw = perspz * f_moneopersp;

			if (v[i].sx < phd_winxmin)
				clipdistance++;
			else if (phd_winxmax < v[i].sx)
				clipdistance += 2;

			if (v[i].sy < phd_winymin)
				clipdistance += 4;
			else if (v[i].sy > phd_winymax)
				clipdistance += 8;
		}

		clip[0] = clipdistance;
	}

	sprite = &spriteinfo[objects[SKY_GRAPHICS].mesh_index + def];
	Tex.drawtype = (ushort)drawtype;
	Tex.flag = 0;
	Tex.tpage = sprite->tpage;
	u1 = sprite->x2;
	u2 = sprite->x1;
	v1 = sprite->y2;
	v2 = sprite->y1;
	Tex.u1 = u1;
	Tex.v1 = v1;
	Tex.u2 = u2;
	Tex.v2 = v1;
	Tex.u3 = u2;
	Tex.v3 = v2;
	Tex.u4 = u1;
	Tex.v4 = v2;
	AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);
	phd_TranslateRel(-9728, 0, 0);
	SetSkyCoords(vec, seg, def);

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
		vec[i].y = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
		vec[i].z = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
		v[i].color |= 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;

	for (int i = 0; i < 4; i++, clip++)
	{
		v[i].tu = (float)vec[i].x;
		v[i].tv = (float)vec[i].y;
		v[i].sz = (float)vec[i].z;
		clipdistance = 0;

		if (v[i].sz < f_mznear)
			clipdistance = -128;
		else
		{
			perspz = f_mpersp / v[i].sz;

			if (v[i].sz > FogEnd)
			{
				v[i].sz = f_zfar;
				clipdistance = 256;
			}

			v[i].sx = perspz * v[i].tu + f_centerx;
			v[i].sy = perspz * v[i].tv + f_centery;
			v[i].rhw = perspz * f_moneopersp;

			if (v[i].sx < phd_winxmin)
				clipdistance++;
			else if (phd_winxmax < v[i].sx)
				clipdistance += 2;

			if (v[i].sy < phd_winymin)
				clipdistance += 4;
			else if (v[i].sy > phd_winymax)
				clipdistance += 8;
		}

		clip[0] = clipdistance;
	}

	if (gfCurrentLevel != 0)
		AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);

	phd_PopMatrix();
}

void DrawPSXSky(ulong color, long zpos, long ypos, long drawtype)
{
	for (int i = 1; i < 5; i++)
		for (int j = 0; j < 4; j++)
			DrawSkySegment(color, drawtype, j, i, zpos, ypos);
}
#endif
void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype)
{
	PHD_VECTOR vec[4];
	D3DTLVERTEX v[4];
	TEXTURESTRUCT Tex;
	short* clip;
	long x, y, z;

#ifdef GENERAL_FIXES
	if (tomb5.PSX_skies)
	{
		DrawPSXSky(color, zpos, ypos, drawtype);
		return;
	}
#endif

	phd_PushMatrix();

	if (gfCurrentLevel)
		phd_TranslateRel(zpos, ypos, 0);
	else
		phd_TranslateRel(0, ypos, 0);

	vec[0].x = -4864;
	vec[0].y = 0;
	vec[0].z = 4864;
	vec[1].x = 4864;
	vec[1].y = 0;
	vec[1].z = 4864;
	vec[2].x = 4864;
	vec[2].y = 0;
	vec[2].z = -4864;
	vec[3].x = -4864;
	vec[3].y = 0;
	vec[3].z = -4864;

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> 14;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> 14;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> 14;
		v[i].color = color | 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], (float)vec[0].x, (float)vec[0].y, (float)vec[0].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[1], (float)vec[1].x, (float)vec[1].y, (float)vec[1].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[2], (float)vec[2].x, (float)vec[2].y, (float)vec[2].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[3], (float)vec[3].x, (float)vec[3].y, (float)vec[3].z, clip);	//the only one that survived
	Tex.drawtype = (ushort)drawtype;
	Tex.flag = 0;
	Tex.tpage = ushort(nTextures - 1);
	Tex.u1 = 0.0;
	Tex.v1 = 0.0;
	Tex.u2 = 1.0;
	Tex.v2 = 0.0;
	Tex.u3 = 1.0;
	Tex.v3 = 1.0;
	Tex.u4 = 0.0;
	Tex.v4 = 1.0;
	AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);
	phd_TranslateRel(-9728, 0, 0);
	vec[0].x = -4864;
	vec[0].y = 0;
	vec[0].z = 4864;
	vec[1].x = 4864;
	vec[1].y = 0;
	vec[1].z = 4864;
	vec[2].x = 4864;
	vec[2].y = 0;
	vec[2].z = -4864;
	vec[3].x = -4864;
	vec[3].y = 0;
	vec[3].z = -4864;

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> 14;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> 14;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> 14;
		v[i].color |= 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], (float)vec[0].x, (float)vec[0].y, (float)vec[0].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[1], (float)vec[1].x, (float)vec[1].y, (float)vec[1].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[2], (float)vec[2].x, (float)vec[2].y, (float)vec[2].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[3], (float)vec[3].x, (float)vec[3].y, (float)vec[3].z, clip);	//the only one that survived

	if (gfCurrentLevel)
		AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);

	phd_PopMatrix();
}

void S_DrawDarts(ITEM_INFO* item)
{
	D3DTLVERTEX v[2];
	long x1, y1, z1, x2, y2, z2, num, mxx, mxy, mxz, xx, yy, zz;
	float zv;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	zv = f_persp / (float)phd_mxptr[M23];
	x1 = (short)((float)(phd_mxptr[M03] * zv + f_centerx));
	y1 = (short)((float)(phd_mxptr[M13] * zv + f_centery));
	z1 = phd_mxptr[M23] >> 14;
	num = (-96 * phd_cos(item->pos.x_rot)) >> 14;
	mxx = (num * phd_sin(item->pos.y_rot)) >> 14;
	mxy = (96 * phd_sin(item->pos.x_rot)) >> 14;
	mxz = (num * phd_cos(item->pos.y_rot)) >> 14;
	xx = phd_mxptr[M00] * mxx + phd_mxptr[M01] * mxy + phd_mxptr[M02] * mxz + phd_mxptr[M03];
	yy = phd_mxptr[M10] * mxx + phd_mxptr[M11] * mxy + phd_mxptr[M12] * mxz + phd_mxptr[M13];
	zz = phd_mxptr[M20] * mxx + phd_mxptr[M21] * mxy + phd_mxptr[M22] * mxz + phd_mxptr[M23];
	zv = f_persp / (float)zz;
	x2 = (short)((float)(xx * zv + f_centerx));
	y2 = (short)((float)(yy * zv + f_centery));
	z2 = zz >> 14;

	if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
	{
		zv = f_mpersp / (float)z1 * f_moneopersp;
		v[0].color = 0xFF000000;
		v[1].color = 0xFF783C14;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[0].sx = (float)x1;
		v[1].sx = (float)x2;
		v[0].sy = (float)y1;
		v[1].sy = (float)y2;
		v[0].sz = f_a - zv * f_boo;
		v[1].sz = f_a - zv * f_boo;
		v[0].rhw = zv;
		v[1].rhw = zv;
		AddLineSorted(v, &v[1], 6);
	}

	phd_PopMatrix();
}

void DrawMoon()
{
	D3DTLVERTEX v[4];
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT tex;
	SVECTOR vec;
	short* c;
	float x1, x2, y1, y2, z;
	ushort tpage;

	c = clipflags;
	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];
	tpage = sprite->tpage < nTextures ? sprite->tpage : 0;
	phd_PushMatrix();
	aSetViewMatrix();
	D3DMView._41 = 0;
	D3DMView._42 = 0;
	D3DMView._43 = 0;
	vec.vx = 0;
	vec.vy = -1024;
	vec.vz = 1920;
	aTransformPerspSV(&vec, v, c, 1, 0);

	if (*c >= 0)
	{
		x1 = v[0].sx - 48.0F;
		x2 = v[0].sx + 48.0F;
		y1 = v[0].sy - 48.0F;
		y2 = v[0].sy + 48.0F;
#ifdef GENERAL_FIXES
		z = f_mzfar;
#else
		z = f_mzfar - 1024.0F;
#endif
		aSetXY4(v, x1, y1, x2, y1, x1, y2, x2, y2, z, c);
		v[0].color = 0xC0E0FF;
		v[1].color = 0xC0E0FF;
		v[2].color = 0xC0E0FF;
		v[3].color = 0xC0E0FF;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		tex.tpage = tpage;
#ifdef GENERAL_FIXES
		tex.drawtype = 2;
		AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);
#else
		tex.drawtype = 0;
		AddQuadZBuffer(v, 0, 1, 3, 2, &tex, 1);
#endif
	}

	phd_PopMatrix();
}

void DrawGasCloud(ITEM_INFO* item)
{
	GAS_CLOUD* cloud;
	long num;

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags < 2)
	{
		cloud = (GAS_CLOUD*)item->data;

		if (!cloud->mTime)
			cloud->yo = -6144.0F;

		TriggerFogBulbFX(0, 128, 0, item->pos.x_pos, (long)(item->pos.y_pos + cloud->yo), item->pos.z_pos, 4096, 40);

		if (cloud->yo >= -3584.0)
		{
			if (cloud->sTime == 32)
			{
				do num = rand() & 7; while (num == cloud->num);
				cloud->num = num;
			}
			else if (cloud->sTime > 32)
			{
				num = cloud->sTime - 32;

				if (num > 128)
				{
					num = 256 - num;

					if (!num)
						cloud->sTime = 0;
				}

				num = 255 - (num << 1);

				if (num < 64)
					num = 64;

				TriggerFogBulbFX(0, 255, 0, item->pos.x_pos + cloud[cloud->num].t.vx, item->pos.y_pos + cloud[cloud->num].t.vy,
					item->pos.z_pos + cloud[cloud->num].t.vz, 1024, num);
			}

			cloud->sTime++;
		}
		else
			cloud->yo += 12.0F;

		cloud->mTime++;

		for (int i = 0; i < 8; i++, cloud++)//what's the point of this loop
		{
			phd_PushMatrix();
			phd_TranslateAbs(item->pos.x_pos + cloud->t.vx, item->pos.y_pos + cloud->t.vy, item->pos.z_pos + cloud->t.vz);
			phd_RotY(-CamRot.vy << 4);
			phd_RotX(-4096);
			phd_PopMatrix();
		}
	}
	else
		item->item_flags[0] = 1;
}

#ifdef GENERAL_FIXES
STARS stars[2048];

static void DrawStars()
{
	STARS* star;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	static long first_time = 0;
	float x, y, z, fx, fy, fz, bx, by, sv;
	long col, r, g, b;

	if (!first_time)
	{
		for (int i = 0; i < 2048; i++)
		{
			star = &stars[i];

			if (i > 1792)
			{
				star->pos.x = ((rand() & 0x1FF) + 32) * fSin(i * 512);
				star->pos.z = ((rand() & 0x1FF) + 32) * fCos(i * 512);
			}
			else
			{
				star->pos.x = ((rand() & 0x7FF) + 512) * fSin(i * 512);
				star->pos.z = ((rand() & 0x7FF) + 512) * fCos(i * 512);
			}

			star->pos.y = (float)(-rand() % 1900);
			star->sv = (rand() & 1) + 1.0F;
			col = rand() & 0x3F;
			star->col = RGBONLY(col + 160, col + 160, col + 160);
		}

		first_time = 1;
	}

	tex.drawtype = 0;
	tex.tpage = 0;
	tex.flag = 0;
	phd_PushMatrix();
	phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);
	SetD3DViewMatrix();
	phd_PopMatrix();
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	for (int i = 0; i < 2048; i++)
	{
		star = &stars[i];
		fx = star->pos.x;
		fy = star->pos.y;
		fz = star->pos.z;
		col = star->col;

		if (GlobalCounter & 1)
		{
			r = CLRR(col);
			g = CLRG(col);
			b = CLRB(col);
			col = GetRandomControl() & 0x3F;

			if (GetRandomControl() & 1)
				col = -col;

			r -= col;
			g -= col;
			b -= col;

			if (r > 255)
				r = 255;
			else if (r < 0)
				r = 0;

			if (g > 255)
				g = 255;
			else if (g < 0)
				g = 0;

			if (b > 255)
				b = 255;
			else if (b < 0)
				b = 0;

			col = RGBONLY(r, g, b);
			star->col = col;
		}

		sv = star->sv;
		x = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31;
		y = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32;
		z = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33;
		

		if (z >= f_mznear)
		{
			fz = f_mpersp / z;
			bx = fz * x + f_centerx;
			by = fz * y + f_centery;

			if (bx >= 0 && bx <= (float)phd_winxmax && by >= 0 && by <= (float)phd_winymax)
			{
				v[0].sx = bx;
				v[0].sy = by;
				v[0].color = col;
				v[0].specular = 0xFF000000;
				v[0].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[0].tu = 0;
				v[0].tv = 0;
				v[1].sx = bx + sv;
				v[1].sy = by;
				v[1].color = col;
				v[1].specular = 0xFF000000;
				v[1].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[1].tu = 0;
				v[1].tv = 0;
				v[2].sx = bx;
				v[2].sy = by + sv;
				v[2].color = col;
				v[2].specular = 0xFF000000;
				v[2].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[2].tu = 0;
				v[2].tv = 0;
				v[3].sx = bx + sv;
				v[3].sy = by + sv;
				v[3].color = col;
				v[3].specular = 0xFF000000;
				v[3].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[3].tu = 0;
				v[3].tv = 0;
				AddQuadZBuffer(v, 0, 1, 3, 2, &tex, 1);
			}
		}
	}
}
#endif

void DrawStarField()
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	static long first_time = 0;
	float* pPos;
	long* pCol;
	float x, y, z, fx, fy, fz, bx, by;
	long col;

#ifdef GENERAL_FIXES
	DrawStars();
	return;
#endif

	if (!first_time)
	{
		pPos = StarFieldPositions;
		pCol = StarFieldColors;

		for (int i = 0; i < 256; i++)
		{
			pPos[0] = ((rand() & 0x1FF) + 512.0F) * fSin(i * 512);
			pPos++;
			pPos[0] = (float)(-rand() % 1900);
			pPos++;
			pPos[0] = ((rand() & 0x1FF) + 512.0F) * fCos(i * 512);
			pPos++;
			pPos[0] = (rand() & 1) + 1.0F;
			pPos++;
			col = rand() & 0x7F;
			pCol[0] = RGBONLY(col + 128, col + 128, col + 192);
			pCol++;
		}

		first_time = 1;
	}

	tex.drawtype = 0;
	tex.tpage = 0;
	tex.flag = 0;
	phd_PushMatrix();
	phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);
	SetD3DViewMatrix();
	phd_PopMatrix();
	pPos = StarFieldPositions;
	pCol = StarFieldColors;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	for (int i = 0; i < 184; i++)
	{
		fx = pPos[0];
		pPos++;
		fy = pPos[0];
		pPos++;
		fz = pPos[0];
		pPos++;
		col = pCol[0];
		pCol++;
		x = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31;
		y = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32;
		z = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33;
		fy = pPos[0];
		pPos++;

		if (z >= f_mznear)
		{
			fz = f_mpersp / z;
			bx = fz * x + f_centerx;
			by = fz * y + f_centery;

			if (bx >= 0 && bx <= (float)phd_winxmax && by >= 0 && by <= (float)phd_winymax)
			{
				v[0].sx = bx;
				v[0].sy = by;
				v[0].color = col;
				v[0].specular = 0xFF000000;
				v[0].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[0].tu = 0;
				v[0].tv = 0;
				v[1].sx = bx + fy;
				v[1].sy = by;
				v[1].color = col;
				v[1].specular = 0xFF000000;
				v[1].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[1].tu = 0;
				v[1].tv = 0;
				v[2].sx = bx;
				v[2].sy = by + fy;
				v[2].color = col;
				v[2].specular = 0xFF000000;
				v[2].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[2].tu = 0;
				v[2].tv = 0;
				v[3].sx = bx + fy;
				v[3].sy = by + fy;
				v[3].color = col;
				v[3].specular = 0xFF000000;
				v[3].rhw = f_mpersp / f_mzfar * f_moneopersp;
				v[3].tu = 0;
				v[3].tv = 0;
				AddQuadZBuffer(v, 0, 1, 3, 2, &tex, 1);
			}
		}
	}
}

void setXYZ3(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[0] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[1] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	clip[2] = clip_distance;
}

void setXYZ4(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv= f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[0] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[1] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[2] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x4;
	v->tv = (float)y4;
	v->sz = (float)z4;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	clip[3] = clip_distance;
}

void setXY3(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long z, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[0] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[1] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[2] = clip_distance;
}

void setXY4(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[0] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[1] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[2] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x4;
	v->sy = (float)y4;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[3] = clip_distance;
}

void S_DrawDrawSparksNEW(SPARKS* sptr, long smallest_size, float* xyz)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float x0, y0, x1, y1, x2, y2, x3, y3;
	float fs1, fs2, sin, cos, sinf1, sinf2, cosf1, cosf2;
	long s, scale, s1, s2;

	if (!(sptr->Flags & 8))
		return;

	if (xyz[2] <= f_mznear || xyz[2] >= f_mzfar)
	{
		if (xyz[2] >= f_mzfar)
			sptr->On = 0;
	}
	else
	{
		if (sptr->Flags & 2)
		{
			scale = sptr->Size << sptr->Scalar;
			s = ((phd_persp * sptr->Size) << sptr->Scalar) / (long)xyz[2];
			s1 = s;
			s2 = s;

			if (s > scale)
				s1 = scale;
			else if (s < smallest_size)
				s1 = smallest_size;

			if (s > scale)
				s1 = scale;
			else if (s < smallest_size)
				s2 = smallest_size;
		}
		else
		{
			s1 = sptr->Size;
			s2 = s1;
		}

		fs1 = (float)s1;
		fs2 = (float)s2;

		if ((fs1 * 2) + xyz[0] >= f_left && xyz[0] - (fs1 * 2) < f_right && (fs2 * 2) + xyz[1] >= f_top && xyz[1] - (fs2 * 2) < f_bottom)
		{
			fs1 *= 0.5F;
			fs2 *= 0.5F;

			if (sptr->Flags & 0x10)
			{
				sin = fSin(sptr->RotAng << 1);
				cos = fCos(sptr->RotAng << 1);
				sinf1 = sin * fs1;
				sinf2 = sin * fs2;
				cosf1 = cos * fs1;
				cosf2 = cos * fs2;
				x0 = cosf2 - sinf1 + xyz[0];
				y0 = xyz[1] - cosf1 - sinf2;
				x1 = sinf1 + cosf2 + xyz[0];
				y1 = cosf1 + xyz[1] - sinf2;
				x2 = sinf1 - cosf2 + xyz[0];
				y2 = cosf1 + xyz[1] + sinf2;
				x3 = -sinf1 - cosf2 + xyz[0];
				y3 = xyz[1] - cosf1 + sinf2;
				aSetXY4(v, x0, y0, x1, y1, x2, y2, x3, y3, xyz[2], clipflags);
			}
			else
			{
				x0 = xyz[0] - fs1;
				x1 = fs1 + xyz[0];
				y0 = xyz[1] - fs2;
				y1 = fs2 + xyz[1];
				aSetXY4(v, x0, y0, x1, y0, x1, y1, x0, y1, xyz[2], clipflags);
			}

			sprite = &spriteinfo[sptr->Def];
			v[0].color = RGBA(sptr->R, sptr->G, sptr->B, 0xFF);
			v[1].color = v[0].color;
			v[2].color = v[0].color;
			v[3].color = v[0].color;
			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;

#ifdef GENERAL_FIXES
			if (sptr->TransType == 3)
				tex.drawtype = 5;
			else
			{
#endif
				if (sptr->TransType)
					tex.drawtype = 2;
				else
					tex.drawtype = 1;
#ifdef GENERAL_FIXES
			}
#endif

			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y1;
			tex.u2 = sprite->x2;
			tex.v2 = sprite->y1;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y2;
			tex.u4 = sprite->x1;
			tex.v4 = sprite->y2;
			AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		}
	}
}

void DoRain()
{
	RAINDROPS* rptr;
	ROOM_INFO* r;
#ifdef GENERAL_FIXES
	FLOOR_INFO* floor;
#endif
	D3DTLVERTEX v[2];
	TEXTURESTRUCT tex;
	FVECTOR vec;
	FVECTOR vec2;
	short* clip;
	float ctop, cbottom, cright, cleft, zv, zz;
	long num_alive, rad, angle, rnd, x, z, x_size, y_size, clr;
	short room_number, clip_distance;

	num_alive = 0;

	for (int i = 0; i < rain_count; i++)
	{
		rptr = &Rain[i];

		if (outside && !rptr->x && num_alive < max_rain)
		{
			num_alive++;
			rad = GetRandomDraw() & 8191;
			angle = GetRandomDraw() & 8190;
			rptr->x = camera.pos.x + (rad * rcossin_tbl[angle] >> 12);
			rptr->y = camera.pos.y + -1024 - (GetRandomDraw() & 2047);
			rptr->z = camera.pos.z + (rad * rcossin_tbl[angle + 1] >> 12);

			if (IsRoomOutside(rptr->x, rptr->y, rptr->z) < 0)
			{
				rptr->x = 0;
				continue;
			}

			if (room[IsRoomOutsideNo].flags & ROOM_UNDERWATER)
			{
				rptr->x = 0;
				continue;
			}

			rptr->xv = (GetRandomDraw() & 7) - 4;
			rptr->yv = (GetRandomDraw() & 3) + 24;
			rptr->zv = (GetRandomDraw() & 7) - 4;
			rptr->room_number = IsRoomOutsideNo;
			rptr->life = 64 - rptr->yv;
		}

		if (rptr->x)
		{
			if (rptr->life > 240 || ABS(CamPos.x - rptr->x) > 6000 || ABS(CamPos.z - rptr->z) > 6000)
			{
				rptr->x = 0;
				continue;
			}

			rptr->x += rptr->xv + 4 * SmokeWindX;
			rptr->y += rptr->yv << 3;
			rptr->z += rptr->zv + 4 * SmokeWindZ;
			r = &room[rptr->room_number];
			x = r->x + 1024;
			z = r->z + 1024;
			x_size = r->x_size - 1;
			y_size = r->y_size - 1;

			if (rptr->y <= r->maxceiling || rptr->y >= r->minfloor || rptr->z <= z ||
				rptr->z >= r->z + (x_size << 10) || rptr->x <= x || rptr->x >= r->x + (y_size << 10))
			{
				room_number = rptr->room_number;
#ifdef GENERAL_FIXES
				floor =		//only need it for Get(Water)Height
#endif
				GetFloor(rptr->x, rptr->y, rptr->z, &room_number);

				if (room_number == rptr->room_number || room[room_number].flags & ROOM_UNDERWATER)
				{
#ifdef GENERAL_FIXES
					if (room[room_number].flags & ROOM_UNDERWATER)
					{
					//	clr = GetWaterHeight(rptr->x, rptr->y, rptr->z, room_number);
					//	SetupRipple(rptr->x, clr, rptr->z, 3, 0);
					//max num of ripples is 32, this fills up very quickly and Lara produces no ripples when walking through water... 
					//increase limit someday.. nothing for now!
					}
					else
					{
						clr = GetHeight(floor, rptr->x, rptr->y, rptr->z);
						TriggerSmallSplash(rptr->x, clr, rptr->z, 1);
					}
#else
					TriggerSmallSplash(rptr->x, rptr->y, rptr->z, 1);
#endif
					rptr->x = 0;
					continue;
				}
				else
					rptr->room_number = room_number;
			}

			rnd = GetRandomDraw();

			if ((rnd & 3) != 3)
			{
				rptr->xv += (rnd & 3) - 1;

				if (rptr->xv < -4)
					rptr->xv = -4;
				else if (rptr->xv > 4)
					rptr->xv = 4;
			}

			rnd = (rnd >> 2) & 3;

			if (rnd != 3)
			{
				rptr->zv += (char)(rnd - 1);

				if (rptr->zv < -4)
					rptr->zv = -4;
				else if (rptr->zv > 4)
					rptr->zv = 4;
			}

			rptr->life -= 2;

			if (rptr->life > 240)
				rptr->x = 0;
		}
	}

	tex.drawtype = 2;
	tex.tpage = 0;
	tex.flag = 0;
	ctop = f_top;
	cleft = f_left + 4.0F;
	cbottom = f_bottom;
	cright = f_right - 4.0F;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	SetD3DViewMatrix();

	for (int i = 0; i < rain_count; i++)
	{
		rptr = &Rain[i];

		if (rptr->x)
		{
			clip_distance = 0;
			clip = clipflags;
			vec.x = (float)(rptr->x - lara_item->pos.x_pos - 2 * SmokeWindX);
			vec.y = (float)(rptr->y - 8 * rptr->yv - lara_item->pos.y_pos);
			vec.z = (float)(rptr->z - lara_item->pos.z_pos - 2 * SmokeWindZ);
			vec2.x = vec.x * D3DMView._11 + vec.y * D3DMView._21 + vec.z * D3DMView._31 + D3DMView._41;
			vec2.y = vec.x * D3DMView._12 + vec.y * D3DMView._22 + vec.z * D3DMView._32 + D3DMView._42;
			vec2.z = vec.x * D3DMView._13 + vec.y * D3DMView._23 + vec.z * D3DMView._33 + D3DMView._43;
			zz = vec2.z;
			clr = (long)((1.0F - (f_mzfar - vec2.z) * (1.0F / f_mzfar)) * 8.0F + 8.0F);
			v[0].specular = 0xFF000000;
			v[0].color = RGBA(clr, clr, clr, 128);
			v[0].tu = vec2.x;
			v[0].tv = vec2.y;

			if (vec2.z < f_mznear)
				clip_distance = -128;
			else
			{
				if (vec2.z > FogEnd)
				{
					zz = f_zfar;
					clip_distance = 16;
				}

				zv = f_mpersp / zz;
				v[0].sx = zv * vec2.x + f_centerx;
				v[0].sy = zv * vec2.y + f_centery;
				v[0].rhw = f_moneopersp * zv;

				if (v[0].sx < cleft)
					clip_distance++;
				else if (v[0].sx > cright)
					clip_distance += 2;

				if (v[0].sy < ctop)
					clip_distance += 4;
				else if (v[0].sy > cbottom)
					clip_distance += 8;
			}

			v[0].sz = zz;
			clip[0] = clip_distance;
			clip_distance = 0;
			clip++;

			vec.x = (float)(rptr->x - lara_item->pos.x_pos);
			vec.y = (float)(rptr->y - lara_item->pos.y_pos);
			vec.z = (float)(rptr->z - lara_item->pos.z_pos);
			vec2.x = vec.x * D3DMView._11 + vec.y * D3DMView._21 + vec.z * D3DMView._31 + D3DMView._41;
			vec2.y = vec.x * D3DMView._12 + vec.y * D3DMView._22 + vec.z * D3DMView._32 + D3DMView._42;
			vec2.z = vec.x * D3DMView._13 + vec.y * D3DMView._23 + vec.z * D3DMView._33 + D3DMView._43;
			clr = (long)((1.0F - (f_mzfar - vec2.z) * (1.0F / f_mzfar)) * 16.0F + 16.0F);
			v[1].specular = 0xFF000000;
			v[1].color = RGBA(clr, clr, clr, 128);
			v[1].tu = vec2.x;
			v[1].tv = vec2.y;

			if (vec2.z < f_mznear)
				clip_distance = -128;
			else
			{
				if (vec2.z > FogEnd)
				{
					zz = f_zfar;
					clip_distance = 16;
				}

				zv = f_mpersp / zz;
				v[1].sx = zv * vec2.x + f_centerx;
				v[1].sy = zv * vec2.y + f_centery;
				v[1].rhw = f_moneopersp * zv;

				if (v[1].sx < cleft)
					clip_distance++;
				else if (v[1].sx > cright)
					clip_distance += 2;

				if (v[1].sy < ctop)
					clip_distance += 4;
				else if (v[1].sy > cbottom)
					clip_distance += 8;
			}

			v[1].sz = zz;
			clip[0] = clip_distance;

			if (!clipflags[0] && !clipflags[1])
				AddPolyLine(v, &tex);
		}
	}

	phd_PopMatrix();
}

void OutputSky()
{
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
	DrawBuckets();
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
	SortPolyList(SortCount, SortList);
	RestoreFPCW(FPCW);
	MMXSetPerspecLimit(0);
	DrawSortList();
	MMXSetPerspecLimit(0x3F19999A);
	MungeFPCW(&FPCW);
	InitBuckets();
	InitialiseSortList();
}

void DoScreenFade()
{
#ifdef GENERAL_FIXES
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long a;

	a = FadeVal << 24;
	FadeVal += FadeStep;
	FadeCnt++;

	if (FadeCnt > 8)
	{
		DoFade = 2;
		a = FadeEnd << 24;
	}

	v[0].sx = 0;
	v[0].sy = 0;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = a;
	v[0].specular = 0xFF000000;

	v[1].sx = float(phd_winxmax + 1);
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = a;
	v[1].specular = 0xFF000000;

	v[2].sx = float(phd_winxmax + 1);
	v[2].sy = float(phd_winymax + 1);
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = a;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)phd_winxmin;
	v[3].sy = float(phd_winymax + 1);
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = a;
	v[3].specular = 0xFF000000;

	tex.drawtype = 3;
	tex.flag = 0;
	tex.tpage = 0;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
#else
	FadeVal += FadeStep;
	FadeCnt++;

	if (FadeCnt > 8)
		DoFade = 2;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
#endif
}

void ClipCheckPoint(D3DTLVERTEX* v, float x, float y, float z, short* clip)
{
	float perspz;
	short clipdistance;

	v->tu = x;
	v->tv = y;
	v->sz = z;
	clipdistance = 0;

	if (v->sz < f_mznear)
		clipdistance = -128;
	else
	{
		perspz = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			v->sz = f_zfar;
			clipdistance = 256;
		}

		v->sx = perspz * v->tu + f_centerx;
		v->sy = perspz * v->tv + f_centery;
		v->rhw = perspz * f_moneopersp;

		if (v->sx < phd_winxmin)
			clipdistance++;
		else if (phd_winxmax < v->sx)
			clipdistance += 2;

		if (v->sy < phd_winymin)
			clipdistance += 4;
		else if (v->sy > phd_winymax)
			clipdistance += 8;
	}

	clip[0] = clipdistance;
}

void aTransformPerspSV(SVECTOR* vec, D3DTLVERTEX* v, short* c, long nVtx, long col)
{
	float x, y, z, vx, vy, vz, zv;
	short clip;

	for (int i = 0; i < nVtx; i++)
	{
		clip = 0;
		vx = vec->vx;
		vy = vec->vy;
		vz = vec->vz;
		x = D3DMView._11 * vx + D3DMView._21 * vy + D3DMView._31 * vz + D3DMView._41;
		y = D3DMView._12 * vx + D3DMView._22 * vy + D3DMView._32 * vz + D3DMView._42;
		z = D3DMView._13 * vx + D3DMView._23 * vy + D3DMView._33 * vz + D3DMView._43;
		v->tu = x;
		v->tv = y;

		if (z < f_mznear)
			clip = -128;
		else
		{
			zv = f_mpersp / z;
			x = x * zv + f_centerx;
			y = y * zv + f_centery;
			v->rhw = f_moneopersp * zv;

			if (x < f_left)
				clip = 1;
			else if (x > f_right)
				clip = 2;

			if (y < f_top)
				clip += 4;
			else if (y > f_bottom)
				clip += 8;

			v->sx = x;
			v->sy = y;
		}

		v->sz = z;
		v->color = col;
		v->specular = 0xFF000000;
		*c++ = clip;
		v++;
		vec++;
	}
}

void DrawBinoculars()
{
	MESH_DATA* mesh;
	D3DTLVERTEX* v;
	TEXTURESTRUCT* tex;
	D3DTLVERTEX vtx[256];
	D3DTLVERTEX irVtx[4];
	TEXTURESTRUCT irTex;
	short* clip;
	short* quad;
	short* tri;
	ushort drawbak;
	short c;

	if (LaserSight)
		mesh = targetMeshP;
	else
		mesh = binocsMeshP;

	v = (D3DTLVERTEX*)mesh->aVtx;
	clip = clipflags;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		c = 0;
		vtx[i] = v[i];
		vtx[i].sx = (vtx[i].sx * float(phd_winxmax / 512.0F)) + f_centerx;
		vtx[i].sy = (vtx[i].sy * float(phd_winymax / 240.0F)) + f_centery;

		if (vtx[i].sx < f_left)
			c = 1;
		else if (vtx[i].sx > f_right)
			c = 2;

		if (vtx[i].sy < f_top)
			c += 4;
		else if (vtx[i].sy > f_bottom)
			c += 8;

		*clip++ = c;
	}

	quad = mesh->gt4;
	tri = mesh->gt3;

	if (LaserSight || SniperOverlay)
	{
		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			tex = &textinfo[quad[4] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (quad[5] & 1)
			{
				vtx[quad[0]].color = 0xFF000000;
				vtx[quad[1]].color = 0xFF000000;
				vtx[quad[2]].color = 0;
				vtx[quad[3]].color = 0;
				tex->drawtype = 3;
			}

			AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
			tex->drawtype = drawbak;
		}

		for (int i = 0, j = 0; i < mesh->ngt3; i++, tri += 5)
		{
			tex = &textinfo[tri[3] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (tri[4] & 1)
			{
				vtx[tri[0]].color = TargetGraphColTab[j] << 24;
				vtx[tri[1]].color = TargetGraphColTab[j + 1] << 24;
				vtx[tri[2]].color = TargetGraphColTab[j + 2] << 24;
				tex->drawtype = 3;
				j += 3;
			}

			AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
			tex->drawtype = drawbak;
		}
	}
	else
	{
		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			tex = &textinfo[quad[4] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (gfCurrentLevel == 9)
			{
				if (i < 14)
				{
					if (quad[5] & 1)
					{
						vtx[quad[0]].color = 0xFF000000;
						vtx[quad[1]].color = 0xFF000000;
						vtx[quad[2]].color = 0xFF000000;
						vtx[quad[3]].color = 0xFF000000;
						tex->drawtype = 3;
					}
				}
				else
				{
					if (quad[5] & 1)
					{
						vtx[quad[0]].color = 0;
						vtx[quad[1]].color = 0;
						vtx[quad[2]].color = 0xFF000000;
						vtx[quad[3]].color = 0xFF000000;
						tex->drawtype = 3;
					}
				}
			}
			else
			{
				if (quad[5] & 1)
				{
					vtx[quad[0]].color = 0xFF000000;
					vtx[quad[1]].color = 0xFF000000;
					vtx[quad[2]].color = 0;
					vtx[quad[3]].color = 0;
					tex->drawtype = 3;
				}
			}

			AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
			tex->drawtype = drawbak;
		}

		for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		{
			tex = &textinfo[tri[3] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (gfCurrentLevel < 11)
			{
				if (tri[4] & 1)
				{
					vtx[tri[0]].color = 0;
					vtx[tri[1]].color = 0xFF000000;
					vtx[tri[2]].color = 0;
					tex->drawtype = 3;
				}
			}
			else
			{
				if (i < mesh->ngt3 - 2)
				{
					if (tri[4] & 1)
					{
						vtx[tri[0]].color = 0xFF000000;
						vtx[tri[1]].color = 0xFF000000;
						vtx[tri[2]].color = 0;
						tex->drawtype = 3;
					}
				}
				else if (i == mesh->ngt3 - 2)
				{
					if (tri[4] & 1)
					{
						vtx[tri[0]].color = 0;
						vtx[tri[1]].color = 0;
						vtx[tri[2]].color = 0xFF000000;
						tex->drawtype = 3;
					}
				}
				else
				{
					if (tri[4] & 1)
					{
						vtx[tri[0]].color = 0;
						vtx[tri[1]].color = 0xFF000000;
						vtx[tri[2]].color = 0;
						tex->drawtype = 3;
					}
				}
			}

			AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
			tex->drawtype = drawbak;
		}

		if (InfraRed)
		{
			aSetXY4(irVtx, 0, 0, phd_winxmax, 0, 0, phd_winymax, phd_winxmax, phd_winymax, f_mznear + 1, clipflags);
			irVtx[0].color = 0x64FF0000;
			irVtx[1].color = 0x64FF0000;
			irVtx[2].color = 0x64FF0000;
			irVtx[3].color = 0x64FF0000;
			irVtx[0].specular = 0xFF000000;
			irVtx[1].specular = 0xFF000000;
			irVtx[2].specular = 0xFF000000;
			irVtx[3].specular = 0xFF000000;
			irTex.drawtype = 3;
			irTex.tpage = 0;
			AddQuadSorted(irVtx, 0, 1, 3, 2, &irTex, 1);
		}
	}
}

void aDrawWreckingBall(ITEM_INFO* item, long shade)
{
	SPRITESTRUCT* sprite;
	SVECTOR* vec;
	TEXTURESTRUCT tex;
	long x, z, s;

	aSetViewMatrix();
	vec = (SVECTOR*)&scratchpad[0];
	s = (400 * shade) >> 7;
	x = -s;

	for (int i = 0; i < 3; i++)
	{
		z = -s;

		for (int j = 0; j < 3; j++)
		{
			vec->vx = (short)x;
			vec->vy = -2;
			vec->vz = (short)z;
			vec++;
			z += s;
		}

		x += s;
	}

	if (shade < 100)
		shade = 100;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
	vec = (SVECTOR*)&scratchpad[0];
	aTransformPerspSV(vec, aVertexBuffer, clipflags, 9, shade << 24);

	tex.drawtype = 3;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;
	AddQuadSorted(aVertexBuffer, 0, 2, 8, 6, &tex, 1);
}

void ClearFX()
{
	for (int i = 0; i < 2048; i++)
	{
		Rain[i].x = 0;
		Snow[i].x = 0;
	}
}

void AddPolyLine(D3DTLVERTEX* vtx, TEXTURESTRUCT* tex)
{
	D3DTLVERTEX v[4];
	float x0, y0, x1, y1, x2, y2, x3, y3;
	short cf0, cf1;

	x0 = vtx->sx;
	y0 = vtx->sy;
	x1 = vtx[1].sx;
	y1 = vtx[1].sy;
	cf0 = clipflags[0];
	cf1 = clipflags[1];

	if (fabs(x1 - x0) <= fabs(y1 - y0))
	{
		x2 = x0 + 2.0F;
		y2 = y0;
		x3 = x1 + 2.0F;
		y3 = y1;
	}
	else
	{
		x2 = x0;
		y2 = y0 + 2.0F;
		x3 = x1;
		y3 = y1 + 2.0F;
	}

	v[0].sx = x0;
	v[0].sy = y0;
	v[0].rhw = vtx->rhw;
	v[0].tu = vtx->tu;
	v[0].tv = vtx->tv;
	v[0].color = vtx->color;
	v[0].specular = 0xFF000000;

	v[1].sx = x2;
	v[1].sy = y2;
	v[1].rhw = vtx->rhw;
	v[1].tu = vtx->tu;
	v[1].tv = vtx->tv;
	v[1].color = vtx->color;
	v[1].specular = 0xFF000000;

	v[2].sx = x1;
	v[2].sy = y1;
	v[2].rhw = vtx->rhw;
	v[2].tu = vtx[1].tu;
	v[2].tv = vtx[1].tv;
	v[2].color = vtx[1].color;
	v[2].specular = 0xFF000000;

	v[3].sx = x3;
	v[3].sy = y3;
	v[3].rhw = vtx->rhw;
	v[3].tu = vtx[1].tu;
	v[3].tv = vtx[1].tv;
	v[3].color = vtx[1].color;
	v[3].specular = 0xFF000000;

	clipflags[0] = cf0;
	clipflags[1] = cf0;
	clipflags[2] = cf1;
	clipflags[3] = cf1;

	if (tex->drawtype == 3 || tex->drawtype == 2)
		AddQuadSorted(v, 0, 1, 2, 3, tex, 0);
	else
		AddQuadZBuffer(v, 0, 1, 2, 3, tex, 0);
}

void DoSnow()
{
	SNOWFLAKE* snow;
	ROOM_INFO* r;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float* pSize;
	float x, y, z, xv, yv, zv, vx, vy, xSize, ySize;
	long num_alive, rad, angle, ox, oy, oz, col;
	short room_number, clipFlag;

	num_alive = 0;

	for (int i = 0; i < snow_count; i++)
	{
		snow = &Snow[i];

		if (!snow->x)
		{
			if (!snow_outside || num_alive >= max_snow)
				continue;

			num_alive++;
			rad = GetRandomDraw() & 0x1FFF;
			angle = (GetRandomDraw() & 0xFFF) << 1;
			snow->x = camera.pos.x + (rad * rcossin_tbl[angle] >> 12);
			snow->y = camera.pos.y - 1024 - (GetRandomDraw() & 0x7FF);
			snow->z = camera.pos.z + (rad * rcossin_tbl[angle + 1] >> 12);

			if (IsRoomOutside(snow->x, snow->y, snow->z) < 0)
			{
				snow->x = 0;
				continue;
			}

			if (room[IsRoomOutsideNo].flags & ROOM_UNDERWATER)
			{
				snow->x = 0;
				continue;
			}

			snow->stopped = 0;
			snow->xv = (GetRandomDraw() & 7) - 4;
			snow->yv = ((GetRandomDraw() & 0xF) + 8) << 3;
			snow->zv = (GetRandomDraw() & 7) - 4;
			snow->room_number = IsRoomOutsideNo;
			snow->life = 112 - (snow->yv >> 2);
		}

		ox = snow->x;
		oy = snow->y;
		oz = snow->z;

		if (!snow->stopped)
		{
			snow->x += snow->xv;
			snow->y += (snow->yv >> 1) & 0xFC;
			snow->z += snow->zv;
			r = &room[snow->room_number];

			if (snow->y <= r->maxceiling || snow->y >= r->minfloor ||
				snow->z <= r->z + 1024 || snow->z >= (r->x_size << 10) + r->z - 1024 ||
				snow->x <= r->x + 1024 || snow->x >= (r->y_size << 10) + r->x - 1024)
			{
				room_number = snow->room_number;
				GetFloor(snow->x, snow->y, snow->z, &room_number);

				if (room_number == snow->room_number)
				{
					snow->x = 0;
					continue;
				}

				if (room[room_number].flags & ROOM_UNDERWATER)
				{
					snow->stopped = 1;
					snow->x = ox;
					snow->y = oy;
					snow->z = oz;

					if (snow->life > 16)
						snow->life = 16;
				}
				else
					snow->room_number = room_number;
			}
		}

		if (!snow->life)
		{
			snow->x = 0;
			continue;
		}

		if ((abs(CamPos.x - snow->x) > 6000 || abs(CamPos.z - snow->z) > 6000) && snow->life > 16)
			snow->life = 16;

		if (snow->xv < SmokeWindX << 2)
			snow->xv += 2;
		else if (snow->xv > SmokeWindX << 2)
			snow->xv -= 2;

		if (snow->zv < SmokeWindZ << 2)
			snow->zv += 2;
		else if (snow->zv > SmokeWindZ << 2)
			snow->zv -= 2;

		snow->life -= 2;

		if ((snow->yv & 7) != 7)
			snow->yv++;
	}

	mAddProfilerEvent();
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 10];
	tex.tpage = sprite->tpage;
	tex.drawtype = 2;
	tex.flag = 0;
	tex.u1 = sprite->x2;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y2;
	tex.v3 = sprite->y2;
	tex.u3 = sprite->x1;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;

	phd_PushMatrix();
	phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);
	SetD3DViewMatrix();

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	for (int i = 0; i < snow_count; i++)
	{
		snow = &Snow[i];

		if (!snow->x)
			continue;

		x = float(snow->x - camera.pos.x);
		y = float(snow->y - camera.pos.y);
		z = float(snow->z - camera.pos.z);
		zv = x * D3DMView._13 + y * D3DMView._23 + z * D3DMView._33 + D3DMView._43;

		if (zv < f_mznear)
			continue;

		col = 0;

		if ((snow->yv & 7) != 7)
			col = (snow->yv & 7) << 4;
		else if (snow->life > 32)
			col = 130;
		else
			col = snow->life << 3;

		col = RGBA(col, col, col, 0xFF);
		pSize = &SnowSizes[snow->yv & 24];
		xv = x * D3DMView._11 + y * D3DMView._21 + z * D3DMView._31 + D3DMView._41;
		yv = x * D3DMView._12 + y * D3DMView._22 + z * D3DMView._32 + D3DMView._42;
		zv = f_mpersp / zv;

		for (int j = 0; j < 4; j++)
		{
			xSize = pSize[0] * zv;
			ySize = pSize[1] * zv;
			pSize += 2;

			vx = xv * zv + xSize + f_centerx;
			vy = yv * zv + ySize + f_centery;
			clipFlag = 0;

			if (vx < f_left)
				clipFlag++;
			else if (vx > f_right)
				clipFlag += 2;

			if (vy < f_top)
				clipFlag += 4;
			else if (vy > f_bottom)
				clipFlag += 8;

			clipflags[j] = clipFlag;
			v[j].sx = vx;
			v[j].sy = vy;
			v[j].rhw = zv * f_moneopersp;
			v[j].tu = 0;
			v[j].tv = 0;
			v[j].color = col;
			v[j].specular = 0xFF000000;
		}

		AddTriSorted(v, 2, 0, 1, &tex, 1);
	}

	phd_PopMatrix();
}

void aInitFX()
{
	if (G_dxptr->Flags & 0x80)
	{
		snow_count = 2048;
		rain_count = 2048;
		max_snow = 128;
		max_rain = 128;
	}
	else
	{
		snow_count = 256;
		rain_count = 256;
		max_snow = 8;
		max_rain = 8;
	}
}

void DoWeather()
{
	if (WeatherType == 1)
		DoRain();
	else if (WeatherType == 2)
		DoSnow();
}

void aSetXY4(D3DTLVERTEX* v, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, short* clip)
{
	float zv;
	short clipFlag;

	zv = f_mpersp / z * f_moneopersp;

	clipFlag = 0;

	if (x1 < f_left)
		clipFlag++;
	else if (x1 > f_right)
		clipFlag += 2;

	if (y1 < f_top)
		clipFlag += 4;
	else if (y1 > f_bottom)
		clipFlag += 8;

	clip[0] = clipFlag;
	v[0].sx = x1;
	v[0].sy = y1;
	v[0].sz = z;
	v[0].rhw = zv;

	clipFlag = 0;

	if (x2 < f_left)
		clipFlag++;
	else if (x2 > f_right)
		clipFlag += 2;

	if (y2 < f_top)
		clipFlag += 4;
	else if (y2 > f_bottom)
		clipFlag += 8;

	clip[1] = clipFlag;
	v[1].sx = x2;
	v[1].sy = y2;
	v[1].sz = z;
	v[1].rhw = zv;

	clipFlag = 0;

	if (x3 < f_left)
		clipFlag++;
	else if (x3 > f_right)
		clipFlag += 2;

	if (y3 < f_top)
		clipFlag += 4;
	else if (y3 > f_bottom)
		clipFlag += 8;

	clip[2] = clipFlag;
	v[2].sx = x3;
	v[2].sy = y3;
	v[2].sz = z;
	v[2].rhw = zv;

	clipFlag = 0;

	if (x4 < f_left)
		clipFlag++;
	else if (x4 > f_right)
		clipFlag += 2;

	if (y4 < f_top)
		clipFlag += 4;
	else if (y4 > f_bottom)
		clipFlag += 8;

	clip[3] = clipFlag;
	v[3].sx = x4;
	v[3].sy = y4;
	v[3].sz = z;
	v[3].rhw = zv;
}

void InitTarget()
{
	OBJECT_INFO* obj;
	ACMESHVERTEX* p;
	D3DTLVERTEX* v;

	obj = &objects[TARGET_GRAPHICS];

	if (!obj->loaded)
		return;

	targetMeshP = (MESH_DATA*)meshes[obj->mesh_index];
	p = targetMeshP->aVtx;
	targetMeshP->aVtx = (ACMESHVERTEX*)game_malloc(targetMeshP->nVerts * sizeof(ACMESHVERTEX), 0);
	v = (D3DTLVERTEX*)targetMeshP->aVtx;	//makes no sense otherwise

	for (int i = 0; i < targetMeshP->nVerts; i++)
	{
		v[i].sx = (p[i].x * 80.0F) / 96.0F;
		v[i].sy = (p[i].y * 60.0F) / 224.0F;
		v[i].sz = 0;
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
	}
}

void InitBinoculars()
{
	OBJECT_INFO* obj;
	ACMESHVERTEX* p;
	D3DTLVERTEX* v;

	obj = &objects[BINOCULAR_GRAPHICS];

	if (!obj->loaded)
		return;

	binocsMeshP = (MESH_DATA*)meshes[obj->mesh_index];
	p = binocsMeshP->aVtx;
	binocsMeshP->aVtx = (ACMESHVERTEX*)game_malloc(binocsMeshP->nVerts * sizeof(ACMESHVERTEX), 0);
	v = (D3DTLVERTEX*)binocsMeshP->aVtx;	//makes no sense otherwise

	for (int i = 0; i < binocsMeshP->nVerts; i++)
	{
		v[i].sx = (p[i].x * 32.0F) / 96.0F;
		v[i].sy = (p[i].y * 30.0F) / 224.0F;
		v[i].sz = 0;
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
	}
}

void inject_specificfx(bool replace)
{
	INJECT(0x004C2F10, S_PrintShadow, replace);
	INJECT(0x004C7320, DrawLaserSightSprite, replace);
	INJECT(0x004C5EA0, DrawFlatSky, replace);
	INJECT(0x004CBB10, S_DrawDarts, replace);
	INJECT(0x004CF2F0, DrawMoon, replace);
	INJECT(0x004CFF80, DrawGasCloud, replace);
	INJECT(0x004C0060, DrawStarField, replace);
	INJECT(0x004C0F30, setXYZ3, replace);
	INJECT(0x004C09E0, setXYZ4, replace);
	INJECT(0x004C0810, setXY3, replace);
	INJECT(0x004C05B0, setXY4, replace);
	INJECT(0x004C4790, S_DrawDrawSparksNEW, replace);
	INJECT(0x004BF3C0, DoRain, replace);
	INJECT(0x004C6D10, OutputSky, replace);
	INJECT(0x004CA770, DoScreenFade, replace);
	INJECT(0x004C6BA0, ClipCheckPoint, replace);
	INJECT(0x004CD750, aTransformPerspSV, replace);
	INJECT(0x004C36B0, DrawBinoculars, replace);
	INJECT(0x004CF1B0, aDrawWreckingBall, replace);
	INJECT(0x004BFD70, ClearFX, replace);
	INJECT(0x004BFDA0, AddPolyLine, replace);
	INJECT(0x004BEBD0, DoSnow, replace);
	INJECT(0x004BEB50, aInitFX, replace);
	INJECT(0x004C0580, DoWeather, replace);
	INJECT(0x004C3EB0, aSetXY4, replace);
	INJECT(0x004C34F0, InitTarget, replace);
	INJECT(0x004C35D0, InitBinoculars, replace);
}
