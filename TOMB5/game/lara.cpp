#include "../tomb5/pch.h"
#include "lara.h"
#include "collide.h"
#include "laramisc.h"
#include "lara_states.h"
#include "effects.h"
#include "sound.h"
#include "control.h"
#include "draw.h"
#include "delstuff.h"
#include "items.h"
#include "sound.h"
#include "objects.h"
#include "gameflow.h"
#include "larafire.h"
#include "laraswim.h"
#include "larasurf.h"
#include "laraclmb.h"
#include "../specific/3dmath.h"
#include "rope.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "camera.h"
#include "../specific/input.h"
#include "footprnt.h"
#include "../tomb5/tomb5.h"

void(*lara_control_routines[NUM_LARA_STATES + 1])(ITEM_INFO* item, COLL_INFO* coll) =
{
	lara_as_walk,
	lara_as_run,
	lara_as_stop,
	lara_as_forwardjump,
	lara_void_func,
	lara_as_fastback,
	lara_as_turn_r,
	lara_as_turn_l,
	lara_as_death,
	lara_as_fastfall,
	lara_as_hang,
	lara_as_reach,
	lara_as_splat,
	lara_as_tread,
	lara_void_func,
	lara_as_compress,
	lara_as_back,
	lara_as_swim,
	lara_as_glide,
	lara_as_null,
	lara_as_fastturn,
	lara_as_stepright,
	lara_as_stepleft,
	lara_void_func,
	lara_as_slide,
	lara_as_backjump,
	lara_as_rightjump,
	lara_as_leftjump,
	lara_as_upjump,
	lara_as_fallback,
	lara_as_hangleft,
	lara_as_hangright,
	lara_as_slideback,
	lara_as_surftread,
	lara_as_surfswim,
	lara_as_dive,
	lara_as_pushblock,
	lara_as_pullblock,
	lara_as_ppready,
	lara_as_pickup,
	lara_as_switchon,
	lara_as_switchoff,
	lara_as_usekey,
	lara_as_usepuzzle,
	lara_as_uwdeath,
	lara_void_func,
	lara_as_special,
	lara_as_surfback,
	lara_as_surfleft,
	lara_as_surfright,
	lara_void_func,
	lara_void_func,
	lara_as_swandive,
	lara_as_fastdive,
	lara_as_gymnast,
	lara_as_waterout,
	lara_as_climbstnc,
	lara_as_climbing,
	lara_as_climbleft,
	lara_as_climbend,
	lara_as_climbright,
	lara_as_climbdown,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_as_wade,
	lara_as_waterroll,
	lara_as_pickupflare,
	lara_void_func,
	lara_void_func,
	lara_as_deathslide,
	lara_as_duck,
	lara_as_duckroll,
	lara_as_dash,
	lara_as_dashdive,
	lara_as_hang2,
	lara_as_monkeyswing,
	lara_as_monkeyl,
	lara_as_monkeyr,
	lara_as_monkey180,
	lara_as_all4s,
	lara_as_crawl,
	lara_as_hangturnl,
	lara_as_hangturnr,
	lara_as_all4turnl,
	lara_as_all4turnr,
	lara_as_crawlb,
	lara_as_null,
	lara_as_null,
	lara_as_controlled,
	lara_as_ropel,
	lara_as_roper,
	lara_as_controlled,
	lara_as_controlled,
	lara_as_controlled,
	lara_as_controlledl,
	lara_as_controlledl,
	lara_as_controlled,
	lara_as_pickup,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_poleleft,
	lara_as_poleright,
	lara_as_pulley,
	lara_as_duckl,
	lara_as_duckr,
	lara_as_extcornerl,
	lara_as_extcornerr,
	lara_as_intcornerl,
	lara_as_intcornerr,
	lara_as_rope,
	lara_as_climbrope,
	lara_as_climbroped,
	lara_as_rope,
	lara_as_rope,
	lara_void_func,
	lara_as_controlled,
	lara_as_swimcheat,
	lara_as_trpose,
	lara_as_null,
	lara_as_trwalk,
	lara_as_trfall,
	lara_as_trfall,
	lara_as_null,
	lara_as_null,
	lara_as_switchon,
	lara_as_null,
	lara_as_parallelbars,
	lara_as_pbleapoff,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null,
	lara_as_null
};

void(*lara_collision_routines[NUM_LARA_STATES + 1])(ITEM_INFO* item, COLL_INFO* coll) =
{
	lara_col_walk,
	lara_col_run,
	lara_col_stop,
	lara_col_forwardjump,
	lara_col_pose,
	lara_col_fastback,
	lara_col_turn_r,
	lara_col_turn_l,
	lara_col_death,
	lara_col_fastfall,
	lara_col_hang,
	lara_col_reach,
	lara_col_splat,
	lara_col_tread,
	lara_col_land,
	lara_col_compress,
	lara_col_back,
	lara_col_swim,
	lara_col_glide,
	lara_default_col,
	lara_col_fastturn,
	lara_col_stepright,
	lara_col_stepleft,
	lara_col_roll2,
	lara_col_slide,
	lara_col_backjump,
	lara_col_rightjump,
	lara_col_leftjump,
	lara_col_upjump,
	lara_col_fallback,
	lara_col_hangleft,
	lara_col_hangright,
	lara_col_slideback,
	lara_col_surftread,
	lara_col_surfswim,
	lara_col_dive,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_col_uwdeath,
	lara_col_roll,
	lara_void_func,
	lara_col_surfback,
	lara_col_surfleft,
	lara_col_surfright,
	lara_void_func,
	lara_void_func,
	lara_col_swandive,
	lara_col_fastdive,
	lara_default_col,
	lara_default_col,
	lara_col_climbstnc,
	lara_col_climbing,
	lara_col_climbleft,
	lara_col_climbend,
	lara_col_climbright,
	lara_col_climbdown,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_col_wade,
	lara_col_waterroll,
	lara_default_col,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_col_duck,
	lara_col_duckroll,
	lara_col_dash,
	lara_col_dashdive,
	lara_col_hang2,
	lara_col_monkeyswing,
	lara_col_monkeyl,
	lara_col_monkeyr,
	lara_col_monkey180,
	lara_col_all4s,
	lara_col_crawl,
	lara_col_hangturnlr,
	lara_col_hangturnlr,
	lara_col_all4turnlr,
	lara_col_all4turnlr,
	lara_col_crawlb,
	lara_void_func,
	lara_col_crawl2hang,
	lara_default_col,
	lara_void_func,
	lara_void_func,
	lara_default_col,
	lara_void_func,
	lara_void_func,
	lara_col_turnswitch,
	lara_void_func,
	lara_void_func,
	lara_default_col,
	lara_col_polestat,
	lara_col_poleup,
	lara_col_poledown,
	lara_void_func,
	lara_void_func,
	lara_default_col,
	lara_col_ducklr,
	lara_col_ducklr,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_col_rope,
	lara_void_func,
	lara_void_func,
	lara_col_ropefwd,
	lara_col_ropefwd,
	lara_void_func,
	lara_void_func,
	lara_col_swim,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_default_col,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_void_func
};

LARA_INFO lara;
ITEM_INFO* lara_item;
short DashTimer;
char LaraDrawType;
char WeatherType;

static short LeftClimbTab[4] = { 512, 1024, 2048, 256 };
static short RightClimbTab[4] = { 2048, 256, 512, 1024 };

static void TiltHer(ITEM_INFO* item, long rad, long height)
{
	FLOOR_INFO* floor;
	FVECTOR plane;
	long wy[4];
	long yT, y, wx, wz, dy;
	short room_number, rotX, rotZ;

	if (!tomb5.crawltilt)
		return;

	yT = item->pos.y_pos - height - 162;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, yT, item->pos.z_pos, &room_number);
	y = GetHeight(floor, item->pos.x_pos, yT, item->pos.z_pos);

	if (!OnObject)
	{
		plane.x = -(float)tiltyoff / 4;
		plane.y = -(float)tiltxoff / 4;
	}
	else
	{
		wx = item->pos.x_pos & 0xFFFFFC00 | 0xFF;
		wz = item->pos.z_pos & 0xFFFFFC00 | 0xFF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[0] = GetHeight(floor, wx, yT, wz);
		wx = item->pos.x_pos & 0xFFFFFC00 | 0x2FF;
		wz = item->pos.z_pos & 0xFFFFFC00 | 0xFF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[1] = GetHeight(floor, wx, yT, wz);
		wx = item->pos.x_pos & 0xFFFFFC00 | 0xFF;
		wz = item->pos.z_pos & 0xFFFFFC00 | 0x2FF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[2] = GetHeight(floor, wx, yT, wz);
		plane.x = (float)(wy[1] - wy[0]) / 512;
		plane.y = (float)(wy[2] - wy[0]) / 512;
	}

	plane.z = item->pos.y_pos - plane.x * item->pos.x_pos - plane.y * item->pos.z_pos;

	for (int i = 0; i < 4; i++)
	{
		wx = item->pos.x_pos + (rad * phd_sin(item->pos.y_rot + 0x4000 * i) >> W2V_SHIFT);
		wz = item->pos.z_pos + (rad * phd_cos(item->pos.y_rot + 0x4000 * i) >> W2V_SHIFT);
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[i] = GetHeight(floor, wx, yT, wz);

		if (abs(y - wy[i]) > rad / 2)
			wy[i] = (long)(plane.x * wx + plane.y * wz + plane.z);
	}

	dy = wy[0] - wy[2];
	rotX = (short)phd_atan(2 * rad, dy);

	if (dy > 0 && rotX > 0 || dy < 0 && rotX < 0)
		rotX = -rotX;

	dy = wy[3] - wy[1];
	rotZ = (short)phd_atan(2 * rad, dy);

	if (dy > 0 && rotZ > 0 || dy < 0 && rotZ < 0)
		rotZ = -rotZ;

	if (abs(rotX - item->pos.x_rot) < 546)
		item->pos.x_rot = rotX;
	else if (rotX > item->pos.x_rot)
		item->pos.x_rot += 546;
	else if (rotX < item->pos.x_rot)
		item->pos.x_rot -= 546;

	if (item->pos.x_rot > 8192)
		item->pos.x_rot = 8192;
	else if (item->pos.x_rot < -8192)
		item->pos.x_rot = -8192;

	if (abs(rotZ - item->pos.z_rot) < 546)
		item->pos.z_rot = rotZ;
	else if (rotZ > item->pos.z_rot)
		item->pos.z_rot += 546;
	else if (rotZ < item->pos.z_rot)
		item->pos.z_rot -= 546;

	if (item->pos.z_rot > 8192)
		item->pos.z_rot = 8192;
	else if (item->pos.z_rot < -8192)
		item->pos.z_rot = -8192;
}

void LaraDeflectEdgeJump(ITEM_INFO* item, COLL_INFO* coll)
{
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_FRONT:
	case CT_TOP_FRONT:

		if (lara.climb_status && item->speed == 2)
			return;

		if (coll->mid_floor <= 512)
		{
			if (coll->mid_floor <= 128)
			{
				item->goal_anim_state = AS_LAND;
				item->current_anim_state = AS_LAND;
				item->anim_number = ANIM_LAND;
				item->frame_number = anims[ANIM_LAND].frame_base;
			}
		}
		else
		{
			item->goal_anim_state = AS_FASTFALL;
			item->current_anim_state = AS_FASTFALL;
			item->anim_number = ANIM_FASTSPLAT;
			item->frame_number = anims[ANIM_FASTSPLAT].frame_base + 1;
		}

		item->speed /= 4;
		lara.move_angle -= 0x8000;

		if (item->fallspeed < 1)
			item->fallspeed = 1;

		break;

	case CT_TOP:

		if (item->fallspeed < 1)
			item->fallspeed = 1;

		break;

	case CT_LEFT:
		item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 910;
		break;

	case CT_CLAMP:
		item->pos.z_pos -= 100 * phd_cos(coll->facing) >> W2V_SHIFT;
		item->pos.x_pos -= 100 * phd_sin(coll->facing) >> W2V_SHIFT;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed < 1)
			item->fallspeed = 16;

		break;
	}
}

long LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll)
{
	short land_speed;
	
	land_speed = item->fallspeed - 140;

	if (land_speed <= 0)
		return 0;

	if (land_speed <= 14)
		item->hit_points += -1000 * SQUARE(land_speed) / 196;
	else
		item->hit_points = -1;

	return item->hit_points <= 0;
}

long TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll)
{
	static short old_ang = 1;
	short ang_diff, ang;

	if (abs(coll->tilt_x) <= 2 && abs(coll->tilt_z) <= 2)
		return 0;

	ang = 0;

	if (coll->tilt_x > 2)
		ang = -0x4000;
	else if (coll->tilt_x < -2)
		ang = 0x4000;

	if (coll->tilt_z > 2 && coll->tilt_z > abs(coll->tilt_x))
		ang = -0x8000;
	else if (coll->tilt_z < -2 && (-coll->tilt_z > abs(coll->tilt_x)))
		ang = 0;

	ang_diff = ang - item->pos.y_rot;
	ShiftItem(item, coll);

	if (ang_diff >= -0x4000 && ang_diff <= 0x4000)
	{
		if (item->current_anim_state == AS_SLIDE && old_ang == ang)
			return 1;

		item->anim_number = ANIM_SLIDE;
		item->frame_number = anims[ANIM_SLIDE].frame_base;
		item->current_anim_state = AS_SLIDE;
		item->goal_anim_state = AS_SLIDE;
		item->pos.y_rot = ang;
		lara.move_angle = ang;
		old_ang = ang;
	}
	else
	{
		if (item->current_anim_state == AS_SLIDEBACK && old_ang == ang)
			return 1;

		item->anim_number = ANIM_SLIDEBACK;
		item->frame_number = anims[ANIM_SLIDEBACK].frame_base;
		item->current_anim_state = AS_SLIDEBACK;
		item->goal_anim_state = AS_SLIDEBACK;
		item->pos.y_rot = ang - 0x8000;
		lara.move_angle = ang;
		old_ang = ang;
	}

	return 1;
}

long LaraDeflectEdge(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->goal_anim_state = AS_STOP;
		item->speed = 0;
		item->gravity_status = 0;
		return 1;
	}
	
	if (coll->coll_type == CT_LEFT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot += 910;
	}
	else if (coll->coll_type == CT_RIGHT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot -= 910;
	}

	return 0;
}

void LaraCollideStop(ITEM_INFO* item, COLL_INFO* coll)
{
	switch (coll->old_anim_state)
	{
	case AS_STOP:
	case AS_TURN_R:
	case AS_TURN_L:
	case AS_FASTTURN:

		item->current_anim_state = coll->old_anim_state;
		item->anim_number = coll->old_anim_number;
		item->frame_number = coll->old_frame_number;

		if (input & IN_LEFT)
			item->goal_anim_state = AS_TURN_L;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_TURN_R;
		else
			item->goal_anim_state = AS_STOP;

		AnimateLara(item);
		break;

	default:
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		break;
	}
}

long LaraHitCeiling(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_TOP || coll->coll_type == CT_CLAMP)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->goal_anim_state = AS_STOP;
		item->current_anim_state = AS_STOP;
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->speed = 0;
		item->fallspeed = 0;
		item->gravity_status = 0;
		return 1;
	}
	
	return 0;
}

void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 762);
}

void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
	item->goal_anim_state = AS_STOP;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;
	}
}

void lara_col_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = 0;
	lara.move_angle = item->pos.y_rot - 0x8000;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 1;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (coll->mid_floor > 200)
	{
		item->current_anim_state = AS_FALLBACK;
		item->goal_anim_state = AS_FALLBACK;
		item->anim_number = ANIM_FALLBACK;
		item->frame_number = anims[ANIM_FALLBACK].frame_base;
		item->fallspeed = 0;
		item->gravity_status = 1;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
		LaraCollideStop(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS)
		item->goal_anim_state = AS_REACH;
}

