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
#include "alexstuff.h"
#include "../game/sphere.h"
#include "../game/lasers.h"
#include "../game/rope.h"
#include "../game/gameflow.h"
#include "output.h"
#include "file.h"
#include "texture.h"
#include "../game/camera.h"
#include "dxshell.h"
#include "../game/lara.h"
#include "../game/effects.h"
#include "../game/effect2.h"
#include "gamemain.h"
#include "../game/draw.h"
#include "../tomb5/tomb5.h"
#include "../game/lara_states.h"
#include "../game/deltapak.h"

long DoFade;
long snow_outside;

static STARS stars[2048];
static RAINDROPS Rain[1024];
static SNOWFLAKE Snow[1024];
static UWEFFECTS uwdust[256];
static PHD_VECTOR NodeVectors[16];
static long FadeVal;
static long FadeStep;
static long FadeCnt;
static long FadeEnd;
static short rain_count;
static short snow_count;
static short max_rain;
static short max_snow;

static uchar TargetGraphColTab[48] =
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

static float SnowSizes[32]
{
	-24.0F, -24.0F, -24.0F, 24.0F, 24.0F, -24.0F, 24.0F, 24.0F, -12.0F, -12.0F, -12.0F, 12.0F, 12.0F, -12.0F, 12.0F, 12.0F,
	-8.0F, -8.0F, -8.0F, 8.0F, 8.0F, -8.0F, 8.0F, 8.0F, -6.0F, -6.0F, -6.0F, 6.0F, 6.0F, -6.0F, 6.0F, 6.0F
};

static NODEOFFSET_INFO NodeOffsets[16] =
{
	{ -16, 40, 160, -LMX_HAND_L, 0 },
	{ -16, -8, 160, 0, 0 },
	{ 0, 0, 256, 8, 0 },
	{ 0, 0, 256, 17, 0 },
	{ 0, 0, 256, 26, 0 },
	{ 0, 144, 40, 10, 0 },
	{ -40, 64, 360, 14, 0 },
	{ 0, -600, -40, 0, 0 },
	{ 0, 32, 16, 9, 0 },

	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

static uchar SplashLinks[]
{
	16, 18, 0, 2,
	18, 20, 2, 4,
	20, 22, 4, 6,
	22, 24, 6, 8,
	24, 26, 8, 10,
	26, 28, 10, 12,
	28, 30, 12, 14,
	30, 16, 14, 0
};

static long ShadowTable[14 * 3] =	//NUM_TRIS * 3 points
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

void setXY3(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long z, short* clip)
{
	float zv;
	short clipFlag;

	clipFlag = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[0] = clipFlag;
	v++;
	clipFlag = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[1] = clipFlag;
	v++;
	clipFlag = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[2] = clipFlag;
}

void setXY4(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip)
{
	float zv;
	short clipFlag;

	clipFlag = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[0] = clipFlag;
	v++;
	clipFlag = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[1] = clipFlag;
	v++;
	clipFlag = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[2] = clipFlag;
	v++;
	clipFlag = 0;
	v->sx = (float)x4;
	v->sy = (float)y4;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clipFlag++;
	else if (phd_winxmax < v->sx)
		clipFlag += 2;

	if (phd_winymin > v->sy)
		clipFlag += 4;
	else if (phd_winymax < v->sy)
		clipFlag += 8;

	clip[3] = clipFlag;
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

void setXYZ3(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip)
{
	float zv;
	short clipFlag;

	clipFlag = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	v++;
	clip[0] = clipFlag;
	clipFlag = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	v++;
	clip[1] = clipFlag;
	clipFlag = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	clip[2] = clipFlag;
}

void setXYZ4(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip)
{
	float zv;
	short clipFlag;

	clipFlag = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	v++;
	clip[0] = clipFlag;
	clipFlag = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	v++;
	clip[1] = clipFlag;
	clipFlag = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	v++;
	clip[2] = clipFlag;
	clipFlag = 0;
	v->tu = (float)x4;
	v->tv = (float)y4;
	v->sz = (float)z4;

	if (v->sz < f_mznear)
		clipFlag = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > f_mzfar)
		{
			clipFlag = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clipFlag++;
		else if (phd_winxmax < v->sx)
			clipFlag += 2;

		if (phd_winymin > v->sy)
			clipFlag += 4;
		else if (phd_winymax < v->sy)
			clipFlag += 8;
	}

	clip[3] = clipFlag;
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

		if (v->sz > f_mzfar)
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
		vx = vec->x;
		vy = vec->y;
		vz = vec->z;
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

void AddPolyLine(D3DTLVERTEX* vtx, TEXTURESTRUCT* tex, float p)
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
		x2 = x0 + p;
		y2 = y0;
		x3 = x1 + p;
		y3 = y1;
	}
	else
	{
		x2 = x0;
		y2 = y0 + p;
		x3 = x1;
		y3 = y1 + p;
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
		AddQuadSorted(v, 0, 1, 2, 3, tex, 1);
	else
		AddQuadZBuffer(v, 0, 1, 2, 3, tex, 1);
}

bool ClipLine(long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xMin, long yMin, long w, long h)
{
	float clip;

	if (z1 < 20 || z2 < 20)
		return 0;

	if (x1 < xMin && x2 < xMin || y1 < yMin && y2 < yMin)
		return 0;

	if (x1 > w && x2 > w || y1 > h && y2 > h)
		return 0;

	if (x1 > w)
	{
		clip = ((float)w - x2) / float(x1 - x2);
		x1 = w;
		y1 = long((y1 - y2) * clip + y2);
	}

	if (x2 > w)
	{
		clip = ((float)w - x1) / float(x2 - x1);
		x2 = w;
		y2 = long((y2 - y1) * clip + y1);
	}

	if (x1 < xMin)
	{
		clip = ((float)xMin - x1) / float(x2 - x1);
		x1 = xMin;
		y1 = long((y2 - y1) * clip + y1);
	}

	if (x2 < xMin)
	{
		clip = ((float)xMin - x2) / float(x1 - x2);
		x2 = xMin;
		y2 = long((y1 - y2) * clip + y2);
	}

	if (y1 > h)
	{
		clip = ((float)h - y2) / float(y1 - y2);
		y1 = h;
		x1 = long((x1 - x2) * clip + x2);
	}

	if (y2 > h)
	{
		clip = ((float)h - y1) / float(y2 - y1);
		y2 = h;
		x2 = long((x2 - x1) * clip + x1);
	}

	if (y1 < yMin)
	{
		clip = ((float)yMin - y1) / float(y2 - y1);
		y1 = yMin;
		x1 = long((x2 - x1) * clip + x1);
	}

	if (y2 < yMin)
	{
		clip = ((float)yMin - y2) / float(y1 - y2);
		y2 = yMin;
		x2 = long((x1 - x2) * clip + x2);
	}

	return 1;
}

void aInitFX()
{
	snow_count = 1024;
	rain_count = 1024;
	max_snow = 64;
	max_rain = 64;
}

void ClearFX()
{
	for (int i = 0; i < 256; i++)
		uwdust[i].x = 0;

	for (int i = 0; i < 1024; i++)
	{
		Rain[i].x = 0;
		Snow[i].x = 0;
	}
}

void InitTarget()
{
	OBJECT_INFO* obj;
	ACMESHVERTEX* p;
	D3DTLVERTEX* v;
	MESH_DATA* mesh;

	obj = &objects[TARGET_GRAPHICS];

	if (!obj->loaded)
		return;

	mesh = (MESH_DATA*)meshes[obj->mesh_index];
	p = mesh->aVtx;
	mesh->aVtx = (ACMESHVERTEX*)game_malloc(mesh->nVerts * sizeof(ACMESHVERTEX));
	v = (D3DTLVERTEX*)mesh->aVtx;

	for (int i = 0; i < mesh->nVerts; i++)
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
	MESH_DATA* mesh;
	float x, y;

	obj = &objects[BINOCULAR_GRAPHICS];

	if (!obj->loaded)
		return;

	mesh = (MESH_DATA*)meshes[obj->mesh_index];
	p = mesh->aVtx;
	mesh->aVtx = (ACMESHVERTEX*)game_malloc(mesh->nVerts * sizeof(ACMESHVERTEX));
	v = (D3DTLVERTEX*)mesh->aVtx;	//makes no sense otherwise

	x = gfCurrentLevel == LVL5_LABYRINTH ? 33.0F : 32.0F;
	y = 30.0F;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		v[i].sx = (p[i].x * x) / 96.0F;
		v[i].sy = (p[i].y * y) / 224.0F;
		v[i].sz = 0;
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
	}
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
	DrawSortList();
	InitBuckets();
	InitialiseSortList();
}

void SuperShowLogo()
{
	D3DTLVERTEX* v;
	TEXTURESTRUCT tex;
	float x, y, w;

	v = aVertexBuffer;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;

	tex.drawtype = 1;
	tex.flag = 0;
	tex.u1 = 1.0F / 256.0F;
	tex.v1 = 1.0F / 256.0F;
	tex.u2 = 1.0F - (1.0F / 256.0F);
	tex.v2 = 1.0F / 256.0F;
	tex.u3 = 1.0F - (1.0F / 256.0F);
	tex.v3 = 1.0F - (1.0F / 256.0F);
	tex.u4 = 1.0F / 256.0F;
	tex.v4 = 1.0F - (1.0F / 256.0F);
	w = (float)GetFixedScale(256);
	x = phd_centerx - w;
	y = phd_winymin + w;

	v[0].sx = x;
	v[0].sy = (float)phd_winymin;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = 0xFFFFFFFF;
	v[0].specular = 0xFF000000;

	v[1].sx = x + w;
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = 0xFFFFFFFF;
	v[1].specular = 0xFF000000;

	v[2].sx = x + w;
	v[2].sy = y;
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = 0xFFFFFFFF;
	v[2].specular = 0xFF000000;

	v[3].sx = x;
	v[3].sy = y;
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = 0xFFFFFFFF;
	v[3].specular = 0xFF000000;
	
	tex.tpage = ushort(nTextures - 5);
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = x + w;
	v[0].sy = (float)phd_winymin;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = 0xFFFFFFFF;
	v[0].specular = 0xFF000000;

	v[1].sx = x + (w * 2);
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = 0xFFFFFFFF;
	v[1].specular = 0xFF000000;

	v[2].sx = x + (w * 2);
	v[2].sy = y;
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = 0xFFFFFFFF;
	v[2].specular = 0xFF000000;

	v[3].sx = x + w;
	v[3].sy = y;
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = 0xFFFFFFFF;
	v[3].specular = 0xFF000000;

	tex.tpage = ushort(nTextures - 4);
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
}

void SetFade(long start, long end)
{
	DoFade = 1;
	FadeVal = start;
	FadeStep = (end - start) >> 3;
	FadeCnt = 0;
	FadeEnd = end;
}

void DoScreenFade()
{
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
}

void DoWeather()
{
	if (WeatherType == 1)
		DoRain();
	else if (WeatherType == 2)
		DoSnow();
}

