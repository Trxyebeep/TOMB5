#pragma once
#include "../tomb5/pch.h"
#include "../global/types.h"
#include "lara.h"
#include "collide.h"
#include "laramisc.h"
#include "lara_states.h"
#include "../specific/maths.h"
#include "effects.h"
#include "sound.h"
#include "control.h"
#include "effect2.h"
#include "../specific/calclara.h"
#include "camera.h"
#include "items.h"
#include "sound.h"
#include "objects.h"

char breath_pitch_shifter[12] =
{
	4, 5, 6, 8, 0xB, 0xE, 0x12, 0x17, 0, 0, 0, 0 
};


void LaraDeflectEdgeJump(ITEM_INFO* item, COLL_INFO* coll)
{
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_FRONT:
	case CT_TOP_FRONT:

		if (lara.climb_status && item->speed == 2)
			return;

		if (coll->mid_floor <= 0x200)
		{
			if (coll->mid_floor <= 0x80)
			{
				item->goal_anim_state = STATE_LARA_GRAB_TO_FALL;
				item->current_anim_state = STATE_LARA_GRAB_TO_FALL;
				item->anim_number = ANIMATION_LARA_LANDING_LIGHT;
				item->frame_number = anims[ANIMATION_LARA_LANDING_LIGHT].frame_base;
			}
		}
		else
		{
			item->goal_anim_state = STATE_LARA_FREEFALL;
			item->current_anim_state = STATE_LARA_FREEFALL;
			item->anim_number = ANIMATION_LARA_SMASH_JUMP;
			item->frame_number = anims[ANIMATION_LARA_SMASH_JUMP].frame_base + 1;
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
		item->pos.y_rot += 0x38E;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 0x38E;
		break;

	case CT_CLAMP:
		item->pos.z_pos -= (100 * COS(coll->facing)) >> 14;
		item->pos.x_pos -= (100 * SIN(coll->facing)) >> 14;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed < 1)
			item->fallspeed = 16;

		break;
	}
}

int LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll)
{
	int land_speed;
	
	land_speed = item->fallspeed - 0x8C;

	if (land_speed < 0)
		return 0;

	if (0xE < land_speed)
	{
		item->hit_points = 0;
		return (item->hit_points < 1);
	}

	item->hit_points += ((land_speed * land_speed * -1000) / 0xC4);
	return (item->hit_points < 1);
}

int TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll)
{
	static short old_ang = 1;
	short ang_diff, ang;

	if ((ABS(coll->tilt_x)) <= 2 && (ABS(coll->tilt_z)) <= 2)
		return 0;

	ang = 0;

	if (coll->tilt_x > 2)
		ang = -16384;
	else if (coll->tilt_x < -2)
		ang = 16384;

	if (coll->tilt_z > 2 && coll->tilt_z > ABS(coll->tilt_x))
		ang = -32768;
	else if ((coll->tilt_z < -2) && (-coll->tilt_z > ABS(coll->tilt_x)))
		ang = 0;

	ang_diff = ang - item->pos.y_rot;
	ShiftItem(item, coll);

	if (ang_diff >= -16384 && ang_diff <= 16384)
	{
		if (item->current_anim_state == STATE_LARA_SLIDE_FORWARD && old_ang == ang)
			return 1;

		item->anim_number = ANIMATION_LARA_SLIDE_FORWARD;
		item->frame_number = anims[ANIMATION_LARA_SLIDE_FORWARD].frame_base;
		item->goal_anim_state = STATE_LARA_SLIDE_FORWARD;
		item->current_anim_state = STATE_LARA_SLIDE_FORWARD;
		item->pos.y_rot = ang;
		lara.move_angle = ang;
		old_ang = ang;
	}
	else
	{
		if (item->current_anim_state == STATE_LARA_SLIDE_BACK && old_ang == ang)
			return 1;

		item->anim_number = ANIMATION_LARA_START_SLIDE_BACKWARD;
		item->frame_number = anims[ANIMATION_LARA_START_SLIDE_BACKWARD].frame_base;
		item->goal_anim_state = STATE_LARA_SLIDE_BACK;
		item->current_anim_state = STATE_LARA_SLIDE_BACK;
		item->pos.y_rot = ang - 32768;
		lara.move_angle = ang;
		old_ang = ang;
	}

	return 1;
}

int LaraDeflectEdge(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->goal_anim_state = STATE_LARA_STOP;
		item->speed = 0;
		item->gravity_status = 0;
		return 1;
	}
	else if (coll->coll_type == CT_LEFT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot += (910);
		return 0;
	}
	else
	{
		if (coll->coll_type == CT_RIGHT)
		{
			ShiftItem(item, coll);
			item->pos.y_rot -= (910);
		}

		return 0;
	}
}

void LaraCollideStop(ITEM_INFO* item, COLL_INFO* coll)
{
	switch (coll->old_anim_state)
	{
	case STATE_LARA_STOP:
	case STATE_LARA_TURN_RIGHT_SLOW:
	case STATE_LARA_TURN_LEFT_SLOW:
	case STATE_LARA_TURN_FAST:

		item->current_anim_state = coll->old_anim_state;
		item->anim_number = coll->old_anim_number;
		item->frame_number = coll->old_frame_number;

		if (input & IN_LEFT)
			item->goal_anim_state = STATE_LARA_TURN_LEFT_SLOW;
		else if (input & IN_RIGHT)
			item->goal_anim_state = STATE_LARA_TURN_RIGHT_SLOW;
		else
			item->goal_anim_state = STATE_LARA_STOP;

		AnimateLara(item);
		break;

	default:
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		break;
	}
}

int LaraHitCeiling(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_TOP || coll->coll_type == CT_CLAMP)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->goal_anim_state = STATE_LARA_STOP;
		item->current_anim_state = STATE_LARA_STOP;
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		item->speed = 0;
		item->fallspeed = 0;
		item->gravity_status = 0;
		return 1;
	}
	else
		return 0;
}

void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 762);
}

void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
    item->goal_anim_state = STATE_LARA_STOP;

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
	lara.move_angle = item->pos.y_rot - 32768;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 1;
	coll->bad_pos = 32512;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (coll->mid_floor > 200)
	{
		item->current_anim_state = STATE_LARA_FALL_BACKWARD;
		item->goal_anim_state = STATE_LARA_FALL_BACKWARD;
		item->anim_number = ANIMATION_LARA_FREE_FALL_BACK;
		item->frame_number = anims[ANIMATION_LARA_FREE_FALL_BACK].frame_base;
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
	if (item->fallspeed > 0x83)
		item->goal_anim_state = STATE_LARA_FREEFALL;

	if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS)
		item->goal_anim_state = STATE_LARA_REACH;

	return;
}

void lara_col_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x8000;
	coll->bad_pos = 32512;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);
	
	if ((coll->mid_floor < 1) && (0 < item->fallspeed))
	{
		if (!LaraLandedBad(item, coll))
			item->goal_anim_state = STATE_LARA_STOP;
		else
			item->goal_anim_state = STATE_LARA_DEATH;
											
		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}

}

void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = 32512;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = STATE_LARA_DEATH;
		else
			item->goal_anim_state = STATE_LARA_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	if (item->fallspeed <= 131)
	{
		if (input & IN_RIGHT)
			if (item->goal_anim_state != STATE_LARA_STOP)
				item->goal_anim_state = STATE_LARA_JUMP_ROLL;
	}
	else
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	if (item->fallspeed <= 131)
	{
		if (input & IN_LEFT)
			if (item->goal_anim_state != STATE_LARA_STOP)
				item->goal_anim_state = STATE_LARA_JUMP_ROLL;
	}
	else
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 24570;
	if (item->fallspeed <= 131)
	{
		if (item->goal_anim_state == STATE_LARA_RUN_FORWARD)
			item->goal_anim_state = STATE_LARA_STOP;
		else if (input & (IN_FORWARD | IN_ROLL) && item->goal_anim_state != STATE_LARA_STOP)
			item->goal_anim_state = STATE_LARA_JUMP_ROLL;
	}
	else
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_col_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot - 32768;
	else
		lara.move_angle = item->pos.y_rot;

	coll->bad_pos = 32512;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot;

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = STATE_LARA_DEATH;
		else
		{
			if (lara.water_status == LW_WADE)
				item->goal_anim_state = STATE_LARA_STOP;
			else
			{
				if (input & IN_FORWARD && !(input & IN_WALK))
					item->goal_anim_state = STATE_LARA_RUN_FORWARD;
				else
					item->goal_anim_state = STATE_LARA_STOP;
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
	if (item->goal_anim_state == STATE_LARA_SWANDIVE_BEGIN || item->goal_anim_state == STATE_LARA_REACH)
		item->goal_anim_state = STATE_LARA_JUMP_FORWARD;

	if (item->goal_anim_state != STATE_LARA_DEATH && item->goal_anim_state != STATE_LARA_STOP && item->goal_anim_state != STATE_LARA_RUN_FORWARD)
	{
		if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS)
			item->goal_anim_state = STATE_LARA_REACH;

		if ((input & IN_ROLL) || (input & IN_BACK))
			item->goal_anim_state = STATE_LARA_JUMP_ROLL;

		if ((input & IN_WALK) && lara.gun_status == LG_NO_ARMS)
			item->goal_anim_state = STATE_LARA_SWANDIVE_BEGIN;

		if (item->fallspeed > 131)
			item->goal_anim_state = STATE_LARA_FREEFALL;
	}

	if ((input & IN_LEFT))
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -546)
			lara.turn_rate = -546;
	}
	else if ((input & IN_RIGHT))
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
				lara.head_x_rot += 364 * (BinocularRange - 1792) / 3072;
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
				lara.head_x_rot += 364 * (1792 - BinocularRange) / 3072;
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
				lara.head_y_rot += 364 * (BinocularRange - 1792) / 1536;
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
				lara.head_y_rot += 364 * (1792 - BinocularRange) / 1536;
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
			lara.head_x_rot = lara.head_x_rot / -8 + lara.head_x_rot;
		else
			lara.head_x_rot = 0;

		if (lara.head_y_rot <= -364 || lara.head_y_rot >= 364)
			lara.head_y_rot = lara.head_y_rot / -8 + lara.head_y_rot;
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
	return;
}

