#include "../tomb5/pch.h"
#include "output.h"
#include "../specific/3dmath.h"
#include "../game/newinv2.h"
#include "../specific/d3dmatrix.h"
#include "lighting.h"
#include "function_table.h"
#include "../game/gameflow.h"
#include "../game/effects.h"
#include "specificfx.h"
#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#include "../game/draw.h"
#include "../game/health.h"
#include "../game/text.h"
#endif
#include "drawroom.h"
#include "function_stubs.h"
#include "time.h"
#include "dxshell.h"
#include "profiler.h"

void S_DrawPickup(short object_number)
{
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	SetD3DViewMatrix();
	aSetViewMatrix();
	DrawThreeDeeObject2D(long(phd_winxmax * 0.001953125 * 448.0 + PickupX), long(phd_winymax * 0.00390625 * 216.0), convert_obj_to_invobj(object_number),
		128, 0, (GnFrameCounter & 0x7F) << 9, 0, 0, 1);
}

void phd_PutPolygons(short* objptr, long clipstatus)
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
					tex.tpage = ushort(nTextures - 3);
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
					tex.tpage = ushort(nTextures - 3);
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

void phd_PutPolygonSkyMesh(short* objptr, long clipstatus)
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

void aTransformLightClipMesh(MESH_DATA* mesh)
{
	POINTLIGHT_STRUCT* point;
	SUNLIGHT_STRUCT* sun;
#ifdef GENERAL_FIXES
	SPOTLIGHT_STRUCT* spot;
#endif
	FOGBULB_STRUCT* bulb;
	FVECTOR vec;
	FVECTOR vec2;
	FVECTOR vec3;
	FVECTOR vec4;
	short* clip;
	float fR, fG, fB, val, val2, val3, zv, fCol, fCol2;
#ifdef GENERAL_FIXES
	static float DistanceFogStart, iDistanceFogStart;
	float fNum, fZ;
#endif
	long sR, sG, sB, cR, cG, cB;
	short clipFlag;

	clip = clipflags;

#ifdef GENERAL_FIXES
	DistanceFogStart = tomb5.distance_fog * 1024.0F;
	iDistanceFogStart = 1.0F / DistanceFogStart;
	fNum = iDistanceFogStart * 255.0F;
#endif

	for (int i = 0; i < mesh->nVerts; i++)
	{
		sR = 0;
		sG = 0;
		sB = 0;
		vec.x = (mesh->aVtx[i].x * D3DMView._11) + (mesh->aVtx[i].y * D3DMView._21) + (mesh->aVtx[i].z * D3DMView._31) + D3DMView._41;
		vec.y = (mesh->aVtx[i].x * D3DMView._12) + (mesh->aVtx[i].y * D3DMView._22) + (mesh->aVtx[i].z * D3DMView._32) + D3DMView._42;
		vec.z = (mesh->aVtx[i].x * D3DMView._13) + (mesh->aVtx[i].y * D3DMView._23) + (mesh->aVtx[i].z * D3DMView._33) + D3DMView._43;
#ifdef GENERAL_FIXES
		fZ = vec.z;
#endif

		if (TotalNumLights)
		{
			fR = (float)aAmbientR;
			fG = (float)aAmbientG;
			fB = (float)aAmbientB;

			if (NumPointLights)
			{
				for (int j = 0; j < NumPointLights; j++)
				{
					point = &PointLights[j];

#ifdef GENERAL_FIXES
					if (tomb5.tr4_point_lights)
						val = (point->vec.x * mesh->aVtx[i].nx + point->vec.y * mesh->aVtx[i].ny + point->vec.z * mesh->aVtx[i].nz);
					else
					{
						val = (point->vec.x * mesh->aVtx[i].nx + point->vec.y * mesh->aVtx[i].ny + point->vec.z * mesh->aVtx[i].nz + 1.0F) * 0.5F;
						val *= val;
					}
#else
					val = (point->vec.x * mesh->aVtx[i].nx + point->vec.y * mesh->aVtx[i].ny + point->vec.z * mesh->aVtx[i].nz + 1.0F) * 0.5F;
#endif

					if (val > 0)
					{
						val *= point->rad;
						fR += val * point->r;
						fG += val * point->g;
						fB += val * point->b;
					}
				}
			}

#ifdef GENERAL_FIXES
			if (NumSpotLights)
			{
				for (int j = 0; j < NumSpotLights; j++)
				{
					spot = &SpotLights[j];
					val = spot->vec.x * mesh->aVtx[i].nx + spot->vec.y * mesh->aVtx[i].ny + spot->vec.z * mesh->aVtx[i].nz;

					if (val > 0)
					{
						val *= spot->rad;
						fR += val * spot->r;
						fG += val * spot->g;
						fB += val * spot->b;
					}
				}
			}
#endif

			if (NumSunLights)
			{
				for (int j = 0; j < NumSunLights; j++)
				{
					sun = &SunLights[j];
					val = sun->vec.x * mesh->aVtx[i].nx + sun->vec.y * mesh->aVtx[i].ny + sun->vec.z * mesh->aVtx[i].nz;

					if (val > 0)
					{
#ifdef GENERAL_FIXES
						if (!InventoryActive)	//fucking shit
							val *= 0.75F;
						else
#endif
							val += val;

						fR += val * sun->r;
						fG += val * sun->g;
						fB += val * sun->b;
					}
				}
			}

			cR = (long)fR;
			cG = (long)fG;
			cB = (long)fB;
		}
		else
		{
			cR = aAmbientR;
			cG = aAmbientG;
			cB = aAmbientB;
		}

#ifdef GENERAL_FIXES
		if (fZ > DistanceFogStart)
		{
			val = fNum * (fZ - DistanceFogStart);
			cR -= (long)val;
			cG -= (long)val;
			cB -= (long)val;
		}
#endif

		if (cR - 128 <= 0)
			cR <<= 1;
		else
		{
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if (cG - 128 <= 0)
			cG <<= 1;
		else
		{
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if (cB - 128 <= 0)
			cB <<= 1;
		else
		{
			sB = (cB - 128) >> 1;
			cB = 255;
		}

		vec2.x = vec.x;
		vec2.y = vec.y;
		vec2.z = vec.z;
		aVertexBuffer[i].tu = vec.x;
		aVertexBuffer[i].tv = vec.y;
		clipFlag = 0;

		if (vec.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vec.z;
			vec.x = vec.x * zv + f_centerx;
			vec.y = vec.y * zv + f_centery;
			aVertexBuffer[i].rhw = f_moneopersp * zv;

			if (vec.x < clip_left)
				clipFlag++;
			else if (vec.x > clip_right)
				clipFlag += 2;

			if (vec.y < clip_top)
				clipFlag += 4;
			else if (vec.y > clip_bottom)
				clipFlag += 8;
		}

		clip[0] = clipFlag;
		clip++;

		aVertexBuffer[i].sx = vec.x;
		aVertexBuffer[i].sy = vec.y;
		aVertexBuffer[i].sz = vec.z;
		fCol2 = 0;

		if (NumFogBulbs)
		{
			for (int j = 0; j < NumFogBulbs; j++)
			{
				bulb = &FogBulbs[j];
				fCol = 0;

				if (bulb->rad + vec2.z > 0)
				{
					if (fabs(vec2.x) - bulb->rad < fabs(vec2.z) && fabs(vec2.y) - bulb->rad < fabs(vec2.z))
					{
						vec3.x = 0;
						vec3.y = 0;
						vec3.z = 0;
						vec4.x = 0;
						vec4.y = 0;
						vec4.z = 0;
						val = SQUARE(bulb->pos.x - vec2.x) + SQUARE(bulb->pos.y - vec2.y) + SQUARE(bulb->pos.z - vec2.z);

						if (bulb->sqlen >= bulb->sqrad)
						{
							if (val >= bulb->sqrad)
							{
								val = SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x);
								val2 = 1.0F / sqrt(val);
								vec3.x = val2 * vec2.x;
								vec3.y = val2 * vec2.y;
								vec3.z = val2 * vec2.z;
								val2 = bulb->pos.x * vec3.x + bulb->pos.y * vec3.y + bulb->pos.z * vec3.z;

								if (val2 > 0)
								{
									val3 = SQUARE(val2);

									if (val > val3)
									{
										val = bulb->sqlen - val3;

										if (val >= bulb->sqrad)
										{
											vec3.x = 0;
											vec3.y = 0;
											vec3.z = 0;
											vec4.x = 0;
											vec4.y = 0;
											vec4.z = 0;
										}
										else
										{
											val3 = sqrtf(bulb->sqrad - val);
											val = val2 - val3;
											vec4.x = val * vec3.x;
											vec4.y = val * vec3.y;
											vec4.z = val * vec3.z;
											val = val2 + val3;
											vec3.x *= val;
											vec3.y *= val;
											vec3.z *= val;
										}
									}
								}
							}
							else
							{
								vec4.z = vec2.z;
								vec4.x = vec2.x;
								vec4.y = vec2.y;
								val = 1.0F / sqrt(SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x));
								vec3.x = val * vec2.x;
								vec3.y = val * vec2.y;
								vec3.z = val * vec2.z;
								val2 = vec3.x * bulb->pos.x + vec3.y * bulb->pos.y + vec3.z * bulb->pos.z;
								val = val2 - sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
								vec3.x *= val;
								vec3.y *= val;
								vec3.z *= val;
							}
						}
						else if (val >= bulb->sqrad)
						{
							val = 1.0F / sqrt(SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x));
							vec4.x = val * vec2.x;
							vec4.y = val * vec2.y;
							vec4.z = val * vec2.z;
							val2 = vec4.x * bulb->pos.x + vec4.y * bulb->pos.y + vec4.z * bulb->pos.z;
							val = val2 + sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
							vec4.x *= val;
							vec4.y *= val;
							vec4.z *= val;
						}
						else
						{
							vec4.x = vec2.x;
							vec4.y = vec2.y;
							vec4.z = vec2.z;
						}

						fCol = sqrt(SQUARE((vec4.z - vec3.z)) + SQUARE((vec4.y - vec3.y)) + SQUARE((vec4.x - vec3.x))) * bulb->d;
					}
				}

				if (fCol)
				{
					fCol2 += fCol;
					sR += (long)(fCol * bulb->r);
					sG += (long)(fCol * bulb->g);
					sB += (long)(fCol * bulb->b);
				}
			}

			cR -= (long)fCol2;
			cG -= (long)fCol2;
			cB -= (long)fCol2;
		}

		if (sR > 255)
			sR = 255;
		else if (sR < 0)
			sR = 0;

		if (sG > 255)
			sG = 255;
		else if (sG < 0)
			sG = 0;

		if (sB > 255)
			sB = 255;
		else if (sB < 0)
			sB = 0;

		if (cR > 255)
			cR = 255;
		else if (cR < 0)
			cR = 0;

		if (cG > 255)
			cG = 255;
		else if (cG < 0)
			cG = 0;

		if (cB > 255)
			cB = 255;
		else if (cB < 0)
			cB = 0;

		aVertexBuffer[i].color = cB | GlobalAlpha | ((cG | (cR << 8)) << 8);
		aVertexBuffer[i].specular = sB | ((sG | ((sR | 0xFFFFFF00) << 8)) << 8);
	}
}

