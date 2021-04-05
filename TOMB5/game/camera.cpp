#pragma once
#include "../tomb5/pch.h"
#include "camera.h"
#include "../global/types.h"
#include "gameflow.h"
#include "../specific/DS.h"
#include "effects.h"
#include "../specific/calclara.h"
#include "sound.h"
#include "../specific/maths.h"
#include "deltapak.h"
#include "control.h"
#include "effect2.h"
#include "objects.h"

void InitialiseCamera()
{
	last_target.x = lara_item->pos.x_pos;
	camera.target.x = last_target.x;
	camera.shift = lara_item->pos.y_pos - 0x400;
	last_target.y = camera.shift;
	camera.target.y = camera.shift;
	last_target.z = lara_item->pos.z_pos;
	camera.target.z = last_target.z;
	camera.pos.y = camera.shift;
	last_target.room_number = lara_item->room_number;
	camera.target.room_number = lara_item->room_number;
	camera.pos.x = last_target.x;
	camera.pos.z = last_target.z - 0x64;
	camera.pos.room_number = lara_item->room_number;
	camera.target_distance = 0x600;
	camera.item = 0;
	camera.number_frames = 1;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	camera.flags = 1;
	camera.bounce = 0;
	camera.number = -1;
	camera.fixed_camera = 0;
	AlterFOV(14560);
	UseForcedFixedCamera = 0;
	CalculateCamera();
}

void AlterFOV(short fov)//why is this here
{
	short FOV;

	FOV = fov;

	if (!fov ) 
		FOV = (short)dword_55D224;
	else 
		dword_55D224 = (int)fov;

	CurrentFOV = FOV;
	phd_persp = phd_winwidth / 2 * COS(fov / 2) / (SIN(fov / 2));
	f_persp_bis = (float)phd_persp;
	flt_55D1F8 = dword_50A440 / f_persp_bis;
	f_persp_bis_over_znear3 = f_persp_bis / f_znear3;
	LfAspectCorrection = (4.0f / 3.0f) / (phd_winwidth / phd_winheight);
	f_persp = phd_persp;
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;
	return;
}

