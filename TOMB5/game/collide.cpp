#pragma once
#include "../tomb5/pch.h"
#include "collide.h"
#include "control.h"
#include "../global/types.h"
#include "tomb4fx.h"
#include "../specific/calclara.h"
//0x004134E0 Move3DPosTo3DPos
void TriggerLaraBlood()
{
	PHD_VECTOR vec;
	int node;

	node = 1;

	for (int i = 0; i < 15; i++)
	{
		if (node & lara_item->touch_bits)
		{
			vec.x = (GetRandomControl() & 0x1F) - 16;
			vec.y = (GetRandomControl() & 0x1F) - 16;
			vec.z = (GetRandomControl() & 0x1F) - 16;
			GetLaraJointPos(&vec, LM[i]);
			DoBloodSplat(vec.x, vec.y, vec.z, (GetRandomControl() & 7) + 8, 2 * GetRandomControl(), lara_item->room_number);
		}

		node <<= 1;
	}
}

void GetCollisionInfo(COLL_INFO* coll, long xpos, long ypos, long zpos, short room_number, long objheight)
{
	bool reset_room;
	int y, yT;
	short fspeed, room_num, room_num2;
	int xright, xleft, zright, zleft;
	int x, z;
	long height;
	short ceiling;
	unsigned short tilt_type;

	if (objheight < 0)
	{
		objheight = -objheight;
		reset_room = true;
	}
	else
		reset_room = false;

	y = ypos - objheight;
	yT = y - 160;
	fspeed = yT - lara_item->fallspeed;

	coll->coll_type = CT_NONE;
	coll->shift.x = 0;
	coll->shift.y = 0;
	coll->shift.z = 0;
	coll->quadrant = (unsigned short)(coll->facing + 8192) / 16384;

	room_num = room_number;
	FLOOR_INFO* floor = GetFloor(xpos, yT, zpos, &room_num);
	height = GetHeight(floor, xpos, yT, zpos);

	if (height != NO_HEIGHT)
		height -= ypos;

	
	ceiling = GetCeiling(floor, xpos, fspeed, zpos);
	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->mid_floor = height;
	coll->mid_ceiling = ceiling;
	coll->mid_type = height_type;
	coll->trigger = trigger_index;

	tilt_type = GetTiltType(floor, xpos, lara_item->pos.y_pos, zpos);
	coll->tilt_x = (char)tilt_type;
	coll->tilt_z = tilt_type >> 8;

	

	switch (coll->quadrant)
	{
	case NORTH:
		xfront = (SIN((unsigned short)(coll->facing) * coll->radius)) >> 14;
		zfront = coll->radius;
		xright = coll->radius; 
		zright = coll->radius; 
		zleft = coll->radius;
		xleft = -coll->radius;
		break;
	case EAST:
		xfront = coll->radius;
		zfront = (COS((unsigned short)(coll->facing) * coll->radius)) >> 14;
		xright = coll->radius;
		zright = -coll->radius;
		zleft = coll->radius;
		xleft = coll->radius;
		break;
	case SOUTH:
		xfront = (SIN((unsigned short)coll->facing) * coll->radius) >> 14;
		zfront = -coll->radius;
		xright = -coll->radius;
		zright = -coll->radius;
		zleft = -coll->radius;
		xleft = coll->radius;
		break;
	case WEST:
		xfront = -coll->radius;
		zfront = (COS((unsigned short)coll->facing) * coll->radius) >> 14;
		xright = -coll->radius;
		zright = coll->radius;
		zleft = -coll->radius;
		xleft = -coll->radius;
		break;
	default:
		xfront = zfront = 0;
		xright = zright = 0;
		zleft = xleft = 0;
	}

	if (reset_room)
		room_num = room_number;

	x = xfront + xpos;
	z = zfront + zpos;

	floor = GetFloor(x, yT, z, &room_num);
	height = GetHeight(floor, x, yT, z);

	if (height != NO_HEIGHT)
		height -= ypos;

	ceiling = GetCeiling(floor, x, fspeed, z);

	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->front_ceiling = ceiling;
	coll->front_floor = height;
	coll->front_type = height_type;

	floor = GetFloor(x + xfront, yT, z + zfront, &room_num);
	height = GetHeight(floor, x + xfront, yT, z + zfront);

	if (height != NO_HEIGHT)
		height -= ypos;


	if (coll->slopes_are_walls &&
		((coll->front_type == BIG_SLOPE) || (coll->front_type == DIAGONAL)) &&
		coll->front_floor < coll->mid_floor &&
		height < coll->front_floor &&
		coll->front_floor < 0)
		coll->front_floor = -32767;
	else if (coll->slopes_are_pits && 
		((coll->front_type == BIG_SLOPE) || (coll->front_type == DIAGONAL)) &&
		coll->mid_floor < coll->front_floor)
		coll->front_floor = 512;
	else if (coll->lava_is_pit && coll->front_floor > 0 && trigger_index != NULL)
	{
		if ((trigger_index[0] & 0x1F) == LAVA_TYPE)
			coll->front_floor = 512;
	}

	room_num2 = room_number;
	x = xpos + xleft;
	z = zpos + zleft;

	floor = GetFloor(x, yT, z, &room_num2);
	height = GetHeight(floor, x, yT, z);
	if (height != NO_HEIGHT)
		height -= ypos;

	ceiling = GetCeiling(floor, x, fspeed, z);
	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->left_floor = height;
	coll->left_ceiling = ceiling;
	coll->left_type = height_type;

	if (coll->slopes_are_walls == 1 && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor < 0)
		coll->left_floor = -32767;
	else if (coll->slopes_are_pits && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor > 0)
		coll->left_floor = 512;
	else if (coll->lava_is_pit && coll->left_floor > 0 && trigger_index != NULL && ( (*(trigger_index) & 0x1F) == LAVA_TYPE) )
		coll->left_floor = 512;

	x = xpos + xleft;
	z = zpos + zleft;
	floor = GetFloor(x, yT, z, &room_num);
	height = GetHeight(floor, x, yT, z);
	if (height != NO_HEIGHT)
		height -= ypos;

	ceiling = GetCeiling(floor, x, fspeed, z);
	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->left_floor2 = height;
	coll->left_ceiling2 = ceiling;
	coll->left_type2 = height_type;

	if (coll->slopes_are_walls == 1 && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 < 0)
		coll->left_floor2 = -32767;
	else if (coll->slopes_are_pits && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 > 0)
		coll->left_floor2 = 512;
	else if (coll->lava_is_pit && coll->left_floor2 > 0 && trigger_index != NULL && ((*(trigger_index) & 0x1F) == LAVA_TYPE))
		coll->left_floor2 = 512;


	x = xpos + xright;
	z = zpos + zright;
	room_num2 = room_number;

	floor = GetFloor(x, yT, z, &room_num2);
	height = GetHeight(floor, x, yT, z);
	if (height != NO_HEIGHT)
		height -= ypos;

	ceiling = GetCeiling(floor, x, fspeed, z);
	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->right_floor = height;
	coll->right_ceiling = ceiling;
	coll->right_type = height_type;

	if (coll->slopes_are_walls == 1 && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor < 0)
		coll->right_floor = -32767;
	else if (coll->slopes_are_pits && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor > 0)
		coll->right_floor = 512;
	else if (coll->lava_is_pit && coll->right_floor > 0 && trigger_index != NULL && ((*(trigger_index) & 0x1F) == LAVA_TYPE))
		coll->right_floor = 512;


	x = xpos + xright;
	z = zpos + zright;

	floor = GetFloor(x, yT, z, &room_num);
	height = GetHeight(floor, x, yT, z);
	if (height != NO_HEIGHT)
		height -= ypos;

	ceiling = GetCeiling(floor, x, fspeed, z);
	if (ceiling != NO_HEIGHT)
		ceiling -= y;

	coll->right_floor2 = height;
	coll->right_ceiling2 = ceiling;
	coll->right_type2 = height_type;

	if (coll->slopes_are_walls == 1 && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 < 0)
		coll->right_floor2 = -32767;
	else if (coll->slopes_are_pits && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 > 0)
		coll->right_floor2 = 512;
	else if (coll->lava_is_pit && coll->right_floor2 > 0 && trigger_index != NULL && ((*(trigger_index) & 0x1F) == LAVA_TYPE))
		coll->right_floor2 = 512;

	CollideStaticObjects(coll, xpos, ypos, zpos, room_number, objheight);

	if (coll->mid_floor == NO_HEIGHT)
	{
		coll->coll_type = CT_FRONT;
		coll->shift.x = coll->old.x - xpos;
		coll->shift.y = coll->old.y - ypos;
		coll->shift.z = coll->old.z - zpos;
		return;
	}
	if ((coll->mid_floor == coll->mid_ceiling) || (coll->mid_floor - coll->mid_ceiling < 0))
	{
		coll->coll_type = CT_CLAMP;
		coll->shift.x = coll->old.x - xpos;
		coll->shift.y = coll->old.y - ypos;
		coll->shift.z = coll->old.z - zpos;
		return;
	}

	if (-1 < coll->mid_ceiling)
	{
		coll->shift.y = coll->mid_ceiling;
		coll->coll_type = CT_TOP;
		coll->hit_ceiling = true;
	}

	if (coll->bad_pos < coll->front_floor || coll->front_floor < coll->bad_neg ||
		coll->bad_ceiling < coll->front_ceiling)
	{
		if (coll->front_type == DIAGONAL || coll->coll_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - xpos;
			coll->shift.z = coll->old.z - zpos;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = coll->old.x - xpos;
				coll->shift.z = FindGridShift(zpos + zfront, zpos);
				break;
			case EAST:
			case WEST:
				coll->shift.x = FindGridShift(xpos + xfront, xpos);
				coll->shift.z = coll->old.z - zpos;
				break;
			}
		}
		coll->coll_type = CT_FRONT;
		return;
	}

	if (coll->front_ceiling >= coll->bad_ceiling)
	{
		coll->coll_type = CT_TOP_FRONT;
		coll->shift.x = coll->old.x - xpos;
		coll->shift.y = coll->old.y - ypos;
		coll->shift.z = coll->old.z - zpos;
		return;
	}

	if (coll->bad_pos < coll->left_floor || coll->left_floor < coll->bad_neg ||
		coll->bad_ceiling < coll->left_ceiling)
	{
		if (coll->left_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - xpos;
			coll->shift.z = coll->old.z - zpos;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = FindGridShift(xpos + xleft, xpos + xfront);
				break;
			case EAST:
			case WEST:
				coll->shift.z = FindGridShift(zpos + zleft, zpos + zfront);
				break;
			}
		}
		coll->coll_type = CT_LEFT;
		return;
	}

	if (coll->bad_pos < coll->right_floor || coll->right_floor < coll->bad_neg ||
		coll->bad_ceiling < coll->right_ceiling)
	{
		if (coll->right_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - xpos;
			coll->shift.z = coll->old.z - zpos;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = FindGridShift(xpos + xright, xpos + xfront);
				break;
			case EAST:
			case WEST:
				coll->shift.z = FindGridShift(zpos + zright, zpos + zfront);
				break;
			}
		}
		coll->coll_type = CT_RIGHT;
		return;
	}
}

