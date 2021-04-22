#include "../tomb5/pch.h"
#include "larasurf.h"
#include "../global/types.h"
#include "lara_states.h"
#include "collide.h"
#include "lara.h"
#include "control.h"
#include "laramisc.h"
#include "larafire.h"
#include "laraswim.h"

int LaraTestWaterStepOut(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->mid_type == BIG_SLOPE || coll->mid_type == DIAGONAL || coll->mid_floor >= 0)
		return 0;

	if (coll->mid_floor >= -128)
	{
		if (item->goal_anim_state == STATE_LARA_ONWATER_LEFT)
			item->goal_anim_state = STATE_LARA_WALK_LEFT;
		else if (item->goal_anim_state == STATE_LARA_ONWATER_RIGHT)
			item->goal_anim_state = STATE_LARA_WALK_RIGHT;
		else
		{
			item->anim_number = ANIMATION_LARA_WADE;
			item->frame_number = anims[ANIMATION_LARA_WADE].frame_base;
			item->goal_anim_state = STATE_LARA_WADE_FORWARD;
			item->current_anim_state = STATE_LARA_WADE_FORWARD;
		}
	}
	else
	{
		item->anim_number = ANIMATION_LARA_ONWATER_TO_WADE_DEEP;
		item->frame_number = anims[ANIMATION_LARA_ONWATER_TO_WADE_DEEP].frame_base;
		item->current_anim_state = STATE_LARA_ONWATER_EXIT;
		item->goal_anim_state = STATE_LARA_STOP;
	}

	item->pos.y_pos += coll->front_floor + 695;
	UpdateLaraRoom(item, -381);
	item->pos.z_rot = 0;
	item->pos.x_rot = 0;
	item->gravity_status = true;
	item->speed = 0;
	item->fallspeed = 0;
	lara.water_status = LW_WADE;
	return 1;
}

int LaraTestWaterClimbOut(ITEM_INFO* item, COLL_INFO* coll)
{
	int hdif;
	short angle;

	if (coll->coll_type != CT_FRONT || !(input & IN_ACTION))
		return 0;

	if (ABS(coll->left_floor2 - coll->right_floor2) >= 60 || 
		lara.gun_status != LG_NO_ARMS && 
		(lara.gun_status != LG_READY || lara.gun_type != WEAPON_FLARE))
		return 0;

	if (coll->front_ceiling > 0)
		return 0;

	if (coll->mid_ceiling > 0)
		return 0;

	hdif = coll->front_floor + 700;

	if (hdif <= -512 || hdif > 316)
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -6370 && angle <= 6370)
		angle = 0;
	else if (angle >= 10014 && angle <= 22754)
		angle = 16384;
	else if (angle >= 26397 || angle <= -26397)
		angle = -32768;
	else if (angle >= -22754 && angle <= -10014)
		angle = -16384;

	if (angle & 0x3FFF)
		return 0;

	item->pos.y_pos += coll->front_floor + 695;
	UpdateLaraRoom(item, -381);

	if (angle == 0)
		item->pos.z_pos = (item->pos.z_pos & -1024) + 1124;
	else if (angle == 16384)
		item->pos.x_pos = (item->pos.x_pos & -1024) + 1124;
	else if (angle == -32768)
		item->pos.z_pos = (item->pos.z_pos & -1024) - 100;
	else if (angle == -16384)
		item->pos.x_pos = (item->pos.x_pos & -1024) - 100;

	if (hdif < -128)
	{
		item->anim_number = ANIMATION_LARA_CLIMB_OUT_OF_WATER;
		item->frame_number = anims[ANIMATION_LARA_CLIMB_OUT_OF_WATER].frame_base;
	}
	else if (hdif < 128)
	{
		item->anim_number = ANIMATION_LARA_ONWATER_TO_LAND_LOW;
		item->frame_number = anims[ANIMATION_LARA_ONWATER_TO_LAND_LOW].frame_base;
	}
	else
	{
		item->anim_number = ANIMATION_LARA_ONWATER_TO_WADE;
		item->frame_number = anims[ANIMATION_LARA_ONWATER_TO_WADE].frame_base;
	}

	item->current_anim_state = STATE_LARA_ONWATER_EXIT;
	item->goal_anim_state = STATE_LARA_STOP;
	item->pos.y_rot = angle;
	lara.gun_status = LG_HANDS_BUSY;
	item->pos.z_rot = 0;
	item->pos.x_rot = 0;
	item->gravity_status = 0;
	item->speed = 0;
	item->fallspeed = 0;
	lara.water_status = LW_ABOVE_WATER;
	return 1;
}