void lara_col_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x8000;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);
	
	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;
											
		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
	else if (input & IN_RIGHT && item->goal_anim_state != AS_STOP)
		item->goal_anim_state = AS_TWIST;
}

void lara_col_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x4000;
	lara_col_jumper(item, coll);
}

void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
	else if (input & IN_LEFT && item->goal_anim_state != AS_STOP)
		item->goal_anim_state = AS_TWIST;
}

void lara_col_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x4000;
	lara_col_jumper(item, coll);
}

void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 0x5FFA;

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
	else if (item->goal_anim_state == AS_RUN)
		item->goal_anim_state = AS_STOP;
	else if (input & (IN_FORWARD | IN_ROLL) && item->goal_anim_state != AS_STOP)
		item->goal_anim_state = AS_TWIST;
}

void lara_col_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	lara_col_jumper(item, coll);
}

void lara_col_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot - 0x8000;
	else
		lara.move_angle = item->pos.y_rot;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot;

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
		{
			if (lara.water_status == LW_WADE)
				item->goal_anim_state = AS_STOP;
			else
			{
				if (input & IN_FORWARD && !(input & IN_WALK))
					item->goal_anim_state = AS_RUN;
				else
					item->goal_anim_state = AS_STOP;
			}
		}

		item->gravity_status = 0;
		item->fallspeed = 0;
		item->speed = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state == AS_SWANDIVE || item->goal_anim_state == AS_REACH)
		item->goal_anim_state = AS_FORWARDJUMP;

	if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN)
	{
		if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS)
			item->goal_anim_state = AS_REACH;

		if (input & IN_ROLL || (input & IN_BACK))
			item->goal_anim_state = AS_TWIST;

		if (input & IN_WALK && lara.gun_status == LG_NO_ARMS)
			item->goal_anim_state = AS_SWANDIVE;

		if (item->fallspeed > 131)
			item->goal_anim_state = AS_FASTFALL;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -546)
			lara.turn_rate = -546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate >= 546)
			lara.turn_rate = 546;
	}
}

void LookUpDown()
{
	camera.type = LOOK_CAMERA;

	if (input & IN_FORWARD)
	{
		input -= IN_FORWARD;

		if (lara.head_x_rot > -6370)
		{
			if (BinocularRange)
				lara.head_x_rot += short(364 * (BinocularRange - 1792) / 3072);
			else
				lara.head_x_rot -= 364;
		}
	}
	else if (input & IN_BACK)
	{
		input -= IN_BACK;

		if (lara.head_x_rot < 5460)
		{
			if (BinocularRange)
				lara.head_x_rot += short(364 * (1792 - BinocularRange) / 3072);
			else
				lara.head_x_rot += 364;
		}
	}

	if (lara.gun_status != LG_HANDS_BUSY && !lara.left_arm.lock && !lara.right_arm.lock)
		lara.torso_x_rot = lara.head_x_rot;
}

void LookLeftRight()
{
	camera.type = LOOK_CAMERA;

	if (input & IN_LEFT)
	{
		input -= IN_LEFT;

		if (lara.head_y_rot > -8008)
		{
			if (BinocularRange)
				lara.head_y_rot += short(364 * (BinocularRange - 1792) / 1536);
			else
				lara.head_y_rot -= 364;
		}
	}
	else if (input & IN_RIGHT)
	{
		input -= IN_RIGHT;

		if (lara.head_y_rot < 8008)
		{
			if (BinocularRange)
				lara.head_y_rot += short(364 * (1792 - BinocularRange) / 1536);
			else
				lara.head_y_rot += 364;
		}
	}

	if (lara.gun_status != LG_HANDS_BUSY && !lara.left_arm.lock && !lara.right_arm.lock)
		lara.torso_y_rot = lara.head_y_rot;
}

void ResetLook()
{
	if (camera.type != LOOK_CAMERA)
	{
		if (lara.head_x_rot <= -364 || lara.head_x_rot >= 364)
			lara.head_x_rot -= lara.head_x_rot / 8;
		else
			lara.head_x_rot = 0;

		if (lara.head_y_rot <= -364 || lara.head_y_rot >= 364)
			lara.head_y_rot -= lara.head_y_rot / 8;
		else
			lara.head_y_rot = 0;

		if (lara.gun_status == LG_HANDS_BUSY || lara.left_arm.lock || lara.right_arm.lock)
		{
			if (!lara.head_x_rot)
				lara.torso_x_rot = 0;

			if (!lara.head_y_rot)
				lara.torso_y_rot = 0;
		}
		else
		{
			lara.torso_y_rot = lara.head_y_rot;
			lara.torso_x_rot = lara.head_x_rot;
		}
	}
}

void lara_void_func(ITEM_INFO* item, COLL_INFO* coll)
{

}

void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (lara.IsMoving)
		return;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}

	if (!(input & IN_FORWARD))
		item->goal_anim_state = AS_STOP;
	else if (lara.water_status == LW_WADE)
		item->goal_anim_state = AS_WADE;
	else if (input & IN_WALK)
		item->goal_anim_state = AS_WALK;
	else
		item->goal_anim_state = AS_RUN;

	if (item->anim_number == 7 && item->frame_number == anims[7].frame_base + 16)
		AddFootprint(item);
}

void lara_col_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->lava_is_pit = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		if (item->frame_number >= 29 && item->frame_number <= 47)
		{
			item->anim_number = ANIM_WALK_STOP_RIGHT;
			item->frame_number = anims[ANIM_WALK_STOP_RIGHT].frame_base;
		}
		else
		{
			if (item->frame_number >= 22 && item->frame_number <= 28 || item->frame_number >= 48 && item->frame_number <= 57)
			{
				item->anim_number = ANIM_WALK_STOP_LEFT;
				item->frame_number = anims[ANIM_WALK_STOP_LEFT].frame_base;
			}
			else
				LaraCollideStop(item, coll);
		}
	}

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor > 128)
	{
		if (coll->front_floor == NO_HEIGHT || coll->front_floor <= 128)
			coll->mid_floor = 0;
		else if (item->frame_number >= 28 && item->frame_number <= 45)
		{
			item->anim_number = ANIM_WALKSTEPD_RIGHT;
			item->frame_number = anims[ANIM_WALKSTEPD_RIGHT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_WALKSTEPD_LEFT;
			item->frame_number = anims[ANIM_WALKSTEPD_LEFT].frame_base;
		}
	}

	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (coll->front_floor == NO_HEIGHT || coll->front_floor < -384 || coll->front_floor >= -128)
			coll->mid_floor = 0;
		else if (item->frame_number >= 27 && item->frame_number <= 44)
		{
			item->anim_number = ANIM_WALKSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_WALKSTEPUP_RIGHT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_WALKSTEPUP_LEFT;
			item->frame_number = anims[ANIM_WALKSTEPUP_LEFT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

long LaraFallen(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.water_status == LW_WADE || coll->mid_floor <= 384)
		return 0;

	item->anim_number = ANIM_FALLDOWN;
	item->current_anim_state = AS_FORWARDJUMP;
	item->goal_anim_state = AS_FORWARDJUMP;
	item->frame_number = anims[ANIM_FALLDOWN].frame_base;
	item->fallspeed = 0;
	item->gravity_status = 1;
	return 1;
}

long TestLaraVault(ITEM_INFO* item, COLL_INFO* coll)
{
	long hdif, slope;
	short angle;

	if (!(input & IN_ACTION) || lara.gun_status != LG_NO_ARMS || coll->coll_type != CT_FRONT)
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -0x1554 && angle <= 0x1554)
		angle = 0;
	else if (angle >= 0x2AAC && angle <= 0x5554)
		angle = 0x4000;
	else if (angle >= 0x6AAB || angle <= -0x6AAB)
		angle = -0x8000;
	else if (angle >= -0x5554 && angle <= -0x2AAC)
		angle = -0x4000;

	if (angle & 0x3FFF)
		return 0;

	hdif = coll->front_floor;
	slope = abs(coll->left_floor2 - coll->right_floor2) >= 60;

	if (hdif >= -640 && hdif <= -384)
	{
		if (slope || hdif - coll->front_ceiling < 0 || coll->left_floor2 - coll->left_ceiling2 < 0 || coll->right_floor2 - coll->right_ceiling2 < 0)
			return 0;

		item->anim_number = ANIM_VAULT2;
		item->frame_number = anims[ANIM_VAULT2].frame_base;
		item->current_anim_state = AS_NULL;
		item->goal_anim_state = AS_STOP;
		item->pos.y_pos += hdif + 512;
		lara.gun_status = LG_HANDS_BUSY;
	}
	else if (hdif >= -896 && hdif <= -640)
	{
		if (slope || hdif - coll->front_ceiling < 0 || coll->left_floor2 - coll->left_ceiling2 < 0 || coll->right_floor2 - coll->right_ceiling2 < 0)
			return 0;

		item->anim_number = ANIM_VAULT3;
		item->frame_number = anims[ANIM_VAULT3].frame_base;
		item->current_anim_state = AS_NULL;
		item->goal_anim_state = AS_STOP;
		item->pos.y_pos += hdif + 768;
		lara.gun_status = LG_HANDS_BUSY;
	}
	else if (!slope && hdif >= -1920 && hdif <= -896)
	{
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->goal_anim_state = AS_UPJUMP;
		item->current_anim_state = AS_STOP;
		lara.calc_fallspeed = (short)(-3 - phd_sqrt(hdif * -12 + -9600));
		AnimateLara(item);
	}
	else
	{
		if (!lara.climb_status)
			return 0;

		if (hdif > -1920 || lara.water_status == LW_WADE || coll->left_floor2 > -1920 || coll->right_floor2 > -2048 || coll->mid_ceiling > -1158)
		{
			if ((hdif < -1024 || coll->front_ceiling >= 506) && coll->mid_ceiling <= -518)
			{
				ShiftItem(item, coll);

				if (LaraTestClimbStance(item, coll))
				{
					item->anim_number = ANIM_STOP;
					item->frame_number = anims[ANIM_STOP].frame_base;
					item->goal_anim_state = AS_CLIMBSTNC;
					item->current_anim_state = AS_STOP;
					AnimateLara(item);
					item->pos.y_rot = angle;
					lara.gun_status = LG_HANDS_BUSY;
					return 1;
				}
			}

			return 0;
		}

		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->goal_anim_state = AS_UPJUMP;
		item->current_anim_state = AS_STOP;
		lara.calc_fallspeed = -116;
		AnimateLara(item);
	}

	item->pos.y_rot = angle;
	ShiftItem(item, coll);
	angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
		item->pos.z_pos = (item->pos.z_pos | 1023) - 100;
		break;

	case EAST:
		item->pos.x_pos = (item->pos.x_pos | 1023) - 100;
		break;

	case SOUTH:
		item->pos.z_pos = (item->pos.z_pos & ~1023) + 100;
		break;

	case WEST:
		item->pos.x_pos = (item->pos.x_pos & ~1023) + 100;
		break;
	}

	return 1;
}

void lara_as_null(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (item->frame_number == anims[97].frame_base + 69)
		AddFootprint(item);
}

void lara_as_run(ITEM_INFO* item, COLL_INFO* coll)
{
	static long jump_ok = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIM_ROLL;
		item->frame_number = anims[ANIM_ROLL].frame_base + 2;
		item->current_anim_state = AS_ROLL;
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_SPRINT && DashTimer)
	{
		item->goal_anim_state = AS_DASH;
		return;
	}

	if (input & IN_DUCK && lara.water_status != LW_WADE &&
		(lara.gun_status == LG_NO_ARMS || lara.gun_type == WEAPON_NONE || lara.gun_type == WEAPON_PISTOLS ||
			lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_UZI || lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1456)
			lara.turn_rate = -1456;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2002)
			item->pos.z_rot = -2002;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1456)
			lara.turn_rate = 1456;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2002)
			item->pos.z_rot = 2002;
	}

	if (item->anim_number == ANIM_STARTRUN)
		jump_ok = 0;
	else if (item->anim_number != ANIM_RUN || item->frame_number == 4)
		jump_ok = 1;

	if (input & IN_JUMP && jump_ok && !item->gravity_status)
		item->goal_anim_state = AS_FORWARDJUMP;
	else if (!(input & IN_FORWARD))
		item->goal_anim_state = AS_STOP;
	else if (lara.water_status == LW_WADE)
		item->goal_anim_state = AS_WADE;
	else if (input & IN_WALK)
		item->goal_anim_state = AS_WALK;
	else
		item->goal_anim_state = AS_RUN;
}

void lara_col_run(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		item->pos.z_rot = 0;

		if (item->anim_number != ANIM_STARTRUN && TestWall(item, 256, 0, -640))
		{
			item->current_anim_state = AS_SPLAT;

			if (item->frame_number >= 0 && item->frame_number <= 9)
			{
				item->anim_number = ANIM_HITWALLLEFT;
				item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
				return;
			}

			if (item->frame_number >= 10 && item->frame_number <= 21)
			{
				item->anim_number = ANIM_HITWALLRIGHT;
				item->frame_number = anims[ANIM_HITWALLRIGHT].frame_base;
				return;
			}
		}

		LaraCollideStop(item, coll);
	}

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (coll->front_floor == NO_HEIGHT || coll->front_floor < -384 || coll->front_floor >= -128)
			coll->mid_floor = 0;
		else if (item->frame_number >= 3 && item->frame_number <= 14)
		{
			item->anim_number = ANIM_RUNSTEPUP_LEFT;
			item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_RUNSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	short height, ceiling, fheight, rheight;

	fheight = NO_HEIGHT;
	rheight = NO_HEIGHT;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (item->anim_number != 226 && item->anim_number != 228)
		StopSoundEffect(SFX_LARA_SLIPPING);

	if (input & IN_ROLL && lara.water_status != LW_WADE)
	{
		item->anim_number = ANIM_ROLL;
		item->frame_number = anims[ANIM_ROLL].frame_base + 2;
		item->current_anim_state = AS_ROLL;
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_DUCK && lara.water_status != LW_WADE && item->current_anim_state == AS_STOP &&
		(lara.gun_status == LG_NO_ARMS || lara.gun_type == WEAPON_NONE || lara.gun_type == WEAPON_PISTOLS ||
			lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_UZI || lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	item->goal_anim_state = AS_STOP;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_FORWARD)
		fheight = LaraFloorFront(item, item->pos.y_rot, 104);
	else if (input & IN_BACK)
		rheight = LaraFloorFront(item, item->pos.y_rot - 0x8000, 104);

	if (input & IN_LSTEP)
	{
		height = LaraFloorFront(item, item->pos.y_rot - 0x4000, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot - 0x4000, 116, 762);

		if (height < 128 && height > -128 && height_type != BIG_SLOPE && ceiling <= 0)
			item->goal_anim_state = AS_STEPLEFT;
	}
	else if (input & IN_RSTEP)
	{
		height = LaraFloorFront(item, item->pos.y_rot + 0x4000, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot + 0x4000, 116, 762);

		if (height < 128 && height > -128 && height_type != BIG_SLOPE && ceiling <= 0)
			item->goal_anim_state = AS_STEPRIGHT;
	}
	else if (input & IN_LEFT)
		item->goal_anim_state = AS_TURN_L;
	else if (input & IN_RIGHT)
		item->goal_anim_state = AS_TURN_R;

	if (lara.water_status == LW_WADE)
	{
		if (input & IN_JUMP)
			item->goal_anim_state = AS_COMPRESS;

		if (input & IN_FORWARD)
		{
			if (fheight >= 383 || fheight <= -383)
			{
				lara.move_angle = item->pos.y_rot;
				coll->bad_pos = -NO_HEIGHT;
				coll->bad_neg = -384;
				coll->bad_ceiling = 0;
				coll->radius = 102;
				coll->slopes_are_walls = 1;
				GetLaraCollisionInfo(item, coll);

				if (!TestLaraVault(item, coll))
					coll->radius = 100;
			}
			else
				lara_as_wade(item, coll);
		}
		else if (input & IN_BACK && rheight < 383 && rheight > -383)
			lara_as_back(item, coll);
	}
	else if (input & IN_JUMP)
		item->goal_anim_state = AS_COMPRESS;
	else if (input & IN_FORWARD)
	{
		ceiling = LaraCeilingFront(item, item->pos.y_rot, 104, 762);
		height = LaraFloorFront(item, item->pos.y_rot, 104);

		if ((height_type == BIG_SLOPE || height_type == DIAGONAL) && height < 0 || ceiling > 0)
		{
			item->goal_anim_state = AS_STOP;
			return;
		}

		if (height >= -256 || fheight >= -256)
		{
			if (input & IN_WALK)
				lara_as_walk(item, coll);
			else
				lara_as_run(item, coll);
		}
		else
		{
			lara.move_angle = item->pos.y_rot;
			coll->bad_pos = -NO_HEIGHT;
			coll->bad_neg = -384;
			coll->bad_ceiling = 0;
			coll->radius = 102;
			coll->slopes_are_walls = 1;
			GetLaraCollisionInfo(item, coll);

			if (!TestLaraVault(item, coll))
			{
				coll->radius = 100;
				item->goal_anim_state = AS_STOP;
			}
		}
	}
	else if (input & IN_BACK)
	{
		if (input & IN_WALK)
		{
			if (rheight < 383 && rheight > -383 && height_type != BIG_SLOPE)
				lara_as_back(item, coll);
		}
		else if (rheight > -383)
			item->goal_anim_state = AS_FASTBACK;
	}
}

void lara_col_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (LaraFallen(item, coll))
		return;

	if (TestLaraSlide(item, coll))
		return;

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_back(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if (input & IN_BACK && (input & IN_WALK || lara.water_status == LW_WADE))
			item->goal_anim_state = AS_BACK;
		else
			item->goal_anim_state = AS_STOP;

		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -728)
				lara.turn_rate = -728;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 728)
				lara.turn_rate = 728;
		}
	}
}

