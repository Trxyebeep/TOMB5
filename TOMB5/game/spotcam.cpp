#include "../tomb5/pch.h"
#include "spotcam.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"

long Spline(long x, long* knots, int nk)
{
	long* k;
	long span, c1, c2;

	span = x * (nk - 3) >> 16;

	if (span >= nk - 3)
		span = nk - 4;

	k = &knots[span];
	x = x * (nk - 3) - span * 65536;
	c1 = (k[1] >> 1) - (k[2] >> 1) - k[2] + k[1] + (k[3] >> 1) + ((-k[0] - 1) >> 1);
	c2 = 2 * k[2] - 2 * k[1] - (k[1] >> 1) - (k[3] >> 1) + k[0];
	return ((long long)x * (((long long)x * (((long long)x * c1 >> 16) + c2) >> 16) + (k[2] >> 1) + ((-k[0] - 1) >> 1)) >> 16) + k[1];
}

void InitSpotCamSequences()
{
	uchar s, cc, ce;	//int in original code according to symbols but unisgned char avoids errors and
								//works fine since CameraCnt, SpotRemap, and Spotcam->sequence are unsigned chars

	bTrackCamInit = 0;

	if (number_spotcams)
	{
		ce = 0;
		s = SpotCam[0].sequence;
		cc = 1;

		for (int i = 1; i < number_spotcams; i++)
		{
			if (SpotCam[i].sequence == s)
				cc++;
			else
			{
				CameraCnt[ce] = cc;
				SpotRemap[s] = ce;
				s = SpotCam[i].sequence;
				cc = 1;
				ce++;
			}
		}

		CameraCnt[ce] = cc;
		SpotRemap[s] = ce;
	}
}