void DoRain()
{
	RAINDROPS* rptr;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	D3DTLVERTEX v[2];
	TEXTURESTRUCT tex;
	short* clip;
	float ctop, cbottom, cright, cleft, zv, fx, fy, fz, mx, my, mz;
	long num_alive, rad, angle, rnd, x, z, x_size, y_size, c;
	short room_number, clipFlag;

	num_alive = 0;

	for (int i = 0; i < rain_count; i++)
	{
		rptr = &Rain[i];

		if (outside && !rptr->x && num_alive < max_rain)
		{
			num_alive++;
			rad = GetRandomDraw() & 8191;
			angle = GetRandomDraw() & 8190;
			rptr->x = camera.pos.x + (rad * rcossin_tbl[angle] >> (W2V_SHIFT - 2));
			rptr->y = camera.pos.y + -1024 - (GetRandomDraw() & 0x7FF);
			rptr->z = camera.pos.z + (rad * rcossin_tbl[angle + 1] >> (W2V_SHIFT - 2));

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
			rptr->yv = uchar((GetRandomDraw() & 3) + GetFixedScale(8));
			rptr->zv = (GetRandomDraw() & 7) - 4;
			rptr->room_number = IsRoomOutsideNo;
			rptr->life = 64 - rptr->yv;
		}

		if (rptr->x)
		{
			if (rptr->life > 240 || abs(CamPos.x - rptr->x) > 6000 || abs(CamPos.z - rptr->z) > 6000)
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
				floor = GetFloor(rptr->x, rptr->y, rptr->z, &room_number);

				if (room_number == rptr->room_number || room[room_number].flags & ROOM_UNDERWATER)
				{
					if (room[room_number].flags & ROOM_UNDERWATER && (i & 7) == 7)
						SetupRipple(rptr->x, GetWaterHeight(rptr->x, rptr->y, rptr->z, room_number), rptr->z, 3, 0);
					else
						TriggerSmallSplash(rptr->x, GetHeight(floor, rptr->x, rptr->y, rptr->z), rptr->z, 1);

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
	aSetViewMatrix();

	for (int i = 0; i < rain_count; i++)
	{
		rptr = &Rain[i];

		if (rptr->x)
		{
			clipFlag = 0;
			clip = clipflags;
			fx = float(rptr->x - lara_item->pos.x_pos - (SmokeWindX << 2));
			fy = float(rptr->y - (rptr->yv << 3) - lara_item->pos.y_pos);
			fz = float(rptr->z - lara_item->pos.z_pos - (SmokeWindZ << 2));
			mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
			my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
			mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

			c = long((1.0F - (f_mzfar - mz) * (1.0F / f_mzfar)) * 8.0F + 8.0F);
			v[0].specular = 0xFF000000;
			v[0].color = RGBA(c, c, c, 128);
			v[0].tu = mx;
			v[0].tv = my;

			if (mz < f_mznear)
				clipFlag = -128;
			else
			{
				if (mz > f_mzfar)
				{
					mz = f_zfar;
					clipFlag = 16;
				}

				zv = f_mpersp / mz;
				v[0].sx = zv * mx + f_centerx;
				v[0].sy = zv * my + f_centery;
				v[0].rhw = f_moneopersp * zv;

				if (v[0].sx < cleft)
					clipFlag++;
				else if (v[0].sx > cright)
					clipFlag += 2;

				if (v[0].sy < ctop)
					clipFlag += 4;
				else if (v[0].sy > cbottom)
					clipFlag += 8;
			}

			v[0].sz = mz;
			*clip++ = clipFlag;
			clipFlag = 0;

			fx = float(rptr->x - lara_item->pos.x_pos);
			fy = float(rptr->y - lara_item->pos.y_pos);
			fz = float(rptr->z - lara_item->pos.z_pos);
			mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
			my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
			mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

			c = long((1.0F - (f_mzfar - mz) * (1.0F / f_mzfar)) * 16.0F + 16.0F);
			c <<= 1;
			v[1].specular = 0xFF000000;
			v[1].color = RGBA(c, c, c, 0xFF);
			v[1].tu = mx;
			v[1].tv = my;

			if (mz < f_mznear)
				clipFlag = -128;
			else
			{
				if (mz > f_mzfar)
				{
					mz = f_zfar;
					clipFlag = 16;
				}

				zv = f_mpersp / mz;
				v[1].sx = zv * mx + f_centerx;
				v[1].sy = zv * my + f_centery;
				v[1].rhw = f_moneopersp * zv;

				if (v[1].sx < cleft)
					clipFlag++;
				else if (v[1].sx > cright)
					clipFlag += 2;

				if (v[1].sy < ctop)
					clipFlag += 4;
				else if (v[1].sy > cbottom)
					clipFlag += 8;
			}

			v[1].sz = mz;
			*clip-- = clipFlag;

			if (!clip[0] && !clip[1])
				AddPolyLine(v, &tex, (float)GetFixedScale(1));
		}
	}

	phd_PopMatrix();
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
			snow->x = camera.pos.x + (rad * rcossin_tbl[angle] >> (W2V_SHIFT - 2));
			snow->y = camera.pos.y - 1024 - (GetRandomDraw() & 0x7FF);
			snow->z = camera.pos.z + (rad * rcossin_tbl[angle + 1] >> (W2V_SHIFT - 2));

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
	aSetViewMatrix();

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

	if (LaserSight || SniperOverlay)
		mesh = (MESH_DATA*)meshes[objects[TARGET_GRAPHICS].mesh_index];
	else
		mesh = (MESH_DATA*)meshes[objects[BINOCULAR_GRAPHICS].mesh_index];

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

			if (gfCurrentLevel == LVL5_LABYRINTH)
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

			if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR)
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
			irVtx[0].color = 0x3FFF0000;
			irVtx[1].color = 0x3FFF0000;
			irVtx[2].color = 0x3FFF0000;
			irVtx[3].color = 0x3FFF0000;
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

void S_DrawSparks()
{
	SPARKS* sptr;
	FX_INFO* fx;
	ITEM_INFO* item;
	D3DTLVERTEX v[2];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	FVECTOR fpos;
	float fX, fY, fZ, zv;
	float p[8];
	long x, y, z, smallest_size, r, g, b, c0, c1;

	smallest_size = 0;	//uninitialized
	tex.drawtype = 2;
	tex.tpage = 0;
	tex.flag = 0;

	for (int i = 0; i < 16; i++)
		NodeOffsets[i].GotIt = 0;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	aSetViewMatrix();

	for (int i = 0; i < 1024; i++)
	{
		sptr = &spark[i];

		if (!sptr->On)
			continue;

		if (sptr->Flags & SF_FX)
		{
			fx = &effects[sptr->FxObj];
			x = sptr->x + fx->pos.x_pos;
			y = sptr->y + fx->pos.y_pos;
			z = sptr->z + fx->pos.z_pos;

			if (sptr->sLife - sptr->Life > (GetRandomDraw() & 7) + 4)
			{
				sptr->x = x;
				sptr->y = y;
				sptr->z = z;
				sptr->Flags &= ~SF_FX;
			}
		}
		else if (sptr->Flags & SF_ITEM)
		{
			item = &items[sptr->FxObj];

			if (sptr->Flags & SF_ATTACHEDNODE)
			{
				if (NodeOffsets[sptr->NodeNumber].GotIt)
				{
					pos.x = NodeVectors[sptr->NodeNumber].x;
					pos.y = NodeVectors[sptr->NodeNumber].y;
					pos.z = NodeVectors[sptr->NodeNumber].z;
				}
				else
				{
					pos.x = NodeOffsets[sptr->NodeNumber].x;
					pos.y = NodeOffsets[sptr->NodeNumber].y;
					pos.z = NodeOffsets[sptr->NodeNumber].z;

					if (NodeOffsets[sptr->NodeNumber].mesh_num < 0)
						GetLaraJointPos(&pos, -NodeOffsets[sptr->NodeNumber].mesh_num);
					else
						GetJointAbsPosition(item, &pos, NodeOffsets[sptr->NodeNumber].mesh_num);

					NodeOffsets[sptr->NodeNumber].GotIt = 1;
					NodeVectors[sptr->NodeNumber].x = pos.x;
					NodeVectors[sptr->NodeNumber].y = pos.y;
					NodeVectors[sptr->NodeNumber].z = pos.z;
				}

				x = sptr->x + pos.x;
				y = sptr->y + pos.y;
				z = sptr->z + pos.z;

				if (sptr->sLife - sptr->Life > (GetRandomDraw() & 3) + 8)
				{
					sptr->x = x;
					sptr->y = y;
					sptr->z = z;
					sptr->Flags &= ~(SF_ATTACHEDNODE | SF_ITEM);
				}
			}
			else
			{
				x = sptr->x + item->pos.x_pos;
				y = sptr->y + item->pos.y_pos;
				z = sptr->z + item->pos.z_pos;
			}
		}
		else
		{
			x = sptr->x;
			y = sptr->y;
			z = sptr->z;
		}

		fX = float(x - lara_item->pos.x_pos);
		fY = float(y - lara_item->pos.y_pos);
		fZ = float(z - lara_item->pos.z_pos);
		fpos.x = fX * D3DMView._11 + fY * D3DMView._21 + D3DMView._31 * fZ + D3DMView._41;
		fpos.y = fX * D3DMView._12 + fY * D3DMView._22 + D3DMView._32 * fZ + D3DMView._42;
		fpos.z = fX * D3DMView._13 + fY * D3DMView._23 + D3DMView._33 * fZ + D3DMView._43;

		clipflags[0] = 0;
		clipflags[1] = 0;
		clipflags[2] = 0;
		clipflags[3] = 0;

		if (fpos.z < f_mznear)
			continue;

		zv = f_mpersp / fpos.z;
		p[0] = zv * fpos.x + f_centerx;
		p[1] = zv * fpos.y + f_centery;
		p[2] = fpos.z;
		p[3] = f_moneopersp * zv;

		if (p[0] < f_left || p[0] > f_right || p[1] < f_top || p[1] > f_bottom)
			continue;

		if (sptr->Flags & SF_DEF)
		{
			if (sptr->Flags & SF_SCALE)
				smallest_size = 4;

			S_DrawDrawSparksNEW(sptr, smallest_size, p);
		}
		else
		{
			fX -= float(sptr->Xvel >> 4);
			fY -= float(sptr->Yvel >> 4);
			fZ -= float(sptr->Zvel >> 4);
			fpos.x = fX * D3DMView._11 + fY * D3DMView._21 + D3DMView._31 * fZ + D3DMView._41;
			fpos.y = fX * D3DMView._12 + fY * D3DMView._22 + D3DMView._32 * fZ + D3DMView._42;
			fpos.z = fX * D3DMView._13 + fY * D3DMView._23 + D3DMView._33 * fZ + D3DMView._43;

			if (fpos.z < f_mznear)
				continue;

			zv = f_mpersp / fpos.z;
			p[4] = zv * fpos.x + f_centerx;
			p[5] = zv * fpos.y + f_centery;
			p[6] = fpos.z;
			p[7] = f_moneopersp * zv;

			if (p[4] < f_left || p[4] > f_right || p[5] < f_top || p[5] > f_bottom)
				continue;

			z = (long)fpos.z;

			if (z <= 0x3000)
			{
				c0 = RGBA(sptr->R, sptr->G, sptr->B, 0xFF);
				c1 = c0;
			}
			else
			{
				z = 0x5000 - z;
				r = (z * sptr->R) >> 13;
				g = (z * sptr->G) >> 13;
				b = (z * sptr->B) >> 13;
				c0 = RGBA(r, g, b, 0xFF);
				c1 = RGBA(r >> 1, g >> 1, b >> 1, 0xFF);
			}

			v[0].sx = p[0];
			v[0].sy = p[1];
			v[0].sz = p[2];
			v[0].rhw = p[3];
			v[0].color = c0;
			v[0].specular = 0xFF000000;

			v[1].sx = p[4];
			v[1].sy = p[5];
			v[1].sz = p[6];
			v[1].rhw = p[7];
			v[1].color = c1;
			v[1].specular = 0xFF000000;

			AddPolyLine(v, &tex, (float)GetFixedScale(1));
		}
	}

	phd_PopMatrix();
}

void S_DrawDrawSparksNEW(SPARKS* sptr, long smallest_size, float* xyz)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float x0, y0, x1, y1, x2, y2, x3, y3;
	float fs1, fs2, sin, cos, sinf1, sinf2, cosf1, cosf2;
	long s, scale, s1, s2;

	if (xyz[2] <= f_mznear || xyz[2] >= f_mzfar)
	{
		if (xyz[2] >= f_mzfar)
			sptr->On = 0;

		return;
	}

	if (sptr->Flags & SF_SCALE)
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

		if (sptr->Flags & SF_ROTATE)
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

		if (sptr->TransType == 3)
			tex.drawtype = 5;
		else if (sptr->TransType)
			tex.drawtype = 2;
		else
			tex.drawtype = 1;

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

void S_DrawFireSparks(long size, long life)
{
	FIRE_SPARKS* sptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	FVECTOR pos;
	long* XY;
	long* Z;
	long* offsets;
	float perspz;
	ulong r, g, b, col;
	long newSize, s, c, sx1, cx1, sx2, cx2;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 20; i++)
	{
		sptr = &fire_spark[i];

		if (!sptr->On)
			continue;

		dx = sptr->x >> (2 - size);
		dy = sptr->y >> (2 - size);
		dz = sptr->z >> (2 - size);

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
			continue;

		offsets[0] = dx;
		offsets[1] = dy;
		offsets[2] = dz;
		pos.x = offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03];
		pos.y = offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13];
		pos.z = offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23];
		perspz = f_persp / pos.z;
		XY[0] = long(pos.x * perspz + f_centerx);
		XY[1] = long(pos.y * perspz + f_centery);
		Z[0] = (long)pos.z;


		if (Z[0] <= 0 || Z[0] >= 0x5000)
			continue;

		newSize = (((phd_persp * sptr->Size) << 2) / Z[0]) >> (2 - size);

		if (newSize > (sptr->Size << 2))
			newSize = (sptr->Size << 2);
		else if (newSize < 4)
			newSize = 4;

		newSize >>= 1;

		if (XY[0] + newSize < phd_winxmin || XY[0] - newSize >= phd_winxmax || XY[1] + newSize < phd_winymin || XY[1] - newSize >= phd_winymax)
			continue;

		if (sptr->Flags & SF_ROTATE)
		{
			ang = sptr->RotAng << 1;
			s = rcossin_tbl[ang];
			c = rcossin_tbl[ang + 1];
			sx1 = (-newSize * s) >> (W2V_SHIFT - 2);
			cx1 = (-newSize * c) >> (W2V_SHIFT - 2);
			sx2 = (newSize * s) >> (W2V_SHIFT - 2);
			cx2 = (newSize * c) >> (W2V_SHIFT - 2);
			x1 = XY[0] + (sx1 - cx1);
			y1 = XY[1] + sx1 + cx1;
			x2 = XY[0] + (sx2 - cx1);
			y2 = XY[1] + sx1 + cx2;
			x3 = XY[0] + (sx2 - cx2);
			y3 = XY[1] + sx2 + cx2;
			x4 = XY[0] + (sx1 - cx2);
			y4 = XY[1] + sx2 + cx1;
			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
		}
		else
		{
			x1 = XY[0] - newSize;
			x2 = XY[0] + newSize;
			y1 = XY[1] - newSize;
			y2 = XY[1] + newSize;
			setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		}

		sprite = &spriteinfo[sptr->Def];

		if (Z[0] <= 0x3000)
		{
			r = sptr->R;
			g = sptr->G;
			b = sptr->B;
		}
		else
		{
			r = ((0x5000 - Z[0]) * sptr->R) >> 13;
			g = ((0x5000 - Z[0]) * sptr->G) >> 13;
			b = ((0x5000 - Z[0]) * sptr->B) >> 13;
		}

		r = (r * life) >> 8;
		g = (g * life) >> 8;
		b = (b * life) >> 8;
		col = RGBA(r, g, b, 0xFF);

		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[3].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
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
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
	}
}

void S_DrawSmokeSparks()
{
	SMOKE_SPARKS* sptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	FVECTOR pos;
	long* XY;
	long* Z;
	long* offsets;
	float perspz;
	long is_mirror, size, col, s, c, ss, cs, sm, cm;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];
	is_mirror = 0;
	sptr = &smoke_spark[0];

	for (int i = 0; i < 32; i++)
	{
		if (!sptr->On)
		{
			sptr++;
			continue;
		}

		if (sptr->mirror && !is_mirror)
			is_mirror = 1;
		else
			is_mirror = 0;

		dx = sptr->x - lara_item->pos.x_pos;
		dy = sptr->y - lara_item->pos.y_pos;
		dz = sptr->z - lara_item->pos.z_pos;

		if (is_mirror)
			dz = 2 * gfMirrorZPlane - lara_item->pos.z_pos - sptr->z;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		offsets[0] = dx;
		offsets[1] = dy;
		offsets[2] = dz;
		pos.x = offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03];
		pos.y = offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13];
		pos.z = offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23];
		perspz = f_persp / pos.z;
		XY[0] = long(pos.x * perspz + f_centerx);
		XY[1] = long(pos.y * perspz + f_centery);
		Z[0] = (long)pos.z;

		if (Z[0] <= 0 || Z[0] >= 0x5000)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		size = ((phd_persp * sptr->Size) << 2) / Z[0];

		if (size > sptr->Size << 2)
			size = sptr->Size << 2;
		else if (size < 4)
			size = 4;

		size >>= 1;

		if (XY[0] + size < phd_winxmin || XY[0] - size >= phd_winxmax || XY[1] + size < phd_winymin || XY[1] - size >= phd_winymax)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		if (sptr->Flags & SF_ROTATE)
		{
			ang = sptr->RotAng << 1;
			s = rcossin_tbl[ang];
			c = rcossin_tbl[ang + 1];
			ss = (s * size) >> (W2V_SHIFT - 2);
			cs = (c * size) >> (W2V_SHIFT - 2);
			sm = (s * -size) >> (W2V_SHIFT - 2);
			cm = (c * -size) >> (W2V_SHIFT - 2);

			x1 = sm + XY[0] - cm;
			y1 = sm + XY[1] + cm;
			x2 = XY[0] - cm + ss;
			y2 = sm + XY[1] + cs;
			x3 = ss + XY[0] - cs;
			y3 = cs + XY[1] + ss;
			x4 = sm + XY[0] - cs;
			y4 = ss + XY[1] + cm;

			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
		}
		else
		{
			x1 = XY[0] - size;
			y1 = XY[1] - size;
			x2 = XY[0] + size;
			y2 = XY[1] + size;
			setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		}

		sprite = &spriteinfo[sptr->Def];

		if (Z[0] <= 0x3000)
			col = sptr->Shade;
		else
			col = ((0x5000 - Z[0]) * sptr->Shade) >> 13;

		v[0].color = RGBA(col, col, col, 0xFF);
		v[1].color = RGBA(col, col, col, 0xFF);
		v[2].color = RGBA(col, col, col, 0xFF);
		v[3].color = RGBA(col, col, col, 0xFF);
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

		if (!is_mirror)
			sptr++;
	}

	phd_PopMatrix();
}

