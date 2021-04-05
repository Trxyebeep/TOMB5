#pragma once
#include "../tomb5/pch.h"
#include "../global/types.h"
#include "laraclmb.h"
#include "lara.h"
#include "lara_states.h"
#include "control.h"
#include "laramisc.h"

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.IsClimbing = 1;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_elevation = -3640;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_LEFT || input & IN_LSTEP)
	{
		item->goal_anim_state = STATE_LARA_LADDER_LEFT;
		lara.move_angle = item->pos.y_rot - 16384;
	}
	else if (input & IN_RIGHT || input & IN_RSTEP)
	{
		item->goal_anim_state = STATE_LARA_LADDER_RIGHT;
		lara.move_angle = item->pos.y_rot + 16384;
	}
	else if (input & IN_JUMP)
	{
		if (item->anim_number == ANIMATION_LARA_LADDER_IDLE)
		{
			item->goal_anim_state = STATE_LARA_JUMP_BACK;
			lara.gun_status = LG_NO_ARMS;
			lara.move_angle = item->pos.y_rot + 32768;
		}
	}
}

void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	int result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->anim_number != ANIMATION_LARA_LADDER_IDLE)
		return;
	
	if (input & IN_FORWARD)
	{
		if (item->goal_anim_state == STATE_LARA_GRABBING)
			return;

		item->goal_anim_state = STATE_LARA_LADDER_IDLE;
		result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
		result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + 120), &shift_l, &ledge_l);

		if (!result_r || !result_l)
			return;

		if (result_r < 0 || result_l < 0)
		{
			if (ABS(ledge_l - ledge_r) > 120)
				return;

			if (result_r == -1 && result_l == -1)
			{
				item->goal_anim_state = STATE_LARA_GRABBING;
				item->pos.y_pos += (ledge_l + ledge_r) / 2 - 256;
			}
			else
			{
				item->goal_anim_state = STATE_LARA_UNKNOWN_138;
				item->required_anim_state = STATE_LARA_CROUCH_IDLE;
			}

			return;
		}

		if (shift_r)
		{
			if (shift_l)
			{

				if ((shift_r < 0) ^ (shift_l < 0))
					return;

				if (shift_r < 0 && shift_r < shift_l)
					shift_l = shift_r;
				else if (shift_r > 0 && shift_r > shift_l)
					shift_l = shift_r;
			}
			else
				shift_l = shift_r;
		}

		item->goal_anim_state = STATE_LARA_LADDER_UP;
		item->pos.y_pos += shift_l;
		return;
	}
	else if (input & IN_BACK)
	{
		if (item->goal_anim_state == STATE_LARA_HANG)
			return;

		item->goal_anim_state = STATE_LARA_LADDER_IDLE;
		item->pos.y_pos += 256;
		result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
		result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift_l);
		item->pos.y_pos -= 256;

		if (result_r == 0 || result_l == 0 || result_l == -2 || result_r == -2)
			return;

		if (shift_r && shift_l)
		{
			if ((shift_r < 0) ^ (shift_l < 0))
				return;
			if (shift_r < 0 && shift_r < shift_l)
				shift_l = shift_r;
			else if (shift_r > 0 && shift_r > shift_l)
				shift_l = shift_r;
		}

		if (result_r != 1 || result_l != 1)
			item->goal_anim_state = STATE_LARA_HANG;
		else
		{
			item->goal_anim_state = STATE_LARA_LADDER_DOWN;
			item->pos.y_pos += shift_l;
		}
	}

	return;
}

static long LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll)
{
	short room_number;

	item->gravity_status = 0;
	item->fallspeed = 0;
	room_number = item->room_number;
	GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
		item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	coll->trigger = trigger_index;

	if (input & IN_ACTION && item->hit_points > 0)
		return 0;

	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.head_y_rot = 0;
	lara.head_x_rot = 0;
	item->goal_anim_state = STATE_LARA_JUMP_FORWARD;
	item->current_anim_state = STATE_LARA_JUMP_FORWARD;
	item->anim_number = ANIMATION_LARA_FREE_FALL_FORWARD;
	item->frame_number = anims[ANIMATION_LARA_FREE_FALL_FORWARD].frame_base;
	item->speed = 2;
	item->gravity_status = 1;
	item->fallspeed = 1;
	lara.gun_status = LG_NO_ARMS;
	return 1;
}

void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_elevation = 5460;
}