void lara_col_back(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot - 0x8000;

	if (lara.water_status == LW_WADE)
		coll->bad_pos = -NO_HEIGHT;
	else
		coll->bad_pos = 384;

	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
		LaraCollideStop(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor > 128 && coll->mid_floor < 384)
	{
		if (item->frame_number >= 964 && item->frame_number <= 993)
		{
			item->anim_number = ANIM_BACKSTEPD_RIGHT;
			item->frame_number = anims[ANIM_BACKSTEPD_RIGHT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_BACKSTEPD_LEFT;
			item->frame_number = anims[ANIM_BACKSTEPD_LEFT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (lara.water_status == LW_WADE && coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	camera.target_elevation = -4004;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1456)
			lara.turn_rate = -1456;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2002)
			item->pos.z_rot = -2002;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1456)
			lara.turn_rate = 1456;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2002)
			item->pos.z_rot = 2002;
	}

	if (!(input & IN_FORWARD))
		item->goal_anim_state = AS_STOP;
	else if (lara.water_status == LW_ABOVE_WATER)
		item->goal_anim_state = AS_RUN;
	else
		item->goal_anim_state = AS_WADE;
}

void lara_col_wade(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		item->pos.z_rot = 0;

		if ((coll->front_type == WALL || coll->front_type == SPLIT_TRI) && coll->front_floor < -640)
		{
			item->current_anim_state = AS_SPLAT;

			if (item->frame_number >= 0 && item->frame_number <= 9)
			{
				item->anim_number = ANIM_HITWALLLEFT;
				item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
				return;
			}

			if (item->frame_number >= 10 && item->frame_number <= 21)
			{
				item->anim_number = ANIM_HITWALLRIGHT;
				item->frame_number = anims[ANIM_HITWALLRIGHT].frame_base;
				return;
			}
		}

		LaraCollideStop(item, coll);
	}
	
	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (item->frame_number >= 3 && item->frame_number <= 14)
		{
			item->anim_number = ANIM_RUNSTEPUP_LEFT;
			item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_RUNSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
		}
	}

	if (coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_dash(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || !DashTimer || !(input & IN_SPRINT) || lara.water_status == LW_WADE)
	{
		item->goal_anim_state = AS_RUN;
		return;
	}

	DashTimer--;

	if (input & IN_DUCK && (lara.gun_status == LG_NO_ARMS || lara.gun_type == WEAPON_NONE || lara.gun_type == WEAPON_PISTOLS ||
		lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_UZI || lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2912)
			item->pos.z_rot = -2912;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2912)
			item->pos.z_rot = 2912;
	}

	if (input & IN_JUMP && !item->gravity_status)
		item->goal_anim_state = AS_DASHDIVE;
	else if (!(input & IN_FORWARD))
	{
		if (!(input & (IN_LEFT | IN_RIGHT)))
			item->goal_anim_state = AS_STOP;
	}
	else if (input & IN_WALK)
		item->goal_anim_state = AS_WALK;
	else
		item->goal_anim_state = AS_DASH;

}

void lara_col_dash(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		item->pos.z_rot = 0;

		if (TestWall(item, 256, 0, -640))
		{
			item->current_anim_state = AS_SPLAT;
			item->anim_number = ANIM_HITWALLLEFT;
			item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
			return;
		}

		LaraCollideStop(item, coll);
	}

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (item->frame_number >= 3 && item->frame_number <= 14)
		{
			item->anim_number = ANIM_RUNSTEPUP_LEFT;
			item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_RUNSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_dashdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN && item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_dashdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot - 0x8000;
	else
		lara.move_angle = item->pos.y_rot;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -256;
	coll->bad_ceiling = 192;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot;

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else if (lara.water_status == LW_WADE || !(input & IN_FORWARD) || input & IN_WALK)
			item->goal_anim_state = AS_STOP;
		else
			item->goal_anim_state = AS_RUN;

		item->gravity_status = 0;
		item->fallspeed = 0;
		item->pos.y_pos += coll->mid_floor;
		item->speed = 0;
		AnimateLara(item);
	}

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_default_col(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);
}

long TestWall(ITEM_INFO* item, long front, long right, long down)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short angle, room_num;

	room_num = item->room_number;
	x = item->pos.x_pos;
	y = down + item->pos.y_pos;
	z = item->pos.z_pos;
	angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
		x -= right;
		break;

	case EAST:
		z -= right;
		break;

	case SOUTH:
		x += right;
		break;

	case WEST:
		z += right;
		break;
	}

	GetFloor(x, y, z, &room_num);

	switch (angle)
	{
	case NORTH:
		z += front;
		break;

	case EAST:
		x += front;
		break;

	case SOUTH:
		z -= front;
		break;

	case WEST:
		x -= front;
		break;
	}

	floor = GetFloor(x, y, z, &room_num);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h == NO_HEIGHT)
		return 1;

	if (h - y <= 0 || c - y >= 0)
		return 2;

	return 0;
}

short LaraFloorFront(ITEM_INFO* item, short ang, long dist)
{
	FLOOR_INFO* floor;
	long x, y, z, height;
	short room_num;

	room_num = item->room_number;
	x = item->pos.x_pos + (dist * phd_sin(ang) >> W2V_SHIFT);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + (dist * phd_cos(ang) >> W2V_SHIFT);
	floor = GetFloor(x, y, z, &room_num);
	height = GetHeight(floor, x, y, z);

	if (height != NO_HEIGHT)
		height -= item->pos.y_pos;

	return (short)height;
}

short LaraCeilingFront(ITEM_INFO* item, short ang, long dist, long h)
{
	FLOOR_INFO* floor;
	long x, y, z, height;
	short room_num;

	x = item->pos.x_pos + (dist * phd_sin(ang) >> W2V_SHIFT);
	y = item->pos.y_pos - h;
	z = item->pos.z_pos + (dist * phd_cos(ang) >> W2V_SHIFT);
	room_num = item->room_number;
	floor = GetFloor(x, y, z, &room_num);
	height = GetCeiling(floor, x, y, z);

	if (height != NO_HEIGHT)
		height += h - item->pos.y_pos;

	return (short)height;
}

void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.turn_rate += 409;

	if (lara.gun_status == LG_READY && lara.water_status != LW_WADE)
		item->goal_anim_state = AS_FASTTURN;
	else if (lara.turn_rate > 728)
	{
		if (input & IN_WALK || lara.water_status == LW_WADE)
			lara.turn_rate = 728;
		else
			item->goal_anim_state = AS_FASTTURN;
	}
		

	if (!(input & IN_FORWARD))
	{
		if (!(input & IN_RIGHT))
			item->goal_anim_state = AS_STOP;

		return;
	}

	if (lara.water_status == LW_WADE)
		item->goal_anim_state = AS_WADE;
	else if (input & IN_WALK)
		item->goal_anim_state = AS_WALK;
	else
		item->goal_anim_state = AS_RUN;
}

void lara_col_turn_r(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = 0;
	lara.move_angle = item->pos.y_rot;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (coll->mid_floor > 100)
	{
		item->anim_number = ANIM_FALLDOWN;
		item->frame_number = anims[ANIM_FALLDOWN].frame_base;
		item->current_anim_state = AS_FORWARDJUMP;
		item->goal_anim_state = AS_FORWARDJUMP;
		item->gravity_status = 1;
		item->fallspeed = 0;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.turn_rate -= 409;

	if (lara.gun_status == LG_READY && lara.water_status != LW_WADE)
		item->goal_anim_state = AS_FASTTURN;
	else if (lara.turn_rate < -728)
	{
		if (input & IN_WALK || lara.water_status == LW_WADE)
			lara.turn_rate = -728;
		else
			item->goal_anim_state = AS_FASTTURN;
	}

	if (!(input & IN_FORWARD))
	{
		if (!(input & IN_LEFT))
			item->goal_anim_state = AS_STOP;

		return;
	}

	if (lara.water_status == LW_WADE)
		item->goal_anim_state = AS_WADE;
	else if (input & IN_WALK)
		item->goal_anim_state = AS_WALK;
	else
		item->goal_anim_state = AS_RUN;
}

void lara_col_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_turn_r(item, coll);
}

void lara_as_death(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (BinocularRange)
	{
		BinocularRange = 0;
		LaserSight = 0;
		AlterFOV(GAME_FOV);
		lara_item->mesh_bits = -1;
		lara.Busy = 0;
	}
}

void lara_col_death(ITEM_INFO* item, COLL_INFO* coll)
{
	StopSoundEffect(SFX_LARA_FALL);
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->radius = 400;
	GetLaraCollisionInfo(item, coll);
	ShiftItem(item, coll);
	item->hit_points = -1;
	lara.air = -1;

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_fastfall(ITEM_INFO* item, COLL_INFO* coll)
{
	item->speed = 95 * item->speed / 100;

	if (item->fallspeed == 154)
		SoundEffect(SFX_LARA_FALL, &item->pos, SFX_DEFAULT);
}

void lara_col_fastfall(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 1;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraSlideEdgeJump(item, coll);

	if (coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
		{
			item->goal_anim_state = AS_STOP;
			item->current_anim_state = AS_STOP;
			item->anim_number = ANIM_LANDFAR;
			item->frame_number = anims[ANIM_LANDFAR].frame_base;
		}

		StopSoundEffect(SFX_LARA_FALL);
		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void LaraSlideEdgeJump(ITEM_INFO* item, COLL_INFO* coll)
{
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_LEFT:
		item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 910;
		break;

	case CT_TOP:
	case CT_TOP_FRONT:
		if (item->fallspeed <= 0)
			item->fallspeed = 1;
		break;

	case CT_CLAMP:
		item->pos.x_pos -= 100 * phd_sin(coll->facing) >> W2V_SHIFT;
		item->pos.z_pos -= 100 * phd_cos(coll->facing) >> W2V_SHIFT;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed <= 0)
			item->fallspeed = 16;

		break;
	}
}

void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.IsClimbing = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 0;
	camera.target_elevation = -8190;
}

void lara_col_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	long flag;

	item->fallspeed = 0;
	item->gravity_status = 0;

	if (tomb5.fix_climb_up_delay)
		flag = item->anim_number == ANIM_GRABLEDGE;
	else
		flag = item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21;

	if (flag)
	{
		if (input & IN_LEFT || input & IN_LSTEP)
		{
			if (CanLaraHangSideways(item, coll, -0x4000))
			{
				item->goal_anim_state = AS_HANGLEFT;
				return;
			}

			flag = LaraHangLeftCornerTest(item, coll);

			if (flag)
			{
				if (flag <= 0)
					item->goal_anim_state = AS_CORNERINTL;
				else
					item->goal_anim_state = AS_CORNEREXTL;

				return;
			}
		}

		if (input & IN_RIGHT || input & IN_RSTEP)
		{
			if (CanLaraHangSideways(item, coll, 0x4000))
			{
				item->goal_anim_state = AS_HANGRIGHT;
				return;
			}

			flag = LaraHangRightCornerTest(item, coll);

			if (flag)
			{
				if (flag <= 0)
					item->goal_anim_state = AS_CORNERINTR;
				else
					item->goal_anim_state = AS_CORNEREXTR;

				return;
			}
		}
	}

	lara.move_angle = item->pos.y_rot;
	LaraHangTest(item, coll);

	if (tomb5.fix_climb_up_delay)
		flag = item->anim_number == ANIM_GRABLEDGE;
	else
		flag = item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21;

	if (!flag)
		return;

	TestForObjectOnLedge(item, coll);

	if (input & IN_FORWARD)
	{
		if (coll->front_floor > -850)
		{
			if (coll->front_floor < -650 && coll->front_floor >= coll->front_ceiling &&
				coll->front_floor >= coll->left_ceiling2 && coll->front_floor >= coll->right_ceiling2)
			{
				if (abs(coll->left_floor2 - coll->right_floor2) < 60 && !coll->hit_static)
				{
					if (input & IN_WALK)
						item->goal_anim_state = AS_GYMNAST;
					else if (input & IN_DUCK)
					{
						item->goal_anim_state = AS_HANG2DUCK;
						item->required_anim_state = AS_DUCK;
					}
					else
						item->goal_anim_state = AS_NULL;

					return;
				}
			}

			if (coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= -256 &&
				coll->front_floor - coll->left_ceiling2 >= -256 && coll->front_floor - coll->right_ceiling2 >= -256)
			{
				if (abs(coll->left_floor2 - coll->right_floor2) < 60 && !coll->hit_static)
				{
					item->goal_anim_state = AS_HANG2DUCK;
					item->required_anim_state = AS_DUCK;
					return;
				}
			}
		}

		if (lara.climb_status && coll->mid_ceiling <= -256 && abs(coll->left_ceiling2 - coll->right_ceiling2) < 60)
		{
			if (LaraTestClimbStance(item, coll))
				item->goal_anim_state = AS_CLIMBSTNC;
			else
			{
				item->anim_number = ANIM_HANGUP;
				item->frame_number = anims[ANIM_HANGUP].frame_base;
				item->current_anim_state = AS_HANG;
				item->goal_anim_state = AS_HANG;
			}
		}

		return;
	}

	if (input & IN_BACK && lara.climb_status && coll->mid_floor > 344)
	{
		if (LaraTestClimbStance(item, coll))
			item->goal_anim_state = AS_CLIMBSTNC;
		else
		{
			item->anim_number = ANIM_HANGDOWN;
			item->frame_number = anims[ANIM_HANGDOWN].frame_base;
			item->current_anim_state = AS_HANG;
			item->goal_anim_state = AS_HANG;
		}
	}
}