void phd_PutPolygonsPickup(short* objptr, float x, float y, long color)
{
	MESH_DATA* mesh;
	ENVUV* envuv;
	ACMESHVERTEX* vtx;
	TEXTURESTRUCT* pTex;
	TEXTURESTRUCT tex;
	short* quad;
	short* tri;
	float val, xbak, ybak;
	long clrbak[4];
	long spcbak[4];
	long num;
	ushort drawbak;
	bool envmap;

	old_lighting_water = 0;
	SetD3DViewMatrix();
	aSetViewMatrix();
	mesh = (MESH_DATA*)objptr;
	lGlobalMeshPos.x = mesh->bbox[3] - mesh->bbox[0];
	lGlobalMeshPos.y = mesh->bbox[4] - mesh->bbox[1];
	lGlobalMeshPos.z = mesh->bbox[5] - mesh->bbox[2];
	SuperResetLights();
	ClearDynamicLighting();
	ClearObjectLighting();
	clip_left = f_left;
	clip_top = f_top;
	clip_right = f_right;
	clip_bottom = f_bottom;
	SunLights[0].r = (float)CLRR(color);
	SunLights[0].g = (float)CLRG(color);
	SunLights[0].b = (float)CLRB(color);
	val = 1.0F / sqrt(12500.0F);
	SunLights[0].vec.x = (D3DMView._12 * -50.0F + D3DMView._13 * -100.0F) * val;	//x is 0
	SunLights[0].vec.y = (D3DMView._22 * -50.0F + D3DMView._23 * -100.0F) * val;
	SunLights[0].vec.z = (D3DMView._32 * -50.0F + D3DMView._33 * -100.0F) * val;
	NumSunLights = 1;
	TotalNumLights = 1;
	aAmbientR = 8;
	aAmbientG = 8;
	aAmbientB = 8;
	xbak = f_centerx;
	ybak = f_centery;
	f_centerx = x;
	f_centery = y;
	aTransformLightClipMesh(mesh);
	f_centerx = xbak;
	f_centery = ybak;

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
		envmap = 0;
		pTex = &textinfo[quad[4] & 0x7FFF];
		drawbak = pTex->drawtype;

		if (quad[5] & 1)
			pTex->drawtype = 2;

		if (quad[5] & 2)
		{
			envmap = 1;
			tex.drawtype = 2;
			tex.flag = pTex->flag;
			tex.tpage = ushort(nTextures - 3);
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
			pTex->drawtype = 0;
			AddQuadSorted(aVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
		}

		pTex->drawtype = drawbak;
	}

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
	{
		envmap = 0;
		pTex = &textinfo[tri[3] & 0x7FFF];
		drawbak = pTex->drawtype;

		if (tri[4] & 1)
			pTex->drawtype = 2;

		if (tri[4] & 2)
		{
			envmap = 1;
			tex.drawtype = 2;
			tex.flag = pTex->flag;
			tex.tpage = ushort(nTextures - 3);
			tex.u1 = aMappedEnvUV[*tri].u;
			tex.v1 = aMappedEnvUV[*tri].v;
			tex.u2 = aMappedEnvUV[tri[1]].u;
			tex.v2 = aMappedEnvUV[tri[1]].v;
			tex.u3 = aMappedEnvUV[tri[2]].u;
			tex.v3 = aMappedEnvUV[tri[2]].v;
			num = (tri[4] & 0x7C) << 1;
		}

		if (GlobalAlpha == 0xFF000000)
		{
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

void aTransformLightPrelightClipMesh(MESH_DATA* mesh)
{
	FOGBULB_STRUCT* bulb;
#ifdef GENERAL_FIXES
	DYNAMIC* light;
#endif
	FVECTOR vec;
	FVECTOR vec2;
	FVECTOR vec3;
	FVECTOR vec4;
	short* clip;
	float val, val2, val3, zv, fCol, fCol2;
#ifdef GENERAL_FIXES
	static float DistanceFogStart, iDistanceFogStart;
	float fNum, fZ;
#endif
	long sR, sG, sB, cR, cG, cB, pR, pG, pB;
	short clipFlag;

	clip = clipflags;
	pR = (StaticMeshShade & 0x1F) << 3;
	pG = ((StaticMeshShade >> 5) & 0x1F) << 3;
	pB = ((StaticMeshShade >> 10) & 0x1F) << 3;

#ifdef GENERAL_FIXES
	DistanceFogStart = tomb5.distance_fog * 1024.0F;
	iDistanceFogStart = 1.0F / DistanceFogStart;
	fNum = iDistanceFogStart * 255.0F;
#endif

	for (int i = 0; i < mesh->nVerts; i++)
	{
		sR = 0;
		sG = 0;
		sB = 0;
		vec.x = (mesh->aVtx[i].x * D3DMView._11) + (mesh->aVtx[i].y * D3DMView._21) + (mesh->aVtx[i].z * D3DMView._31) + D3DMView._41;
		vec.y = (mesh->aVtx[i].x * D3DMView._12) + (mesh->aVtx[i].y * D3DMView._22) + (mesh->aVtx[i].z * D3DMView._32) + D3DMView._42;
		vec.z = (mesh->aVtx[i].x * D3DMView._13) + (mesh->aVtx[i].y * D3DMView._23) + (mesh->aVtx[i].z * D3DMView._33) + D3DMView._43;
#ifdef GENERAL_FIXES
		fZ = vec.z;
#endif
		cR = CLRR(mesh->aVtx[i].prelight);
		cG = CLRG(mesh->aVtx[i].prelight);
		cB = CLRB(mesh->aVtx[i].prelight);
		cR = (cR * pR) >> 8;
		cG = (cG * pG) >> 8;
		cB = (cB * pB) >> 8;

#ifdef GENERAL_FIXES
		if (tomb5.static_lighting)
		{
			for (int j = 0; j < 32; j++)
			{
				light = &dynamics[j];

				if (light->on)
				{
					vec2.x = light->x - lGlobalMeshPos.x;
					vec2.y = light->y - lGlobalMeshPos.y;
					vec2.z = light->z - lGlobalMeshPos.z;
					vec3.x = (aLightMatrix._11 * vec2.x + aLightMatrix._12 * vec2.y + aLightMatrix._13 * vec2.z);
					vec3.y = (aLightMatrix._21 * vec2.x + aLightMatrix._22 * vec2.y + aLightMatrix._23 * vec2.z);
					vec3.z = (aLightMatrix._31 * vec2.x + aLightMatrix._32 * vec2.y + aLightMatrix._33 * vec2.z);
					val = sqrt(SQUARE(vec3.x - mesh->aVtx[i].x) + SQUARE(vec3.y - mesh->aVtx[i].y) + SQUARE(vec3.z - mesh->aVtx[i].z)) * 1.7F;

					if (val <= light->falloff)
					{
						val2 = (light->falloff - val) / light->falloff;
						cR += long(val2 * light->r);
						cG += long(val2 * light->g);
						cB += long(val2 * light->b);
					}
				}
			}
		}

		if (fZ > DistanceFogStart)
		{
			val = fNum * (fZ - DistanceFogStart);
			cR -= (long)val;
			cG -= (long)val;
			cB -= (long)val;
		}
#endif

		if (cR - 128 <= 0)
			cR <<= 1;
		else
		{
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if (cG - 128 <= 0)
			cG <<= 1;
		else
		{
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if (cB - 128 <= 0)
			cB <<= 1;
		else
		{
			sB = (cB - 128) >> 1;
			cB = 255;
		}

		vec2.x = vec.x;
		vec2.y = vec.y;
		vec2.z = vec.z;
		aVertexBuffer[i].tu = vec.x;
		aVertexBuffer[i].tv = vec.y;
		clipFlag = 0;

		if (vec.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vec.z;
			vec.x = vec.x * zv + f_centerx;
			vec.y = vec.y * zv + f_centery;
			aVertexBuffer[i].rhw = f_moneopersp * zv;

			if (vec.x < clip_left)
				clipFlag++;
			else if (vec.x > clip_right)
				clipFlag += 2;

			if (vec.y < clip_top)
				clipFlag += 4;
			else if (vec.y > clip_bottom)
				clipFlag += 8;
		}

		clip[0] = clipFlag;
		clip++;

		aVertexBuffer[i].sx = vec.x;
		aVertexBuffer[i].sy = vec.y;
		aVertexBuffer[i].sz = vec.z;
		fCol2 = 0;

		if (NumFogBulbs)
		{
			for (int j = 0; j < NumFogBulbs; j++)
			{
				bulb = &FogBulbs[j];
				fCol = 0;

				if (bulb->rad + vec2.z > 0)
				{
					if (fabs(vec2.x) - bulb->rad < fabs(vec2.z) && fabs(vec2.y) - bulb->rad < fabs(vec2.z))
					{
						vec3.x = 0;
						vec3.y = 0;
						vec3.z = 0;
						vec4.x = 0;
						vec4.y = 0;
						vec4.z = 0;
						val = SQUARE(bulb->pos.x - vec2.x) + SQUARE(bulb->pos.y - vec2.y) + SQUARE(bulb->pos.z - vec2.z);

						if (bulb->sqlen >= bulb->sqrad)
						{
							if (val >= bulb->sqrad)
							{
								val = SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x);
								val2 = 1.0F / sqrt(val);
								vec3.x = val2 * vec2.x;
								vec3.y = val2 * vec2.y;
								vec3.z = val2 * vec2.z;
								val2 = bulb->pos.x * vec3.x + bulb->pos.y * vec3.y + bulb->pos.z * vec3.z;

								if (val2 > 0)
								{
									val3 = SQUARE(val2);

									if (val > val3)
									{
										val = bulb->sqlen - val3;

										if (val >= bulb->sqrad)
										{
											vec3.x = 0;
											vec3.y = 0;
											vec3.z = 0;
											vec4.x = 0;
											vec4.y = 0;
											vec4.z = 0;
										}
										else
										{
											val3 = sqrtf(bulb->sqrad - val);
											val = val2 - val3;
											vec4.x = val * vec3.x;
											vec4.y = val * vec3.y;
											vec4.z = val * vec3.z;
											val = val2 + val3;
											vec3.x *= val;
											vec3.y *= val;
											vec3.z *= val;
										}
									}
								}
							}
							else
							{
								vec4.z = vec2.z;
								vec4.x = vec2.x;
								vec4.y = vec2.y;
								val = 1.0F / sqrt(SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x));
								vec3.x = val * vec2.x;
								vec3.y = val * vec2.y;
								vec3.z = val * vec2.z;
								val2 = vec3.x * bulb->pos.x + vec3.y * bulb->pos.y + vec3.z * bulb->pos.z;
								val = val2 - sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
								vec3.x *= val;
								vec3.y *= val;
								vec3.z *= val;
							}
						}
						else if (val >= bulb->sqrad)
						{
							val = 1.0F / sqrt(SQUARE(vec2.z) + SQUARE(vec2.y) + SQUARE(vec2.x));
							vec4.x = val * vec2.x;
							vec4.y = val * vec2.y;
							vec4.z = val * vec2.z;
							val2 = vec4.x * bulb->pos.x + vec4.y * bulb->pos.y + vec4.z * bulb->pos.z;
							val = val2 + sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
							vec4.x *= val;
							vec4.y *= val;
							vec4.z *= val;
						}
						else
						{
							vec4.x = vec2.x;
							vec4.y = vec2.y;
							vec4.z = vec2.z;
						}

						fCol = sqrt(SQUARE((vec4.z - vec3.z)) + SQUARE((vec4.y - vec3.y)) + SQUARE((vec4.x - vec3.x))) * bulb->d;
					}
				}

				if (fCol)
				{
					fCol2 += fCol;
					sR += (long)(fCol * bulb->r);
					sG += (long)(fCol * bulb->g);
					sB += (long)(fCol * bulb->b);
				}
			}

			cR -= (long)fCol2;
			cG -= (long)fCol2;
			cB -= (long)fCol2;
		}

		if (sR > 255)
			sR = 255;
		else if (sR < 0)
			sR = 0;

		if (sG > 255)
			sG = 255;
		else if (sG < 0)
			sG = 0;

		if (sB > 255)
			sB = 255;
		else if (sB < 0)
			sB = 0;

		if (cR > 255)
			cR = 255;
		else if (cR < 0)
			cR = 0;

		if (cG > 255)
			cG = 255;
		else if (cG < 0)
			cG = 0;

		if (cB > 255)
			cB = 255;
		else if (cB < 0)
			cB = 0;

		aVertexBuffer[i].color = cB | GlobalAlpha | ((cG | (cR << 8)) << 8);
		aVertexBuffer[i].specular = sB | ((sG | ((sR | 0xFFFFFF00) << 8)) << 8);
	}
}

void phd_PutPolygons_train(short* objptr, long x)
{
	phd_PutPolygons(objptr, x);
}

#ifdef GENERAL_FIXES
TR4LS tr4_load_screens[15] =
{
	{30548, 1770, 14103, 29452, 1576, 14853, 36},			//Title
	{58434, -634, 42783, 57337, -1048, 40945, 59},			//Streets of Rome
	{71226, 336, 32992, 71875, -568, 33761, 23},			//Trajan's Markets
	{37136, -111, 31451, 38734, 343, 33029, 47},			//Colosseum
	{46136, -6491, 27144, 46971, -6059, 28119, 95},			//The Base
	{57948, 3115, 47907, 56425, 3509, 50335, 49},			//Sub
	{22752, 14192, 63136, 24777, 13541, 61615, 11},			//Deepsea
	{61212, -1112, 58204, 59492, -582, 57087, 4},			//Sinking
	{63265, 8282, 62685, 63871, 8091, 64431, 113},			//Gallows
	{48924, 18279, 38112, 47348, 19063, 39496, 55},			//Labyrinth
	{36048, 9395, 41333, 36985, 8725, 42910, 101},			//Old Mill
	{34286, -5004, 44626, 34288, -3651, 44523, 16},			//13th
	{18204, -24322, 69690, 17000, -25322, 67556, 97},		//Iris
	{46959, -1147, 71996, 47934, -1109, 70446, 0},			//Security breach cut
	{51416, -2639, 47734, 51437, -2444, 36968, 119},		//Red Alert
};

static inline void GetLoadScreenCam()
{
	TR4LS* ls;

	ls = &tr4_load_screens[gfCurrentLevel];
	load_cam.x = ls->px;
	load_cam.y = ls->py;
	load_cam.z = ls->pz;
	load_target.x = ls->tx;
	load_target.y = ls->ty;
	load_target.z = ls->tz;
	load_roomnum = ls->rn;
}
#endif

void RenderLoadPic(long unused)
{
	short poisoned;

#ifdef GENERAL_FIXES
	GetLoadScreenCam();
#endif
	camera.pos.x = load_cam.x;
	camera.pos.y = load_cam.y;
	camera.pos.z = load_cam.z;
	lara_item->pos.x_pos = camera.pos.x;
	lara_item->pos.y_pos = camera.pos.y;
	lara_item->pos.z_pos = camera.pos.z;
	camera.target.x = load_target.x;
	camera.target.y = load_target.y;
	camera.target.z = load_target.z;
	camera.pos.room_number = load_roomnum;

	if (load_roomnum == 255)
		return;

	KillActiveBaddies((ITEM_INFO*)0xABCDEF);
	SetFade(255, 0);
	poisoned = lara.poisoned;
	FadeScreenHeight = 0;
	lara.poisoned = 0;
	GlobalFogOff = 1;
	BinocularRange = 0;

	if (App.dx.InScene)
		_EndScene();

#ifdef GENERAL_FIXES
	InitialisePickUpDisplay();

	do
	{
		phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
		S_InitialisePolyList();
		RenderIt(camera.pos.room_number);

		if (tomb5.loadingtxt && tomb5.tr4_loadbar)
			PrintString((ushort)phd_centerx, ushort((float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1)),
				5, SCRIPT_TEXT(STR_LOADING), FF_CENTER);

		S_OutputPolyList();
		S_DumpScreen();

	} while (DoFade != 2);

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	S_InitialisePolyList();
	RenderIt(camera.pos.room_number);

	if (tomb5.loadingtxt && tomb5.tr4_loadbar)
		PrintString((ushort)phd_centerx, ushort((float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1)),
			5, SCRIPT_TEXT(STR_LOADING), FF_CENTER);

	S_OutputPolyList();
	S_DumpScreen();
#endif

	lara.poisoned = poisoned;
	GlobalFogOff = 0;
}

long S_GetObjectBounds(short* bounds)
{
	FVECTOR vtx[8];
	float xMin, xMax, yMin, yMax, zMin, zMax, numZ, xv, yv, zv;

	if (phd_mxptr[11] >= phd_zfar && !outside)
		return 0;

	xMin = bounds[0];
	xMax = bounds[1];
	yMin = bounds[2];
	yMax = bounds[3];
	zMin = bounds[4];
	zMax = bounds[5];

	vtx[0].x = xMin;
	vtx[0].y = yMin;
	vtx[0].z = zMin;

	vtx[1].x = xMax;
	vtx[1].y = yMin;
	vtx[1].z = zMin;

	vtx[2].x = xMax;
	vtx[2].y = yMax;
	vtx[2].z = zMin;

	vtx[3].x = xMin;
	vtx[3].y = yMax;
	vtx[3].z = zMin;

	vtx[4].x = xMin;
	vtx[4].y = yMin;
	vtx[4].z = zMax;

	vtx[5].x = xMax;
	vtx[5].y = yMin;
	vtx[5].z = zMax;

	vtx[6].x = xMax;
	vtx[6].y = yMax;
	vtx[6].z = zMax;

	vtx[7].x = xMin;
	vtx[7].y = yMax;
	vtx[7].z = zMax;

	xMin = (float)0x3FFFFFFF;
	xMax = (float)-0x3FFFFFFF;
	yMin = (float)0x3FFFFFFF;
	yMax = (float)-0x3FFFFFFF;
	numZ = 0;

	for (int i = 0; i < 8; i++)
	{
		zv = vtx[i].x * phd_mxptr[M20] + vtx[i].y * phd_mxptr[M21] + vtx[i].z * phd_mxptr[M22] + phd_mxptr[M23];

		if (zv > phd_znear && phd_zfar > zv)
		{
			numZ++;
			zv /= phd_persp;

			if (!zv)
				zv = 1;

			zv = 1 / zv;
			xv = zv * (vtx[i].x * phd_mxptr[M00] + vtx[i].y * phd_mxptr[M01] + vtx[i].z * phd_mxptr[M02] + phd_mxptr[M03]);

			if (xv < xMin)
				xMin = xv;

			if (xv > xMax)
				xMax = xv;

			yv = zv * (vtx[i].x * phd_mxptr[M10] + vtx[i].y * phd_mxptr[M11] + vtx[i].z * phd_mxptr[M12] + phd_mxptr[M13]);

			if (yv < yMin)
				yMin = yv;

			if (yv > yMax)
				yMax = yv;
		}
	}

	xMin += phd_centerx;
	xMax += phd_centerx;
	yMin += phd_centery;
	yMax += phd_centery;

	if (numZ < 8 || xMin < 0 || yMin < 0 || phd_winxmax < xMax || phd_winymax < yMax)
		return -1;

	if (phd_right >= xMin && phd_bottom >= yMin && phd_left <= xMax && phd_top <= yMax)
		return 1;
	else
		return 0;
}

void S_AnimateTextures(long n)
{
	TEXTURESTRUCT* tex;
	TEXTURESTRUCT tex2;
	short* range;
	float voff;
	static long comp;
	short nRanges, nRangeFrames;

	for (comp += n; comp > 5; comp -= 5)
	{
		nRanges = *aranges;
		range = aranges + 1;

		for (int i = 0; i < nRanges; i++)
		{
			nRangeFrames = *range++;

			if (i < nAnimUVRanges && gfUVRotate)
			{
				while (nRangeFrames > 0)
				{
					range++;
					nRangeFrames--;
				}
			}
			else
			{
				tex2 = textinfo[*range];

				while (nRangeFrames > 0)
				{
					textinfo[range[0]] = textinfo[range[1]];
					range++;
					nRangeFrames--;
				}

				textinfo[*range] = tex2;
			}

			range++;
		}
	}

	if (gfUVRotate)
	{
		range = aranges + 1;
		AnimatingTexturesVOffset = (AnimatingTexturesVOffset - gfUVRotate * (n >> 1)) & 0x1F;

		for (int i = 0; i < nAnimUVRanges; i++)
		{
			nRangeFrames = *range++;

			while (nRangeFrames >= 0)
			{
				tex = &textinfo[range[0]];
				voff = AnimatingTexturesVOffset * (1.0F / 256.0F);
				tex->v1 = voff + AnimatingTexturesV[i][nRangeFrames][0];
				tex->v2 = voff + AnimatingTexturesV[i][nRangeFrames][0];
				tex->v3 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
				tex->v4 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
				range++;
				nRangeFrames--;
			}
		}
	}
}

long aCheckMeshClip(MESH_DATA* mesh)
{
	float* stash;
	short* checkBox;
	float left, right, top, bottom, front, back;
	float x, y, z, bx, by, bz, zv;
	long tooNear, offScreen;

	if (aGlobalSkinMesh)
		return 2;

	left = 10000.0F;
	right = -10000.0F;
	top = 10000.0F;
	bottom = -10000.0F;
	front = 10000.0F;
	back = -10000.0F;
	stash = aBoundingBox;
	checkBox = CheckClipBox;
	tooNear = 0;
	offScreen = 0;

	for (int i = 0; i < 8; i++)
	{
		bx = mesh->bbox[checkBox[0]];
		by = mesh->bbox[checkBox[1]];
		bz = mesh->bbox[checkBox[2]];
		checkBox += 3;

		x = bx * D3DMView._11 + by * D3DMView._21 + bz * D3DMView._31 + D3DMView._41;
		y = bx * D3DMView._12 + by * D3DMView._22 + bz * D3DMView._32 + D3DMView._42;
		z = bx * D3DMView._13 + by * D3DMView._23 + bz * D3DMView._33 + D3DMView._43;
		stash[0] = x;
		stash[1] = y;
		stash[2] = z;
		stash += 3;

		if (z < f_mznear)
		{
			tooNear++;
			z = f_mznear;
		}

		zv = f_mpersp / z;
		x = zv * x + f_centerx;
		y = zv * y + f_centery;

		if (x < left)
			left = x;

		if (x > right)
			right = x;

		if (y < top)
			top = y;

		if (y > bottom)
			bottom = y;

		if (z < front)
			front = z;

		if (z > back)
			back = z;

		if (left < f_left)
		{
			left = f_left;
			offScreen++;
		}

		if (right > f_right)
		{
			right = f_right;
			offScreen++;
		}

		if (top < f_top)
		{
			top = f_top;
			offScreen++;
		}

		if (bottom > f_bottom)
		{
			bottom = f_bottom;
			offScreen++;
		}
	}

	if (tooNear == 8 || left > f_right || right < f_left || top > f_bottom || bottom < f_top)
		return 0;

	if (tooNear || offScreen)
		return 1;

	return 2;
}

void ProjectTrainVerts(short nVerts, D3DTLVERTEX* v, short* clip, long x)
{
	float zv;
	short clipFlag;

	for (int i = 0; i < nVerts; i++)
	{
		clipFlag = 0;
		v->tu = v->sx;
		v->tv = v->sy;

		if (v->sz < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / v->sz;

			if (v->sz > FogEnd)
			{
				v->sz = f_zfar;
				clipFlag = 256;
			}

			v->sx = zv * v->sx + f_centerx;
			v->sy = zv * v->sy + f_centery;
			v->rhw = f_moneopersp * zv;

			if (v->sx < clip_left)
				clipFlag++;
			else if (v->sx > clip_right)
				clipFlag += 2;

			if (v->sy < clip_top)
				clipFlag += 4;
			else if (v->sy > clip_bottom)
				clipFlag += 8;
		}

		clip[0] = clipFlag;
		clip++;
		v++;
	}
}

HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, long x, long y, long dx, long dy)
{
	HDC hdc;
	HDC hdc2;
	BITMAP bitmap;
	DDSURFACEDESC2 desc;
	HRESULT result;
	long l, t;

	if (!hbm || !surf)
		return E_FAIL;

	surf->Restore();
	hdc = CreateCompatibleDC(0);

	if (!hdc)
		OutputDebugString("createcompatible dc failed\n");

	SelectObject(hdc, hbm);
	GetObject(hbm, sizeof(BITMAP), &bitmap);

	if (!dx)
		dx = bitmap.bmWidth;

	if (!dy)
		dy = bitmap.bmHeight;

	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
	surf->GetSurfaceDesc(&desc);
	l = 0;
	t = 0;

	if (!(App.dx.Flags & 0x80))
	{
		surf = App.dx.lpPrimaryBuffer;

		if (App.dx.Flags & 2)
		{
			l = App.dx.rScreen.left;
			t = App.dx.rScreen.top;
		}
	}

	result = surf->GetDC(&hdc2);

	if (!result)
	{
		StretchBlt(hdc2, l, t, desc.dwWidth, desc.dwHeight, hdc, x, y, dx, dy, SRCCOPY);
		surf->ReleaseDC(hdc2);
	}

	DeleteDC(hdc);
	return result;
}

HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name)
{
	HBITMAP hBitmap;
	HRESULT result;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (!hBitmap)
		hBitmap = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (!hBitmap)
	{
		OutputDebugString("handle is null\n");
		return E_FAIL;
	}

	result = DDCopyBitmap(surf, hBitmap, 0, 0, 0, 0);

	if (result != DD_OK)
		OutputDebugString("ddcopybitmap failed\n");

	DeleteObject(hBitmap);
	return result;
}

HRESULT aLoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name)
{
	HBITMAP hBitmap;
	HRESULT result;

	hBitmap = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (hBitmap)
	{
		result = DDCopyBitmap(surf, hBitmap, 0, 0, 0, 0);
		DeleteObject(hBitmap);

		if (result == DD_OK)
			return result;
	}

	Log(1, "LoadBitmap FAILED");
	return E_FAIL;
}