void CalculateCamera()
{
	ITEM_INFO* item;
	short* bounds;
	short tilt;
	short change;
	long shift;
	long fixed_camera;
	long y;
	long gotit;

	CamOldPos.x = camera.pos.x;
	CamOldPos.y = camera.pos.y;
	CamOldPos.z = camera.pos.z;
	SniperOverlay = 0;
	SniperCamActive = 0;

	if (BinocularRange)
	{
		BinocularOn = 1;
		BinocularCamera(lara_item);

		if (BinocularRange)
			return;
	}

	if (BinocularOn == 1)
		BinocularOn = -8;

	if (UseForcedFixedCamera)
	{
		camera.type = FIXED_CAMERA;

		if (camera.old_type == FIXED_CAMERA)
			camera.speed = 1;
	}

	if (gfCurrentLevel != LVL5_STREETS_OF_ROME || XATrack != 0 && XATrack != 13)
	{
		if (TLFlag == 1 && camera.underwater)
			camera.underwater = 0;

		TLFlag = 0;
	}
	else
	{
		if (camera.underwater && MusicVolume != 0)
			CDDA_SetMasterVolume(25 * MusicVolume + 5);

		TLFlag = 1;
	}

	if (gfCurrentLevel != LVL5_DEEPSEA_DIVE)
	{
		if (room[camera.pos.room_number].flags & RF_FILL_WATER)
		{
			SoundEffect(SFX_UNDERWATER, 0, SFX_ALWAYS);
			if (camera.underwater == 0)
			{
				if (!GLOBAL_playing_cutseq && !TLFlag)
					CDDA_SetMasterVolume(0);

				camera.underwater = 1;
			}
		}
		else if (camera.underwater)
		{
			if (MusicVolume)
				CDDA_SetMasterVolume(25 * MusicVolume + 5);

			camera.underwater = 0;
		}
	}

	if (camera.type == CINEMATIC_CAMERA)
	{
		do_new_cutscene_camera();
		return;
	}

	item = camera.item;
	if (camera.item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
		fixed_camera = 1;
	else
	{
		item = lara_item;
		fixed_camera = 0;
	}

	bounds = GetBoundsAccurate(item);
	y = ((bounds[2] + bounds[3]) >> 1) + item->pos.y_pos - 256;

	if (camera.item)
	{
		if (!fixed_camera)
		{
			shift = phd_sqrt((camera.item->pos.z_pos - item->pos.z_pos) * (camera.item->pos.z_pos - item->pos.z_pos));
			gotit = phd_atan(camera.item->pos.z_pos - item->pos.z_pos, camera.item->pos.x_pos - item->pos.x_pos) - item->pos.y_rot;
			gotit >>= 1;
			bounds = GetBoundsAccurate(camera.item);
			tilt = phd_atan(shift, y - (bounds[2] + bounds[3]) / 2 - camera.pos.y) >> 1;

			if (gotit > -9100 && gotit < 9100 && tilt > -15470 && tilt < 15470)
			{
				change = gotit - lara.head_y_rot;

				if (change <= 728)
				{
					if (change >= -728)
						lara.head_y_rot += gotit;
					else
						lara.head_y_rot -= 728;
				}
				else
					lara.head_y_rot += 728;

				lara.torso_y_rot = lara.head_y_rot;

				if (tilt - lara.head_x_rot <= -728)
				{
					if (tilt - lara.head_x_rot >= -728)
						lara.head_x_rot += tilt;
					else
						lara.head_x_rot -= 728;
				}
				else
					lara.head_x_rot += 728;

				lara.torso_x_rot = lara.head_x_rot;
				camera.type = LOOK_CAMERA;
				camera.item->looked_at = 1;

			}
		}
	}

	if (camera.type != LOOK_CAMERA)
	{
		last_target.x = camera.target.x;

		if (camera.type != COMBAT_CAMERA)
		{
			last_target.y = camera.target.y;
			last_target.z = camera.target.z;
			last_target.room_number = camera.target.room_number;
			camera.target.room_number = item->room_number;
			camera.target.y = y;
			bool flag = false;

			if (camera.type != CHASE_CAMERA && camera.flags != 3)
			{
				OBJECT_VECTOR* fixed;
				PHD_VECTOR v;

				fixed = &camera.fixed[camera.number];
				SniperCamActive = fixed->flags & 3;

				if (fixed->flags & 2)
				{
					v.x = 0;
					v.y = 0;
					v.z = 0;
					GetLaraJointPos(&v, 7);
					camera.target.x = v.x;
					camera.target.y = v.y;
					y = v.y;
					camera.target.z = v.z;
					flag = true;
				}
			}

			if (!flag)
			{
					shift = (bounds[0] + bounds[1] + bounds[4] + bounds[5]) >> 2;
					camera.target.x = ((SIN(item->pos.y_rot) * shift) >> 12) + item->pos.x_pos;
					camera.target.z = ((COS(item->pos.y_rot) * shift) >> 12) + item->pos.z_pos;

					if (item->object_number == LARA)
					{
						ConfirmCameraTargetPos();
						y = camera.target.y;
					}
			}		

			if (fixed_camera == camera.fixed_camera)
			{
				camera.fixed_camera = 0;

				if (camera.speed != 1 && camera.old_type != LOOK_CAMERA && BinocularOn >= 0)
				{
					if (TargetSnaps <= 8)
					{
						camera.target.x = last_target.x + ((camera.target.x - last_target.x) >> 2);
						camera.target.y = last_target.y + ((camera.target.y - last_target.y) >> 2);
						y = camera.target.y;
						camera.target.z = last_target.z + ((camera.target.z - last_target.z) >> 2);
					}
					else
						TargetSnaps = 0;
				}
			}
			else
			{
				SniperCount = 30;
				camera.fixed_camera = 1;
				camera.speed = 1;
			}

			GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);

			if (ABS(last_target.x - camera.target.x) < 4 &&
				ABS(last_target.y - camera.target.y) < 4 &&
				ABS(last_target.z - camera.target.z) < 4)
			{
				camera.target.x = last_target.x;
				camera.target.y = last_target.y;
				camera.target.z = last_target.z;
			}

			if (camera.type != CHASE_CAMERA && camera.flags != 3)
				FixedCamera();
			else
				ChaseCamera(item);

			camera.fixed_camera = fixed_camera;
			camera.last = camera.number;

			if (camera.type != HEAVY_CAMERA || camera.timer == -1)
			{
				camera.type = CHASE_CAMERA;
				camera.speed = 10;
				camera.number = -1;
				camera.last_item = camera.item;
				camera.item = 0;
				camera.target_elevation = 0;
				camera.target_angle = 0;
				camera.target_distance = 1536;
				camera.flags = 0;
				camera.lara_node = -1;
			}

			return;
		}

		last_target.y = camera.target.y;
		last_target.z = camera.target.z;
		last_target.room_number = camera.target.room_number;
	}

	camera.target.room_number = item->room_number;

	if (camera.fixed_camera || BinocularOn < 0)
	{
		camera.target.y = y;
		camera.speed = 1;
	}
	else
	{
		camera.target.y += (y - camera.target.y) >> 2;
		camera.speed = camera.type != LOOK_CAMERA ? 8 : 4;
	}

	camera.fixed_camera = 0;

	if (camera.type == LOOK_CAMERA)
		LookCamera(item);
	else
		CombatCamera(item);

	camera.fixed_camera = fixed_camera;
	camera.last = camera.number;

	if (camera.type != HEAVY_CAMERA || camera.timer == -1)
	{
		camera.type = CHASE_CAMERA;
		camera.speed = 10;
		camera.number = -1;
		camera.last_item = camera.item;
		camera.item = 0;
		camera.target_elevation = 0;
		camera.target_angle = 0;
		camera.target_distance = 1536;
		camera.flags = 0;
		camera.lara_node = -1;
	}

	return;
}

void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness)
{
	GAME_VECTOR s;
	GAME_VECTOR d;
	long dx;
	long dy;
	long dz;
	long radius;

	s.x = dx = Soffset->x;
	s.y = dy = Soffset->y;
	s.z = dz = Soffset->z;
	s.room_number = lara_item->room_number;
	d.x = Eoffset->x;
	d.y = Eoffset->y;
	d.z = Eoffset->z;
	TriggerDynamic(dx, dy, dz, 12, brightness, brightness, brightness >> 1);

	if (!LOS(&s, &d))
	{
		long tmp = (phd_sqrt((s.x - d.x) * (s.x - d.x) +
			(s.y - d.y) * (s.y - d.y) +
			(s.z - d.z) * (s.z - d.z)) >> 8) + 8;

		radius = tmp + 8;

		if (radius > 31)
			radius = 31;

		if (brightness - tmp >= 0)
			TriggerDynamic(d.x, d.y, d.z, radius, brightness - tmp, brightness - tmp, (brightness - tmp) >> 1);
	}
}

void inject_camera()
{
	INJECT(0x0040C690, InitialiseCamera);
	INJECT(0x0048EDC0, AlterFOV);
	INJECT(0x0040ED30, CalculateCamera);
	INJECT(0x00410550, LaraTorch);
}
