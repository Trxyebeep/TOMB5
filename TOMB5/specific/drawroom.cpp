#include "../tomb5/pch.h"
#include "drawroom.h"
#include "function_table.h"
#include "function_stubs.h"
#include "d3dmatrix.h"
#include "alexstuff.h"
#include "dxshell.h"
#include "polyinsert.h"
#include "output.h"
#include "winmain.h"
#include "gamemain.h"
#include "texture.h"
#include "file.h"
#include "../game/camera.h"
#include "../game/control.h"
#include "3dmath.h"
#include "../game/effect2.h"
#include "../game/gameflow.h"
#include "../game/objects.h"
#include "../tomb5/tomb5.h"

FOGBULB_INFO LevelFogBulbs[64];		//list of all fogbulbs in the level (copied from room data)
FOGBULB_STRUCT ActiveFogBulbs[64];	//list of active fog bulbs
FOGBULB_STRUCT FXFogBulbs[4];		//list of fx fog bulbs
FOGBULB_STRUCT FogBulbs[16];		//to apply on object vertices	
FOGBULB_STRUCT RoomletFogBulbs[16];	//to apply on roomlet vertices
short CheckClipBox[8 * 3] = { 0, 1, 2, 3, 1, 2, 0, 1, 5, 3, 1, 5, 0, 4, 2, 3, 4, 2, 0, 4, 5, 3, 4, 5 };

MESH_DATA** mesh_vtxbuf;
TEXTUREBUCKET Bucket[30];
float clip_left;
float clip_top;
float clip_right;
float clip_bottom;

long NumActiveFogBulbs;
long num_level_meshes;

static ROOMLET_LIGHT RoomletLights[64];
static long nRoomletFogBulbs;
static long nFXFogBulbs;
static long NumLevelFogBulbs;
static float rClipTop;
static float rClipBottom;
static float rClipLeft;
static float rClipRight;
static ROOM_INFO* CurrentRoomPtr;
static long CurrentRoomUnderwater;
static bool has_water_neighbor;