void LaraSurfaceCollision(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos + 700, item->pos.z_pos, item->room_number, 800);
	ShiftItem(item, coll);

	if (coll->coll_type & (CT_CLAMP | CT_TOP_FRONT | CT_TOP | CT_FRONT) ||
		coll->mid_floor < 0 &&
		(coll->mid_type == BIG_SLOPE || coll->mid_type == DIAGONAL))
	{
		item->fallspeed = 0;
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
	}
	else if (coll->coll_type == CT_LEFT)
		item->pos.y_rot += 910;
	else if (coll->coll_type == CT_RIGHT)
		item->pos.y_rot -= 910;

	if (GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number) - item->pos.y_pos > -100)
		LaraTestWaterStepOut(item, coll);
	else
	{
		item->goal_anim_state = STATE_LARA_UNDERWATER_FORWARD;
		item->current_anim_state = STATE_LARA_UNDERWATER_DIVING;
		item->anim_number = ANIMATION_LARA_FREE_FALL_TO_UNDERWATER_ALTERNATE;
		item->frame_number = anims[ANIMATION_LARA_FREE_FALL_TO_UNDERWATER_ALTERNATE].frame_base;
		item->pos.x_rot = -8190;
		item->fallspeed = 80;
		lara.water_status = LW_UNDERWATER;
	}
}

void LaraSurface(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = -4004;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -128;
	coll->bad_ceiling = 100;
	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	coll->radius = 100;
	coll->trigger = 0;

	if (input & IN_LOOK && lara.look)
		LookUpDown();
	else
		ResetLook();

	lara.look = 1;
	lara_control_routines[item->current_anim_state](item, coll);
	
	if (item->pos.z_rot >= -364 || item->pos.z_rot <= 364)
		item->pos.z_rot = 0;
	else if (item->pos.z_rot >= 0)
		item->pos.z_rot -= 364;
	else
		item->pos.z_rot += 364;
	
	if (lara.current_active && lara.water_status != LW_FLYCHEAT)
		LaraWaterCurrent(coll);

	AnimateLara(item);
	item->pos.x_pos += item->fallspeed * SIN(lara.move_angle) >> 16;
	item->pos.z_pos += item->fallspeed * COS(lara.move_angle) >> 16;
	LaraBaddieCollision(item, coll);
	lara_collision_routines[item->current_anim_state](item, coll);
	UpdateLaraRoom(item, 100);
	LaraGun();
	TestTriggers(coll->trigger, 0, 0);
}

void lara_as_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;

	if (!(input & IN_FORWARD))
		item->goal_anim_state = STATE_LARA_ONWATER_STOP;

	if (input & IN_JUMP)
		item->goal_anim_state = STATE_LARA_ONWATER_STOP;

	item->fallspeed += 8;
	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_BACK))
		item->goal_anim_state = STATE_LARA_ONWATER_STOP;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_LSTEP))
		item->goal_anim_state = STATE_LARA_ONWATER_STOP;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_RSTEP))
		item->goal_anim_state = STATE_LARA_ONWATER_STOP;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed -= 4;

	if (item->fallspeed < 0)
		item->fallspeed = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	if (input & IN_LOOK)
	{
		LookUpDown();
		return;
	}

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;
	

	if (input & IN_FORWARD)
		item->goal_anim_state = STATE_LARA_ONWATER_FORWARD;
	else if (input & IN_BACK)
		item->goal_anim_state = STATE_LARA_ONWATER_BACK;

	if (input & IN_LSTEP)
		item->goal_anim_state = STATE_LARA_ONWATER_LEFT;
	else if (input & IN_RSTEP)
		item->goal_anim_state = STATE_LARA_ONWATER_RIGHT;

	if (input & IN_JUMP)
	{
		lara.dive_count++;
		if (lara.dive_count == 10)
			item->goal_anim_state = STATE_LARA_UNDERWATER_FORWARD;
	}
	else
		lara.dive_count = 0;
}

void lara_col_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_neg = -384;
	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
	LaraTestWaterClimbOut(item, coll);
}

void lara_col_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 32768;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 16384;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 16384;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state == STATE_LARA_UNDERWATER_FORWARD)
	{
		item->current_anim_state = STATE_LARA_UNDERWATER_DIVING;
		item->anim_number = ANIMATION_LARA_FREE_FALL_TO_UNDERWATER_ALTERNATE;
		item->pos.x_rot = -8190;
		item->frame_number = anims[ANIMATION_LARA_FREE_FALL_TO_UNDERWATER_ALTERNATE].frame_base;
		item->fallspeed = 80;
		lara.water_status = LW_UNDERWATER;
	}

	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
}


void inject_larasurf()
{
	INJECT(0x00458240, LaraTestWaterStepOut);
	INJECT(0x00457E50, LaraTestWaterClimbOut);
	INJECT(0x00458100, LaraSurfaceCollision);
	INJECT(0x00457940, LaraSurface);
	INJECT(0x00457B10, lara_as_surfswim);
	INJECT(0x00457BA0, lara_as_surfback);
	INJECT(0x00457C20, lara_as_surfleft);
	INJECT(0x00457CA0, lara_as_surfright);
	INJECT(0x00457D20, lara_as_surftread);
	INJECT(0x00457E10, lara_col_surfswim);
	INJECT(0x00458380, lara_col_surfback);
	INJECT(0x004583C0, lara_col_surfleft);
	INJECT(0x00458400, lara_col_surfright);
	INJECT(0x00458440, lara_col_surftread);
}