void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	int frame;
	int yshift;
	int result_r, result_l;
	int shift_r, shift_l;
	int ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->anim_number == ANIMATION_LARA_LADDER_UP)
	{
		frame = item->frame_number - anims[ANIMATION_LARA_LADDER_UP].frame_base;
		
		if (frame == 0)
			yshift = 0;
		else if (frame == 28 || frame == 29)
			yshift = -256;
		else if (frame == 57)
			yshift = -512;
		else
			return;

		item->pos.y_pos += yshift - 256;
		result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
		result_l = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_l, &ledge_l);
		item->pos.y_pos += 256;

		if (result_r && result_l && input & IN_FORWARD)
		{
			if (result_r < 0 || result_l < 0)
			{
				item->goal_anim_state = STATE_LARA_LADDER_IDLE;
				AnimateLara(item);

				if (ABS(ledge_r - ledge_l) <= 120)
				{
					if (result_r != -1 || result_l != -1)
					{
						item->goal_anim_state = STATE_LARA_UNKNOWN_138;
						item->required_anim_state = STATE_LARA_CROUCH_IDLE;
					}
					else
					{
						item->goal_anim_state = STATE_LARA_GRABBING;
						item->pos.y_pos += (ledge_r + ledge_l) / 2 - 256;
					}
				}
			}
			else
			{
				item->goal_anim_state = STATE_LARA_LADDER_UP;
				item->pos.y_pos -= yshift;
			}
		}
		else
		{
			item->goal_anim_state = STATE_LARA_LADDER_IDLE;

			if (yshift != 0)
				AnimateLara(item);
		}
	}
}

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll)//
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -5460;
	camera.target_elevation = -2730;

	if (!(input & (IN_LEFT | IN_LSTEP)))
		item->goal_anim_state = STATE_LARA_LADDER_IDLE;
}

void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	int shift, res;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot - 16384;
		res = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, res, shift);
	}
}

void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 5460;
	camera.target_elevation = -2730;

	if (!(input & (IN_RIGHT | IN_RSTEP)))
		item->goal_anim_state = STATE_LARA_LADDER_IDLE;
}

void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	int shift, res;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot + 16384;
		res = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, res, shift);
	}
}

void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = -8190;
}

void lara_col_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	return;
}

void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_elevation = -8190;
}

void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	int frame;
	int yshift;
	int result_r, result_l;
	int shift_r, shift_l;

	if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIMATION_LARA_LADDER_DOWN)
		return;

	frame = item->frame_number - anims[ANIMATION_LARA_LADDER_DOWN].frame_base;

	if (frame == 0)
		yshift = 0;
	else if (frame == 28 || frame == 29)
		yshift = 256;
	else if (frame == 57)
		yshift = 512;
	else
		return;

	item->pos.y_pos += yshift + 256;
	result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
	result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift_l);
	item->pos.y_pos -= 256;

	if (result_r != 0 && result_l != 0 &&
		result_r != -2 && result_l != -2 &&
		input & IN_BACK)
	{
		if (shift_r && shift_l)
		{
			if (shift_r < 0 != shift_l < 0)
			{
				item->goal_anim_state = STATE_LARA_LADDER_IDLE;
				AnimateLara(item);
				return;
			}

			if (shift_r < 0 && shift_r < shift_l ||
				shift_r > 0 && shift_r > shift_l)
				shift_l = shift_r;
		}

		if (result_r == -1 || result_l == -1)
		{
			item->anim_number = ANIMATION_LARA_LADDER_IDLE;
			item->frame_number = anims[ANIMATION_LARA_LADDER_IDLE].frame_base;
			item->current_anim_state = STATE_LARA_LADDER_IDLE;
			item->goal_anim_state = STATE_LARA_HANG;
			AnimateLara(item);
		}
		else
		{
			item->goal_anim_state = STATE_LARA_LADDER_DOWN;
			item->pos.y_pos -= yshift;
		}

		return;
	}

	item->goal_anim_state = STATE_LARA_LADDER_IDLE;

	if (yshift)
		AnimateLara(item);
}

void inject_laraclmb()
{
	INJECT(0x00450D40, lara_as_climbstnc);
	INJECT(0x00450E20, lara_col_climbstnc);
	INJECT(0x004510E0, LaraCheckForLetGo);
	INJECT(0x00451ED0, lara_as_climbing);
	INJECT(0x00451F00, lara_col_climbing);
	INJECT(0x004514D0, lara_as_climbleft);
	INJECT(0x00451520, lara_col_climbleft);
	INJECT(0x00451E00, lara_as_climbright);
	INJECT(0x00451E50, lara_col_climbright);
	INJECT(0x00452340, lara_as_climbend);
	INJECT(0x00452380, lara_col_climbend);
	INJECT(0x004520E0, lara_as_climbdown);
	INJECT(0x00452110, lara_col_climbdown);
}
