#include "../tomb5/pch.h"
#include "polyinsert.h"
#include "dxshell.h"
#include "drawroom.h"
#include "function_table.h"
#include "clipping.h"
#include "output.h"
#include "winmain.h"
#include "texture.h"
#include "function_stubs.h"
#include "3dmath.h"
#include "gamemain.h"

SORTLIST* SortList[65536];
long SortCount;

static char SortBuffer[2621440];
static SORTLIST** pSortList;
static char* pSortBuffer;

D3DTLBUMPVERTEX XYUVClipperBuffer[20];
D3DTLBUMPVERTEX zClipperBuffer[20];

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type)
{
	switch (type)
	{
	case 0:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		break;

	case 1:
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		break;

	case 2:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	case 3:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
		break;

	case 4:
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));

		if (App.dx.lpZBuffer)
		{
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
		}

		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

		if (App.dx.lpZBuffer)
		{
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
		}

		break;

	case 5:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		break;

	case 6:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_LINELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		break;

	case 7:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		break;
	}
}

void DrawSortList()
{
	SORTLIST* pSort;
	D3DTLBUMPVERTEX* vtx;
	D3DTLBUMPVERTEX* bVtx;
	D3DTLBUMPVERTEX* bVtxbak;
	long num;
	short nVtx, tpage, drawtype, total_nVtx;

	nVtx = 0;

	if (!SortCount)
		return;

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);

	if (!App.dx.lpZBuffer)
	{
		for (int i = 0; i < SortCount; i++)
		{
			pSort = SortList[i];
			vtx = (D3DTLBUMPVERTEX*)(pSort + 1);

			if (pSort->polytype == 4)
				App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);

			HWR_DrawSortList(vtx, pSort->nVtx, pSort->tpage, pSort->drawtype);

			if (pSort->polytype == 4)
				App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);

		}
	}
	else
	{
		pSort = SortList[0];

		for (num = 0; num < SortCount; num++)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 0 || pSort->drawtype == 1 || pSort->drawtype == 4)
				break;
		}

		bVtxbak = Bucket[0].vtx;
		bVtx = bVtxbak;
		tpage = pSort->tpage;
		drawtype = pSort->drawtype;

		for (; num < SortCount; num++)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 0 || pSort->drawtype == 1 || pSort->drawtype == 4)
			{
				if (pSort->drawtype == drawtype && pSort->tpage == tpage)
				{
					vtx = (D3DTLBUMPVERTEX*)(pSort + 1);

					for (int i = 0; i < pSort->nVtx; i++, vtx++, bVtx++)
					{
						bVtx->sx = vtx->sx;
						bVtx->sy = vtx->sy;
						bVtx->sz = vtx->sz;
						bVtx->rhw = vtx->rhw;
						bVtx->color = vtx->color;
						bVtx->specular = vtx->specular;
						bVtx->tu = vtx->tu;
						bVtx->tv = vtx->tv;
						nVtx++;
					}
				}
				else
				{
					HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);	//inlined
					drawtype = pSort->drawtype;
					tpage = pSort->tpage;
					bVtx = bVtxbak;
					nVtx = 0;
					num--;
				}
			}
		}

		if (nVtx)
			HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);

		for (num = SortCount - 1; num >= 0; num--)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
				break;
		}

		tpage = pSort->tpage;
		drawtype = pSort->drawtype;
		bVtx = bVtxbak;
		nVtx = 0;
		total_nVtx = 0;

		for (; num >= 0; num--)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
			{
				if (pSort->tpage == tpage && pSort->drawtype == drawtype)
				{
					vtx = (D3DTLBUMPVERTEX*)(pSort + 1);
					total_nVtx += pSort->nVtx;

					if (total_nVtx >= 2076)
					{
						HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
						nVtx = 0;
						bVtx = bVtxbak;
						total_nVtx = 0;
					}

					for (int i = 0; i < pSort->nVtx; i++, vtx++, bVtx++)
					{
						bVtx->sx = vtx->sx;
						bVtx->sy = vtx->sy;
						bVtx->sz = vtx->sz;
						bVtx->rhw = vtx->rhw;
						bVtx->color = vtx->color;
						bVtx->specular = vtx->specular;
						bVtx->tu = vtx->tu;
						bVtx->tv = vtx->tv;
						nVtx++;
					}
				}
				else
				{
					HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
					tpage = pSort->tpage;
					nVtx = 0;
					drawtype = pSort->drawtype;
					bVtx = bVtxbak;
					num++;
				}
			}
		}

		if (nVtx)
			HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
	}

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	InitBuckets();
}