long CanLaraHangSideways(ITEM_INFO* item,COLL_INFO* coll, short angle)
{
	long oldx, oldz, x, z, res;

	oldx = item->pos.x_pos;
	oldz = item->pos.z_pos;
	x = item->pos.x_pos;
	z = item->pos.z_pos;
	lara.move_angle = angle + item->pos.y_rot;

	switch (ushort(lara.move_angle + 0x2000) / 0x4000)
	{
	case NORTH:
		z += 16;
		break;

	case EAST:
		x += 16;
		break;

	case SOUTH:
		z -= 16;
		break;

	case WEST:
		x -= 16;
		break;
	}

	item->pos.x_pos = x;
	item->pos.z_pos = z;
	coll->old.y = item->pos.y_pos;
	res = LaraHangTest(item, coll);
	item->pos.x_pos = oldx;
	item->pos.z_pos = oldz;
	lara.move_angle = angle + item->pos.y_rot;
	return !res;
}

void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 15470;

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	angle = 1;
	edge = 0;
	edge_catch = 0;

	if (lara.RopePtr == -1)
		item->gravity_status = 1;

	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = 0;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && !coll->hit_static)
	{
		if (lara.CanMonkeySwing && coll->coll_type == CT_TOP)
		{
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			item->anim_number = ANIM_HANG2STOP;
			item->frame_number = anims[ANIM_HANG2STOP].frame_base;
			item->goal_anim_state = AS_HANG2;
			item->current_anim_state = AS_HANG2;
			item->gravity_status = 0;
			item->speed = 0;
			item->fallspeed = 0;
			lara.gun_status = LG_HANDS_BUSY;
			return;
		}

		if (coll->mid_ceiling <= -384 && coll->mid_floor >= 200 && coll->coll_type == CT_FRONT)
		{
			edge_catch = LaraTestEdgeCatch(item, coll, &edge);

			if (edge_catch && (edge_catch >= 0 || LaraTestHangOnClimbWall(item, coll)))
			{
				angle = item->pos.y_rot;

				if (angle >= -0x18E2 && angle <= 0x18E2)
					angle = 0;
				else if (angle >= 0x271E && angle <= 0x58E2)
					angle = 0x4000;
				else if (angle >= 0x671D || angle <= -0x671D)
					angle = -0x8000;
				else if (angle >= -0x58E2 && angle <= -0x271E)
					angle = -0x4000;
			}
		}
	}

	if (!(angle & 0x3FFF))
	{
		if (TestHangSwingIn(item, angle))
		{
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			item->anim_number = ANIM_HANG2STOP;
			item->frame_number = anims[ANIM_HANG2STOP].frame_base;
			item->current_anim_state = AS_HANG2;
			item->goal_anim_state = AS_HANG2;
		}
		else
		{
			item->anim_number = ANIM_GRABLEDGE;
			item->frame_number = anims[ANIM_GRABLEDGE].frame_base;
			item->current_anim_state = AS_HANG;
			item->goal_anim_state = AS_HANG;
		}

		bounds = GetBoundsAccurate(item);

		if (edge_catch <= 0)
			item->pos.y_pos = edge - bounds[2] - 22;
		else
		{
			item->pos.y_pos += coll->front_floor - bounds[2];

			switch (ushort(item->pos.y_rot + 0x2000) / 0x4000)
			{
			case NORTH:
				item->pos.z_pos = (item->pos.z_pos | 1023) - 100;
				item->pos.x_pos += coll->shift.x;
				break;

			case EAST:
				item->pos.x_pos = (item->pos.x_pos | 1023) - 100;
				item->pos.z_pos += coll->shift.z;
				break;

			case SOUTH:
				item->pos.z_pos = (item->pos.z_pos & ~1023) + 100;
				item->pos.x_pos += coll->shift.x;
				break;

			case WEST:
				item->pos.x_pos = (item->pos.x_pos & ~1023) + 100;
				item->pos.z_pos += coll->shift.z;
				break;
			}
		}

		item->pos.y_rot = angle;
		item->gravity_status = 1;
		item->speed = 2;
		item->fallspeed = 1;
		lara.gun_status = LG_HANDS_BUSY;
	}
	else
	{
		LaraSlideEdgeJump(item, coll);
		GetLaraCollisionInfo(item, coll);
		ShiftItem(item, coll);

		if (item->fallspeed > 0 && coll->mid_floor <= 0)
		{
			if (LaraLandedBad(item, coll))
				item->goal_anim_state = AS_DEATH;
			else
			{
				item->gravity_status = 0;
				item->goal_anim_state = AS_STOP;
				item->fallspeed = 0;

				if (coll->mid_floor != NO_HEIGHT)
					item->pos.y_pos += coll->mid_floor;
			}
		}
	}
}

void lara_as_splat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
}

void lara_col_splat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);
	ShiftItem(item, coll);

	if (coll->mid_floor >= -256 && coll->mid_floor <= 256)
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_land(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stop(item, coll);
}

void lara_as_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.water_status != LW_WADE)
	{
		if (input & IN_FORWARD && LaraFloorFront(item, item->pos.y_rot, 256) >= -384)
		{
			item->goal_anim_state = AS_FORWARDJUMP;
			lara.move_angle = item->pos.y_rot;
		}
		else if (input & IN_LEFT && LaraFloorFront(item, item->pos.y_rot - 0x4000, 256) >= -384)
		{
			item->goal_anim_state = AS_LEFTJUMP;
			lara.move_angle = item->pos.y_rot - 0x4000;
		}
		else if (input & IN_RIGHT && LaraFloorFront(item, item->pos.y_rot + 0x4000, 256) >= -384)
		{
			item->goal_anim_state = AS_RIGHTJUMP;
			lara.move_angle = item->pos.y_rot + 0x4000;
		}
		else if (input & IN_BACK && LaraFloorFront(item, item->pos.y_rot - 0x8000, 256) >= -384)
		{
			item->goal_anim_state = AS_BACKJUMP;
			lara.move_angle = item->pos.y_rot - 0x8000;
		}
	}

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = 0;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_ceiling > -100)
	{
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->goal_anim_state = AS_STOP;
		item->current_anim_state = AS_STOP;
		item->speed = 0;
		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
	}

	if (coll->mid_floor > -256 && coll->mid_floor < 256)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (lara.turn_rate >= 0)
	{
		lara.turn_rate = 1456;

		if (!(input & IN_RIGHT))
			item->goal_anim_state = AS_STOP;
	}
	else
	{
		lara.turn_rate = -1456;

		if (!(input & IN_LEFT))
			item->goal_anim_state = AS_STOP;
	}
}

void lara_col_fastturn(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stop(item, coll);
}

void lara_as_stepright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if (!(input & IN_RSTEP))
			item->goal_anim_state = AS_STOP;

		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -728)
				lara.turn_rate = -728;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 728)
				lara.turn_rate = 728;
		}
	}
}

void lara_col_stepright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->current_anim_state == AS_STEPRIGHT)
		lara.move_angle = item->pos.y_rot + 0x4000;
	else
		lara.move_angle = item->pos.y_rot - 0x4000;

	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->bad_pos = lara.water_status == LW_WADE ? -NO_HEIGHT : 128;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->bad_neg = -128;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
		LaraCollideStop(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (TestLaraSlide(item, coll))
		return;

	if (lara.water_status == LW_WADE && coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if (!(input & IN_LSTEP))
			item->goal_anim_state = AS_STOP;

		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -728)
				lara.turn_rate = -728;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 728)
				lara.turn_rate = 728;
		}
	}
}

void lara_col_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stepright(item, coll);
}

void lara_col_roll2(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.lara_node = LMX_HIPS;
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot - 0x8000;
	coll->slopes_are_walls = 1;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor > 200)
	{
		item->anim_number = ANIM_FALLBACK;
		item->frame_number = anims[ANIM_FALLBACK].frame_base;
		item->current_anim_state = AS_FALLBACK;
		item->goal_anim_state = AS_FALLBACK;
		item->gravity_status = 1;
		item->fallspeed = 0;
		return;
	}

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_JUMP && !(input & IN_BACK))
		item->goal_anim_state = AS_FORWARDJUMP;
}

void lara_col_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	lara_slide_slope(item, coll);
}

void lara_slide_slope(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -512;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	LaraDeflectEdge(item, coll);

	if (coll->mid_floor <= 200)
	{
		TestLaraSlide(item, coll);
		item->pos.y_pos += coll->mid_floor;

		if (abs(coll->tilt_x) <= 2 && abs(coll->tilt_z) <= 2)
		{
			item->goal_anim_state = AS_STOP;
			StopSoundEffect(SFX_LARA_SLIPPING);
		}
	}
	else
	{
		if (item->current_anim_state == AS_SLIDE)
		{
			item->anim_number = ANIM_FALLDOWN;
			item->frame_number = anims[ANIM_FALLDOWN].frame_base;
			item->current_anim_state = AS_FORWARDJUMP;
			item->goal_anim_state = AS_FORWARDJUMP;
		}
		else
		{
			item->anim_number = ANIM_FALLBACK;
			item->frame_number = anims[ANIM_FALLBACK].frame_base;
			item->current_anim_state = AS_FALLBACK;
			item->goal_anim_state = AS_FALLBACK;
		}

		StopSoundEffect(SFX_LARA_SLIPPING);
		item->gravity_status = 1;
		item->fallspeed = 0;
	}
}

void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	coll->hit_ceiling = 0;
	coll->facing = item->speed >= 0 ? lara.move_angle : lara.move_angle - 0x8000;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && !coll->hit_static)
	{
		if (lara.CanMonkeySwing && coll->coll_type == CT_TOP)
		{
			item->goal_anim_state = AS_HANG2;
			item->current_anim_state = AS_HANG2;
			item->anim_number = ANIM_UPJUMPGRAB;
			item->frame_number = anims[ANIM_UPJUMPGRAB].frame_base;
			item->gravity_status = 0;
			item->speed = 0;
			item->fallspeed = 0;
			lara.gun_status = LG_HANDS_BUSY;
			MonkeySwingSnap(item, coll);
			return;
		}

		if (coll->coll_type == CT_FRONT && coll->mid_ceiling <= -384)
		{
			edge_catch = LaraTestEdgeCatch(item, coll, &edge);

			if (edge_catch && (edge_catch >= 0 || LaraTestHangOnClimbWall(item, coll)))
			{
				angle = item->pos.y_rot;

				if (angle >= -0x18E2 && angle <= 0x18E2)
					angle = 0;
				else if (angle >= 0x271E && angle <= 0x58E2)
					angle = 0x4000;
				else if (angle >= 0x671D || angle <= -0x671D)
					angle = -0x8000;
				else if (angle >= -0x58E2 && angle <= -0x271E)
					angle = -0x4000;

				if (!(angle & 0x3FFF))
				{
					if (TestHangSwingIn(item, angle))
					{
						item->anim_number = ANIM_UPJUMPGRAB;
						item->frame_number = anims[ANIM_UPJUMPGRAB].frame_base;
						item->current_anim_state = AS_HANG2;
						item->goal_anim_state = AS_HANG2;
					}
					else
					{
						item->anim_number = ANIM_GRABLEDGE;
						item->frame_number = anims[ANIM_GRABLEDGE].frame_base + 12;
						item->current_anim_state = AS_HANG;
						item->goal_anim_state = AS_HANG;
					}

					bounds = GetBoundsAccurate(item);

					if (edge_catch <= 0)
						item->pos.y_pos = edge - bounds[2] + 4;
					else
						item->pos.y_pos += coll->front_floor - bounds[2];

					item->pos.x_pos += coll->shift.x;
					item->pos.z_pos += coll->shift.z;
					item->pos.y_rot = angle;
					item->gravity_status = 0;
					item->speed = 0;
					item->fallspeed = 0;
					lara.gun_status = LG_HANDS_BUSY;
					lara.torso_y_rot = 0;
					lara.torso_x_rot = 0;
					return;
				}
			}
		}
	}

	ShiftItem(item, coll);

	if (coll->coll_type == CT_CLAMP || coll->coll_type == CT_TOP || coll->coll_type == CT_TOP_FRONT || coll->hit_ceiling)
		item->fallspeed = 1;

	if (coll->coll_type != CT_NONE)
		item->speed = item->speed <= 0 ? -2 : 2;
	else if (item->fallspeed < -70)
	{
		if (input & IN_FORWARD && item->speed < 5)
			item->speed++;
		else if (input & IN_BACK && item->speed > -5)
			item->speed -= 2;
	}

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->gravity_status = 0;
		item->fallspeed = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void MonkeySwingSnap(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_num;
	
	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 704;
}

void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 0;
	camera.target_elevation = -8190;

	if (!(input & (IN_LEFT | IN_LSTEP)))
		item->goal_anim_state = AS_HANG;
}

void lara_col_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x4000;
	coll->radius = 102;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot - 0x4000;
}

void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 0;
	camera.target_elevation = -8190;

	if (!(input & (IN_RIGHT | IN_RSTEP)))
		item->goal_anim_state = AS_HANG;
}

void lara_col_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x4000;
	coll->radius = 102;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot + 0x4000;
}

void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_JUMP && !(input & IN_FORWARD))
		item->goal_anim_state = AS_BACKJUMP;
}

void lara_col_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	lara_slide_slope(item, coll);
}

void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = 0x3FFC;
	camera.target_elevation = -4550;
	camera.lara_node = LMX_TORSO;
}

void lara_as_pullblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = 6370;
	camera.target_elevation = -4550;
	camera.lara_node = LMX_TORSO;
}

void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 13650;

	if (!(input & IN_ACTION))
		item->goal_anim_state = AS_STOP;
}

void lara_as_usepuzzle(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;

	if (item->frame_number == anims[item->anim_number].frame_end && item->item_flags[0])
	{
		item->anim_number = item->item_flags[0];
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = AS_CONTROLLED;
	}
}

void lara_as_usekey(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;
}

void lara_as_switchoff(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;
	camera.speed = 6;
}

void lara_as_switchon(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;
	camera.speed = 6;

	if (item->frame_number == anims[ANIM_AIRLOCKSW].frame_end - 1 || item->frame_number == anims[ANIM_CROWDOVE].frame_end - 1)
		lara.gun_status = LG_NO_ARMS;
}

void lara_as_pickupflare(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 23660;
	camera.target_elevation = -2730;
	camera.target_distance = 1024;

	if (item->frame_number == anims[item->anim_number].frame_end - 1)
		lara.gun_status = LG_NO_ARMS;
}

void lara_as_pickup(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -23660;
	camera.target_elevation = -2730;
	camera.target_distance = 1024;

	if (item->frame_number == anims[ANIM_CROWBAR].frame_end - 1)
		lara.gun_status = LG_NO_ARMS;
}

void lara_as_special(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.flags = 1;
	camera.target_angle = 30940;
	camera.target_elevation = -4550;
}

void lara_as_swandive(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 1;
	coll->enable_spaz = 0;

	if (item->fallspeed > 131 && item->goal_anim_state != AS_DIVE)
		item->goal_anim_state = AS_FASTDIVE;
}

void lara_col_swandive(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		item->goal_anim_state = AS_STOP;
		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_ROLL && item->goal_anim_state == AS_FASTDIVE)
		item->goal_anim_state = AS_TWIST;

	coll->enable_baddie_push = 1;
	coll->enable_spaz = 0;
	item->speed = 95 * item->speed / 100;
}