void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
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

	if (input & IN_FORWARD)
	{
		if (lara.water_status == LW_WADE)
			item->goal_anim_state = STATE_LARA_WADE_FORWARD;
		else if (input & IN_WALK)
			item->goal_anim_state = STATE_LARA_WALK_FORWARD;
		else
			item->goal_anim_state = STATE_LARA_RUN_FORWARD;
	}
	else
		item->goal_anim_state = STATE_LARA_STOP;

	return;

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

	if (!LaraHitCeiling(item, coll) && !TestLaraVault(item, coll))
	{
		if (LaraDeflectEdge(item, coll))
		{
			if (item->frame_number >= 29 && item->frame_number <= 47)
			{
				item->anim_number = ANIMATION_LARA_END_WALK_LEFT;
				item->frame_number = anims[ANIMATION_LARA_END_WALK_LEFT].frame_base;
			}
			else
			{
				if (item->frame_number >= 22 && item->frame_number <= 28 ||
					item->frame_number >= 48 && item->frame_number <= 57)
				{
					item->anim_number = ANIMATION_LARA_END_WALK_RIGHT;
					item->frame_number = anims[ANIMATION_LARA_END_WALK_RIGHT].frame_base;
				}
				else
					LaraCollideStop(item, coll);
			}
		}

		if (!LaraFallen(item, coll))
		{
			if (coll->mid_floor > 128)
			{
				if (coll->front_floor == NO_HEIGHT || coll->front_floor <= 128)
					coll->mid_floor = 0;
				else
				{
					if (item->frame_number >= 28 && item->frame_number <= 45)
					{
						item->anim_number = ANIMATION_LARA_WALK_DOWN_LEFT;
						item->frame_number = anims[ANIMATION_LARA_WALK_DOWN_LEFT].frame_base;
					}
					else
					{
						item->anim_number = ANIMATION_LARA_WALK_DOWN_RIGHT;
						item->frame_number = anims[ANIMATION_LARA_WALK_DOWN_RIGHT].frame_base;
					}
				}
			}

			if (coll->mid_floor >= -384 && coll->mid_floor < -128)
			{
				if (coll->front_floor == NO_HEIGHT ||
					coll->front_floor < -384 ||
					coll->front_floor >= -128)
					coll->mid_floor = 0;
				else
				{
					if (item->frame_number >= 27 && item->frame_number <= 44)
					{
						item->anim_number = ANIMATION_LARA_WALK_UP_STEP_LEFT;
						item->frame_number = anims[ANIMATION_LARA_WALK_UP_STEP_LEFT].frame_base;
					}
					else
					{
						item->anim_number = ANIMATION_LARA_WALK_UP_STEP_RIGHT;
						item->frame_number = anims[ANIMATION_LARA_WALK_UP_STEP_RIGHT].frame_base;
					}
				}
			}

			if (!TestLaraSlide(item, coll) && coll->mid_floor != NO_HEIGHT)
				item->pos.y_pos += coll->mid_floor;
		}
	}
}

int LaraFallen(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.water_status == LW_WADE || coll->mid_floor <= 384)
		return 0;
	else
	{
		item->anim_number = ANIMATION_LARA_FREE_FALL_FORWARD;
		item->current_anim_state = STATE_LARA_JUMP_FORWARD;
		item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
		item->frame_number = anims[ANIMATION_LARA_FREE_FALL_FORWARD].frame_base;
		item->fallspeed = 0;
		item->gravity_status = 1;
		return 1;
	}
}

int TestLaraVault(ITEM_INFO* item, COLL_INFO* coll)//wat
{
	int hdif, slope;
	short angle;

	if (!(input & IN_ACTION))
		return 0;

	if (lara.gun_status != LG_NO_ARMS)
		return 0;

	if (coll->coll_type != CT_FRONT)
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -5460 && angle <= 5460)
		angle = 0;
	else if (angle >= 10924 && angle <= 21844)
		angle = 16384;
	else if (angle >= 27307 || angle <= -27307)
		angle = -32768;
	else if (angle >= -21844 && angle <= -10924)
		angle = -16384;

	if (angle & 0x3FFF)
		return 0;

	hdif = coll->front_floor;
	slope = (ABS(coll->left_floor2 - coll->right_floor2) >= 60);

	if (hdif >= -640 && hdif <= -384)
	{
		if (!slope &&
			hdif - coll->front_ceiling >= 0 &&
			coll->left_floor2 - coll->left_ceiling2 >= 0 &&
			coll->right_floor2 - coll->right_ceiling2 >= 0)
		{
			item->anim_number = ANIMATION_LARA_CLIMB_2CLICK;
			item->frame_number = anims[ANIMATION_LARA_CLIMB_2CLICK].frame_base;
			item->current_anim_state = STATE_LARA_GRABBING;
			item->goal_anim_state = STATE_LARA_STOP;
			item->pos.y_pos += hdif + 512;
			lara.gun_status = LG_HANDS_BUSY;
		}
		else
			return 0;
	}
	else if (hdif >= -896 && hdif <= -640)
	{
		if (!slope &&
			hdif - coll->front_ceiling >= 0 &&
			coll->left_floor2 - coll->left_ceiling2 >= 0 &&
			coll->right_floor2 - coll->right_ceiling2 >= 0)
		{
			item->anim_number = ANIMATION_LARA_CLIMB_3CLICK;
			item->frame_number = anims[ANIMATION_LARA_CLIMB_3CLICK].frame_base;
			item->current_anim_state = STATE_LARA_GRABBING;
			item->goal_anim_state = STATE_LARA_STOP;
			item->pos.y_pos += hdif + 768;
			lara.gun_status = LG_HANDS_BUSY;
		}
		else
			return 0;
	}
	else if (slope || hdif < -1920 || hdif > -896)
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
					item->anim_number = ANIMATION_LARA_STAY_SOLID;
					item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
					item->goal_anim_state = STATE_LARA_LADDER_IDLE;
					item->current_anim_state = STATE_LARA_STOP;
					AnimateLara(item);
					item->pos.y_rot = angle;
					lara.gun_status = LG_HANDS_BUSY;
					return 1;
				}
			}

			return 0;
		}

		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		item->goal_anim_state = STATE_LARA_JUMP_UP;
		item->current_anim_state = STATE_LARA_STOP;
		lara.calc_fallspeed = -116;
		AnimateLara(item);
	}
	else
	{
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		item->goal_anim_state = STATE_LARA_JUMP_UP;
		item->current_anim_state = STATE_LARA_STOP;
		lara.calc_fallspeed = -3 - phd_sqrt(hdif * -12 + -9600);
		AnimateLara(item);
	}

	item->pos.y_rot = angle;
	ShiftItem(item, coll);
	angle = (unsigned short)((item->pos.y_rot + 0x2000) >> 14);

	switch (angle)
	{
	case NORTH:
		item->pos.z_pos = (item->pos.z_pos | 0x3FF) - 100;
		break;

	case EAST:
		item->pos.x_pos = (item->pos.x_pos | 0x3FF) - 100;
		break;

	case SOUTH:
		item->pos.z_pos = (item->pos.z_pos & 0xFFFFFC00) + 100;
		break;

	case WEST:
		item->pos.x_pos = (item->pos.x_pos & 0xFFFFFC00) + 100;
		break;

	default:
		break;
	}

	return 1;
}

void lara_as_null(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	return;
}

void lara_as_run(ITEM_INFO* item, COLL_INFO* coll)
{
	static int jump_ok = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_DEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIMATION_LARA_ROLL_BEGIN;
		item->frame_number = anims[ANIMATION_LARA_ROLL_BEGIN].frame_base + 2;
		item->current_anim_state = STATE_LARA_ROLL_FORWARD;
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (input & IN_SPRINT && DashTimer)
	{
		item->goal_anim_state = STATE_LARA_SPRINT;
		return;
	}

	if (input & IN_DUCK &&
		lara.water_status != 4 &&
		(lara.gun_status == LG_NO_ARMS ||
			lara.gun_type == WEAPON_NONE ||
			lara.gun_type == WEAPON_PISTOLS ||
			lara.gun_type == WEAPON_REVOLVER ||
			lara.gun_type == WEAPON_UZI ||
			lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = STATE_LARA_CROUCH_IDLE;
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

	if (item->anim_number == ANIMATION_LARA_STAY_TO_RUN)
		jump_ok = 0;
	else if (item->anim_number != ANIMATION_LARA_RUN || item->frame_number == 4)
		jump_ok = 1;

	if (input & IN_JUMP && jump_ok && !item->gravity_status)
		item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
	else if (input & IN_FORWARD)
	{
		if (lara.water_status == LW_WADE)
			item->goal_anim_state = STATE_LARA_WADE_FORWARD;
		else if (input & IN_WALK)
			item->goal_anim_state = STATE_LARA_WALK_FORWARD;
		else
			item->goal_anim_state = STATE_LARA_RUN_FORWARD;
	}
	else
		item->goal_anim_state = STATE_LARA_STOP;
}

void lara_col_run(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (!LaraHitCeiling(item, coll) && !TestLaraVault(item, coll))
	{
		if (LaraDeflectEdge(item, coll))
		{
			item->pos.z_rot = 0;
			if (item->anim_number != ANIMATION_LARA_STAY_TO_RUN && TestWall(item, 256, 0, -640))
			{
				item->current_anim_state = STATE_LARA_SPLAT;

				if (item->frame_number >= 0 && item->frame_number <= 9)
				{
					item->anim_number = ANIMATION_LARA_WALL_SMASH_LEFT;
					item->frame_number = anims[ANIMATION_LARA_WALL_SMASH_LEFT].frame_base;
					return;
				}

				if (item->frame_number >= 10 && item->frame_number <= 21)
				{
					item->anim_number = ANIMATION_LARA_WALL_SMASH_RIGHT;
					item->frame_number = anims[ANIMATION_LARA_WALL_SMASH_RIGHT].frame_base;
					return;
				}
			}

			LaraCollideStop(item, coll);
		}

		if (!LaraFallen(item, coll))
		{
			if (coll->mid_floor >= -384 && coll->mid_floor < -128)
			{
				if (coll->front_floor == NO_HEIGHT || coll->front_floor < -384 || coll->front_floor >= -128)
					coll->mid_floor = 0;
				else
				{
					if (item->frame_number >= 3 && item->frame_number <= 14)
					{
						item->anim_number = ANIMATION_LARA_RUN_UP_STEP_LEFT;
						item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_LEFT].frame_base;
					}
					else
					{
						item->anim_number = ANIMATION_LARA_RUN_UP_STEP_RIGHT;
						item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_RIGHT].frame_base;
					}
				}
			}

			if (!TestLaraSlide(item, coll))
			{
				if (coll->mid_floor < 50)
				{
					if (coll->mid_floor != NO_HEIGHT)
						item->pos.y_pos += coll->mid_floor;
				}
				else
					item->pos.y_pos += 50;
			}
		}
	}
}

