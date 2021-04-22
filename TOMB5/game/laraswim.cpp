#include "../tomb5/pch.h"
#include "laraswim.h"
#include "../global/types.h"
#include "effect2.h"
#include "lara_states.h"
#include "lara.h"
#include "control.h"
#include "effects.h"
#include "sound.h"
#include "collide.h"

void LaraTestWaterDepth(ITEM_INFO* item, COLL_INFO* coll)
{
	int wd;
	FLOOR_INFO* floor;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	wd = GetWaterDepth(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);

	if (wd == NO_HEIGHT)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->fallspeed = 0;
	}
	else if (wd <= 512)
	{
		item->anim_number = ANIMATION_LARA_UNDERWATER_TO_WADE;
		item->frame_number = anims[ANIMATION_LARA_UNDERWATER_TO_WADE].frame_base;
		item->current_anim_state = STATE_LARA_ONWATER_EXIT;
		item->goal_anim_state = STATE_LARA_STOP;
		item->pos.z_rot = 0;
		item->pos.x_rot = 0;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		lara.water_status = LW_WADE;
		item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	}
}

void LaraSwimCollision(ITEM_INFO* item, COLL_INFO* coll)
{
	int height;
	short oxr;
	short oyr;
	short hit;
	long ox;
	long oy;
	long oz;
	struct COLL_INFO coll2;
	struct COLL_INFO coll3;
	long pitch;

	hit = 0;
	ox = item->pos.x_pos;
	oy = item->pos.y_pos;
	oz = item->pos.z_pos;
	oxr = item->pos.x_rot;
	oyr = item->pos.y_rot;

	if (oxr < -16384 || oxr > 16384)
	{
		lara.move_angle = item->pos.y_rot - 32768;
		coll->facing = item->pos.y_rot - 32768;
	}
	else
	{
		lara.move_angle = item->pos.y_rot;
		coll->facing = item->pos.y_rot;
	}

	height = 762 * SIN(item->pos.x_rot) >> 14;

	if (height < ((LaraDrawType == LARA_DIVESUIT) * 64) + 200)
		height = ((LaraDrawType == LARA_DIVESUIT) * 64) + 200;

	coll->bad_neg = -64;
	memcpy((char*)&coll2, (char*)coll, sizeof(struct COLL_INFO));
	memcpy((char*)&coll3, (char*)coll, sizeof(struct COLL_INFO) - 2);
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos + (height / 2), item->pos.z_pos, item->room_number, height);
	coll2.facing += 8192;
	GetCollisionInfo(&coll2, item->pos.x_pos, item->pos.y_pos + (height / 2), item->pos.z_pos, item->room_number, height);
	coll3.facing -= 8192;
	GetCollisionInfo(&coll3, item->pos.x_pos, item->pos.y_pos + (height / 2), item->pos.z_pos, item->room_number, height);
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_FRONT:
		if (item->pos.x_rot <= 4550)
		{
			if (item->pos.x_rot >= -4550)
			{
				if (item->pos.x_rot > 910)
					item->pos.x_rot += 91;
				else if (item->pos.x_rot < -910)
					item->pos.x_rot -= 91;
				else if (item->pos.x_rot > 0)
					item->pos.x_rot += 45;
				else if (item->pos.x_rot < 0)
					item->pos.x_rot -= 45;
				else
				{
					hit = 1;
					item->fallspeed = 0;
				}
			}
			else
			{
				item->pos.x_rot -= 182;
				hit = 1;
			}
		}
		else
		{
			item->pos.x_rot += 182;
			hit = 1;
		}

		if (coll2.coll_type == CT_LEFT)
			item->pos.y_rot += 364;
		else if (coll2.coll_type == CT_RIGHT)
			item->pos.y_rot -= 364;
		else if (coll3.coll_type == CT_LEFT)
			item->pos.y_rot += 364;
		else if (coll3.coll_type == CT_RIGHT)
			item->pos.y_rot -= 364;
		break;

	case CT_TOP:
		if (item->pos.x_rot >= -8190)
		{
			hit = 1;
			item->pos.x_rot -= 182;
		}
		break;

	case CT_TOP_FRONT:
		item->fallspeed = 0;
		hit = 1;
		break;

	case CT_LEFT:
		item->pos.y_rot += 364;
		hit = 1;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 364;
		hit = 1;
		break;

	case CT_CLAMP:
		hit = 2;
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->fallspeed = 0;
		break;
	}

	if (coll->mid_floor < 0 && coll->mid_floor != NO_HEIGHT)
	{
		hit = 1;
		item->pos.x_rot += 182;
		item->pos.y_pos += coll->mid_floor;
	}

	if (ox != item->pos.x_pos &&
		oy != item->pos.y_pos &&
		oz != item->pos.z_pos &&
		oxr != item->pos.x_rot &&
		oyr != item->pos.y_rot ||
		SubHitCount ||
		hit != 1)
	{
		if (hit == 2)
			return;
	}
	else if (item->fallspeed > 100)
	{
		if (LaraDrawType == LARA_DIVESUIT)
		{
			pitch = ((2 * GetRandomControl() + 0x8000) << 8) | SFX_ALWAYS | SFX_SETPITCH;
			SoundEffect(SFX_SWIMSUIT_METAL_CLASH, &lara_item->pos, pitch);
		}

		SubHitCount = 30;

		if (lara.Anxiety < 96)
			lara.Anxiety += 16;
	}

	if (lara.water_status != LW_FLYCHEAT)
		LaraTestWaterDepth(item, coll);
}