void lara_col_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (item->fallspeed <= 133)
			item->goal_anim_state = AS_STOP;
		else
			item->goal_anim_state = AS_DEATH;

		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_gymnast(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (item->frame_number == anims[159].frame_base + 157 || item->frame_number == anims[159].frame_base + 188)
		AddFootprint(item);
}

void lara_as_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	short room_num;

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.IsDucked = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	room_num = lara_item->room_number;
	GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_num);

	if (!(input & IN_DUCK) && !lara.keep_ducked)
		return;

	if (lara.water_status == LW_WADE || lara.gun_status != LG_NO_ARMS)
		return;

	if (lara_item->anim_number != ANIM_DUCKBREATHE && lara_item->anim_number != 245)
		return;

	if (input & (IN_B | IN_DRAW))
		return;

	if (lara.gun_type == WEAPON_FLARE && (!lara.flare_age || lara.flare_age >= 900))
		return;

	if (input & (IN_FORWARD | IN_BACK) && !(room[room_num].flags & ROOM_UNDERWATER))
	{
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_SPRINT && LaraFloorFront(item, item->pos.y_rot, 512) < 384 && LaraFloorFront(item, item->pos.y_rot, 256) >= -384)
	{
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		item->anim_number = 218;
		item->frame_number = anims[218].frame_base;
		item->current_anim_state = AS_DUCKROLL;
		item->goal_anim_state = AS_DUCKROLL;
	}
}

void lara_col_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->facing = item->pos.y_rot;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	lara.keep_ducked = coll->mid_ceiling >= -362;
	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;

	if (input & IN_DUCK && lara.water_status != LW_WADE || lara.keep_ducked || item->anim_number != ANIM_DUCKBREATHE)
	{
		if (input & IN_LEFT)
			item->goal_anim_state = AS_DUCKROTL;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_DUCKROTR;
	}
	else
		item->goal_anim_state = AS_STOP;
}

void MonkeySwingFall(ITEM_INFO* item)
{
	item->goal_anim_state = AS_UPJUMP;
	item->current_anim_state = AS_UPJUMP;
	item->anim_number = ANIM_STOPHANG;
	item->frame_number = anims[ANIM_STOPHANG].frame_base + 9;
	item->speed = 2;
	item->gravity_status = 1;
	item->fallspeed = 1;
	item->pos.y_pos += 256;
	lara.gun_status = LG_NO_ARMS;
}

void lara_as_hang2(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (lara.CanMonkeySwing)
	{
		if (!(input & IN_ACTION) || item->hit_points <= 0)
		{
			MonkeySwingFall(item);
			return;
		}

		camera.target_angle = 0;
		camera.target_elevation = -8190;
	}

	if (input & IN_LOOK)
		LookUpDown();
}

void lara_col_hang2(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = 0;

	if (lara.CanMonkeySwing)
	{
		coll->bad_pos = -NO_HEIGHT;
		coll->bad_neg = NO_HEIGHT;
		coll->bad_ceiling = 0;
		coll->slopes_are_walls = 0;
		lara.move_angle = item->pos.y_rot;
		coll->facing = lara.move_angle;
		coll->radius = 100;
		GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

		if (input & IN_FORWARD && coll->coll_type != CT_FRONT && abs(coll->mid_ceiling - coll->front_ceiling) < 50)
			item->goal_anim_state = AS_MONKEYSWING;
		else if (input & IN_LSTEP && TestMonkeyLeft(item, coll))
			item->goal_anim_state = AS_MONKEYL;
		else if (input & IN_RSTEP && TestMonkeyRight(item, coll))
			item->goal_anim_state = AS_MONKEYR;
		else if (input & IN_LEFT)
			item->goal_anim_state = AS_HANGTURNL;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_HANGTURNR;

		MonkeySwingSnap(item, coll);
		return;
	}

	LaraHangTest(item, coll);

	if (item->goal_anim_state != AS_HANG2)
		return;

	TestForObjectOnLedge(item, coll);

	if (!(input & IN_FORWARD) || coll->front_floor <= -850 || coll->front_floor >= -650 || coll->front_floor < coll->front_ceiling ||
		coll->left_floor2 < coll->left_ceiling2 || coll->right_floor2 < coll->right_ceiling2 || coll->hit_static)
	{
		if (!(input & IN_FORWARD) || coll->front_floor <= -850 || coll->front_floor >= -650 || coll->front_floor - coll->front_ceiling < -256 ||
			coll->left_floor2 - coll->left_ceiling2 < -256 || coll->right_floor2 - coll->right_ceiling2 < -256 || coll->hit_static)
		{
			if (input & IN_LEFT || input & IN_LSTEP)
				item->goal_anim_state = AS_HANGLEFT;
			else if (input & IN_RIGHT || input & IN_RSTEP)
				item->goal_anim_state = AS_HANGRIGHT;
		}
		else
		{
			item->goal_anim_state = AS_HANG2DUCK;
			item->required_anim_state = AS_DUCK;
		}
	}
	else if (input & IN_WALK)
		item->goal_anim_state = AS_GYMNAST;
	else if (input & IN_DUCK)
	{
		item->goal_anim_state = AS_HANG2DUCK;
		item->required_anim_state = AS_DUCK;
	}
	else
		item->goal_anim_state = AS_NULL;
}

void lara_as_monkeyswing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_FORWARD)
		item->goal_anim_state = AS_MONKEYSWING;
	else
		item->goal_anim_state = AS_HANG2;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -546)
			lara.turn_rate = -546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 546)
			lara.turn_rate = 546;
	}
}

void lara_col_monkeyswing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (coll->coll_type == CT_FRONT || abs(coll->mid_ceiling - coll->front_ceiling) > 50)
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
	else
	{
		if (abs(coll->mid_ceiling - coll->left_ceiling2) <= 50)
		{
			if (abs(coll->mid_ceiling - coll->right_ceiling2) > 50)
			{
				ShiftItem(item, coll);
				item->pos.y_rot -= 910;
			}
		}
		else
		{
			ShiftItem(item, coll);
			item->pos.y_rot += 910;
		}

		camera.target_elevation = 1820;
		MonkeySwingSnap(item, coll);
	}
}

void lara_as_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_LSTEP)
		item->goal_anim_state = AS_MONKEYL;
	else
		item->goal_anim_state = AS_HANG2;

	camera.target_elevation = 1820;
}

void lara_col_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	if (TestMonkeyLeft(item, coll))
		MonkeySwingSnap(item, coll);
	else
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
}

void lara_as_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_RSTEP)
		item->goal_anim_state = AS_MONKEYR;
	else
		item->goal_anim_state = AS_HANG2;

	camera.target_elevation = 1820;
}

void lara_col_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	if (TestMonkeyRight(item, coll))
		MonkeySwingSnap(item, coll);
	else
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
}

void lara_as_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	item->goal_anim_state = AS_HANG2;
}

void lara_col_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_monkeyswing(item, coll);
}

void lara_as_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	MESH_INFO* StaticMesh;
	GAME_VECTOR s;
	GAME_VECTOR d;
	PHD_VECTOR v;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (input & IN_JUMP)
	{
		if (LaraFloorFront(item, item->pos.y_rot, 768) >= 512 && LaraCeilingFront(item, item->pos.y_rot, 768, 512) != NO_HEIGHT &&
			LaraCeilingFront(item, item->pos.y_rot, 768, 512) <= 0)
		{
			s.x = lara_item->pos.x_pos;
			s.y = lara_item->pos.y_pos - 96;
			s.z = lara_item->pos.z_pos;
			s.room_number = lara_item->room_number;

			d.x = s.x + (768 * phd_sin(lara_item->pos.y_rot) >> W2V_SHIFT);
			d.y = s.y + 160;
			d.z = s.z + (768 * phd_cos(lara_item->pos.y_rot) >> W2V_SHIFT);

			if (LOS(&s, &d))
			{
				if (ObjectOnLOS2(&s, &d, &v, &StaticMesh) == 999 && 
					(lara_item->anim_number == 263 || lara_item->anim_number == 264 || item->anim_number == 266 || item->anim_number == 268))
				{
					item->anim_number = ANIM_CRAWLJUMP;
					item->frame_number = anims[ANIM_CRAWLJUMP].frame_base;
					item->goal_anim_state = AS_CONTROLLED;
					item->current_anim_state = AS_CONTROLLED;
				}
			}
		}
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (item->anim_number == ANIM_DUCK_TO_ALL4S)
		lara.gun_status = LG_HANDS_BUSY;

	camera.target_elevation = -4186;
}

void lara_col_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	ITEM_INFO** itemlist;
	MESH_INFO** meshlist;
	long x, z, slope, collided;
	short height, heightl, heightr;

	item->fallspeed = 0;
	item->gravity_status = 0;

	if (item->goal_anim_state == AS_CRAWL2HANG)
		return;

	lara.move_angle = item->pos.y_rot;
	coll->facing = item->pos.y_rot;
	coll->radius = 200;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	slope = abs(coll->left_floor2 - coll->right_floor2);
	lara.keep_ducked = coll->mid_ceiling >= -362;
	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
		item->pos.y_pos += coll->mid_floor;

	if ((!(input & IN_DUCK) && !lara.keep_ducked) || ((input & (IN_FLARE | IN_DRAW)) && !(input & IN_FORWARD)) || lara.water_status == LW_WADE)
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	if (tomb5.flexible_crawling)
		collided = item->anim_number == ANIM_ALL4S || item->anim_number == ANIM_ALL4S2 || item->anim_number == 266 || item->anim_number == 268;
	else
		collided = item->anim_number == ANIM_ALL4S || item->anim_number == ANIM_ALL4S2;

	if (!collided)
		return;

	if (input & IN_FORWARD)
	{
		if (abs(LaraFloorFront(item, item->pos.y_rot, 256)) < 255 && height_type != BIG_SLOPE)
			item->goal_anim_state = AS_CRAWL;

		return;
	}
	
	if (input & IN_BACK)
	{
		height = LaraCeilingFront(item, item->pos.y_rot, -300, 128);
		heightl = 0;
		heightr = 0;

		if (height == NO_HEIGHT || height > 256)
			return;

		if (input & IN_ACTION)
		{
			x = item->pos.x_pos;
			z = item->pos.z_pos;
			item->pos.x_pos += 128 * phd_sin(item->pos.y_rot - 0x4000) >> W2V_SHIFT;
			item->pos.z_pos += 128 * phd_cos(item->pos.y_rot - 0x4000) >> W2V_SHIFT;
			heightl = LaraFloorFront(item, item->pos.y_rot, -300);
			item->pos.x_pos += 256 * phd_sin(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
			item->pos.z_pos += 256 * phd_cos(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
			heightr = LaraFloorFront(item, item->pos.y_rot, -300);
			item->pos.x_pos = x;
			item->pos.z_pos = z;
		}

		height = LaraFloorFront(item, item->pos.y_rot, -300);

		if (height < 255 && height > -255 && height_type != BIG_SLOPE)
		{
			item->goal_anim_state = AS_CRAWLBACK;
			return;
		}
		
		if (input & IN_ACTION && height > 768 && heightl > 768 && heightr > 768 && slope < 120)
		{
			x = item->pos.x_pos;
			z = item->pos.z_pos;
			item->pos.x_pos -= 100 * phd_sin(coll->facing) >> W2V_SHIFT;
			item->pos.z_pos -= 100 * phd_cos(coll->facing) >> W2V_SHIFT;
			itemlist = (ITEM_INFO**)&tsv_buffer[0];
			meshlist = (MESH_INFO**)&tsv_buffer[1024];
			collided = GetCollidedObjects(item, 100, 1, itemlist, meshlist, 0);
			item->pos.x_pos = x;
			item->pos.z_pos = z;

			if (!collided)
			{
				switch ((ushort)(item->pos.y_rot + 0x2000) / 0x4000)
				{
				case NORTH:
					item->pos.y_rot = 0;
					item->pos.z_pos = (item->pos.z_pos & ~1023) + 225;
					break;

				case EAST:
					item->pos.y_rot = 0x4000;
					item->pos.x_pos = (item->pos.x_pos & ~1023) + 225;
					break;

				case SOUTH:
					item->pos.y_rot = -0x8000;
					item->pos.z_pos = (item->pos.z_pos | 1023) - 225;
					break;

				case WEST:
					item->pos.y_rot = -0x4000;
					item->pos.x_pos = (item->pos.x_pos | 1023) - 225;
					break;
				}

				item->goal_anim_state = AS_CRAWL2HANG;
			}
		}

		return;
	}
	
	if (input & IN_LEFT)
	{
		item->anim_number = ANIM_ALL4TURNL;
		item->frame_number = anims[ANIM_ALL4TURNL].frame_base;
		item->current_anim_state = AS_ALL4TURNL;
		item->goal_anim_state = AS_ALL4TURNL;
	}
	else if (input & IN_RIGHT)
	{
		item->anim_number = ANIM_ALL4TURNR;
		item->frame_number = anims[ANIM_ALL4TURNR].frame_base;
		item->current_anim_state = AS_ALL4TURNR;
		item->goal_anim_state = AS_ALL4TURNR;
	}
}

void lara_as_crawl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || input & IN_JUMP)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	camera.target_elevation = -4186;

	if (input & IN_FORWARD && (input & IN_DUCK || lara.keep_ducked) && lara.water_status != LW_WADE)
	{
		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -546)
				lara.turn_rate = -546;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 546)
				lara.turn_rate = 546;
		}
	}
	else
		item->goal_anim_state = AS_ALL4S;
}

void lara_col_crawl(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->radius = 200;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

	if (LaraDeflectEdgeDuck(item, coll))
	{
		item->current_anim_state = AS_ALL4S;
		item->goal_anim_state = AS_ALL4S;

		if (item->anim_number != ANIM_ALL4S)
		{
			item->anim_number = ANIM_ALL4S;
			item->frame_number = anims[ANIM_ALL4S].frame_base;
		}

		return;
	}

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
		item->pos.y_pos += coll->mid_floor;
}

long LaraDeflectEdgeDuck(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->gravity_status = 0;
		item->speed = 0;
		return 1;
	}
	
	if (coll->coll_type == CT_LEFT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot += 364;
	}
	else if (coll->coll_type == CT_RIGHT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot -= 364;
	}

	return 0;
}

void lara_as_hangturnl(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = 1820;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	item->pos.y_rot -= 273;

	if (item->hit_points <= 0 || !(input & IN_LEFT))
		item->goal_anim_state = AS_HANG2;
}

void lara_as_hangturnr(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = 1820;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	item->pos.y_rot += 273;

	if (item->hit_points <= 0 || !(input & IN_RIGHT))
		item->goal_anim_state = AS_HANG2;
}

void lara_col_hangturnlr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	coll->facing = item->pos.y_rot;
	coll->radius = 100;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);
	MonkeySwingSnap(item, coll);
}

void lara_as_all4turnl(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	camera.target_elevation = -4186;
	item->pos.y_rot -= 273;

	if (item->hit_points <= 0 || !(input & IN_LEFT))
		item->goal_anim_state = AS_ALL4S;
}

void lara_as_all4turnr(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	camera.target_elevation = -4186;
	item->pos.y_rot += 273;

	if (item->hit_points <= 0 || !(input & IN_RIGHT))
		item->goal_anim_state = AS_ALL4S;
}

void lara_col_all4turnlr(ITEM_INFO* item, COLL_INFO* coll)
{
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);
	TiltHer(item, 140, 400);

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_crawlb(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || lara.water_status == LW_WADE)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	camera.target_elevation = -4186;

	if (input & IN_BACK)
	{
		if (input & IN_RIGHT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -546)
				lara.turn_rate = -546;
		}
		else if (input & IN_LEFT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 546)
				lara.turn_rate = 546;
		}
	}
	else
		item->goal_anim_state = AS_ALL4S;
}