void do_boot_screen(long language)
{
	Log(2, "do_boot_screen");

	switch (language)
	{
	case ENGLISH:
	case DUTCH:
		_LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
		break;

	case FRENCH:
		_LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
		break;

	case GERMAN:
		_LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
		break;

	case ITALIAN:
		_LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
		break;

	case SPANISH:
		_LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
		break;

	case US:
		_LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
		break;

	case JAPAN:
		_LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
		S_DumpScreen();
		_LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
		break;
	}
}

void aCalcColorSplit(long col, long* pC, long* pS)
{
	long cR, cG, cB, sR, sG, sB;

	sR = 0;
	sG = 0;
	sB = 0;
	cR = CLRR(col);
	cG = CLRG(col);
	cB = CLRB(col);

	if (cR - 128 <= 0)
		cR <<= 1;
	else
	{
		sR = (cR - 128) >> 1;
		cR = 255;
	}

	if (cG - 128 <= 0)
		cG <<= 1;
	else
	{
		sG = (cG - 128) >> 1;
		cG = 255;
	}

	if (cB - 128 <= 0)
		cB <<= 1;
	else
	{
		sB = (cB - 128) >> 1;
		cB = 255;
	}

	if (cR < 0)
		cR = 0;
	else if (cR > 255)
		cR = 255;

	if (cG < 0)
		cG = 0;
	else if (cG > 255)
		cG = 255;

	if (cB < 0)
		cB = 0;
	else if (cB > 255)
		cB = 255;

	if (sR < 0)
		sR = 0;
	else if (sR > 255)
		sR = 255;

	if (sG < 0)
		sG = 0;
	else if (sG > 255)
		sG = 255;

	if (sB < 0)
		sB = 0;
	else if (sB > 255)
		sB = 255;

	*pC = RGBONLY(cR, cG, cB);
	*pS = RGBONLY(sR, sG, sB);
}

