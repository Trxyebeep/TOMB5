#include "../tomb5/pch.h"
#include "spotcam.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"
#include "camera.h"
#include "control.h"
#include "../specific/function_stubs.h"
#include "switch.h"
#include "gameflow.h"
#include "../specific/input.h"
#include "lara.h"
#include "../specific/dxshell.h"
#include "../tomb5/tomb5.h"

SPOTCAM SpotCam[256];
long bTrackCamInit = 0;
long bUseSpotCam = 0;
long bDisableLaraControl = 0;
long number_spotcams;
short SlowMotion = 0;
short LastSequence;
short CurrentFov;
char SCNoDrawLara;
char SCOverlay;
uchar SpotRemap[16];
uchar CameraCnt[16];

static QUAKE_CAM quakecam;

static PHD_VECTOR LaraFixedPosition;
static PHD_VECTOR InitialCameraPosition;
static PHD_VECTOR InitialCameraTarget;
static short InitialCameraRoom;

static long camera_xposition[18];
static long camera_yposition[18];
static long camera_zposition[18];
static long camera_xtarget[18];
static long camera_ytarget[18];
static long camera_ztarget[18];
static long camera_roll[18];
static long camera_fov[18];
static long camera_speed[18];
static long CameraFade;
static long LaraHealth;
static long LaraAir;
static long spline_to_camera;
static long spline_from_camera;
static long bCheckTrigger = 0;
static long current_spline_position;
static short current_sequence;
static short current_spline_camera;
static short current_camera_cnt;
static short first_camera;
static short last_camera;
static short LastFov;
static short spotcam_timer;
static short spotcam_loopcnt;

long Spline(long x, long* knots, long nk)
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
	uchar s, cc, ce;

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

void InitialiseSpotCam(short Sequence)
{
	SPOTCAM* s;
	long next_spline_camera, n;

	if (bTrackCamInit && Sequence == LastSequence)
	{
		bTrackCamInit = 0;
		return;
	}

	BinocularRange = 0;
	LaserSight = 0;
	AlterFOV(GAME_FOV);
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
	InitialCameraTarget.y = camera.target.y;
	InitialCameraTarget.z = camera.target.z;
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
		spline_from_camera = 0;
		n = 0;
		camera_xposition[n] = SpotCam[first_camera].x;
		camera_yposition[n] = SpotCam[first_camera].y;
		camera_zposition[n] = SpotCam[first_camera].z;
		camera_xtarget[n] = SpotCam[first_camera].tx;
		camera_ytarget[n] = SpotCam[first_camera].ty;
		camera_ztarget[n] = SpotCam[first_camera].tz;
		camera_roll[n] = SpotCam[first_camera].roll;
		camera_fov[n] = SpotCam[first_camera].fov;
		camera_speed[n] = SpotCam[first_camera].speed;
		next_spline_camera = first_camera;
		n++;

		for (int i = 0; i < current_camera_cnt; i++)
		{
			camera_xposition[n] = SpotCam[next_spline_camera].x;
			camera_yposition[n] = SpotCam[next_spline_camera].y;
			camera_zposition[n] = SpotCam[next_spline_camera].z;
			camera_xtarget[n] = SpotCam[next_spline_camera].tx;
			camera_ytarget[n] = SpotCam[next_spline_camera].ty;
			camera_ztarget[n] = SpotCam[next_spline_camera].tz;
			camera_roll[n] = SpotCam[next_spline_camera].roll;
			camera_fov[n] = SpotCam[next_spline_camera].fov;
			camera_speed[n] = SpotCam[next_spline_camera].speed;
			n++;
			next_spline_camera++;
		}

		camera_xposition[n] = SpotCam[last_camera].x;
		camera_yposition[n] = SpotCam[last_camera].y;
		camera_zposition[n] = SpotCam[last_camera].z;
		camera_xtarget[n] = SpotCam[last_camera].tx;
		camera_ytarget[n] = SpotCam[last_camera].ty;
		camera_ztarget[n] = SpotCam[last_camera].tz;
		camera_roll[n] = SpotCam[last_camera].roll;
		camera_fov[n] = SpotCam[last_camera].fov;
		camera_speed[n] = SpotCam[last_camera].speed;
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
			n = current_spline_camera;
			camera_xposition[2] = SpotCam[n].x;
			camera_yposition[2] = SpotCam[n].y;
			camera_zposition[2] = SpotCam[n].z;
			camera_xtarget[2] = SpotCam[n].tx;
			camera_ytarget[2] = SpotCam[n].ty;
			camera_ztarget[2] = SpotCam[n].tz;
			camera_roll[2] = SpotCam[n].roll;
			camera_fov[2] = SpotCam[n].fov;
			camera_speed[2] = SpotCam[n].speed;
			n++;

			if (n > last_camera)
				n = first_camera;

			camera_xposition[3] = SpotCam[n].x;
			camera_yposition[3] = SpotCam[n].y;
			camera_zposition[3] = SpotCam[n].z;
			camera_xtarget[3] = SpotCam[n].tx;
			camera_ytarget[3] = SpotCam[n].ty;
			camera_ztarget[3] = SpotCam[n].tz;
			camera_roll[3] = SpotCam[n].roll;
			camera_fov[3] = SpotCam[n].fov;
			camera_speed[3] = SpotCam[n].speed;
		}
	}

	if (s->flags & SP_NODRAWLARA)
		SCNoDrawLara = 1;

	quakecam.active = 0;
}

