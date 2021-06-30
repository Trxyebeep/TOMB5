#include "../tomb5/pch.h"
#include "camera.h"
#include "gameflow.h"
#include "../specific/DS.h"
#include "effects.h"
#include "draw.h"
#include "sound.h"
#include "deltapak.h"
#include "control.h"
#include "effect2.h"
#include "objects.h"
#include "delstuff.h"
#include "../specific/3dmath.h"

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

void CalculateCamera()
{
	ITEM_INFO* item;
	OBJECT_VECTOR* fixed;
	PHD_VECTOR v;
	short* bounds;
	long shift, fixed_camera, y, gotit;
	short change, tilt;

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
		if (room[camera.pos.room_number].flags & ROOM_UNDERWATER)
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
			shift = phd_sqrt(SQUARE(camera.item->pos.z_pos - item->pos.z_pos) + SQUARE(camera.item->pos.x_pos - item->pos.x_pos));
			gotit = phd_atan(camera.item->pos.z_pos - item->pos.z_pos, camera.item->pos.x_pos - item->pos.x_pos) - item->pos.y_rot;
			gotit >>= 1;
			bounds = GetBoundsAccurate(camera.item);
			tilt = (short)(phd_atan(shift, y - (bounds[2] + bounds[3]) / 2 - camera.item->pos.y_pos) >> 1);

			if (gotit > -9100 && gotit < 9100 && tilt > -15470 && tilt < 15470)
			{
				change = (short)(gotit - lara.head_y_rot);

				if (change <= 728)
				{
					if (change >= -728)
						lara.head_y_rot = (short)gotit;
					else
						lara.head_y_rot -= 728;
				}
				else
					lara.head_y_rot += 728;

				lara.torso_y_rot = lara.head_y_rot;
				change = tilt - lara.head_x_rot;

				if (change <= 728)
				{
					if (change >= -728)
						lara.head_x_rot = tilt;
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
			gotit = 0;

			if (camera.type != CHASE_CAMERA && camera.flags != 3)
			{
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
					gotit = 1;
				}
			}

			if (!gotit)
			{
				shift = (bounds[0] + bounds[1] + bounds[4] + bounds[5]) >> 2;
				camera.target.x = ((phd_sin(item->pos.y_rot) * shift) >> 12) + item->pos.x_pos;
				camera.target.z = ((phd_cos(item->pos.y_rot) * shift) >> 12) + item->pos.z_pos;

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
	GAME_VECTOR s, d;
	long dx, dy, dz, radius;

	s.x = Soffset->x;
	s.y = Soffset->y;
	s.z = Soffset->z;
	s.room_number = lara_item->room_number;
	d.x = Eoffset->x;
	d.y = Eoffset->y;
	d.z = Eoffset->z;

	TriggerDynamic(s.x, s.y, s.z, 12, brightness, brightness, brightness >> 1);

	if (!LOS(&s, &d))
	{
		dx = (s.x - d.x);
		dy = (s.y - d.y);
		dz = (s.z - d.z);
		dx = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
		radius = 8 + (dx >> 8);

		if (radius > 31)
			radius = 31;

		brightness -= dx >> 8;

		if (brightness >= 0)
			TriggerDynamic(d.x, d.y, d.z, radius, brightness, brightness, brightness >> 1);
	}
}

void ScreenShake(ITEM_INFO* item, short MaxVal, short MaxDist)
{
	long dx, dy, dz;

	dx = item->pos.x_pos - camera.pos.x;
	dy = item->pos.y_pos - camera.pos.y;
	dz = item->pos.z_pos - camera.pos.z;
	dy = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

	if (dy < MaxDist)
	{
		if (MaxDist == -1)
			camera.bounce = MaxVal;
		else
			camera.bounce = -(MaxVal * (MaxDist - dy) / MaxDist);
	}
	else if (MaxDist == -1)
		camera.bounce = MaxVal;
}

long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
	short room_number, room_number2;

	dx = (target->x - start->x) >> 3;
	dy = (target->y - start->y) >> 3;
	dz = (target->z - start->z) >> 3;
	x = start->x;
	y = start->y;
	z = start->z;
	room_number = start->room_number; 
	room_number2 = room_number;
	nc = 0;
	clipped = 0;

	for (i = 0; i < 8; i++)
	{
		room_number = room_number2;
		floor = GetFloor(x, y, z, &room_number2);
		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		{
			if (!nc)
			{
				x += dx;
				y += dy;
				z += dz;
				continue;
			}

			clipped = 1;
			break;
		}

		if (y > h)
		{
			hdiff = y - h;

			if (hdiff < push)
				y = h;
			else
			{
				clipped = 1;
				break;
			}
		}

		if (y < c)
		{
			cdiff = c - y;

			if (cdiff < push)
				y = c;
			else
			{
				clipped = 1;
				break;
			}
		}

		nc = 1;

		x += dx;
		y += dy;
		z += dz;
	}

	if (i)
	{
		x -= dx;
		y -= dy;
		z -= dz;
	}

	target->x = x;
	target->y = y;
	target->z = z;
	GetFloor(x, y, z, &room_number);
	target->room_number = room_number;
	return (!clipped);
}

void inject_camera()
{
	INJECT(0x0040C690, InitialiseCamera);
	INJECT(0x0040ED30, CalculateCamera);
	INJECT(0x00410550, LaraTorch);
	INJECT(0x004108D0, ScreenShake);
	INJECT(0x0040FA70, mgLOS);
}