void CalcColorSplit(D3DCOLOR s, D3DCOLOR* d)
{
	long r, g, b, sr, sg, sb;

	sr = 0;
	sg = 0;
	sb = 0;
	r = CLRR(s);
	g = CLRG(s);
	b = CLRB(s);
	r -= 128;
	g -= 128;
	b -= 128;

	if (r <= 0)
		r = CLRR(s) << 1;
	else
	{
		sr = r;
		r = 255;
	}

	if (g <= 0)
		g = CLRG(s) << 1;
	else
	{
		sg = g;
		g = 255;
	}

	if (b <= 0)
		b = CLRB(s) << 1;
	else
	{
		sb = b;
		b = 255;
	}

	if (r > 255)
		r = 255;

	if (g > 255)
		g = 255;

	if (b > 255)
		b = 255;

	d[0] = (d[0] & 0xFF000000) | RGBONLY(r, g, b);		//color
	d[0] &= 0xFFFFFF;
	d[0] |= GlobalAlpha;

	sr >>= 1;
	sg >>= 1;
	sb >>= 1;
	d[1] = (d[1] & 0xFF000000) | RGBONLY(sr, sg, sb);	//specular
}

void InitialiseSortList()
{
	pSortBuffer = SortBuffer;
	pSortList = SortList;
	SortCount = 0;
}

void DoSort(long left, long right, SORTLIST** list)
{
	SORTLIST* swap;
	float z;
	long l, r;

	l = left;
	r = right;
	z = list[(left + right) / 2]->zVal;

	do
	{
		while (l < right && list[l]->zVal > z)
			l++;

		while (r > left && list[r]->zVal < z)
			r--;

		if (l <= r)
		{
			swap = list[l];
			list[l] = list[r];
			list[r] = swap;
			l++;
			r--;
		}

	} while (l <= r);

	if (r > left)
		DoSort(left, r, list);

	if (l < right)
		DoSort(l, right, list);
}

void SortPolyList(long count, SORTLIST** list)
{
	if (!count)
		return;

	for (int i = 0; i < count; i++)
		list[i]->zVal -= (float)i * 0.1F;

	DoSort(0, count - 1, list);
}

void AddClippedPoly(D3DTLBUMPVERTEX* dest, long nPoints, D3DTLBUMPVERTEX* v, TEXTURESTRUCT* pTex)
{
	D3DTLBUMPVERTEX* p;
	float z;

	p = dest;

	for (int i = 0; i < 3; i++, v++, p++)
	{
		p->sx = v->sx;
		p->sy = v->sy;
		p->sz = f_a - f_boo * v->rhw;
		p->rhw = v->rhw;
		p->color = v->color;
		p->specular = v->specular;
		z = 1.0F / v->rhw;
		p->tu = z * v->tu;
		p->tv = z * v->tv;
	}

	nPoints -= 3;
	v--;

	for (int i = nPoints; i > 0; i--)
	{
		v++;
		p->sx = dest->sx;
		p->sy = dest->sy;
		p->sz = dest->sz;
		p->rhw = dest->rhw;
		p->color = dest->color;
		p->specular = dest->specular;
		p->tu = dest->tu;
		p->tv = dest->tv;
		p++;

		p->sx = p[-2].sx;
		p->sy = p[-2].sy;
		p->sz = p[-2].sz;
		p->rhw = p[-2].rhw;
		p->color = p[-2].color;
		p->specular = p[-2].specular;
		p->tu = p[-2].tu;
		p->tv = p[-2].tv;
		p++;

		p->sx = v->sx;
		p->sy = v->sy;
		p->sz = f_a - f_boo * v->rhw;
		p->rhw = v->rhw;
		p->color = v->color;
		p->specular = v->specular;
		z = 1.0F / v->rhw;
		p->tu = z * v->tu;
		p->tv = z * v->tv;
		p++;
	}
}