void DrawPsxTile(long x_y, long height_width, long color, long u0, long u1)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float x, y, z, rhw, w, h;
	long col;
	ushort drawtype;

	nPolyType = 6;

	if ((color & 0xFF000000) == 0x62000000)
	{
		drawtype = 3;
		col = color << 24;
	}
	else
	{
		drawtype = 2;
		col = color | 0xFF000000;
	}

	if (gfCurrentLevel == LVL5_TITLE)
	{
		z = f_znear + 10;
		rhw = f_moneoznear + 50;
	}
	else
	{
		z = f_znear;
		rhw = f_moneoznear;
	}

	x = float(x_y >> 16);
	y = float(x_y & 0xFFFF);
	w = float(height_width & 0xFFFF);
	h = float(height_width >> 16);

	v[0].sx = x;
	v[0].sy = y;
	v[0].sz = z;
	v[0].rhw = rhw;
	v[0].color = col;
	v[0].specular = 0xFF000000;

	v[1].sx = x + w + 1;
	v[1].sy = y;
	v[1].sz = z;
	v[1].rhw = rhw;
	v[1].color = col;
	v[1].specular = 0xFF000000;

	v[2].sx = x + w + 1;
	v[2].sy = y + h + 1;
	v[2].sz = z;
	v[2].rhw = rhw;
	v[2].color = col;
	v[2].specular = 0xFF000000;

	v[3].sx = x;
	v[3].sy = y + h + 1;
	v[3].sz = z;
	v[3].rhw = rhw;
	v[3].color = col;
	v[3].specular = 0xFF000000;

	tex.drawtype = drawtype;
	tex.flag = 0;
	tex.tpage = 0;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawFlash()
{
	long r, g, b;

	r = ((FlashFadeR * FlashFader) >> 5) & 0xFF;
	g = ((FlashFadeG * FlashFader) >> 5) & 0xFF;
	b = ((FlashFadeB * FlashFader) >> 5) & 0xFF;
	DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(r, g, b, 0x62), 1, 0);
	DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(r, g, b, 0xFF), 2, 0);
}

void DrawDebris()
{
	DEBRIS_STRUCT* dptr;
	TEXTURESTRUCT* tex;
	D3DTLVERTEX v[3];
	long* XY;
	long* Z;
	long* offsets;
	long r, g, b, c;
	ushort drawbak;

	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 256; i++)
	{
		dptr = &debris[i];

		if (!dptr->On)
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(dptr->x, dptr->y, dptr->z);
		phd_RotY(dptr->YRot << 8);
		phd_RotX(dptr->XRot << 8);

		offsets[0] = dptr->XYZOffsets1[0];
		offsets[1] = dptr->XYZOffsets1[1];
		offsets[2] = dptr->XYZOffsets1[2];
		XY[0] = long(aMXPtr[M03] + aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2]);
		XY[1] = long(aMXPtr[M13] + aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2]);
		Z[0] = long(aMXPtr[M23] + aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2]);

		offsets[0] = dptr->XYZOffsets2[0];
		offsets[1] = dptr->XYZOffsets2[1];
		offsets[2] = dptr->XYZOffsets2[2];
		XY[2] = long(aMXPtr[M03] + aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2]);
		XY[3] = long(aMXPtr[M13] + aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2]);
		Z[1] = long(aMXPtr[M23] + aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2]);

		offsets[0] = dptr->XYZOffsets3[0];
		offsets[1] = dptr->XYZOffsets3[1];
		offsets[2] = dptr->XYZOffsets3[2];
		XY[4] = long(aMXPtr[M03] + aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2]);
		XY[5] = long(aMXPtr[M13] + aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2]);
		Z[2] = long(aMXPtr[M23] + aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2]);

		setXYZ3(v, XY[0], XY[1], Z[0], XY[2], XY[3], Z[1], XY[4], XY[5], Z[2], clipflags);
		phd_PopMatrix();

		c = dptr->color1 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[0].color);

		c = dptr->color2 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[1].color);

		c = dptr->color3 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[2].color);

		v[0].color |= 0xFF000000;
		v[1].color |= 0xFF000000;
		v[2].color |= 0xFF000000;
		v[0].specular |= 0xFF000000;
		v[1].specular |= 0xFF000000;
		v[2].specular |= 0xFF000000;

		tex = &textinfo[(long)dptr->TextInfo & 0x7FFF];
		drawbak = tex->drawtype;

		if (dptr->flags & 1)
			tex->drawtype = 2;

		if (!tex->drawtype)
			AddTriZBuffer(v, 0, 1, 2, tex, 1);
		else if (tex->drawtype <= 2)
			AddTriSorted(v, 0, 1, 2, tex, 1);

		tex->drawtype = drawbak;
	}
}

void DrawBlood()
{
	BLOOD_STRUCT* bptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	FVECTOR pos;
	long* XY;
	long* Z;
	long* offsets;
	float perspz;
	ulong r, col;
	long size, s, c;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 15];
	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 32; i++)
	{
		bptr = &blood[i];

		if (!bptr->On)
			continue;

		dx = bptr->x - lara_item->pos.x_pos;
		dy = bptr->y - lara_item->pos.y_pos;
		dz = bptr->z - lara_item->pos.z_pos;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
			continue;

		offsets[0] = dx;
		offsets[1] = dy;
		offsets[2] = dz;
		pos.x = offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03];
		pos.y = offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13];
		pos.z = offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23];
		perspz = f_persp / pos.z;
		XY[0] = long(pos.x * perspz + f_centerx);
		XY[1] = long(pos.y * perspz + f_centery);
		Z[0] = (long)pos.z;

		if (Z[0] <= 0 || Z[0] >= 0x5000)
			continue;

		size = ((phd_persp * bptr->Size) << 1) / Z[0];

		if (size > (bptr->Size << 1))
			size = (bptr->Size << 1);
		else if (size < 4)
			size = 4;

		size <<= 1;
		ang = bptr->RotAng << 1;
		s = (size * rcossin_tbl[ang]) >> (W2V_SHIFT - 2);
		c = (size * rcossin_tbl[ang + 1]) >> (W2V_SHIFT - 2);
		x1 = c + XY[0] - s;
		y1 = XY[1] - c - s;
		x2 = s + c + XY[0];
		y2 = c + XY[1] - s;
		x3 = s + XY[0] - c;
		y3 = s + XY[1] + c;
		x4 = XY[0] - c - s;
		y4 = XY[1] - c + s;
		setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);

		if (Z[0] <= 0x3000)
			col = RGBA(bptr->Shade, 0, 0, 0xFF);
		else
		{
			r = ((0x5000 - Z[0]) * bptr->Shade) >> 13;
			col = RGBA(r, 0, 0, 0xFF);
		}

		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[3].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
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

	phd_PopMatrix();
}

void DrawDrips()
{
	DRIP_STRUCT* drip;
	FVECTOR vec;
	D3DTLVERTEX v[2];
	TEXTURESTRUCT tex;
	long* XY;
	long* Z;
	long* pos;
	float zv;
	long x0, y0, z0, x1, y1, z1, r, g, b;

	tex.drawtype = 2;
	tex.tpage = 0;
	tex.flag = 0;

	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	pos = (long*)&tsv_buffer[1024];

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	for (int i = 0; i < 32; i++)
	{
		drip = &Drips[i];

		if (!drip->On)
			continue;

		pos[0] = drip->x - lara_item->pos.x_pos;
		pos[1] = drip->y - lara_item->pos.y_pos;
		pos[2] = drip->z - lara_item->pos.z_pos;

		if (pos[0] < -0x5000 || pos[0] > 0x5000 || pos[1] < -0x5000 || pos[1] > 0x5000 || pos[2] < -0x5000 || pos[2] > 0x5000)
			continue;

		vec.x = pos[0] * aMXPtr[M00] + pos[1] * aMXPtr[M01] + pos[2] * aMXPtr[M02] + aMXPtr[M03];
		vec.y = pos[0] * aMXPtr[M10] + pos[1] * aMXPtr[M11] + pos[2] * aMXPtr[M12] + aMXPtr[M13];
		vec.z = pos[0] * aMXPtr[M20] + pos[1] * aMXPtr[M21] + pos[2] * aMXPtr[M22] + aMXPtr[M23];

		zv = f_persp / vec.z;
		XY[0] = long(vec.x * zv + f_centerx);
		XY[1] = long(vec.y * zv + f_centery);
		Z[0] = (long)vec.z;

		pos[1] -= GetFixedScale(drip->Yvel >> 6);

		if (room[drip->RoomNumber].flags & ROOM_NOT_INSIDE)
		{
			pos[0] -= SmokeWindX >> 1;
			pos[1] -= SmokeWindZ >> 1;
		}

		vec.x = pos[0] * aMXPtr[M00] + pos[1] * aMXPtr[M01] + pos[2] * aMXPtr[M02] + aMXPtr[M03];
		vec.y = pos[0] * aMXPtr[M10] + pos[1] * aMXPtr[M11] + pos[2] * aMXPtr[M12] + aMXPtr[M13];
		vec.z = pos[0] * aMXPtr[M20] + pos[1] * aMXPtr[M21] + pos[2] * aMXPtr[M22] + aMXPtr[M23];

		zv = f_persp / vec.z;
		XY[2] = long(vec.x * zv + f_centerx);
		XY[3] = long(vec.y * zv + f_centery);
		Z[1] = (long)vec.z;

		if (!Z[0])
			continue;

		if (Z[0] > 0x5000)
		{
			drip->On = 0;
			continue;
		}

		x0 = XY[0];
		y0 = XY[1];
		z0 = Z[0];
		x1 = XY[2];
		y1 = XY[3];
		z1 = Z[1];

		if (ClipLine(x0, y0, z0, x1, y1, z1, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
		{
			r = drip->R << 2;
			g = drip->G << 2;
			b = drip->B << 2;

			v[0].sx = (float)x0;
			v[0].sy = (float)y0;
			v[0].sz = (float)z0;
			v[0].rhw = f_mpersp / v[0].sz * f_moneopersp;
			v[0].color = RGBA(r, g, b, 0xFF);
			v[0].specular = 0xFF000000;

			r >>= 1;
			g >>= 1;
			b >>= 1;

			v[1].sx = (float)x1;
			v[1].sy = (float)y1;
			v[1].sz = (float)z1;
			v[1].rhw = f_mpersp / v[1].sz * f_moneopersp;
			v[1].color = RGBA(r, g, b, 0xFF);
			v[1].specular = 0xFF000000;

			clipflags[0] = 0;
			clipflags[1] = 0;
			clipflags[2] = 0;
			clipflags[3] = 0;
			AddPolyLine(v, &tex, (float)GetFixedScale(1));
		}
	}

	phd_PopMatrix();
}

void DoUwEffect()
{
	UWEFFECTS* p;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX* v;
	TEXTURESTRUCT tex;
	FVECTOR pos;
	long* XY;
	long* Z;
	long* offsets;
	float perspz;
	long num_alive, rad, ang, x, y, z, size, col, yv;

	if (tomb5.uw_dust == 1)
		return;

	v = aVertexBuffer;
	num_alive = 0;

	for (int i = 0; i < 256; i++)
	{
		p = &uwdust[i];

		if (!p->x && num_alive < 16)
		{
			num_alive++;
			rad = GetRandomDraw() & 0xFFF;
			ang = GetRandomDraw() & 0x1FFE;
			x = (rad * rcossin_tbl[ang]) >> (W2V_SHIFT - 2);
			y = (GetRandomDraw() & 0x7FF) - 1024;
			z = (rad * rcossin_tbl[ang + 1]) >> (W2V_SHIFT - 2);
			p->x = lara_item->pos.x_pos + x;
			p->y = lara_item->pos.y_pos + y;
			p->z = lara_item->pos.z_pos + z;

			if (IsRoomOutside(p->x, p->y, p->z) < 0 || !(room[IsRoomOutsideNo].flags & ROOM_UNDERWATER))
			{
				p->x = 0;
				continue;
			}

			p->stopped = 1;
			p->life = (GetRandomDraw() & 7) + 16;
			p->xv = GetRandomDraw() & 3;

			if (p->xv == 2)
				p->xv = -1;

			p->yv = ((GetRandomDraw() & 7) + 8) << 3;
			p->zv = GetRandomDraw() & 3;

			if (p->zv == 2)
				p->zv = -1;
		}

		p->x += p->xv;
		p->y += (p->yv & ~7) >> 6;
		p->z += p->zv;

		if (!p->life)
		{
			p->x = 0;
			continue;
		}

		p->life--;

		if ((p->yv & 7) < 7)
			p->yv++;
	}

	if (tomb5.uw_dust == 2)
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 15];
	else
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];

	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	for (int i = 0; i < 256; i++)
	{
		p = &uwdust[i];

		if (!p->x)
			continue;

		x = p->x - lara_item->pos.x_pos;
		y = p->y - lara_item->pos.y_pos;
		z = p->z - lara_item->pos.z_pos;
		offsets[0] = x;
		offsets[1] = y;
		offsets[2] = z;
		pos.x = offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03];
		pos.y = offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13];
		pos.z = offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23];
		perspz = f_persp / pos.z;
		XY[0] = long(pos.x * perspz + f_centerx);
		XY[1] = long(pos.y * perspz + f_centery);
		Z[0] = (long)pos.z;

		if (Z[0] < 32)
		{
			if (p->life > 16)
				p->life = 16;

			continue;
		}

		if (XY[0] < phd_winxmin || XY[0] > phd_winxmax || XY[1] < phd_winymin || XY[1] > phd_winymax)
			continue;

		size = phd_persp * (p->yv >> 3) / (Z[0] >> 2);

		if (size < 4)
			size = 4;
		else if (size > 16)
			size = 16;

		size = (size * 0x2AAB) >> 15;
		size = GetFixedScale(size) >> 1;

		if ((p->yv & 7) == 7)
		{
			if (p->life > 18)
				col = 0xFF404040;
			else
				col = (p->life | ((p->life | ((p->life | 0xFFFFFFC0) << 8)) << 8)) << 2;	//decipher me
		}
		else
		{
			yv = (p->yv & 7) << 2;
			col = (yv | ((yv | ((yv | 0xFFFFFF80) << 8)) << 8)) << 1;	//decipher me
		}

		setXY4(v, XY[0] + size, XY[1] - (size << 1), XY[0] + size, XY[1] + size,
			XY[0] - (size << 1), XY[1] + size, XY[0] - (size << 1), XY[1] - (size << 1), Z[0], clipflags);

		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[3].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;

		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;

		tex.u1 = sprite->x2;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y2;
		tex.u3 = sprite->x1;
		tex.v3 = sprite->y2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y1;

		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
	}

	phd_PopMatrix();
}

void DrawBubbles()
{
	BUBBLE_STRUCT* bubble;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	FVECTOR pos;
	long* XY;
	long* Z;
	long* offsets;
	float zv;
	long dx, dy, dz, size, x1, y1, x2, y2;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	bubble = Bubbles;

	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 40; i++)
	{
		if (!bubble->size)
		{
			bubble++;
			continue;
		}

		dx = bubble->pos.x - lara_item->pos.x_pos;
		dy = bubble->pos.y - lara_item->pos.y_pos;
		dz = bubble->pos.z - lara_item->pos.z_pos;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
		{
			bubble->size = 0;
			bubble++;
			continue;
		}

		offsets[0] = dx;
		offsets[1] = dy;
		offsets[2] = dz;
		pos.x = offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03];
		pos.y = offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13];
		pos.z = offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23];
		zv = f_persp / pos.z;
		XY[0] = long(pos.x * zv + f_centerx);
		XY[1] = long(pos.y * zv + f_centery);
		Z[0] = (long)pos.z;

		if (Z[0] < 32)
		{
			bubble++;
			continue;
		}

		if (Z[0] > 0x5000)
		{
			bubble->size = 0;
			bubble++;
			continue;
		}

		size = phd_persp * (bubble->size >> 1) / Z[0];

		if (size > 128)
		{
			bubble->size = 0;
			continue;
		}

		if (size < 4)
			size = 4;

		size >>= 1;

		x1 = XY[0] - size;
		y1 = XY[1] - size;
		x2 = XY[0] + size;
		y2 = XY[1] + size;

		if (x2 < phd_winxmin || x1 >= phd_winxmax || y2 < phd_winymin || y1 >= phd_winymax)
		{
			bubble++;
			continue;
		}

		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 13];
		setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		v[0].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[1].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[2].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[3].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		bubble++;
	}

	phd_PopMatrix();
}

