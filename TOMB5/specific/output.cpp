#include "../tomb5/pch.h"
#include "output.h"
#include "../specific/3dmath.h"
#include "../game/newinv2.h"
#include "../specific/d3dmatrix.h"
#include "lighting.h"
#include "function_table.h"
#include "../game/gameflow.h"

void S_DrawPickup(short object_number)
{
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	SetD3DViewMatrix();
	aSetViewMatrix();
	DrawThreeDeeObject2D((int)(phd_winxmax * 0.001953125 * 448.0 + PickupX), (int)(phd_winymax * 0.00390625 * 216.0), convert_obj_to_invobj(object_number),
		128, 0, (GnFrameCounter & 0x7F) << 9, 0, 0, 1);
}

void phd_PutPolygons(short* objptr, int clipstatus)
{
	MESH_DATA* p;
	short* f;
	D3DTLVERTEX* tv;
	ACMESHVERTEX* pVs;
	TEXTURESTRUCT* pTex;
	TEXTURESTRUCT EnvTex;
	LPENVUV pUV;
	LPD3DMATRIX pM;
	int envmul, drawbak;
	long c1, c2, c3, c4, s1, s2, s3, s4;
	bool bEnvMap;

	pM = &D3DMView;
	aSetViewMatrix();

	if (!objptr)
		return;

	clip_left = f_left;
	clip_right = f_right;
	clip_top = f_top;
	clip_bottom = f_bottom;
	
	p = (MESH_DATA*)objptr;


	if (!aCheckMeshClip(p))
		return;

	lGlobalMeshPos.vx = p->bbox[3] - p->bbox[0];
	lGlobalMeshPos.vy = p->bbox[4] - p->bbox[1];
	lGlobalMeshPos.vz = p->bbox[5] - p->bbox[2];
	SuperResetLights();
	
	if (GlobalAmbient)
	{
		ClearObjectLighting();
		ClearDynamicLighting();
		App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, GlobalAmbient);
		aAmbientR = ((GlobalAmbient >> 16) & 0xFF);
		aAmbientG = ((GlobalAmbient >> 8) & 0xFF);
		aAmbientB = GlobalAmbient & 0xFF;
		GlobalAmbient = 0;
	}
	else
	{
		if (p->prelight)
		{
			ClearObjectLighting();
			InitDynamicLighting(current_item);
		}
		else
			InitObjectLighting(current_item);

		InitObjectFogBulbs();
	}

	if (p->aFlags & 2)
		aTransformLightPrelightClipMesh(p);
	else
		aTransformLightClipMesh(p);

	if (p->aFlags & 1)
	{
		pVs = p->aVtx;

		pUV = aMappedEnvUV;

		for (int i = 0; i < p->nVerts; i++)
		{
			pUV->u = 0.75F + (((pVs->nx * pM->_11) + (pVs->ny * pM->_21) + (pVs->nz * pM->_31)) * 0.25F);
			pUV->v = 0.25F + (((pVs->nx * pM->_12) + (pVs->ny * pM->_22) + (pVs->nz * pM->_32)) * 0.25F);
			pVs++;
			pUV++;
		}
	}

	tv = aVertexBuffer;
	f = p->gt4;
	pUV = aMappedEnvUV;

	for (int n = 0; n < p->ngt4; n++, f += 6)
	{
		pTex = textinfo + (f[4] & 0x7FFF);
		bEnvMap = 0;
		drawbak = pTex->drawtype;

		if (f[5] & 1)
			pTex->drawtype = 2;

		if (f[5] & 2)
		{
			EnvTex.drawtype = 2;
			EnvTex.flag = pTex->flag;
			EnvTex.tpage = nTextures - 3;
			EnvTex.u1 = (pUV + f[0])->u;
			EnvTex.v1 = (pUV + f[0])->v;
			EnvTex.u2 = (pUV + f[1])->u;
			EnvTex.v2 = (pUV + f[1])->v;
			EnvTex.u3 = (pUV + f[2])->u;
			EnvTex.v3 = (pUV + f[2])->v;
			EnvTex.u4 = (pUV + f[3])->u;
			EnvTex.v4 = (pUV + f[3])->v;
			envmul = ((f[5] >> 2) & 0x1F) << 3;
			bEnvMap = 1;
		}

		if (GlobalAlpha == 0xFF000000)
		{
			switch (pTex->drawtype)
			{
			case 0:
				AddQuadZBuffer(tv, f[0], f[1], f[2], f[3], pTex, 0);
				break;

			case 1:
			case 2:
				AddQuadSorted(tv, f[0], f[1], f[2], f[3], pTex, 0);
				break;
			}

			if (bEnvMap)
			{
				c1 = tv[f[0]].color;
				c2 = tv[f[1]].color;
				c3 = tv[f[2]].color;
				c4 = tv[f[3]].color;
				s1 = tv[f[0]].specular;
				s2 = tv[f[1]].specular;
				s3 = tv[f[2]].specular;
				s4 = tv[f[3]].specular;

				RGB_M(tv[f[0]].color, envmul);
				RGB_M(tv[f[1]].color, envmul);
				RGB_M(tv[f[2]].color, envmul);
				RGB_M(tv[f[3]].color, envmul);
				RGB_M(tv[f[0]].specular, envmul);
				RGB_M(tv[f[1]].specular, envmul);
				RGB_M(tv[f[2]].specular, envmul);
				RGB_M(tv[f[3]].specular, envmul);

				AddQuadSorted(tv, f[0], f[1], f[2], f[3], &EnvTex, 0);
				tv[f[0]].color = c1;
				tv[f[1]].color = c2;
				tv[f[2]].color = c3;
				tv[f[3]].color = c4;
				tv[f[0]].specular = s1;
				tv[f[1]].specular = s2;
				tv[f[2]].specular = s3;
				tv[f[3]].specular = s4;
			}
		}
		else
		{
			pTex->drawtype = 7;
			AddQuadSorted(tv, f[0], f[1], f[2], f[3], pTex, 0);
		}

		pTex->drawtype = drawbak;
	}

	f = p->gt3;

	for (int n = 0; n < p->ngt3; n++, f += 5)
	{
		pTex = textinfo + (f[3] & 0x7FFF);
		bEnvMap = 0;
		drawbak = pTex->drawtype;

		if (f[4] & 1)
			pTex->drawtype = 2;

		if (f[4] & 2)
		{
			envmul = ((f[4] >> 2) & 0x1F) << 3;
			bEnvMap = 1;
			EnvTex.drawtype = 2;
			EnvTex.flag = pTex->flag;
			EnvTex.tpage = nTextures - 3;
			EnvTex.u1 = (pUV + f[0])->u;
			EnvTex.v1 = (pUV + f[0])->v;
			EnvTex.u2 = (pUV + f[1])->u;
			EnvTex.v2 = (pUV + f[1])->v;
			EnvTex.u3 = (pUV + f[2])->u;
			EnvTex.v3 = (pUV + f[2])->v;
		}

		if (GlobalAlpha == 0xFF000000)
		{
			switch (pTex->drawtype)
			{
			case 0:
				AddTriZBuffer(tv, f[0], f[1], f[2], pTex, 0);
				break;

			case 1:
			case 2:
				AddTriSorted(tv, f[0], f[1], f[2], pTex, 0);
				break;
			}

			if (bEnvMap)
			{
				c1 = tv[f[0]].color;
				c2 = tv[f[1]].color;
				c3 = tv[f[2]].color;
				s1 = tv[f[0]].specular;
				s2 = tv[f[1]].specular;
				s3 = tv[f[2]].specular;

				RGB_M(tv[f[0]].color, envmul);
				RGB_M(tv[f[1]].color, envmul);
				RGB_M(tv[f[2]].color, envmul);
				RGB_M(tv[f[0]].specular, envmul);
				RGB_M(tv[f[1]].specular, envmul);
				RGB_M(tv[f[2]].specular, envmul);

				AddTriSorted(tv, f[0], f[1], f[2], &EnvTex, 0);
				tv[f[0]].color = c1;
				tv[f[1]].color = c2;
				tv[f[2]].color = c3;
				tv[f[0]].specular = s1;
				tv[f[1]].specular = s2;
				tv[f[2]].specular = s3;
			}
		}
		else
		{
			pTex->drawtype = 7;
			AddTriSorted(tv, f[0], f[1], f[2], pTex, 0);
		}

		pTex->drawtype = drawbak;
	}
}