void SwimTurn(ITEM_INFO* item)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 364;
	else if (input & IN_BACK)
		item->pos.x_rot += 364;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;
		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;
		item->pos.z_rot -= 546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;
		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;
		item->pos.z_rot += 546;
	}
}

void SwimTurnSubsuit(ITEM_INFO* item)
{
	if (item->pos.y_pos < 14080)
		subsuit.YVel += (14080 - item->pos.y_pos) >> 4;

	if (input & IN_FORWARD && item->pos.x_rot > -15470)
		subsuit.dXRot = -8190;
	else if (input & IN_BACK && item->pos.x_rot < 15470)
		subsuit.dXRot = 8190;
	else
		subsuit.dXRot = 0;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 136;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;

		item->pos.z_rot -= 546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 136;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;

		item->pos.z_rot += 546;
	}
}

void UpdateSubsuitAngles()
{
	short vel;

	if (subsuit.YVel)
	{
		lara_item->pos.y_pos += subsuit.YVel >> 2;
		subsuit.YVel += -1 - (subsuit.YVel >> 4);
	}

	vel = ABS(4 * lara_item->fallspeed);
	subsuit.Vel[0] = vel;
	subsuit.Vel[1] = vel;

	if (subsuit.XRot >= subsuit.dXRot)
	{
		if (subsuit.XRot > subsuit.dXRot)
		{
			if (subsuit.XRot > 0 && subsuit.dXRot < 0)
				subsuit.XRot -= (subsuit.XRot >> 2);

			subsuit.XRot -= 364;

			if (subsuit.XRot < subsuit.dXRot)
				subsuit.XRot = subsuit.dXRot;
		}
	}
	else
	{
		if (subsuit.XRot < 0 && subsuit.dXRot > 0)
			subsuit.XRot -= (subsuit.XRot >> 2);

		subsuit.XRot += 364;

		if (subsuit.XRot > subsuit.dXRot)
			subsuit.XRot = subsuit.dXRot;
	}

	if (subsuit.dXRot)
	{
		if (subsuit.XRot > 364)
			lara_item->pos.x_rot += 364;
		else if (subsuit.XRot < 364)
			lara_item->pos.x_rot -= 364;
	}

	vel = ABS(subsuit.XRot >> 3);
	subsuit.Vel[0] += vel;
	subsuit.Vel[1] += vel;

	if (lara.turn_rate > 0)
		subsuit.Vel[0] += 2 * ABS(lara.turn_rate);
	else if (lara.turn_rate < 0)
		subsuit.Vel[1] += 2 * ABS(lara.turn_rate);

	if (subsuit.Vel[0] > 1536)
		subsuit.Vel[0] = 1536;

	if (subsuit.Vel[1] > 1536)
		subsuit.Vel[1] = 1536;

	if (subsuit.Vel[0] || subsuit.Vel[1])
	{
		short vol = (subsuit.Vel[0] + subsuit.Vel[1]) >> 6;

		if (vol > 31)
			vol = 31;

		SoundEffect(SFX_LARA_UNDERWATER_ENGINE, &lara_item->pos, (vol << 8) | SFX_SETVOL | SFX_ALWAYS);
	}

}

void lara_as_swimcheat(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 546;
	else if (input & IN_BACK)
		item->pos.x_rot += 546;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 613;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 613;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;
	}

	if (input & IN_ACTION)
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 31, 255, 255, 255);

	if (input & IN_ROLL)
		lara.turn_rate = -2184;

	if (input & IN_JUMP)
	{
		item->fallspeed += 16;

		if (item->fallspeed > 400)
			item->fallspeed = 400;
	}
	else
	{
		if (item->fallspeed >= 8)
			item->fallspeed -= item->fallspeed >> 3;
		else
			item->fallspeed = 0;
	}
}