void DrawShockwaves()
{
	SHOCKWAVE_STRUCT* wave;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX vtx[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR p1, p2, p3;
	long* XY;
	long* Z;
	long* offsets;
	long v, x1, y1, x2, y2, x3, y3, x4, y4, r, g, b, c;
	short rad;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 8];
	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 16; i++)
	{
		wave = &ShockWaves[i];

		if (!wave->life)
			continue;

		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];
		phd_PushMatrix();
		phd_TranslateAbs(wave->x, wave->y, wave->z);
		phd_RotX(wave->XRot);
		offsets[1] = 0;
		offsets[5] = 0;
		offsets[9] = 0;
		rad = wave->OuterRad;

		for (int j = 0; j < 2; j++)
		{
			offsets[0] = (rad * phd_sin(0)) >> W2V_SHIFT;
			offsets[2] = (rad * phd_cos(0)) >> W2V_SHIFT;
			offsets[4] = (rad * phd_sin(0x1000)) >> W2V_SHIFT;
			offsets[6] = (rad * phd_cos(0x1000)) >> W2V_SHIFT;
			offsets[8] = (rad * phd_sin(0x2000)) >> W2V_SHIFT;
			offsets[10] = (rad * phd_cos(0x2000)) >> W2V_SHIFT;

			for (int k = 1; k < 7; k++)
			{
				v = k * 0x3000;

				p1.x = long(offsets[0] * aMXPtr[M00] + offsets[1] * aMXPtr[M01] + offsets[2] * aMXPtr[M02] + aMXPtr[M03]);
				p1.y = long(offsets[0] * aMXPtr[M10] + offsets[1] * aMXPtr[M11] + offsets[2] * aMXPtr[M12] + aMXPtr[M13]);
				p1.z = long(offsets[0] * aMXPtr[M20] + offsets[1] * aMXPtr[M21] + offsets[2] * aMXPtr[M22] + aMXPtr[M23]);

				p2.x = long(offsets[4] * aMXPtr[M00] + offsets[5] * aMXPtr[M01] + offsets[6] * aMXPtr[M02] + aMXPtr[M03]);
				p2.y = long(offsets[4] * aMXPtr[M10] + offsets[5] * aMXPtr[M11] + offsets[6] * aMXPtr[M12] + aMXPtr[M13]);
				p2.z = long(offsets[4] * aMXPtr[M20] + offsets[5] * aMXPtr[M21] + offsets[6] * aMXPtr[M22] + aMXPtr[M23]);

				p3.x = long(offsets[8] * aMXPtr[M00] + offsets[9] * aMXPtr[M01] + offsets[10] * aMXPtr[M02] + aMXPtr[M03]);
				p3.y = long(offsets[8] * aMXPtr[M10] + offsets[9] * aMXPtr[M11] + offsets[10] * aMXPtr[M12] + aMXPtr[M13]);
				p3.z = long(offsets[8] * aMXPtr[M20] + offsets[9] * aMXPtr[M21] + offsets[10] * aMXPtr[M22] + aMXPtr[M23]);

				offsets[0] = (rad * phd_sin(v)) >> W2V_SHIFT;
				offsets[2] = (rad * phd_cos(v)) >> W2V_SHIFT;
				offsets[4] = (rad * phd_sin(v + 0x1000)) >> W2V_SHIFT;
				offsets[6] = (rad * phd_cos(v + 0x1000)) >> W2V_SHIFT;
				offsets[8] = (rad * phd_sin(v + 0x2000)) >> W2V_SHIFT;
				offsets[10] = (rad * phd_cos(v + 0x2000)) >> W2V_SHIFT;

				XY[0] = p1.x;
				XY[1] = p1.y;
				Z[0] = p1.z;

				XY[2] = p2.x;
				XY[3] = p2.y;
				Z[1] = p2.z;

				XY[4] = p3.x;
				XY[5] = p3.y;
				Z[2] = p3.z;

				XY += 6;
				Z += 3;
			}

			rad = wave->InnerRad;
		}

		phd_PopMatrix();
		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];

		for (int j = 0; j < 16; j++)
		{
			x1 = XY[0];
			y1 = XY[1];
			x2 = XY[2];
			y2 = XY[3];
			x3 = XY[36];
			y3 = XY[37];
			x4 = XY[38];
			y4 = XY[39];
			setXYZ4(vtx, x1, y1, Z[0], x2, y2, Z[1], x4, y4, Z[19], x3, y3, Z[18], clipflags);

			r = wave->r;
			g = wave->g;
			b = wave->b;

			if (wave->life < 8)
			{
				r = (r * wave->life) >> 3;
				g = (g * wave->life) >> 3;
				b = (b * wave->life) >> 3;
			}

			c = RGBA(b, g, r, 0xFF);
			vtx[0].color = c;
			vtx[1].color = c;
			vtx[2].color = c;
			vtx[3].color = c;
			vtx[0].specular = 0xFF000000;
			vtx[1].specular = 0xFF000000;
			vtx[2].specular = 0xFF000000;
			vtx[3].specular = 0xFF000000;

			tex.drawtype = 2;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y2;
			tex.u2 = sprite->x2;
			tex.v2 = sprite->y2;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y1;
			tex.u4 = sprite->x1;
			tex.v4 = sprite->y1;
			AddQuadSorted(vtx, 0, 1, 2, 3, &tex, 1);

			XY += 2;
			Z++;
		}
	}
}

void DrawLightning()
{
	LIGHTNING_STRUCT* l;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX* pV;
	TEXTURESTRUCT tex;
	float* pPos;
	float* pVtx;
	short* pC;
	float pos[32 * 3];
	float vtx[32 * 6];
	float px, py, pz, px1, py1, pz1, px2, py2, pz2, px3, py3, pz3, n, nx, ny, nz, xAdd, yAdd, zAdd;
	float mx, my, mz, zv, size, size2, step, s, c, uAdd;
	long r1, r2, r3, col, r, g, b, lp;
	static long rand = 0xD371F947;
	short clip[32];
	short clipFlag;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	aSetViewMatrix();

	for (int i = 0; i < 16; i++)
	{
		l = &Lightning[i];

		if (!l->Life)
			continue;

		px = (float)l->Point[0].x;
		py = (float)l->Point[0].y;
		pz = (float)l->Point[0].z;

		px1 = (float)l->Point[1].x - px;
		py1 = (float)l->Point[1].y - py;
		pz1 = (float)l->Point[1].z - pz;

		px2 = (float)l->Point[2].x - px;
		py2 = (float)l->Point[2].y - py;
		pz2 = (float)l->Point[2].z - pz;

		px3 = (float)l->Point[3].x - px;
		py3 = (float)l->Point[3].y - py;
		pz3 = (float)l->Point[3].z - pz;

		px = float(l->Point[0].x - lara_item->pos.x_pos);
		py = float(l->Point[0].y - lara_item->pos.y_pos);
		pz = float(l->Point[0].z - lara_item->pos.z_pos);

		r1 = rand;
		n = 0;
		pPos = pos;

		for (lp = 0; lp < 32; lp++)
		{
			if (!lp || lp == 31)
			{
				xAdd = 0;
				yAdd = 0;
				zAdd = 0;
			}
			else
			{
				r2 = 0x41C64E6D * r1 + 0x3039;
				r3 = 0x41C64E6D * r2 + 0x3039;
				r1 = 0x41C64E6D * r3 + 0x3039;
				xAdd = float(((r2 >> 10) & 0xF) - 8);
				yAdd = float(((r3 >> 10) & 0xF) - 8);
				zAdd = float(((r1 >> 10) & 0xF) - 8);
			}

			nx = (1.0F - n) * (1.0F - n) * n * 4.0F;
			ny = (1.0F - n) * (n * n) * 4.0F;
			nz = ((n + n) - 1.0F) * (n * n);
			pPos[0] = nx * px1 + ny * px2 + nz * px3 + px + xAdd;
			pPos[1] = nx * py1 + ny * py2 + nz * py3 + py + yAdd;
			pPos[2] = nx * pz1 + ny * pz2 + nz * pz3 + pz + zAdd;

			n += (1.0F / 32.0F);
			pPos += 3;
		}

		rand = r1;
		pPos = pos;
		pVtx = vtx;
		pC = clip;

		for (lp = 0; lp < 32; lp++)
		{
			px = pPos[0];
			py = pPos[1];
			pz = pPos[2];
			mx = px * D3DMView._11 + py * D3DMView._21 + pz * D3DMView._31 + D3DMView._41;
			my = px * D3DMView._12 + py * D3DMView._22 + pz * D3DMView._32 + D3DMView._42;
			mz = px * D3DMView._13 + py * D3DMView._23 + pz * D3DMView._33 + D3DMView._43;

			clipFlag = 0;

			if (mz < f_mznear + 2.0F)
				clipFlag = -128;

			zv = f_mpersp / mz;

			pVtx[0] = mx * zv + f_centerx;
			pVtx[1] = my * zv + f_centery;
			pVtx[2] = zv * f_moneopersp;

			if (pVtx[0] < phd_winxmin)
				clipFlag++;
			else if (pVtx[0] > phd_winxmax)
				clipFlag += 2;

			if (pVtx[1] < phd_winymin)
				clipFlag += 4;
			else if (pVtx[1] > phd_winymax)
				clipFlag += 8;

			*pC++ = clipFlag;
			pVtx[3] = mz;
			pVtx[4] = mx;
			pVtx[5] = my;

			pPos += 3;
			pVtx += 6;
		}

		size = float(l->Size >> 1);
		step = 0;

		if (l->Flags & 8)
		{
			step = size * 0.125F;
			size = 0;
		}
		else if (l->Flags & 4)
			step = -(size * (1.0F / 32.0F));

		pPos = pos;
		pVtx = vtx;

		for (lp = 0; lp < 32; lp++)
		{
			r = phd_atan(long(pVtx[6] - pVtx[0]), long(pVtx[7] - pVtx[1]));
			s = fSin(-r);
			c = fCos(-r);

			if (size <= 0)
				size2 = 2.0F;
			else
				size2 = size;

			zv = f_mpersp / pVtx[3] * size2;

			pPos[0] = zv * s;
			pPos[1] = zv * c;

			size += step;

			if (l->Flags & 8 && lp == 8)
			{
				if (l->Flags & 4)
					step = float(l->Size >> 1) * (-1.0F / 28.0F);
				else
					step = 0;

				l->Flags &= ~8;
			}

			pPos += 3;
			pVtx += 6;
		}

		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 28];
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;

		uAdd = float(31 - 4 * (aWibble & 7)) * (1.0F / 256.0F);

		if (l->Life < 16)
		{
			r = (l->Life * l->r) >> 4;
			g = (l->Life * l->g) >> 4;
			b = (l->Life * l->b) >> 4;
		}
		else
		{
			r = l->r;
			g = l->g;
			b = l->b;
		}

		col = RGBONLY(b, g, r);

		pPos = pos;
		pVtx = vtx;
		pC = clip;
		pV = aVertexBuffer;
		zv = f_mznear + 128.0F;

		for (lp = 0; lp < 31; lp++)
		{
			tex.u1 = (float(lp & 3) * 8.0F * (1.0F / 256.0F) + sprite->x1) + (1.0F / 512.0F) + uAdd;
			tex.v1 = sprite->y1;
			tex.u2 = tex.u1;
			tex.v2 = sprite->y2;
			tex.u3 = (float(lp & 3) * 8.0F * (1.0F / 256.0F) + sprite->x1) + (8.0F / 256.0F) + uAdd;
			tex.v3 = sprite->y2;
			tex.u4 = tex.u3;
			tex.v4 = sprite->y1;

			if (pVtx[3] >= zv && pVtx[9] >= zv)
			{
				pV[0].sx = pVtx[0] - pPos[0];
				pV[0].sy = pVtx[1] - pPos[1];
				pV[0].rhw = pVtx[2];
				pV[0].sz = pVtx[3];
				pV[0].tu = pVtx[4];
				pV[0].tv = pVtx[5];
				pV[0].color = col;
				pV[0].specular = 0xFF000000;

				pV[1].sx = pVtx[0] + pPos[0];
				pV[1].sy = pVtx[1] + pPos[1];
				pV[1].rhw = pVtx[2];
				pV[1].sz = pVtx[3];
				pV[1].tu = pVtx[4];
				pV[1].tv = pVtx[5];
				pV[1].color = col;
				pV[1].specular = 0xFF000000;

				pV[2].sx = pVtx[6] - pPos[3];
				pV[2].sy = pVtx[7] - pPos[4];
				pV[2].rhw = pVtx[8];
				pV[2].sz = pVtx[9];
				pV[2].tu = pVtx[10];
				pV[2].tv = pVtx[11];
				pV[2].color = col;
				pV[2].specular = 0xFF000000;

				pV[3].sx = pVtx[6] + pPos[3];
				pV[3].sy = pVtx[7] + pPos[4];
				pV[3].rhw = pVtx[8];
				pV[3].sz = pVtx[9];
				pV[3].tu = pVtx[10];
				pV[3].tv = pVtx[11];
				pV[3].color = col;
				pV[3].specular = 0xFF000000;

				clipflags[0] = pC[0];
				clipflags[1] = pC[0];
				clipflags[2] = pC[1];
				clipflags[3] = pC[1];

				AddQuadSorted(pV, 1, 0, 2, 3, &tex, 1);
				AddQuadSorted(pV, 1, 0, 2, 3, &tex, 1);
			}

			pPos += 3;
			pVtx += 6;
			pC++;
		}
	}

	phd_PopMatrix();
}