void phd_PutPolygonSkyMesh(short* objptr, int clipstatus)
{
	TEXTURESTRUCT* pTex;
	D3DTLVERTEX* tv;
	MESH_DATA* p;
	short* f;
	ushort flag;

	p = (MESH_DATA*)objptr;
	aSetViewMatrix();
	SuperResetLights();
	ClearDynamicLighting();
	ClearObjectLighting();
	aAmbientR = 128;
	aAmbientG = 128;
	aAmbientB = 128;
	clip_top = f_top;
	clip_bottom = f_bottom;
	clip_left = f_left;
	clip_right = f_right;
	aTransformLightClipMesh(p);
	tv = aVertexBuffer;
	f = p->gt4;
	
	for (int i = 0; i < p->ngt4; i++, f += 6)
	{
		pTex = &textinfo[f[4] & 0x7FFF];
		flag = pTex->drawtype;

		if (f[5] & 1)
		{
			if (gfLevelFlags & GF_HORIZONCOLADD)
				pTex->drawtype = 2;
			else
			{
				if (App.dx.lpZBuffer)
				{
					tv[f[0]].color = 0;
					tv[f[1]].color = 0;
					tv[f[2]].color = 0xFF000000;
					tv[f[3]].color = 0xFF000000;
					pTex->drawtype = 3;
				}
				else
				{
					tv[f[0]].color = 0;
					tv[f[1]].color = 0;
					tv[f[2]].color = 0;
					tv[f[3]].color = 0;
					pTex->drawtype = 0;
				}
			}
		}
		else
			pTex->drawtype = 4;

		tv[f[0]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		tv[f[1]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		tv[f[2]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		tv[f[3]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		AddQuadSorted(tv, f[0], f[1], f[2], f[3], pTex, 0);
		pTex->drawtype = flag;
	}

	f = p->gt3;

	for (int i = 0; i < p->ngt3; i++, f += 5)
	{
		pTex = &textinfo[f[3] & 0x7FFF];
		flag = pTex->drawtype;
		pTex->drawtype = 4;
		AddTriSorted(tv, f[0], f[1], f[2], pTex, 0);
		pTex->drawtype = flag;
	}
}

void inject_output(bool replace)
{
	INJECT(0x004B78D0, S_DrawPickup, replace);
	INJECT(0x004B3F00, phd_PutPolygons, replace);
	INJECT(0x004B74D0, phd_PutPolygonSkyMesh, replace);
}