void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	short height, ceiling;
	short fheight = NO_HEIGHT;
	short rheight = NO_HEIGHT;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_DEATH;
		return;
	}

	if (item->anim_number != ANIMATION_LARA_SPRINT_SLIDE_STAND_RIGHT &&
		item->anim_number != ANIMATION_LARA_SPRINT_SLIDE_STAND_LEFT)
		StopSoundEffect(SFX_LARA_SLIPPING);

	if (input & IN_ROLL && lara.water_status != LW_WADE)
	{
		item->anim_number = ANIMATION_LARA_ROLL_BEGIN;
		item->frame_number = anims[ANIMATION_LARA_ROLL_BEGIN].frame_base + 2;
		item->current_anim_state = STATE_LARA_ROLL_FORWARD;
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (input & IN_DUCK &&
		lara.water_status != LW_WADE &&
		item->current_anim_state == STATE_LARA_STOP &&
		(lara.gun_status == LG_NO_ARMS ||
			lara.gun_type == WEAPON_NONE ||
			lara.gun_type == WEAPON_PISTOLS ||
			lara.gun_type == WEAPON_REVOLVER ||
			lara.gun_type == WEAPON_UZI ||
			lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = STATE_LARA_CROUCH_IDLE;
		return;
	}

	item->goal_anim_state = STATE_LARA_STOP;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_FORWARD)
		fheight = LaraFloorFront(item, item->pos.y_rot, 104);
	else if (input & IN_BACK)
		rheight = LaraFloorFront(item, item->pos.y_rot - 32768, 104);

	if (input & IN_LSTEP)
	{
		height = LaraFloorFront(item, item->pos.y_rot - 16384, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot - 16384, 116, 762);
		if (height < 128 && height > -128 &&
			height_type != BIG_SLOPE &&
			ceiling <= 0)
			item->goal_anim_state = STATE_LARA_WALK_LEFT;
	}
	else if (input & IN_RSTEP)
	{
		height = LaraFloorFront(item, item->pos.y_rot + 16384, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot + 16384, 116, 762);
		if (height < 128 && height > -128 &&
			height_type != BIG_SLOPE &&
			ceiling <= 0)
			item->goal_anim_state = STATE_LARA_WALK_RIGHT;
	}
	else if (input & IN_LEFT)
		item->goal_anim_state = STATE_LARA_TURN_LEFT_SLOW;
	else if (input & IN_RIGHT)
		item->goal_anim_state = STATE_LARA_TURN_RIGHT_SLOW;

	if (lara.water_status == LW_WADE)
	{
		if (input & IN_JUMP)
			item->goal_anim_state = STATE_LARA_JUMP_PREPARE;

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
		item->goal_anim_state = STATE_LARA_JUMP_PREPARE;
	else if (input & IN_FORWARD)
	{
		ceiling = LaraCeilingFront(item, item->pos.y_rot, 104, 762);
		height = LaraFloorFront(item, item->pos.y_rot, 104);

		if ((height_type == BIG_SLOPE || height_type == DIAGONAL) && height < 0 || ceiling > 0)
		{
			item->goal_anim_state = STATE_LARA_STOP;
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
				item->goal_anim_state = STATE_LARA_STOP;
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
			item->goal_anim_state = STATE_LARA_RUN_BACK;
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
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if ((input & IN_BACK) && ((input & IN_WALK) || lara.water_status == LW_WADE))
			item->goal_anim_state = STATE_LARA_WALK_BACK;
		else
			item->goal_anim_state = STATE_LARA_STOP;

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
	lara.move_angle = item->pos.y_rot - 32768;

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
			item->anim_number = ANIMATION_LARA_WALK_DOWN_BACK_RIGHT;
			item->frame_number = anims[ANIMATION_LARA_WALK_DOWN_BACK_RIGHT].frame_base;
		}
		else
		{
			item->anim_number = ANIMATION_LARA_WALK_DOWN_BACK_LEFT;
			item->frame_number = anims[ANIMATION_LARA_WALK_DOWN_BACK_LEFT].frame_base;
		}
	}

	if (!TestLaraSlide(item, coll) && coll->mid_floor != NO_HEIGHT)
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
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

	if (input & IN_FORWARD)
	{
		if (lara.water_status == LW_ABOVE_WATER)
			item->goal_anim_state = STATE_LARA_RUN_FORWARD;
		else
			item->goal_anim_state = STATE_LARA_WADE_FORWARD;
	}
	else
		item->goal_anim_state = STATE_LARA_STOP;
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
		if ((coll->front_type == CT_NONE || coll->front_type == CT_RIGHT) && coll->front_floor < -640)
		{
			item->current_anim_state = STATE_LARA_SPLAT;

			if (item->frame_number >= 0 && item->frame_number <= 9)
			{
				item->anim_number = ANIMATION_LARA_WALL_SMASH_LEFT;
				item->frame_number = anims[ANIMATION_LARA_WALL_SMASH_LEFT].frame_base;
				return;
			}

			if (item->frame_number >= 10 && item->frame_number <= 21)
			{
				item->anim_number = ANIMATION_LARA_WALL_SMASH_RIGHT;
				item->frame_number = anims[ANIMATION_LARA_WALL_SMASH_RIGHT].frame_base;
				return;
			}
		}
		LaraCollideStop(item, coll);
	}
	
	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (item->frame_number >= 3 && item->frame_number <= 14)
		{
			item->anim_number = ANIMATION_LARA_RUN_UP_STEP_LEFT;
			item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIMATION_LARA_RUN_UP_STEP_RIGHT;
			item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_RIGHT].frame_base;
		}
	}
	if (coll->mid_floor < 50)
	{
		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
	else
		item->pos.y_pos += 50;
}

void DoSubsuitStuff()
{
	static short anxiety2;
	static short timer1sub;
	static short timer2sub;
	char anx;
	int anx_mod;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;

	if (CheckCutPlayed(40))
		TriggerAirBubbles();

	if (SubHitCount)
		SubHitCount -= 1;

	anx = lara.Anxiety;

	if (lara.Anxiety > 127)
		anx = 127;

	if (++timer1sub >= timer2sub)
	{
		anxiety2 = anx & 0x70;
		if (lara_item->hit_points > 0)
			SoundEffect(SFX_LARA_SUB_BREATHE, 0, ((((anxiety2 + 128) << 8) | breath_pitch_shifter[anxiety2 >> 4]) << 8) | SFX_ALWAYS | SFX_SETPITCH | SFX_SETVOL);

		timer1sub = -40 - (30 * (128 - anxiety2) >> 7);

		if (lara.Anxiety)
		{
			if (lara.Anxiety <= 128)
				anx = -4;
			else
				anx = -16;

			anx_mod = (lara.Anxiety & 0xFF) + anx;

			if (anx_mod < 0)
				anx_mod = 0;

			lara.Anxiety = lara.Anxiety & 0xffffff00 | anx_mod & 0xff;
			timer2sub = 0;
		}
		else if (timer2sub < 16)
			timer2sub += 2;
	}

	pos.x = 0;
	pos.y = -1024;
	pos.z = -128;
	GetLaraJointPos(&pos, 7);
	pos2.x = 0;
	pos.y = -20480;
	pos.z = -128;
	GetLaraJointPos(&pos2, 7);
	LaraTorch(&pos, &pos2, lara_item->pos.y_rot, 255);
	TriggerEngineEffects();

	if (lara.ChaffTimer)
		lara.ChaffTimer -= 1;

	if (dbinput & IN_SPRINT)
		if (!lara.ChaffTimer)
			FireChaff();
}

void FireChaff()
{
	short item_number;
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	int height;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;

	if (lara.puzzleitems[0])
	{
		item_number = CreateItem();

		if (item_number != NO_ITEM)
		{
			item = &items[item_number];
			SoundEffect(SFX_UNDERWATER_CHAFF, &lara_item->pos, SFX_ALWAYS);
			item->object_number = CHAFF;
			item->shade = -15856;
			--lara.puzzleitems[0];
			pos.x = 0;
			pos.y = -112;
			pos.z = -112;
			GetLaraJointPos(&pos, 7);
			item->room_number = lara_item->room_number;
			floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
			height = GetHeight(floor, pos.x, pos.y, pos.z);

			if (height >= pos.y)
			{
				item->pos.x_pos = pos.x;
				item->pos.y_pos = pos.y;
				item->pos.z_pos = pos.z;
			}
			else
			{
				item->pos.x_pos = lara_item->pos.x_pos;
				item->pos.y_pos = pos.y;
				item->pos.z_pos = lara_item->pos.z_pos;
				item->room_number = lara_item->room_number;
			}

			InitialiseItem(item_number);
			item->pos.x_rot = 0;
			item->pos.y_rot = lara_item->pos.y_rot - 0x8000;
			item->pos.z_rot = 0;
			item->speed = 32;
			item->fallspeed = -128;
			AddActiveItem(item_number);
			lara.ChaffTimer = -116;

			for (int i = 8, b = 63, c = 127; i; --i)
			{
				pos.x = 0;
				pos.y = (GetRandomControl() & 0x1F) - 128;
				pos.z = -112;
				GetLaraJointPos(&pos, 7);
				pos2.x = (GetRandomControl() & 0xFF) - 128;
				pos2.y = c & GetRandomControl();
				pos2.z = -112 - (b & GetRandomControl());
				GetLaraJointPos(&pos2, 7);
				TriggerTorpedoSteam(&pos, &pos2, 1);
			}
		}
	}
}