void SubdivideEdge(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v, short* c, float tu1, float tv1, float tu2, float tv2, float* tu, float* tv)
{
	float zv;
	short cf, r0, g0, b0, a0, r1, g1, b1, a1;

	cf = 0;
	*tu = (tu1 + tu2) * 0.5F;
	*tv = (tv1 + tv2) * 0.5F;
	v->sx = (v0->tu + v1->tu) * 0.5F;
	v->sy = (v0->tv + v1->tv) * 0.5F;
	v->sz = (v0->sz + v1->sz) * 0.5F;
	v->tu = v->sx;
	v->tv = v->sy;

	if (v->sz < f_mznear)
		cf = -128;
	else
	{
		zv = f_mpersp / v->sz;
		v->sx = zv * v->sx + f_centerx;
		v->sy = zv * v->sy + f_centery;
		v->rhw = f_moneopersp * zv;

		if (v->sx < f_left)
			cf++;
		else if (v->sx > f_right)
			cf += 2;

		if (v->sy < f_top)
			cf += 4;
		else if (v->sy > f_bottom)
			cf += 8;
	}

	*c = cf;

	a0 = CLRA(v0->color);
	r0 = CLRR(v0->color);
	g0 = CLRG(v0->color);
	b0 = CLRB(v0->color);

	a1 = CLRA(v1->color);
	r1 = CLRR(v1->color);
	g1 = CLRG(v1->color);
	b1 = CLRB(v1->color);

	a1 = (a0 + a1) >> 1;
	r1 = (r0 + r1) >> 1;
	g1 = (g0 + g1) >> 1;
	b1 = (b0 + b1) >> 1;
	v->color = RGBA(r1, g1, b1, a1);

	a0 = CLRA(v0->specular);
	r0 = CLRR(v0->specular);
	g0 = CLRG(v0->specular);
	b0 = CLRB(v0->specular);

	a1 = CLRA(v1->specular);
	r1 = CLRR(v1->specular);
	g1 = CLRG(v1->specular);
	b1 = CLRB(v1->specular);

	a1 = (a0 + a1) >> 1;
	r1 = (r0 + r1) >> 1;
	g1 = (g0 + g1) >> 1;
	b1 = (b0 + b1) >> 1;
	v->specular = RGBA(r1, g1, b1, a1);
}

void SubdivideQuad(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, D3DTLVERTEX* v3, TEXTURESTRUCT* tex, long double_sided, long steps, short* c)
{
	D3DTLVERTEX v[5];
	TEXTURESTRUCT tex2;
	float uv[10];
	short aclip[5];
	short bclip[4];

	if (!steps)
	{
		bclip[0] = clipflags[0];
		bclip[1] = clipflags[1];
		bclip[2] = clipflags[2];
		bclip[3] = clipflags[3];
		clipflags[0] = c[0];
		clipflags[1] = c[1];
		clipflags[2] = c[2];
		clipflags[3] = c[3];
		v[0] = *v0;
		v[1] = *v1;
		v[2] = *v2;
		v[3] = *v3;
		AddQuadClippedSorted(v, 0, 1, 2, 3, tex, double_sided);
		clipflags[0] = bclip[0];
		clipflags[1] = bclip[1];
		clipflags[2] = bclip[2];
		clipflags[3] = bclip[3];
		return;
	}

	tex2.drawtype = tex->drawtype;
	tex2.tpage = tex->tpage;
	tex2.flag = tex->flag;

	SubdivideEdge(v0, v1, &v[0], &aclip[0], tex->u1, tex->v1, tex->u2, tex->v2, &uv[0], &uv[1]);
	SubdivideEdge(v2, v3, &v[1], &aclip[1], tex->u3, tex->v3, tex->u4, tex->v4, &uv[2], &uv[3]);
	SubdivideEdge(v0, v3, &v[2], &aclip[2], tex->u1, tex->v1, tex->u4, tex->v4, &uv[4], &uv[5]);
	SubdivideEdge(v1, v2, &v[3], &aclip[3], tex->u2, tex->v2, tex->u3, tex->v3, &uv[6], &uv[7]);
	SubdivideEdge(v0, v2, &v[4], &aclip[4], tex->u1, tex->v1, tex->u3, tex->v3, &uv[8], &uv[9]);

	tex2.v1 = tex->v1;
	tex2.u1 = tex->u1;
	tex2.u2 = uv[0];
	tex2.v2 = uv[1];
	tex2.u3 = uv[8];
	tex2.v3 = uv[9];
	tex2.v4 = uv[5];
	tex2.u4 = uv[4];

	bclip[0] = *c;
	bclip[1] = aclip[0];
	bclip[2] = aclip[4];
	bclip[3] = aclip[2];
	SubdivideQuad(v0, &v[0], &v[4], &v[2], &tex2, double_sided, steps - 1, bclip);

	tex2.u1 = uv[0];
	tex2.v1 = uv[1];
	tex2.u2 = tex->u2;
	tex2.v2 = tex->v2;
	tex2.u3 = uv[6];
	tex2.v3 = uv[7];
	tex2.u4 = uv[8];
	tex2.v4 = uv[9];

	bclip[0] = aclip[0];
	bclip[1] = c[1];
	bclip[2] = aclip[3];
	bclip[3] = aclip[4];
	SubdivideQuad(&v[0], v1, &v[3], &v[4], &tex2, double_sided, steps - 1, bclip);

	tex2.u1 = uv[8];
	tex2.v1 = uv[9];
	tex2.u2 = uv[6];
	tex2.v2 = uv[7];
	tex2.u3 = tex->u3;
	tex2.v3 = tex->v3;
	tex2.u4 = uv[2];
	tex2.v4 = uv[3];

	bclip[0] = aclip[4];
	bclip[1] = aclip[3];
	bclip[2] = c[2];
	bclip[3] = aclip[1];
	SubdivideQuad(&v[4], &v[3], v2, &v[1], &tex2, double_sided, steps - 1, bclip);

	tex2.u1 = uv[4];
	tex2.v1 = uv[5];
	tex2.u2 = uv[8];
	tex2.v2 = uv[9];
	tex2.u3 = uv[2];
	tex2.v3 = uv[3];
	tex2.u4 = tex->u4;
	tex2.v4 = tex->v4;

	bclip[0] = aclip[2];
	bclip[1] = aclip[4];
	bclip[2] = aclip[1];
	bclip[3] = c[3];
	SubdivideQuad(&v[2], &v[4], &v[1], v3, &tex2, double_sided, steps - 1, bclip);
}

