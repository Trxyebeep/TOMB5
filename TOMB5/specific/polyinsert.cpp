#include "../tomb5/pch.h"
#include "polyinsert.h"
#include "dxshell.h"
#include "drawroom.h"

static long rgb80h = 0x808080;
static long rgbmask = 0xFFFFFFFF;
static long zero = 0;

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

#ifdef GENERAL_FIXES
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
#endif

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

	DrawPrimitiveCnt++;
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
#ifdef GENERAL_FIXES				//just to shut VS up
		pSort = SortList[0];		//if SortCount is < 0 then pSort will be uninitialized in the original, but I don't think that ever happens
#endif

		for (num = 0; num < SortCount; num++)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 0 || pSort->drawtype == 1 || pSort->drawtype == 4)
				break;
		}

		bVtxbak = Bucket[0].Vertex;
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
					//literal copy of HWR_DrawSortList, but ok
#ifdef GENERAL_FIXES
					HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
#else
					switch (drawtype)
					{
					case 0:

						if (App.dx.lpZBuffer)
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
						break;

					case 1:
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
						break;

					case 2:

						if (App.dx.lpZBuffer)
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
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
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
						App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
						break;

					case 4:
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));

						if (App.dx.lpZBuffer)
						{
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
						}

						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

						if (App.dx.lpZBuffer)
						{
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
						}

						break;

					case 6:

						if (App.dx.lpZBuffer)
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_LINELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
						break;

					case 7:

						if (App.dx.lpZBuffer)
							App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
						App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
						App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
						DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[tpage].tex));
						App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bVtxbak, nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
						App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
						break;
					}

					DrawPrimitiveCnt++;
#endif
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

#ifdef GENERAL_FIXES
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
#else
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7)
#endif
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

#ifdef GENERAL_FIXES
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
#else
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7)
#endif
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

void ClearFXFogBulbs()
{

}

void ControlFXBulb()
{

}

void CreateFXBulbs()
{

}

void TriggerFXFogBulb()
{

}

long IsVolumetric()
{
	return App.Volumetric;
}