void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		if (LaraDrawType != LARA_DIVESUIT)
		{
			item->current_anim_state = STATE_LARA_UNDERWATER_TURNAROUND;
			item->anim_number = ANIMATION_LARA_UNDERWATER_ROLL_BEGIN;
			item->frame_number = anims[ANIMATION_LARA_UNDERWATER_ROLL_BEGIN].frame_base;
			return;
		}
	}
	else if (LaraDrawType != LARA_DIVESUIT)
		SwimTurn(item);
	else
		SwimTurnSubsuit(item);

	item->fallspeed += 8;

	if (item->fallspeed > 200)
		item->fallspeed = 200;

	if (!(input & IN_JUMP))
		item->goal_anim_state = STATE_LARA_UNDERWATER_INERTIA;
}

void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		if (LaraDrawType != LARA_DIVESUIT)
		{
			item->current_anim_state = STATE_LARA_UNDERWATER_TURNAROUND;
			item->anim_number = ANIMATION_LARA_UNDERWATER_ROLL_BEGIN;
			item->frame_number = anims[ANIMATION_LARA_UNDERWATER_ROLL_BEGIN].frame_base;
			return;
		}
	}
	else if (LaraDrawType != LARA_DIVESUIT)
		SwimTurn(item);
	else
		SwimTurnSubsuit(item);

	if (input & IN_JUMP)
		item->goal_anim_state = STATE_LARA_UNDERWATER_FORWARD;

	item->fallspeed -= 6;

	if (item->fallspeed < 0)
		item->fallspeed = 0;

	if (item->fallspeed <= 133)
		item->goal_anim_state = STATE_LARA_UNDERWATER_STOP;
}

void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = STATE_LARA_WATER_DEATH;
		return;
	}

	if (input & IN_ROLL && LaraDrawType != LARA_DIVESUIT)
	{
		item->current_anim_state = STATE_LARA_UNDERWATER_TURNAROUND;
		item->anim_number = ANIMATION_LARA_UNDERWATER_ROLL_BEGIN;
		item->frame_number = anims[ANIMATION_LARA_UNDERWATER_ROLL_BEGIN].frame_base;
	}
	else
	{
		if (input & IN_LOOK)
			LookUpDown();

		if (LaraDrawType == LARA_DIVESUIT)
			SwimTurnSubsuit(item);
		else
			SwimTurn(item);

		if (input & IN_JUMP)
			item->goal_anim_state = STATE_LARA_UNDERWATER_FORWARD;

		item->fallspeed -= 6;

		if (item->fallspeed < 0)
			item->fallspeed = 0;

		if (lara.gun_status == LG_HANDS_BUSY)
			lara.gun_status = LG_NO_ARMS;
	}
}

void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 182;
}

void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	item->fallspeed -= 8;

	if (item->fallspeed <= 0)
		item->fallspeed = 0;

	if ((item->pos.x_rot < -364) || (item->pos.x_rot > 364))
	{
		if (item->pos.x_rot >= 0)
			item->pos.x_rot -= 364;
		else
			item->pos.x_rot += 364;
	}
	else
		item->pos.x_rot = 0;
}

void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
}

void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	int wh;

	item->hit_points = -1;
	lara.air = -1;
	lara.gun_status = LG_HANDS_BUSY;
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (wh != NO_HEIGHT)
	{
		if (wh < item->pos.y_pos - 100)
			item->pos.y_pos -= 5;
	}

	LaraSwimCollision(item, coll);
}

void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void inject_laraswim()
{
	INJECT(0x00459470, LaraTestWaterDepth);
	INJECT(0x00458FF0, LaraSwimCollision);
	INJECT(0x00458BC0, SwimTurn);
	INJECT(0x00458C80, SwimTurnSubsuit);
	INJECT(0x004584C0, UpdateSubsuitAngles);
	//LaraUnderWater
	INJECT(0x004589F0, lara_as_swimcheat);
	INJECT(0x00458B20, lara_as_swim);
	INJECT(0x00458D60, lara_as_glide);
	INJECT(0x00458E20, lara_as_tread);
	INJECT(0x00458EF0, lara_as_dive);
	INJECT(0x00458F20, lara_as_uwdeath);
	INJECT(0x00458FA0, lara_as_waterroll);
	INJECT(0x00458FC0, lara_col_swim);
	INJECT(0x00459590, lara_col_glide);
	INJECT(0x004595C0, lara_col_tread);
	INJECT(0x004595F0, lara_col_dive);
	INJECT(0x00459620, lara_col_uwdeath);
	INJECT(0x004596A0, lara_col_waterroll);
	//GetWaterDepth
	//LaraWaterCurrent
}
