#include "../tomb5/pch.h"
#include "hair.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "gameflow.h"
#include "objects.h"
#include "draw.h"
#include "lara_states.h"
#include "control.h"

void DrawHair()
{
	HAIR_STRUCT* hair;
	short** meshpp;
	int ii;

	for (int i = 0; i < 2; i++)
	{
		ii = i * 6;
		meshpp = &meshes[objects[HAIR].mesh_index];
		meshpp += 2;

		hair = &hairs[i][1];

		for (int j = 1; j < 6; j += 2, meshpp += 4, hair += 2)
		{
			phd_PushMatrix();
			phd_TranslateAbs(hair->pos.x_pos, hair->pos.y_pos, hair->pos.z_pos);
			phd_RotY(hair->pos.y_rot);
			phd_RotX(hair->pos.x_rot);
			phd_PutPolygons(*meshpp, -1);

			if (j == 5)
				StashSkinVertices(33 + ii);
			else
			{
				StashSkinVertices(28 + ii + j);
				StashSkinVertices(29 + ii + j);
			}

			phd_PopMatrix();
		}

		meshpp = &meshes[objects[HAIR].mesh_index];

		for (int j = 0; j < 6; j += 2, meshpp += 4)
		{
			SkinVerticesToScratch(28 + ii + j);
			GetCorrectStashPoints(i, j, 29 + ii + j);
			SkinVerticesToScratch(29 + ii + j);
			phd_PutPolygons(*meshpp, -1);
		}

		if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA))
			break;
	}
}