void mD3DTransform(FVECTOR* vec, D3DMATRIX* mx)
{
	float x, y, z;

	x = vec->x * mx->_11 + mx->_21 * vec->y + mx->_31 * vec->z + mx->_41;
	y = vec->x * mx->_12 + mx->_22 * vec->y + mx->_32 * vec->z + mx->_42;
	z = vec->x * mx->_13 + mx->_23 * vec->y + mx->_33 * vec->z + mx->_43;
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

void CreateFogPos()
{

}

long DistCompare()
{
	return 0;
}

void InitialiseFogBulbs()
{

}

void OmniEffect()
{

}

void OmniFog()
{

}

#pragma warning(push)
#pragma warning(disable : 4799)
void AddPrelitMMX(long prelight, D3DCOLOR* color)
{
	long c;

	c = color[0];

	__asm
	{
		xor eax, eax
		movd mm3, rgbmask
		movd mm4, prelight
		movd mm0, c
		paddusb mm0, mm4
		movd ecx, mm0
		movd mm1, rgb80h
		psubusb mm0, mm1
		movd mm2, eax
		movd mm5, zero
		punpcklbw mm5, mm0
		psrlw mm5, 9
		packuswb mm0, mm5
		psrlq mm0, 0x20
		movd ebx, mm0
		pcmpgtb mm0, mm2
		movd eax, mm0
		pandn mm0, mm3
		movd mm1, ecx
		pand mm1, mm0
		paddusb mm1, mm1
		movd ecx, mm1
		mov edx, color
		or ecx, eax
		mov[edx], ecx
		mov ecx, [edx + 4]
		and ebx, 0xFFFFFF
		or ecx, ebx
		mov[edx + 4], ecx
	}

	//caller does the emms
}

void AddPrelitMeshMMX(MESH_DATA* m, long p, D3DCOLOR* color)
{
	long prelight;
	long c;

	prelight = m->prelight[p];
	c = color[0];

	__asm
	{
		xor eax, eax
		movd mm3, rgbmask
		movd mm4, prelight
		movd mm0, c
		paddusb mm0, mm4
		movd ecx, mm0
		movd mm1, rgb80h
		psubusb mm0, mm1
		movd mm2, eax
		movd mm5, zero
		punpcklbw mm5, mm0
		psrlw mm5, 9
		packuswb mm0, mm5
		psrlq mm0, 0x20
		movd ebx, mm0
		pcmpgtb mm0, mm2
		movd eax, mm0
		pandn mm0, mm3
		movd mm1, ecx
		pand mm1, mm0
		paddusb mm1, mm1
		movd ecx, mm1
		mov edx, color
		or ecx, eax
		mov[edx], ecx
		mov ecx, [edx + 4]
		and ebx, 0xFFFFFF
		or ecx, ebx
		mov[edx + 4], ecx
	}

	//caller does the emms
}

void CalcColorSplitMMX(D3DCOLOR s, D3DCOLOR* d)
{
	__asm
	{
		xor eax, eax
		mov ecx, s
		movd mm3, rgbmask
		movd mm0, ecx
		movd mm1, rgb80h
		psubusb mm0, mm1
		movd mm2, eax
		movd mm5, zero
		punpcklbw mm5, mm0
		psrlw mm5, 9
		packuswb mm0, mm5
		psrlq mm0, 0x20
		movd ebx, mm0
		pcmpgtb mm0, mm2
		movd eax, mm0
		pandn mm0, mm3
		movd mm1, ecx
		pand mm1, mm0
		paddusb mm1, mm1
		movd ecx, mm1
		mov edx, d
		or ecx, eax
		mov[edx], ecx
		mov ecx, [edx + 4]
		and ebx, 0xFFFFFF
		or ecx, ebx
		mov[edx + 4], ecx
	}

	//caller does the emms

	d[0] &= 0xFFFFFF;
	d[0] |= GlobalAlpha;
}
#pragma warning(pop)

void S_DrawLine(long nVtx, D3DTLVERTEX* v)
{
	float zv;

	for (int i = 0; i < nVtx; i++)
	{
		zv = f_persp / v[i].sz;
		v[i].rhw = zv * f_oneopersp;
		v[i].sz = f_a - v[i].rhw * f_boo;
	}

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_TLVERTEX, v, nVtx, D3DDP_DONOTUPDATEEXTENTS);
}

void S_DrawTriFan(long nVtx, D3DTLVERTEX* v)
{
	float zv;

	for (int i = 0; i < nVtx; i++)
	{
		zv = f_persp / v[i].sz;
		v[i].rhw = zv * f_oneopersp;
		v[i].sz = f_a - v[i].rhw * f_boo;
	}

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[0].tex));
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, v, nVtx, D3DDP_DONOTUPDATEEXTENTS);
}

void inject_polyinsert(bool replace)
{
	INJECT(0x004B98E0, HWR_DrawSortList, replace);
	INJECT(0x004B8DB0, DrawSortList, replace);
	INJECT(0x004BD150, CalcColorSplit, replace);
	INJECT(0x004BA100, InitialiseSortList, replace);
	INJECT(0x004B9FB0, DoSort, replace);
	INJECT(0x004BA090, SortPolyList, replace);
	INJECT(0x004BA130, ClearFXFogBulbs, replace);
	INJECT(0x004BA150, ControlFXBulb, replace);
	INJECT(0x004BA170, CreateFXBulbs, replace);
	INJECT(0x004BA190, TriggerFXFogBulb, replace);
	INJECT(0x004BA1B0, IsVolumetric, replace);
	INJECT(0x004BA1D0, mD3DTransform, replace);
	INJECT(0x004BA260, CreateFogPos, replace);
	INJECT(0x004BA280, DistCompare, replace);
	INJECT(0x004BA2A0, InitialiseFogBulbs, replace);
	INJECT(0x004BA2C0, OmniEffect, replace);
	INJECT(0x004BA2E0, OmniFog, replace);
	INJECT(0x004BCFD0, AddPrelitMMX, replace);
	INJECT(0x004BD070, AddPrelitMeshMMX, replace);
	INJECT(0x004BCF30, CalcColorSplitMMX, replace);
	INJECT(0x004BD270, S_DrawLine, replace);
	INJECT(0x004BD300, S_DrawTriFan, replace);
}