void lara_col_crawlb(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->radius = 250;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	lara.move_angle = item->pos.y_rot + 0x8000;
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

	if (LaraDeflectEdgeDuck(item, coll))
	{
		item->current_anim_state = AS_ALL4S;
		item->goal_anim_state = AS_ALL4S;

		if (item->anim_number != ANIM_ALL4S)
		{
			item->anim_number = ANIM_ALL4S;
			item->frame_number = anims[ANIM_ALL4S].frame_base;
		}

		return;
	}

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
		item->pos.y_pos += coll->mid_floor;

	lara.move_angle = item->pos.y_rot;
}

void lara_col_crawl2hang(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	camera.target_angle = 0;
	camera.target_elevation = -8190;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (item->anim_number != ANIM_CRAWL_TO_HANG)
		return;

	item->fallspeed = 512;
	item->pos.y_pos += 255;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);
	edge_catch = LaraTestEdgeCatch(item, coll, &edge);

	if (!edge_catch)
		return;

	if (edge_catch < 0 && !LaraTestHangOnClimbWall(item, coll))
		return;

	angle = item->pos.y_rot;

	if (angle >= -0x18E2 && angle <= 0x18E2)
		angle = 0;
	else if (angle >= 0x271E && angle <= 0x58E2)
		angle = 0x4000;
	else if (angle >= 0x671D || angle <= -0x671D)
		angle = -0x8000;
	else if (angle >= -0x58E2 && angle <= -0x271E)
		angle = -0x4000;

	if (angle & 0x3FFF)
		return;

	if (TestHangSwingIn(item, angle))
	{
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		item->anim_number = ANIM_HANG2STOP;
		item->frame_number = anims[ANIM_HANG2STOP].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
	else
	{
		item->anim_number = ANIM_GRABLEDGE;
		item->frame_number = anims[ANIM_GRABLEDGE].frame_base;
		item->current_anim_state = AS_HANG;
		item->goal_anim_state = AS_HANG;
	}

	bounds = GetBoundsAccurate(item);

	if (edge_catch <= 0)
		item->pos.y_pos = edge - bounds[2];
	else
	{
		item->pos.y_pos += coll->front_floor - bounds[2];
		item->pos.x_pos += coll->shift.x;
		item->pos.z_pos += coll->shift.z;
	}

	item->gravity_status = 1;
	item->pos.y_rot = angle;
	item->speed = 2;
	item->fallspeed = 1;
	lara.gun_status = LG_HANDS_BUSY;
}

void lara_as_controlled(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (item->frame_number == anims[item->anim_number].frame_end - 1)
	{
		lara.gun_status = LG_NO_ARMS;

		if (UseForcedFixedCamera)
			UseForcedFixedCamera = 0;
	}
}

void lara_as_controlledl(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
}

void lara_col_roll(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->slopes_are_pits = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (TestLaraSlide(item, coll))
		return;

	if (LaraFallen(item, coll))
		return;

	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_pose(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stop(item, coll);
}

void lara_as_waterout(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.lara_node = LMX_HIPS;
}

void lara_as_deathslide(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	camera.target_angle = 12740;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	coll->trigger = trigger_index;

	if (!(input & IN_ACTION))
	{
		item->goal_anim_state = AS_FORWARDJUMP;
		AnimateLara(item);
		lara_item->gravity_status = 1;
		lara_item->speed = 100;
		lara_item->fallspeed = 40;
		lara.move_angle = item->pos.y_rot;
	}
}

void lara_col_turnswitch(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->old.x != item->pos.x_pos || coll->old.z != item->pos.z_pos)
	{
		if (item->anim_number == ANIM_TURNSWITCHAC)
		{
			item->pos.y_rot -= 0x4000;
			item->anim_number = ANIM_TURNSWITCHAD;
			item->frame_number = anims[ANIM_TURNSWITCHAD].frame_base;
		}

		if (item->anim_number == ANIM_TURNSWITCHCC)
		{
			item->pos.y_rot += 0x4000;
			item->anim_number = ANIM_TURNSWITCHCD;
			item->frame_number = anims[ANIM_TURNSWITCHCD].frame_base;
		}
	}
}

void lara_as_poleleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (!(input & IN_LEFT) || !(input & IN_ACTION) || input & (IN_FORWARD | IN_BACK) || item->hit_points <= 0)
		item->goal_anim_state = AS_POLESTAT;
	else
		item->pos.y_rot += 256;
}

void lara_as_poleright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (!(input & IN_RIGHT) || !(input & IN_ACTION) || input & (IN_FORWARD | IN_BACK) || item->hit_points <= 0)
		item->goal_anim_state = AS_POLESTAT;
	else
		item->pos.y_rot -= 256;
}

void lara_col_polestat(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_num;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_FASTFALL;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (item->anim_number != ANIM_POLESTAT)
		return;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (input & IN_ACTION)
	{
		item->goal_anim_state = AS_POLESTAT;

		if (input & IN_LEFT)
			item->goal_anim_state = AS_POLELEFT;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_POLERIGHT;

		if (input & IN_LOOK)
			LookUpDown();

		if (input & IN_FORWARD)
		{
			room_num = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);

			if (item->pos.y_pos - GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) > 1024)
				item->goal_anim_state = AS_POLEUP;
		}
		else if (input & IN_BACK && coll->mid_floor > 0)
		{
			item->goal_anim_state = AS_POLEDOWN;
			item->item_flags[2] = 0;
		}

		if (input & IN_JUMP)
			item->goal_anim_state = AS_BACKJUMP;
	}
	else if (coll->mid_floor <= 0)
		item->goal_anim_state = AS_STOP;
	else
	{
		item->goal_anim_state = AS_FASTFALL;
		item->pos.x_pos -= 64 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.z_pos -= 64 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
	}
}

void lara_col_poleup(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_num;

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;

	if (input & IN_LOOK)
		LookUpDown();

	if (!(input & IN_ACTION) || !(input & IN_FORWARD) || item->hit_points <= 0)
		item->goal_anim_state = AS_POLESTAT;

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);

	if (item->pos.y_pos - GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) < 1024)
		item->goal_anim_state = AS_POLESTAT;
}

void lara_col_poledown(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (input & IN_LOOK)
		LookUpDown();

	if ((input & (IN_BACK | IN_ACTION)) != (IN_BACK | IN_ACTION) || item->hit_points <= 0)
		item->goal_anim_state = AS_POLESTAT;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	coll->slopes_are_walls = 1;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	GetLaraCollisionInfo(item, coll);

	if (coll->mid_floor < 0)
	{
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos - 762, item->pos.z_pos);
		item->goal_anim_state = AS_POLESTAT;
		item->item_flags[2] = 0;
	}

	if (input & IN_LEFT)
		item->pos.y_rot += 256;
	else if (input & IN_RIGHT)
		item->pos.y_rot -= 256;

	if (item->anim_number == ANIM_POLESLIDEE)
		item->item_flags[2] -= 1024;
	else
		item->item_flags[2] += 256;

	SoundEffect(SFX_LARA_ROPEDOWN_LOOP, &item->pos, SFX_DEFAULT);
	
	if (item->item_flags[2] > 0x4000)
		item->item_flags[2] = 0x4000;
	else if (item->item_flags[2] < 0)
		item->item_flags[2] = 0;

	item->pos.y_pos += item->item_flags[2] >> 8;
}

void lara_as_duckl(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;

	if ((input & (IN_DUCK | IN_LEFT)) != (IN_DUCK | IN_LEFT) || item->hit_points <= 0)
		item->goal_anim_state = AS_DUCK;

	item->pos.y_rot -= 273;
}

void lara_as_duckr(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;

	if ((input & (IN_DUCK | IN_RIGHT)) != (IN_DUCK | IN_RIGHT) || item->hit_points <= 0)
		item->goal_anim_state = AS_DUCK;

	item->pos.y_rot += 273;
}

void lara_col_ducklr(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.IsDucked = 1;

	if (input & IN_LOOK)
		LookUpDown();

	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->facing = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	lara.keep_ducked = coll->mid_ceiling >= -362;
	ShiftItem(item, coll);

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_extcornerl(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.lara_node = LMX_HEAD;
	camera.target_elevation = -6144;
	SetCornerAnim(item, coll, 0x4000, item->anim_number == ANIM_EXTCORNERL_END || item->anim_number == ANIM_EXTCLIMBL_END);
}

void lara_as_extcornerr(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.lara_node = LMX_HEAD;
	camera.target_elevation = -6144;
	SetCornerAnim(item, coll, -0x4000, item->anim_number == ANIM_EXTCORNERR_END || item->anim_number == ANIM_EXTCLIMBR_END);
}

void lara_as_intcornerl(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.lara_node = LMX_HEAD;
	camera.target_elevation = -6144;
	SetCornerAnim(item, coll, -0x4000, item->anim_number == ANIM_INTCORNERL_END || item->anim_number == ANIM_INTCLIMBL_END);
}

void lara_as_intcornerr(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.lara_node = LMX_HEAD;
	camera.target_elevation = -6144;
	SetCornerAnim(item, coll, 0x4000, item->anim_number == ANIM_INTCORNERR_END || item->anim_number == ANIM_INTCLIMBR_END);
}

void lara_as_pulley(ITEM_INFO* item, COLL_INFO* coll)
{
	ITEM_INFO* p;
	
	p = (ITEM_INFO*)lara.GeneralPtr;
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (input & IN_ACTION && p->trigger_flags)
		item->goal_anim_state = AS_PULLEY;
	else
		item->goal_anim_state = AS_STOP;

	if (item->anim_number == ANIM_PULLEY && item->frame_number == anims[ANIM_PULLEY].frame_base + 44)
	{
		if (p->trigger_flags)
		{
			p->trigger_flags--;

			if (p->trigger_flags)
			{
				if (p->item_flags[2])
				{
					p->item_flags[2] = 0;
					p->status = ITEM_DEACTIVATED;
				}
			}
			else
			{
				if (!p->item_flags[1])
					p->status = ITEM_DEACTIVATED;

				p->item_flags[2] = 1;

				if (p->item_flags[3] >= 0)
					p->trigger_flags = abs(p->item_flags[3]);
				else
					p->item_flags[0] = 1;
			}
		}
	}

	if (item->anim_number == ANIM_PULLEY2STAT && item->frame_number == anims[ANIM_PULLEY2STAT].frame_end - 1)
		lara.gun_status = LG_NO_ARMS;
}

void lara_as_rope(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION))
		FallFromRope(item);

	if (input & IN_LOOK)
		LookUpDown();
}

void lara_col_rope(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_ACTION)
	{
		UpdateRopeSwing(item);

		if (input & IN_SPRINT)
		{
			lara.RopeDFrame = (anims[ANIM_SWINGFWD].frame_base + 32) << 8;
			lara.RopeFrame = lara.RopeDFrame;
			item->goal_anim_state = AS_ROPEFWD;
		}
		else if (input & IN_FORWARD && lara.RopeSegment > 4)
			item->goal_anim_state = AS_CLIMBROPE;
		else if (input & IN_BACK && lara.RopeSegment < 21)
		{
			item->goal_anim_state = AS_SLIDEROPE;
			lara.RopeFlag = 0;
			lara.RopeCount = 0;
		}
		else if (input & IN_LEFT)
			item->goal_anim_state = AS_ROPELEFT;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_ROPERIGHT;
	}
	else
		FallFromRope(item);
}

void lara_as_climbrope(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_ROLL)
		FallFromRope(item);
	else
	{
		camera.target_angle = 5460;

		if (anims[item->anim_number].frame_end == item->frame_number)
		{
			item->frame_number = anims[item->anim_number].frame_base;
			lara.RopeSegment -= 2;
		}

		if (!(input & IN_FORWARD) || lara.RopeSegment <= 4)
			item->goal_anim_state = AS_ROPE;
	}
}

void lara_as_climbroped(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraClimbRope(item, coll);
}

void lara_col_ropefwd(ITEM_INFO* item, COLL_INFO* coll)
{
	long Vel;

	camera.target_distance = 2048;
	UpdateRopeSwing(item);

	if (item->anim_number == ANIM_SWINGFWD)
	{
		if (input & IN_SPRINT)
		{
			if (abs(lara.RopeLastX) < 9000)
				Vel = 192 * (9000 - abs(lara.RopeLastX)) / 9000;
			else
				Vel = 0;

			ApplyVelocityToRope(lara.RopeSegment - 2, item->pos.y_rot + (!lara.RopeDirection ? 0x7FF8 : 0), (ushort)(Vel >> 5));
		}

		if (lara.RopeFrame > lara.RopeDFrame)
		{
			lara.RopeFrame -= lara.RopeFrameRate;

			if (lara.RopeFrame < lara.RopeDFrame)
				lara.RopeFrame = lara.RopeDFrame;
		}
		else if (lara.RopeFrame < lara.RopeDFrame)
		{
			lara.RopeFrame += lara.RopeFrameRate;

			if (lara.RopeFrame > lara.RopeDFrame)
				lara.RopeFrame = lara.RopeDFrame;
		}

		item->frame_number = (short)(lara.RopeFrame >> 8);

		if (!(input & IN_SPRINT) && lara.RopeFrame >> 8 == anims[ANIM_SWINGFWD].frame_base + 32 && lara.RopeMaxXBackward < 6750 && lara.RopeMaxXForward < 6750)
		{
			item->anim_number = ANIM_SWING2HANG;
			item->frame_number = anims[ANIM_SWING2HANG].frame_base;
			item->current_anim_state = AS_ROPE;
			item->goal_anim_state = AS_ROPE;
		}

		if (input & IN_JUMP)
			JumpOffRope(item);
	}
	else if (item->frame_number == anims[ANIM_ROPEKICK].frame_base + 15)
		ApplyVelocityToRope(lara.RopeSegment, item->pos.y_rot, 128);
}

void lara_as_ropel(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION))
		FallFromRope(item);
	else if (input & IN_LEFT)
		lara.RopeY += 256;
	else
		item->goal_anim_state = AS_ROPE;
}

void lara_as_roper(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION))
		FallFromRope(item);
	else if (input & IN_RIGHT)
		lara.RopeY -= 256;
	else
		item->goal_anim_state = AS_ROPE;
}

static void GetTighRopeFallOff(long chance)
{
	if (lara_item->hit_points <= 0 || lara_item->hit_status)
	{
		lara_item->anim_number = ANIM_TROPEFALLOFF_L;
		lara_item->frame_number = anims[ANIM_TROPEFALLOFF_L].frame_base;
		lara_item->current_anim_state = AS_TROPEFALL_L;
		lara_item->goal_anim_state = AS_TROPEFALL_L;
	}

	if (!lara.TightRopeFall && !(GetRandomControl() & chance))
	{
		if (GetRandomControl() & 0xF)
			lara.TightRopeFall = 1;
		else
			lara.TightRopeFall = 2;
	}
}

void lara_as_trpose(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_LOOK)
		LookUpDown();

	GetTighRopeFallOff(127);

	if (lara_item->current_anim_state == AS_TROPEFALL_L)
		return;
	
	if (lara.TightRopeFall)
	{
		if (GetRandomControl() & 1)
			item->goal_anim_state = AS_TROPEFALL_R;
		else
			item->goal_anim_state = AS_TROPEFALL_L;
	}
	else if (input & IN_FORWARD)
		item->goal_anim_state = AS_TROPEWALK;
	else if (input & IN_ROLL || input & IN_BACK)
	{
		if (item->anim_number == ANIM_TRPOSE)
		{
			item->current_anim_state = AS_TROPETURN;
			item->anim_number = ANIM_TROPETURN;
			item->frame_number = anims[ANIM_TROPETURN].frame_base;
			GetTighRopeFallOff(1);
		}
	}
}