void HairControl(int in_cutscene, int pigtail, short* cutscenething)
{
	OBJECT_INFO* obj;
	HAIR_STRUCT* hair;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	SPHERE sphere[5];
	long* bone;
	short* rot;
	short* objptr;
	short* frame;
	long water, height, size, dist, x, y, z, dx, dy, dz;
	short room_num, jerk;

	obj = &objects[LARA];

	if (!cutscenething)
	{
		if (lara.hit_direction < 0)
			frame = GetBestFrame(lara_item);
		else
		{
			if (lara.hit_direction == 0)
			{
				if (lara.IsDucked)
					jerk = ANIM_JERK_DUCKF;
				else
					jerk = ANIM_JERK_FORWARD;
			}
			else if (lara.hit_direction == 1)
			{
				if (lara.IsDucked)
					jerk = ANIM_JERK_DUCKR;
				else
					jerk = ANIM_JERK_RIGHT;
			}
			else if (lara.hit_direction == 2)
			{
				if (lara.IsDucked)
					jerk = ANIM_JERK_DUCKB;
				else
					jerk = ANIM_JERK_BACK;
			}
			else
			{
				if (lara.IsDucked)
					jerk = ANIM_JERK_DUCKL;
				else
					jerk = ANIM_JERK_LEFT;
			}

			frame = anims[jerk].frame_ptr;
			size = anims[jerk].interpolation >> 8;
			frame += lara.hit_frame * size;
		}
	}
	else
		frame = cutscenething;

	//fucking shit FUCK SPHERES
	phd_PushUnitMatrix();
	phd_SetTrans(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);
	rot = frame + 9;
	bone = &bones[obj->bone_index];
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);

	phd_PushMatrix();
	objptr = lara.mesh_ptrs[0];//butt
	phd_TranslateRel(*objptr, objptr[1], objptr[2]);
	sphere[0].x = phd_mxptr[3] >> 14;
	sphere[0].y = phd_mxptr[7] >> 14;
	sphere[0].z = phd_mxptr[11] >> 14;
	sphere[0].r = objptr[3];
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);
	gar_RotYXZsuperpack(&rot, 6);
	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);

	phd_PushMatrix();
	objptr = lara.mesh_ptrs[7];//torso
	phd_TranslateRel(*objptr, objptr[1], objptr[2]);
	sphere[1].x = phd_mxptr[3] >> 14;
	sphere[1].y = phd_mxptr[7] >> 14;
	sphere[1].z = phd_mxptr[11] >> 14;
	sphere[1].r = objptr[3];

	if (gfLevelFlags & GF_LVOP_YOUNG_LARA)
		sphere[1].r -= (sphere[1].r >> 2) + (sphere[1].r >> 3);

	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[29], bone[30], bone[31]);
	gar_RotYXZsuperpack(&rot, 0);
	objptr = lara.mesh_ptrs[8];//right arm
	phd_TranslateRel(*objptr, objptr[1], objptr[2]);
	sphere[3].x = phd_mxptr[3] >> 14;
	sphere[3].y = phd_mxptr[7] >> 14;
	sphere[3].z = phd_mxptr[11] >> 14;
	sphere[3].r = 3 * objptr[3] / 2;
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[41], bone[42], bone[43]);
	gar_RotYXZsuperpack(&rot, 2);
	objptr = lara.mesh_ptrs[11];//left arm
	phd_TranslateRel(*objptr, objptr[1], objptr[2]);
	sphere[4].x = phd_mxptr[3] >> 14;
	sphere[4].y = phd_mxptr[7] >> 14;
	sphere[4].z = phd_mxptr[11] >> 14;
	sphere[4].r = 3 * objptr[3] / 2;
	phd_PopMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	gar_RotYXZsuperpack(&rot, 2);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);

	phd_PushMatrix();
	objptr = lara.mesh_ptrs[14];//head
	phd_TranslateRel(*objptr, objptr[1], objptr[2]);
	sphere[2].x = phd_mxptr[3] >> 14;
	sphere[2].y = phd_mxptr[7] >> 14;
	sphere[2].z = phd_mxptr[11] >> 14;
	sphere[2].r = objptr[3];
	phd_PopMatrix();
	
	if (gfLevelFlags & GF_LVOP_YOUNG_LARA)
	{
		sphere[1].x = (sphere[1].x + sphere[2].x) >> 1;
		sphere[1].y = (sphere[1].y + sphere[2].y) >> 1;
		sphere[1].z = (sphere[1].z + sphere[2].z) >> 1;
	}
	//end of fucking shit spheres

	if (pigtail)
		phd_TranslateRel(44, -48, -50);
	else
	{
		if (gfLevelFlags & GF_LVOP_YOUNG_LARA)
			phd_TranslateRel(-52, -48, -50);
		else
			phd_TranslateRel(-4, -4, -48);
	}

	pos.x = phd_mxptr[3] >> 14;
	pos.y = phd_mxptr[7] >> 14;
	pos.z = phd_mxptr[11] >> 14;
	phd_PopMatrix();

	obj = &objects[HAIR];
	bone = &bones[obj->bone_index];
	hair = &hairs[pigtail][0];

	if (first_hair[pigtail])
	{
		first_hair[pigtail] = 0;
		hair->pos.x_pos = pos.x;
		hair->pos.y_pos = pos.y;
		hair->pos.z_pos = pos.z;

		for (int i = 0; i < 6; i++, bone += 4)
		{
			phd_PushUnitMatrix();
			phd_SetTrans(hair->pos.x_pos, hair->pos.y_pos, hair->pos.z_pos);
			phd_RotYXZ(hair->pos.y_rot, hair->pos.x_rot, 0);
			phd_TranslateRel(bone[1], bone[2], bone[3]);
			(hair + 1)->pos.x_pos = phd_mxptr[3] >> 14;
			(hair + 1)->pos.y_pos = phd_mxptr[7] >> 14;
			(hair + 1)->pos.z_pos = phd_mxptr[11] >> 14;
			phd_PopMatrix();
			hair++;
		}

		SmokeWindZ = 0;
		SmokeWindX = 0;
		hair_wind = 0;
		hair_dwind_angle = 2048;
		hair_wind_angle = 2048;
	}
	else
	{
		hair->pos.x_pos = pos.x;
		hair->pos.y_pos = pos.y;
		hair->pos.z_pos = pos.z;
		room_num = lara_item->room_number;

		if (in_cutscene)
			water = NO_HEIGHT;
		else
		{
			x = (lara_item->pos.x_pos + (frame[0] + frame[1])) >> 1;
			y = (lara_item->pos.y_pos + (frame[2] + frame[3])) >> 1;
			z = (lara_item->pos.z_pos + (frame[4] + frame[5])) >> 1;
			water = GetWaterHeight(x, y, z, room_num);
		}

		hair_wind += (GetRandomControl() & 7) - 3;

		if (hair_wind <= -2)
			hair_wind++;
		else if (hair_wind >= 9)
			hair_wind--;

		hair_dwind_angle = (hair_dwind_angle + 2 * (GetRandomControl() & 0x3F) - 64) & 8190;

		if (hair_dwind_angle < 1024)
			hair_dwind_angle = 2048 - hair_dwind_angle;
		else if (hair_dwind_angle > 3072)
			hair_dwind_angle += 6144 - (2 * hair_dwind_angle);

		hair_wind_angle = (hair_wind_angle + ((hair_dwind_angle - hair_wind_angle) >> 3)) & 8190;
		SmokeWindX = (hair_wind * rcossin_tbl[hair_wind_angle]) >> 12;
		SmokeWindZ = (hair_wind * rcossin_tbl[hair_wind_angle + 1]) >> 12;
		hair++;

		for (int i = 1; i < 7; i++, bone += 4)
		{
			pos.x = hair->pos.x_pos;
			pos.y = hair->pos.y_pos;
			pos.z = hair->pos.z_pos;

			if (in_cutscene)
				height = 32767;
			else
			{
				floor = GetFloor(hair->pos.x_pos, hair->pos.y_pos, hair->pos.z_pos, &room_num);
				height = GetHeight(floor, hair->pos.x_pos, hair->pos.y_pos, hair->pos.z_pos);
			}

			hair->pos.x_pos += 3 * hair->vel.x / 4;
			hair->pos.y_pos += 3 * hair->vel.y / 4;
			hair->pos.z_pos += 3 * hair->vel.z / 4;

			if (lara.water_status == LW_ABOVE_WATER && room[room_num].flags & ROOM_NOT_INSIDE)
			{
				hair->pos.x_pos += SmokeWindX;
				hair->pos.z_pos += SmokeWindZ;
			}

			switch (lara.water_status)
			{
			case LW_ABOVE_WATER:
				hair->pos.y_pos += 10;

				if (water != NO_HEIGHT && hair->pos.y_pos > water)
					hair->pos.y_pos = water;
				else if (hair->pos.y_pos > height)
				{
					hair->pos.x_pos = pos.x;
					hair->pos.z_pos = pos.z;
				}

				break;

			case LW_UNDERWATER:
			case LW_SURFACE:
			case LW_WADE:

				if (hair->pos.y_pos < water)
					hair->pos.y_pos = water;
				else if (hair->pos.y_pos > height)
					hair->pos.y_pos = height;

				break;
			}

			for (int j = 0; j < 5; j++)//5 because we GOT FIVE FUCKING SPHERESSSSSSSSSSSSSSSSSSSS TO TEST AGAINST AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			{
				x = hair->pos.x_pos - sphere[j].x;
				y = hair->pos.y_pos - sphere[j].y;
				z = hair->pos.z_pos - sphere[j].z;
				dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

				if (dist < SQUARE(sphere[j].r))
				{
					dist = phd_sqrt(dist);

					if (!dist)
						dist = 1;//what why

					hair->pos.x_pos = sphere[j].x + x * sphere[j].r / dist;
					hair->pos.y_pos = sphere[j].y + y * sphere[j].r / dist;
					hair->pos.z_pos = sphere[j].z + z * sphere[j].r / dist;
				}
			}

			dx = (hair->pos.x_pos - (hair - 1)->pos.x_pos);
			dy = (hair->pos.y_pos - (hair - 1)->pos.y_pos);
			dz = (hair->pos.z_pos - (hair - 1)->pos.z_pos);
			dist = phd_sqrt(SQUARE(dz) + SQUARE(dx));

#ifdef better_cutseqs
			if ((!(cutseq_num == 17 && ((GLOBAL_cutseq_frame >= 205 && GLOBAL_cutseq_frame <= 211) || (GLOBAL_cutseq_frame >= 474 && GLOBAL_cutseq_frame <= 480)))) ||
				(!(cutseq_num == 8 && (GLOBAL_cutseq_frame >= 840 && GLOBAL_cutseq_frame <= 846))))
			{
				(hair - 1)->pos.y_rot = (short)phd_atan(dz, dx);
				(hair - 1)->pos.x_rot = (short)-phd_atan(dist, dy);
			}
#else
			(hair - 1)->pos.y_rot = (short)phd_atan(dz, dx);
			(hair - 1)->pos.x_rot = (short)-phd_atan(dist, dy);
#endif

			phd_PushUnitMatrix();
			phd_SetTrans((hair - 1)->pos.x_pos, (hair - 1)->pos.y_pos, (hair - 1)->pos.z_pos);
			phd_RotYXZ((hair - 1)->pos.y_rot, (hair - 1)->pos.x_rot, 0);

			if (i == 6)
				phd_TranslateRel(bone[-3], bone[-2], bone[-1]);
			else
				phd_TranslateRel(bone[1], bone[2], bone[3]);

			hair->pos.x_pos = phd_mxptr[3] >> 14;
			hair->pos.y_pos = phd_mxptr[7] >> 14;
			hair->pos.z_pos = phd_mxptr[11] >> 14;

#ifdef better_cutseqs
			if ((cutseq_num == 16 && ((GLOBAL_cutseq_frame >= 409 && GLOBAL_cutseq_frame < 411) || GLOBAL_cutseq_frame == 1873 || GLOBAL_cutseq_frame == 3049)) ||
				(cutseq_num == 17 && ((GLOBAL_cutseq_frame >= 205 && GLOBAL_cutseq_frame <= 211)||(GLOBAL_cutseq_frame >= 474 && GLOBAL_cutseq_frame <= 480))) ||
				(cutseq_num == 8 && (GLOBAL_cutseq_frame >= 840 && GLOBAL_cutseq_frame <= 846)))
			{
				hair->vel.x = 0;
				hair->vel.y = 0;
				hair->vel.z = 0;
			}
			else
#endif
			{
				hair->vel.x = hair->pos.x_pos - pos.x;
				hair->vel.y = hair->pos.y_pos - pos.y;
				hair->vel.z = hair->pos.z_pos - pos.z;
			}

			phd_PopMatrix();
			hair++;
		}
	}
}

void InitialiseHair()
{
	OBJECT_INFO* obj;
	HAIR_STRUCT* hptr;
	long* bone;

	for (int i = 0; i < 2; i++)
	{
		obj = &objects[HAIR];
		bone = &bones[obj->bone_index];
		bone += 4;
		hptr = &hairs[i][0];
		hptr->pos.y_rot = 0;
		hptr->pos.x_rot = -16384;
		first_hair[i] = 1;

		for (int j = 1; j < 7; j++, bone += 4)
		{
			hptr->pos.x_pos = bone[1];
			hptr->pos.y_pos = bone[2];
			hptr->pos.z_pos = bone[3];
			hptr->pos.x_rot = -16384;
			hptr->pos.y_rot = 0;
			hptr->pos.z_rot = 0;
			hptr->vel.x = 0;
			hptr->vel.y = 0;
			hptr->vel.z = 0;
			hptr++;
		}
	}
}

void inject_hair()
{
	INJECT(0x00439A40, DrawHair);
	INJECT(0x00438C80, HairControl);
	INJECT(0x00438BE0, InitialiseHair);
}