void _InitialiseSpotCam(short Sequence)//something wrong with some sequences, like the one at the start of The Base, the rotation is wrong..
{
	SPOTCAM* s;
	long next_spline_camera;

	if (bTrackCamInit && Sequence == LastSequence)
	{
		bTrackCamInit = 0;
		return;
	}

	BinocularRange = 0;
	LaserSight = 0;
	AlterFOV(14560);
	lara_item->mesh_bits = -1;
	lara.Busy = 0;
	CameraFade = -1;
	LastFov = CurrentFov;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	camera.bounce = 0;
	LastSequence = Sequence;
	bTrackCamInit = 0;
	spotcam_timer = 0;
	spotcam_loopcnt = 0;
	bDisableLaraControl = 0;
	LaraHealth = lara_item->hit_points;
	LaraAir = lara.air;
	InitialCameraPosition.x = camera.pos.x;
	InitialCameraPosition.y = camera.pos.y;
	InitialCameraPosition.z = camera.pos.z;
	InitialCameraTarget.x = camera.target.x;
	InitialCameraTarget.x = camera.target.y;
	InitialCameraTarget.x = camera.target.z;
	InitialCameraRoom = camera.pos.room_number;
	LaraFixedPosition.x = lara_item->pos.x_pos;
	LaraFixedPosition.y = lara_item->pos.y_pos;
	LaraFixedPosition.z = lara_item->pos.z_pos;
	current_sequence = Sequence;
	current_spline_camera = 0;

	for (int i = 0; i < SpotRemap[Sequence]; i++)
		current_spline_camera += CameraCnt[i];

	first_camera = current_spline_camera;
	last_camera = current_spline_camera + (CameraCnt[SpotRemap[Sequence]] - 1);
	current_camera_cnt = CameraCnt[SpotRemap[Sequence]];
	current_spline_position = 0;
	spline_to_camera = 0;
	s = &SpotCam[current_spline_camera];

	if (s->flags & SP_NOLARACONTROL || gfGameMode == 1)
	{
		bDisableLaraControl = 1;

		if (gfGameMode != 1)
			SetFadeClip(16, 1);
	}

	if (s->flags & SP_TRACKCAMERA)
	{
		int cunt;

		spline_from_camera = 0;
		cunt = 0;
		camera_xposition[cunt] = SpotCam[first_camera].x;
		camera_yposition[cunt] = SpotCam[first_camera].y;
		camera_zposition[cunt] = SpotCam[first_camera].z;
		camera_xtarget[cunt] = SpotCam[first_camera].tx;
		camera_ytarget[cunt] = SpotCam[first_camera].ty;
		camera_ztarget[cunt] = SpotCam[first_camera].tz;
		camera_roll[cunt] = SpotCam[first_camera].roll;
		camera_fov[cunt] = SpotCam[first_camera].fov;
		camera_speed[cunt] = SpotCam[first_camera].speed;
		next_spline_camera = first_camera;
		cunt++;

		for (int i = 0; i < current_camera_cnt; i++)
		{
			camera_xposition[cunt] = SpotCam[next_spline_camera].x;
			camera_yposition[cunt] = SpotCam[next_spline_camera].y;
			camera_zposition[cunt] = SpotCam[next_spline_camera].z;
			camera_xtarget[cunt] = SpotCam[next_spline_camera].tx;
			camera_ytarget[cunt] = SpotCam[next_spline_camera].ty;
			camera_ztarget[cunt] = SpotCam[next_spline_camera].tz;
			camera_roll[cunt] = SpotCam[next_spline_camera].roll;
			camera_fov[cunt] = SpotCam[next_spline_camera].fov;
			camera_speed[cunt] = SpotCam[next_spline_camera].speed;
			cunt++;
			next_spline_camera++;
		}

		camera_xposition[cunt] = SpotCam[last_camera].x;
		camera_yposition[cunt] = SpotCam[last_camera].y;
		camera_zposition[cunt] = SpotCam[last_camera].z;
		camera_xtarget[cunt] = SpotCam[last_camera].tx;
		camera_ytarget[cunt] = SpotCam[last_camera].ty;
		camera_ztarget[cunt] = SpotCam[last_camera].tz;
		camera_roll[cunt] = SpotCam[last_camera].roll;
		camera_fov[cunt] = SpotCam[last_camera].fov;
		camera_speed[cunt] = SpotCam[last_camera].speed;
	}
	else
	{
		if (s->flags & SP_SNAPCAMERA)
		{
			spline_from_camera = 0;
			next_spline_camera = current_spline_camera;
			camera_xposition[0] = SpotCam[next_spline_camera].x;
			camera_yposition[0] = SpotCam[next_spline_camera].y;
			camera_zposition[0] = SpotCam[next_spline_camera].z;
			camera_xtarget[0] = SpotCam[next_spline_camera].tx;
			camera_ytarget[0] = SpotCam[next_spline_camera].ty;
			camera_ztarget[0] = SpotCam[next_spline_camera].tz;
			camera_roll[0] = SpotCam[next_spline_camera].roll;
			camera_fov[0] = SpotCam[next_spline_camera].fov;
			camera_speed[0] = SpotCam[next_spline_camera].speed;

			for (int i = 1; i < 4; i++)
			{
				if (next_spline_camera > last_camera)
					next_spline_camera = first_camera;

				camera_xposition[i] = SpotCam[next_spline_camera].x;
				camera_yposition[i] = SpotCam[next_spline_camera].y;
				camera_zposition[i] = SpotCam[next_spline_camera].z;
				camera_xtarget[i] = SpotCam[next_spline_camera].tx;
				camera_ytarget[i] = SpotCam[next_spline_camera].ty;
				camera_ztarget[i] = SpotCam[next_spline_camera].tz;
				camera_roll[i] = SpotCam[next_spline_camera].roll;
				camera_fov[i] = SpotCam[next_spline_camera].fov;
				camera_speed[i] = SpotCam[next_spline_camera].speed;
				next_spline_camera++;
			}

			current_spline_camera++;

			if (current_spline_camera > last_camera)
				current_spline_camera = first_camera;

			if (s->flags & SP_TESTTRIGGER)
				bCheckTrigger = 1;

			if (s->flags & SP_VIGNETTE)
			{
				if (s->timer < 0)
					SCOverlay = 1;
				else if (!SlowMotion)
					SlowMotion = s->timer;
			}

			if (s->flags & SP_NODRAWLARA)
				SCNoDrawLara = 1;
		}
		else
		{
			int cunt;

			spline_from_camera = 1;
			camera_xposition[0] = InitialCameraPosition.x;
			camera_yposition[0] = InitialCameraPosition.y;
			camera_zposition[0] = InitialCameraPosition.z;
			camera_xtarget[0] = InitialCameraTarget.x;
			camera_ytarget[0] = InitialCameraTarget.y;
			camera_ztarget[0] = InitialCameraTarget.z;
			camera_roll[0] = 0;
			camera_fov[0] = CurrentFov;
			camera_speed[0] = s->speed;

			camera_xposition[1] = camera_xposition[0];
			camera_yposition[1] = camera_yposition[0];
			camera_zposition[1] = camera_zposition[0];
			camera_xtarget[1] = camera_xtarget[0];
			camera_ytarget[1] = camera_ytarget[0];
			camera_ztarget[1] = camera_ztarget[0];
			camera_roll[1] = camera_roll[0];
			camera_fov[1] = camera_fov[0];
			camera_speed[1] = camera_speed[0];

			cunt = current_spline_camera;
			camera_xposition[2] = SpotCam[cunt].x;
			camera_yposition[2] = SpotCam[cunt].y;
			camera_zposition[2] = SpotCam[cunt].z;
			camera_xtarget[2] = SpotCam[cunt].tx;
			camera_ytarget[2] = SpotCam[cunt].ty;
			camera_ztarget[2] = SpotCam[cunt].tz;
			camera_roll[2] = SpotCam[cunt].roll;
			camera_fov[2] = SpotCam[cunt].fov;
			camera_speed[2] = SpotCam[cunt].speed;
			cunt++;

			if (cunt > last_camera)
				cunt = first_camera;

			camera_xposition[3] = SpotCam[cunt].x;
			camera_yposition[3] = SpotCam[cunt].y;
			camera_zposition[3] = SpotCam[cunt].z;
			camera_xtarget[3] = SpotCam[cunt].tx;
			camera_ytarget[3] = SpotCam[cunt].ty;
			camera_ztarget[3] = SpotCam[cunt].tz;
			camera_roll[3] = SpotCam[cunt].roll;
			camera_fov[3] = SpotCam[cunt].fov;
			camera_speed[3] = SpotCam[cunt].speed;
		}
	}

	if (s->flags & SP_NODRAWLARA)
		SCNoDrawLara = 1;

	quakecam.spos.box_number = 0;
}

void inject_spotcam()
{
	INJECT(0x0047A890, Spline);
	INJECT(0x0047A800, InitSpotCamSequences);
//	INJECT(0x0047A9D0, InitialiseSpotCam);
}