void lara_as_dash(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || !DashTimer || (!(input & IN_SPRINT) || lara.water_status == LW_WADE))
	{
		item->goal_anim_state = 1;
		return;
	}

	DashTimer--;

	if (input & IN_DUCK &&
		(lara.gun_type == WEAPON_NONE ||
			lara.gun_type == WEAPON_PISTOLS ||
			lara.gun_type == WEAPON_REVOLVER ||
			lara.gun_type == WEAPON_UZI ||
			lara.gun_type == WEAPON_FLARE))
	{
		item->goal_anim_state = STATE_LARA_CROUCH_IDLE;
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

	if (!(input & IN_JUMP) || item->gravity_status)
	{
		if (input & IN_FORWARD)
		{
			if (input & IN_WALK)
				item->goal_anim_state = STATE_LARA_WALK_FORWARD;
			else
				item->goal_anim_state = STATE_LARA_SPRINT;
		}
		else if (!(input & IN_LEFT) && !(input & IN_RIGHT))
			item->goal_anim_state = STATE_LARA_STOP;
	}
	else
		item->goal_anim_state = STATE_LARA_SPRINT_ROLL;
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
			item->current_anim_state = STATE_LARA_SPLAT;
			item->anim_number = ANIMATION_LARA_WALL_SMASH_LEFT;
			item->frame_number = anims[ANIMATION_LARA_WALL_SMASH_LEFT].frame_base;
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
			item->anim_number = ANIMATION_LARA_RUN_UP_STEP_LEFT;
			item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIMATION_LARA_RUN_UP_STEP_RIGHT;
			item->frame_number = anims[ANIMATION_LARA_RUN_UP_STEP_RIGHT].frame_base;
		}
	}

	if (!TestLaraSlide(item, coll))
	{
		if (coll->mid_floor < 50)
		{
			if (coll->mid_floor != NO_HEIGHT)
				item->pos.y_pos += coll->mid_floor;
		}
		else
			item->pos.y_pos += 50;
	}
}

void lara_as_dashdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state != STATE_LARA_DEATH &&
		item->goal_anim_state != STATE_LARA_STOP &&
		item->goal_anim_state != STATE_LARA_RUN_FORWARD &&
		item->fallspeed > 131)
		item->goal_anim_state = STATE_LARA_FREEFALL;
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

	if (!LaraFallen(item, coll))
	{
		if (item->speed < 0)
			lara.move_angle = item->pos.y_rot;

		if (coll->mid_floor <= 0 && item->fallspeed > 0)
		{
			if (LaraLandedBad(item, coll))
				item->goal_anim_state = STATE_LARA_DEATH;
			else if (lara.water_status == LW_WADE || !(input & IN_FORWARD) || input & IN_WALK)
				item->goal_anim_state = STATE_LARA_STOP;
			else
				item->goal_anim_state = STATE_LARA_RUN_FORWARD;

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

int TestWall(ITEM_INFO* item, long front, long right, long down)
{
	int x, y, z, h, c;
	FLOOR_INFO* floor;
	short angle, room_num;

	room_num = item->room_number;
	x = item->pos.x_pos;
	y = down + item->pos.y_pos;
	z = item->pos.z_pos;
	angle = item->pos.y_rot + 8192;

	switch ((unsigned short)(angle >> 14))
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
	default:
		break;
	}

	GetFloor(x, y, z, &room_num);

	switch ((unsigned short)angle >> 14)
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

	default:
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
	short room_num;
	long x;
	long y;
	long z;
	short height;

	room_num = item->room_number;
	x = item->pos.x_pos + ((dist * SIN(ang)) >> 14);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * COS(ang)) >> 14);
	height = GetHeight(GetFloor(x, y, z, &room_num), x, y, z);

	if (height != NO_HEIGHT)
		height -= item->pos.y_pos;

	return height;
}

short LaraCeilingFront(ITEM_INFO* item, short ang, long dist, long h)
{
	short room_num;
	long x, y, z;
	short height;

	x = item->pos.x_pos + ((dist * SIN(ang)) >> 14);
	y = item->pos.y_pos - h;
	z = item->pos.z_pos + ((dist * COS(ang)) >> 14);
	room_num = item->room_number;
	height = GetCeiling(GetFloor(x, y, z, &room_num), x, y, z);

	if (height != NO_HEIGHT)
		height += h - item->pos.y_pos;

	return height;
}

void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	lara.turn_rate += 409;

	if (lara.gun_status != LG_READY || lara.water_status == LW_WADE)
	{
		if (lara.turn_rate > 728)
		{
			if (input & IN_WALK || lara.water_status == LW_WADE)
				lara.turn_rate = 728;
			else
				item->goal_anim_state = STATE_LARA_TURN_FAST;
		}
	}
	else
		item->goal_anim_state = STATE_LARA_TURN_FAST;

	if (!(input & IN_FORWARD))
	{
		if (!(input & IN_RIGHT))
			item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (lara.water_status == LW_WADE)
		item->goal_anim_state = STATE_LARA_WADE_FORWARD;
	else if (input & IN_WALK)
		item->goal_anim_state = STATE_LARA_WALK_FORWARD;
	else
		item->goal_anim_state = STATE_LARA_RUN_FORWARD;
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

	if (coll->mid_floor <= 100)
	{
		if (!TestLaraSlide(item, coll) && coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
	else
	{
		item->fallspeed = 0;
		item->anim_number = ANIMATION_LARA_FREE_FALL_FORWARD;
		item->frame_number = anims[ANIMATION_LARA_FREE_FALL_FORWARD].frame_base;
		item->current_anim_state = STATE_LARA_JUMP_FORWARD;
		item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
		item->gravity_status = 1;
	}
}

void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	lara.turn_rate -= 409;

	if (lara.gun_status != LG_READY || lara.water_status == LW_WADE)
	{
		if (lara.turn_rate < -728)
		{
			if (input & IN_WALK || lara.water_status == LW_WADE)
				lara.turn_rate = -728;
			else
				item->goal_anim_state = STATE_LARA_TURN_FAST;
		}
	}
	else
		item->goal_anim_state = STATE_LARA_TURN_FAST;

	if (!(input & IN_FORWARD))
	{
		if (!(input & IN_LEFT))
			item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (lara.water_status == LW_WADE)
		item->goal_anim_state = STATE_LARA_WADE_FORWARD;
	else if (input & IN_WALK)
		item->goal_anim_state = STATE_LARA_WALK_FORWARD;
	else
		item->goal_anim_state = STATE_LARA_RUN_FORWARD;
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
		AlterFOV(14560);
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
	item->speed = (item->speed * 95) / 100;
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
			item->goal_anim_state = STATE_LARA_DEATH;
		else
		{
			item->goal_anim_state = STATE_LARA_STOP;
			item->current_anim_state = STATE_LARA_STOP;
			item->anim_number = ANIMATION_LARA_LANDING_HARD;
			item->frame_number = anims[ANIMATION_LARA_LANDING_HARD].frame_base;
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
		item->pos.z_pos -= (100 * COS(coll->facing)) >> 14;
		item->pos.x_pos -= (100 * SIN(coll->facing)) >> 14;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed <= 0)
			item->fallspeed = 16;

		break;

	default:
		break;
	}
}

void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.IsClimbing = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
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
	item->fallspeed = 0;
	item->gravity_status = 0;

	if (item->anim_number == ANIMATION_LARA_HANG_IDLE && item->frame_number == anims[ANIMATION_LARA_HANG_IDLE].frame_base + 21)
	{
		int flag;

		if (input & IN_LEFT || input & IN_LSTEP)
		{
			if (CanLaraHangSideways(item, coll, -16384))
			{
				item->goal_anim_state = STATE_LARA_SHIMMY_LEFT;
				return;
			}

			flag = LaraHangLeftCornerTest(item, coll);

			if (flag)
			{
				if (flag <= 0)
					item->goal_anim_state = STATE_LARA_CLIMB_CORNER_LEFT_INNER;
				else
					item->goal_anim_state = STATE_LARA_CLIMB_CORNER_LEFT_OUTER;

				return;
			}
		}

		if (input & IN_RIGHT || input & IN_RSTEP)
		{
			if (CanLaraHangSideways(item, coll, 16384))
			{
				item->goal_anim_state = STATE_LARA_SHIMMY_RIGHT;
				return;
			}

			flag = LaraHangRightCornerTest(item, coll);

			if (flag)
			{
				if (flag <= 0)
					item->goal_anim_state = STATE_LARA_CLIMB_CORNER_RIGHT_INNER;
				else
					item->goal_anim_state = STATE_LARA_CLIMB_CORNER_RIGHT_OUTER;

				return;
			}
		}
	}

	lara.move_angle = item->pos.y_rot;
	LaraHangTest(item, coll);

	if (item->anim_number == ANIMATION_LARA_HANG_IDLE && item->frame_number == anims[ANIMATION_LARA_HANG_IDLE].frame_base + 21)
	{
		TestForObjectOnLedge(item, coll);
		if (input & IN_FORWARD)
		{
			if (coll->front_floor > -850)
			{
				if (coll->front_floor < -650 &&
					coll->front_floor >= coll->front_ceiling &&
					coll->front_floor >= coll->left_ceiling2 &&
					coll->front_floor >= coll->right_ceiling2)
				{
					if (ABS(coll->left_floor2 - coll->right_floor2) < 60 && !coll->hit_static)
					{
						if (input & IN_WALK)
							item->goal_anim_state = STATE_LARA_HANDSTAND;
						else if (input & IN_DUCK)
						{
							item->goal_anim_state = STATE_LARA_CLIMB_TO_CRAWL;
							item->required_anim_state = STATE_LARA_CROUCH_IDLE;
						}
						else
							item->goal_anim_state = STATE_LARA_GRABBING;

						return;
					}
				}

				if (coll->front_floor < -650 &&
					coll->front_floor - coll->front_ceiling >= -256 &&
					coll->front_floor - coll->left_ceiling2 >= -256 &&
					coll->front_floor - coll->right_ceiling2 >= -256)
				{
					if (ABS(coll->left_floor2 - coll->right_floor2) < 60 && !coll->hit_static)
					{
						item->goal_anim_state = STATE_LARA_CLIMB_TO_CRAWL;
						item->required_anim_state = STATE_LARA_CROUCH_IDLE;

						return;
					}
				}
			}

			if (lara.climb_status != 0)
			{
				item->goal_anim_state = STATE_LARA_CLIMB_TO_CRAWL;
				item->required_anim_state = STATE_LARA_CROUCH_IDLE;
			}
			else
				item->goal_anim_state = STATE_LARA_GRABBING;

			if (lara.climb_status != 0 &&
				coll->mid_ceiling <= -256 &&
				ABS(coll->left_ceiling2 - coll->right_ceiling2) < 60)
			{
				if (LaraTestClimbStance(item, coll))
					item->goal_anim_state = STATE_LARA_LADDER_IDLE;
				else
				{
					item->anim_number = ANIMATION_LARA_LADDER_UP_HANDS;
					item->frame_number = anims[ANIMATION_LARA_LADDER_UP_HANDS].frame_base;
					item->goal_anim_state = STATE_LARA_HANG;
					item->current_anim_state = STATE_LARA_HANG;
				}
			}

			return;
		}

		if (input & IN_BACK &&
			lara.climb_status != 0 &&
			coll->mid_floor > 344)
		{
			if (LaraTestClimbStance(item, coll))
				item->goal_anim_state = STATE_LARA_LADDER_IDLE;
			else
			{
				item->anim_number = ANIMATION_LARA_LADDER_DOWN_HANDS;
				item->frame_number = anims[ANIMATION_LARA_LADDER_DOWN_HANDS].frame_base;
				item->goal_anim_state = STATE_LARA_HANG;
				item->current_anim_state = STATE_LARA_HANG;
			}
		}
	}
}