void lara_as_trwalk(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	if (lara.TightRopeOnCount)
		lara.TightRopeOnCount--;
	else if (lara.TightRopeOff)
	{
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

		if (GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) == item->pos.y_pos)
		{
			lara.TightRopeOff = 0;
			item->goal_anim_state = AS_TROPEGETOFF;
		}
	}
	else
		GetTighRopeFallOff(127);

	if (lara_item->current_anim_state != AS_TROPEFALL_L)
	{
		if (input & IN_LOOK)
			LookUpDown();

		if (item->goal_anim_state != AS_TROPEGETOFF &&
			(lara.TightRopeFall || (input & (IN_BACK | IN_ROLL) || !(input & IN_FORWARD)) && !lara.TightRopeOnCount && !lara.TightRopeOff))
			item->goal_anim_state = AS_TROPEPOSE;
	}
}

void lara_as_trfall(ITEM_INFO* item, COLL_INFO* coll)
{
	PHD_VECTOR pos;
	long UndoInput, WrongInput, UndoAnim, UndoFrame;

	if (item->anim_number == ANIM_TROPEFALLOFF_L || item->anim_number == ANIM_TROPEFALLOFF_R)
	{
		if (item->frame_number == anims[item->anim_number].frame_end)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraJointPos(&pos, LMX_FOOT_R);
			item->pos.x_pos = pos.x;
			item->pos.y_pos = pos.y + 75;
			item->pos.z_pos = pos.z;
			item->goal_anim_state = AS_FASTFALL;
			item->current_anim_state = AS_FASTFALL;
			item->anim_number = ANIM_FASTFALL;
			item->frame_number = anims[ANIM_FASTFALL].frame_base;
			item->fallspeed = 81;
			lara.gun_status = LG_NO_ARMS;
		}

		return;
	}

	if (lara.TightRopeOnCount)
		lara.TightRopeOnCount--;

	if (item->anim_number == ANIM_TROPEFALL_L)
	{
		UndoInput = IN_RIGHT;
		WrongInput = IN_LEFT;
		UndoAnim = ANIM_TROPEUNDO_L;
	}
	else if (item->anim_number == ANIM_TROPEFALL_R)
	{
		UndoInput = IN_LEFT;
		WrongInput = IN_RIGHT;
		UndoAnim = ANIM_TROPEUNDO_R;
	}
	else
		return;

	UndoFrame = anims[item->anim_number].frame_end + anims[UndoAnim].frame_base - item->frame_number;

	if (input & UndoInput && !lara.TightRopeOnCount)
	{
		item->anim_number = (short)UndoAnim;
		item->frame_number = (short)UndoFrame;
		item->current_anim_state = AS_TROPEUNDOFALL;
		item->goal_anim_state = AS_TROPEPOSE;
		lara.TightRopeFall--;
	}
	else if (input & WrongInput && lara.TightRopeOnCount < 10)
		lara.TightRopeOnCount += (GetRandomControl() & 3) + 2;
}

void lara_as_parallelbars(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION))
		item->goal_anim_state = AS_PBLEAP;
}

void lara_as_pbleapoff(ITEM_INFO* item, COLL_INFO* coll)
{
	ITEM_INFO* pitem;
	long Dist;
	
	pitem = (ITEM_INFO*)lara.GeneralPtr;

	item->gravity_status = 1;

	if (item->frame_number == anims[item->anim_number].frame_base)
	{
		if (item->pos.y_rot == pitem->pos.y_rot)
			Dist = pitem->trigger_flags / 100 - 2;
		else
			Dist = pitem->trigger_flags % 100 - 2;

		item->fallspeed = -(short(20 * Dist + 64));
		item->speed = short(20 * Dist + 58);
	}

	if (item->frame_number == anims[item->anim_number].frame_end)
	{
		item->pos.x_pos += 700 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.y_pos -= 361;
		item->pos.z_pos += 700 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		item->anim_number = ANIM_GRABLOOP;
		item->frame_number = anims[ANIM_GRABLOOP].frame_base;
		item->goal_anim_state = AS_REACH;
		item->current_anim_state = AS_REACH;
	}
}

void LaraAboveWater(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->old_anim_state = item->current_anim_state;
	coll->old_anim_number = item->anim_number;
	coll->old_frame_number = item->frame_number;
	coll->radius = 100;
	coll->trigger = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_baddie_push = 1;
	coll->enable_spaz = 1;

	if (input & IN_LOOK && lara.look)
		LookLeftRight();
	else
		ResetLook();

	lara.look = 1;
	lara_control_routines[item->current_anim_state](item, coll);

	if (item->pos.z_rot < -182)
		item->pos.z_rot += 182;
	else if (item->pos.z_rot > 182)
		item->pos.z_rot -= 182;
	else item->pos.z_rot = 0;

	if (item->pos.x_rot < -182)
		item->pos.x_rot += 182;
	else if (item->pos.x_rot > 182)
		item->pos.x_rot -= 182;
	else item->pos.x_rot = 0;

	if (lara.turn_rate < -364)
		lara.turn_rate += 364;
	else if (lara.turn_rate > 364)
		lara.turn_rate -= 364;
	else
		lara.turn_rate = 0;

	item->pos.y_rot += lara.turn_rate;
	AnimateLara(item);
	LaraBaddieCollision(item, coll);
	lara_collision_routines[item->current_anim_state](item, coll);
	UpdateLaraRoom(item, -381);

	if (lara.gun_type == WEAPON_CROSSBOW && !LaserSight && gfLevelFlags & GF_OFFICE)
		input &= ~IN_ACTION;

	LaraGun();
	TestTriggers(coll->trigger, 0, 0);
}

void FallFromRope(ITEM_INFO* item)
{
	item->speed = short((abs(CurrentPendulum.Velocity.x >> 16) + abs(CurrentPendulum.Velocity.z >> 16)) >> 1);
	item->pos.x_rot = 0;
	item->pos.y_pos += 320;
	item->anim_number = ANIM_FALLDOWN;
	item->frame_number = anims[ANIM_FALLDOWN].frame_base;
	item->current_anim_state = AS_FORWARDJUMP;
	item->goal_anim_state = AS_FORWARDJUMP;
	item->fallspeed = 0;
	item->gravity_status = 1;
	lara.gun_status = LG_NO_ARMS;
	lara.RopePtr = -1;
}

void UpdateRopeSwing(ITEM_INFO* item)
{
	static uchar LegsSwinging;
	long temp;

	if (lara.RopeMaxXForward > 9000)
		lara.RopeMaxXForward = 9000;

	if (lara.RopeMaxXBackward > 9000)
		lara.RopeMaxXBackward = 9000;

	if (lara.RopeDirection)
	{
		if (item->pos.x_rot > 0 && item->pos.x_rot - lara.RopeLastX < -100)
		{
			lara.RopeArcFront = lara.RopeLastX;
			lara.RopeDirection = 0;
			lara.RopeMaxXBackward = 0;
			temp = 256 * (15 * lara.RopeMaxXForward / 18000 + anims[ANIM_SWINGFWD].frame_base + 47);

			if (temp > lara.RopeDFrame)
			{
				lara.RopeDFrame = temp;
				LegsSwinging = 1;
			}
			else
				LegsSwinging = 0;

			SoundEffect(SFX_LARA_ROPE_CREAK, &item->pos, 0);
		}
		else if (lara.RopeLastX < 0 && lara.RopeFrame == lara.RopeDFrame)
		{
			LegsSwinging = 0;
			lara.RopeDFrame = 256 * (15 * lara.RopeMaxXBackward / 18000 + anims[ANIM_SWINGFWD].frame_base + 47);
			lara.RopeFrameRate = 15 * lara.RopeMaxXBackward / 9000 + 1;
		}
		else if (lara.RopeFrameRate < 512)
			lara.RopeFrameRate += (LegsSwinging ? 31 : 7) * lara.RopeMaxXBackward / 9000 + 1;
	}
	else if (item->pos.x_rot < 0 && item->pos.x_rot - lara.RopeLastX > 100)
	{
		lara.RopeArcBack = lara.RopeLastX;
		lara.RopeDirection = 1;
		lara.RopeMaxXForward = 0;
		temp = 256 * (anims[ANIM_SWINGFWD].frame_base - 15 * lara.RopeMaxXBackward / 18000 + 17);

		if (temp < lara.RopeDFrame)
		{
			lara.RopeDFrame = temp;
			LegsSwinging = 1;
		}
		else
			LegsSwinging = 0;

		SoundEffect(SFX_LARA_ROPE_CREAK, &item->pos, 0);
	}
	else if (lara.RopeLastX > 0 && lara.RopeFrame == lara.RopeDFrame)
	{
		LegsSwinging = 0;
		lara.RopeDFrame = 256 * (anims[ANIM_SWINGFWD].frame_base - 15 * lara.RopeMaxXForward / 18000 + 17);
		lara.RopeFrameRate = 15 * lara.RopeMaxXForward / 9000 + 1;
	}
	else if (lara.RopeFrameRate < 512)
		lara.RopeFrameRate += (LegsSwinging ? 31 : 7) * lara.RopeMaxXForward / 9000 + 1;

	lara.RopeLastX = item->pos.x_rot;

	if (lara.RopeDirection)
	{
		if (item->pos.x_rot > lara.RopeMaxXForward)
			lara.RopeMaxXForward = item->pos.x_rot;
	}
	else if (item->pos.x_rot < -lara.RopeMaxXBackward)
		lara.RopeMaxXBackward = abs(item->pos.x_rot);
}

void ApplyVelocityToRope(long node, ushort angle, ushort n)
{
	long xvel, zvel;

	xvel = n * phd_sin(angle) >> 2;
	zvel = n * phd_cos(angle) >> 2;
	SetPendulumVelocity(xvel, 0, zvel);
}

void JumpOffRope(ITEM_INFO* item)
{
	if (lara.RopePtr != -1)
	{
		if (item->pos.x_rot >= 0)
		{
			item->fallspeed = -112;
			item->speed = item->pos.x_rot / 128;
		}
		else
		{
			item->speed = 0;
			item->fallspeed = -20;
		}

		item->pos.x_rot = 0;
		item->gravity_status = 1;
		lara.gun_status = LG_NO_ARMS;

		if (item->frame_number - anims[ANIM_SWINGFWD].frame_base > 42)
			item->anim_number = 406;
		else if (item->frame_number - anims[ANIM_SWINGFWD].frame_base > 21)
			item->anim_number = 407;
		else
			item->anim_number = 386;

		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = AS_REACH;
		item->goal_anim_state = AS_REACH;
		lara.RopePtr = -1;
	}
}

long LaraHangTest(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long x, z, oldfloor, hdif, flag;
	short angle, move, wall, ceiling, dir;

	move = 0;
	flag = 0;
	angle = lara.move_angle;

	if (angle == short(item->pos.y_rot - 0x4000))
		move = -100;
	else if (angle == short(item->pos.y_rot + 0x4000))
		move = 100;

	wall = LaraFloorFront(item, angle, 100);

	if (wall < 200)
		flag = 1;

	ceiling = LaraCeilingFront(item, angle, 100, 0);
	dir = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (dir)
	{
	case NORTH:
		item->pos.z_pos += 4;
		break;

	case EAST:
		item->pos.x_pos += 4;
		break;

	case SOUTH:
		item->pos.z_pos -= 4;
		break;

	case WEST:
		item->pos.x_pos -= 4;
		break;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	GetLaraCollisionInfo(item, coll);

	if (lara.climb_status)
	{
		if (input & IN_ACTION && item->hit_points > 0)
		{
			lara.move_angle = angle;

			if (!LaraTestHangOnClimbWall(item, coll))
			{
				if (item->anim_number != ANIM_GRABRIGHT && item->anim_number != ANIM_GRABLEFT)
				{
					SnapLaraToEdgeOfBlock(item, coll, dir);
					item->pos.y_pos = coll->old.y;
					item->anim_number = ANIM_GRABLEDGE;
					item->frame_number = anims[item->anim_number].frame_base + 21;
					item->current_anim_state = AS_HANG;
					item->goal_anim_state = AS_HANG;
				}

				return 1;
			}

			if (item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21 && LaraTestClimbStance(item, coll))
				item->goal_anim_state = AS_CLIMBSTNC;
		}
		else
		{
			item->anim_number = ANIM_FALLDOWN;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = AS_FORWARDJUMP;
			item->goal_anim_state = AS_FORWARDJUMP;
			item->pos.y_pos += 256;
			item->gravity_status = 1;
			item->speed = 2;
			item->fallspeed = 1;
			lara.gun_status = LG_NO_ARMS;
		}

		return 0;
	}
	
	if (!(input & IN_ACTION) || item->hit_points <= 0 || coll->front_floor > 0)
	{
		item->anim_number = ANIM_STOPHANG;
		item->frame_number = anims[item->anim_number].frame_base + 9;
		item->current_anim_state = AS_UPJUMP;
		item->goal_anim_state = AS_UPJUMP;

		bounds = GetBoundsAccurate(item);
		item->pos.x_pos += coll->shift.x;
		item->pos.y_pos += bounds[3];
		item->pos.z_pos += coll->shift.z;
		item->gravity_status = 1;
		item->speed = 2;
		item->fallspeed = 1;
		lara.gun_status = LG_NO_ARMS;
	}
	else
	{
		if (flag && wall > 0 && (move > 0 && coll->left_floor > coll->right_floor || move < 0 && coll->left_floor < coll->right_floor))
			flag = 0;

		bounds = GetBoundsAccurate(item);
		oldfloor = coll->front_floor;
		hdif = coll->front_floor - bounds[2];
		wall = 0;
		x = item->pos.x_pos;
		z = item->pos.z_pos;

		switch (dir)
		{
		case NORTH:
			x += move;
			break;

		case EAST:
			z -= move;
			break;

		case SOUTH:
			x -= move;
			break;

		case WEST:
			z += move;
			break;
		}

		lara.move_angle = angle;

		if (256 << dir & GetClimbTrigger(x, item->pos.y_pos, z, item->room_number))
		{
			if (!LaraTestHangOnClimbWall(item, coll))
				hdif = 0;
		}
		else if (abs(coll->left_floor2 - coll->right_floor2) >= 60 && (move < 0 && coll->left_floor2 != coll->front_floor || move > 0 && coll->right_floor2 != coll->front_floor))
			wall = 1;

		coll->front_floor = oldfloor;

		if (!wall && coll->mid_ceiling < 0 && coll->coll_type == CT_FRONT && !flag && !coll->hit_static && ceiling <= -950 && hdif >= -60 && hdif <= 60)
		{
			switch (dir)
			{
			case NORTH:
			case SOUTH:
				item->pos.z_pos += coll->shift.z;
				break;

			case EAST:
			case WEST:
				item->pos.x_pos += coll->shift.x;
				break;
			}

			item->pos.y_pos += hdif;
		}
		else
		{
			item->pos.x_pos = coll->old.x;
			item->pos.y_pos = coll->old.y;
			item->pos.z_pos = coll->old.z;

			if (item->current_anim_state == AS_HANGLEFT || item->current_anim_state == AS_HANGRIGHT)
			{
				item->current_anim_state = AS_HANG;
				item->goal_anim_state = AS_HANG;
				item->anim_number = ANIM_GRABLEDGE;
				item->frame_number = anims[item->anim_number].frame_base + 21;
			}

			return 1;
		}
	}

	return 0;
}

void SetCornerAnim(ITEM_INFO* item, COLL_INFO* coll, short rot, short flip)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (item->hit_points <= 0)
	{
		item->anim_number = ANIM_FALLDOWN;
		item->frame_number = anims[ANIM_FALLDOWN].frame_base;
		item->current_anim_state = AS_FORWARDJUMP;
		item->goal_anim_state = AS_FORWARDJUMP;
		item->gravity_status = 1;
		item->speed = 2;
		item->pos.y_pos += 256;
		item->fallspeed = 1;
		lara.gun_status = LG_NO_ARMS;
		item->pos.y_rot += rot / 2;
	}
	else if (flip)
	{
		if (lara.IsClimbing)
		{
			item->anim_number = ANIM_CLIMBSTNC;
			item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
			item->current_anim_state = AS_CLIMBSTNC;
			item->goal_anim_state = AS_CLIMBSTNC;
		}
		else
		{
			item->anim_number = ANIM_GRABLEDGE;
			item->frame_number = anims[ANIM_GRABLEDGE].frame_base + 21;
			item->current_anim_state = AS_HANG;
			item->goal_anim_state = AS_HANG;
		}

		coll->old.x = lara.CornerX;
		coll->old.z = lara.CornerZ;
		item->pos.x_pos = lara.CornerX;
		item->pos.z_pos = lara.CornerZ;
		item->pos.y_rot += rot;
	}
}

