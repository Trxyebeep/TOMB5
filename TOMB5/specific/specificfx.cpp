#include "../tomb5/pch.h"
#include "specificfx.h"
#include "3dmath.h"
#include "../game/delstuff.h"
#include "../game/control.h"
#include "../specific/function_table.h"
#include "../game/objects.h"

long ShadowTable[] =
{4, 1, 5, 5, 1, 6, 6, 1, 2, 6, 2, 7, 8, 4, 9, 9, 4, 5, 9, 5, 10, 10, 5, 6, 10, 6, 11, 11, 6, 7, 13, 8, 9, 13, 9, 14, 14, 9, 10, 14, 10, 11};

void S_PrintShadow(short size, short* box, ITEM_INFO* item, short unknown)
{
	FLOOR_INFO* floor;
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[4];
	PHD_VECTOR vector;
	float* pmxz;
	long* phxz;
	long* h;
	long* mptr;
	float mxz[48];
	float fx, fy, fz;
	long hxz[32], height [16];
	long x1, y1, z1, x2, y2, z2, x3, y3, z3;
	long x, y, z, sx, sz, xextent, zextent, sxstep, szstep, color, a, b, c;
	short room_number;
	
	xextent = size * (box[1] - box[0]) / 192;
	zextent = size * (box[5] - box[4]) / 192;
	pmxz = mxz;
	phxz = hxz;
	sxstep = xextent / 4;
	szstep = zextent / 4;
	sx = -sxstep - (sxstep >> 1);
	sz = szstep + (szstep >> 1);

	for (int i = 0; i < 4; i++, sz -= szstep)
	{
		for (int j = 0; j < 4; j++, pmxz += 3, phxz += 2, sx += sxstep)
		{
			pmxz[0] = (float)sx;
			pmxz[2] = (float)sz;
			phxz[0] = sx;
			phxz[1] = sz;
		}

		sx = -sxstep - (sxstep >> 1);
	}

	phd_PushUnitMatrix();

	if (item == lara_item)
	{
		vector.x = 0;
		vector.y = 0;
		vector.z = 0;
		GetLaraJointPos(&vector, 7);
		room_number = lara_item->room_number;
		floor = GetFloor(vector.x, vector.y, vector.z, &room_number);
		y = GetHeight(floor, vector.x, vector.y, vector.z);

		if (y == NO_HEIGHT)
			y = item->floor;
	}
	else
	{
		vector.x = item->pos.x_pos;
		y = item->floor;
		vector.z = item->pos.z_pos;
	}

	phd_TranslateRel(vector.x, y - 16, vector.z);
	phd_RotY(item->pos.y_rot);
	mptr = phd_mxptr;
	phxz = hxz;

	for (int i = 0; i < 16; i++, phxz += 2)
	{
		x = phxz[0];
		z = phxz[1];
		phxz[0] = (x * mptr[0] + z * mptr[2] + mptr[3]) >> 14;
		phxz[1] = (x * mptr[8] + z * mptr[10] + mptr[11]) >> 14;
	}

	phd_PopMatrix();
	phxz = hxz;
	h = height;

	for (int i = 0; i < 16; i++, phxz += 2, h++)
	{
		room_number = item->room_number;
		floor = GetFloor(phxz[0], item->floor, phxz[1], &room_number);
		*h = GetHeight(floor, phxz[0], item->floor, phxz[1]);

		if (ABS(*h - item->floor) > 196)
			*h = item->floor;
	}

	phd_PushMatrix();
	phd_TranslateAbs(vector.x, y - 16, vector.z);
	phd_RotY(item->pos.y_rot);
	mptr = phd_mxptr;
	pmxz = mxz;
	h = height;

	for (int i = 0; i < 16; i++, pmxz += 3, h++)
	{
		fx = pmxz[0];
		fy = (float)(*h - item->floor);
		fz = pmxz[2];
		pmxz[0] = aMXPtr[0] * fx + aMXPtr[1] * fy + aMXPtr[2] * fz + aMXPtr[3];
		pmxz[1] = aMXPtr[4] * fx + aMXPtr[5] * fy + aMXPtr[6] * fz + aMXPtr[7];
		pmxz[2] = aMXPtr[8] * fx + aMXPtr[9] * fy + aMXPtr[10] * fz + aMXPtr[11];
	}

	phd_PopMatrix();
	pmxz = mxz;
	color = 0x4F000000;

	for (int i = 0; i < 14; i++)
	{
		a = ShadowTable[(i * 3) + 0] * 3;
		b = ShadowTable[(i * 3) + 1] * 3;
		c = ShadowTable[(i * 3) + 2] * 3;
		x1 = (long)pmxz[a + 0];
		y1 = (long)pmxz[a + 1];
		z1 = (long)pmxz[a + 2];
		x2 = (long)pmxz[b + 0];
		y2 = (long)pmxz[b + 1];
		z2 = (long)pmxz[b + 2];
		x3 = (long)pmxz[c + 0];
		y3 = (long)pmxz[c + 1];
		z3 = (long)pmxz[c + 2];
		setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);
		v[0].color = color;
		v[1].color = color;
		v[2].color = color;
		v[3].color = color;

		if (item->after_death)
		{
			v[0].color &= 0x00FFFFFF;
			v[1].color &= 0x00FFFFFF;
			v[2].color &= 0x00FFFFFF;
			v[3].color &= 0x00FFFFFF;
			v[0].color |= (128 - item->after_death) << 24;
			v[1].color |= (128 - item->after_death) << 24;
			v[2].color |= (128 - item->after_death) << 24;
			v[3].color |= (128 - item->after_death) << 24;
		}

		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		Tex.drawtype = 3;
		Tex.flag = 0;
		Tex.tpage = 0;
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
	TEXTURESTRUCT Tex;
	long* TempIDK;
	short* TempDist;
	short* TempXY;
	float zv, u1, u2, v1, v2;
	long results[3];

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	TempDist = (short*)&UNK_EffectDistance;
	TempXY = (short*)&UNK_EffectXY;
	TempIDK = (long*)&UNK_00E913E0;
	TempDist[0] = (short)(LaserSightX - lara_item->pos.x_pos);
	TempDist[1] = (short)(LaserSightY - lara_item->pos.y_pos);
	TempDist[2] = (short)(LaserSightZ - lara_item->pos.z_pos);
	results[0] = phd_mxptr[0] * TempDist[0] + phd_mxptr[1] * TempDist[1] + phd_mxptr[2] * TempDist[2] + phd_mxptr[3];
	results[1] = phd_mxptr[4] * TempDist[0] + phd_mxptr[5] * TempDist[1] + phd_mxptr[6] * TempDist[2] + phd_mxptr[7];
	results[2] = phd_mxptr[8] * TempDist[0] + phd_mxptr[9] * TempDist[1] + phd_mxptr[10] * TempDist[2] + phd_mxptr[11];
	zv = f_persp / (float)results[2];
	TempXY[0] = short(float(results[0] * zv + f_centerx));
	TempXY[1] = short(float(results[1] * zv + f_centery));
	TempIDK[0] = results[2] >> 14;//?????????????????????? why lmao
	phd_PopMatrix();
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	setXY4(v, TempXY[0] - 2, TempXY[1] - 2, TempXY[0] + 2, TempXY[1] - 2, TempXY[0] - 2, TempXY[1] + 2, TempXY[0] + 2, TempXY[1] + 2, (int)f_mznear, clipflags);
	v[0].color = LaserSightCol ? 0x0000FF00 : 0x00FF0000;//if LaserSightCol is on, it turns green
	v[1].color = v[0].color;
	v[2].color = v[0].color;
	v[3].color = v[0].color;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	u1 = sprite->x2;
	u2 = sprite->x1;
	v1 = sprite->y2;
	v2 = sprite->y1;
	Tex.drawtype = 2;
	Tex.flag = 0;
	Tex.tpage = sprite->tpage;
	Tex.u1 = u1;
	Tex.v1 = v1;
	Tex.u2 = u2;
	Tex.v2 = v1;
	Tex.u3 = u2;
	Tex.v3 = v2;
	Tex.u4 = u1;
	Tex.v4 = v2;
	AddQuadSorted(v, 0, 1, 2, 3, &Tex, 0);
	LaserSightCol = 0;
	LaserSightActive = 0;
}

void inject_specificfx(bool replace)
{
	INJECT(0x004C2F10, S_PrintShadow, replace);
	INJECT(0x004C7320, DrawLaserSightSprite, replace);
}