void S_DrawSplashes()
{
	SPLASH_STRUCT* splash;
	RIPPLE_STRUCT* ripple;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long* XY;
	long* Z;
	long* offsets;
	uchar* links;
	ulong c0, c1;
	long x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, linkNum, r, g, b;
	short rads[6];
	short yVals[6];

	offsets = (long*)&tsv_buffer[1024];

	for (int i = 0; i < 4; i++)
	{
		splash = &splashes[i];

		if (!(splash->flags & 1))
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(splash->x, splash->y, splash->z);
		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];

		rads[0] = splash->InnerRad;
		rads[1] = splash->InnerRad + splash->InnerSize;
		rads[2] = splash->MiddleRad;
		rads[3] = splash->MiddleRad + splash->MiddleSize;
		rads[4] = splash->OuterRad;
		rads[5] = splash->OuterRad + splash->OuterSize;

		yVals[0] = 0;
		yVals[1] = splash->InnerY;
		yVals[2] = 0;
		yVals[3] = splash->MiddleY;
		yVals[4] = 0;
		yVals[5] = 0;

		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 0x10000; k += 0x2000)
			{
				offsets[0] = (rads[j] * phd_sin(k)) >> (W2V_SHIFT - 1);
				offsets[1] = yVals[j] >> 3;
				offsets[2] = (rads[j] * phd_cos(k)) >> (W2V_SHIFT - 1);
				*XY++ = long(aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2] + aMXPtr[M03]);
				*XY++ = long(aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2] + aMXPtr[M13]);
				*Z++ = long(aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2] + aMXPtr[M23]);
				Z++;
			}
		}

		phd_PopMatrix();
		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];

		for (int j = 0; j < 3; j++)
		{
			if (j == 2 || (!j && splash->flags & 4) || (j == 1 && splash->flags & 8))
				sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 4 + ((wibble >> 4) & 3)];
			else
				sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 8];

			links = SplashLinks;
			linkNum = j << 5;

			for (int k = 0; k < 8; k++)
			{
				x1 = XY[links[0] + linkNum];
				y1 = XY[links[0] + linkNum + 1];
				z1 = Z[links[0] + linkNum];
				links++;

				x2 = XY[links[0] + linkNum];
				y2 = XY[links[0] + linkNum + 1];
				z2 = Z[links[0] + linkNum];
				links++;

				x3 = XY[links[0] + linkNum];
				y3 = XY[links[0] + linkNum + 1];
				z3 = Z[links[0] + linkNum];
				links++;

				x4 = XY[links[0] + linkNum];
				y4 = XY[links[0] + linkNum + 1];
				z4 = Z[links[0] + linkNum];
				links++;

				setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);

				r = splash->life << 1;
				g = splash->life << 1;
				b = splash->life << 1;

				if (r > 255)
					r = 255;

				if (g > 255)
					g = 255;

				if (b > 255)
					b = 255;

				c0 = RGBA(r, g, b, 0xFF);

				r = (splash->life - (splash->life >> 2)) << 1;
				g = (splash->life - (splash->life >> 2)) << 1;
				b = (splash->life - (splash->life >> 2)) << 1;

				if (r > 255)
					r = 255;

				if (g > 255)
					g = 255;

				if (b > 255)
					b = 255;

				c1 = RGBA(r, g, b, 0xFF);

				v[0].color = c0;
				v[1].color = c0;
				v[2].color = c1;
				v[3].color = c1;
				v[0].specular = 0xFF000000;
				v[1].specular = 0xFF000000;
				v[2].specular = 0xFF000000;
				v[3].specular = 0xFF000000;
				tex.drawtype = 2;
				tex.flag = 0;
				tex.tpage = sprite->tpage;
				tex.u1 = sprite->x1;
				tex.v1 = sprite->y1;
				tex.u2 = sprite->x2;
				tex.v2 = sprite->y1;
				tex.v3 = sprite->y2;
				tex.u3 = sprite->x2;
				tex.u4 = sprite->x1;
				tex.v4 = sprite->y2;
				AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
			}
		}
	}

	for (int i = 0; i < MAX_RIPPLES; i++)
	{
		ripple = &ripples[i];

		if (!(ripple->flags & 1))
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(ripple->x, ripple->y, ripple->z);

		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];

		offsets[0] = -ripple->size;
		offsets[1] = 0;
		offsets[2] = -ripple->size;
		*XY++ = long(aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2] + aMXPtr[M03]);
		*XY++ = long(aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2] + aMXPtr[M13]);
		*Z++ = long(aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2] + aMXPtr[M23]);
		Z++;

		offsets[0] = -ripple->size;
		offsets[1] = 0;
		offsets[2] = ripple->size;
		*XY++ = long(aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2] + aMXPtr[M03]);
		*XY++ = long(aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2] + aMXPtr[M13]);
		*Z++ = long(aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2] + aMXPtr[M23]);
		Z++;

		offsets[0] = ripple->size;
		offsets[1] = 0;
		offsets[2] = ripple->size;
		*XY++ = long(aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2] + aMXPtr[M03]);
		*XY++ = long(aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2] + aMXPtr[M13]);
		*Z++ = long(aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2] + aMXPtr[M23]);
		Z++;

		offsets[0] = ripple->size;
		offsets[1] = 0;
		offsets[2] = -ripple->size;
		*XY++ = long(aMXPtr[M00] * offsets[0] + aMXPtr[M01] * offsets[1] + aMXPtr[M02] * offsets[2] + aMXPtr[M03]);
		*XY++ = long(aMXPtr[M10] * offsets[0] + aMXPtr[M11] * offsets[1] + aMXPtr[M12] * offsets[2] + aMXPtr[M13]);
		*Z++ = long(aMXPtr[M20] * offsets[0] + aMXPtr[M21] * offsets[1] + aMXPtr[M22] * offsets[2] + aMXPtr[M23]);
		Z++;

		phd_PopMatrix();

		XY = (long*)&tsv_buffer[0];
		Z = (long*)&tsv_buffer[512];

		if (ripple->flags & 0x20)
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index];
		else
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 9];

		x1 = *XY++;
		y1 = *XY++;
		z1 = *Z++;
		Z++;

		x2 = *XY++;
		y2 = *XY++;
		z2 = *Z++;
		Z++;

		x3 = *XY++;
		y3 = *XY++;
		z3 = *Z++;
		Z++;

		x4 = *XY++;
		y4 = *XY++;
		z4 = *Z++;
		Z++;

		setXYZ4(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, clipflags);

		if (ripple->flags & 0x10)
		{
			if (ripple->flags & 0x20)
			{
				if (ripple->init)
				{
					r = (ripple->init >> 1) << 1;
					g = 0;
					b = (ripple->init >> 4) << 1;
				}
				else
				{
					r = (ripple->life >> 1) << 1;
					g = 0;
					b = (ripple->life >> 4) << 1;
				}
			}
			else
			{
				if (ripple->init)
				{
					r = ripple->init << 1;
					g = ripple->init << 1;
					b = ripple->init << 1;
				}
				else
				{
					r = ripple->life << 1;
					g = ripple->life << 1;
					b = ripple->life << 1;
				}
			}
		}
		else
		{
			if (ripple->init)
			{
				r = ripple->init << 2;
				g = ripple->init << 2;
				b = ripple->init << 2;
			}
			else
			{
				r = ripple->life << 2;
				g = ripple->life << 2;
				b = ripple->life << 2;
			}
		}

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c0 = RGBA(r, g, b, 0xFF);

		v[0].color = c0;
		v[1].color = c0;
		v[2].color = c0;
		v[3].color = c0;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y1;
		tex.v3 = sprite->y2;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
	}
}

void DrawRope(ROPE_STRUCT* rope)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long dx, dy, d, b, w, spec;
	long x1, y1, z1, x2, y2, z2, x3, y3, x4, y4;

	ProjectRopePoints(rope);
	dx = rope->Coords[1][0] - rope->Coords[0][0];
	dy = rope->Coords[1][1] - rope->Coords[0][1];
	d = SQUARE(dx) + SQUARE(dy);
	d = phd_sqrt(abs(d));

	dx <<= 16;
	dy <<= 16;
	d <<= 16;

	if (d)
	{
		d = ((0x1000000 / (d >> 8)) << 8) >> 8;
		b = dx;
		dx = ((__int64)-dy * (__int64)d) >> 16;
		dy = ((__int64)b * (__int64)d) >> 16;
	}

	w = 0x60000;

	if (rope->Coords[0][2])
	{
		w = 0x60000 * phd_persp / rope->Coords[0][2];

		if (w < 1)
			w = 1;
	}

	w <<= 16;
	dx = (((__int64)dx * (__int64)w) >> 16) >> 16;
	dy = (((__int64)dy * (__int64)w) >> 16) >> 16;
	x1 = rope->Coords[0][0] - dx;
	y1 = rope->Coords[0][1] - dy;
	z1 = rope->Coords[0][2] >> W2V_SHIFT;
	x4 = rope->Coords[0][0] + dx;
	y4 = rope->Coords[0][1] + dy;

	for (int i = 0; i < 23; i++)
	{
		dx = rope->Coords[i + 1][0] - rope->Coords[i][0];
		dy = rope->Coords[i + 1][1] - rope->Coords[i][1];
		d = SQUARE(dx) + SQUARE(dy);
		d = phd_sqrt(abs(d));

		dx <<= 16;
		dy <<= 16;
		d <<= 16;

		if (d)
		{
			d = ((0x1000000 / (d >> 8)) << 8) >> 8;
			b = dx;
			dx = ((__int64)-dy * (__int64)d) >> 16;
			dy = ((__int64)b * (__int64)d) >> 16;
		}

		w = 0x60000;

		if (rope->Coords[i][2])
		{
			w = 0x60000 * phd_persp / rope->Coords[i][2];

			if (w < 3)
				w = 3;
		}

		w <<= 16;
		dx = (((__int64)dx * (__int64)w) >> 16) >> 16;
		dy = (((__int64)dy * (__int64)w) >> 16) >> 16;
		x2 = rope->Coords[i + 1][0] - dx;
		y2 = rope->Coords[i + 1][1] - dy;
		z2 = rope->Coords[i + 1][2] >> W2V_SHIFT;
		x3 = rope->Coords[i + 1][0] + dx;
		y3 = rope->Coords[i + 1][1] + dy;

		if ((double)z1 > f_mznear && (double)z2 > f_mznear)
		{
			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);
			v[0].color = 0xFF7F7F7F;
			v[1].color = 0xFF7F7F7F;
			v[2].color = 0xFF7F7F7F;
			v[3].color = 0xFF7F7F7F;

			spec = 255;

			if (z1 > 0x3000)
				spec = (255 * (0x5000 - z1)) >> 13;

			v[0].specular = spec << 24;
			v[1].specular = spec << 24;
			v[2].specular = spec << 24;
			v[3].specular = spec << 24;
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 16];
			tex.drawtype = 1;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y1;
			tex.u2 = sprite->x1;
			tex.v2 = sprite->y2;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y2;
			tex.u4 = sprite->x2;
			tex.v4 = sprite->y1;
			AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		}

		x1 = x2;
		y1 = y2;
		z1 = z2;
		x4 = x3;
		y4 = y3;
	}
}

void DrawTwogunLaser(TWOGUN_INFO* info)
{
	FVECTOR* pos;
	D3DTLVERTEX* v;
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT tex;
	float* pVtx;
	short* c;
	float vtx[128 * 6];
	float x, y, z, mx, my, mz, zv, uAdd;
	float size, size2;
	long r, g, b, step, col, lp;
	short angle, pz, clipFlag;
	short clip[128];

	if (info->fadein < 8)
	{
		r = (info->fadein * (uchar)info->r) >> 3;
		g = (info->fadein * (uchar)info->g) >> 3;
		b = (info->fadein * (uchar)info->b) >> 3;
	}
	else if (info->life < 16)
	{
		r = (info->life * (uchar)info->r) >> 4;
		g = (info->life * (uchar)info->g) >> 4;
		b = (info->life * (uchar)info->b) >> 4;
	}
	else
	{
		r = (uchar)info->r;
		g = (uchar)info->g;
		b = (uchar)info->b;
	}

	phd_PushMatrix();
	phd_TranslateAbs(info->pos.x_pos, info->pos.y_pos, info->pos.z_pos);
	phd_RotYXZ(info->pos.y_rot, info->pos.x_rot, info->pos.z_rot);
	aSetViewMatrix();

	pos = (FVECTOR*)&tsv_buffer[0];
	size = 0;
	step = info->size << 2;
	pz = 0;
	angle = info->spin;

	for (lp = 0; lp < 8; lp++)
	{
		size2 = size / 2;

		if (size2 > 48)
			size2 = 48;

		pos->x = (size * fSin(angle)) / 2;
		pos->y = (size * fCos(angle)) / 2;
		pos->z = pz;
		pos[1].x = pos->x - size2;
		pos[1].y = pos->y - size2;
		pos[1].z = pz;

		size += step;
		pz += info->length >> 6;
		angle += info->coil;
		pos += 2;
	}

	for (lp = 0; lp < 56; lp++)
	{
		size2 = size / 2;

		if (size2 > 48)
			size2 = 48;

		pos->x = (size * fSin(angle)) / 2;
		pos->y = (size * fCos(angle)) / 2;
		pos->z = pz;
		pos[1].x = pos->x - size2;
		pos[1].y = pos->y - size2;
		pos[1].z = pz;

		if (lp & 1)
			size -= info->size;

		if (size < 4)
			size = 4;

		pz += info->length >> 6;
		angle += info->coil;
		pos += 2;
	}

	pos = (FVECTOR*)&tsv_buffer[0];
	pVtx = vtx;
	c = clip;

	for (lp = 0; lp < 128; lp++)
	{
		x = pos->x;
		y = pos->y;
		z = pos->z;
		mx = D3DMView._11 * x + D3DMView._21 * y + D3DMView._31 * z + D3DMView._41;
		my = D3DMView._12 * x + D3DMView._22 * y + D3DMView._32 * z + D3DMView._42;
		mz = D3DMView._13 * x + D3DMView._23 * y + D3DMView._33 * z + D3DMView._43;

		clipFlag = 0;

		if (mz < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / mz;
			pVtx[0] = mx * zv + f_centerx;
			pVtx[1] = my * zv + f_centery;
			pVtx[2] = f_moneopersp * zv;

			if (pVtx[0] < phd_winxmin)
				clipFlag++;
			else if (pVtx[0] > phd_winxmax)
				clipFlag += 2;

			if (pVtx[1] < phd_winymin)
				clipFlag += 4;
			else if (pVtx[1] > phd_winymax)
				clipFlag += 8;
		}

		*c++ = clipFlag;
		pVtx[3] = mz;
		pVtx[4] = mx;
		pVtx[5] = my;

		pVtx += 6;
		pos++;
	}

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 28];
	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	uAdd = float(31 - 4 * (aWibble & 7)) * (1.0F / 256.0F);
	col = RGBONLY(r, g, b);

	v = aVertexBuffer;
	pVtx = vtx;
	c = clip;

	for (lp = 0; lp < 63; lp++)
	{
		tex.u1 = ((lp & 3) * 8.0F * (1.0F / 256.0F) + sprite->x1) + uAdd + (1.0F / 512.0F);
		tex.u2 = tex.u1;
		tex.u3 = ((lp & 3) * 8.0F * (1.0F / 256.0F) + sprite->x1) + uAdd + (8.0F / 256.0F);
		tex.u4 = tex.u3;
		tex.v1 = sprite->y1;
		tex.v2 = sprite->y2;
		tex.v3 = sprite->y2;
		tex.v4 = sprite->y1;

		v[0].sx = pVtx[0];
		v[0].sy = pVtx[1];
		v[0].rhw = pVtx[2];
		v[0].sz = pVtx[3];
		v[0].tu = pVtx[4];
		v[0].tv = pVtx[5];
		v[0].color = col;
		v[0].specular = 0xFF000000;

		v[1].sx = pVtx[6];
		v[1].sy = pVtx[7];
		v[1].rhw = pVtx[8];
		v[1].sz = pVtx[9];
		v[1].tu = pVtx[10];
		v[1].tv = pVtx[11];
		v[1].color = col;
		v[1].specular = 0xFF000000;

		v[2].sx = pVtx[12];
		v[2].sy = pVtx[13];
		v[2].rhw = pVtx[14];
		v[2].sz = pVtx[15];
		v[2].tu = pVtx[16];
		v[2].tv = pVtx[17];
		v[2].color = col;
		v[2].specular = 0xFF000000;

		v[3].sx = pVtx[18];
		v[3].sy = pVtx[19];
		v[3].rhw = pVtx[20];
		v[3].sz = pVtx[21];
		v[3].tu = pVtx[22];
		v[3].tv = pVtx[23];
		v[3].color = col;
		v[3].specular = 0xFF000000;

		clipflags[0] = c[0];
		clipflags[1] = c[1];
		clipflags[2] = c[2];
		clipflags[3] = c[3];

		AddQuadSorted(v, 1, 0, 2, 3, &tex, 1);

		c += 2;
		pVtx += 12;
	}

	phd_PopMatrix();
}