int CanLaraHangSideways(ITEM_INFO* item,COLL_INFO* coll, short angle)
{
	long oldx, oldz, x, z;
	int res;

	oldx = item->pos.x_pos;
	oldz = item->pos.z_pos;
	x = item->pos.x_pos;
	z = item->pos.z_pos;
	lara.move_angle = angle + item->pos.y_rot;

	switch ((unsigned short)(lara.move_angle + 0x2000) / (unsigned short)0x4000)
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

	if (!res)
		return 1;
	else
		return 0;
}

void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 15470;

	if (item->fallspeed > 131)
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_col_reach(ITEM_INFO* item, COLL_INFO* coll)//whoever wrote this is one son of a whore.
{
	short angle = 1;
	long edge = 0;
	int edge_catch = 0;

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
			item->anim_number = ANIMATION_LARA_OSCILLATE_HANG_ON;
			item->frame_number = anims[ANIMATION_LARA_OSCILLATE_HANG_ON].frame_base;
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->gravity_status = 0;
			item->speed = 0;
			item->fallspeed = 0;
			lara.gun_status = LG_HANDS_BUSY;
			return;
		}


		if (coll->mid_ceiling <= -384 &&
			coll->mid_floor >= 200 &&
			coll->coll_type == CT_FRONT)
		{
			edge_catch = LaraTestEdgeCatch(item, coll, &edge);

			if (!(!edge_catch || edge_catch < 0 && !LaraTestHangOnClimbWall(item, coll)))
			{
				angle = item->pos.y_rot;

				if (angle >= -6370 && angle <= 6370)
					angle = 0;
				else if (angle >= 10014 && angle <= 22754)
					angle = 16384;
				else if (angle >= 26397 || angle <= -26397)
					angle = -32768;
				else if (angle >= -22754 && angle <= -10014)
					angle = -16384;
			}
		}
	}

	if (!(angle & 0x3FFF))
	{
		short* bounds;

		if (TestHangSwingIn(item, angle))
		{
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			item->anim_number = ANIMATION_LARA_OSCILLATE_HANG_ON;
			item->frame_number = anims[ANIMATION_LARA_OSCILLATE_HANG_ON].frame_base;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
		}
		else
		{
			item->anim_number = ANIMATION_LARA_HANG_IDLE;
			item->frame_number = anims[ANIMATION_LARA_HANG_IDLE].frame_base;
			item->current_anim_state = STATE_LARA_HANG;
			item->goal_anim_state = STATE_LARA_HANG;
		}

		bounds = GetBoundsAccurate(item);

		if (edge_catch <= 0)
			item->pos.y_pos = edge - bounds[2] - 22;
		else
		{
			item->pos.y_pos += coll->front_floor - bounds[2];

			switch ((unsigned short)(item->pos.y_rot + 8192) >> 14)
			{
			case NORTH:
				item->pos.z_pos = (item->pos.z_pos | 0x3FF) - 100;
				item->pos.x_pos += coll->shift.x;
				break;

			case EAST:
				item->pos.x_pos = (item->pos.x_pos | 0x3FF) - 100;
				item->pos.z_pos += coll->shift.z;
				break;

			case SOUTH:
				item->pos.z_pos = (item->pos.z_pos & 0xFFFFFC00) + 100;
				item->pos.x_pos += coll->shift.x;
				break;

			case WEST:
				item->pos.x_pos = (item->pos.x_pos & 0xFFFFFC00) + 100;
				item->pos.z_pos += coll->shift.z;
				break;

			default:
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
				item->goal_anim_state = STATE_LARA_DEATH;
			else
			{
				item->gravity_status = 0;
				item->goal_anim_state = STATE_LARA_STOP;
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
			item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
			lara.move_angle = item->pos.y_rot;
		}
		else if (input & IN_LEFT && LaraFloorFront(item, item->pos.y_rot - 16384, 256) >= -384)
		{
			item->goal_anim_state = STATE_LARA_JUMP_RIGHT;
			lara.move_angle = item->pos.y_rot - 16384;
		}
		else if (input & IN_RIGHT && LaraFloorFront(item, item->pos.y_rot + 16384, 256) >= -384)
		{
			item->goal_anim_state = STATE_LARA_JUMP_LEFT;
			lara.move_angle = item->pos.y_rot + 16384;
		}
		else if (input & IN_BACK && LaraFloorFront(item, item->pos.y_rot - 32768, 256) >= -384)
		{
			item->goal_anim_state = STATE_LARA_JUMP_BACK;
			lara.move_angle = item->pos.y_rot - 32768;
		}
	}

	if (item->fallspeed > 131)
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_col_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = 0;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (!LaraFallen(item, coll))
	{
		if (coll->mid_ceiling > -100)
		{
			item->anim_number = ANIMATION_LARA_STAY_SOLID;
			item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
			item->goal_anim_state = STATE_LARA_STOP;
			item->current_anim_state = STATE_LARA_STOP;
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
}

void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (lara.turn_rate >= 0)
	{
		lara.turn_rate = 1456;

		if (!(input & IN_RIGHT))
			item->goal_anim_state = STATE_LARA_STOP;
	}
	else
	{
		lara.turn_rate = -1456;

		if (!(input & IN_LEFT))
			item->goal_anim_state = STATE_LARA_STOP;
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
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if (!(input & IN_RSTEP))
			item->goal_anim_state = STATE_LARA_STOP;

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
	if (item->current_anim_state == STATE_LARA_WALK_RIGHT)
		lara.move_angle = item->pos.y_rot + 16384;
	else
		lara.move_angle = item->pos.y_rot - 16384;

	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->bad_pos = lara.water_status == LW_WADE ? -NO_HEIGHT : 128;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->bad_neg = -128;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (!LaraHitCeiling(item, coll))
	{
		if (LaraDeflectEdge(item, coll))
			LaraCollideStop(item, coll);
		if (!LaraFallen(item, coll) && !TestLaraSlide(item, coll) && coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	if (!lara.IsMoving)
	{
		if (!(input & IN_LSTEP))
			item->goal_anim_state = STATE_LARA_STOP;

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
	camera.lara_node = 0;
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot - 32768;
	coll->slopes_are_walls = 1;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (!LaraHitCeiling(item, coll) && !TestLaraSlide(item, coll))
	{
		if (coll->mid_floor <= 200)
		{
			ShiftItem(item, coll);

			if (coll->mid_floor != NO_HEIGHT)
				item->pos.y_pos += coll->mid_floor;
		}
		else
		{
			item->fallspeed = 0;
			item->anim_number = ANIMATION_LARA_FREE_FALL_BACK;
			item->frame_number = anims[ANIMATION_LARA_FREE_FALL_BACK].frame_base;
			item->current_anim_state = STATE_LARA_FALL_BACKWARD;
			item->goal_anim_state = STATE_LARA_FALL_BACKWARD;
			item->gravity_status = 1;
		}
	}
}

void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	if ((input & IN_JUMP) && !(input & IN_BACK))
		item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
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

	if (!LaraHitCeiling(item, coll))
	{
		LaraDeflectEdge(item, coll);

		if (coll->mid_floor <= 200)
		{
			TestLaraSlide(item, coll);
			item->pos.y_pos += coll->mid_floor;

			if (ABS(coll->tilt_x) <= 2 && ABS(coll->tilt_z) <= 2)
			{
				item->goal_anim_state = STATE_LARA_STOP;
				StopSoundEffect(SFX_LARA_SLIPPING);
			}
		}
		else
		{
			if (item->current_anim_state == STATE_LARA_SLIDE_FORWARD)
			{
				item->anim_number = ANIMATION_LARA_FREE_FALL_FORWARD;
				item->frame_number = anims[ANIMATION_LARA_FREE_FALL_FORWARD].frame_base;
				item->current_anim_state = STATE_LARA_JUMP_FORWARD;
				item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
			}
			else
			{
				item->anim_number = ANIMATION_LARA_FREE_FALL_BACK;
				item->frame_number = anims[ANIMATION_LARA_FREE_FALL_BACK].frame_base;
				item->current_anim_state = STATE_LARA_FALL_BACKWARD;
				item->goal_anim_state = STATE_LARA_FALL_BACKWARD;
			}

			StopSoundEffect(SFX_LARA_SLIPPING);
			item->gravity_status = 1;
			item->fallspeed = 0;
		}
	}
}

void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallspeed > 131)
		item->goal_anim_state = STATE_LARA_FREEFALL;
}

void lara_col_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	coll->hit_ceiling = 0;
	coll->facing = item->speed >= 0 ? lara.move_angle : lara.move_angle - 32768;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && !coll->hit_static)
	{
		if (lara.CanMonkeySwing && coll->coll_type == CT_TOP)
		{
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->anim_number = ANIMATION_LARA_MONKEY_GRAB;
			item->frame_number = anims[ANIMATION_LARA_MONKEY_GRAB].frame_base;
			item->gravity_status = 0;
			item->speed = 0;
			item->fallspeed = 0;
			lara.gun_status = LG_HANDS_BUSY;
			MonkeySwingSnap(item, coll);
			return;
		}

		if (coll->coll_type == CT_FRONT && coll->mid_ceiling <= -384)
		{
			long edge;
			const int edge_catch = LaraTestEdgeCatch(item, coll, &edge);

			if (edge_catch)
			{
				if (edge_catch >= 0 || LaraTestHangOnClimbWall(item, coll))
				{
					short angle = item->pos.y_rot;

					if (angle >= -6370 && angle <= 6370)
						angle = 0;
					else if (angle >= 10014 && angle <= 22754)
						angle = 16384;
					else if (angle >= 26397 || angle <= -26397)
						angle = -32768;
					else if (angle >= -22754 && angle <= -10014)
						angle = -16384;

					if (!(angle & 0x3FFF))
					{
						short* bounds;

						if (TestHangSwingIn(item, angle))
						{
							item->anim_number = ANIMATION_LARA_MONKEY_GRAB;
							item->frame_number = anims[ANIMATION_LARA_MONKEY_GRAB].frame_base;
							item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
							item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
						}
						else
						{
							item->anim_number = ANIMATION_LARA_HANG_IDLE;
							item->frame_number = anims[ANIMATION_LARA_HANG_IDLE].frame_base + 12;
							item->goal_anim_state = STATE_LARA_HANG;
							item->current_anim_state = STATE_LARA_HANG;
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
	}

	ShiftItem(item, coll);

	if (coll->coll_type == CT_CLAMP ||
		coll->coll_type == CT_TOP ||
		coll->coll_type == CT_TOP_FRONT ||
		coll->hit_ceiling)
		item->fallspeed = 1;

	if (coll->coll_type == CT_NONE)
	{
		if (item->fallspeed < -70)
		{
			if (input & IN_FORWARD && item->speed < 5)
				item->speed++;
			else if (input & IN_BACK && item->speed > -5)
				item->speed -= 2;
		}
	}
	else
		item->speed = item->speed <= 0 ? -2 : 2;

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = STATE_LARA_DEATH;
		else
			item->goal_anim_state = STATE_LARA_STOP;

		item->gravity_status = 0;
		item->fallspeed = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void MonkeySwingSnap(ITEM_INFO* item, COLL_INFO* coll)
{
	short room_num;
	
	room_num = item->room_number;
	item->pos.y_pos = GetCeiling(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num),
		item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 704;
}

void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 0;
	camera.target_elevation = -8190;

	if (!(input & (IN_LEFT | IN_LSTEP)))
		item->goal_anim_state = STATE_LARA_HANG;
}

void lara_col_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 16384;
	coll->radius = 102;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot - 16384;
}

void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 0;
	camera.target_elevation = -8190;

	if (!(input & (IN_RIGHT | IN_RSTEP)))
		item->goal_anim_state = STATE_LARA_HANG;
}

void lara_col_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 16384;
	coll->radius = 102;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot + 16384;
}

void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	if ((input & IN_JUMP) && !(input & IN_FORWARD))
		item->goal_anim_state = STATE_LARA_JUMP_BACK;
}

void lara_col_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 32768;
	lara_slide_slope(item, coll);
}

void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = 16380;
	camera.target_elevation = -4550;
}

