#include "../tomb5/pch.h"
#include "drawroom.h"
#include "function_table.h"
#include "function_stubs.h"
#include "d3dmatrix.h"
#include "alexstuff.h"
#include "dxshell.h"
#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#include "../game/gameflow.h"
#endif
#include "polyinsert.h"

short CheckClipBox[8 * 3] = { 0, 1, 2, 3, 1, 2, 0, 1, 5, 3, 1, 5, 0, 4, 2, 3, 4, 2, 0, 4, 5, 3, 4, 5 };

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

void DrawBoundsRectangleII(float left, float top, float right, float bottom, long rgba)
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

#ifdef GENERAL_FIXES
bool has_water_neighbor;

static bool IsMistVert(FVECTOR* v)
{
	PORTAL* p;
	ROOM_INFO* r;
	ROOMLET* rlet;
	float* verts;
	short* door;
	float x, y, z;
	short drn;

	door = current_room_ptr->door;

	if (!door)
		return 0;

	x = v->x + current_room_ptr->x;
	y = v->y;
	z = v->z + current_room_ptr->z;
	drn = *door++;

	for (p = (PORTAL*)door; drn > 0; drn--, p++)
	{
		r = &room[p->rn];

		if (p->normal[1] != -1 || v->x < p->v1[0] || v->x > p->v3[0] || v->z < p->v1[2] || v->z > p->v2[2])
			continue;

		rlet = r->pRoomlets;

		for (int i = 0; i < r->nRoomlets; i++, rlet++)
		{
			verts = rlet->pSVtx;

			for (int n = 0; n < rlet->nVtx; n++, verts += 7)
			{
				if (x == verts[0] + r->x && y == verts[1] && z == verts[2] + r->z)
					return 0;
			}
		}
	}

	return 1;
}

static bool IsReflectionVert(FVECTOR* v)
{
	PORTAL* p;
	PORTAL* np;
	ROOM_INFO* r;
	ROOM_INFO* nr;
	ROOMLET* rlet;
	float* verts;
	short* door;
	short* ndoor;
	float x, y, z;
	short drn, ndrn, cont;

	has_water_neighbor = 0;
	door = current_room_ptr->door;

	if (!door)
		return 0;

	cont = 0;
	x = v->x + current_room_ptr->x;
	y = v->y;
	z = v->z + current_room_ptr->z;
	drn = *door++;

	for (p = (PORTAL*)door; drn > 0; drn--, p++)
	{
		r = &room[p->rn];

		if (p->normal[1] != -1 || v->x < p->v1[0] || v->x > p->v3[0] || v->z < p->v1[2] || v->z > p->v2[2])
			continue;

		if (r->flags & ROOM_UNDERWATER)
			cont = 1;

		rlet = r->pRoomlets;

		for (int i = 0; i < r->nRoomlets; i++, rlet++)
		{
			verts = rlet->pSVtx;

			for (int n = 0; n < rlet->nVtx; n++, verts += 7)
			{
				if (x == verts[0] + r->x && y == verts[1] && z == verts[2] + r->z)
					return 0;
			}
		}
	}

	if (cont)
	{
		has_water_neighbor = 1;
		door = current_room_ptr->door;

		if (door)
		{
			drn = *door++;

			for (p = (PORTAL*)door; drn > 0; drn--, p++)
			{
				r = &room[p->rn];

				ndoor = r->door;
				ndrn = *ndoor++;

				for (np = (PORTAL*)ndoor; ndrn > 0; ndrn--, np++)
				{
					nr = &room[np->rn];

					if (!(nr->flags & ROOM_UNDERWATER))
						continue;

					rlet = nr->pRoomlets;

					for (int i = 0; i < nr->nRoomlets; i++, rlet++)
					{
						verts = rlet->pSVtx;

						for (int n = 0; n < rlet->nVtx; n++, verts += 7)
						{
							if (x == verts[0] + nr->x && y == verts[1] && z == verts[2] + nr->z)
								return 0;
						}
					}
				}
			}
		}
	}

	return cont;
}