void DrawLasers(ITEM_INFO* item)
{
	LASER_STRUCT* laser;
	SPRITESTRUCT* sprite;
	LASER_VECTOR* vtx;
	LASER_VECTOR* vtx2;
	D3DTLVERTEX* vbuf;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	TEXTURESTRUCT tex2;
	short* rand;
	float val, hStep, fx, fy, fz, mx, my, mz, zv;
	long lp, lp2, x, y, z, xStep, yStep, zStep, c1, c2, c3, c4;
	short* c;
	short clip[36];
	short clipFlag;

	if (!TriggerActive(item) || (item->trigger_flags & 1 && !InfraRed && !item->item_flags[3]))
		return;

	laser = (LASER_STRUCT*)item->data;
	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];

	tex.drawtype = 2;
	tex.tpage = 0;

	tex2.drawtype = 2;
	tex2.tpage = sprite->tpage;

	val = float((GlobalCounter >> 1) & 0x1F) * (1.0F / 256.0F) + sprite->y1;
	tex2.u1 = sprite->x1 + (1.0F / 512.0F);
	tex2.v1 = val;
	tex2.u2 = sprite->x1 + (31.0F / 256.0F) - (1.0F / 512.0F);
	tex2.v2 = val;
	tex2.u3 = sprite->x1 + (31.0F / 256.0F) - (1.0F / 512.0F);
	tex2.u4 = sprite->x1 + (1.0F / 512.0F);
	tex2.v4 = val + (1.0F / 512.0F);
	tex2.v3 = val + (1.0F / 512.0F);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	aSetViewMatrix();

	for (int i = 0; i < 3; i++)
	{
		vtx = (LASER_VECTOR*)&tsv_buffer[0];
		x = laser->v1[i].x;
		y = laser->v1[i].y;
		z = laser->v1[i].z;
		xStep = (laser->v4[i].x - x) >> 3;
		yStep = laser->v4[i].y - y;
		zStep = (laser->v4[i].z - z) >> 3;
		rand = laser->Rand;

		for (lp = 0; lp < 2; lp++)
		{
			for (lp2 = 0; lp2 < 9; lp2++)
			{
				vtx->num = 1.0F;
				vtx->x = (float)x;
				vtx->y = (float)y;
				vtx->z = (float)z;

				if (!lp2 || lp2 == 8)
					vtx->color = 0;
				else if (!(item->trigger_flags & 1) || InfraRed)
					vtx->color = (item->item_flags[3] >> 1) + abs(phd_sin((*rand << i) + (GlobalCounter << 9)) >> 8);
				else
					vtx->color = (item->item_flags[3] * abs(phd_sin((*rand << i) + (GlobalCounter << 9)) >> 8)) >> 6;

				x += xStep;
				z += zStep;
				vtx++;
				rand++;
			}

			x = laser->v1[i].x;
			y += yStep;
			z = laser->v1[i].z;
		}

		hStep = float(yStep >> 1);
		vtx2 = (LASER_VECTOR*)&tsv_buffer[0];

		for (lp = 0; lp < 9; lp++)
		{
			vtx->x = vtx2->x;
			vtx->y = vtx2->y + hStep;
			vtx->z = vtx2->z;
			vtx->num = 1.0F;
			vtx->color = vtx2->color;
			vtx++;
			vtx2++;
		}

		for (lp = 0; lp < 9; lp++)
		{
			vtx->x = vtx2->x;
			vtx->y = vtx2->y - hStep;
			vtx->z = vtx2->z;
			vtx->num = 1.0F;
			vtx->color = vtx2->color;
			vtx++;
			vtx2++;
		}

		vtx = (LASER_VECTOR*)&tsv_buffer[0];
		vbuf = aVertexBuffer;

		for (lp = 0; lp < 36; lp++)
		{
			fx = vtx->x;
			fy = vtx->y;
			fz = vtx->z;

			mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
			my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
			mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

			vbuf->tu = mx;
			vbuf->tv = my;

			clipFlag = 0;

			if (mz < f_mznear)
				clipFlag = -128;
			else
			{
				zv = f_mpersp / mz;
				mx = mx * zv + f_centerx;
				my = my * zv + f_centery;
				vbuf->rhw = f_moneopersp * zv;

				if (mx < f_left)
					clipFlag++;
				else if (mx > f_right)
					clipFlag += 2;

				if (my < f_top)
					clipFlag += 4;
				else if (my > f_bottom)
					clipFlag += 8;

				vbuf->sx = mx;
				vbuf->sy = my;
			}

			vbuf->sz = mz;
			clip[lp] = clipFlag;
			vbuf++;
			vtx++;
		}

		vtx = (LASER_VECTOR*)&tsv_buffer[0];
		vbuf = aVertexBuffer;
		c = clip;

		for (lp = 0; lp < 8; lp++)
		{
			c1 = vtx[0].color;
			c2 = vtx[1].color;
			c3 = vtx[9].color;
			c4 = vtx[10].color;

			v[0].sx = vbuf[0].sx;
			v[0].sy = vbuf[0].sy;
			v[0].sz = vbuf[0].sz;
			v[0].rhw = vbuf[0].rhw;
			v[0].tu = vbuf[0].tu;
			v[0].tv = vbuf[0].tv;

			v[1].sx = vbuf[1].sx;
			v[1].sy = vbuf[1].sy;
			v[1].sz = vbuf[1].sz;
			v[1].rhw = vbuf[1].rhw;
			v[1].tu = vbuf[1].tu;
			v[1].tv = vbuf[1].tv;

			v[2].sx = vbuf[18].sx;
			v[2].sy = vbuf[18].sy;
			v[2].sz = vbuf[18].sz;
			v[2].rhw = vbuf[18].rhw;
			v[2].tu = vbuf[18].tu;
			v[2].tv = vbuf[18].tv;

			v[3].sx = vbuf[19].sx;
			v[3].sy = vbuf[19].sy;
			v[3].sz = vbuf[19].sz;
			v[3].rhw = vbuf[19].rhw;
			v[3].tu = vbuf[19].tu;
			v[3].tv = vbuf[19].tv;

			if (item->trigger_flags & 2)
			{
				v[0].color = RGBA(c1, 0, 0, 0xFF);
				v[1].color = RGBA(c2, 0, 0, 0xFF);
			}
			else
			{
				v[0].color = RGBA(0, c1, 0, 0xFF);
				v[1].color = RGBA(0, c2, 0, 0xFF);
			}

			v[2].color = 0;
			v[3].color = 0;

			v[0].specular = vbuf[0].specular;
			v[1].specular = vbuf[1].specular;
			v[2].specular = vbuf[18].specular;
			v[3].specular = vbuf[19].specular;

			clipflags[0] = c[0];
			clipflags[1] = c[1];
			clipflags[2] = c[18];
			clipflags[3] = c[19];

			AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

			v[0].sx = vbuf[9].sx;
			v[0].sy = vbuf[9].sy;
			v[0].sz = vbuf[9].sz;
			v[0].rhw = vbuf[9].rhw;
			v[0].tu = vbuf[9].tu;
			v[0].tv = vbuf[9].tv;

			v[1].sx = vbuf[10].sx;
			v[1].sy = vbuf[10].sy;
			v[1].sz = vbuf[10].sz;
			v[1].rhw = vbuf[10].rhw;
			v[1].tu = vbuf[10].tu;
			v[1].tv = vbuf[10].tv;

			v[2].sx = vbuf[27].sx;
			v[2].sy = vbuf[27].sy;
			v[2].sz = vbuf[27].sz;
			v[2].rhw = vbuf[27].rhw;
			v[2].tu = vbuf[27].tu;
			v[2].tv = vbuf[27].tv;

			v[3].sx = vbuf[28].sx;
			v[3].sy = vbuf[28].sy;
			v[3].sz = vbuf[28].sz;
			v[3].rhw = vbuf[28].rhw;
			v[3].tu = vbuf[28].tu;
			v[3].tv = vbuf[28].tv;

			if (item->trigger_flags & 2)
			{
				v[0].color = RGBA(c3, 0, 0, 0xFF);
				v[1].color = RGBA(c4, 0, 0, 0xFF);
			}
			else
			{
				v[0].color = RGBA(0, c3, 0, 0xFF);
				v[1].color = RGBA(0, c4, 0, 0xFF);
			}

			v[2].color = 0;
			v[3].color = 0;

			v[0].specular = vbuf[9].specular;
			v[1].specular = vbuf[10].specular;
			v[2].specular = vbuf[27].specular;
			v[3].specular = vbuf[28].specular;

			clipflags[0] = c[9];
			clipflags[1] = c[10];
			clipflags[2] = c[27];
			clipflags[3] = c[28];

			AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

			v[0] = vbuf[0];
			v[1] = vbuf[1];
			v[2] = vbuf[9];
			v[3] = vbuf[10];

			clipflags[0] = c[0];
			clipflags[1] = c[1];
			clipflags[2] = c[9];
			clipflags[3] = c[10];

			if (item->trigger_flags & 2)
			{
				v[0].color = RGBA(c1, 0, 0, 0xFF);
				v[1].color = RGBA(c2, 0, 0, 0xFF);
				v[2].color = RGBA(c3, 0, 0, 0xFF);
				v[3].color = RGBA(c4, 0, 0, 0xFF);
			}
			else
			{
				v[0].color = RGBA(0, c1, 0, 0xFF);
				v[1].color = RGBA(0, c2, 0, 0xFF);
				v[2].color = RGBA(0, c3, 0, 0xFF);
				v[3].color = RGBA(0, c4, 0, 0xFF);
			}

			AddQuadSorted(v, 0, 1, 3, 2, &tex2, 1);

			vtx++;
			vbuf++;
			c++;
		}
	}

	phd_PopMatrix();
}

void DrawSteamLasers(ITEM_INFO* item)
{
	STEAMLASER_STRUCT* laser;
	SPRITESTRUCT* sprite;
	LASER_VECTOR* vtx;		//original uses SVECTOR
	D3DTLVERTEX* vbuf;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	TEXTURESTRUCT tex2;
	short* rand;
	short* c;
	float fx, fy, fz, mx, my, mz, zv, val;
	long on, x, y, z, xStep, yStep, zStep, col, lp, lp2, c1, c2, c3, c4;
	short clip[36];
	short clipFlag;

	if (!TriggerActive(item) || !SteamLasers[(GlobalCounter >> 5) & 7][item->trigger_flags])
		return;

	on = IsSteamOn(item);

	if (!on && !InfraRed && !item->item_flags[3])
		return;

	laser = (STEAMLASER_STRUCT*)item->data;
	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];

	tex.drawtype = 2;
	tex.tpage = 0;

	tex2.drawtype = 2;
	tex2.tpage = sprite->tpage;
	tex2.u1 = sprite->x1 + (1.0F / 512.0F);
	tex2.u2 = sprite->x1 + (31.0F / 256.0F) - (1.0F / 512.0F);
	tex2.u3 = sprite->x1 + (31.0F / 256.0F) - (1.0F / 512.0F);
	tex2.u4 = sprite->x1 + (1.0F / 512.0F);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos + item->item_flags[0], item->pos.z_pos);
	aSetViewMatrix();

	for (int i = 0; i < 2; i++)
	{
		vtx = (LASER_VECTOR*)&tsv_buffer[0];
		x = laser->v1[i].x;
		y = laser->v1[i].y;
		z = laser->v1[i].z;
		xStep = (laser->v4[i].x - x) >> 3;
		yStep = (laser->v4[i].y - y) >> 1;
		zStep = (laser->v4[i].z - z) >> 3;
		rand = laser->Rand;

		for (lp = 0; lp < 3; lp++)
		{
			for (lp2 = 0; lp2 < 9; lp2++)
			{
				vtx->x = (float)x;
				vtx->y = (float)y;
				vtx->z = (float)z;

				if (!lp2 || lp == 8)
					vtx->color = 0;
				else
					vtx->color = abs(phd_sin(*rand + (GlobalCounter << 9)) >> 8);

				if (on)
					col = GetSteamMultiplier(item, y, z) << 1;
				else
					col = 0;

				col += item->item_flags[3];

				if (InfraRed)
					col += 64;

				vtx->color = (vtx->color * col) >> 6;

				x += xStep;
				z += zStep;
				vtx++;
				rand++;
			}

			x = laser->v1[i].x;
			y += yStep;
			z = laser->v1[i].z;
		}

		vbuf = aVertexBuffer;
		vtx = (LASER_VECTOR*)&tsv_buffer[0];

		for (lp = 0; lp < 27; lp++)
		{
			fx = vtx->x;
			fy = vtx->y;
			fz = vtx->z;

			mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
			my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
			mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

			vbuf->tu = mx;
			vbuf->tv = my;

			clipFlag = 0;

			if (mz < f_mznear)
				clipFlag = -128;
			else
			{
				zv = f_mpersp / mz;
				mx = mx * zv + f_centerx;
				my = my * zv + f_centery;
				vbuf->rhw = zv * f_moneopersp;

				if (mx < f_left)
					clipFlag++;
				else if (mx > f_right)
					clipFlag += 2;

				if (my < f_top)
					clipFlag += 4;
				else if (my > f_bottom)
					clipFlag += 8;
			}

			clip[lp] = clipFlag;
			vbuf->sx = mx;
			vbuf->sy = my;
			vbuf->sz = mz;

			vbuf++;
			vtx++;
		}

		vbuf = aVertexBuffer;
		vtx = (LASER_VECTOR*)&tsv_buffer[0];
		c = clip;

		val = float((item->item_flags[0] >> 2) & 0x1F) * (1.0F / 256.0F) + sprite->y1;
		tex2.v1 = val;
		tex2.v2 = val;
		tex2.v3 = val + (31.0F / 256.0F);
		tex2.v4 = val + (31.0F / 256.0F);

		for (lp = 0; lp < 16; lp++)
		{
			c1 = vtx[0].color;
			c2 = vtx[1].color;
			c3 = vtx[9].color;
			c4 = vtx[10].color;

			if (lp < 8)
			{
				v[0].sx = vbuf[0].sx;
				v[0].sy = vbuf[0].sy;
				v[0].rhw = vbuf[0].rhw;
				v[0].tu = vbuf[0].tu;
				v[0].tv = vbuf[0].tv;

				v[1].sx = vbuf[1].sx;
				v[1].sy = vbuf[1].sy;
				v[1].rhw = vbuf[1].rhw;
				v[1].tu = vbuf[1].tu;
				v[1].tv = vbuf[1].tv;

				v[2].sx = vbuf[9].sx;
				v[2].sy = (3 * vbuf[0].sy + vbuf[9].sy) * 0.25F;
				v[2].rhw = vbuf[9].rhw;
				v[2].tu = vbuf[9].tu;
				v[2].tv = vbuf[9].tv;

				v[3].sx = vbuf[10].sx;
				v[3].sy = (3 * vbuf[1].sy + vbuf[10].sy) * 0.25F;
				v[3].rhw = vbuf[10].rhw;
				v[3].tu = vbuf[10].tu;
				v[3].tv = vbuf[10].tv;

				c3 = 0;
				c4 = 0;
			}
			else
			{
				v[0].sx = vbuf[0].sx;
				v[0].sy = vbuf[9].sy;
				v[0].rhw = vbuf[0].rhw;
				v[0].tu = vbuf[0].tu;
				v[0].tv = vbuf[0].tv;

				v[1].sx = vbuf[1].sx;
				v[1].sy = vbuf[10].sy;
				v[1].rhw = vbuf[1].rhw;
				v[1].tu = vbuf[1].tu;
				v[1].tv = vbuf[1].tv;

				v[2].sx = vbuf[9].sx;
				v[2].sy = (3 * vbuf[9].sy + vbuf[0].sy) * 0.25F;
				v[2].rhw = vbuf[9].rhw;
				v[2].tu = vbuf[9].tu;
				v[2].tv = vbuf[9].tv;

				v[3].sx = vbuf[10].sx;
				v[3].sy = (3 * vbuf[10].sy + vbuf[1].sy) * 0.25F;
				v[3].rhw = vbuf[10].rhw;
				v[3].tu = vbuf[10].tu;
				v[3].tv = vbuf[10].tv;

				c1 = 0;
				c2 = 0;
			}

			v[0].sz = vbuf[0].sz;
			v[1].sz = vbuf[1].sz;
			v[2].sz = vbuf[9].sz;
			v[3].sz = vbuf[10].sz;

			v[0].color = RGBA(c1, 0, 0, 0xFF);
			v[1].color = RGBA(c2, 0, 0, 0xFF);
			v[2].color = RGBA(c3, 0, 0, 0xFF);
			v[3].color = RGBA(c4, 0, 0, 0xFF);

			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;

			clipflags[0] = c[0];
			clipflags[1] = c[1];
			clipflags[2] = c[9];
			clipflags[3] = c[10];

			if (App.dx.Flags & DXF_HWR)
				AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

			v[0].sx = vbuf[0].sx;
			v[0].sy = vbuf[0].sy;
			v[0].rhw = vbuf[0].rhw;
			v[0].tu = vbuf[0].tu;
			v[0].tv = vbuf[0].tv;

			v[1].sx = vbuf[1].sx;
			v[1].sy = vbuf[1].sy;
			v[1].rhw = vbuf[1].rhw;
			v[1].tu = vbuf[1].tu;
			v[1].tv = vbuf[1].tv;

			v[2].sx = vbuf[9].sx;
			v[2].sy = vbuf[9].sy;
			v[2].rhw = vbuf[9].rhw;
			v[2].tu = vbuf[9].tu;
			v[2].tv = vbuf[9].tv;

			v[3].sx = vbuf[10].sx;
			v[3].sy = vbuf[10].sy;
			v[3].rhw = vbuf[10].rhw;
			v[3].tu = vbuf[10].tu;
			v[3].tv = vbuf[10].tv;

			v[0].color = RGBA(c1, 0, 0, 0xFF);
			v[1].color = RGBA(c2, 0, 0, 0xFF);
			v[2].color = RGBA(c3, 0, 0, 0xFF);
			v[3].color = RGBA(c4, 0, 0, 0xFF);

			AddQuadSorted(v, 0, 1, 3, 2, &tex2, 1);

			if (lp == 7)
			{
				vbuf += 2;
				vtx += 2;
				c += 2;
			}
			else
			{
				vbuf++;
				vtx++;
				c++;
			}
		}
	}

	phd_PopMatrix();
}