void InsertRoom(ROOM_INFO* r)
{
	clip_left = (float)r->left;
	clip_top = (float)r->top;
	clip_right = (float)r->right;
	clip_bottom = (float)r->bottom;
	rClipTop = clip_top;
	rClipRight = clip_right;
	rClipLeft = clip_left;
	rClipBottom = clip_bottom;

	if (r->nVerts)
	{
		CurrentRoomPtr = r;
		CurrentRoomUnderwater = r->flags & ROOM_UNDERWATER;

		for (int i = 0; i < r->nRoomlets; i++)
		{
			if (CheckBoundsClip(r->pRoomlets[i].bBox))
				InsertRoomlet(&r->pRoomlets[i]);
		}
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

static bool IsMistVert(FVECTOR* v)
{
	ROOM_PORTAL* p;
	ROOM_INFO* r;
	ROOMLET* rlet;
	float* verts;
	short* door;
	float x, y, z;
	short drn;

	door = CurrentRoomPtr->door;

	if (!door)
		return 0;

	x = v->x + CurrentRoomPtr->x;
	y = v->y;
	z = v->z + CurrentRoomPtr->z;
	drn = *door++;

	for (p = (ROOM_PORTAL*)door; drn > 0; drn--, p++)
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
	ROOM_PORTAL* p;
	ROOM_PORTAL* np;
	ROOM_INFO* r;
	ROOM_INFO* nr;
	ROOMLET* rlet;
	float* verts;
	short* door;
	short* ndoor;
	float x, y, z;
	short drn, ndrn, cont;

	has_water_neighbor = 0;
	door = CurrentRoomPtr->door;

	if (!door)
		return 0;

	cont = 0;
	x = v->x + CurrentRoomPtr->x;
	y = v->y;
	z = v->z + CurrentRoomPtr->z;
	drn = *door++;

	for (p = (ROOM_PORTAL*)door; drn > 0; drn--, p++)
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
		door = CurrentRoomPtr->door;

		if (door)
		{
			drn = *door++;

			for (p = (ROOM_PORTAL*)door; drn > 0; drn--, p++)
			{
				r = &room[p->rn];

				ndoor = r->door;
				ndrn = *ndoor++;

				for (np = (ROOM_PORTAL*)ndoor; ndrn > 0; ndrn--, np++)
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
	ROOM_PORTAL* p;
	ROOM_INFO* r;
	ROOMLET* rlet;
	float* verts;
	short* door;
	float x, y, z;
	short drn;

	door = CurrentRoomPtr->door;

	if (!door)
		return 0;

	x = v->x + CurrentRoomPtr->x;
	y = v->y;
	z = v->z + CurrentRoomPtr->z;
	drn = *door++;

	for (p = (ROOM_PORTAL*)door; drn > 0; drn--, p++)
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

void aRoomletTransformLight(float* verts, long nVerts, long nLights, long nWaterVerts, long nShoreVerts)
{
	ROOMLET_LIGHT* light;
	FOGBULB_STRUCT* bulb;
	ROOMLET_VERTEX* pVtx;
	FVECTOR pos;
	FVECTOR n;
	FVECTOR lPos;
	FVECTOR stash;
	FVECTOR vec;
	FVECTOR vec2;
	short* clip;
	static float DistanceFogStart;
	static float iDistanceFogStart;
	float zv, zv2, fR, fG, fB, val, val2, val3, intensity;
	long x, y, z, col, cR, cG, cB, sR, sG, sB;
	short clipFlag;
	uchar flags, rnd, absval;
	char choppy, shimmer;

	pVtx = (ROOMLET_VERTEX*)verts;
	clip = clipflags;
	DistanceFogStart = tomb5.distance_fog * 1024.0F;
	iDistanceFogStart = 1.0F / DistanceFogStart;

	for (int i = 0; i < nVerts; i++)
	{
		pos.x = pVtx->x;
		pos.y = pVtx->y;
		pos.z = pVtx->z;
		n.x = pVtx->nx;
		n.y = pVtx->ny;
		n.z = pVtx->nz;

		if (i < nWaterVerts)
		{
			x = long(pos.x + CurrentRoomPtr->x) >> 6;
			y = long(pos.y + CurrentRoomPtr->y) >> 6;
			z = long(pos.z + CurrentRoomPtr->z) >> 7;
			rnd = WaterTable[CurrentRoomPtr->MeshEffect][(x + y + z) & 0x3F].random;
			pos.y += WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
		}

		vec.x = pos.x * D3DMView._11 + pos.y * D3DMView._21 + pos.z * D3DMView._31 + D3DMView._41;
		vec.y = pos.x * D3DMView._12 + pos.y * D3DMView._22 + pos.z * D3DMView._32 + D3DMView._42;
		vec.z = pos.x * D3DMView._13 + pos.y * D3DMView._23 + pos.z * D3DMView._33 + D3DMView._43;

		//Stash transformed unprojected pos for fog calculation
		stash.x = vec.x;
		stash.y = vec.y;
		stash.z = vec.z;

		aVertexBuffer[i].tu = vec.x;
		aVertexBuffer[i].tv = vec.y;
		clipFlag = 0;

		if (vec.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vec.z;

			if (camera.underwater)
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

		*clip++ = clipFlag;
		aVertexBuffer[i].sx = vec.x;
		aVertexBuffer[i].sy = vec.y;
		aVertexBuffer[i].sz = vec.z;
		col = pVtx->prelight;
		fR = 0;
		fG = 0;
		fB = 0;
		sR = 0;
		sG = 0;
		sB = 0;

		for (int j = 0; j < nLights; j++)
		{
			light = &RoomletLights[j];
			lPos.x = pos.x - light->x;
			lPos.y = pos.y - light->y;
			lPos.z = pos.z - light->z;
			val = SQUARE(lPos.x) + SQUARE(lPos.y) + SQUARE(lPos.z);

			if (val < light->sqr_falloff)
			{
				val = sqrt(val);
				val2 = light->inv_falloff * (light->falloff - val);
				lPos.x = (n.x * D3DMView._11 + n.y * D3DMView._21 + n.z * D3DMView._31) * (1.0F / val * lPos.x);
				lPos.y = (n.x * D3DMView._12 + n.y * D3DMView._22 + n.z * D3DMView._32) * (1.0F / val * lPos.y);
				lPos.z = (n.x * D3DMView._13 + n.y * D3DMView._23 + n.z * D3DMView._33) * (1.0F / val * lPos.z);
				val = val2 * (1.0F - (lPos.x + lPos.y + lPos.z));
				fR += light->r * val;
				fG += light->g * val;
				fB += light->b * val;
			}
		}

		fR = CLRR(col) + fR * 255.0F;
		fG = CLRG(col) + fG * 255.0F;
		fB = CLRB(col) + fB * 255.0F;
		cR = (long)fR;
		cG = (long)fG;
		cB = (long)fB;

		flags = 0;

		if (tomb5.shimmer)
		{
			if (CurrentRoomPtr->flags & ROOM_REFLECT && CurrentRoomPtr->flags & ROOM_REFLECT_CEILING)
			{
				if (gfCurrentLevel == LVL5_RED_ALERT)
				{
					if (pos.y != CurrentRoomPtr->minfloor)
						flags |= 2;
				}
				else if (!IsMistVert(&pos) || abs(pos.y - CurrentRoomPtr->minfloor) > 1536)
					flags |= 1;
			}
			else if (pos.y == CurrentRoomPtr->minfloor)
			{
				if (CurrentRoomPtr->flags & ROOM_MIST)
				{
					if (IsMistVert(&pos))
						flags |= 1;
				}
				else if (CurrentRoomPtr->flags & ROOM_REFLECT)
				{
					if (IsReflectionVert(&pos))
						flags |= 1;
					else if (IsShoreVert(&pos) || has_water_neighbor)
						flags |= 1;
				}
			}
		}

		if (CurrentRoomUnderwater)
		{
			x = long(pos.x + CurrentRoomPtr->x) >> 6;
			y = long(pos.y + CurrentRoomPtr->y) >> 6;
			z = long(pos.z + CurrentRoomPtr->z) >> 7;
			rnd = WaterTable[CurrentRoomPtr->MeshEffect][(x + y + z) & 0x3F].random;
			choppy = WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
			col = -2 * choppy;
			cR += col;
			cG += col;
			cB += col;
		}
		else if (flags & 1)
		{
			x = long(pos.x + CurrentRoomPtr->x) >> 6;
			y = long(pos.y + CurrentRoomPtr->y) >> 6;
			z = long(pos.z + CurrentRoomPtr->z) >> 7;
			rnd = WaterTable[CurrentRoomPtr->MeshEffect][(x + y + z) & 0x3F].random;
			shimmer = WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer;
			absval = WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs;
			col = (shimmer + absval) << 3;
			cR += col;
			cG += col;
			cB += col;
		}
		else if (flags & 2)	//special Red Alert! gas rooms wibble (slower and green only)
		{
			x = long(pos.x + CurrentRoomPtr->x) >> 6;
			y = long(pos.y + CurrentRoomPtr->y) >> 6;
			z = long(pos.z + CurrentRoomPtr->z) >> 7;
			rnd = WaterTable[CurrentRoomPtr->MeshEffect][(x + y + z) & 0x3F].random;
			shimmer = WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 3) + rnd) & 0x3F].shimmer;
			absval = WaterTable[CurrentRoomPtr->MeshEffect][((wibble >> 3) + rnd) & 0x3F].abs;
			col = (shimmer + absval) << 3;
			cG += abs(col);
		}

		if (stash.z > DistanceFogStart)
		{
			val = (stash.z - DistanceFogStart) * (iDistanceFogStart * 255.0F);
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
			col = 0;

			for (int j = 0; j < nRoomletFogBulbs; j++)
			{
				bulb = &RoomletFogBulbs[j];
				intensity = 0;

				if (stash.z + bulb->rad > 0 && abs(stash.x) - bulb->rad < abs(stash.z) && abs(stash.y) - bulb->rad < abs(stash.z))
				{
					vec.x = 0;
					vec.y = 0;
					vec.z = 0;
					vec2.x = 0;
					vec2.y = 0;
					vec2.z = 0;
					val = SQUARE(bulb->pos.x - stash.x) + SQUARE(bulb->pos.y - stash.y) + SQUARE(bulb->pos.z - stash.z);

					if (bulb->sqlen < bulb->sqrad)
					{
						if (val < bulb->sqrad)
						{
							vec2.x = stash.x;
							vec2.y = stash.y;
							vec2.z = stash.z;
						}
						else
						{
							val = 1.0F / sqrt(SQUARE(stash.x) + SQUARE(stash.y) + SQUARE(stash.z));
							vec2.x = val * stash.x;
							vec2.y = val * stash.y;
							vec2.z = val * stash.z;
							val2 = bulb->pos.x * vec2.x + bulb->pos.y * vec2.y + bulb->pos.z * vec2.z;
							val = val2 + sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
							vec2.x *= val;
							vec2.y *= val;
							vec2.z *= val;
						}
					}
					else if (val < bulb->sqrad)
					{
						vec2.x = stash.x;
						vec2.y = stash.y;
						vec2.z = stash.z;
						val = 1.0F / sqrt(SQUARE(stash.x) + SQUARE(stash.y) + SQUARE(stash.z));
						vec.x = val * stash.x;
						vec.y = val * stash.y;
						vec.z = val * stash.z;
						val2 = bulb->pos.x * vec.x + bulb->pos.y * vec.y + bulb->pos.z * vec.z;
						val = val2 - sqrt(bulb->sqrad - (bulb->sqlen - SQUARE(val2)));
						vec.x *= val;
						vec.y *= val;
						vec.z *= val;
					}
					else
					{
						val = SQUARE(stash.x) + SQUARE(stash.y) + SQUARE(stash.z);
						val2 = 1.0F / sqrt(val);
						vec.x = val2 * stash.x;
						vec.y = val2 * stash.y;
						vec.z = val2 * stash.z;
						val2 = bulb->pos.x * vec.x + bulb->pos.y * vec.y + bulb->pos.z * vec.z;

						if (val2 > 0)
						{
							val3 = SQUARE(val2);

							if (val > val3)
							{
								val = bulb->sqlen - val3;

								if (val < bulb->sqrad)
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
								else
								{
									vec.x = 0;
									vec.y = 0;
									vec.z = 0;
									vec2.x = 0;
									vec2.y = 0;
									vec2.z = 0;
								}
							}
						}
					}

					intensity = sqrt(SQUARE(vec2.x - vec.x) + SQUARE(vec2.y - vec.y) + SQUARE(vec2.z - vec.z)) * bulb->d;
				}

				if (intensity)
				{
					col += (long)intensity;
					sR += long(intensity * bulb->r);
					sG += long(intensity * bulb->g);
					sB += long(intensity * bulb->b);
				}
			}

			cR -= col;
			cG -= col;
			cB -= col;
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
		pVtx++;
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
			vec2.x = vec.x - CamPos.x;
			vec2.y = vec.y - CamPos.y;
			vec2.z = vec.z - CamPos.z;

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
	short* mesh_ptr;
	short* last_mesh_ptr;
	short* gtx;
	float minx, miny, minz, maxx, maxy, maxz;
	long num, c;
	bool hand;

	Log(__FUNCTION__ " %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)game_malloc(4 * num_meshes);
	mesh_base = (short*)malloc_ptr;
	last_mesh_ptr = 0;
	data = 0;

	for (int i = 0; i < num_meshes; i++)
	{
		mesh_ptr = meshes[i];
		hand = i == objects[LARA_SKIN].mesh_index + LM_LHAND * 2 && gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL;

		if (mesh_ptr == last_mesh_ptr)
		{
			meshes[i] = (short*)data;
			mesh_vtxbuf[i] = data;
			continue;
		}

		last_mesh_ptr = meshes[i];
		minx = 20000.0F;
		miny = 20000.0F;
		minz = 20000.0F;
		maxx = -20000.0F;
		maxy = -20000.0F;
		maxz = -20000.0F;
		data = (MESH_DATA*)game_malloc(sizeof(MESH_DATA));
		memset(data, 0, sizeof(MESH_DATA));
		meshes[i] = (short*)data;
		mesh_vtxbuf[i] = data;
		data->x = *mesh_ptr++;
		data->y = *mesh_ptr++;
		data->z = *mesh_ptr++;
		data->r = *mesh_ptr++;
		data->flags = *mesh_ptr++;
		data->nVerts = *mesh_ptr & 0xFF;

		if (!data->nVerts)
			num = *mesh_ptr >> 8;

		mesh_ptr++;
		data->aVtx = (ACMESHVERTEX*)game_malloc(sizeof(ACMESHVERTEX) * data->nVerts);

		if (data->nVerts)
		{
			for (int j = 0; j < data->nVerts; j++)
			{
				vec.x = *mesh_ptr++;
				vec.y = *mesh_ptr++;
				vec.z = *mesh_ptr++;
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
			data->nNorms = *mesh_ptr++;

			if (!data->nNorms)
				data->nNorms = data->nVerts;

			if (data->nNorms <= 0)
			{
				data->Normals = 0;
				data->prelight = (long*)game_malloc(4 * data->nVerts);

				for (int j = 0; j < data->nVerts; j++)
				{
					c = 255 - (*mesh_ptr++ >> 5);
					data->prelight[j] = RGBONLY(c, c, c);
					data->aVtx[j].prelight = RGBONLY(c, c, c);
				}

				data->aFlags |= 2;
			}
			else
			{
				data->Normals = (D3DVECTOR*)game_malloc(sizeof(D3DVECTOR) * data->nNorms);

				for (int j = 0; j < data->nVerts; j++)
				{
					d3dvec.x = *mesh_ptr++;
					d3dvec.y = *mesh_ptr++;
					d3dvec.z = *mesh_ptr++;
					D3DNormalise(&d3dvec);
					data->aVtx[j].nx = d3dvec.x;
					data->aVtx[j].ny = d3dvec.y;
					data->aVtx[j].nz = d3dvec.z;
				}

				data->prelight = 0;
			}
		}
		else
			mesh_ptr += (6 * num) + 1;

		data->ngt4 = *mesh_ptr++;

		if (data->ngt4)
		{
			data->gt4 = (short*)game_malloc(12 * data->ngt4);
			memcpy(data->gt4, mesh_ptr, 12 * data->ngt4);
			mesh_ptr += 6 * data->ngt4;
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

		data->ngt3 = *mesh_ptr++;

		if (data->ngt3)
		{
			data->gt3 = (short*)game_malloc(10 * data->ngt3);
			memcpy(data->gt3, mesh_ptr, 10 * data->ngt3);

			if (hand)
			{
				gtx = &data->gt3[30 * 5];
				gtx[0] = 3;
				gtx[1] = 4;
			}

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

	Log("End " __FUNCTION__);
}

long aBuildRoomletLights(ROOMLET* r)
{
	ROOMLET_LIGHT* light;
	DYNAMIC* dynamic;
	FOGBULB_STRUCT* fogbulb;
	FOGBULB_STRUCT* rFog;
	FVECTOR dPos;
	float falloff;
	long numLights;

	light = RoomletLights;
	numLights = 0;

	for (int i = 0; i < 32; i++)
	{
		dynamic = &dynamics[i];

		if (!dynamic->on)
			continue;

		dPos.x = dynamic->x - CurrentRoomPtr->posx;
		dPos.y = dynamic->y - CurrentRoomPtr->posy;
		dPos.z = dynamic->z - CurrentRoomPtr->posz;
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

			if (gfCurrentLevel == LVL5_TRAJAN_MARKETS || gfCurrentLevel == LVL5_COLOSSEUM)
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

void DrawBucket(TEXTUREBUCKET* bucket)
{
	if (bucket->tpage == 1)
		bucket->tpage = 1;

	if (!bucket->nVtx)
		return;

	if (Textures[bucket->tpage].bump && App.BumpMapping)
	{
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
	}

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

	if (App.BumpMapping)
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

	bucket->nVtx = 0;
	bucket->tpage = -1;
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
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[Textures[bucket->tpage].bumptpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
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

	if (left > rClipRight || right < rClipLeft || top > rClipBottom || bottom < rClipTop)
		return 0;

	return 1;
}