void CalculateSpotCams()
{
	SPOTCAM* s;
	CAMERA_INFO backup;
	camera_type ctype;
	long cpx, cpy, cpz, ctx, cty, ctz, cspeed, cfov, croll, next_spline_camera, n;
	long dx, dy, dz, cs, cp, clen, tlen, cx, cy, cz, lx, ly, lz, sp;
	long dist, ds, sval;
	static long bFirstLook = 0;
	short spline_cnt;

	s = &SpotCam[first_camera];
	sp = 0;

	if (bDisableLaraControl)
	{
		lara_item->hit_points = (short)LaraHealth;
		lara.air = (short)LaraAir;
	}

	if (s->flags & SP_TRACKCAMERA)
		spline_cnt = current_camera_cnt + 2;
	else
		spline_cnt = 4;

	cpx = Spline(current_spline_position, camera_xposition, spline_cnt);
	cpy = Spline(current_spline_position, camera_yposition, spline_cnt);
	cpz = Spline(current_spline_position, camera_zposition, spline_cnt);
	ctx = Spline(current_spline_position, camera_xtarget, spline_cnt);
	cty = Spline(current_spline_position, camera_ytarget, spline_cnt);
	ctz = Spline(current_spline_position, camera_ztarget, spline_cnt);
	cspeed = Spline(current_spline_position, camera_speed, spline_cnt);
	croll = Spline(current_spline_position, camera_roll, spline_cnt);
	cfov = Spline(current_spline_position, camera_fov, spline_cnt);

	if (SpotCam[current_spline_camera].flags & SP_FADEINSCREEN && CameraFade != current_spline_camera)
	{
		CameraFade = current_spline_camera;

		if (gfCurrentLevel != LVL5_TITLE)
		{
			ScreenFadedOut = 0;
			ScreenFade = 255;
			dScreenFade = 0;
			SetScreenFadeIn(16);
		}
	}

	if (SpotCam[current_spline_camera].flags & SP_FADEOUTSCREEN && CameraFade != current_spline_camera)
	{
		CameraFade = current_spline_camera;

		if (gfCurrentLevel != LVL5_TITLE)
		{
			ScreenFadedOut = 0;
			ScreenFade = 0;
			dScreenFade = 255;
			SetScreenFadeOut(16, 0);
		}
	}

	if (s->flags & SP_TRACKCAMERA)
	{
		cp = 0;
		cs = 0x2000;
		lx = lara_item->pos.x_pos;
		ly = lara_item->pos.y_pos;
		lz = lara_item->pos.z_pos;

		for (int i = 0; i < 8; i++)
		{
			tlen = 0x10000;

			for (int j = 0; j < 8; j++)
			{
				cx = Spline(sp, camera_xposition, spline_cnt) - lx;
				cy = Spline(sp, camera_yposition, spline_cnt) - ly;
				cz = Spline(sp, camera_zposition, spline_cnt) - lz;
				clen = phd_sqrt(SQUARE(cx) + SQUARE(cy) + SQUARE(cz));

				if (clen <= tlen)
				{
					cp = sp;
					tlen = clen;
				}

				sp += cs;

				if (sp > 0x10000)
					break;
			}

			cs >>= 1;
			sp = cp - (cs << 1);

			if (sp < 0)
				sp = 0;
		}

		current_spline_position += (cp - current_spline_position) >> 5;

		if (s->flags & SP_SNAPCAMERA)
		{
			if (abs(cp - current_spline_position) > 0x8000)
				current_spline_position = cp;
		}

		if (cp < 0)
			current_spline_position = 0;
		else if (cp > 0x10000)
			current_spline_position = 0x10000;
	}
	else if (!spotcam_timer)
		current_spline_position += cspeed;

	if (tomb5.cutseq_skipper && keymap[DIK_ESCAPE] && gfCurrentLevel != LVL5_TITLE)
		current_spline_position = 0x10000;

	if (!(input & IN_LOOK))
		bFirstLook = 0;

	if (s->flags & SP_NOBREAK || !(input & IN_LOOK) || gfGameMode == 1)
	{
		camera.pos.x = cpx;
		camera.pos.y = cpy;
		camera.pos.z = cpz;

		if (s->flags & (SP_TARGETLARA | SP_TRACKCAMERA))
		{
			camera.target.x = lara_item->pos.x_pos;
			camera.target.y = lara_item->pos.y_pos;
			camera.target.z = lara_item->pos.z_pos;
		}
		else
		{
			camera.target.x = ctx;
			camera.target.y = cty;
			camera.target.z = ctz;
		}

		IsRoomOutsideNo = -1;
		IsRoomOutside(camera.pos.x, camera.pos.y, camera.pos.z);

		if (IsRoomOutsideNo != -1)
			camera.pos.room_number = IsRoomOutsideNo;
		else
		{
			camera.pos.room_number = SpotCam[current_spline_camera].room_number;
			GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
		}

		AlterFOV((short)cfov);

		if (quakecam.active)
		{
			dx = camera.pos.x - quakecam.end.x;
			dy = camera.pos.y - quakecam.end.y;
			dz = camera.pos.z - quakecam.end.z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

			if (dist < quakecam.dist)
			{
				dist = quakecam.dist - dist;
				ds = quakecam.end_strength - quakecam.start_strength;
				sval = (dist * ds / quakecam.dist) + quakecam.start_strength;

				if (sval > 0)
				{
					ds = sval >> 1;
					camera.pos.x += GetRandomControl() % sval - ds;
					camera.pos.y += GetRandomControl() % sval - ds;
					camera.pos.z += GetRandomControl() % sval - ds;
				}
			}
		}

		phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, (short)croll);

		if (bCheckTrigger)
		{
			ctype = camera.type;
			camera.type = HEAVY_CAMERA;

			if (gfCurrentLevel != LVL5_TITLE)
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
			else
			{
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
			}

			camera.type = ctype;
			bCheckTrigger = 0;
		}

		if (s->flags & SP_TRACKCAMERA)
			bTrackCamInit = 1;
		else if (current_spline_position > 0x10000 - cspeed)
		{
			if (SpotCam[current_spline_camera].flags & SP_VIGNETTE)
			{
				if (SpotCam[current_spline_camera].timer < 0)
					SCOverlay = 1;
				else if (!SlowMotion)
					SlowMotion = SpotCam[current_spline_camera].timer;
			}

			if (SpotCam[current_spline_camera].flags & SP_NODRAWLARA)
				SCNoDrawLara = 1;

			if (SpotCam[current_spline_camera].flags & SP_TESTTRIGGER)
				bCheckTrigger = 1;

			if (SpotCam[current_spline_camera].flags & SP_QUAKE)
			{
				if (quakecam.active && SpotCam[current_spline_camera].timer == -1)
					quakecam.active = 0;
				else
				{
					quakecam.active = 1;

					quakecam.start.x = SpotCam[current_spline_camera].x;
					quakecam.start.y = SpotCam[current_spline_camera].y;
					quakecam.start.z = SpotCam[current_spline_camera].z;

					if (SpotCam[current_spline_camera].timer == -1)
						quakecam.start_strength = 0;
					else
						quakecam.start_strength = SpotCam[current_spline_camera].timer << 3;

					quakecam.end.x = SpotCam[current_spline_camera + 1].x;
					quakecam.end.y = SpotCam[current_spline_camera + 1].y;
					quakecam.end.z = SpotCam[current_spline_camera + 1].z;

					if (SpotCam[current_spline_camera + 1].timer == -1)
						quakecam.end_strength = 0;
					else
						quakecam.end_strength = SpotCam[current_spline_camera + 1].timer << 3;

					dx = quakecam.start.x - quakecam.end.x;
					dy = quakecam.start.y - quakecam.end.y;
					dz = quakecam.start.z - quakecam.end.z;
					quakecam.dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
				}
			}

			if (!spotcam_timer)
			{
				current_spline_position = 0;

				if (current_spline_camera == first_camera)
					next_spline_camera = last_camera;
				else
					next_spline_camera = current_spline_camera - 1;

				n = 1;

				if (spline_from_camera)
				{
					spline_from_camera = 0;
					next_spline_camera = first_camera - 1;
				}
				else
				{
					if (SpotCam[current_spline_camera].flags & SP_ENABLELARACONTROL)
						bDisableLaraControl = 0;

					if (SpotCam[current_spline_camera].flags & SP_NOLARACONTROL)
					{
						SetFadeClip(16, 1);
						bDisableLaraControl = 1;
					}

					n = 0;

					if (SpotCam[current_spline_camera].flags & SP_JUMPTO)
					{
						next_spline_camera = first_camera + (SpotCam[current_spline_camera].timer & 0xF);
						current_spline_camera = (short)next_spline_camera;
						camera_xposition[0] = SpotCam[next_spline_camera].x;
						camera_yposition[0] = SpotCam[next_spline_camera].y;
						camera_zposition[0] = SpotCam[next_spline_camera].z;
						camera_xtarget[0] = SpotCam[next_spline_camera].tx;
						camera_ytarget[0] = SpotCam[next_spline_camera].ty;
						camera_ztarget[0] = SpotCam[next_spline_camera].tz;
						camera_roll[0] = SpotCam[next_spline_camera].roll;
						camera_fov[0] = SpotCam[next_spline_camera].fov;
						camera_speed[0] = SpotCam[next_spline_camera].speed;
						n = 1;
					}

					camera_xposition[n] = SpotCam[next_spline_camera].x;
					camera_yposition[n] = SpotCam[next_spline_camera].y;
					camera_zposition[n] = SpotCam[next_spline_camera].z;
					camera_xtarget[n] = SpotCam[next_spline_camera].tx;
					camera_ytarget[n] = SpotCam[next_spline_camera].ty;
					camera_ztarget[n] = SpotCam[next_spline_camera].tz;
					camera_roll[n] = SpotCam[next_spline_camera].roll;
					camera_fov[n] = SpotCam[next_spline_camera].fov;
					camera_speed[n] = SpotCam[next_spline_camera].speed;
					n++;
				}

				next_spline_camera++;

				if (n < 4)
				{
					do
					{
						if (s->flags & SP_LOOPCAMERA)
						{
							if (next_spline_camera > last_camera)
								next_spline_camera = first_camera;
						}
						else if (next_spline_camera > last_camera)
							next_spline_camera = last_camera;

						camera_xposition[n] = SpotCam[next_spline_camera].x;
						camera_yposition[n] = SpotCam[next_spline_camera].y;
						camera_zposition[n] = SpotCam[next_spline_camera].z;
						camera_xtarget[n] = SpotCam[next_spline_camera].tx;
						camera_ytarget[n] = SpotCam[next_spline_camera].ty;
						camera_ztarget[n] = SpotCam[next_spline_camera].tz;
						camera_roll[n] = SpotCam[next_spline_camera].roll;
						camera_fov[n] = SpotCam[next_spline_camera].fov;
						camera_speed[n] = SpotCam[next_spline_camera].speed;
						n++;
						next_spline_camera++;

					} while (n < 4);
				}

				current_spline_camera++;

				if (current_spline_camera > last_camera)
				{
					if (s->flags & SP_LOOPCAMERA)
					{
						spotcam_loopcnt++;
						current_spline_camera = first_camera;
					}
					else if (s->flags & SP_SNAPBACK || spline_to_camera)
					{
						if (bCheckTrigger)
						{
							ctype = camera.type;
							camera.type = HEAVY_CAMERA;

							if (gfCurrentLevel != LVL5_TITLE)
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
							else
							{
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
							}

							camera.type = ctype;
							bCheckTrigger = 0;
						}

						SetFadeClip(0, 1);
						bUseSpotCam = 0;
						bDisableLaraControl = 0;
						bCheckTrigger = 0;
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;

						if (s->flags & SP_SNAPBACK)
						{
							camera.pos.x = InitialCameraPosition.x;
							camera.pos.y = InitialCameraPosition.y;
							camera.pos.z = InitialCameraPosition.z;
							camera.target.x = InitialCameraTarget.x;
							camera.target.y = InitialCameraTarget.y;
							camera.target.z = InitialCameraTarget.z;
							camera.pos.room_number = InitialCameraRoom;
						}

						SCOverlay = 0;
						SCNoDrawLara = 0;
						AlterFOV(LastFov);
					}
					else
					{
						current_spline_camera--;
						camera_xposition[0] = SpotCam[current_spline_camera - 1].x;
						camera_yposition[0] = SpotCam[current_spline_camera - 1].y;
						camera_zposition[0] = SpotCam[current_spline_camera - 1].z;
						camera_xtarget[0] = SpotCam[current_spline_camera - 1].tx;
						camera_ytarget[0] = SpotCam[current_spline_camera - 1].ty;
						camera_ztarget[0] = SpotCam[current_spline_camera - 1].tz;
						camera_roll[0] = SpotCam[current_spline_camera - 1].roll;
						camera_fov[0] = SpotCam[current_spline_camera - 1].fov;
						camera_speed[0] = SpotCam[current_spline_camera - 1].speed;
						camera_xposition[1] = SpotCam[current_spline_camera].x;
						camera_yposition[1] = SpotCam[current_spline_camera].y;
						camera_zposition[1] = SpotCam[current_spline_camera].z;
						camera_xtarget[1] = SpotCam[current_spline_camera].tx;
						camera_ytarget[1] = SpotCam[current_spline_camera].ty;
						camera_ztarget[1] = SpotCam[current_spline_camera].tz;
						camera_roll[1] = SpotCam[current_spline_camera].roll;
						camera_fov[1] = SpotCam[current_spline_camera].fov;
						camera_speed[1] = SpotCam[current_spline_camera].speed;
						memcpy(&backup, &camera, sizeof(CAMERA_INFO));
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;
						CalculateCamera();
						camera_fov[2] = CurrentFov;
						InitialCameraPosition.x = camera.pos.x;
						InitialCameraPosition.y = camera.pos.y;
						InitialCameraPosition.z = camera.pos.z;
						InitialCameraTarget = *(PHD_VECTOR*)&camera.target.x;
						camera_xposition[2] = camera.pos.x;
						camera_yposition[2] = camera.pos.y;
						camera_zposition[2] = camera.pos.z;
						camera_xtarget[2] = camera.target.x;
						camera_ytarget[2] = camera.target.y;
						camera_ztarget[2] = camera.target.z;
						camera_roll[2] = 0;
						camera_speed[2] = camera_speed[1];
						camera_xposition[3] = camera.pos.x;
						camera_yposition[3] = camera.pos.y;
						camera_zposition[3] = camera.pos.z;
						camera_xtarget[3] = camera.target.x;
						camera_ytarget[3] = camera.target.y;
						camera_ztarget[3] = camera.target.z;
						camera_fov[3] = CurrentFov;
						camera_speed[3] = camera_speed[1] >> 1;
						memcpy(&camera, &backup, sizeof(CAMERA_INFO));
						phd_LookAt(backup.pos.x, backup.pos.y, backup.pos.z, backup.target.x, backup.target.y, backup.target.z, (short)croll);
						spline_to_camera = 1;
					}
				}
			}
		}
	}
	else if (s->flags & SP_TRACKCAMERA)
	{
		if (!bFirstLook)
		{
			camera.old_type = FIXED_CAMERA;
			bFirstLook = 1;
		}

		CalculateCamera();
	}
	else
	{
		SetFadeClip(0, 1);
		bUseSpotCam = 0;
		bDisableLaraControl = 0;
		camera.speed = 1;
		AlterFOV(LastFov);
		CalculateCamera();
		bCheckTrigger = 0;
	}
}