static bool IsShoreVert(FVECTOR* v)
{
	PORTAL* p;
	ROOM_INFO* r;
	ROOMLET* rlet;
	float* verts;
	short* door;
	float x, y, z;
	short drn;

	door = current_room_ptr->door;

	if (!door)
		return 0;

	x = v->x + current_room_ptr->x;
	y = v->y;
	z = v->z + current_room_ptr->z;
	drn = *door++;

	for (p = (PORTAL*)door; drn > 0; drn--, p++)
	{
		r = &room[p->rn];

		if (!(r->flags & ROOM_UNDERWATER) || !p->normal[1] || v->x < p->v1[0] || v->x > p->v3[0] || v->z < p->v1[2] || v->z > p->v2[2])
			continue;

		rlet = r->pRoomlets;

		for (int i = 0; i < r->nRoomlets; i++, rlet++)
		{
			verts = rlet->pSVtx;

			for (int n = 0; n < rlet->nVtx; n++, verts += 7)
			{
				if (x == verts[0] + r->x && y == verts[1] && z == verts[2] + r->z)
					return 1;
			}
		}
	}

	return 0;
}
#endif

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
	static float DistanceFogStart = 12.0F * 1024.0F;
	static float iDistanceFogStart = 1.0F / DistanceFogStart;
	static float DistanceFogEnd = 20.0F * 1024.0F;
	float num, zbak, zv, zv2, fR, fG, fB, val, val2, val3, fCol;
	long cam_underwater, wx, wy, wz, prelight, sR, sG, sB, cR, cG, cB, iVal;
	short clipFlag;
	uchar rnd, abs;
#ifdef GENERAL_FIXES
	uchar flags;
#endif
	char choppy, shimmer;

	clip = clipflags;
	cam_underwater = camera.underwater;

	if (!(App.dx.Flags & 0x80))	//no wibble on software mode
		cam_underwater = 0;

#ifdef GENERAL_FIXES
	DistanceFogStart = tomb5.distance_fog * 1024.0F;
	iDistanceFogStart = 1.0F / DistanceFogStart;
