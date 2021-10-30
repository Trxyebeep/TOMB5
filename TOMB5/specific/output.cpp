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
	MESH_DATA* mesh;
	ENVUV* envuv;
	ACMESHVERTEX* vtx;
	TEXTURESTRUCT* pTex;
	TEXTURESTRUCT tex;
	short* quad;
	short* tri;
	long clrbak[4];
	long spcbak[4];
	long num;
	ushort drawbak;
	bool envmap;

	aSetViewMatrix();
	mesh = (MESH_DATA*)objptr;

	if (objptr)
	{
		clip_left = f_left;
		clip_top = f_top;
		clip_right = f_right;
		clip_bottom = f_bottom;

		if (aCheckMeshClip(mesh))
		{
			lGlobalMeshPos.x = mesh->bbox[3] - mesh->bbox[0];
			lGlobalMeshPos.y = mesh->bbox[4] - mesh->bbox[1];
			lGlobalMeshPos.z = mesh->bbox[5] - mesh->bbox[2];
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
				if (mesh->prelight)
				{
					ClearObjectLighting();
					InitDynamicLighting(current_item);
				}
				else
					InitObjectLighting(current_item);

				InitObjectFogBulbs();
			}

			if (mesh->aFlags & 2)
				aTransformLightPrelightClipMesh(mesh);
			else
				aTransformLightClipMesh(mesh);

			if (mesh->aFlags & 1)
			{
				envuv = aMappedEnvUV;
				vtx = mesh->aVtx;

				for (int i = 0; i < mesh->nVerts; i++, envuv++, vtx++)
				{
					envuv->u = (vtx->nx * D3DMView._11 + vtx->ny * D3DMView._21 + vtx->nz * D3DMView._31) * 0.25F + 0.25F;
					envuv->v = (vtx->nx * D3DMView._12 + vtx->ny * D3DMView._22 + vtx->nz * D3DMView._32) * 0.25F + 0.25F;
				}
			}

			quad = mesh->gt4;

			for (int i = 0; i < mesh->ngt4; i++, quad += 6)
			{
				pTex = &textinfo[quad[4] & 0x7FFF];
				envmap = 0;
				drawbak = pTex->drawtype;

				if (quad[5] & 1)
					pTex->drawtype = 2;

				if (quad[5] & 2)
				{
					envmap = 1;
					tex.drawtype = 2;
					tex.flag = pTex->flag;
					tex.tpage = nTextures - 3;
					tex.u1 = aMappedEnvUV[quad[0]].u;
					tex.v1 = aMappedEnvUV[quad[0]].v;
					tex.u2 = aMappedEnvUV[quad[1]].u;
					tex.v2 = aMappedEnvUV[quad[1]].v;
					tex.u3 = aMappedEnvUV[quad[2]].u;
					tex.v3 = aMappedEnvUV[quad[2]].v;
					tex.u4 = aMappedEnvUV[quad[3]].u;
					tex.v4 = aMappedEnvUV[quad[3]].v;
					num = (quad[5] & 0x7C) << 1;
				}

				if (GlobalAlpha == 0xFF000000)
				{
					if (!pTex->drawtype)
						AddQuadZBuffer(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
					else if (pTex->drawtype <= 2)
						AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);

					if (envmap)
					{
						clrbak[0] = aVertexBuffer[quad[0]].color;
						clrbak[1] = aVertexBuffer[quad[1]].color;
						clrbak[2] = aVertexBuffer[quad[2]].color;
						clrbak[3] = aVertexBuffer[quad[3]].color;
						spcbak[0] = aVertexBuffer[quad[0]].specular;
						spcbak[1] = aVertexBuffer[quad[1]].specular;
						spcbak[2] = aVertexBuffer[quad[2]].specular;
						spcbak[3] = aVertexBuffer[quad[3]].specular;
						RGB_M(aVertexBuffer[quad[0]].color, num);
						RGB_M(aVertexBuffer[quad[1]].color, num);
						RGB_M(aVertexBuffer[quad[2]].color, num);
						RGB_M(aVertexBuffer[quad[3]].color, num);
						RGB_M(aVertexBuffer[quad[0]].specular, num);
						RGB_M(aVertexBuffer[quad[1]].specular, num);
						RGB_M(aVertexBuffer[quad[2]].specular, num);
						RGB_M(aVertexBuffer[quad[3]].specular, num);
						AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], &tex, 0);
						aVertexBuffer[quad[0]].color = clrbak[0];
						aVertexBuffer[quad[1]].color = clrbak[1];
						aVertexBuffer[quad[2]].color = clrbak[2];
						aVertexBuffer[quad[3]].color = clrbak[3];
						aVertexBuffer[quad[0]].specular = spcbak[0];
						aVertexBuffer[quad[1]].specular = spcbak[1];
						aVertexBuffer[quad[2]].specular = spcbak[2];
						aVertexBuffer[quad[3]].specular = spcbak[3];
					}
				}
				else
				{
					pTex->drawtype = 7;
					AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
				}

				pTex->drawtype = drawbak;
			}

			tri = mesh->gt3;

			for (int i = 0; i < mesh->ngt3; i++, tri += 5)
			{
				pTex = &textinfo[tri[3] & 0x7FFF];
				envmap = 0;
				drawbak = pTex->drawtype;

				if (tri[4] & 1)
					pTex->drawtype = 2;

				if (tri[4] & 2)
				{
					envmap = 1;
					tex.drawtype = 2;
					tex.flag = pTex->flag;
					tex.tpage = nTextures - 3;
					tex.u1 = aMappedEnvUV[tri[0]].u;
					tex.v1 = aMappedEnvUV[tri[0]].v;
					tex.u2 = aMappedEnvUV[tri[1]].u;
					tex.v2 = aMappedEnvUV[tri[1]].v;
					tex.u3 = aMappedEnvUV[tri[2]].u;
					tex.v3 = aMappedEnvUV[tri[2]].v;
					num = (tri[4] & 0x7C) << 1;
				}

				if (GlobalAlpha == 0xFF000000)
				{
					if (!pTex->drawtype)
						AddTriZBuffer(aVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
					else if (pTex->drawtype <= 2)
						AddTriSorted(aVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);

					if (envmap)
					{
						clrbak[0] = aVertexBuffer[tri[0]].color;
						clrbak[1] = aVertexBuffer[tri[1]].color;
						clrbak[2] = aVertexBuffer[tri[2]].color;
						spcbak[0] = aVertexBuffer[tri[0]].specular;
						spcbak[1] = aVertexBuffer[tri[1]].specular;
						spcbak[2] = aVertexBuffer[tri[2]].specular;
						RGB_M(aVertexBuffer[tri[0]].color, num);
						RGB_M(aVertexBuffer[tri[1]].color, num);
						RGB_M(aVertexBuffer[tri[2]].color, num);
						RGB_M(aVertexBuffer[tri[0]].specular, num);
						RGB_M(aVertexBuffer[tri[1]].specular, num);
						RGB_M(aVertexBuffer[tri[2]].specular, num);
						AddTriSorted(aVertexBuffer, tri[0], tri[1], tri[2], &tex, 0);
						aVertexBuffer[tri[0]].color = clrbak[0];
						aVertexBuffer[tri[1]].color = clrbak[1];
						aVertexBuffer[tri[2]].color = clrbak[2];
						aVertexBuffer[tri[0]].specular = spcbak[0];
						aVertexBuffer[tri[1]].specular = spcbak[1];
						aVertexBuffer[tri[2]].specular = spcbak[2];
					}
				}
				else
				{
					pTex->drawtype = 7;
					AddTriSorted(aVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
				}

				pTex->drawtype = drawbak;
			}
		}
	}
}

