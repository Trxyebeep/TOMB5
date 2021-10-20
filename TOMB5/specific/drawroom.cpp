#include "../tomb5/pch.h"
#include "drawroom.h"
#include "function_table.h"

void DrawBoundsRectangle(float left, float top, float right, float bottom)
{
	D3DTLVERTEX v[8];

	for (int i = 0; i < 8; i++)
	{
		v[i].color = 0xFF00FF00;
		v[i].specular = 0xFF000000;
		clipflags[i] = 0;
	}

	v[0].sx = left;
	v[0].sy = top;
	v[1].sx = right;
	v[1].sy = top;
	v[2].sx = right;
	v[2].sy = bottom;
	v[3].sx = left;
	v[3].sy = bottom;
	v[4].sx = left + 4;
	v[4].sy = top + 4;
	v[5].sx = right + 4;
	v[5].sy = top + 4;
	v[6].sx = right + 4;
	v[6].sy = bottom + 4;
	v[7].sx = left + 4;
	v[7].sy = bottom + 4;
	v[7].rhw = f_mpersp / f_mznear * f_moneopersp;

	for (int i = 0; i < 7; i++)
	{
		v[0].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[1].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[2].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[3].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[4].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[5].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[6].rhw = f_mpersp / f_mznear * f_moneopersp;
	}

	AddQuadZBuffer(v, 0, 1, 5, 4, textinfo, 1);
	AddQuadZBuffer(v, 3, 2, 6, 7, textinfo, 1);
	AddQuadZBuffer(v, 1, 5, 6, 2, textinfo, 1);
	AddQuadZBuffer(v, 0, 4, 7, 3, textinfo, 1);
}

void DrawBoundsRectangleII(float left, float top, float right, float bottom, int rgba)
{
	D3DTLVERTEX v[8];
	TEXTURESTRUCT Tex;

	Tex.drawtype = 0;
	Tex.flag = 1;
	Tex.tpage = 0;

	for (int i = 0; i < 8; i++)
	{
		v[i].color = rgba;
		v[i].specular = 0xFF000000;
		clipflags[i] = 0;
	}

	v[0].sx = left;
	v[0].sy = top;
	v[1].sx = right;
	v[1].sy = top;
	v[2].sx = right;
	v[2].sy = bottom;
	v[3].sx = left;
	v[3].sy = bottom;
	v[4].sx = left + 2;
	v[4].sy = top + 2;
	v[5].sx = right + 2;
	v[5].sy = top + 2;
	v[6].sx = right + 2;
	v[6].sy = bottom + 2;
	v[7].sx = left + 2;
	v[7].sy = bottom + 2;

	for (int i = 0; i < 8; i++)
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;

	AddQuadZBuffer(v, 0, 1, 5, 4, &Tex, 1);
	AddQuadZBuffer(v, 3, 2, 6, 7, &Tex, 1);
	AddQuadZBuffer(v, 1, 5, 6, 2, &Tex, 1);
	AddQuadZBuffer(v, 0, 4, 7, 3, &Tex, 1);
}

void DrawClipRectangle(ROOM_INFO* r)
{
	DrawBoundsRectangle((float)r->left, (float)r->top, (float)r->right, (float)r->bottom);
}

void inject_drawroom(bool replace)
{
	INJECT(0x0049C9F0, DrawBoundsRectangle, replace);
	INJECT(0x0049CC20, DrawBoundsRectangleII, replace);
	INJECT(0x0049CE40, DrawClipRectangle, replace);
}