void S_DrawFloorLasers(ITEM_INFO* item)
{
	FLOORLASER_STRUCT* laser;
	SPRITESTRUCT* sprite;
	FVECTOR* vtx;
	D3DTLVERTEX* vbuf;
	TEXTURESTRUCT tex;
	D3DTLVERTEX v[4];
	short* rand;
	short* pulse;
	short* c;
	float fx, fy, fz, mx, my, mz, zv, val;
	long w, h, x, y, z, xStep, zStep, f, lp, lp2, c1, c2, c3, c4;
	short clip[128];
	short clipFlag;

	if (!TriggerActive(item) || (!InfraRed && !item->item_flags[3] && item->item_flags[2] <= 0 && !item->trigger_flags))
		return;

	laser = (FLOORLASER_STRUCT*)item->data;
	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];

	xStep = (laser->v4.x - laser->v1.x) / (item->item_flags[1] << 1);
	zStep = (laser->v4.z - laser->v1.z) / (item->item_flags[0] << 1);

	vtx = (FVECTOR*)&tsv_buffer[0];
	rand = laser->Rand;
	pulse = laser->Pulse;

	h = (item->item_flags[1] << 1) + 1;
	w = (item->item_flags[0] << 1) + 1;
	x = laser->v1.x;
	y = laser->v1.y;

	for (lp = 0; lp < h; lp++)
	{
		z = laser->v1.z;

		for (lp2 = 0; lp2 < w; lp2++)
		{
			vtx->x = (float)x;
			vtx->y = (float)y;
			vtx->z = (float)z;

			if (item->trigger_flags)
				*pulse = (GetRandomControl() & item->trigger_flags) << 1;	//contact with water, pulse randomly
			else
			{
				f = *rand + (GlobalCounter << 9);

				if (item->item_flags[3])
					*pulse = (item->item_flags[3] * (item->item_flags[3] + (phd_sin(f) >> 7) - 64)) >> 4;
				else
					*pulse = ((phd_sin(f) >> 7) - 64) << 1;

				if (item->item_flags[2] > 0)
				{
					f = abs(item->item_flags[2] - 2048 - z);

					if (InfraRed || item->item_flags[3])
					{
						if (f < 2048)
							*pulse += short((2048 - f) >> 3);
					}
					else if (f > 2048)
						*pulse = 0;
					else
						*pulse = short(((2048 - f) * (*pulse + 127)) >> 11);
				}
			}

			if (*pulse < 0)
				*pulse = 0;
			else if (*pulse > 127)
				*pulse = 127;

			z += zStep;
			vtx++;
			rand++;
			pulse++;
		}

		x += xStep;
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	aSetViewMatrix();

	vbuf = aVertexBuffer;
	vtx = (FVECTOR*)&tsv_buffer[0];

	for (lp = 0; lp < h * w; lp++)
	{
		fx = vtx->x;
		fy = vtx->y;
		fz = vtx->z;

		mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
		my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
		mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

		vbuf->tu = mx;
		vbuf->tv = my;

		clipFlag = 0;

		if (mz < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / mz;
			mx = mx * zv + f_centerx;
			my = my * zv + f_centery;
			vbuf->rhw = zv * f_moneopersp;

			if (mx < f_left)
				clipFlag++;
			else if (mx > f_right)
				clipFlag += 2;

			if (my < f_top)
				clipFlag += 4;
			else if (my > f_bottom)
				clipFlag += 8;
		}

		clip[lp] = clipFlag;
		vbuf->sx = mx;
		vbuf->sy = my;
		vbuf->sz = mz;

		vbuf++;
		vtx++;
	}

	phd_PopMatrix();

	val = float((GlobalCounter >> 2) & 0x1F) * (1.0F / 256.0F) + sprite->y1;
	tex.drawtype = 2;
	tex.tpage = sprite->tpage;

	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x1 + (30.0F / 256.0F);
	tex.v2 = sprite->y1;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y1 + (32.0F / 256.0F);
	tex.u3 = sprite->x1 + (30.0F / 256.0F);
	tex.v3 = sprite->y1 + (32.0F / 256.0F);

	pulse = laser->Pulse;
	vbuf = aVertexBuffer;
	c = clip;

	for (lp = 0; lp < h - 1; lp++)
	{
		for (lp2 = 0; lp2 < w - 1; lp2++)
		{
			c1 = pulse[0] << 1;
			c2 = pulse[1] << 1;
			c3 = pulse[0 + w] << 1;
			c4 = pulse[1 + w] << 1;

			v[0] = vbuf[0];
			v[1] = vbuf[1];
			v[2] = vbuf[0 + w];
			v[3] = vbuf[1 + w];

			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;

			v[0].color = RGBA(c1, 0, 0, 0xFF);
			v[1].color = RGBA(c2, 0, 0, 0xFF);
			v[2].color = RGBA(c3, 0, 0, 0xFF);
			v[3].color = RGBA(c4, 0, 0, 0xFF);

			clipflags[0] = c[0];
			clipflags[1] = c[1];
			clipflags[2] = c[0 + w];
			clipflags[3] = c[1 + w];

			AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

			vbuf++;
			c++;
			pulse++;
		}

		vbuf++;
		c++;
		pulse++;
	}
}

static void S_DrawGasCloud(ITEM_INFO* item, GAS_CLOUD* cloud)
{
	FVECTOR* vtx;
	D3DTLVERTEX* vbuf;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	short* rand;
	short* pulse;
	short* c;
	float fx, fy, fz, mx, my, mz, zv;
	long x, y, z, xStep, zStep, c1, c2, c3, c4, lp, lp2;
	short clip[36];
	short clipFlag;

	vtx = (FVECTOR*)&tsv_buffer[0];
	rand = cloud->Rand;
	pulse = cloud->Pulse;

	xStep = (cloud->v4.x - cloud->v1.x) / 5;
	zStep = (cloud->v4.z - cloud->v1.z) / 5;
	x = cloud->v1.x;
	y = cloud->v1.y;

	for (lp = 0; lp < 6; lp++)
	{
		z = cloud->v1.z;

		for (lp2 = 0; lp2 < 6; lp2++)
		{
			vtx->x = (float)x;
			vtx->y = (float)y;
			vtx->z = (float)z;

			if (!lp || lp == 5 || !lp2 || lp2 == 5)
				pulse[0] = 0;
			else
			{
				pulse[0] = ((phd_sin(*rand + (GlobalCounter << 9)) >> 7) - 64) << 1;

				if (item->item_flags[0] < 256)
					pulse[0] = (pulse[0] * item->item_flags[0]) >> 8;

				if (pulse[0] < 0)
					pulse[0] = 0;
				else if (pulse[0] > 127)
					pulse[0] = 127;
			}

			rand++;
			pulse++;
			vtx++;
			z += zStep;
		}

		x += xStep;
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos + cloud->t.x, item->pos.y_pos + cloud->t.y, item->pos.z_pos + cloud->t.z);
	phd_RotYXZ(-CamRot.y << 4, -4096, 0);
	aSetViewMatrix();

	vbuf = aVertexBuffer;
	vtx = (FVECTOR*)&tsv_buffer[0];

	for (lp = 0; lp < 36; lp++)
	{
		fx = vtx->x;
		fy = vtx->y;
		fz = vtx->z;

		mx = fx * D3DMView._11 + fy * D3DMView._21 + fz * D3DMView._31 + D3DMView._41;
		my = fx * D3DMView._12 + fy * D3DMView._22 + fz * D3DMView._32 + D3DMView._42;
		mz = fx * D3DMView._13 + fy * D3DMView._23 + fz * D3DMView._33 + D3DMView._43;

		vbuf->tu = mx;
		vbuf->tv = my;

		clipFlag = 0;

		if (mz < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / mz;
			mx = mx * zv + f_centerx;
			my = my * zv + f_centery;
			vbuf->rhw = zv * f_moneopersp;

			if (mx < f_left)
				clipFlag++;
			else if (mx > f_right)
				clipFlag += 2;

			if (my < f_top)
				clipFlag += 4;
			else if (my > f_bottom)
				clipFlag += 8;
		}

		clip[lp] = clipFlag;
		vbuf->sx = mx;
		vbuf->sy = my;
		vbuf->sz = mz;

		vbuf++;
		vtx++;
	}

	phd_PopMatrix();

	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];
	tex.drawtype = 2;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x1 + (30.0F / 256.0F);
	tex.v2 = sprite->y1;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y1 + (32.0F / 256.0F);
	tex.u3 = sprite->x1 + (30.0F / 256.0F);
	tex.v3 = sprite->y1 + (32.0F / 256.0F);

	pulse = cloud->Pulse;
	vbuf = aVertexBuffer;
	c = clip;

	for (lp = 0; lp < 5; lp++)
	{
		for (lp2 = 0; lp2 < 5; lp2++)
		{
			c1 = pulse[0];
			c2 = pulse[1];
			c3 = pulse[0 + 6];
			c4 = pulse[1 + 6];

			v[0] = vbuf[0];
			v[1] = vbuf[1];
			v[2] = vbuf[0 + 6];
			v[3] = vbuf[1 + 6];

			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;

			v[0].color = RGBA(0, c1, 0, 0xFF);
			v[1].color = RGBA(0, c2, 0, 0xFF);
			v[2].color = RGBA(0, c3, 0, 0xFF);
			v[3].color = RGBA(0, c4, 0, 0xFF);

			clipflags[0] = c[0];
			clipflags[1] = c[1];
			clipflags[2] = c[0 + 6];
			clipflags[3] = c[1 + 6];

			AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

			vbuf++;
			c++;
			pulse++;
		}

		vbuf++;
		c++;
		pulse++;
	}
}

void DrawGasCloud(ITEM_INFO* item)
{
	GAS_CLOUD* cloud;
	GAS_CLOUD* sec;
	long num;

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags >= 2)
	{
		item->item_flags[0] = 1;
		return;
	}

	cloud = (GAS_CLOUD*)item->data;

	if (!cloud->mTime)
		cloud->yo = -6144.0F;

	TriggerFogBulbFX(0, 196, 0, item->pos.x_pos, long(item->pos.y_pos + cloud->yo), item->pos.z_pos, 2048, 40);

	if (cloud->yo < -3584.0F)
		cloud->yo += 12.0F;
	else
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

			sec = &cloud[cloud->num];
			TriggerFogBulbFX(0, 196, 0, item->pos.x_pos + sec->t.x, item->pos.y_pos + sec->t.y, item->pos.z_pos + sec->t.z, 1536, num);
		}

		cloud->sTime++;
	}

	cloud->mTime++;

	for (int i = 0; i < 8; i++, cloud++)
		S_DrawGasCloud(item, cloud);
}