void lara_as_pullblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = 6370;
	camera.target_elevation = -4550;
}

void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 13650;
	if (!(input & IN_ACTION))
		item->goal_anim_state = STATE_LARA_STOP;
}

void lara_as_usepuzzle(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;

	if (item->frame_number == anims[item->anim_number].frame_end)
	{
		if (item->item_flags[0])
		{
			item->anim_number = item->item_flags[0];
			item->current_anim_state = STATE_LARA_MISC_CONTROL;
			item->frame_number = anims[item->item_flags[0]].frame_base;
		}
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
	coll->enable_baddie_push = FALSE;
	coll->enable_spaz = FALSE;
	camera.target_angle = -23660;
	camera.target_elevation = -2730;
	camera.target_distance = 1024;
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

	if (item->fallspeed > 131 && item->goal_anim_state != STATE_LARA_UNDERWATER_DIVING)
		item->goal_anim_state = STATE_LARA_SWANDIVE_END;
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
		item->goal_anim_state = STATE_LARA_STOP;
		item->fallspeed = 0;
		item->gravity_status = 0;

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;
	}
}

void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_ROLL && item->goal_anim_state == STATE_LARA_SWANDIVE_END)
		item->goal_anim_state = STATE_LARA_JUMP_ROLL;

	coll->enable_baddie_push = 1;
	coll->enable_spaz = 0;
	item->speed = (item->speed * 95) / 100;
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
			item->goal_anim_state = STATE_LARA_STOP;
		else
			item->goal_anim_state = STATE_LARA_DEATH;

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
}

void lara_as_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	short room_num;

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.IsDucked = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
		return;
	}

	if ((input & IN_LOOK))
		LookUpDown();

	room_num = lara_item->room_number;
	GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_num);

	if ((input & IN_FORWARD || input & IN_BACK))
	{
		if (((input & IN_DUCK) || lara.keep_ducked && lara.water_status != LW_WADE) && lara.gun_status == LG_NO_ARMS)
		{
			if (!(room[room_num].flags & RF_FILL_WATER))
			{
				if (lara_item->anim_number == ANIMATION_LARA_CROUCH_IDLE || lara_item->anim_number == ANIMATION_LARA_CROUCH_PREPARE)
				{
					if (!(input & (IN_B | IN_DRAW)))
					{
						if (lara.gun_type != WEAPON_FLARE || lara.flare_age < 900 && lara.flare_age)
						{
							lara.torso_y_rot = 0;
							lara.torso_x_rot = 0;
							item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
						}
					}
				}
			}
		}
	}
}

void lara_col_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = FALSE;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->facing = item->pos.y_rot;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraFallen(item, coll))
		lara.gun_status = LG_NO_ARMS;
	else if (!TestLaraSlide(item, coll))
	{
		if (coll->mid_ceiling >= -362)
			lara.keep_ducked = 1;
		else
			lara.keep_ducked = 0;

		ShiftItem(item, coll);

		if (coll->mid_floor != NO_HEIGHT)
			item->pos.y_pos += coll->mid_floor;

		if (input & IN_DUCK && lara.water_status != LW_WADE || lara.keep_ducked || item->anim_number != ANIMATION_LARA_CROUCH_IDLE)
		{
			if (input & IN_LEFT)
				item->goal_anim_state = STATE_LARA_CROUCH_TURN_LEFT;
			else if (input & IN_RIGHT)
				item->goal_anim_state = STATE_LARA_CROUCH_TURN_RIGHT;
		}
		else
			item->goal_anim_state = STATE_LARA_STOP;
	}
}

void MonkeySwingFall(ITEM_INFO* item)
{
	item->goal_anim_state = STATE_LARA_JUMP_UP;
	item->current_anim_state = STATE_LARA_JUMP_UP;
	item->anim_number = ANIMATION_LARA_TRY_HANG_VERTICAL;
	item->frame_number = anims[ANIMATION_LARA_TRY_HANG_VERTICAL].frame_base + 9;
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
		item->goal_anim_state = STATE_LARA_STOP;
		return;
	}

	coll->enable_baddie_push = FALSE;
	coll->enable_spaz = FALSE;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (lara.CanMonkeySwing)
	{
		if (!(input & IN_ACTION) || item->hit_points <= 0)
			MonkeySwingFall(item);

		camera.target_angle = 0;
		camera.target_elevation = -8190;
	}

	if (input & IN_LOOK)
		LookUpDown();
}