void phd_PutPolygonSkyMesh(short* objptr, int clipstatus)
{
	TEXTURESTRUCT* pTex;
	MESH_DATA* mesh;
	short* quad;
	short* tri;
	static float num;
	ushort drawbak;

	num = 0;
	mesh = (MESH_DATA*)objptr;
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
	aTransformLightClipMesh(mesh);
	quad = mesh->gt4;
	
	for (int i = 0; i < mesh->ngt4; i++, quad += 6)
	{
		pTex = &textinfo[quad[4] & 0x7FFF];
		drawbak = pTex->drawtype;

		if (quad[5] & 1)
		{
			if (gfLevelFlags & GF_HORIZONCOLADD)
				pTex->drawtype = 2;
			else
			{
				if (App.dx.lpZBuffer)
				{
					aVertexBuffer[quad[0]].color = 0;
					aVertexBuffer[quad[1]].color = 0;
					aVertexBuffer[quad[2]].color = 0xFF000000;
					aVertexBuffer[quad[3]].color = 0xFF000000;
					pTex->drawtype = 3;
				}
				else
				{
					aVertexBuffer[quad[0]].color = 0;
					aVertexBuffer[quad[1]].color = 0;
					aVertexBuffer[quad[2]].color = 0;
					aVertexBuffer[quad[3]].color = 0;
					pTex->drawtype = 0;
				}
			}
		}
		else
			pTex->drawtype = 4;

		aVertexBuffer[quad[0]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		aVertexBuffer[quad[1]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		aVertexBuffer[quad[2]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		aVertexBuffer[quad[3]].rhw = (f_mpersp / f_mzfar) * f_moneopersp;
		AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);

		if (aVertexBuffer[quad[0]].sy > num)
			num = aVertexBuffer[quad[0]].sy;

		if (aVertexBuffer[quad[1]].sy > num)
			num = aVertexBuffer[quad[1]].sy;

		if (aVertexBuffer[quad[2]].sy > num)
			num = aVertexBuffer[quad[2]].sy;

		if (aVertexBuffer[quad[3]].sy > num)
			num = aVertexBuffer[quad[3]].sy;

		pTex->drawtype = drawbak;
	}

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
	{
		pTex = &textinfo[tri[3] & 0x7FFF];
		drawbak = pTex->drawtype;
		pTex->drawtype = 4;
		AddTriSorted(aVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
		pTex->drawtype = drawbak;
	}

	num = (float)phd_centery;
}

void inject_output(bool replace)
{
	INJECT(0x004B78D0, S_DrawPickup, replace);
	INJECT(0x004B3F00, phd_PutPolygons, replace);
	INJECT(0x004B74D0, phd_PutPolygonSkyMesh, replace);
}