#endif

	num = iDistanceFogStart * 255.0F;

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
		clipFlag = 0;

		if (vec.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vec.z;

			if (cam_underwater)
			{
				zv2 = 1.0F / (vec.z / 512.0F);
				vec.x = vec.x * zv + f_centerx + vert_wibble_table[((wibble + long(zv2 * vec.y)) >> 3) & 0x1F];
				vec.y = vec.y * zv + f_centery + vert_wibble_table[((wibble + long(zv2 * vec.x)) >> 3) & 0x1F];
			}
			else
			{
				vec.x = vec.x * zv + f_centerx;
				vec.y = vec.y * zv + f_centery;
			}

			aVertexBuffer[i].rhw = zv * f_moneopersp;

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

#ifdef GENERAL_FIXES
		flags = 0;

		if (tomb5.shimmer)
		{
			if (current_room_ptr->flags & ROOM_REFLECT && current_room_ptr->flags & ROOM_REFLECT_CEILING)
			{
				if (gfCurrentLevel == LVL5_RED_ALERT)
				{
					if (xyz.y != current_room_ptr->minfloor)
						flags |= 2;
				}
				else if (!IsMistVert(&xyz) || ABS(xyz.y - current_room_ptr->minfloor) > 1536)
					flags |= 1;
			}
			else if (xyz.y == current_room_ptr->minfloor)
			{
				if (current_room_ptr->flags & ROOM_MIST)
				{
					if (IsMistVert(&xyz))
						flags |= 1;
				}
				else if (current_room_ptr->flags & ROOM_REFLECT)
				{
					if (IsReflectionVert(&xyz))
						flags |= 1;
					else if (IsShoreVert(&xyz) || has_water_neighbor)
						flags |= 1;
				}
			}
		}
#endif

		if (current_room_underwater)
		{
			wx = long(xyz.x / 64.0F);
			wy = long(xyz.y / 64.0F);
			wz = long(xyz.z / 128.0F);
			rnd = WaterTable[current_room_ptr->MeshEffect][(wx + wy + wz) & 0x3F].random;
			choppy = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
			iVal = -2 * choppy;
			cR += iVal;
			cG += iVal;
			cB += iVal;
		}
#ifdef GENERAL_FIXES
		else if (flags & 1)
		{
			wx = long(xyz.x / 64.0F);
			wy = long(xyz.y / 64.0F);
			wz = long(xyz.z / 128.0F);
			rnd = WaterTable[current_room_ptr->MeshEffect][(wx + wy + wz) & 0x3F].random;
			shimmer = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer;
			abs = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs;
			iVal = (shimmer + abs) << 3;
			cR += iVal;
			cG += iVal;
			cB += iVal;
		}
		else if (flags & 2)	//special Red Alert! gas rooms wibble (slower and green only)
		{
			wx = long(xyz.x / 64.0F);
			wy = long(xyz.y / 64.0F);
			wz = long(xyz.z / 128.0F);
			rnd = WaterTable[current_room_ptr->MeshEffect][(wx + wy + wz) & 0x3F].random;
			shimmer = WaterTable[current_room_ptr->MeshEffect][((wibble >> 3) + rnd) & 0x3F].shimmer;
			abs = WaterTable[current_room_ptr->MeshEffect][((wibble >> 3) + rnd) & 0x3F].abs;
			iVal = (shimmer + abs) << 3;
			cG += ABS(iVal);
		}
#else
		else if (nShoreVerts && i > nWaterVerts && i < nShoreVerts + nWaterVerts)
		{
			wx = long(xyz.x * 0.015625F);
			wy = long(xyz.y * 0.015625F);
			wz = long(xyz.z * 0.0078125F);
			rnd = WaterTable[current_room_ptr->MeshEffect][(wx + wy + wz) & 0x3F].random;
			shimmer = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer;
			abs = WaterTable[current_room_ptr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs;
			iVal = shimmer + abs;
			cR += iVal;
			cG += iVal;
			cB += iVal;
		}
#endif

#ifdef GENERAL_FIXES
		if (zbak > DistanceFogStart)
		{
			val = (zbak - DistanceFogStart) * num;
#else
		if (zbak > DistanceFogEnd)
		{
			val = (zbak - DistanceFogEnd) * num;
#endif
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
	FOGBULB_INFO* Fog;
	FVECTOR vec;
	FVECTOR vec2;
	static float unused1 = 0.025F;
	static float unused2 = 64.0F;
	long num_active;

	if (NumLevelFogBulbs)
	{
		num_active = 0;
		ActiveFog = ActiveFogBulbs;

		for (int i = 0; i < NumLevelFogBulbs; i++)
		{
			Fog = &LevelFogBulbs[i];
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
			ActiveFog++;
		}

		NumActiveFogBulbs = num_active;
	}

	unused1 = 1.0F / unused2;
}

void ProcessMeshData(long num_meshes)
{
	MESH_DATA* data;
	D3DVECTOR d3dvec;
	FVECTOR vec;
	short* sMeshPtr;
	short* no_mesh;
	short* gtx;
	float minx, miny, minz, maxx, maxy, maxz;
	long num, pre;

	Log(2, "ProcessMeshData %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)game_malloc(4 * num_meshes, 0);
	mesh_base = (short*)malloc_ptr;
	no_mesh = 0;
#ifdef GENERAL_FIXES	//uninitialized
	data = 0;
#endif

	for (int i = 0; i < num_meshes; i++)
	{
		sMeshPtr = meshes[i];

		if (no_mesh == sMeshPtr)
		{
			meshes[i] = (short*)data;
			mesh_vtxbuf[i] = data;
		}
		else
		{
			no_mesh = meshes[i];
			minx = 20000.0F;
			miny = 20000.0F;
			minz = 20000.0F;
			maxx = -20000.0F;
			maxy = -20000.0F;
			maxz = -20000.0F;
			data = (MESH_DATA*)game_malloc(sizeof(MESH_DATA), 0);
			memset(data, 0, sizeof(MESH_DATA));
			meshes[i] = (short*)data;
			mesh_vtxbuf[i] = data;
			data->x = *sMeshPtr;
			sMeshPtr++;
			data->y = *sMeshPtr;
			sMeshPtr++;
			data->z = *sMeshPtr;
			sMeshPtr++;
			data->r = *sMeshPtr;
			sMeshPtr++;
			data->flags = *sMeshPtr;
			sMeshPtr++;
			data->nVerts = *sMeshPtr & 0xFF;

			if (!data->nVerts)
				num = *sMeshPtr >> 8;

			sMeshPtr++;
			data->aVtx = (ACMESHVERTEX*)game_malloc(sizeof(ACMESHVERTEX) * data->nVerts, 0);

			if (data->nVerts)
			{
				for (int j = 0; j < data->nVerts; j++)
				{
					vec.x = *sMeshPtr;
					sMeshPtr++;
					vec.y = *sMeshPtr;
					sMeshPtr++;
					vec.z = *sMeshPtr;
					sMeshPtr++;
					data->aVtx[j].x = vec.x;
					data->aVtx[j].y = vec.y;
					data->aVtx[j].z = vec.z;

					if (vec.x < minx)
						minx = vec.x;

					if (vec.x > maxx)
						maxx = vec.x;

					if (vec.y < miny)
						miny = vec.y;

					if (vec.y > maxy)
						maxy = vec.y;

					if (vec.z < minz)
						minz = vec.z;

					if (vec.z > maxz)
						maxz = vec.z;
				}

				data->bbox[0] = minx;
				data->bbox[1] = miny;
				data->bbox[2] = minz;
				data->bbox[3] = maxx;
				data->bbox[4] = maxy;
				data->bbox[5] = maxz;
				data->nNorms = *sMeshPtr;
				sMeshPtr++;

				if (!data->nNorms)
					data->nNorms = data->nVerts;

				if (data->nNorms <= 0)
				{
					data->Normals = 0;
					data->prelight = (long*)game_malloc(4 * data->nVerts, 0);

					for (int j = 0; j < data->nVerts; j++)
					{
						pre = 255 - (*sMeshPtr >> 5);
						sMeshPtr++;
						data->prelight[j] = pre | ((pre | (pre << 8)) << 8);
						data->aVtx[j].prelight = pre | ((pre | (pre << 8)) << 8);
					}

					data->aFlags |= 2;
				}
				else
				{
					data->Normals = (D3DVECTOR*)game_malloc(sizeof(D3DVECTOR) * data->nNorms, 0);

					for (int j = 0; j < data->nVerts; j++)
					{
						d3dvec.x = *sMeshPtr;
						sMeshPtr++;
						d3dvec.y = *sMeshPtr;
						sMeshPtr++;
						d3dvec.z = *sMeshPtr;
						sMeshPtr++;
						D3DNormalise(&d3dvec);
						data->aVtx[j].nx = d3dvec.x;
						data->aVtx[j].ny = d3dvec.y;
						data->aVtx[j].nz = d3dvec.z;
					}

					data->prelight = 0;
				}
			}
			else
				sMeshPtr += (6 * num) + 1;

			data->ngt4 = *sMeshPtr;
			sMeshPtr++;

			if (data->ngt4)
			{
				data->gt4 = (short*)game_malloc(12 * data->ngt4, 0);
				memcpy(data->gt4, sMeshPtr, 12 * data->ngt4);
				sMeshPtr += 6 * data->ngt4;
				gtx = data->gt4 + 5;

				for (int j = 0; j < data->ngt4; j++)
				{
					if (gtx[j * 6] & 2)
					{
						data->aFlags |= 1;
						break;
					}
				}
			}

			data->ngt3 = *sMeshPtr;
			sMeshPtr++;

			if (data->ngt3)
			{
				data->gt3 = (short*)game_malloc(10 * data->ngt3, 0);
				memcpy(data->gt3, sMeshPtr, 10 * data->ngt3);
				gtx = data->gt3 + 4;

				for (int j = 0; j < data->ngt3; j++)
				{
					if (gtx[j * 5] & 2)
					{
						data->aFlags |= 1;
						break;
					}
				}
			}
		}
	}

	Log(2, "End ProcessMeshData");
}

void DrawRoomletBounds(ROOMLET* r)
{
	D3DTLVERTEX v[32];
	D3DVECTOR bounds[8];
	float zv;
	ulong col;

	bounds[0].x = r->bBox[0];
	bounds[0].y = r->bBox[1] + 32;
	bounds[0].z = r->bBox[2];

	bounds[1].x = r->bBox[3];
	bounds[1].y = r->bBox[1] + 32;
	bounds[1].z = r->bBox[2];

	bounds[2].x = r->bBox[0];
	bounds[2].y = r->bBox[1] + 32;
	bounds[2].z = r->bBox[5];

	bounds[3].x = r->bBox[3];
	bounds[3].y = r->bBox[1] + 32;
	bounds[3].z = r->bBox[5];

	bounds[4].x = r->bBox[0];
	bounds[4].y = r->bBox[4] + 32;
	bounds[4].z = r->bBox[2];

	bounds[5].x = r->bBox[3];
	bounds[5].y = r->bBox[4] + 32;
	bounds[5].z = r->bBox[2];

	bounds[6].x = r->bBox[0];
	bounds[6].y = r->bBox[4] + 32;
	bounds[6].z = r->bBox[5];

	bounds[7].x = r->bBox[3];
	bounds[7].y = r->bBox[4] + 32;
	bounds[7].z = r->bBox[5];

	aTransformClip_D3DV(bounds, &v[0], 8, 0);
	aTransformClip_D3DV(bounds, &v[8], 8, 8);
	aTransformClip_D3DV(bounds, &v[16], 8, 16);
	aTransformClip_D3DV(bounds, &v[24], 8, 24);

	col = 0xFF000000 | RoomRGB;
	zv = f_mpersp / f_mznear * f_moneopersp;

	for (int i = 0; i < 32; i++)
	{
		v[i].color = col;
		v[i].specular = 0xFF000000;
		v[i].rhw = zv;
	}

	for (int i = 0; i < 4; i++)
	{
		v[i + 8].sx += 8;
		v[i + 12].sx += 8;
		v[i + 24].sx += 8;
		v[i + 28].sx += 8;
		AddQuadZBuffer(v, i, i + 8, i + 28, i + 20, textinfo, 1);
	}
}

long aBuildRoomletLights(ROOMLET* r)
{
	ROOMLET_LIGHT* light;
	DYNAMIC* dynamic;
	FOGBULB_STRUCT* fogbulb;
	FOGBULB_STRUCT* rFog;
	FVECTOR dPos;
	FVECTOR fPos;
	FVECTOR box;
	float* bbox;
	float falloff, sqr, val;
	long numLights;

	light = RoomletLights;
	numLights = 0;

	for (int i = 0; i < 32; i++)
	{
		dynamic = &dynamics[i];

		if (!dynamic->on)
			continue;

		dPos.x = dynamic->x - current_room_ptr->posx;
		dPos.y = dynamic->y - current_room_ptr->posy;
		dPos.z = dynamic->z - current_room_ptr->posz;
		falloff = float((dynamic->falloff >> 1) + (dynamic->falloff >> 3));

		if (dPos.x - falloff <= r->bBox[3] && falloff + dPos.x >= r->bBox[0] &&
			dPos.z - falloff <= r->bBox[5] && falloff + dPos.z >= r->bBox[2])
		{
			light->type = 0;
			light->x = dPos.x;
			light->y = dPos.y;
			light->z = dPos.z;
			light->r = (float)dynamic->r * (1.0F / 255.0F);
			light->g = (float)dynamic->g * (1.0F / 255.0F);
			light->b = (float)dynamic->b * (1.0F / 255.0F);
			light->falloff = falloff;
			light->inv_falloff = 1.0F / light->falloff;
			light->sqr_falloff = SQUARE(light->falloff);
			light++;
			numLights++;
		}
	}

	nRoomletFogBulbs = 0;
	rFog = RoomletFogBulbs;

	for (int i = 0; i < NumActiveFogBulbs; i++)
	{
		fogbulb = &ActiveFogBulbs[i];

		if (!fogbulb->visible)
			continue;

		if (fogbulb->sqlen >= fogbulb->sqrad)	//this block does nothing
		{
			bbox = aBoundingBox;

			for (int j = 0; j < 8; j++)
			{
				box.x = *bbox++;
				box.y = *bbox++;
				box.z = *bbox++;
				fPos.x = fogbulb->pos.x - box.x;
				fPos.y = fogbulb->pos.y - box.y;
				fPos.z = fogbulb->pos.z - box.z;

				if (SQUARE(fPos.x) + SQUARE(fPos.y) + SQUARE(fPos.y) < fogbulb->sqrad)
					break;

				sqr = SQUARE(box.x) + SQUARE(box.y) + SQUARE(box.z);
				val = 1.0F / sqrt(sqr);
				fPos.x = box.x * val;
				fPos.y = box.y * val;
				fPos.z = box.z * val;
				val = fogbulb->pos.x * fPos.x + fogbulb->pos.y * fPos.y + fogbulb->pos.z * fPos.z;

				if (val > 0)
				{
					val = SQUARE(val);

					if (sqr > val && fogbulb->sqlen - val < fogbulb->sqrad)
						break;
				}
			}
		}

		rFog->pos.x = fogbulb->pos.x;
		rFog->pos.y = fogbulb->pos.y;
		rFog->pos.z = fogbulb->pos.z;
		rFog->world.x = fogbulb->world.x;
		rFog->world.y = fogbulb->world.y;
		rFog->world.z = fogbulb->world.z;
		rFog->r = fogbulb->r;
		rFog->g = fogbulb->g;
		rFog->b = fogbulb->b;
		rFog->sqlen = fogbulb->sqlen;
		rFog->d = fogbulb->d;
		rFog->rad = fogbulb->rad;
		rFog->sqrad = fogbulb->sqrad;
		rFog++;
		nRoomletFogBulbs++;
		numLights |= 0x80000000;
	}

	return numLights;
}

void aRoomInit()
{
	ROOM_INFO* r;
	FOGBULB_INFO* bulb;
	long nBulbs;

	nBulbs = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (!r->nFogBulbs)
			continue;

		for (int j = 0; j < r->nFogBulbs; j++)
		{
			bulb = &LevelFogBulbs[nBulbs];
			memcpy(bulb, &r->fogbulb[j], sizeof(FOGBULB_INFO));

			if (gfCurrentLevel == 2 || gfCurrentLevel == 3)
			{
				bulb->den -= bulb->den * 0.5F;

				if (bulb->den < 0)
					bulb->den = 0;
			}

			bulb->den = (90.0F - bulb->den / 1024.0F) * 0.8F + 0.2F;

			if (bulb->den < 14)
				bulb->den = 14;

			nBulbs++;
		}
	}

	NumLevelFogBulbs = nBulbs;
}

void aResetFogBulbList()
{
	NumActiveFogBulbs = 0;
}

void TriggerFogBulbFX(long r, long g, long b, long x, long y, long z, long rad, long den)
{
	FOGBULB_STRUCT* bulb;

	if (nFXFogBulbs < 3)
	{
		bulb = &FXFogBulbs[nFXFogBulbs];
		nFXFogBulbs++;

		bulb->pos.x = (float)x;
		bulb->pos.y = (float)y;
		bulb->pos.z = (float)z;
		bulb->rad = (float)rad;
		bulb->sqrad = SQUARE(bulb->rad);
		bulb->d = 1.0F / den;
		bulb->r = (float)r;
		bulb->g = (float)g;
		bulb->b = (float)b;
	}
}

void aBuildFXFogBulbList()
{
	FOGBULB_STRUCT* bulb;
	FOGBULB_STRUCT* fxBulb;
	FVECTOR pos;
	long nBulbs;

	if (!nFXFogBulbs)
		return;

	bulb = &ActiveFogBulbs[NumActiveFogBulbs];
	nBulbs = NumActiveFogBulbs;

	for (int i = 0; i < nFXFogBulbs; i++)
	{
		fxBulb = &FXFogBulbs[i];
		pos.x = fxBulb->pos.x;
		pos.y = fxBulb->pos.y;
		pos.z = fxBulb->pos.z;
		bulb->world.x = pos.x;
		bulb->world.y = pos.y;
		bulb->world.y = pos.z;
		bulb->pos.x = pos.x * D3DCameraMatrix._11 + pos.y * D3DCameraMatrix._21 + pos.z * D3DCameraMatrix._31 + D3DCameraMatrix._41;
		bulb->pos.y = pos.x * D3DCameraMatrix._12 + pos.y * D3DCameraMatrix._22 + pos.z * D3DCameraMatrix._32 + D3DCameraMatrix._42;
		bulb->pos.z = pos.x * D3DCameraMatrix._13 + pos.y * D3DCameraMatrix._23 + pos.z * D3DCameraMatrix._33 + D3DCameraMatrix._43;
		bulb->rad = fxBulb->rad;
		bulb->sqrad = fxBulb->sqrad;
		bulb->sqlen = SQUARE(bulb->pos.x) + SQUARE(bulb->pos.y) + SQUARE(bulb->pos.z);
		bulb->visible = 1;
		bulb->d = fxBulb->d;
		bulb->r = fxBulb->r / 255.0F;
		bulb->g = fxBulb->g / 255.0F;
		bulb->b = fxBulb->b / 255.0F;
		bulb++;
		nBulbs++;
	}

	NumActiveFogBulbs = nBulbs;
	nFXFogBulbs = 0;
}

void InitBuckets()
{
	TEXTUREBUCKET* bucket;

	for (int i = 0; i < 30; i++)
	{
		bucket = &Bucket[i];
		bucket->tpage = -1;
		bucket->nVtx = 0;
	}
}

void aSetBumpComponent(TEXTUREBUCKET* bucket)
{
	for (int i = 0; i < bucket->nVtx; i++)
	{
		if (bucket->vtx[i].specular & 0xFFFFFF)
		{
			BucketSpecular[i] = bucket->vtx[i].specular;
			bucket->vtx[i].specular = 0;
		}
		else
			BucketSpecular[i] = 0;
	}
}

void aResetBumpComponent(TEXTUREBUCKET* bucket)
{
	for (int i = 0; i < bucket->nVtx; i++)
	{
		if (BucketSpecular[i])
			bucket->vtx[i].specular = BucketSpecular[i];
	}
}

void DrawBucket(TEXTUREBUCKET* bucket)
{
	if (bucket->tpage == 1)
		bucket->tpage = 1;

	if (!bucket->nVtx)
		return;

	if (Textures[bucket->tpage].bump && App.BumpMapping)
	{
		aSetBumpComponent(bucket);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[Textures[bucket->tpage].bumptpage].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		DrawPrimitiveCnt++;
		aResetBumpComponent(bucket);
	}

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

	if (App.BumpMapping)
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

	bucket->nVtx = 0;
	bucket->tpage = -1;
	DrawPrimitiveCnt++;
}

void DrawBuckets()
{
	TEXTUREBUCKET* bucket;

	if (App.BumpMapping)
	{
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		for (int i = 0; i < 30; i++)
		{
			bucket = &Bucket[i];

			if (Textures[bucket->tpage].bump && bucket->nVtx)
			{
				aSetBumpComponent(bucket);
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[Textures[bucket->tpage].bumptpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
				DrawPrimitiveCnt++;
				aResetBumpComponent(bucket);
			}
		}

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		for (int i = 0; i < 30; i++)
		{
			bucket = &Bucket[i];

			if (Textures[bucket->tpage].bump && bucket->nVtx)
			{
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

		for (int i = 0; i < 30; i++)
		{
			bucket = &Bucket[i];

			if (!Textures[bucket->tpage].bump && bucket->nVtx)
			{
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}
	}
	else
	{
		for (int i = 0; i < 30; i++)
		{
			bucket = &Bucket[i];
			DrawBucket(bucket);
		}
	}
}

void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp)
{
	TEXTUREBUCKET* bucket;
	long nVtx, biggest;

	for (int i = 0; i < 30; i++)
	{
		bucket = &Bucket[i];

		if (bucket->tpage == tpage && bucket->nVtx < 2048)
		{
			*Vpp = &bucket->vtx[bucket->nVtx];
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if (bucket->nVtx > 2048)
		{
			DrawBucket(bucket);
			bucket->tpage = tpage;
			bucket->nVtx = 0;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}
	}

	nVtx = 0;
	biggest = 0;

	for (int i = 0; i < 30; i++)
	{
		bucket = &Bucket[i];

		if (bucket->tpage == -1)
		{
			bucket->tpage = tpage;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if (bucket->nVtx > nVtx)
		{
			nVtx = bucket->nVtx;
			biggest = i;
		}
	}

	bucket = &Bucket[biggest];
	DrawBucket(bucket);
	bucket->tpage = tpage;
	bucket->nVtx = 0;
	*Vpp = bucket->vtx;
	*nVtxpp = &bucket->nVtx;
}

long CheckBoundsClip(float* box)
{
	float* stash;
	short* checkBox;
	float left, right, top, bottom, front, back;
	float x, y, z, bx, by, bz, zv;

	left = 10000.0F;
	right = -10000.0F;
	top = 10000.0F;
	bottom = -10000.0F;
	front = 10000.0F;
	back = -10000.0F;
	stash = aBoundingBox;
	checkBox = CheckClipBox;

	for (int i = 0; i < 8; i++)
	{
		bx = box[checkBox[0]];
		by = box[checkBox[1]];
		bz = box[checkBox[2]];
		checkBox += 3;

		x = bx * D3DMView._11 + by * D3DMView._21 + bz * D3DMView._31 + D3DMView._41;
		y = bx * D3DMView._12 + by * D3DMView._22 + bz * D3DMView._32 + D3DMView._42;
		z = bx * D3DMView._13 + by * D3DMView._23 + bz * D3DMView._33 + D3DMView._43;
		stash[0] = x;
		stash[1] = y;
		stash[2] = z;
		stash += 3;

		if (z < f_mznear)
			z = f_mznear;

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
			left = f_left;

		if (right > f_right)
			right = f_right;

		if (top < f_top)
			top = f_top;

		if (bottom > f_bottom)
			bottom = f_bottom;
	}

	if (left > room_clip_right || right < room_clip_left || top > room_clip_bottom || bottom < room_clip_top)
		return 0;

	return 1;
}

void PrelightVertsMMXByRoomlet(D3DTLVERTEX* v, ROOMLET* r)
{
	long* prelight;

	prelight = r->pPrelight;

	for (int i = 0; i < r->nVtx; i++)
		AddPrelitMMX(prelight[i], &v[i].color);

	__asm
	{
		emms
	}
}

void PrelightVertsNonMMXByRoomlet(D3DTLVERTEX* v, ROOMLET* r)
{
	long* prelight;
	long pr, pg, pb, vr, vg, vb;

	prelight = r->pPrelight;

	for (int i = 0; i < r->nVtx; i++)
	{
		pr = prelight[i] & 0xFF0000;
		pg = prelight[i] & 0x00FF00;
		pb = prelight[i] & 0x0000FF;
		vr = v[i].color & 0xFF0000;
		vg = v[i].color & 0x00FF00;
		vb = v[i].color & 0x0000FF;
		pr += vr;
		pg += vg;
		pb += vb;

		if (pr > 0xFF0000)
			pr = 0xFF0000;

		if (pg > 0x00FF00)
			pg = 0x00FF00;

		if (pb > 0x0000FF)
			pb = 0x0000FF;

		v[i].color = (v[i].color & 0xFF000000) | pr | pg | pb;
		CalcColorSplit(v[i].color, &v[i].color);
	}
}

void CalcTriFaceNormal(D3DVECTOR* p1, D3DVECTOR* p2, D3DVECTOR* p3, D3DVECTOR* n)
{
	FVECTOR u, v;

	u.x = p1->x - p2->x;
	u.y = p1->y - p2->y;
	u.z = p1->z - p2->z;
	v.x = p3->x - p2->x;
	v.y = p3->y - p2->y;
	v.z = p3->z - p2->z;
	n->x = v.z * u.y - v.y * u.z;
	n->y = v.x * u.z - v.z * u.x;
	n->z = v.y * u.x - v.x * u.y;
}

void CreateVertexNormals(MESH_DATA* mesh)
{
	LPD3DVERTEX v;
	LPD3DVECTOR fnormals;
	D3DVECTOR p1;
	D3DVECTOR p2;
	D3DVECTOR p3;
	D3DVECTOR n1;
	D3DVECTOR n2;
	short* quad;
	short* tri;

	fnormals = (LPD3DVECTOR)malloc(sizeof(D3DVECTOR) * (mesh->ngt3 + mesh->ngt4));
	mesh->SourceVB->Lock(0, (LPVOID*)&v, 0);
	quad = mesh->gt4;

	for (int i = 0; i < mesh->ngt4; i++)
	{
		p1.x = v[quad[0]].x;
		p1.y = v[quad[0]].y;
		p1.z = v[quad[0]].z;

		p2.x = v[quad[1]].x;
		p2.y = v[quad[1]].y;
		p2.z = v[quad[1]].z;

		p3.x = v[quad[2]].x;
		p3.y = v[quad[2]].y;
		p3.z = v[quad[2]].z;

		CalcTriFaceNormal(&p1, &p2, &p3, &n1);

		p1.x = v[quad[0]].x;
		p1.y = v[quad[0]].y;
		p1.z = v[quad[0]].z;

		p2.x = v[quad[2]].x;
		p2.y = v[quad[2]].y;
		p2.z = v[quad[2]].z;

		p3.x = v[quad[3]].x;
		p3.y = v[quad[3]].y;
		p3.z = v[quad[3]].z;

		CalcTriFaceNormal(&p1, &p2, &p3, &n2);

		n1.x += n2.x;
		n1.y += n2.y;
		n1.z += n2.z;
		D3DNormalise(&n1);

		n1.x = 0;
		n1.y = 1.0F;
		n1.z = 0;
		fnormals[i] = n1;
		quad += 6;
	}

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++)
	{
		p1.x = v[tri[0]].x;
		p1.y = v[tri[0]].y;
		p1.z = v[tri[0]].z;

		p2.x = v[tri[1]].x;
		p2.y = v[tri[1]].y;
		p2.z = v[tri[1]].z;

		p3.x = v[tri[2]].x;
		p3.y = v[tri[2]].y;
		p3.z = v[tri[2]].z;

		CalcTriFaceNormal(&p1, &p2, &p3, &n1);
		D3DNormalise(&n1);
		fnormals[mesh->ngt4 + i] = n1;
		tri += 5;
	}

	for (int i = 0; i < mesh->nVerts; i++)
	{
		n1.x = 0;
		n1.y = 0;
		n1.z = 0;

		quad = mesh->gt4;

		for (int j = 0; j < mesh->ngt4; j++)
		{
			if (quad[0] == i || quad[1] == i || quad[2] == i || quad[3] == i)
			{
				n1.x += fnormals[j].x;
				n1.y += fnormals[j].y;
				n1.z += fnormals[j].z;
			}

			quad += 6;
		}

		tri = mesh->gt3;

		for (int j = 0; j < mesh->ngt3; j++)
		{
			if (tri[0] == i || tri[1] == i || tri[2] == i)
			{
				n1.x += fnormals[mesh->ngt4 + j].x;
				n1.y += fnormals[mesh->ngt4 + j].y;
				n1.z += fnormals[mesh->ngt4 + j].z;
			}

			tri += 5;
		}

		D3DNormalise(&n1);
		n1.x = 0;
		n1.y = 1.0F;
		n1.z = 0;
		v[i].nx = n1.x;
		v[i].ny = n1.y;
		v[i].nz = n1.z;
	}

	mesh->SourceVB->Unlock();
	free(fnormals);
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
	INJECT(0x0049A3D0, ProcessMeshData, replace);
	INJECT(0x0049CEB0, DrawRoomletBounds, replace);
	INJECT(0x0049B390, aBuildRoomletLights, replace);
	INJECT(0x0049AD90, aRoomInit, replace);
	INJECT(0x0049AD70, aResetFogBulbList, replace);
	INJECT(0x0049AEF0, TriggerFogBulbFX, replace);
	INJECT(0x0049B1C0, aBuildFXFogBulbList, replace);
	INJECT(0x0049D220, InitBuckets, replace);
	INJECT(0x0049D3B0, aSetBumpComponent, replace);
	INJECT(0x0049D420, aResetBumpComponent, replace);
	INJECT(0x0049D460, DrawBucket, replace);
	INJECT(0x0049D750, DrawBuckets, replace);
	INJECT(0x0049D250, FindBucket, replace);
	INJECT(0x0049C6B0, CheckBoundsClip, replace);
	INJECT(0x0049D0E0, PrelightVertsMMXByRoomlet, replace);
	INJECT(0x0049D130, PrelightVertsNonMMXByRoomlet, replace);
	INJECT(0x0049DB10, CalcTriFaceNormal, replace);
	INJECT(0x0049DBA0, CreateVertexNormals, replace);
}