void lara_col_hang2(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = FALSE;

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

		if (input & IN_FORWARD && coll->coll_type != CT_FRONT && ABS(coll->mid_ceiling - coll->front_ceiling) < 50)
			item->goal_anim_state = STATE_LARA_MONKEYSWING_FORWARD;
		else if (input & IN_LSTEP && TestMonkeyLeft(item, coll))
			item->goal_anim_state = STATE_LARA_MONKEYSWING_LEFT;
		else if (input & IN_RSTEP && TestMonkeyRight(item, coll))
			item->goal_anim_state = STATE_LARA_MONKEYSWING_RIGHT;
		else if (input & IN_LEFT)
			item->goal_anim_state = STATE_LARA_MONKEYSWING_TURN_LEFT;
		else if (input & IN_RIGHT)
			item->goal_anim_state = STATE_LARA_MONKEYSWING_TURN_RIGHT;

		MonkeySwingSnap(item, coll);
	}
	else
	{
		LaraHangTest(item, coll);

		if (item->goal_anim_state == STATE_LARA_MONKEYSWING_IDLE)
		{
			TestForObjectOnLedge(item, coll);

			if (!(input & IN_FORWARD) ||
				coll->front_floor <= -850 ||
				coll->front_floor >= -650 ||
				coll->front_floor < coll->front_ceiling ||
				coll->left_floor2 < coll->left_ceiling2 ||
				coll->right_floor2 < coll->right_ceiling2 ||
				coll->hit_static)
			{
				if (!(input & IN_FORWARD) ||
					coll->front_floor <= -850 ||
					coll->front_floor >= -650 ||
					coll->front_floor - coll->front_ceiling < -256 ||
					coll->left_floor2 - coll->left_ceiling2 < -256 ||
					coll->right_floor2 - coll->right_ceiling2 < -256 ||
					coll->hit_static)
				{
					if (input & IN_LEFT || input & IN_LSTEP)
						item->goal_anim_state = STATE_LARA_SHIMMY_LEFT;
					else if (input & IN_RIGHT || input & IN_RSTEP)
						item->goal_anim_state = STATE_LARA_SHIMMY_RIGHT;
				}
				else
				{
					item->goal_anim_state = STATE_LARA_CLIMB_TO_CRAWL;
					item->required_anim_state = STATE_LARA_CROUCH_IDLE;
				}
			}
			else if (input & IN_WALK)
				item->goal_anim_state = STATE_LARA_HANDSTAND;
			else if (input & IN_DUCK)
			{
				item->goal_anim_state = STATE_LARA_CLIMB_TO_CRAWL;
				item->required_anim_state = STATE_LARA_CROUCH_IDLE;
			}
			else
				item->goal_anim_state = STATE_LARA_GRABBING;
		}
	}
}

void lara_as_monkeyswing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_FORWARD)
		item->goal_anim_state = STATE_LARA_MONKEYSWING_FORWARD;
	else
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;

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
	if (input & IN_ACTION && lara.CanMonkeySwing)
	{
		coll->bad_pos = -NO_HEIGHT;
		coll->bad_neg = NO_HEIGHT;
		coll->bad_ceiling = 0;
		lara.move_angle = item->pos.y_rot;
		coll->enable_spaz = FALSE;
		coll->enable_baddie_push = FALSE;
		coll->facing = lara.move_angle;
		coll->radius = 100;
		GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

		if (coll->coll_type == CT_FRONT
			|| ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
		{
			item->anim_number = ANIMATION_LARA_MONKEY_IDLE;
			item->frame_number = anims[ANIMATION_LARA_MONKEY_IDLE].frame_base;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
		}
		else
		{
			if (ABS(coll->mid_ceiling - coll->left_ceiling2) <= 50)
			{
				if (ABS(coll->mid_ceiling - coll->right_ceiling2) > 50)
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
	else
		MonkeySwingFall(item);
}

void lara_as_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
		return;
	}

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_LSTEP)
		item->goal_anim_state = STATE_LARA_MONKEYSWING_LEFT;
	else
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;

	camera.target_elevation = 1820;
}

void lara_col_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if ((input & IN_ACTION) && lara.CanMonkeySwing)
	{
		if (TestMonkeyLeft(item, coll))
			MonkeySwingSnap(item, coll);
		else
		{
			item->anim_number = ANIMATION_LARA_MONKEY_IDLE;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->frame_number = anims[ANIMATION_LARA_MONKEY_IDLE].frame_base;
		}
	}
	else
		MonkeySwingFall(item);
}

void lara_as_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
		return;
	}

	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;

	if (input & IN_RSTEP)
		item->goal_anim_state = STATE_LARA_MONKEYSWING_RIGHT;
	else
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;

	camera.target_elevation = 1820;
}

void lara_col_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if ((input & IN_ACTION) && lara.CanMonkeySwing)
	{
		if (TestMonkeyRight(item, coll))
			MonkeySwingSnap(item, coll);
		else
		{
			item->anim_number = ANIMATION_LARA_MONKEY_IDLE;
			item->current_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
			item->frame_number = anims[ANIMATION_LARA_MONKEY_IDLE].frame_base;
		}
	}
	else
		MonkeySwingFall(item);
}

void lara_as_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
}

void lara_col_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_monkeyswing(item, coll);
}

void lara_as_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_DEATH;
		return;
	}

	if (input & IN_JUMP)
	{
		if (LaraFloorFront(item, item->pos.y_rot, 768) >= 512 &&
			LaraCeilingFront(item, item->pos.y_rot, 768, 512) != NO_HEIGHT &&
			LaraCeilingFront(item, item->pos.y_rot, 768, 512) <= 0)
		{
			GAME_VECTOR s, d;
			MESH_INFO* StaticMesh;
			PHD_VECTOR v;

			s.x = lara_item->pos.x_pos;
			s.y = lara_item->pos.y_pos - 96;
			s.z = lara_item->pos.z_pos;
			s.room_number = lara_item->room_number;
			d.x = s.x + (768 * SIN(lara_item->pos.y_rot) >> 14);
			d.y = s.y + 160;
			d.z = s.z + (768 * COS(lara_item->pos.y_rot) >> 14);

			if (LOS(&s, &d))
			{
				if (ObjectOnLOS2(&s, &d, &v, &StaticMesh) == 999)
				{
					item->anim_number = ANIMATION_LARA_CRAWL_JUMP_DOWN;
					item->frame_number = anims[ANIMATION_LARA_CRAWL_JUMP_DOWN].frame_base;
					item->goal_anim_state = STATE_LARA_MISC_CONTROL;
					item->current_anim_state = STATE_LARA_MISC_CONTROL;
				}
			}
		}
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = FALSE;
	coll->enable_baddie_push = TRUE;

	if (item->anim_number == ANIMATION_LARA_CROUCH_TO_CRAWL_BEGIN)
		lara.gun_status = LG_HANDS_BUSY;

	camera.target_elevation = -4186;
}

void lara_col_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
	item->gravity_status = FALSE;

	if (item->goal_anim_state == STATE_LARA_CRAWL_TO_CLIMB)
		return;

	lara.move_angle = item->pos.y_rot;
	coll->facing = item->pos.y_rot;
	coll->radius = 200;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraFallen(item, coll))
		lara.gun_status = LG_NO_ARMS;
	else if (!TestLaraSlide(item, coll))
	{
		int slope;
		
		slope = ABS(coll->left_floor2 - coll->right_floor2);
		lara.keep_ducked = coll->mid_ceiling >= -362;
		ShiftItem(item, coll);

		if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
			item->pos.y_pos += coll->mid_floor;

		if ((input & IN_DUCK || (input & IN_DUCK && input & IN_FORWARD) && lara.water_status != LW_WADE ) || lara.keep_ducked)
		{

			if (input & IN_DRAW || input & IN_FLARE)
			{
				item->goal_anim_state = STATE_LARA_CROUCH_IDLE;
				return;
			}

			if (item->anim_number == ANIMATION_LARA_CRAWL_IDLE ||
				item->anim_number == ANIMATION_LARA_CROUCH_TO_CRAWL_END)
			{
				if (input & IN_FORWARD)
				{
					if (ABS(LaraFloorFront(item, item->pos.y_rot, 256)) < 255 && height_type != BIG_SLOPE)
						item->goal_anim_state = STATE_LARA_CRAWL_FORWARD;
				}
				else if (input & IN_BACK)
				{
					short height = LaraCeilingFront(item, item->pos.y_rot, -300, 128);
					short heightl = 0;
					short heightr = 0;

					if (height != NO_HEIGHT && height <= 256)
					{
						if (input & IN_ACTION)
						{
							long x; 
							long z;

							x = item->pos.x_pos;
							z = item->pos.z_pos;
							item->pos.x_pos += 128 * SIN(item->pos.y_rot - 16384) >> 14;
							item->pos.z_pos += 128 * COS(item->pos.y_rot - 16384) >> 14;
							heightl = LaraFloorFront(item, item->pos.y_rot, -300);
							item->pos.x_pos += 256 * SIN(item->pos.y_rot + 16384) >> 14;
							item->pos.z_pos += 256 * COS(item->pos.y_rot + 16384) >> 14;
							heightr = LaraFloorFront(item, item->pos.y_rot, -300);
							item->pos.x_pos = x;
							item->pos.z_pos = z;
						}

						height = LaraFloorFront(item, item->pos.y_rot, -300);

						if (ABS(height) >= 255 || height_type == BIG_SLOPE)
						{
							if (input & IN_ACTION)
							{
								if (height > 768 &&
									heightl > 768 &&
									heightr > 768 &&
									slope < 120)
								{
									int collided;
									long x;
									long z;

									x = item->pos.x_pos;
									z = item->pos.z_pos;
									item->pos.x_pos -= 100 * SIN(coll->facing) >> 14;
									item->pos.z_pos -= 100 * SIN(coll->facing) >> 14;
									collided = GetCollidedObjects(item, 100, 1, itemlist, meshlist, 0);
									item->pos.x_pos = x;
									item->pos.z_pos = z;

									if (!collided)
									{
										switch ((unsigned short)(item->pos.y_rot + 8192) >> 14)
										{
										case NORTH:
											item->pos.y_rot = 0;
											item->pos.z_pos = (item->pos.z_pos & 0xFFFFFC00) + 225;
											break;
										case EAST:
											item->pos.y_rot = 16384;
											item->pos.x_pos = (item->pos.x_pos & 0xFFFFFC00) + 225;
											break;
										case SOUTH:
											item->pos.y_rot = 32768;
											item->pos.z_pos = (item->pos.z_pos | 0x3FF) - 225;
											break;
										case WEST:
											item->pos.y_rot = -16384;
											item->pos.x_pos = (item->pos.x_pos | 0x3FF) - 225;
											break;
										}

										item->goal_anim_state = STATE_LARA_CRAWL_TO_CLIMB;
									}
								}
							}
						}
						else
							item->goal_anim_state = STATE_LARA_CRAWL_BACK;
					}
				}
				else if (input & IN_LEFT)
				{
					item->anim_number = ANIMATION_LARA_CRAWL_TURN_LEFT;
					item->frame_number = anims[ANIMATION_LARA_CRAWL_TURN_LEFT].frame_base;
					item->current_anim_state = STATE_LARA_CRAWL_TURN_LEFT;
					item->goal_anim_state = STATE_LARA_CRAWL_TURN_LEFT;
				}
				else if (input & IN_RIGHT)
				{
					item->anim_number = ANIMATION_LARA_CRAWL_TURN_RIGHT;
					item->frame_number = anims[ANIMATION_LARA_CRAWL_TURN_RIGHT].frame_base;
					item->current_anim_state = STATE_LARA_CRAWL_TURN_RIGHT;
					item->goal_anim_state = STATE_LARA_CRAWL_TURN_RIGHT;
				}
			}
		}
		else
			item->goal_anim_state = STATE_LARA_CROUCH_IDLE;
	}
}

