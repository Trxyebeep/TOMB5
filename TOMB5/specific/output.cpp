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

void aTransformLightClipMesh(MESH_DATA* mesh)
{
	POINTLIGHT_STRUCT* point;
	SUNLIGHT_STRUCT* sun;
	FOGBULB_STRUCT* bulb;
	FVECTOR vec;
	FVECTOR vec2;
	FVECTOR vec3;
	FVECTOR vec4;
	short* clip;
	float fR, fG, fB, val, val2, val3, zv, fCol, fCol2;
	long sR, sG, sB, cR, cG, cB;
	short clip_distance;

	clip = clipflags;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		sR = 0;
		sG = 0;
		sB = 0;
		vec.x = (mesh->aVtx[i].x * D3DMView._11) + (mesh->aVtx[i].y * D3DMView._21) + (mesh->aVtx[i].z * D3DMView._31) + D3DMView._41;
		vec.y = (mesh->aVtx[i].x * D3DMView._12) + (mesh->aVtx[i].y * D3DMView._22) + (mesh->aVtx[i].z * D3DMView._32) + D3DMView._42;
		vec.z = (mesh->aVtx[i].x * D3DMView._13) + (mesh->aVtx[i].y * D3DMView._23) + (mesh->aVtx[i].z * D3DMView._33) + D3DMView._43;

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
					val = (point->vec.x * mesh->aVtx[i].nx + point->vec.y * mesh->aVtx[i].ny + point->vec.z * mesh->aVtx[i].nz + 1.0F) * 0.5F;

					if (val > 0)
					{
						val *= point->rad;
						fR += val * point->r;
						fG += val * point->g;
						fB += val * point->b;
					}
				}
			}

			if (NumSunLights)
			{
				for (int j = 0; j < NumSunLights; j++)
				{
					sun = &SunLights[j];
					val = sun->vec.x * mesh->aVtx[i].nx + sun->vec.y * mesh->aVtx[i].ny + sun->vec.z * mesh->aVtx[i].nz;

					if (val > 0)
					{
#ifndef GENERAL_FIXES
						val += val;
#endif

						fR += val * sun->r;
						fG += val * sun->g;
						fB += val * sun->b;
					}
				}
			}

#ifdef  GENERAL_FIXES
			//spotlights here, just do the same as the others, val = etc. then add it to fR fG fB
			//and shadows??
#endif

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
		clip_distance = 0;

		if (vec.z < f_mznear)
			clip_distance = -128;
		else
		{
			zv = f_mpersp / vec.z;
			vec.x = vec.x * zv + f_centerx;
			vec.y = vec.y * zv + f_centery;
			aVertexBuffer[i].rhw = f_moneopersp * zv;

			if (vec.x < clip_left)
				clip_distance = 1;
			else if (vec.x > clip_right)
				clip_distance = 2;

			if (vec.y < clip_top)
				clip_distance += 4;
			else if (vec.y > clip_bottom)
				clip_distance += 8;
		}

		clip[0] = clip_distance;
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

void inject_output(bool replace)
{
	INJECT(0x004B78D0, S_DrawPickup, replace);
	INJECT(0x004B3F00, phd_PutPolygons, replace);
	INJECT(0x004B74D0, phd_PutPolygonSkyMesh, replace);
	INJECT(0x004B2BA0, aTransformLightClipMesh, replace);
}