long S_DumpScreen()
{
	long n;

	mAddProfilerEvent(0);
	mDrawProfiler(0, 64, App.dx.dwRenderHeight - 32);
	n = Sync();

	while (n < 2)
	{
		while (!Sync());	//wait for sync
		n++;
	}

	GnFrameCounter++;
	_EndScene();
	DXShowFrame();
	mAddProfilerEvent(0x805C805C);
	App.dx.DoneBlit = 1;
	return n;
}

void inject_output(bool replace)
{
	INJECT(0x004B78D0, S_DrawPickup, replace);
	INJECT(0x004B3F00, phd_PutPolygons, replace);
	INJECT(0x004B74D0, phd_PutPolygonSkyMesh, replace);
	INJECT(0x004B2BA0, aTransformLightClipMesh, replace);
	INJECT(0x004B66B0, phd_PutPolygonsPickup, replace);
	INJECT(0x004B35F0, aTransformLightPrelightClipMesh, replace);
	INJECT(0x004B74A0, phd_PutPolygons_train, replace);
	INJECT(0x004B8660, RenderLoadPic, replace);
	INJECT(0x004B7EB0, S_GetObjectBounds, replace);
	INJECT(0x004B8310, S_AnimateTextures, replace);
	INJECT(0x004B2800, aCheckMeshClip, replace);
	INJECT(0x004B8530, ProjectTrainVerts, replace);
	INJECT(0x004B8780, DDCopyBitmap, replace);
	INJECT(0x004B8930, _LoadBitmap, replace);
	INJECT(0x004B89F0, aLoadBitmap, replace);
	INJECT(0x004B8A80, do_boot_screen, replace);
	INJECT(0x004B8C50, aCalcColorSplit, replace);
	INJECT(0x004B7DA0, S_DumpScreen, replace);
}