void lara_as_crawl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || input & IN_JUMP)
	{
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	camera.target_elevation = -4186;

	if (input & IN_FORWARD
		&& (input & IN_DUCK || lara.keep_ducked)
		&& lara.water_status != LW_WADE)
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
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
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

	if (LaraDeflectEdgeDuck(item, coll))
	{
		item->current_anim_state = STATE_LARA_CRAWL_IDLE;
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;

		if (item->anim_number != ANIMATION_LARA_CRAWL_IDLE)
		{
			item->anim_number = ANIMATION_LARA_CRAWL_IDLE;
			item->frame_number = anims[ANIMATION_LARA_CRAWL_IDLE].frame_base;
		}
	}
	else if (LaraFallen(item, coll))
		lara.gun_status = LG_NO_ARMS;
	else if (!TestLaraSlide(item, coll))
	{
		ShiftItem(item, coll);

		if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
			item->pos.y_pos += coll->mid_floor;
	}
}

int LaraDeflectEdgeDuck(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->gravity_status = 0;
		item->speed = 0;
		return 1;
	}
	else if (coll->coll_type == CT_LEFT)
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
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
}

void lara_as_hangturnr(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = 1820;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	item->pos.y_rot += 273;

	if (item->hit_points <= 0 || !(input & IN_RIGHT))
		item->goal_anim_state = STATE_LARA_MONKEYSWING_IDLE;
}

void lara_col_hangturnlr(ITEM_INFO* item, COLL_INFO* coll)
{
	if ((input & IN_ACTION) && lara.CanMonkeySwing)
	{
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
	else
		MonkeySwingFall(item);
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
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
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
		item->goal_anim_state = STATE_LARA_CRAWL_IDLE;
}

void lara_col_all4turnlr(ITEM_INFO* item, COLL_INFO* coll)
{
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (!TestLaraSlide(item, coll))
		if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
			item->pos.y_pos += coll->mid_floor;
}

void inject_lara()
{
	INJECT(0x00445AE0, LaraDeflectEdgeJump);
	INJECT(0x00445A50, LaraLandedBad);
	INJECT(0x004431F0, TestLaraSlide);
	INJECT(0x00445060, LaraDeflectEdge);
	INJECT(0x004457B0, LaraCollideStop);
	INJECT(0x00444FD0, LaraHitCeiling);
	INJECT(0x00444F80, GetLaraCollisionInfo);
	INJECT(0x00449550, lara_as_fastback);
	INJECT(0x0044C080, lara_col_fastback);
	INJECT(0x0044B270, lara_as_fallback);
	INJECT(0x0044CAF0, lara_col_fallback);
	INJECT(0x0044D180, lara_col_jumper);
	INJECT(0x0044B220, lara_as_leftjump);
	INJECT(0x0044B1D0, lara_as_rightjump);
	INJECT(0x0044B160, lara_as_backjump);
	INJECT(0x00448B80, lara_col_forwardjump);
	INJECT(0x00448A70, lara_as_forwardjump);
	INJECT(0x0044D310, LookUpDown);
	INJECT(0x0044D440, LookLeftRight);
	INJECT(0x00449840, lara_void_func);
	INJECT(0x00449260, lara_as_walk);
	INJECT(0x0044BBC0, lara_col_walk);
	INJECT(0x00443380, LaraFallen);
	INJECT(0x00445100, TestLaraVault);
	INJECT(0x0044AEE0, lara_as_null);
	INJECT(0x00449330, lara_as_run);
	INJECT(0x0044BE30, lara_col_run);
	INJECT(0x00448010, lara_as_stop);
	INJECT(0x00448540, lara_col_stop);
	INJECT(0x0044AE20, lara_as_back);
	INJECT(0x0044C5E0, lara_col_back);
	INJECT(0x0044B770, lara_as_wade);
	INJECT(0x0044CF40, lara_col_wade);
	INJECT(0x0047C950, DoSubsuitStuff);
	INJECT(0x0047C6D0, FireChaff);
	INJECT(0x00444C20, lara_as_dash);
	INJECT(0x00444DD0, lara_col_dash);
	INJECT(0x004458A0, lara_as_dashdive);
	INJECT(0x004458E0, lara_col_dashdive);
	INJECT(0x0044D110, lara_default_col);
	INJECT(0x00445670, TestWall);
	INJECT(0x004438F0, LaraFloorFront);
	INJECT(0x00442DB0, LaraCeilingFront);
	INJECT(0x004495D0, lara_as_turn_r);
	INJECT(0x0044C1B0, lara_col_turn_r);
	INJECT(0x004496B0, lara_as_turn_l);
	INJECT(0x0044C2A0, lara_col_turn_l);
	INJECT(0x00449780, lara_as_death);
	INJECT(0x0044C2D0, lara_col_death);
	INJECT(0x004497F0, lara_as_fastfall);
	INJECT(0x0044C360, lara_col_fastfall);
	INJECT(0x00449120, LaraSlideEdgeJump);
	INJECT(0x00449860, lara_as_hang);
	INJECT(0x004498D0, lara_col_hang);
	INJECT(0x0044A780, CanLaraHangSideways);
	INJECT(0x00448CA0, lara_as_reach);
	INJECT(0x00448CD0, lara_col_reach);
	INJECT(0x0044ACA0, lara_as_splat);
	INJECT(0x0044C440, lara_col_splat);
	INJECT(0x0044C4D0, lara_col_land);
	INJECT(0x0044ACC0, lara_as_compress);
	INJECT(0x0044C500, lara_col_compress);
	INJECT(0x0044AF00, lara_as_fastturn);
	INJECT(0x0044AF70, lara_col_fastturn);
	INJECT(0x0044AFA0, lara_as_stepright);
	INJECT(0x0044C750, lara_col_stepright);
	INJECT(0x0044B060, lara_as_stepleft);
	INJECT(0x0044C870, lara_col_stepleft);
	INJECT(0x0044CCC0, lara_col_roll2);
	INJECT(0x0044B120, lara_as_slide);
	INJECT(0x0044C8A0, lara_col_slide);
	INJECT(0x0044C8D0, lara_slide_slope);
	INJECT(0x00448610, lara_as_upjump);
	INJECT(0x00448640, lara_col_upjump);
	INJECT(0x004467A0, MonkeySwingSnap);
	INJECT(0x0044A840, lara_as_hangleft);
	INJECT(0x0044A890, lara_col_hangleft);
	INJECT(0x0044A8E0, lara_as_hangright);
	INJECT(0x0044A930, lara_col_hangright);
	INJECT(0x0044B2B0, lara_as_slideback);
	INJECT(0x0044CBB0, lara_col_slideback);
	INJECT(0x0044B2E0, lara_as_pushblock);
	INJECT(0x0044B330, lara_as_pullblock);
	INJECT(0x0044B380, lara_as_ppready);
	INJECT(0x0044B580, lara_as_usepuzzle);
	INJECT(0x0044B530, lara_as_usekey);
	INJECT(0x0044B4E0, lara_as_switchoff);
	INJECT(0x0044B490, lara_as_switchon);
	INJECT(0x0044B410, lara_as_pickupflare);
	INJECT(0x0044B3C0, lara_as_pickup);
	INJECT(0x0044B620, lara_as_special);
	INJECT(0x0044B650, lara_as_swandive);
	INJECT(0x0044CDE0, lara_col_swandive);
	INJECT(0x0044B6A0, lara_as_fastdive);
	INJECT(0x0044CE80, lara_col_fastdive);
	INJECT(0x0044B720, lara_as_gymnast);
	INJECT(0x00443400, lara_as_duck);
	INJECT(0x00443560, lara_col_duck);
	INJECT(0x00445D70, MonkeySwingFall);
	INJECT(0x00445CE0, lara_as_hang2);
	INJECT(0x00445DF0, lara_col_hang2);
	INJECT(0x00446A70, lara_as_monkeyswing);
	INJECT(0x00446B50, lara_col_monkeyswing);
	INJECT(0x00446CD0, lara_as_monkeyl);
	INJECT(0x00446D50, lara_col_monkeyl);
	INJECT(0x00446DE0, lara_as_monkeyr);
	INJECT(0x00446E60, lara_col_monkeyr);
	INJECT(0x00447060, lara_as_monkey180);
	INJECT(0x00447090, lara_col_monkey180);
	INJECT(0x004436D0, lara_as_all4s);
	INJECT(0x00443990, lara_col_all4s);
	INJECT(0x00443F30, lara_as_crawl);
	INJECT(0x00444050, lara_col_crawl);
	INJECT(0x004441B0, LaraDeflectEdgeDuck);
	INJECT(0x00446EF0, lara_as_hangturnl);
	INJECT(0x00447010, lara_as_hangturnr);
	INJECT(0x00446F40, lara_col_hangturnlr);
	INJECT(0x00444250, lara_as_all4turnl);
	INJECT(0x00444340, lara_as_all4turnr);
	INJECT(0x004442D0, lara_col_all4turnlr);
}

