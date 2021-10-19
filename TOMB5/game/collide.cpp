#include "../tomb5/pch.h"
#include "collide.h"
#include "control.h"
#include "delstuff.h"
#include "effects.h"
#include "sphere.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "draw.h"
#include "../specific/3dmath.h"

void TriggerLaraBlood()
{
	PHD_VECTOR vec;

	for (int i = 0, node = 1; i < 15; i++, node <<= 1)
	{
		if (node & lara_item->touch_bits)
		{
			vec.x = (GetRandomControl() & 0x1F) - 16;
			vec.y = (GetRandomControl() & 0x1F) - 16;
			vec.z = (GetRandomControl() & 0x1F) - 16;
			GetLaraJointPos(&vec, LM[i]);
			DoBloodSplat(vec.x, vec.y, vec.z, (GetRandomControl() & 7) + 8, GetRandomControl() << 1, lara_item->room_number);
		}
	}
}

void GetCollisionInfo(COLL_INFO* coll, long xpos, long ypos, long zpos, short room_number, long objheight)
{
	FLOOR_INFO* floor;
	long x, y, yT, z, xright, xleft, zright, zleft, height, ceiling, reset_room;
	ushort tilt_type;
	short fspeed, room_num, room_num2;

	if (objheight < 0)
	{
		objheight = -objheight;
		reset_room = 1;
	}
	else
		reset_room = 0;

	y = ypos - objheight;
	yT = y - 160;
	fspeed = (short)(yT - lara_item->fallspeed);
	coll->coll_type = CT_NONE;
	coll->shift.x = 0;
	coll->shift.y = 0;
	coll->shift.z = 0;
	coll->quadrant = (ushort)(coll->facing + 8192) / 16384;
	room_num = room_number;
	floor = GetFloor(xpos, yT, zpos, &room_num);
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
		xfront = (phd_sin(coll->facing) * coll->radius) >> 14;
		zfront = coll->radius;
		xright = coll->radius; 
		zright = coll->radius; 
		zleft = coll->radius;
		xleft = -coll->radius;
		break;

	case EAST:
		xfront = coll->radius;
		zfront = (phd_cos(coll->facing) * coll->radius) >> 14;
		xright = coll->radius;
		zright = -coll->radius;
		zleft = coll->radius;
		xleft = coll->radius;
		break;

	case SOUTH:
		xfront = (phd_sin(coll->facing) * coll->radius) >> 14;
		zfront = -coll->radius;
		xright = -coll->radius;
		zright = -coll->radius;
		zleft = -coll->radius;
		xleft = coll->radius;
		break;

	case WEST:
		xfront = -coll->radius;
		zfront = (phd_cos(coll->facing) * coll->radius) >> 14;
		xright = -coll->radius;
		zright = coll->radius;
		zleft = -coll->radius;
		xleft = -coll->radius;
		break;

	default:
		xfront = 0;
		zfront = 0;
		xright = 0;
		zright = 0;
		zleft = 0;
		xleft = 0;
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

	if (coll->slopes_are_walls && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) &&
		coll->front_floor < coll->mid_floor && height < coll->front_floor && coll->front_floor < 0)
		coll->front_floor = -32767;
	else if (coll->slopes_are_pits && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) && coll->mid_floor < coll->front_floor)
		coll->front_floor = 512;
	else if (coll->lava_is_pit && coll->front_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->front_floor = 512;

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
	else if (coll->lava_is_pit && coll->left_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
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
	else if (coll->lava_is_pit && coll->left_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
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
	else if (coll->lava_is_pit && coll->right_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
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
	else if (coll->lava_is_pit && coll->right_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
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

	if (coll->mid_floor == coll->mid_ceiling || coll->mid_floor - coll->mid_ceiling < 0)
	{
		coll->coll_type = CT_CLAMP;
		coll->shift.x = coll->old.x - xpos;
		coll->shift.y = coll->old.y - ypos;
		coll->shift.z = coll->old.z - zpos;
		return;
	}

	if (coll->mid_ceiling > -1)
	{
		coll->shift.y = coll->mid_ceiling;
		coll->coll_type = CT_TOP;
		coll->hit_ceiling = 1;
	}

	if (coll->bad_pos < coll->front_floor || coll->front_floor < coll->bad_neg || coll->bad_ceiling < coll->front_ceiling)
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

	if (coll->bad_pos < coll->left_floor || coll->left_floor < coll->bad_neg || coll->bad_ceiling < coll->left_ceiling)
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

	if (coll->bad_pos < coll->right_floor || coll->right_floor < coll->bad_neg || coll->bad_ceiling < coll->right_ceiling)
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
		return 0;

	src &= 1023;

	if (dstw > srcw)
		return 1025 - src;
	else
		return -1 - src;
}

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x_pos += coll->shift.x;
	item->pos.y_pos += coll->shift.y;
	item->pos.z_pos += coll->shift.z;
	coll->shift.x = 0;
	coll->shift.y = 0;
	coll->shift.z = 0;
}

short GetTiltType(FLOOR_INFO* floor, long x, long y, long z)
{
	ROOM_INFO* Room;
	short* floordata;
	short type, t0, t1, t2, t3, tilt, x2, z2, x3, y2;

	while (floor->pit_room != NO_ROOM)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		Room = &room[floor->pit_room];
		floor = &Room->floor[((z - Room->z) >> 10) + (((x - Room->x) >> 10) * Room->x_size)];
	}

	if (y + 512 < ((long)floor->floor << 8))
		return 0;

	if (floor->index)
	{
		floordata = &floor_data[floor->index];
		type = (floordata[0] & 0x1F);

		if (type == TILT_TYPE)
			return floordata[1];

		if ((((type == SPLIT1 || type == SPLIT2) || type == NOCOLF1T) || ((type == NOCOLF2T || type == NOCOLF1B))) || type == NOCOLF2B)
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

void GenericSphereBoxCollision(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;
	SPHERE* sptr;
	long TouchBits, DeadlyBits, dx, dy, dz;

	item = &items[item_num];

	if (item->status != ITEM_INVISIBLE && TestBoundsCollide(item, laraitem, coll->radius))
	{
		TouchBits = TestCollision(item, laraitem);

		if (TouchBits)
		{
			dy = item->pos.y_rot;
			item->pos.y_rot = 0;
			GetSpheres(item, Slist, 1);
			item->pos.y_rot = (short) dy;
			sptr = Slist;
			DeadlyBits = *(long*) &item->item_flags[0];

			do
			{
				if (TouchBits & 1)
				{
					GlobalCollisionBounds[0] = (short) (sptr->x - sptr->r - item->pos.x_pos);
					GlobalCollisionBounds[2] = (short) (sptr->y - sptr->r - item->pos.y_pos);
					GlobalCollisionBounds[4] = (short) (sptr->z - sptr->r - item->pos.z_pos);
					GlobalCollisionBounds[1] = (short) (sptr->x + sptr->r - item->pos.x_pos);
					GlobalCollisionBounds[3] = (short) (sptr->y + sptr->r - item->pos.y_pos);
					GlobalCollisionBounds[5] = (short) (sptr->z + sptr->r - item->pos.z_pos);
					dx = laraitem->pos.x_pos;
					dy = laraitem->pos.y_pos;
					dz = laraitem->pos.z_pos;

					if (ItemPushLara(item, laraitem, coll, coll->enable_spaz & TouchBits, 3) && DeadlyBits & 1)
					{
						laraitem->hit_points -= item->item_flags[3];
						dx -= laraitem->pos.x_pos;
						dy -= laraitem->pos.y_pos;
						dz -= laraitem->pos.z_pos;

						if (!coll->enable_baddie_push)
						{
							laraitem->pos.x_pos += dx;
							laraitem->pos.y_pos += dy;
							laraitem->pos.z_pos += dz;
						}

						if ((dx || dy || dz) && TriggerActive(item))
							TriggerLaraBlood();
					}
				}

				TouchBits >>= 1;
				sptr++;
				DeadlyBits >>= 1;

			} while (TouchBits);
		}
	}
}

void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 1);
}

void CreatureCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;
	long x, z, rx, rz, c, s;

	item = &items[item_number];

	if (item->object_number == HITMAN && item->current_anim_state == 43)
		return;

	if (TestBoundsCollide(item, laraitem, coll->radius))
	{
		if (TestCollision(item, laraitem))
		{
			if (coll->enable_baddie_push || lara.water_status == LW_UNDERWATER || lara.water_status == LW_SURFACE)
				ItemPushLara(item, laraitem, coll, coll->enable_spaz, 0);
			else
			{
				if (coll->enable_spaz)
				{
					bounds = GetBestFrame(item);
					s = phd_sin(laraitem->pos.y_rot);
					c = phd_cos(laraitem->pos.y_rot);
					x = (bounds[0] + bounds[1]) >> 1;
					z = (bounds[3] - bounds[2]) >> 1;
					rx = (laraitem->pos.x_pos - item->pos.x_pos) - ((c * x + s * z) >> 14);
					rz = (laraitem->pos.z_pos - item->pos.z_pos) - ((c * z - s * x) >> 14);

					if (bounds[3] - bounds[2] > 256)
					{
						lara.hit_direction = (ushort)((laraitem->pos.y_rot - phd_atan(rz, rx) - 24576)) >> 14;
						lara.hit_frame++;

						if (lara.hit_frame > 30)
							lara.hit_frame = 30;
					}
				}
			}
		}
	}
}

void AIPickupCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number == SWITCH_TYPE7 && !(item->mesh_bits & 1))
		item->status = ITEM_INVISIBLE;
}

void TrapCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_ACTIVE)
	{
		if (!TestBoundsCollide(item, laraitem, coll->radius))
			return;
	}
	else if (item->status == ITEM_INVISIBLE)
		return;

	ObjectCollision(item_number, laraitem, coll);
}

void inject_coll(bool replace)
{
	INJECT(0x00414370, TriggerLaraBlood, replace);
	INJECT(0x00411100, GetCollisionInfo, replace);
	INJECT(0x00411D70, FindGridShift, replace);
	INJECT(0x004120A0, ShiftItem, replace);
	INJECT(0x00410EF0, GetTiltType, replace);
	INJECT(0x00413A90, GenericSphereBoxCollision, replace);
	INJECT(0x004126E0, ObjectCollision, replace);
	INJECT(0x004124E0, CreatureCollision, replace);
	INJECT(0x00412770, AIPickupCollision, replace);
	INJECT(0x004127C0, TrapCollision, replace);
}