void SubdivideTri(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, TEXTURESTRUCT* tex, long double_sided, long steps, short* c)
{
	D3DTLVERTEX v[3];
	TEXTURESTRUCT tex2;
	float uv[6];
	short bclip[4];
	short aclip[3];

	if (!steps)
	{
		bclip[0] = clipflags[0];
		bclip[1] = clipflags[1];
		bclip[2] = clipflags[2];
		clipflags[0] = c[0];
		clipflags[1] = c[1];
		clipflags[2] = c[2];
		v[0] = *v0;
		v[1] = *v1;
		v[2] = *v2;
		AddTriClippedSorted(v, 0, 1, 2, tex, double_sided);
		clipflags[0] = bclip[0];
		clipflags[1] = bclip[1];
		clipflags[2] = bclip[2];
		return;
	}

	tex2.drawtype = tex->drawtype;
	tex2.tpage = tex->tpage;
	tex2.flag = tex->flag;
	SubdivideEdge(v0, v1, &v[0], &aclip[0], tex->u1, tex->v1, tex->u2, tex->v2, &uv[0], &uv[1]);
	SubdivideEdge(v1, v2, &v[1], &aclip[1], tex->u2, tex->v2, tex->u3, tex->v3, &uv[2], &uv[3]);
	SubdivideEdge(v2, v0, &v[2], &aclip[2], tex->u3, tex->v3, tex->u1, tex->v1, &uv[4], &uv[5]);

	tex2.u1 = tex->u1;
	tex2.v1 = tex->v1;
	tex2.u2 = uv[0];
	tex2.v2 = uv[1];
	tex2.u3 = uv[4];
	tex2.v3 = uv[5];

	bclip[0] = c[0];
	bclip[1] = aclip[0];
	bclip[2] = aclip[2];
	SubdivideTri(v0, &v[0], &v[2], &tex2, double_sided, steps - 1, bclip);

	tex2.u1 = uv[0];
	tex2.v1 = uv[1];
	tex2.u2 = tex->u2;
	tex2.v2 = tex->v2;
	tex2.u3 = uv[2];
	tex2.v3 = uv[3];

	bclip[0] = aclip[0];
	bclip[1] = c[1];
	bclip[2] = aclip[1];
	SubdivideTri(&v[0], v1, &v[1], &tex2, double_sided, steps - 1, bclip);

	tex2.u2 = uv[0];
	tex2.u1 = uv[4];
	tex2.v1 = uv[5];
	tex2.v3 = uv[3];
	tex2.v2 = uv[1];
	tex2.u3 = uv[2];
	tex2.u4 = tex->u3;
	tex2.v4 = tex->v3;

	bclip[0] = aclip[2];
	bclip[1] = aclip[0];
	bclip[2] = aclip[1];
	bclip[3] = c[2];
	SubdivideQuad(&v[2], v, &v[1], v2, &tex2, double_sided, steps - 1, bclip);
}

void AddTriSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided)
{
	long steps;
	short c[4];

	if (nPolyType || v[v2].sz >= 3500)
		AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
	else
	{
		steps = 1;

		if (v[v2].sz < 2000)
			steps = 2;

		c[0] = clipflags[v0];
		c[1] = clipflags[v1];
		c[2] = clipflags[v2];
		SubdivideTri(&v[v0], &v[v1], &v[v2], tex, double_sided, steps, c);
	}
}

void AddQuadSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided)
{
	long steps;
	short c[4];

	if (nPolyType || v[v3].sz >= 3500)
		AddQuadClippedSorted(v, v0, v1, v2, v3, tex, double_sided);
	else
	{
		steps = 1;

		if (v[v3].sz < 2000)
			steps = 2;

		c[0] = clipflags[v0];
		c[1] = clipflags[v1];
		c[2] = clipflags[v2];
		c[3] = clipflags[v3];
		SubdivideQuad(&v[v0], &v[v1], &v[v2], &v[v3], tex, double_sided, steps, c);
	}
}

void AddQuadClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided)
{
	D3DTLBUMPVERTEX* p;
	D3DTLVERTEX* pV;
	SORTLIST* sl;
	TEXTURESTRUCT tex2;
	short* c;
	float z;
	short swap;

	c = clipflags;

	if (c[v0] & c[v1] & c[v2] & c[v3])
		return;

	if ((c[v0] | c[v1] | c[v2] | c[v3]) & 0x8000)
	{
		AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u1;
		tex2.v1 = tex->v1;
		tex2.u2 = tex->u3;
		tex2.v2 = tex->v3;
		tex2.u3 = tex->u4;
		tex2.v3 = tex->v4;
		AddTriClippedSorted(v, v0, v2, v3, &tex2, double_sided);
		return;
	}

	if (IsVisible(&v[v0], &v[v1], &v[v2]))
	{
		if (!double_sided)
			return;

		swap = v0;
		v0 = v2;
		v2 = swap;
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u3;
		tex2.v1 = tex->v3;
		tex2.u2 = tex->u2;
		tex2.v2 = tex->v2;
		tex2.u3 = tex->u1;
		tex2.v3 = tex->v1;
		tex2.u4 = tex->u4;
		tex2.v4 = tex->v4;
		tex = &tex2;
	}

	if (!double_sided && IsVisible(&v[v0], &v[v1], &v[v2]))		//redundant
		return;

	if (c[v0] | c[v1] | c[v2] | c[v3])
	{
		AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u1;
		tex2.v1 = tex->v1;
		tex2.u2 = tex->u3;
		tex2.v2 = tex->v3;
		tex2.u3 = tex->u4;
		tex2.v3 = tex->v4;
		AddTriClippedSorted(v, v0, v2, v3, &tex2, double_sided);
		return;
	}

	p = (D3DTLBUMPVERTEX*)(sizeof(SORTLIST) + pSortBuffer);
	sl = (SORTLIST*)pSortBuffer;
	sl->drawtype = tex->drawtype;
	sl->tpage = tex->tpage;
	sl->nVtx = 6;
	sl->polytype = (short)nPolyType;
	pSortBuffer += 6 * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
	*pSortList++ = sl;
	SortCount++;
	z = 0;

	pV = &v[v0];
	p->sx = pV->sx;
	p->sy = pV->sy;
	p->sz = f_a - f_boo * pV->rhw;
	p->rhw = pV->rhw;
	p->color = pV->color;
	p->specular = pV->specular;
	p->tu = tex->u1;
	p->tv = tex->v1;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	p[3].sx = p->sx;
	p[3].sy = p->sy;
	p[3].sz = p->sz;
	p[3].rhw = p->rhw;
	p[3].color = p->color;
	p[3].specular = p->specular;
	p[3].tu = p->tu;
	p[3].tv = p->tv;

	pV = &v[v1];
	p[1].sx = pV->sx;
	p[1].sy = pV->sy;
	p[1].sz = f_a - f_boo * pV->rhw;
	p[1].rhw = pV->rhw;
	p[1].color = pV->color;
	p[1].specular = pV->specular;
	p[1].tu = tex->u2;
	p[1].tv = tex->v2;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	pV = &v[v2];
	p[2].sx = pV->sx;
	p[2].sy = pV->sy;
	p[2].sz = f_a - f_boo * pV->rhw;
	p[2].rhw = pV->rhw;
	p[2].color = pV->color;
	p[2].specular = pV->specular;
	p[2].tu = tex->u3;
	p[2].tv = tex->v3;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	p[4].sx = p[2].sx;
	p[4].sy = p[2].sy;
	p[4].sz = p[2].sz;
	p[4].rhw = p[2].rhw;
	p[4].color = p[2].color;
	p[4].specular = p[2].specular;
	p[4].tu = p[2].tu;
	p[4].tv = p[2].tv;

	pV = &v[v3];
	p[5].sx = pV->sx;
	p[5].sy = pV->sy;
	p[5].sz = f_a - f_boo * pV->rhw;
	p[5].rhw = pV->rhw;
	p[5].color = pV->color;
	p[5].specular = pV->specular;
	p[5].tu = tex->u4;
	p[5].tv = tex->v4;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	if (nPolyType)
		z *= 0.25F;

	sl->zVal = z;
}

void AddTriClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided)
{
	D3DTLBUMPVERTEX* p;
	D3DTLVERTEX* pV;
	SORTLIST* sl;
	TEXTURESTRUCT tex2;
	short* c;
	float z;
	long num;
	short swap;
	bool clip, clipZ;

	c = clipflags;
	clipZ = 0;
	clip = 1;
	sl = 0;

	if (c[v0] & c[v1] & c[v2])
		return;

	if ((c[v0] | c[v1] | c[v2]) & 0x8000)
	{
		if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
		{
			if (!double_sided)
				return;

			swap = v1;
			v1 = v2;
			v2 = swap;

			if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
				return;

			tex2.drawtype = tex->drawtype;
			tex2.flag = tex->flag;
			tex2.tpage = tex->tpage;
			tex2.u1 = tex->u1;
			tex2.v1 = tex->v1;
			tex2.u2 = tex->u3;
			tex2.v2 = tex->v3;
			tex2.u3 = tex->u2;
			tex2.v3 = tex->v2;
			tex = &tex2;
		}

		clipZ = 1;
		p = zClipperBuffer;
	}
	else
	{
		if (!double_sided && IsVisible(&v[v0], &v[v1], &v[v2]))
			return;

		if (IsVisible(&v[v0], &v[v1], &v[v2]))
		{
			if (!double_sided)
				return;

			swap = v1;
			v1 = v2;
			v2 = swap;

			tex2.drawtype = tex->drawtype;
			tex2.flag = tex->flag;
			tex2.tpage = tex->tpage;
			tex2.u1 = tex->u1;
			tex2.v1 = tex->v1;
			tex2.u2 = tex->u3;
			tex2.v2 = tex->v3;
			tex2.u3 = tex->u2;
			tex2.v3 = tex->v2;
			tex = &tex2;
		}

		if (c[v0] | c[v1] | c[v2])
			p = XYUVClipperBuffer;
		else
		{
			clip = 0;
			p = (D3DTLBUMPVERTEX*)(sizeof(SORTLIST) + pSortBuffer);
			sl = (SORTLIST*)pSortBuffer;
			sl->tpage = tex->tpage;
			sl->drawtype = tex->drawtype;
			sl->nVtx = 3;
			sl->polytype = (short)nPolyType;
			pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
			*pSortList++ = sl;
		}
	}

	z = 0;

	pV = &v[v0];
	p->sx = pV->sx;
	p->sy = pV->sy;
	p->sz = pV->sz;
	p->rhw = pV->rhw;
	p->color = pV->color;
	p->specular = pV->specular;
	p->tu = tex->u1;
	p->tv = tex->v1;
	p->tx = pV->tu;
	p->ty = pV->tv;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	pV = &v[v1];
	p[1].sx = pV->sx;
	p[1].sy = pV->sy;
	p[1].sz = pV->sz;
	p[1].rhw = pV->rhw;
	p[1].color = pV->color;
	p[1].specular = pV->specular;
	p[1].tu = tex->u2;
	p[1].tv = tex->v2;
	p[1].tx = pV->tu;
	p[1].ty = pV->tv;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	pV = &v[v2];
	p[2].sx = pV->sx;
	p[2].sy = pV->sy;
	p[2].sz = pV->sz;
	p[2].rhw = pV->rhw;
	p[2].color = pV->color;
	p[2].specular = pV->specular;
	p[2].tu = tex->u3;
	p[2].tv = tex->v3;
	p[2].tx = pV->tu;
	p[2].ty = pV->tv;

	if (nPolyType)
		z += pV->sz;
	else if (pV->sz > z)
		z = pV->sz;

	if (clip)
	{
		num = 3;

		if (clipZ)
		{
			num = ZClipper(3, zClipperBuffer, XYUVClipperBuffer);

			if (!num)
				return;
		}
		else
		{
			p = XYUVClipperBuffer;

			for (int i = 0; i < 3; i++, p++)
			{
				p->tu *= p->rhw;
				p->tv *= p->rhw;
			}
		}

		num = XYUVGClipper(num, XYUVClipperBuffer);

		if (num)
		{
			p = (D3DTLBUMPVERTEX*)(sizeof(SORTLIST) + pSortBuffer);
			sl = (SORTLIST*)pSortBuffer;
			sl->drawtype = tex->drawtype;
			sl->nVtx = short(3 * num - 6);

			if (nPolyType)
				sl->zVal = z * 0.333333F;
			else
				sl->zVal = z;

			sl->tpage = tex->tpage;
			sl->polytype = (short)nPolyType;
			pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
			*pSortList++ = sl;
			SortCount++;
			AddClippedPoly(p, num, XYUVClipperBuffer, tex);
		}
	}
	else
	{
		p->sz = f_a - f_boo * p->rhw;
		p[1].sz = f_a - f_boo * p[1].rhw;
		p[2].sz = f_a - f_boo * p[2].rhw;

		if (nPolyType)
			sl->zVal = z * 0.333333F;
		else
			sl->zVal = z;

		SortCount++;
	}
}

