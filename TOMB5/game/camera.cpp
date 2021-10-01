#include "../tomb5/pch.h"
#include "camera.h"
#include "gameflow.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "draw.h"
#include "sound.h"
#include "deltapak.h"
#include "control.h"
#include "effect2.h"
#include "objects.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "sphere.h"
#include "debris.h"
#include "tomb4fx.h"
#include "tower1.h"
#include "larafire.h"

void InitialiseCamera()
{
	last_target.x = lara_item->pos.x_pos;
	camera.target.x = last_target.x;
	camera.shift = lara_item->pos.y_pos - 1024;
	last_target.y = camera.shift;
	camera.target.y = camera.shift;
	last_target.z = lara_item->pos.z_pos;
	camera.target.z = last_target.z;
	camera.pos.y = camera.shift;
	last_target.room_number = lara_item->room_number;
	camera.target.room_number = lara_item->room_number;
	camera.pos.x = last_target.x;
	camera.pos.z = last_target.z - 100;
	camera.pos.room_number = lara_item->room_number;
	camera.target_distance = 1536;
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

#ifndef SMOOTH_FIXED_CAM
		if (camera.old_type == FIXED_CAMERA)
			camera.speed = 1;
#endif
	}

	if (gfCurrentLevel == LVL5_STREETS_OF_ROME && (!XATrack || XATrack == 13))
	{
		if (camera.underwater && MusicVolume)
			S_CDVolume(25 * MusicVolume + 5);

		TLFlag = 1;
	}
	else
	{
		if (TLFlag == 1 && camera.underwater)
			camera.underwater = 0;

		TLFlag = 0;
	}

	if (gfCurrentLevel != LVL5_DEEPSEA_DIVE)
	{
		if (room[camera.pos.room_number].flags & ROOM_UNDERWATER)
		{
			SoundEffect(SFX_UNDERWATER, 0, SFX_ALWAYS);

			if (!camera.underwater)
			{
				if (!GLOBAL_playing_cutseq && !TLFlag)
					S_CDVolume(0);

				camera.underwater = 1;
			}
		}
		else if (camera.underwater)
		{
			if (MusicVolume)
				S_CDVolume(25 * MusicVolume + 5);

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
			gotit = (phd_atan(camera.item->pos.z_pos - item->pos.z_pos, camera.item->pos.x_pos - item->pos.x_pos) - item->pos.y_rot) >> 1;
			bounds = GetBoundsAccurate(camera.item);
			tilt = (short)(phd_atan(shift, y - (bounds[2] + bounds[3]) / 2 - camera.item->pos.y_pos) >> 1);

			if (gotit > -9100 && gotit < 9100 && tilt > -15470 && tilt < 15470)
			{
				change = (short)(gotit - lara.head_y_rot);

				if (change > 728)
					lara.head_y_rot += 728;
				else if (change > -728)
					lara.head_y_rot -= 728;
				else
					lara.head_y_rot += change;//oh christ, IDA shows lara.head_y_rot = (short)gotit, but it makes no sense
				//it's just this. which is lara.head_y_rot + gotit - lara.head_y_rot which just becomes = gotit. this just makes more sense!

				lara.torso_y_rot = lara.head_y_rot;
				change = tilt - lara.head_x_rot;

				if (change > 728)
					lara.head_x_rot += 728;
				else if (change < -728)
					lara.head_x_rot -= 728;
				else
					lara.head_x_rot += change;//same as y_rot above

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
					GetLaraJointPos(&v, LM_TORSO);
					camera.target.x = v.x;
					camera.target.y = v.y;
					camera.target.z = v.z;
					y = v.y;
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
						camera.target.z = last_target.z + ((camera.target.z - last_target.z) >> 2);
						y = camera.target.y;
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

			if (ABS(last_target.x - camera.target.x) < 4 && ABS(last_target.y - camera.target.y) < 4 && ABS(last_target.z - camera.target.z) < 4)
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
		dx = s.x - d.x;
		dy = s.y - d.y;
		dz = s.z - d.z;
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
	return !clipped;
}

long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst)
{
	FLOOR_INFO* floor;
	long wx, wy, wz, h, c;
	short room_number;

	wx = ideal->x;
	wy = ideal->y;
	wz = ideal->z;

	if (yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (h + c) >> 1;
		else if (h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - 255;
		else if (c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + 255;
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx - push, wy, wz, &room_number);
	h = GetHeight(floor, wx - push, wy, wz);
	c = GetCeiling(floor, wx - push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = (wx & ~1023) + push;

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz - push, &room_number);
	h = GetHeight(floor, wx, wy, wz - push);
	c = GetCeiling(floor, wx, wy, wz - push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = (wz & ~1023) + push;

	room_number = ideal->room_number;
	floor = GetFloor(wx + push, wy, wz, &room_number);
	h = GetHeight(floor, wx + push, wy, wz);
	c = GetCeiling(floor, wx + push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = (wx | 1023) - push;

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz + push, &room_number);
	h = GetHeight(floor, wx, wy, wz + push);
	c = GetCeiling(floor, wx, wy, wz + push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = (wz | 1023) - push;

	if (!yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (h + c) >> 1;
		else if (h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - 255;
		else if (c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + 255;
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (h < wy || wy < c || h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		return 1;

	GetFloor(wx, wy, wz, &ideal->room_number);
	ideal->x = wx;
	ideal->y = wy;
	ideal->z = wz;
	return 0;
}

void UpdateCameraElevation()
{
	PHD_VECTOR pos;
	PHD_VECTOR pos1;

	if (camera.lara_node != -1)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, camera.lara_node);
		pos1.x = 0;
		pos1.y = -256;
		pos1.z = 2048;
		GetLaraJointPos(&pos1, camera.lara_node);
		pos.z = pos1.z - pos.z;
		pos.x = pos1.x - pos.x;
		camera.actual_angle = (short)(camera.target_angle + phd_atan(pos.z, pos.x));
	}
	else
		camera.actual_angle = lara_item->pos.y_rot + camera.target_angle;

	camera.actual_elevation += (camera.target_elevation - camera.actual_elevation) >> 3;
}

void ConfirmCameraTargetPos()
{
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long wx, wy, wz, c, h;
	short room_number;

	pos.z = 0;
	pos.y = 0;
	pos.x = 0;
	GetJointAbsPosition(lara_item, &pos, LM_HEAD);

	if (camera.lara_node != -1)
	{
		camera.target.x = pos.x;
		camera.target.y = pos.y;
		camera.target.z = pos.z;
	}
	else
	{
		camera.target.x = lara_item->pos.x_pos;
		camera.target.y = (camera.target.y + pos.y) >> 1;
		camera.target.z = lara_item->pos.z_pos;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || h < wy || h <= c || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		camera.target.x = pos.x;
		camera.target.y = pos.y;
		camera.target.z = pos.z;
	}
}

void FixedCamera()
{
	MESH_INFO* StaticMesh;
	OBJECT_VECTOR* fixed;
	GAME_VECTOR ideal;
	GAME_VECTOR t2;
	PHD_VECTOR v;
	long Target, ricochet, HitLara;

	if (UseForcedFixedCamera)
	{
		ideal.x = ForcedFixedCamera.x;
		ideal.y = ForcedFixedCamera.y;
		ideal.z = ForcedFixedCamera.z;
		ideal.room_number = ForcedFixedCamera.room_number;
	}
	else
	{
		fixed = &camera.fixed[camera.number];
		ideal.x = fixed->x;
		ideal.y = fixed->y;
		ideal.z = fixed->z;
		ideal.room_number = fixed->data;

		if (fixed->flags & 2)//sniper
		{
			if (FlashFader > 2)
				FlashFader = (FlashFader >> 1) & -2;

			SniperOverlay = 1;
			camera.target.x = (camera.target.x + 2 * last_target.x) / 3;
			camera.target.y = (camera.target.y + 2 * last_target.y) / 3;
			camera.target.z = (camera.target.z + 2 * last_target.z) / 3;

			if (SniperCount)
				SniperCount--;
			else
			{
				t2.x = camera.target.x + ((camera.target.x - ideal.x) >> 1);
				t2.y = camera.target.y + ((camera.target.y - ideal.y) >> 1);
				t2.z = camera.target.z + ((camera.target.z - ideal.z) >> 1);
				ricochet = LOS(&ideal, &t2);
				GetLaraOnLOS = 1;
				Target = ObjectOnLOS2(&ideal, &t2, &v, &StaticMesh);
				HitLara = Target != 999 && Target >= 0 && !items[Target].object_number;

				if (!(GetRandomControl() & 0x3F) || !(GlobalCounter & 0x3F) || HitLara && !(GlobalCounter & 0xF) && GetRandomControl() & 1)
				{
					SoundEffect(SFX_EXPLOSION1, 0, SFX_SETPITCH | 0x5000000);
					SoundEffect(SFX_HK_FIRE, 0, 0);
					FlashFadeR = 192;
					FlashFadeG = (GetRandomControl() & 0x1F) + 160;
					FlashFadeB = 0;
					FlashFader = 24;
					SniperCount = 15;

					if (HitLara && GetRandomControl() & 3)
					{
						DoBloodSplat(v.x, v.y, v.z, (GetRandomControl() & 3) + 3, GetRandomControl() << 1, lara_item->room_number);
						lara_item->hit_points -= 100;
						GetLaraOnLOS = 0;
					}
					else
					{
						if (Target < 0)
						{
							if (StaticMesh->static_number >= 50 && StaticMesh->static_number < 58)
							{
								ShatterObject(0, StaticMesh, 128, t2.room_number, 0);
								SmashedMeshRoom[SmashedMeshCount] = t2.room_number;
								SmashedMesh[SmashedMeshCount] = StaticMesh;
								SmashedMeshCount++;
								StaticMesh->Flags &= -2;
								SoundEffect(ShatterSounds[gfCurrentLevel][StaticMesh->static_number - 50], (PHD_3DPOS*)&StaticMesh->x, 0);
							}

							TriggerRicochetSpark((GAME_VECTOR*)&v, GetRandomControl() << 1, 3, 0);
							TriggerRicochetSpark((GAME_VECTOR*)&v, GetRandomControl() << 1, 3, 0);
						}
						else if (!ricochet)
							TriggerRicochetSpark(&t2, GetRandomControl() << 1, 3, 0);
					}
				}

				GetLaraOnLOS = 0;
			}
		}
	}

	camera.fixed_camera = 1;

#ifdef SMOOTH_FIXED_CAM
	fixed = &camera.fixed[camera.number];

	if (UseForcedFixedCamera || !(fixed->flags & 2))//trapdoor fixed cams and not the stupid sniper :)
		MoveCamera(&ideal, 30);
	else
#endif
		MoveCamera(&ideal, 1);

	if (camera.timer)
	{
		camera.timer--;

		if (!camera.timer)
			camera.timer = -1;
	}
}

void ChaseCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[5];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	if (!camera.target_elevation)
		camera.target_elevation = -1820;

	camera.target_elevation += item->pos.x_rot;
	UpdateCameraElevation();

	if (camera.actual_elevation > 15470)
		camera.actual_elevation = 15470;
	else if (camera.actual_elevation < -15470)
		camera.actual_elevation = -15470;

	distance = camera.target_distance * phd_cos(camera.actual_elevation) >> 14;
	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		TargetSnaps++;
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	}
	else
		TargetSnaps = 0;

	for (int i = 0; i < 5; i++)
		ideals[i].y = (camera.target_distance * phd_sin(camera.actual_elevation) >> 14) + camera.target.y;

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < 5; i++)
	{
		if (i)
			angle = (i - 1) << 14;
		else
			angle = camera.actual_angle;

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> 14);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> 14);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (mgLOS(&temp[0], &temp[1], 0) || !i)
			{
				if (!i)
				{
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (mgLOS(&temp[0], &temp[1], 0))
			{
				dx = SQUARE(camera.target.x - ideals[i].x);
				dz = SQUARE(camera.target.z - ideals[i].z);
				dz += dx;

				if (dz > 589824)
				{
					farthestnum = 0;
					break;
				}
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

#ifndef SMOOTH_FIXED_CAM
	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;
#endif

	MoveCamera(&ideal, camera.speed);
}

void CombatCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[9];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	camera.target.x = item->pos.x_pos;
	camera.target.z = item->pos.z_pos;

	if (lara.target)
	{
		camera.target_angle = lara.target_angles[0];
		camera.target_elevation = lara.target_angles[1] + item->pos.x_rot;
	}
	else
	{
		camera.target_angle = lara.head_y_rot + lara.torso_y_rot;
		camera.target_elevation = lara.head_x_rot + lara.torso_x_rot + item->pos.x_rot - 2730;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	floor = GetFloor(wx, wy, wz, &camera.target.room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (c + 64 > h - 64 && h != NO_HEIGHT && c != NO_HEIGHT)
	{
		camera.target.y = (c + h) >> 1;
		camera.target_elevation = 0;
	}
	else if (camera.target.y > h - 64 && h != NO_HEIGHT)
	{
		camera.target.y = h - 64;
		camera.target_elevation = 0;
	}
	else if (camera.target.y < c + 64 && c != NO_HEIGHT)
	{
		camera.target.y = c + 64;
		camera.target_elevation = 0;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		TargetSnaps++;
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	}
	else
		TargetSnaps = 0;

	UpdateCameraElevation();
	camera.target_distance = 1536;
	distance = 1536 * phd_cos(camera.actual_elevation) >> 14;

	//I think both distance and the for loop for the ideals.y used camera.target_distance
	//but the compiler optimized it to 1536 since that's what target_distance is, at this point. Keeping it like this anyway.
	//but if we wanna change the distance in the future, we should keep this in mind :]

	for (int i = 0; i < 5; i++)
		ideals[i].y = (1536 * phd_sin(camera.actual_elevation) >> 14) + camera.target.y;

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < 5; i++)
	{
		if (i)
			angle = (i - 1) << 14;
		else
			angle = camera.actual_angle;

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> 14);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> 14);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (mgLOS(&temp[0], &temp[1], 0) || !i)
			{
				if (!i)
				{
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (mgLOS(&temp[0], &temp[1], 0))
			{
				dx = SQUARE(camera.target.x - ideals[i].x);
				dz = SQUARE(camera.target.z - ideals[i].z);
				dz += dx;

				if (dz > 589824)
				{
					farthestnum = 0;
					break;
				}
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

#ifndef SMOOTH_FIXED_CAM
	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;
#endif

	MoveCamera(&ideal, camera.speed);
}

void MoveCamera(GAME_VECTOR* ideal, long speed)
{
	FLOOR_INFO* floor;
//	GAME_VECTOR tcp;//unused
	GAME_VECTOR temp1;
	GAME_VECTOR temp2;
	long height, ceiling, shake, rndval, wx, wy, wz, dx, dy, dz;
	short room_number;

	if (BinocularOn < 0)
	{
		speed = 1;
		BinocularOn++;
	}

	if (old_cam.pos.x_rot == lara_item->pos.x_rot && old_cam.pos.y_rot == lara_item->pos.y_rot && old_cam.pos.z_rot == lara_item->pos.z_rot &&
		old_cam.pos2.x_rot == lara.head_x_rot && old_cam.pos2.y_rot == lara.head_y_rot && old_cam.pos2.x_pos == lara.torso_x_rot &&
		old_cam.pos2.y_pos == lara.torso_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos && old_cam.pos.y_pos == lara_item->pos.y_pos &&
		old_cam.pos.z_pos == lara_item->pos.z_pos && old_cam.current_anim_state == lara_item->current_anim_state &&
		old_cam.goal_anim_state == lara_item->goal_anim_state && old_cam.target_distance == camera.target_distance &&
		old_cam.target_elevation == camera.target_elevation && old_cam.actual_elevation == camera.actual_elevation &&
		old_cam.target_angle == camera.actual_angle && old_cam.t.x == camera.target.x && old_cam.t.y == camera.target.y &&
		old_cam.t.z == camera.target.z && camera.old_type == camera.type && !SniperOverlay && BinocularOn >= 0)
	{
		ideal->x = last_ideal.x;
		ideal->y = last_ideal.y;
		ideal->z = last_ideal.z;
		ideal->room_number = last_ideal.room_number;
	}
	else
	{
		old_cam.pos.x_rot = lara_item->pos.x_rot;
		old_cam.pos.y_rot = lara_item->pos.y_rot;
		old_cam.pos.z_rot = lara_item->pos.z_rot;
		old_cam.pos2.x_rot = lara.head_x_rot;
		old_cam.pos2.y_rot = lara.head_y_rot;
		old_cam.pos2.x_pos = lara.torso_x_rot;
		old_cam.pos2.y_pos = lara.torso_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		old_cam.target_distance = camera.target_distance;
		old_cam.target_elevation = camera.target_elevation;
		old_cam.actual_elevation = camera.actual_elevation;
		old_cam.target_angle = camera.actual_angle;
		old_cam.t.x = camera.target.x;
		old_cam.t.y = camera.target.y;
		old_cam.t.z = camera.target.z;
		last_ideal.x = ideal->x;
		last_ideal.y = ideal->y;
		last_ideal.z = ideal->z;
		last_ideal.room_number = ideal->room_number;
	}

	camera.pos.x += (ideal->x - camera.pos.x) / speed;
	camera.pos.y += (ideal->y - camera.pos.y) / speed;
	camera.pos.z += (ideal->z - camera.pos.z) / speed;
	camera.pos.room_number = ideal->room_number;
	camera.fpos.x += ((float)ideal->x - camera.fpos.x) / (float)speed;
	camera.fpos.y += ((float)ideal->y - camera.fpos.y) / (float)speed;
	camera.fpos.z += ((float)ideal->z - camera.fpos.z) / (float)speed;

	if (camera.bounce)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.x += GetRandomControl() % rndval - shake;
			camera.target.y += GetRandomControl() % rndval - shake;
			camera.target.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy < ceiling || wy > height)
	{
		mgLOS(&camera.target, &camera.pos, 0);
		dx = ABS(camera.pos.x - ideal->x);
		dy = ABS(camera.pos.y - ideal->y);
		dz = ABS(camera.pos.z - ideal->z);

		if (dx < 768 && dy < 768 && dz < 768)
		{
			temp1.x = camera.pos.x;
			temp1.y = camera.pos.y;
			temp1.z = camera.pos.z;
			temp1.room_number = camera.pos.room_number;
			temp2.x = ideal->x;
			temp2.y = ideal->y;
			temp2.z = ideal->z;
			temp2.room_number = ideal->room_number;

			if (!(mgLOS(&temp2, &temp1, 0)))
			{
				camerasnaps++;

				if (camerasnaps >= 8)
				{
					camera.pos.x = ideal->x;
					camera.pos.y = ideal->y;
					camera.pos.z = ideal->z;
					camera.pos.room_number = ideal->room_number;
					camerasnaps = 0;
				}
			}
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy - 255 < ceiling && height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = (height + ceiling) >> 1;
	else if (height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = height - 255;
	else if (wy - 255 < ceiling && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = ceiling + 255;
	else if (ceiling >= height || height == NO_HEIGHT || ceiling == NO_HEIGHT)
	{
		camera.pos.x = ideal->x;
		camera.pos.y = ideal->y;
		camera.pos.z = ideal->z;
		camera.pos.room_number = ideal->room_number;
		camera.fpos.x = (float)ideal->x;
		camera.fpos.y = (float)ideal->y;
		camera.fpos.z = (float)ideal->z;
	}

	if (gfCurrentLevel == LVL5_THIRTEENTH_FLOOR || gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		CheckForRichesIllegalDiagonalWalls();

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	aLookAt((float)camera.pos.x, (float)camera.pos.y, (float)camera.pos.z, (float)camera.target.x, (float)camera.target.y, (float)camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		dx = phd_atan(dz, dx);
		camera.mike_pos.x = (phd_sin(dx) * phd_persp >> 14) + camera.pos.x;
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = (phd_cos(dx) * phd_persp >> 14) + camera.pos.z;
	}

	camera.old_type = camera.type;
}

void BinocularCamera(ITEM_INFO* item)
{
	PHD_VECTOR pos1;
	PHD_VECTOR pos3;
	PHD_VECTOR Soffset;
	PHD_VECTOR Eoffset;
	short* ammo;
	long shake, speed, c, BinocStep, pit, rndval;
	short room_number, hxrot, hyrot;
	char Fire;

	if (LSHKTimer)
		LSHKTimer--;

	if (!LaserSight)
	{
		if (inputBusy & IN_DRAW)
			ExittingBinos = 1;
		else if (ExittingBinos)
		{
			ExittingBinos = 0;
			BinocularRange = 0;
			AlterFOV(14560);
			lara_item->mesh_bits = -1;
			lara.Busy = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			bLaraTorch = 0;
			camera.type = BinocularOldCamera;
			return;
		}
	}

	lara_item->mesh_bits = 0;
	AlterFOV((short)(7 * (2080 - BinocularRange)));
	hxrot = lara.head_x_rot << 1;
	hyrot = lara.head_y_rot;

	if (hxrot > 13650)
		hxrot = 13650;
	else if (hxrot < -13650)
		hxrot = -13650;

	if (hyrot > 14560)
		hyrot = 14560;
	else if (hyrot < -14560)
		hyrot = -14560;

	hyrot += lara_item->pos.y_rot;
	pos1.x = lara_item->pos.x_pos;
	pos1.y = lara_item->pos.y_pos - 512;
	pos1.z = lara_item->pos.z_pos;
	room_number = lara_item->room_number;
	c = GetCeiling(GetFloor(pos1.x, pos1.y, pos1.z, &room_number), pos1.x, pos1.y, pos1.z);

	if (c <= pos1.y - 256)
		pos1.y -= 256;
	else
		pos1.y += 64;

	speed = (20736 * phd_cos(hxrot)) >> 14;
	pos3.x = pos1.x + (phd_sin(hyrot) * speed >> 14);
	pos3.y = pos1.y - (phd_sin(hxrot) * 20736 >> 14);
	pos3.z = pos1.z + (phd_cos(hyrot) * speed >> 14);
	camera.pos.x = pos1.x;
	camera.pos.y = pos1.y;
	camera.pos.z = pos1.z;
	camera.pos.room_number = room_number;

	if (camera.old_type == FIXED_CAMERA)
	{
		camera.target.x = pos3.x;
		camera.target.y = pos3.y;
		camera.target.z = pos3.z;
		camera.target.room_number = lara_item->room_number;
	}
	else
	{
		camera.target.x += (pos3.x - camera.target.x) >> 2;
		camera.target.y += (pos3.y - camera.target.y) >> 2;
		camera.target.z += (pos3.z - camera.target.z) >> 2;
		camera.target.room_number = lara_item->room_number;
	}

	if (camera.bounce && camera.type == camera.old_type)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.x += (GetRandomControl() % rndval - shake) << 4;
			camera.target.y += (GetRandomControl() % rndval - shake) << 4;
			camera.target.z += (GetRandomControl() % rndval - shake) << 4;
			camera.bounce += 5;
		}
		else
		{
			camera.bounce = 0;
			camera.target.y += camera.bounce;
		}
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	aLookAt((float)camera.pos.x, (float)camera.pos.y, (float)camera.pos.z, (float)camera.target.x, (float)camera.target.y, (float)camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		c = phd_atan(camera.target.z - camera.pos.z, camera.target.x - camera.pos.x);
		camera.mike_pos.x = (phd_sin(c) * phd_persp >> 14) + camera.pos.x;
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = (phd_cos(c) * phd_persp >> 14) + camera.pos.z;
	}

	camera.old_type = camera.type;

	if (!(inputBusy & IN_WALK))
	{
		BinocStep = 64;
		pit = 0x10000;
	}
	else
	{
		BinocStep = 32;
		pit = 0x8000;
	}

	if (inputBusy & IN_SPRINT)
	{
		BinocularRange -= BinocStep;

		if (BinocularRange < 128)
			BinocularRange = 128;
		else
			SoundEffect(SFX_ZOOM_VIEW_WHIRR, 0, (pit << 8) | SFX_ALWAYS | SFX_SETPITCH);
	}
	else if (inputBusy & IN_DUCK)
	{
		BinocularRange += BinocStep;

		if (BinocularRange > 1536)
			BinocularRange = 1536;
		else
			SoundEffect(SFX_ZOOM_VIEW_WHIRR, 0, (pit << 8) | SFX_ALWAYS | SFX_SETPITCH);
	}

	Soffset.x = camera.pos.x;
	Soffset.y = camera.pos.y;
	Soffset.z = camera.pos.z;
	Eoffset.x = camera.target.x;
	Eoffset.y = camera.target.y;
	Eoffset.z = camera.target.z;

	if (LaserSight)
	{
		Fire = 0;
		ammo = get_current_ammo_pointer(lara.gun_type);

		if (!(inputBusy & IN_ACTION) || WeaponDelay || !ammo[0])
		{
			if (!(inputBusy & IN_ACTION))
			{
				if (lara.gun_type != WEAPON_CROSSBOW)
					WeaponDelay = 0;

				LSHKShotsFired = 0;
				camera.bounce = 0;
			}
		}
		else
		{
			if (lara.gun_type == WEAPON_REVOLVER)
			{
				Fire = 1;
				WeaponDelay = 16;
				savegame.Game.AmmoUsed++;

				if (ammo[0] != -1)
					ammo[0]--;
			}
			else if (lara.gun_type == WEAPON_CROSSBOW)
			{
				Fire = 1;
				WeaponDelay = 32;
			}
			else
			{
				if (lara.hk_type_carried & WTYPE_AMMO_1)//sniper mode!
				{
					WeaponDelay = 12;
					Fire = 1;

					if (lara.hk_type_carried & WTYPE_SILENCER)
						SoundEffect(14, 0, 0);
					else
					{
						SoundEffect(105, 0, 83888140);
						SoundEffect(68, 0, 0);
					}

					if (ammo[0] != -1)
						ammo[0]--;
				}
				else if (lara.hk_type_carried & WTYPE_AMMO_2)//burst mode!
				{
					if (LSHKTimer)
					{
						camera.bounce = -16 - (GetRandomControl() & 0x1F);

						if (lara.hk_type_carried & WTYPE_SILENCER)
							SoundEffect(14, 0, 0);
						else
						{
							SoundEffect(105, 0, 83888140);
							SoundEffect(68, 0, 0);
						}
					}
					else
					{
						LSHKShotsFired++;

						if (LSHKShotsFired == 5)
						{
							LSHKShotsFired = 0;
							WeaponDelay = 12;
						}

						LSHKTimer = 4;
						Fire = 1;

						if (lara.hk_type_carried & WTYPE_SILENCER)
							SoundEffect(14, 0, 0);
						else
						{
							SoundEffect(105, 0, 83888140);
							SoundEffect(68, 0, 0);
						}
					}
				}
				else//rapid mode!
				{
					if (LSHKTimer)
					{
						if (lara.hk_type_carried & WTYPE_SILENCER)
							SoundEffect(14, 0, 0);
						else
						{
							SoundEffect(105, 0, 83888140);
							SoundEffect(68, 0, 0);
						}
					}
					else
					{
						LSHKTimer = 4;
						Fire = 1;

						if (lara.hk_type_carried & WTYPE_SILENCER)
							SoundEffect(14, 0, 0);
						else
						{
							SoundEffect(105, 0, 83888140);
							SoundEffect(68, 0, 0);
						}

						if (ammo[0] != -1)
							ammo[0]--;
					}

					camera.bounce = -16 - (GetRandomControl() & 0x1F);
				}
			}
		}

		GetTargetOnLOS(&camera.pos, &camera.target, 1, Fire);
	}
	else
	{
		GetTargetOnLOS(&camera.pos, &camera.target, 0, 0);

		if (!(inputBusy & IN_ACTION) || InfraRed)
			bLaraTorch = 0;
		else
			LaraTorch(&Soffset, &Eoffset, lara.head_y_rot, 192);
	}
}

void LookCamera(ITEM_INFO* item)
{
	GAME_VECTOR ideal;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	PHD_VECTOR pos3;
	FLOOR_INFO* floor;
	long shake, dx, dy, dz, wx, wy, wz, clipped, h, c, hxrot, txrot, hyrot, tyrot, rndval, lp;
	short room_number, room_number2;

	hxrot = lara.head_x_rot;
	hyrot = lara.head_y_rot;
	txrot = lara.torso_x_rot;
	tyrot = lara.torso_y_rot;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.head_x_rot <<= 1;
	lara.head_y_rot <<= 1;

	if (lara.head_x_rot > 10010)
		lara.head_x_rot = 10010;
	else if (lara.head_x_rot < -13650)
		lara.head_x_rot = -13650;

	if (lara.head_y_rot < -14560)
		lara.head_y_rot = -14560;
	else if (lara.head_y_rot > 14560)
		lara.head_y_rot = 14560;

	if (ABS(lara.head_x_rot - old_cam.pos.x_rot) >= 16)
		old_cam.pos.x_rot = (lara.head_x_rot + old_cam.pos.x_rot) >> 1;
	else
		old_cam.pos.x_rot = lara.head_x_rot;

	if (ABS(lara.head_y_rot - old_cam.pos.y_rot) >= 16)
		old_cam.pos.y_rot = (lara.head_y_rot + old_cam.pos.y_rot) >> 1;
	else
		old_cam.pos.y_rot = lara.head_y_rot;

	pos1.x = 0;
	pos1.y = 16;
	pos1.z = 64;
	GetLaraJointPos(&pos1, 8);
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
	{
		pos1.x = 0;
		pos1.y = 16;
		pos1.z = 0;
		GetLaraJointPos(&pos1, 8);
		floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
		h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
		c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
		{
			pos1.x = 0;
			pos1.y = 16;
			pos1.z = -64;
			GetLaraJointPos(&pos1, 8);
		}
	}

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = -1024;
	GetLaraJointPos(&pos2, 8);
	pos3.x = 0;
	pos3.y = 0;
	pos3.z = 2048;
	GetLaraJointPos(&pos3, 8);
	wy = pos1.y;
	wx = pos1.x;
	wz = pos1.z;
	dx = (pos2.x - pos1.x) >> 3;
	dy = (pos2.y - pos1.y) >> 3;
	dz = (pos2.z - pos1.z) >> 3;
	room_number2 = lara_item->room_number;

	for (lp = 0; lp < 8; lp++)
	{
		room_number = room_number2;
		floor = GetFloor(wx, wy, wz, &room_number2);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (h != NO_HEIGHT && c != NO_HEIGHT && c < h && wy <= h && wy >= c)
		{
			wz += dz;
			wx += dx;
			wy += dy;
		}
	}

	if (lp)
	{
		wx -= dx;
		wy -= dy;
		wz -= dz;
	}

	ideal.x = wx;
	ideal.y = wy;
	ideal.z = wz;
	ideal.room_number = room_number;

	if (old_cam.pos.x_rot == lara.head_x_rot && old_cam.pos.y_rot == lara.head_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos &&
		old_cam.pos.y_pos == lara_item->pos.y_pos && old_cam.pos.z_pos == lara_item->pos.z_pos && 
		old_cam.current_anim_state == lara_item->current_anim_state && old_cam.goal_anim_state == lara_item->goal_anim_state
		&& camera.old_type == LOOK_CAMERA)
	{
		ideal.x = static_lookcamp.x;
		ideal.y = static_lookcamp.y;
		ideal.z = static_lookcamp.z;
		ideal.room_number = static_lookcamp.room_number;
		pos3.x = static_lookcamt.x;
		pos3.y = static_lookcamt.y;
		pos3.z = static_lookcamt.z;
	}
	else
	{
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		static_lookcamp.x = wx;
		static_lookcamp.y = wy;
		static_lookcamp.z = wz;
		static_lookcamp.room_number = ideal.room_number;
		static_lookcamt.x = pos3.x;
		static_lookcamt.y = pos3.y;
		static_lookcamt.z = pos3.z;
	}

	CameraCollisionBounds(&ideal, 224, 1);

	if (camera.old_type == FIXED_CAMERA)
	{
		camera.pos.x = ideal.x;
		camera.pos.y = ideal.y;
		camera.pos.z = ideal.z;
		camera.target.x = pos3.x;
		camera.target.y = pos3.y;
		camera.target.z = pos3.z;
		camera.fpos.x = (float)ideal.x;
		camera.fpos.y = (float)ideal.y;
		camera.fpos.z = (float)ideal.z;
	}
	else
	{
		dx = ideal.x - camera.pos.x;
		dy = ideal.y - camera.pos.y;
		dz = ideal.z - camera.pos.z;
		camera.pos.x += dx >> 2;
		camera.pos.y += dy >> 2;
		camera.pos.z += dz >> 2;
		camera.fpos.x += (float)(dx >> 2);
		camera.fpos.y += (float)(dy >> 2);
		camera.fpos.z += (float)(dz >> 2);
		dx = pos3.x - camera.target.x;
		dy = pos3.y - camera.target.y;
		dz = pos3.z - camera.target.z;
		camera.target.x += dx >> 2;
		camera.target.y += dy >> 2;
		camera.target.z += dz >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if (camera.bounce && camera.type == camera.old_type)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = -camera.bounce >> 1;
			camera.target.x += GetRandomControl() % rndval - shake;
			camera.target.y += GetRandomControl() % rndval - shake;
			camera.target.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);

	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (c > wy - 255 && h<wy + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = (h + c) >> 1;
	else if (h < wy + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = h - 255;
	else if (c > wy - 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = c + 255;

	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
		mgLOS(&camera.target, &camera.pos, 0);

	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		camera.pos.x = pos1.x;
		camera.pos.y = pos1.y;
		camera.pos.z = pos1.z;
		camera.pos.room_number = lara_item->room_number;
	}

	if (gfCurrentLevel == LVL5_THIRTEENTH_FLOOR || gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		CheckForRichesIllegalDiagonalWalls();

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	aLookAt((float)camera.pos.x, (float)camera.pos.y, (float)camera.pos.z, (float)camera.target.x, (float)camera.target.y, (float)camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		clipped = phd_atan(dz, dx);
		camera.mike_pos.x = ((phd_persp * phd_sin(clipped)) >> 14) + camera.pos.x;
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = ((phd_persp * phd_cos(clipped)) >> 14)+ camera.pos.z;
	}

	camera.old_type = camera.type;
	lara.head_x_rot = (short)hxrot;
	lara.head_y_rot = (short)hyrot;
	lara.torso_x_rot = (short)txrot;
	lara.torso_y_rot = (short)tyrot;
}

void inject_camera(bool replace)
{
	INJECT(0x0040C690, InitialiseCamera, replace);
	INJECT(0x0040ED30, CalculateCamera, replace);
	INJECT(0x00410550, LaraTorch, replace);
	INJECT(0x004108D0, ScreenShake, replace);
	INJECT(0x0040FA70, mgLOS, replace);
	INJECT(0x0040F5C0, CameraCollisionBounds, replace);
	INJECT(0x004107C0, UpdateCameraElevation, replace);
	INJECT(0x00410680, ConfirmCameraTargetPos, replace);
	INJECT(0x0040E890, FixedCamera, replace);
	INJECT(0x0040D150, ChaseCamera, replace);
	INJECT(0x0040D640, CombatCamera, replace);
	INJECT(0x0040C7A0, MoveCamera, replace);
	INJECT(0x0040FC20, BinocularCamera, replace);
	INJECT(0x0040DC10, LookCamera, replace);
}
