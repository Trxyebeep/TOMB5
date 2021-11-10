#include "../tomb5/pch.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "delstuff.h"
#include "deltapak.h"
#include "../specific/drawlara.h"
#include "health.h"
#include "objects.h"
#include "gameflow.h"
#include "sound.h"
#include "../specific/specificfx.h"
#include "../specific/drawroom.h"
#include "../specific/polyinsert.h"
#include "../specific/function_stubs.h"
#include "../specific/lighting.h"
#include "items.h"
#include "../specific/file.h"
#include "../specific/polyinsert.h"
#include "door.h"
#include "tomb4fx.h"
#include "../specific/d3dmatrix.h"
#include "../specific/profiler.h"
#include "rope.h"
#include "rat.h"
#include "bat.h"
#include "spider.h"
#include "twogun.h"
#include "mirror.h"
#include "../specific/alexstuff.h"
#include "lara_states.h"
#include "control.h"
#ifdef FOOTPRINTS
#include "footprnt.h"
#endif

short no_rotation[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

short* GetBoundsAccurate(ITEM_INFO* item)
{
	short* bptr;
	short* frmptr[2];
	long rate, frac;
	
	frac = GetFrames(item, frmptr, &rate);

	if (frac == 0)
		return frmptr[0];

	bptr = interpolated_bounds;

	for (int i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
		*bptr = (short)(*frmptr[0] + (*frmptr[1] - *frmptr[0]) * frac / rate);

	return interpolated_bounds;
}

short* GetBestFrame(ITEM_INFO* item)
{
	short* frm[2];
	long rate, ret;
	
	ret = GetFrames(item, frm, &rate);

	if (ret > (rate >> 1))
		return frm[1];
	else
		return frm[0];
}

void InitInterpolate(int frac, int rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = IMstack;
	aIMXPtr = aIFMStack;
	memcpy(IMstack, phd_mxptr, 48);
	memcpy(aIFMStack, aMXPtr, 48);
}

void phd_PopMatrix_I()
{
	phd_PopMatrix();
	IMptr -= 12;
	aIMXPtr -= 12;
}

void phd_PushMatrix_I()
{
	phd_PushMatrix();
	memcpy(IMptr + 12, IMptr, 48);
	memcpy(aIMXPtr + 12, aIMXPtr, 48);
	IMptr += 12;
	aIMXPtr += 12;
}

void phd_RotY_I(short ang)
{
	float* amxptr;
	long* mxptr;

	phd_RotY(ang);
	mxptr = phd_mxptr;
	amxptr = aMXPtr;
	phd_mxptr = IMptr;
	aMXPtr = aIMXPtr;
	phd_RotY(ang);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void phd_RotX_I(short ang)
{
	float* amxptr;
	long* mxptr;

	phd_RotX(ang);
	mxptr = phd_mxptr;
	amxptr = aMXPtr;
	phd_mxptr = IMptr;
	aMXPtr = aIMXPtr;
	phd_RotX(ang);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void phd_RotZ_I(short ang)
{
	float* amxptr;
	long* mxptr;

	phd_RotZ(ang);
	mxptr = phd_mxptr;
	amxptr = aMXPtr;
	phd_mxptr = IMptr;
	aMXPtr = aIMXPtr;
	phd_RotZ(ang);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void phd_TranslateRel_I(int x, int y, int z)
{
	float* amxptr;
	long* mxptr;

	phd_TranslateRel(x, y, z);
	mxptr = phd_mxptr;
	phd_mxptr = IMptr;
	amxptr = aMXPtr;
	aMXPtr = aIMXPtr;
	phd_TranslateRel(x, y, z);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void phd_TranslateRel_ID(int x, int y, int z, int x2, int y2, int z2)
{
	float* amxptr;
	long* mxptr;

	phd_TranslateRel(x, y, z);
	mxptr = phd_mxptr;
	amxptr = aMXPtr;
	phd_mxptr = IMptr;
	aMXPtr = aIMXPtr;
	phd_TranslateRel(x2, y2, z2);
	aMXPtr = amxptr;
	phd_mxptr = mxptr;
}

void phd_RotYXZ_I(short y, short x, short z)
{
	float* amxptr;
	long* mxptr;

	phd_RotYXZ(y, x, z);
	mxptr = phd_mxptr;
	phd_mxptr = IMptr;
	amxptr = aMXPtr;
	aMXPtr = aIMXPtr;
	phd_RotYXZ(y, x, z);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, int skip)
{
	float* amxptr;
	long* mxptr;

	gar_RotYXZsuperpack(pprot1, skip);
	mxptr = phd_mxptr;
	amxptr = aMXPtr;
	phd_mxptr = IMptr;
	aMXPtr = aIMXPtr;
	gar_RotYXZsuperpack(pprot2, skip);
	phd_mxptr = mxptr;
	aMXPtr = amxptr;
}

void gar_RotYXZsuperpack(short** pprot, int skip)
{
	ushort* prot;
//	long packed;

	while (skip)
	{
		prot = (ushort*)*pprot;

		if (*prot & (49152))
			*pprot += 1;
		else
			*pprot += 2;

		skip--;
	}

	prot = (ushort*)*pprot;

	if ((*prot >> 14))
	{
		if ((*prot >> 14) == 1)
			phd_RotX((short)((*prot & 4095) << 4));
		else if ((*prot >> 14) == 2)
			phd_RotY((short)((*prot & 4095) << 4));
		else
			phd_RotZ((short)((*prot & 4095) << 4));
	}
	else
	{
		phd_RotYXZpack(((ushort)**pprot << 16) + (ushort)(*pprot)[1]);
		*pprot += 2;
		return;
	}

	++*pprot;
}

void phd_PutPolygons_I(short* ptr, int clip)
{
	phd_PushMatrix();
	InterpolateMatrix();
	phd_PutPolygons(ptr, clip);
	phd_PopMatrix();
}

void aInterpolateMatrix()
{
	float* matrixp;
	float* iMatrixp;

	matrixp = aMXPtr;
	iMatrixp = aIMXPtr;

	if (IM_rate == 2)
	{
		matrixp[0] = (float)((iMatrixp[0] + matrixp[0]) * 0.5);
		matrixp[1] = (float)((iMatrixp[1] + matrixp[1]) * 0.5);
		matrixp[2] = (float)((iMatrixp[2] + matrixp[2]) * 0.5);
		matrixp[3] = (float)((iMatrixp[3] + matrixp[3]) * 0.5);
		matrixp[4] = (float)((iMatrixp[4] + matrixp[4]) * 0.5);
		matrixp[5] = (float)((iMatrixp[5] + matrixp[5]) * 0.5);
		matrixp[6] = (float)((iMatrixp[6] + matrixp[6]) * 0.5);
		matrixp[7] = (float)((iMatrixp[7] + matrixp[7]) * 0.5);
		matrixp[8] = (float)((iMatrixp[8] + matrixp[8]) * 0.5);
		matrixp[9] = (float)((iMatrixp[9] + matrixp[9]) * 0.5);
		matrixp[10] = (float)((iMatrixp[10] + matrixp[10]) * 0.5);
		matrixp[11] = (float)((iMatrixp[11] + matrixp[11]) * 0.5);
		return;
	}

	if (IM_frac == 2)
	{
		if (IM_rate != 4)
		{
			matrixp[0] = iMatrixp[0] - (float)((iMatrixp[0] - matrixp[0]) * 0.25);
			matrixp[1] = iMatrixp[1] - (float)((iMatrixp[1] - matrixp[1]) * 0.25);
			matrixp[2] = iMatrixp[2] - (float)((iMatrixp[2] - matrixp[2]) * 0.25);
			matrixp[3] = iMatrixp[3] - (float)((iMatrixp[3] - matrixp[3]) * 0.25);
			matrixp[4] = iMatrixp[4] - (float)((iMatrixp[4] - matrixp[4]) * 0.25);
			matrixp[5] = iMatrixp[5] - (float)((iMatrixp[5] - matrixp[5]) * 0.25);
			matrixp[6] = iMatrixp[6] - (float)((iMatrixp[6] - matrixp[6]) * 0.25);
			matrixp[7] = iMatrixp[7] - (float)((iMatrixp[7] - matrixp[7]) * 0.25);
			matrixp[8] = iMatrixp[8] - (float)((iMatrixp[8] - matrixp[8]) * 0.25);
			matrixp[9] = iMatrixp[9] - (float)((iMatrixp[9] - matrixp[9]) * 0.25);
			matrixp[10] = iMatrixp[10] - (float)((iMatrixp[10] - matrixp[10]) * 0.25);
			matrixp[11] = iMatrixp[11] - (float)((iMatrixp[11] - matrixp[11]) * 0.25);
			return;
		}

		matrixp[0] = (float)((iMatrixp[0] + matrixp[0]) * 0.5);
		matrixp[1] = (float)((iMatrixp[1] + matrixp[1]) * 0.5);
		matrixp[2] = (float)((iMatrixp[2] + matrixp[2]) * 0.5);
		matrixp[3] = (float)((iMatrixp[3] + matrixp[3]) * 0.5);
		matrixp[4] = (float)((iMatrixp[4] + matrixp[4]) * 0.5);
		matrixp[5] = (float)((iMatrixp[5] + matrixp[5]) * 0.5);
		matrixp[6] = (float)((iMatrixp[6] + matrixp[6]) * 0.5);
		matrixp[7] = (float)((iMatrixp[7] + matrixp[7]) * 0.5);
		matrixp[8] = (float)((iMatrixp[8] + matrixp[8]) * 0.5);
		matrixp[9] = (float)((iMatrixp[9] + matrixp[9]) * 0.5);
		matrixp[10] = (float)((iMatrixp[10] + matrixp[10]) * 0.5);
		matrixp[11] = (float)((iMatrixp[11] + matrixp[11]) * 0.5);
		return;
	}

	if (IM_frac != 1)
	{
		matrixp[0] = iMatrixp[0] - (float)((iMatrixp[0] - matrixp[0]) * 0.25);
		matrixp[1] = iMatrixp[1] - (float)((iMatrixp[1] - matrixp[1]) * 0.25);
		matrixp[2] = iMatrixp[2] - (float)((iMatrixp[2] - matrixp[2]) * 0.25);
		matrixp[3] = iMatrixp[3] - (float)((iMatrixp[3] - matrixp[3]) * 0.25);
		matrixp[4] = iMatrixp[4] - (float)((iMatrixp[4] - matrixp[4]) * 0.25);
		matrixp[5] = iMatrixp[5] - (float)((iMatrixp[5] - matrixp[5]) * 0.25);
		matrixp[6] = iMatrixp[6] - (float)((iMatrixp[6] - matrixp[6]) * 0.25);
		matrixp[7] = iMatrixp[7] - (float)((iMatrixp[7] - matrixp[7]) * 0.25);
		matrixp[8] = iMatrixp[8] - (float)((iMatrixp[8] - matrixp[8]) * 0.25);
		matrixp[9] = iMatrixp[9] - (float)((iMatrixp[9] - matrixp[9]) * 0.25);
		matrixp[10] = iMatrixp[10] - (float)((iMatrixp[10] - matrixp[10]) * 0.25);
		matrixp[11] = iMatrixp[11] - (float)((iMatrixp[11] - matrixp[11]) * 0.25);
		return;
	}

	matrixp[0] += (float)((iMatrixp[0] - matrixp[0]) * 0.25);
	matrixp[1] += (float)((iMatrixp[1] - matrixp[1]) * 0.25);
	matrixp[2] += (float)((iMatrixp[2] - matrixp[2]) * 0.25);
	matrixp[3] += (float)((iMatrixp[3] - matrixp[3]) * 0.25);
	matrixp[4] += (float)((iMatrixp[4] - matrixp[4]) * 0.25);
	matrixp[5] += (float)((iMatrixp[5] - matrixp[5]) * 0.25);
	matrixp[6] += (float)((iMatrixp[6] - matrixp[6]) * 0.25);
	matrixp[7] += (float)((iMatrixp[7] - matrixp[7]) * 0.25);
	matrixp[8] += (float)((iMatrixp[8] - matrixp[8]) * 0.25);
	matrixp[9] += (float)((iMatrixp[9] - matrixp[9]) * 0.25);
	matrixp[10] += (float)((iMatrixp[10] - matrixp[10]) * 0.25);
	matrixp[11] += (float)((iMatrixp[11] - matrixp[11]) * 0.25);
}

long DrawPhaseGame()
{
	CalcLaraMatrices(0);
	phd_PushUnitMatrix();
	CalcLaraMatrices(1);
	phd_PopMatrix();

	if (GLOBAL_playing_cutseq)
		frigup_lara();

	SetLaraUnderwaterNodes();
	DrawRooms(camera.pos.room_number);
	DrawGameInfo(1);
	S_OutputPolyList();
	camera.number_frames = S_DumpScreen();
	S_AnimateTextures(camera.number_frames);
	return camera.number_frames;
}

void SkyDrawPhase()
{
	if (outside)
	{
		if (!objects[HORIZON].loaded)
		{
			outside = -1;
			return;
		}

#ifdef IRELAND_SKIES
		if (gfCurrentLevel == LVL5_GALLOWS_TREE || gfCurrentLevel == LVL5_LABYRINTH || gfCurrentLevel == LVL5_OLD_MILL)
		{
			DrawMoon();
			DrawStarField();
		}
#endif

		if (BinocularRange)
			AlterFOV(14560 - (short)BinocularRange);

		phd_PushMatrix();
		phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);

		if (gfLevelFlags & GF_LIGHTNING)
		{
			if (!LightningCount && !LightningRand)
			{
				if (!(GetRandomDraw() & 127))
				{
					LightningCount = (GetRandomDraw() & 0x1F) + 16;
					dLightningRand = (GetRandomDraw() & 0xFF) + 256;
					LightningSFXDelay = (GetRandomDraw() & 3) + 12;
				}
			}
			else
			{
				UpdateSkyLightning();

				if (LightningSFXDelay > -1)
					LightningSFXDelay--;

				if (!LightningSFXDelay)
					SoundEffect(SFX_THUNDER_RUMBLE, 0, SFX_DEFAULT);
			}
		}

		nPolyType = 6;
		DrawBuckets();
		DrawSortList();
		phd_PushMatrix();

		if (gfLevelFlags & GF_LAYER1)
		{
			phd_RotY(32760);

			if (gfLevelFlags & GF_LIGHTNING)
				DrawFlatSky(RGBA(LightningRGB[0], LightningRGB[1], LightningRGB[2], 44), SkyPos, -1536, 4);
			else
				DrawFlatSky(*(ulong*)&gfLayer1Col, SkyPos, -1536, 4);
		}

		if (gfLevelFlags & GF_LAYER2)
			DrawFlatSky(0xFF000000 | *(ulong*)&gfLayer2Col, SkyPos2, -1536, 2);

		if (gfLevelFlags & GF_LAYER1 || gfLevelFlags & GF_LAYER2)
			OutputSky();

		phd_PopMatrix();

		if (gfLevelFlags & GF_HORIZON)
		{
			if (gfCurrentLevel == LVL5_TITLE && jobyfrigger)
				phd_PutPolygonSkyMesh(meshes[objects[CHEF_MIP].mesh_index], -1);
			else
				phd_PutPolygonSkyMesh(meshes[objects[HORIZON].mesh_index], -1);

			OutputSky();
		}

		phd_PopMatrix();

		if (BinocularRange)
			AlterFOV(7 * (2080 - (short)BinocularRange));
	}
}

void UpdateSkyLightning()
{
	if (LightningCount <= 0)
	{
		if (LightningRand < 4)
			LightningRand = 0;
		else
			LightningRand -= LightningRand >> 2;
	}
	else
	{
		LightningCount--;

		if (LightningCount)
		{
			dLightningRand = GetRandomDraw() & 0x1FF;
			LightningRand += (dLightningRand - LightningRand) >> 1;
		}
		else
		{
			dLightningRand = 0;
			LightningRand = (GetRandomDraw() & 0x7F) + 400;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		LightningRGB[i] = LightningRGBs[i] + ((LightningRGBs[i] * LightningRand) >> 8);

		if (LightningRGB[i] > 255)
			LightningRGB[i] = 255;
	}
}

void CalculateObjectLighting(ITEM_INFO* item, short* frame)
{
	long x, y, z;

	if (item->shade >= 0)
		S_CalculateStaticMeshLight(item->pos.x_pos,item->pos.y_pos,item->pos.z_pos, item->shade & 0x7FFF, &room[item->room_number]);
	else
	{
		phd_PushUnitMatrix();
		phd_SetTrans(0, 0, 0);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		phd_TranslateRel((frame[0] + frame[1]) >> 1, (frame[2] + frame[3]) >> 1, (frame[4] + frame[5]) >> 1);
		x = item->pos.x_pos + (phd_mxptr[3] >> 14);
		y = item->pos.y_pos + (phd_mxptr[7] >> 14);
		z = item->pos.z_pos + (phd_mxptr[11] >> 14);
		phd_PopMatrix();
		current_item = item;
		item->il.item_pos.x = x;
		item->il.item_pos.y = y;
		item->il.item_pos.z = z;
		CalcAmbientLight(item);
		CreateLightList(item);
	}
}

void DrawAnimatingItem(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* data;
	short* rot;
	short* rot2;
	long frac, rate, clip, bit;

	frac = GetFrames(item, frm, &rate);
	obj = &objects[item->object_number];
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	if (obj->object_mip && (obj + 1)->loaded && phd_mxptr[M23] >> 16 > obj->object_mip)
		obj++;

	calc_animating_item_clip_window(item, frm[0]);
	clip = S_GetObjectBounds(frm[0]);

	if (clip)
	{
		CalculateObjectLighting(item, frm[0]);

		if (!item->data)
			data = no_rotation;
		else
			data = (short*)item->data;

		bit = 1;
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];

		if (frac)
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
			rot = frm[0] + 9;
			rot2 = frm[1] + 9;
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (item->mesh_bits & 1)
			{
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons_I(meshpp[1], clip);
				else
					phd_PutPolygons_I(meshpp[0], clip);
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
			{
				if (bone[0] & 1)
					phd_PopMatrix_I();

				if (bone[0] & 2)
					phd_PushMatrix_I();

				phd_TranslateRel_I(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack_I(&rot, &rot2, 0);

				if (bone[0] & 0x1C)
				{
					if (bone[0] & 8)
						phd_RotY_I(*data++);

					if (bone[0] & 4)
						phd_RotX_I(*data++);

					if (bone[0] & 16)
						phd_RotZ_I(*data++);
				}

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					if (bit & item->meshswap_meshbits)
						phd_PutPolygons_I(meshpp[1], clip);
					else
						phd_PutPolygons_I(meshpp[0], clip);
				}
			}
		}
		else
		{
			phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
			rot = frm[0] + 9;
			gar_RotYXZsuperpack(&rot, 0);

			if (item->mesh_bits & 1)
			{
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons(meshpp[1], clip);
				else
					phd_PutPolygons(meshpp[0], clip);
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
			{
				if (bone[0] & 1)
					phd_PopMatrix();

				if (bone[0] & 2)
					phd_PushMatrix();

				phd_TranslateRel(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack(&rot, 0);

				if (bone[0] & 0x1C)
				{
					if (bone[0] & 8)
						phd_RotY(*data++);

					if (bone[0] & 4)
						phd_RotX(*data++);

					if (bone[0] & 16)
						phd_RotZ(*data++);
				}

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					if (bit & item->meshswap_meshbits)
						phd_PutPolygons(meshpp[1], clip);
					else
						phd_PutPolygons(meshpp[0], clip);
				}
			}
		}
	}

	phd_right = phd_winwidth;
	phd_left = 0;
	phd_top = 0;
	phd_bottom = phd_winheight;
	phd_PopMatrix();
}

void PrintObjects(short room_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	OBJECT_INFO* obj;

	CurrentRoom = room_number;
	nPolyType = 1;
	r = &room[room_number];
	r->bound_active = 0;
	DrawStaticObjects(room_number);
	phd_PushMatrix();
	phd_TranslateAbs(r->x, r->y, r->z);
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax + 1;
	phd_bottom = phd_winymax + 1;
	nPolyType = 2;

	for (int i = r->item_number; i != NO_ITEM; i = items[i].next_item)
	{
		GlobalRoomNumber = room_number;
		item = &items[i];
		obj = &objects[item->object_number];

		if (item->status != ITEM_INVISIBLE)
		{
			if (item->after_death)
				GlobalAlpha = 0xFE000000 * item->after_death;

			if (gfCurrentLevel != LVL5_BASE || item->object_number != BRIDGE_FLAT)
			{
				if (obj->draw_routine)
					obj->draw_routine(item);

				if (obj->draw_routine_extra)
					obj->draw_routine_extra(item);
			}

			GlobalAlpha = 0xFF000000;
		}

		if (item->after_death < 128 && item->after_death > 0 && !(wibble & 3))
			item->after_death++;

		if (item->after_death == 128)
			KillItem(i);
	}

	nPolyType = 3;

	for (int i = r->fx_number; i != NO_ITEM; i = effects[i].next_fx)
		DrawEffect(i);

	phd_PopMatrix();
	r->left = phd_winxmax;
	r->top = phd_winymax;
	r->right = 0;
	r->bottom = 0;
}

void DrawRooms(short current_room)
{
	ROOM_INFO* r;
	long lx, ly, lz;
	short lr;

	DoMonitorScreen();
	CurrentRoom = current_room;
	r = &room[current_room];
	r->test_left = 0;
	r->test_top = 0;
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	r->test_right = phd_winxmax;
	r->test_bottom = phd_winymax;
	outside = r->flags & ROOM_OUTSIDE;
	snow_outside = 0;
	camera_underwater = r->flags & ROOM_UNDERWATER;
	r->bound_active = 2;
	draw_room_list[0] = current_room;
	room_list_start = 0;
	room_list_end = 1;
	number_draw_rooms = 0;

	if (outside)
	{
		outside_top = 0;
		outside_left = 0;
		outside_right = phd_winxmax;
		outside_bottom = phd_winymax;
	}
	else
	{
		outside_top = phd_winymax;
		outside_left = phd_winxmax;
		outside_right = 0;
		outside_bottom = 0;
	}

	GetRoomBounds();
	InitialiseFogBulbs();
	CreateFXBulbs();
	ProcessClosedDoors();

	if (gfCurrentLevel)
		SkyDrawPhase();

	if (objects[LARA].loaded)
	{
		if (!(lara_item->flags & IFL_INVISIBLE))
		{
			nPolyType = 4;

			if (lara_item->mesh_bits && !SCNoDrawLara)
			{
				if (lara.skelebob)
					SetupSkelebobMeshswaps();

				DrawLara(lara_item, 0);

				if (lara.skelebob)
					RestoreLaraMeshswaps();

				phd_PushMatrix();

				if (lara.right_arm.flash_gun)
				{
					aMXPtr[M00] = lara_matricesF[132 + M00];
					aMXPtr[M01] = lara_matricesF[132 + M01];
					aMXPtr[M02] = lara_matricesF[132 + M02];
					aMXPtr[M03] = lara_matricesF[132 + M03];
					aMXPtr[M10] = lara_matricesF[132 + M10];
					aMXPtr[M11] = lara_matricesF[132 + M11];
					aMXPtr[M12] = lara_matricesF[132 + M12];
					aMXPtr[M13] = lara_matricesF[132 + M13];
					aMXPtr[M20] = lara_matricesF[132 + M20];
					aMXPtr[M21] = lara_matricesF[132 + M21];
					aMXPtr[M22] = lara_matricesF[132 + M22];
					aMXPtr[M23] = lara_matricesF[132 + M23];
					SetGunFlash(lara.gun_type);
				}

				if (lara.left_arm.flash_gun)
				{
					aMXPtr[M00] = lara_matricesF[168 + M00];
					aMXPtr[M01] = lara_matricesF[168 + M01];
					aMXPtr[M02] = lara_matricesF[168 + M02];
					aMXPtr[M03] = lara_matricesF[168 + M03];
					aMXPtr[M10] = lara_matricesF[168 + M10];
					aMXPtr[M11] = lara_matricesF[168 + M11];
					aMXPtr[M12] = lara_matricesF[168 + M12];
					aMXPtr[M13] = lara_matricesF[168 + M13];
					aMXPtr[M20] = lara_matricesF[168 + M20];
					aMXPtr[M21] = lara_matricesF[168 + M21];
					aMXPtr[M22] = lara_matricesF[168 + M22];
					aMXPtr[M23] = lara_matricesF[168 + M23];
					SetGunFlash(lara.gun_type);
				}

				phd_PopMatrix();
				DrawGunflashes();
			}

			if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
				Draw_Mirror_Lara();
		}
	}

	InitDynamicLighting_noparams();
	nPolyType = 0;

	for (int i = 0; i < 32; i++)
	{
		if (dynamics[i].on)
		{
			if (dynamics[i].z < 0)
				dynamics[i].z = 0;

			if (dynamics[i].x < 0)
				dynamics[i].x = 0;
		}
	}

	phd_PushMatrix();
	phd_TranslateAbs(0, 0, 0);
	SaveD3DCameraMatrix();
	phd_PopMatrix();
	aResetFogBulbList();
	RoomTestThing();
	aBuildFogBulbList();
	aBuildFXFogBulbList();
	mAddProfilerEvent();

	for (int i = 0; i < number_draw_rooms; i++)
	{
		r = &room[draw_rooms[i]];
		phd_PushMatrix();
		phd_TranslateAbs(r->x, r->y, r->z);
		CurrentRoom = draw_rooms[i];
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;
		aSetViewMatrix();
		InsertRoom(r);
		phd_PopMatrix();
	}

	mAddProfilerEvent();
	DrawGunshells();
	nPolyType = 3;

	if (GLOBAL_playing_cutseq)
		DrawCutSeqActors();

	DrawRopeList();
	S_DrawSparks();
	DrawRats();
	DrawBats();
	DrawSpiders();
	lx = lara_item->pos.x_pos;
	ly = lara_item->pos.y_pos;
	lz = lara_item->pos.z_pos;
	lr = lara_item->room_number;
	lara_item->pos.x_pos = camera.pos.x;
	lara_item->pos.y_pos = camera.pos.y;
	lara_item->pos.z_pos = camera.pos.z;
	lara_item->room_number = camera.pos.room_number;
	DoWeather();
	mAddProfilerEvent();
	DoUwEffect();
	S_DrawFires();
	S_DrawSmokeSparks();
	S_DrawSplashes();
	DrawBubbles();
	DrawDebris();
	DrawBlood();
	DrawDrips();
	DrawShockwaves();
	DrawLightning();
	DrawTwogunLasers();
#ifdef FOOTPRINTS
	S_DrawFootPrints();
#endif
	lara_item->pos.x_pos = lx;
	lara_item->pos.y_pos = ly;
	lara_item->pos.z_pos = lz;
	lara_item->room_number = lr;
	mAddProfilerEvent();

	if (gfLevelFlags & GF_LENSFLARE)
		SetUpLensFlare(gfLensFlare.x, gfLensFlare.y - 4096, gfLensFlare.z, 0);

	if (LaserSightActive)
		DrawLaserSightSprite();

	for (int i = 0; i < number_draw_rooms; i++)
		PrintObjects(draw_rooms[i]);

	aUpdate();
}

void CalculateObjectLightingLara()
{
	PHD_VECTOR pos;
	short room_no;

	if (GLOBAL_playing_cutseq)
		CalculateObjectLightingLaraCutSeq();
	else
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;

		if (lara_item->anim_number == ANIM_DUCKBREATHE || lara_item->anim_number == ANIM_ALL4S || lara_item->anim_number == ANIM_BREATH)
		{
			pos.x = lara_item->pos.x_pos;

			if (lara_item->anim_number == ANIM_BREATH)
				pos.y = lara_item->pos.y_pos - 512;
			else
				pos.y = lara_item->pos.y_pos - 192;

			pos.z = lara_item->pos.z_pos;
			room_no = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_no);
		}
		else
		{
			GetLaraJointPos(&pos, 7);
			room_no = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_no);
		}

		current_item = lara_item;
		lara_item->il.item_pos.x = pos.x;
		lara_item->il.item_pos.y = pos.y;
		lara_item->il.item_pos.z = pos.z;
		CalcAmbientLight(lara_item);
		CreateLightList(lara_item);
	}
}

void inject_draw(bool replace)
{
	INJECT(0x0042CF80, GetBoundsAccurate, replace);
	INJECT(0x0042D020, GetBestFrame, replace);
	INJECT(0x0042BE90, InitInterpolate, replace);
	INJECT(0x0042BF00, phd_PopMatrix_I, replace);
	INJECT(0x0042BF50, phd_PushMatrix_I, replace);
	INJECT(0x0042BFC0, phd_RotY_I, replace);
	INJECT(0x0042C030, phd_RotX_I, replace);
	INJECT(0x0042C0A0, phd_RotZ_I, replace);
	INJECT(0x0042C110, phd_TranslateRel_I, replace);
	INJECT(0x0042C190, phd_TranslateRel_ID, replace);
	INJECT(0x0042C210, phd_RotYXZ_I, replace);
	INJECT(0x0042C290, gar_RotYXZsuperpack_I, replace);
	INJECT(0x0042C310, gar_RotYXZsuperpack, replace);
	INJECT(0x0042C3F0, phd_PutPolygons_I, replace);
	INJECT(0x0042C440, aInterpolateMatrix, replace);
	INJECT(0x0042A400, DrawPhaseGame, replace);
	INJECT(0x0042A4A0, SkyDrawPhase, replace);
	INJECT(0x0042A310, UpdateSkyLightning, replace);
	INJECT(0x0042CD50, CalculateObjectLighting, replace);
	INJECT(0x0042B900, DrawAnimatingItem, replace);
	INJECT(0x0042D290, PrintObjects, replace);
	INJECT(0x0042A7A0, DrawRooms, replace);
	INJECT(0x0042A1B0, CalculateObjectLightingLara, replace);
}