void AddLineClippedSorted(D3DTLVERTEX* v0, D3DTLVERTEX* v1, short drawtype)
{
	D3DTLBUMPVERTEX* v;
	SORTLIST* sl;

	v = (D3DTLBUMPVERTEX*)(sizeof(SORTLIST) + pSortBuffer);
	sl = (SORTLIST*)pSortBuffer;
	sl->tpage = 0;
	sl->drawtype = drawtype;
	sl->nVtx = 2;
	pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
	*pSortList++ = sl;
	sl->zVal = v0->sz;
	SortCount++;
	v->sx = v0->sx;
	v->sy = v0->sy;
	v->sz = f_a - f_boo * v0->rhw;
	v->rhw = v0->rhw;
	v->color = v0->color;
	v->specular = v0->specular;
	v[1].sx = v1->sx;
	v[1].sy = v1->sy;
	v[1].sz = f_a - f_boo * v1->rhw;
	v[1].rhw = v1->rhw;
	v[1].color = v1->color;
	v[1].specular = v1->specular;
}

void AddQuadClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided)
{
	D3DTLBUMPVERTEX* p;
	D3DTLBUMPVERTEX* bp;
	D3DTLVERTEX* vtx;
	TEXTURESTRUCT tex2;
	long* nVtx;
	short* c;
	short swap;

	c = clipflags;

	if (c[v0] & c[v1] & c[v2] & c[v3])
		return;

	if ((c[v0] | c[v1] | c[v2] | c[v3]) < 0)
	{
		AddTriClippedZBuffer(v, v0, v1, v2, tex, double_sided);
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u1;
		tex2.v1 = tex->v1;
		tex2.u2 = tex->u3;
		tex2.v2 = tex->v3;
		tex2.u3 = tex->u4;
		tex2.v3 = tex->v4;
		AddTriClippedZBuffer(v, v0, v2, v3, &tex2, double_sided);
		return;
	}

	if (!double_sided && IsVisible(&v[v0], &v[v1], &v[v2]))
		return;

	if (IsVisible(&v[v0], &v[v1], &v[v2]))
	{
		if (!double_sided)
			return;

		swap = v0;
		v0 = v2;
		v2 = swap;
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u3;
		tex2.v1 = tex->v3;
		tex2.u2 = tex->u2;
		tex2.v2 = tex->v2;
		tex2.u3 = tex->u1;
		tex2.v3 = tex->v1;
		tex2.u4 = tex->u4;
		tex2.v4 = tex->v4;
		tex = &tex2;
	}

	if (c[v0] | c[v1] | c[v2] | c[v3])
	{
		AddTriClippedZBuffer(v, v0, v1, v2, tex, double_sided);
		tex2.drawtype = tex->drawtype;
		tex2.flag = tex->flag;
		tex2.tpage = tex->tpage;
		tex2.u1 = tex->u1;
		tex2.v1 = tex->v1;
		tex2.u2 = tex->u3;
		tex2.v2 = tex->v3;
		tex2.u3 = tex->u4;
		tex2.v3 = tex->v4;
		AddTriClippedZBuffer(v, v0, v2, v3, &tex2, double_sided);
		return;
	}

	FindBucket(tex->tpage, &p, &nVtx);
	*nVtx += 6;
	bp = p;

	vtx = &v[v0];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = f_a - f_boo * vtx->rhw;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tu = tex->u1;
	p->tv = tex->v1;

	p[3].sx = p->sx;
	p[3].sy = p->sy;
	p[3].sz = p->sz;
	p[3].rhw = p->rhw;
	p[3].color = p->color;
	p[3].specular = p->specular;
	p[3].tu = p->tu;
	p[3].tv = p->tv;
	p++;

	vtx = &v[v1];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = f_a - f_boo * vtx->rhw;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tu = tex->u2;
	p->tv = tex->v2;
	p++;

	vtx = &v[v2];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = f_a - f_boo * vtx->rhw;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tu = tex->u3;
	p->tv = tex->v3;

	p[2].sx = p->sx;
	p[2].sy = p->sy;
	p[2].sz = p->sz;
	p[2].rhw = p->rhw;
	p[2].color = p->color;
	p[2].specular = p->specular;
	p[2].tu = p->tu;
	p[2].tv = p->tv;

	p += 3;
	vtx = &v[v3];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = f_a - f_boo * vtx->rhw;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tu = tex->u4;
	p->tv = tex->v4;
}

void AddTriClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided)
{
	D3DTLVERTEX* vtx;
	D3DTLBUMPVERTEX* p;
	D3DTLBUMPVERTEX* bp;
	TEXTURESTRUCT tex2;
	long* nVtx;
	short* c;
	long nPoints;
	short swap;
	bool clip, clipZ;

	c = clipflags;
	clipZ = 0;

	if (c[v0] & c[v1] & c[v2])
		return;

	if ((c[v0] | c[v1] | c[v2]) < 0)
	{
		if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
		{
			if (!double_sided)
				return;

			swap = v1;
			v1 = v2;
			v2 = swap;

			if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
				return;

			tex2.drawtype = tex->drawtype;
			tex2.flag = tex->flag;
			tex2.tpage = tex->tpage;
			tex2.u1 = tex->u1;
			tex2.v1 = tex->v1;
			tex2.u2 = tex->u3;
			tex2.v2 = tex->v3;
			tex2.u3 = tex->u2;
			tex2.v3 = tex->v2;
			tex = &tex2;
		}

		clip = 1;
		clipZ = 1;
		p = zClipperBuffer;
	}
	else
	{
		if (IsVisible(&v[v0], &v[v1], &v[v2]))
		{
			if (!double_sided)
				return;

			swap = v1;
			v1 = v2;
			v2 = swap;
			tex2.drawtype = tex->drawtype;
			tex2.flag = tex->flag;
			tex2.tpage = tex->tpage;
			tex2.u1 = tex->u1;
			tex2.v1 = tex->v1;
			tex2.u2 = tex->u3;
			tex2.v2 = tex->v3;
			tex2.u3 = tex->u2;
			tex2.v3 = tex->v2;
			tex = &tex2;
		}

		if (c[v0] | c[v1] | c[v2])
		{
			p = XYUVClipperBuffer;
			clip = 1;
		}
		else
		{
			clip = 0;
			FindBucket(tex->tpage, &p, &nVtx);
			*nVtx += 3;
		}
	}

	bp = p;

	vtx = &v[v0];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = vtx->sz;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tx = vtx->tu;
	p->ty = vtx->tv;
	p->tu = tex->u1;
	p->tv = tex->v1;

	p++;
	vtx = &v[v1];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = vtx->sz;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tx = vtx->tu;
	p->ty = vtx->tv;
	p->tu = tex->u2;
	p->tv = tex->v2;

	p++;
	vtx = &v[v2];
	p->sx = vtx->sx;
	p->sy = vtx->sy;
	p->sz = vtx->sz;
	p->rhw = vtx->rhw;
	p->color = vtx->color;
	p->specular = vtx->specular;
	p->tx = vtx->tu;
	p->ty = vtx->tv;
	p->tu = tex->u3;
	p->tv = tex->v3;

	if (clip)
	{
		nPoints = 3;

		if (clipZ)
		{
			nPoints = ZClipper(3, zClipperBuffer, XYUVClipperBuffer);

			if (!nPoints)
				return;
		}
		else
		{
			p = XYUVClipperBuffer;

			for (int i = 0; i < 3; i++, p++)
			{
				p->tu *= p->rhw;
				p->tv *= p->rhw;
			}
		}

		nPoints = XYUVGClipper(nPoints, XYUVClipperBuffer);

		if (nPoints)
		{
			FindBucket(tex->tpage, &p, &nVtx);
			*nVtx += 3 * nPoints - 6;
			AddClippedPoly(p, nPoints, XYUVClipperBuffer, tex);
		}
	}
	else
	{
		p = bp;
		p->sz = f_a - f_boo * p->rhw;
		p++;
		p->sz = f_a - f_boo * p->rhw;
		p++;
		p->sz = f_a - f_boo * p->rhw;
	}
}