void S_DrawDarts(ITEM_INFO* item)
{
	D3DTLVERTEX v[2];
	float mx, my, mz, zv;
	long x1, y1, z1, x2, y2, z2, num, x, y, z;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	//xyz are 0
	zv = f_persp / aMXPtr[M23];
	x1 = long(aMXPtr[M03] * zv + f_centerx);
	y1 = long(aMXPtr[M13] * zv + f_centery);
	z1 = (long)aMXPtr[M23];

	num = (-96 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	x = (num * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	y = (96 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	z = (num * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	mx = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
	my = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
	mz = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
	zv = f_persp / mz;
	x2 = long(mx * zv + f_centerx);
	y2 = long(my * zv + f_centery);
	z2 = (long)mz;

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

void DrawLaserSightSprite()
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	FVECTOR vec;
	FVECTOR pos;
	float zv, x, y, z;
	long x1, y1, x2, y2, size;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	x = float(LaserSightX - lara_item->pos.x_pos);
	y = float(LaserSightY - lara_item->pos.y_pos);
	z = float(LaserSightZ - lara_item->pos.z_pos);
	vec.x = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
	vec.y = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
	vec.z = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
	zv = f_persp / vec.z;
	pos.x = vec.x * zv + f_centerx;
	pos.y = vec.y * zv + f_centery;
	pos.z = vec.z;

	phd_PopMatrix();

	if (LaserSightCol)
	{
		size = (GlobalCounter & 4) + 8;
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 18];
	}
	else
	{
		size = 3;
		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	}

	size = GetFixedScale(size);
	x1 = long(pos.x - size);
	x2 = long(pos.x + size);
	y1 = long(pos.y - size);
	y2 = long(pos.y + size);
	setXY4(v, x1, y1, x2, y1, x1, y2, x2, y2, (long)f_mznear, clipflags);

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
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 0);

	LaserSightCol = 0;
	LaserSightActive = 0;
}

void DrawMoon()
{
	D3DTLVERTEX* v;
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT tex;
	SVECTOR vec[4];
	short* c;
	short p;
	ushort tpage;

	c = clipflags;
	v = aVertexBuffer;
	p = 320;

	sprite = &spriteinfo[objects[MISC_SPRITES].mesh_index + 3];
	tpage = sprite->tpage < nTextures ? sprite->tpage : 0;
	phd_PushMatrix();
	aSetViewMatrix();
	D3DMView._41 = 0;
	D3DMView._42 = 0;
	D3DMView._43 = 0;

	vec[0].x = 0;
	vec[0].y = -1024;
	vec[0].z = 1920;

	vec[1].x = p;
	vec[1].y = -1024;
	vec[1].z = 1920;

	vec[2].x = p;
	vec[2].y = -1024 + p;
	vec[2].z = 1920;

	vec[3].x = 0;
	vec[3].y = -1024 + p;
	vec[3].z = 1920;

	aTransformPerspSV(vec, v, c, 4, 0);

	v = aVertexBuffer;

	v[0].color = 0xFFC0E0FF;
	v[1].color = 0xFFC0E0FF;
	v[2].color = 0xFFC0E0FF;
	v[3].color = 0xFFC0E0FF;
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
	tex.drawtype = 2;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);

	phd_PopMatrix();
}

void DrawStars()
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
	aSetViewMatrix();
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

#define SetVecXYZ(num, X, Y, Z)	 vec[(num)].x = (X); vec[(num)].y = (Y); vec[(num)].z = (Z);

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

	if (gfCurrentLevel != LVL5_TITLE)
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

			if (v[i].sz > f_mzfar)
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

			if (v[i].sz > f_mzfar)
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

	if (gfCurrentLevel != LVL5_TITLE)
		AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);

	phd_PopMatrix();
}

static void DrawPSXSky(ulong color, long zpos, long ypos, long drawtype)
{
	for (int i = 1; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
			DrawSkySegment(color, drawtype, j, i, zpos, ypos);
	}
}

void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype)
{
	FVECTOR vec[4];
	D3DTLVERTEX v[4];
	TEXTURESTRUCT Tex;
	short* clip;
	float x, y, z;

	if (tomb5.PSX_skies)
	{
		DrawPSXSky(color, zpos, ypos, drawtype);
		return;
	}

	phd_PushMatrix();

	if (gfCurrentLevel != LVL5_TITLE)
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
		vec[i].x = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
		vec[i].y = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
		vec[i].z = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
		v[i].color = color | 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], vec[0].x, vec[0].y, vec[0].z, clip);
	clip++;
	ClipCheckPoint(&v[1], vec[1].x, vec[1].y, vec[1].z, clip);
	clip++;
	ClipCheckPoint(&v[2], vec[2].x, vec[2].y, vec[2].z, clip);
	clip++;
	ClipCheckPoint(&v[3], vec[3].x, vec[3].y, vec[3].z, clip);
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
		vec[i].x = aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03];
		vec[i].y = aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13];
		vec[i].z = aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23];
		v[i].color |= 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], vec[0].x, vec[0].y, vec[0].z, clip);
	clip++;
	ClipCheckPoint(&v[1], vec[1].x, vec[1].y, vec[1].z, clip);
	clip++;
	ClipCheckPoint(&v[2], vec[2].x, vec[2].y, vec[2].z, clip);
	clip++;
	ClipCheckPoint(&v[3], vec[3].x, vec[3].y, vec[3].z, clip);

	if (gfCurrentLevel != LVL5_TITLE)
		AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);

	phd_PopMatrix();
}

void aDrawWreckingBall(ITEM_INFO* item, long shade)
{
	SPRITESTRUCT* sprite;
	SVECTOR* vec;
	TEXTURESTRUCT tex;
	long x, z, s;

	aSetViewMatrix();
	vec = (SVECTOR*)&tsv_buffer[0];
	s = (400 * shade) >> 7;
	x = -s;

	for (int i = 0; i < 3; i++)
	{
		z = -s;

		for (int j = 0; j < 3; j++)
		{
			vec->x = (short)x;
			vec->y = -2;
			vec->z = (short)z;
			vec++;
			z += s;
		}

		x += s;
	}

	if (shade < 100)
		shade = 100;

	vec = (SVECTOR*)&tsv_buffer[0];

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	aTransformPerspSV(vec, aVertexBuffer, clipflags, 9, RGBA(shade / 3, shade / 3, shade / 3, shade));
	tex.drawtype = 5;

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

#define CIRCUMFERENCE_POINTS 32 // Number of points in the circumference
#define LINE_POINTS	4	//number of points in each grid line
#define POINT_HEIGHT_CORRECTION	196	//if the difference between the floor below Lara and the floor height below the point is greater than this value, point height is corrected to lara's floor level.
#define NUM_TRIS	14	//number of triangles needed to create the shadow (this depends on what shape you're doing)
#define GRID_POINTS	(LINE_POINTS * LINE_POINTS)	//number of points in the whole grid

static void S_PrintCircleShadow(short size, short* box, ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	FVECTOR cv[CIRCUMFERENCE_POINTS];
	FVECTOR cp[CIRCUMFERENCE_POINTS];
	FVECTOR ccv;
	FVECTOR ccp;
	PHD_VECTOR pos;
	float fx, fy, fz;
	long x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number, s;

	xSize = size * (box[1] - box[0]) / 192;	//x size of grid
	zSize = size * (box[5] - box[4]) / 192;	//z size of grid
	xDist = xSize / LINE_POINTS;			//distance between each point of the grid on X
	zDist = zSize / LINE_POINTS;			//distance between each point of the grid on Z
	x = xDist + (xDist >> 1);
	z = zDist + (zDist >> 1);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		cp[i].x = x * fSin(65536 * i / CIRCUMFERENCE_POINTS);
		cp[i].z = z * fCos(65536 * i / CIRCUMFERENCE_POINTS);
		cv[i].x = cp[i].x;
		cv[i].z = cp[i].z;
	}

	phd_PushUnitMatrix();
	s = item->current_anim_state;

	if (item == lara_item && s != AS_ALL4S && s != AS_ALL4TURNL && s != AS_ALL4TURNR && s != AS_CRAWL && s != AS_CRAWLBACK)	//position the grid
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LMX_TORSO);
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
		fx = cp[i].x;
		fz = cp[i].z;
		cp[i].x = fx * aMXPtr[M00] + fz * aMXPtr[M02] + aMXPtr[M03];
		cp[i].z = fx * aMXPtr[M20] + fz * aMXPtr[M22] + aMXPtr[M23];
	}

	ccp.x = aMXPtr[M03];
	ccp.z = aMXPtr[M23];
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		room_number = item->room_number;
		floor = GetFloor((long)cp[i].x, item->floor, (long)cp[i].z, &room_number);
		cp[i].y = (float)GetHeight(floor, (long)cp[i].x, item->floor, (long)cp[i].z);

		if (abs(cp[i].y - item->floor) > POINT_HEIGHT_CORRECTION)
			cp[i].y = (float)item->floor;
	}

	room_number = item->room_number;
	floor = GetFloor((long)ccp.x, item->floor, (long)ccp.z, &room_number);
	ccp.y = (float)GetHeight(floor, (long)ccp.x, item->floor, (long)ccp.z);

	if (abs(ccp.y - item->floor) > POINT_HEIGHT_CORRECTION)
		ccp.y = (float)item->floor;

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
	D3DTLVERTEX* v;
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* sXYZ;
	long* hXZ;
	long* hY;
	float uStep, vStep;
	long sxyz[GRID_POINTS * 3];
	long hxz[GRID_POINTS * 2];
	long hy[GRID_POINTS];
	long p, x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, xSize, zSize, xDist, zDist;
	short room_number, s;

	v = aVertexBuffer;
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	uStep = (sprite->x2 - sprite->x1) / (LINE_POINTS - 1);
	vStep = (sprite->y2 - sprite->y1) / (LINE_POINTS - 1);

	xSize = size * (box[1] - box[0]) / 128;
	zSize = size * (box[5] - box[4]) / 128;
	xDist = xSize / LINE_POINTS;
	zDist = zSize / LINE_POINTS;
	x = -xDist - (xDist >> 1);
	z = zDist + (zDist >> 1);
	sXYZ = sxyz;
	hXZ = hxz;

	for (int i = 0; i < LINE_POINTS; i++, z -= zDist)
	{
		for (int j = 0; j < LINE_POINTS; j++, sXYZ += 3, hXZ += 2, x += xDist)
		{
			sXYZ[0] = x;
			sXYZ[2] = z;
			hXZ[0] = x;
			hXZ[1] = z;
		}

		x = -xDist - (xDist >> 1);
	}

	phd_PushUnitMatrix();
	s = item->current_anim_state;

	if (item == lara_item && s != AS_ALL4S && s != AS_ALL4TURNL && s != AS_ALL4TURNR && s != AS_CRAWL && s != AS_CRAWLBACK)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LMX_HIPS);
		room_number = lara_item->room_number;
		pos.y = GetHeight(GetFloor(pos.x, pos.y, pos.z, &room_number), pos.x, pos.y, pos.z);

		if (pos.y == NO_HEIGHT)
			pos.y = item->floor;
	}
	else
	{
		pos.x = item->pos.x_pos;
		pos.y = item->floor;
		pos.z = item->pos.z_pos;
	}

	pos.y -= 16;
	phd_TranslateRel(pos.x, pos.y, pos.z);
	phd_RotY(item->pos.y_rot);
	hXZ = hxz;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2)
	{
		x = hXZ[0];
		z = hXZ[1];
		hXZ[0] = long(x * aMXPtr[M00] + z * aMXPtr[M02] + aMXPtr[M03]);
		hXZ[1] = long(x * aMXPtr[M20] + z * aMXPtr[M22] + aMXPtr[M23]);
	}

	phd_PopMatrix();

	hXZ = hxz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2, hY++)
	{
		room_number = item->room_number;
		*hY = GetHeight(GetFloor(hXZ[0], item->floor, hXZ[1], &room_number), hXZ[0], item->floor, hXZ[1]);

		if (abs(*hY - item->floor) > POINT_HEIGHT_CORRECTION)
			*hY = item->floor;
	}

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, pos.y, pos.z);
	phd_RotY(item->pos.y_rot);
	sXYZ = sxyz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, sXYZ += 3, hY++)
	{
		x = sXYZ[0];
		y = *hY - item->floor;
		z = sXYZ[2];
		sXYZ[0] = long(aMXPtr[M00] * x + aMXPtr[M01] * y + aMXPtr[M02] * z + aMXPtr[M03]);
		sXYZ[1] = long(aMXPtr[M10] * x + aMXPtr[M11] * y + aMXPtr[M12] * z + aMXPtr[M13]);
		sXYZ[2] = long(aMXPtr[M20] * x + aMXPtr[M21] * y + aMXPtr[M22] * z + aMXPtr[M23]);
	}

	phd_PopMatrix();

	tex.drawtype = 5;
	tex.tpage = sprite->tpage;
	tex.flag = 0;

	sXYZ = sxyz;

	for (int i = 0; i < LINE_POINTS - 1; i++)
	{
		for (int j = 0; j < LINE_POINTS - 1; j++)
		{
			p = (j * 3) + (i * 12);
			x1 = sXYZ[p + 0];
			y1 = sXYZ[p + 1];
			z1 = sXYZ[p + 2];
			x2 = sXYZ[p + 3];
			y2 = sXYZ[p + 4];
			z2 = sXYZ[p + 5];

			p = (j * 3) + ((i + 1) * 12);
			x3 = sXYZ[p + 0];
			y3 = sXYZ[p + 1];
			z3 = sXYZ[p + 2];
			x4 = sXYZ[p + 3];
			y4 = sXYZ[p + 4];
			z4 = sXYZ[p + 5];

			setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);

			for (int k = 0; k < 4; k++)
			{
				v[k].color = 0xFF2D2D2D;
				v[k].specular = 0xFF000000;
			}

			tex.u1 = sprite->x1 + (uStep * j);
			tex.v1 = sprite->y1 + (vStep * i);

			tex.u2 = tex.u1 + uStep;
			tex.v2 = tex.v1;

			tex.u3 = tex.u1 + uStep;
			tex.v3 = tex.v1 + vStep;

			tex.u4 = tex.u1;
			tex.v4 = tex.v1 + vStep;

			AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
		}
	}
}

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	float* sXYZ;
	float* hXZ;
	long* hY;
	float sxyz[GRID_POINTS * 3];
	float hxz[GRID_POINTS * 2];
	long hy[GRID_POINTS];
	long triA, triB, triC;
	float fx, fy, fz;
	long x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number, s;

	if (tomb5.shadow_mode != 1)
	{
		if (tomb5.shadow_mode == 4)
			S_PrintSpriteShadow(size, box, item);
		else
			S_PrintCircleShadow(size, box, item);

		return;
	}

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
			hXZ[0] = sXYZ[0];				//fill height XZ array with the points of the grid
			hXZ[1] = sXYZ[2];
		}

		x = -xDist - (xDist >> 1);
	}

	phd_PushUnitMatrix();
	s = item->current_anim_state;

	if (item == lara_item && s != AS_ALL4S && s != AS_ALL4TURNL && s != AS_ALL4TURNR && s != AS_CRAWL && s != AS_CRAWLBACK)	//position the grid
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LMX_TORSO);
		room_number = lara_item->room_number;
		floor = GetFloor(pos.x, pos.y, pos.z, &room_number);
		y = GetHeight(floor, pos.x, pos.y, pos.z);

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
		fx = hXZ[0];
		fz = hXZ[1];
		hXZ[0] = fx * aMXPtr[M00] + fz * aMXPtr[M02] + aMXPtr[M03];
		hXZ[1] = fx * aMXPtr[M20] + fz * aMXPtr[M22] + aMXPtr[M23];
	}

	phd_PopMatrix();

	hXZ = hxz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2, hY++)	//Get height on each grid point and store it in hy array
	{
		room_number = item->room_number;
		floor = GetFloor((long)hXZ[0], item->floor, (long)hXZ[1], &room_number);
		*hY = GetHeight(floor, (long)hXZ[0], item->floor, (long)hXZ[1]);

		if (abs(*hY - item->floor) > POINT_HEIGHT_CORRECTION)
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