int FindGridShift(int src, int dst)
{
	int srcw, dstw;

	srcw = src >> 10;
	dstw = dst >> 10;

	if (srcw == dstw)
		return (0);

	src &= 1024 - 1;

	if (dstw > srcw)
		return (1024 - (src - 1));
	else
		return (-(src + 1));
}

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x_pos += coll->shift.x;
	item->pos.y_pos += coll->shift.y;
	item->pos.z_pos += coll->shift.z;
	coll->shift.x = coll->shift.y = coll->shift.z = 0;
}

short GetTiltType(FLOOR_INFO* floor, long x, long y, long z)
{
	room_info* Room;
	short* floordata;
	short type;
	short t0, t1, t2, t3;
	short tilt, x2, z2, x3, y2;

	while (floor->pit_room != 255)
	{
		if (CheckNoColFloorTriangle(floor, x, z))
			break;

		Room = &room[floor->pit_room];
		floor = &Room->floor[((z - Room->z) >> 10) + (((x - Room->x) >> 10) * Room->x_size)];
	}

	if (y + 512 < ((long)floor->floor << 8))
		return 0;

	if (floor->index != 0)
	{
		floordata = &floor_data[floor->index];
		type = (floordata[0] & 0x1F);

		if (type == TILT_TYPE)
			return floordata[1];

		if (((((type == SPLIT1) || (type == SPLIT2)) || (type == NOCOLF1T)) || ((type == NOCOLF2T || (type == NOCOLF1B)))) || (type == NOCOLF2B))
		{
			tilt = floordata[1];
			t0 = tilt & 0xF;
			t1 = (tilt >> 4) & 0xF;
			t2 = (tilt >> 8) & 0xF;
			t3 = (tilt >> 12) & 0xF;
			x2 = x & 0x3FF;
			z2 = z & 0x3FF;

			if (((type & 0x1F) != SPLIT1) && ((type & 0x1F) != NOCOLF1T) && ((type & 0x1F) != NOCOLF1B))
			{
				if (x2 > z2)
				{
					x3 = t3 - t0;
					y2 = t0 - t1;
				}
				else
				{
					x3 = t2 - t1;
					y2 = t3 - t2;
				}
			}
			else
			{
				if (x2 > (0x400 - z2))
				{
					x3 = t3 - t0;
					y2 = t3 - t2;
				}
				else
				{
					x3 = t2 - t1;
					y2 = t0 - t1;
				}
			}

			return ((x3 << 8) | (y2 & 0xFF));
		}
	}

	return 0;
}

void inject_coll()
{
	INJECT(0x00414370, TriggerLaraBlood);
	INJECT(0x00411100, GetCollisionInfo);
	INJECT(0x00411D70, FindGridShift);
	INJECT(0x004120A0, ShiftItem);
	INJECT(0x00410EF0, GetTiltType);
}