static long IsValidHangPos(ITEM_INFO* item, COLL_INFO* coll)
{
	short angle;

	if (LaraFloorFront(item, lara.move_angle, 100) >= 200)
	{
		angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

		switch (angle)
		{
		case NORTH:
			item->pos.z_pos += 4;
			break;

		case EAST:
			item->pos.x_pos += 4;
			break;

		case SOUTH:
			item->pos.z_pos -= 4;
			break;

		case WEST:
			item->pos.x_pos -= 4;
			break;
		}

		coll->bad_pos = -NO_HEIGHT;
		coll->bad_neg = -512;
		coll->bad_ceiling = 0;
		lara.move_angle = item->pos.y_rot;
		GetLaraCollisionInfo(item, coll);

		if (coll->mid_ceiling < 0 && coll->coll_type == CT_FRONT && !coll->hit_static && abs(coll->front_floor - coll->right_floor2) < 60)
			return 1;
	}

	return 0;
}

long LaraHangRightCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	long oldx, oldz, front, x, z, flag;
	short oldy, angle;

	if (item->anim_number != ANIM_GRABLEDGE || coll->hit_static)
		return 0;

	oldx = item->pos.x_pos;
	oldy = item->pos.y_rot;
	oldz = item->pos.z_pos;
	front = coll->front_floor;
	angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
	case SOUTH:
		x = oldx ^ ((ushort)oldx ^ (ushort)oldz) & 1023;
		z = oldz ^ ((ushort)oldx ^ (ushort)oldz) & 1023;
		break;

	default:
		x = (oldx & ~1023) - (oldz & 1023) + 1024;
		z = (oldz & ~1023) - (oldx & 1023) + 1024;
		break;
	}

	item->pos.x_pos = x;
	item->pos.z_pos = z;
	item->pos.y_rot += 0x4000;
	lara.CornerX = x;
	lara.CornerZ = z;
	flag = -IsValidHangPos(item, coll);

	if (flag)
	{
		if (lara.climb_status)
		{
			if (!(GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftClimbTab[angle]))
				flag = 0;
		}
		else if (abs(front - coll->front_floor) > 60)
			flag = 0;
	}

	if (!flag)
	{
		item->pos.x_pos = oldx;
		item->pos.y_rot = oldy;
		item->pos.z_pos = oldz;
		lara.move_angle = oldy;

		if (LaraFloorFront(item, item->pos.y_rot + 0x4000, 116) < 0)
			return 0;

		switch (angle)
		{
		case NORTH:
			x = ((item->pos.x_pos + 1024) & ~1023) - (item->pos.z_pos & 1023) + 1024;
			z = ((item->pos.z_pos + 1024) & ~1023) - (item->pos.x_pos & 1023) + 1024;
			break;

		case SOUTH:
			x = ((item->pos.x_pos - 1024) & ~1023) - (item->pos.z_pos & 1023) + 1024;
			z = ((item->pos.z_pos - 1024) & ~1023) - (item->pos.x_pos & 1023) + 1024;
			break;

		case WEST:
			x = (item->pos.x_pos ^ ((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023) - 1024;
			z = ((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023 ^ (item->pos.z_pos + 1024);
			break;

		default:
			x = (((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023) ^ (item->pos.x_pos + 1024);
			z = (item->pos.z_pos ^ (((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023)) - 1024;
			break;
		}

		item->pos.x_pos = x;
		item->pos.z_pos = z;
		item->pos.y_rot -= 0x4000;
		lara.CornerX = x;
		lara.CornerZ = z;
		flag = IsValidHangPos(item, coll);

		if (flag)
		{
			item->pos.x_pos = oldx;
			item->pos.y_rot = oldy;
			item->pos.z_pos = oldz;
			lara.move_angle = oldy;

			if (lara.climb_status)
			{
				if (!(GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & RightClimbTab[angle]))
				{
					front = LaraFloorFront(item, item->pos.y_rot, 116);

					if (abs(coll->front_floor - front) > 60 || front < -768)
						flag = 0;
				}
			}
			else
			{
				if (abs(front - coll->front_floor) <= 60)
				{
					switch (angle)
					{
					case NORTH:

						if ((oldx & 1023) < 512)
							flag = 0;

						break;

					case EAST:

						if ((oldz & 1023) > 512)
							flag = 0;

						break;

					case SOUTH:

						if ((oldx & 1023) > 512)
							flag = 0;

						break;

					case WEST:

						if ((oldz & 1023) < 512)
							flag = 0;

						break;
					}
				}
				else
					flag = 0;
			}

			return flag;
		}
	}

	item->pos.x_pos = oldx;
	item->pos.y_rot = oldy;
	item->pos.z_pos = oldz;
	lara.move_angle = oldy;
	return flag;
}

long LaraHangLeftCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	long oldx, oldz, front, x, z, flag;
	short oldy, angle;

	if (item->anim_number != ANIM_GRABLEDGE || coll->hit_static)
		return 0;

	oldx = item->pos.x_pos;
	oldy = item->pos.y_rot;
	oldz = item->pos.z_pos;
	front = coll->front_floor;
	angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
	case SOUTH:
		x = (oldx & ~1023) - (oldz & 1023) + 1024;
		z = (oldz & ~1023) - (oldx & 1023) + 1024;
		break;

	default:
		x = oldx ^ ((ushort)oldx ^ (ushort)oldz) & 1023;
		z = oldz ^ ((ushort)oldx ^ (ushort)oldz) & 1023;
		break;
	}

	item->pos.x_pos = x;
	item->pos.z_pos = z;
	item->pos.y_rot -= 0x4000;
	lara.CornerX = x;
	lara.CornerZ = z;
	flag = -IsValidHangPos(item, coll);

	if (flag)
	{
		if (lara.climb_status)
		{
			if (!(GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & RightClimbTab[angle]))
				flag = 0;
		}
		else if (abs(front - coll->front_floor) > 60)
			flag = 0;
	}

	if (!flag)
	{
		item->pos.x_pos = oldx;
		item->pos.y_rot = oldy;
		item->pos.z_pos = oldz;
		lara.move_angle = oldy;

		if (LaraFloorFront(item, item->pos.y_rot - 0x4000, 116) < 0)
			return 0;

		switch (angle)
		{
		case NORTH:
			x = (item->pos.x_pos ^ ((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023) - 1024;
			z = ((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023 ^ (item->pos.z_pos + 1024);
			break;

		case SOUTH:
			x = (((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023) ^ (item->pos.x_pos + 1024);
			z = (((ushort)item->pos.x_pos ^ (ushort)item->pos.z_pos) & 1023) ^ (item->pos.z_pos - 1024);
			break;

		case WEST:
			x = (item->pos.x_pos & ~1023) - (item->pos.z_pos & 1023);
			z = (item->pos.z_pos & ~1023) - (item->pos.x_pos & 1023);
			break;

		default:
			x = ((item->pos.x_pos + 1024) & ~1023) - (item->pos.z_pos & 1023) + 1024;
			z = ((item->pos.z_pos + 1024) & ~1023) - (item->pos.x_pos & 1023) + 1024;
			break;
		}

		item->pos.x_pos = x;
		item->pos.z_pos = z;
		item->pos.y_rot += 0x4000;
		lara.CornerX = x;
		lara.CornerZ = z;
		flag = IsValidHangPos(item, coll);

		if (flag)
		{
			item->pos.x_pos = oldx;
			item->pos.y_rot = oldy;
			item->pos.z_pos = oldz;
			lara.move_angle = oldy;

			if (lara.climb_status)
			{
				if (!(GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftClimbTab[angle]))
				{
					front = LaraFloorFront(item, item->pos.y_rot, 116);

					if (abs(coll->front_floor - front) > 60 || front < -768)
						flag = 0;
				}
			}
			else
			{
				if (abs(front - coll->front_floor) <= 60)
				{
					switch (angle)
					{
					case NORTH:

						if ((oldx & 1023) > 512)
							flag = 0;

						break;

					case EAST:

						if ((oldz & 1023) < 512)
							flag = 0;

						break;

					case SOUTH:

						if ((oldx & 1023) < 512)
							flag = 0;

						break;

					case WEST:

						if ((oldz & 1023) > 512)
							flag = 0;

						break;
					}
				}
				else
					flag = 0;
			}

			return flag;
		}
	}

	item->pos.x_pos = oldx;
	item->pos.y_rot = oldy;
	item->pos.z_pos = oldz;
	lara.move_angle = oldy;
	return flag;
}

short GetDirOctant(long rot)
{
	rot = abs(rot);
	return rot >= 0x2000 && rot <= 0x6000;
}

short TestMonkeyLeft(ITEM_INFO* item, COLL_INFO* coll)
{
	short oct;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot - 0x4000;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (abs(coll->mid_ceiling - coll->front_ceiling) > 50)
		return 0;

	if (coll->coll_type != CT_NONE)
	{
		oct = GetDirOctant(item->pos.y_rot);

		if (!oct && (coll->coll_type == CT_FRONT || coll->coll_type == CT_LEFT))
			return 0;

		if (oct == 1 && (coll->coll_type == CT_RIGHT || coll->coll_type == CT_LEFT))
			return 0;
	}

	return 1;
}

short TestMonkeyRight(ITEM_INFO* item, COLL_INFO* coll)
{
	short oct;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot + 0x4000;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (abs(coll->mid_ceiling - coll->front_ceiling) > 50)
		return 0;

	if (coll->coll_type != CT_NONE)
	{
		oct = GetDirOctant(item->pos.y_rot);

		if (!oct && coll->coll_type == CT_FRONT)
			return 0;

		if (oct == 1 && (coll->coll_type == CT_FRONT || coll->coll_type == CT_RIGHT || coll->coll_type == CT_LEFT))
			return 0;
	}

	return 1;
}

void SnapLaraToEdgeOfBlock(ITEM_INFO* item, COLL_INFO* coll, short angle)
{
	if (item->current_anim_state == AS_HANGRIGHT)
	{
		switch (angle)
		{
		case NORTH:
			item->pos.x_pos = coll->old.x & ~0x3FF | 0x390;
			break;

		case EAST:
			item->pos.z_pos = coll->old.z & ~0x3FF | 0x70;
			break;

		case SOUTH:
			item->pos.x_pos = coll->old.x & ~0x3FF | 0x70;
			break;

		default:
			item->pos.z_pos = coll->old.z & ~0x3FF | 0x390;
			break;
		}
	}
	else if (item->current_anim_state == AS_HANGLEFT)
	{
		switch (angle)
		{
		case NORTH:
			item->pos.x_pos = coll->old.x & ~0x3FF | 0x70;
			break;

		case EAST:
			item->pos.z_pos = coll->old.z & ~0x3FF | 0x390;
			break;

		case SOUTH:
			item->pos.x_pos = coll->old.x & ~0x3FF | 0x390;
			break;

		default:
			item->pos.z_pos = coll->old.z & ~0x3FF | 0x70;
			break;
		}
	}
}

long LaraTestClimbStance(ITEM_INFO* item, COLL_INFO* coll)
{
	long shift_r, shift_l;

	if (LaraTestClimbPos(item, coll->radius, coll->radius + 120, -700, 512, &shift_r) != 1)
		return 0;

	if (LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -700, 512, &shift_l) != 1)
		return 0;

	if (shift_r)
	{
		if (shift_l)
		{
			if (shift_r < 0 != shift_l < 0)
				return 0;

			if (shift_r < 0 && shift_l < shift_r)
				shift_r = shift_l;
			else if (shift_r > 0 && shift_l > shift_r)
				shift_r = shift_l;
		}

		item->pos.y_pos += shift_r;
	}
	else if (shift_l)
		item->pos.y_pos += shift_l;

	return 1;
}

long TestHangSwingIn(ITEM_INFO* item, short angle)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	room_number = item->room_number;

	switch (angle)
	{
	case 0:
		z += 256;
		break;

	case 0x4000:
		x += 256;
		break;

	case -0x4000:
		x -= 256;
		break;

	case -0x8000:
		z -= 256;
		break;
	}

	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);
	return h != NO_HEIGHT && h - y > 0 && c - y < -400 && y - c - 819 > -72;
}

long LaraTestHangOnClimbWall(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long shift, result;
	short angle, l, r;

	if (!lara.climb_status || item->fallspeed < 0)
		return 0;

	angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
	case SOUTH:
		item->pos.z_pos += coll->shift.z;
		break;

	case EAST:
	case WEST:
		item->pos.x_pos += coll->shift.x;
		break;
	}

	bounds = GetBoundsAccurate(item);

	if (lara.move_angle != item->pos.y_rot)
	{
		r = LaraCeilingFront(item, item->pos.y_rot, 0, 0);
		l = LaraCeilingFront(item, lara.move_angle, 128, 0);

		if (abs(r - l) > 60)
			return 0;
	}

	if (!LaraTestClimbPos(item, coll->radius, coll->radius, bounds[2], bounds[3] - bounds[2], &shift) ||
		!LaraTestClimbPos(item, coll->radius, -coll->radius, bounds[2], bounds[3] - bounds[2], &shift))
		return 0;

	result = LaraTestClimbPos(item, coll->radius, 0, bounds[2], bounds[3] - bounds[2], &shift);

	if (!result)
		return 0;

	if (result != 1)
		item->pos.y_pos += shift;

	return 1;
}

long LaraTestEdgeCatch(ITEM_INFO* item, COLL_INFO* coll, long* edge)
{
	short* bounds;
	long hdif;

	bounds = GetBoundsAccurate(item);
	hdif = coll->front_floor - bounds[2];

	if ((hdif >= 0 || item->fallspeed + hdif >= 0) && (hdif <= 0 || item->fallspeed + hdif <= 0))
	{
		if (abs(coll->left_floor2 - coll->right_floor2) >= 60)
			return 0;

		return 1;
	}

	hdif = item->pos.y_pos + bounds[2];

	if (hdif >> 8 == (hdif + item->fallspeed) >> 8)
		return 0;

	if (item->fallspeed > 0)
		*edge = (hdif + item->fallspeed) & ~255;
	else
		*edge = hdif & ~255;

	return -1;
}

void lara_as_duckroll(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = -3640;
	item->goal_anim_state = AS_DUCK;
}

void lara_col_duckroll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->facing = item->pos.y_rot;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_NO_ARMS;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	lara.keep_ducked = coll->mid_ceiling >= -362;

	if (coll->mid_floor < coll->bad_neg || coll->front_floor > coll->bad_pos)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		return;
	}

	ShiftItem(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}
