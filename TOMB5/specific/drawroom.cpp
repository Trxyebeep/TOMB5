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

void InsertRoom(ROOM_INFO* r)
{
	float dx, dy, dz, dir;

	clip_left = (float)r->left;
	clip_top = (float)r->top;
	clip_right = (float)r->right;
	clip_bottom = (float)r->bottom;
	room_clip_top = clip_top;
	room_clip_right = clip_right;
	room_clip_left = clip_left;
	room_clip_bottom = clip_bottom;
	aCamPos.x = (float)camera.pos.x;
	aCamPos.y = (float)camera.pos.y;
	aCamPos.z = (float)camera.pos.z;
	aCamTar.x = (float)camera.target.x;
	aCamTar.y = (float)camera.target.y;
	aCamTar.z = (float)camera.target.z;
	dx = aCamTar.x - aCamPos.x;
	dy = aCamTar.y - aCamPos.y;
	dz = aCamTar.z - aCamPos.z;
	dir = 1.0F / (SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
	aCamDir.x = dx * dir;
	aCamDir.y = dy * dir;
	aCamDir.z = dz * dir;

	if (r->nVerts)
	{
		current_room_ptr = r;
		current_room_underwater = r->flags & ROOM_UNDERWATER;

		for (int i = 0; i < r->nRoomlets; i++)
		{
			if (CheckBoundsClip(r->pRoomlets[i].bBox))
				InsertRoomlet(&r->pRoomlets[i]);
		}

		RoomRGB = 0x00FFFFFF;
	}
}

void InsertRoomlet(ROOMLET* roomlet)
{
	TEXTURESTRUCT* tex;
	static long* prelightptr;
	short* ptr;
	long lights, double_sided;

	if (roomlet->nVtx)
	{
		prelightptr = roomlet->pPrelight;
		lights = aBuildRoomletLights(roomlet);
		aRoomletTransformLight(roomlet->pSVtx, roomlet->nVtx, lights & 0x7FFFFFFF, roomlet->nWVtx, roomlet->nSVtx);
		ptr = roomlet->pFac;

		for (int i = 0; i < roomlet->nQuad; i++, ptr += 6)
		{
			double_sided = (ptr[4] >> 15) & 1;
			tex = &textinfo[ptr[4] & 0x3FFF];

			if (tex->drawtype)
				AddQuadSorted(aVertexBuffer, ptr[0], ptr[1], ptr[2], ptr[3], tex, double_sided);
			else
				AddQuadZBuffer(aVertexBuffer, ptr[0], ptr[1], ptr[2], ptr[3], tex, double_sided);
		}

		for (int i = 0; i < roomlet->nTri; i++, ptr += 5)
		{
			double_sided = (ptr[3] >> 15) & 1;
			tex = &textinfo[ptr[3] & 0x3FFF];

			if (tex->drawtype)
				AddTriSorted(aVertexBuffer, ptr[0], ptr[1], ptr[2], tex, double_sided);
			else
				AddTriZBuffer(aVertexBuffer, ptr[0], ptr[1], ptr[2], tex, double_sided);
		}
	}
}

void RoomTestThing()
{

}

void aRoomletTransformLight(float* verts, long nVerts, long nLights, long nWaterVerts, long nShoreVerts)
{
	ROOMLET_LIGHT* light;
	FOGBULB_STRUCT* bulb;
	FVECTOR xyz;
	FVECTOR Nxyz;
	FVECTOR fog_bak;
	FVECTOR vec;
	FVECTOR vec2;
	FVECTOR Lxyz;
	short* clip;
	float num, zbak, zv, zv2, fR, fG, fB, val, val2, val3, fCol;
	long cam_underwater, wx, wy, wz, prelight, sR, sG, sB, cR, cG, cB, iVal, n;
	short clip_distance;
	uchar rnd;
	char choppy;

	clip = clipflags;
	cam_underwater = camera.underwater;

	if (!(App.dx.Flags & 0x80))	//no wibble on software mode?
		cam_underwater = 0;

	num = aRoomletTransformLight_num * 255.0F;

	for (int i = 0; i < nVerts; i++)
	{
		xyz.x = verts[0];
		xyz.y = verts[1];
		xyz.z = verts[2];
		Nxyz.x = verts[3];
		Nxyz.y = verts[4];
		Nxyz.z = verts[5];
		verts += 6;

		if (i < nWaterVerts)
		{
			wx = (long)(xyz.x + current_room_ptr->x) >> 6;
			wy = (long)(xyz.y + current_room_ptr->y) >> 6;
			wz = (long)(xyz.z + current_room_ptr->z) >> 7;
			rnd = WaterTable[current_room_ptr->MeshEffect][(wx + wy + wz) & 0x3F].random;
			xyz.y += WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
		}

		vec.x = xyz.x * D3DMView._11 + xyz.y * D3DMView._21 + xyz.z * D3DMView._31 + D3DMView._41;
		vec.y = xyz.x * D3DMView._12 + xyz.y * D3DMView._22 + xyz.z * D3DMView._32 + D3DMView._42;
		vec.z = xyz.x * D3DMView._13 + xyz.y * D3DMView._23 + xyz.z * D3DMView._33 + D3DMView._43;
		fog_bak.x = vec.x;
		fog_bak.y = vec.y;
		fog_bak.z = vec.z;
		zbak = vec.z;
		aVertexBuffer[i].tu = vec.x;
		aVertexBuffer[i].tv = vec.y;
		clip_distance = 0;

		if (vec.z < f_mznear)
			clip_distance = -128;
		else
		{
			zv = f_mpersp / vec.z;

			if (cam_underwater)
			{
				zv2 = 1.0F / (vec.z * 0.001953125F);
				vec.x = vec.x * zv + f_centerx + vert_wibble_table[((wibble + (long)(zv2 * vec.y)) >> 3) & 0x1F];
				vec.y = vec.y * zv + f_centery + vert_wibble_table[((wibble + (long)(zv2 * vec.x)) >> 3) & 0x1F];
			}
			else
			{
				vec.x = vec.x * zv + f_centerx;
				vec.y = vec.y * zv + f_centery;
			}

			aVertexBuffer[i].rhw = zv * f_moneopersp;

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
		prelight = *(long*)verts;
		verts++;
		fR = 0;
		fG = 0;
		fB = 0;
		sR = 0;
		sG = 0;
		sB = 0;

		for (int j = 0; j < nLights; j++)
		{
			light = &RoomletLights[j];
			Lxyz.x = xyz.x - light->x;
			Lxyz.y = xyz.y - light->y;
			Lxyz.z = xyz.z - light->z;
			val = SQUARE(Lxyz.x) + SQUARE(Lxyz.y) + SQUARE(Lxyz.z);

			if (val < light->sqr_falloff)
			{
				val = sqrt(val);
				val2 = light->inv_falloff * (light->falloff - val);
				Lxyz.x = (Nxyz.x * D3DMView._11 + Nxyz.y * D3DMView._21 + Nxyz.z * D3DMView._31) * (1.0F / val * Lxyz.x);
				Lxyz.y = (Nxyz.x * D3DMView._12 + Nxyz.y * D3DMView._22 + Nxyz.z * D3DMView._32) * (1.0F / val * Lxyz.y);
				Lxyz.z = (Nxyz.x * D3DMView._13 + Nxyz.y * D3DMView._23 + Nxyz.z * D3DMView._33) * (1.0F / val * Lxyz.z);
				val = val2 * (1.0F - (Lxyz.x + Lxyz.y + Lxyz.z));
				fR += light->r * val;
				fG += light->g * val;
				fB += light->b * val;
			}
		}

		fR = CLRR(prelight) + fR * 255.0F;
		fG = CLRG(prelight) + fG * 255.0F;
		fB = CLRB(prelight) + fB * 255.0F;
		cR = (long)fR;
		cG = (long)fG;
		cB = (long)fB;

		if (current_room_underwater)
		{
			wx = (long)(xyz.x * 0.015625F);
			wy = (long)(xyz.y * 0.015625F);
			wz = (long)(xyz.z * 0.0078125F);
			rnd = WaterTable[current_room_ptr->MeshEffect][((wx + wy) + wz) & 0x3F].random;
			choppy = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
			iVal = -2 * choppy;
			cR += iVal;
			cG += iVal;
			cB += iVal;
		}
		else if (nShoreVerts && i > nWaterVerts && i < nShoreVerts + nWaterVerts)
		{
			wx = (long)(xyz.x * 0.015625F);
			wy = (long)(xyz.y * 0.015625F);
			wz = (long)(xyz.z * 0.0078125F);
			rnd = WaterTable[current_room_ptr->MeshEffect][((wx + wy) + wz) & 0x3F].random;
			n = (current_room_ptr->MeshEffect << 6) + (((wibble >> 2) + rnd) & 0x3F);
			iVal = WaterTable[0][n].shimmer + WaterTable[0][n].abs;
			cR += iVal;
			cG += iVal;
			cB += iVal;
		}

		if (zbak > aRoomletTransformLight_bignum)
		{
			val = (zbak - aRoomletTransformLight_bignum) * num;
			cR -= (long)val;
			cG -= (long)val;
			cB -= (long)val;
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

		if (nRoomletFogBulbs)
		{
			prelight = 0;

			for (int j = 0; j < nRoomletFogBulbs; j++)
			{
				bulb = &RoomletFogBulbs[j];
				fCol = 0;

				if (fog_bak.z + bulb->rad > 0)
				{
					if (fabs(fog_bak.x) - bulb->rad < fabs(fog_bak.z) && fabs(fog_bak.y) - bulb->rad < fabs(fog_bak.z))
					{
						vec.x = 0;
						vec.y = 0;
						vec.z = 0;
						vec2.x = 0;
						vec2.y = 0;
						vec2.z = 0;
						val = SQUARE(bulb->pos.x - fog_bak.x) + SQUARE(bulb->pos.y - fog_bak.y) + SQUARE(bulb->pos.z - fog_bak.z);

						if (bulb->sqlen >= bulb->sqrad)
						{
							if (val >= bulb->sqrad)
							{
								val = SQUARE(fog_bak.x) + SQUARE(fog_bak.y) + SQUARE(fog_bak.z);
								val2 = 1.0F / sqrt(val);
								vec.x = val2 * fog_bak.x;
								vec.y = val2 * fog_bak.y;
								vec.z = val2 * fog_bak.z;
								val2 = bulb->pos.x * vec.x + bulb->pos.y * vec.y + bulb->pos.z * vec.z;

								if (val2 > 0)
								{
									val3 = SQUARE(val2);

									if (val > val3)
									{
										val = bulb->sqlen - val3;

										if (val >= bulb->sqrad)
										{
											vec.x = 0;
											vec.y = 0;
											vec.z = 0;
											vec2.x = 0;
											vec2.y = 0;
											vec2.z = 0;
										}
										else
										{
											val3 = sqrtf(bulb->sqrad - val);
											val = val2 - val3;
											vec2.x = val * vec.x;
											vec2.y = val * vec.y;
											vec2.z = val * vec.z;
											val = val2 + val3;
											vec.x *= val;
											vec.y *= val;
											vec.z *= val;
										}
									}
								}
							}
							else
							{
								vec2.x = fog_bak.x;
								vec2.y = fog_bak.y;
								vec2.z = fog_bak.z;
								val = 1.0F / sqrt(SQUARE(fog_bak.x) + SQUARE(fog_bak.y) + SQUARE(fog_bak.z));
								vec.x = val * fog_bak.x;
								vec.y = val * fog_bak.y;
								vec.z = val * fog_bak.z;
								val2 = bulb->pos.x * vec.x + bulb->pos.y * vec.y + bulb->pos.z * vec.z;
								val = val2 - sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
								vec.x *= val;
								vec.y *= val;
								vec.z *= val;
							}
						}
						else if (val >= bulb->sqrad)
						{
							val = 1.0F / sqrt(SQUARE(fog_bak.x) + SQUARE(fog_bak.y) + SQUARE(fog_bak.z));
							vec2.x = val * fog_bak.x;
							vec2.y = val * fog_bak.y;
							vec2.z = val * fog_bak.z;
							val2 = bulb->pos.x * vec2.x + bulb->pos.y * vec2.y + bulb->pos.z * vec2.z;
							val = val2 + sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
							vec2.x *= val;
							vec2.y *= val;
							vec2.z *= val;
						}
						else
						{
							vec2.x = fog_bak.x;
							vec2.y = fog_bak.y;
							vec2.z = fog_bak.z;
						}

						fCol = sqrt(SQUARE(vec2.x - vec.x) + SQUARE(vec2.y - vec.y) + SQUARE(vec2.z - vec.z)) * bulb->d;
					}
				}

				if (fCol)
				{
					prelight += (long)fCol;
					sR += (long)(fCol * bulb->r);
					sG += (long)(fCol * bulb->g);
					sB += (long)(fCol * bulb->b);
				}
			}

			cR -= prelight;
			cG -= prelight;
			cB -= prelight;
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

		aVertexBuffer[i].color = RGBA(cR, cG, cB, 255);
		aVertexBuffer[i].specular = RGBA(sR, sG, sB, 255);
	}
}

void aBuildFogBulbList()
{
	FOGBULB_STRUCT* ActiveFog;
	FOGBULB* Fog;
	FVECTOR vec;
	FVECTOR vec2;
	static float unused1 = 0.025F;
	static float unused2 = 64.0F;
	long num_active;

	if (NumLevelFogBulbs)
	{
		num_active = 0;

		for (int i = 0; i < NumLevelFogBulbs; i++)
		{
			ActiveFog = &ActiveFogBulbs[i];
			Fog = &fog_bulbs[i];

			vec.x = Fog->px;
			vec.y = Fog->py;
			vec.z = Fog->pz;
			vec2.x = vec.x - aCamera.pos.x;
			vec2.y = vec.y - aCamera.pos.y;
			vec2.z = vec.z - aCamera.pos.z;

			if (sqrt(SQUARE(vec2.x) + SQUARE(vec2.y) + SQUARE(vec2.z)) >= 20480)
				continue;

			ActiveFog->visible = 1;
			ActiveFog->world.x = vec.x;
			ActiveFog->world.y = vec.y;
			ActiveFog->world.z = vec.z;
			ActiveFog->rad = Fog->rad;
			ActiveFog->sqrad = Fog->sqrad;
			ActiveFog->r = Fog->r;
			ActiveFog->g = Fog->g;
			ActiveFog->b = Fog->b;
			num_active++;
			vec2.x = D3DCameraMatrix._11 * vec.x + D3DCameraMatrix._21 * vec.y + D3DCameraMatrix._31 * vec.z + D3DCameraMatrix._41;
			vec2.y = D3DCameraMatrix._12 * vec.x + D3DCameraMatrix._22 * vec.y + D3DCameraMatrix._32 * vec.z + D3DCameraMatrix._42;
			vec2.z = D3DCameraMatrix._13 * vec.x + D3DCameraMatrix._23 * vec.y + D3DCameraMatrix._33 * vec.z + D3DCameraMatrix._43;
			ActiveFog->pos.x = vec2.x;
			ActiveFog->pos.y = vec2.y;
			ActiveFog->pos.z = vec2.z;
			ActiveFog->sqlen = SQUARE(vec2.x) + SQUARE(vec2.y) + SQUARE(vec2.z);
			ActiveFog->d = 1 / Fog->den;
		}

		NumActiveFogBulbs = num_active;
	}

	unused1 = 1.0F / unused2;
}

void inject_drawroom(bool replace)
{
	INJECT(0x0049C9F0, DrawBoundsRectangle, replace);
	INJECT(0x0049CC20, DrawBoundsRectangleII, replace);
	INJECT(0x0049CE40, DrawClipRectangle, replace);
	INJECT(0x0049A9D0, InsertRoom, replace);
	INJECT(0x0049ABF0, InsertRoomlet, replace);
	INJECT(0x0049A9B0, RoomTestThing, replace);
	INJECT(0x0049B7B0, aRoomletTransformLight, replace);
	INJECT(0x0049AFB0, aBuildFogBulbList, replace);
}
