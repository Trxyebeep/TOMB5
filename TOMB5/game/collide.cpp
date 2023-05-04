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
#include "pickup.h"
#include "lara_states.h"
#include "items.h"
#include "../specific/specificfx.h"
#include "sound.h"
#include "../specific/file.h"
#include "camera.h"
#include "lara.h"

static char LM[15] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 8 };

short GlobalCollisionBounds[6];

void TriggerLaraBlood()
{
	PHD_VECTOR pos;
	long lp, node;

	for (lp = 0, node = 1; lp < 15; lp++)
	{
		if (lara_item->touch_bits & node)
		{
			pos.x = (GetRandomControl() & 0x1F) - 16;
			pos.y = (GetRandomControl() & 0x1F) - 16;
			pos.z = (GetRandomControl() & 0x1F) - 16;
			GetLaraJointPos(&pos, LM[lp]);
			DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 7) + 8, short(GetRandomControl() << 1), lara_item->room_number);
		}

		node <<= 1;
	}
}

void GetCollisionInfo(COLL_INFO* coll, long xpos, long ypos, long zpos, short room_number, long objheight)
{
	FLOOR_INFO* floor;
	static long xfront, zfront;
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
	fspeed = short(yT - lara_item->fallspeed);
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
		xfront = (phd_sin(coll->facing) * coll->radius) >> W2V_SHIFT;
		zfront = coll->radius;
		xright = coll->radius; 
		zright = coll->radius; 
		zleft = coll->radius;
		xleft = -coll->radius;
		break;

	case EAST:
		xfront = coll->radius;
		zfront = (phd_cos(coll->facing) * coll->radius) >> W2V_SHIFT;
		xright = coll->radius;
		zright = -coll->radius;
		zleft = coll->radius;
		xleft = coll->radius;
		break;

	case SOUTH:
		xfront = (phd_sin(coll->facing) * coll->radius) >> W2V_SHIFT;
		zfront = -coll->radius;
		xright = -coll->radius;
		zright = -coll->radius;
		zleft = -coll->radius;
		xleft = coll->radius;
		break;

	case WEST:
		xfront = -coll->radius;
		zfront = (phd_cos(coll->facing) * coll->radius) >> W2V_SHIFT;
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
		if (coll->front_type == DIAGONAL || coll->front_type == SPLIT_TRI)
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

long FindGridShift(long src, long dst)
{
	long srcw, dstw;

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
			item->pos.y_rot = (short)dy;
			sptr = Slist;
			DeadlyBits = *(long*)&item->item_flags[0];

			do
			{
				if (TouchBits & 1)
				{
					GlobalCollisionBounds[0] = short(sptr->x - sptr->r - item->pos.x_pos);
					GlobalCollisionBounds[2] = short(sptr->y - sptr->r - item->pos.y_pos);
					GlobalCollisionBounds[4] = short(sptr->z - sptr->r - item->pos.z_pos);
					GlobalCollisionBounds[1] = short(sptr->x + sptr->r - item->pos.x_pos);
					GlobalCollisionBounds[3] = short(sptr->y + sptr->r - item->pos.y_pos);
					GlobalCollisionBounds[5] = short(sptr->z + sptr->r - item->pos.z_pos);
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
					rx = (laraitem->pos.x_pos - item->pos.x_pos) - ((c * x + s * z) >> W2V_SHIFT);
					rz = (laraitem->pos.z_pos - item->pos.z_pos) - ((c * z - s * x) >> W2V_SHIFT);

					if (bounds[3] - bounds[2] > 256)
					{
						lara.hit_direction = ushort((laraitem->pos.y_rot - phd_atan(rz, rx) - 0x6000)) >> W2V_SHIFT;
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

void TestForObjectOnLedge(ITEM_INFO* item, COLL_INFO* coll)
{
	MESH_INFO* StaticMesh;
	STATIC_INFO* stat;
	GAME_VECTOR s;
	GAME_VECTOR d;
	PHD_VECTOR v;
	long objectonlos2;

	for (int i = 0; i < 3; i++)
	{
		s.x = (i * 96) - 96;	//-96, 0, 96
		s.y = -512;
		s.z = -128;
		GetLaraJointPos((PHD_VECTOR*)&s, LMX_TORSO);
		s.room_number = lara_item->room_number;
		d.x = s.x + ((768 * phd_sin(lara_item->pos.y_rot)) >> W2V_SHIFT);
		d.y = s.y;
		d.z = s.z + ((768 * phd_cos(lara_item->pos.y_rot)) >> W2V_SHIFT);
		LOS(&s, &d);
		objectonlos2 = ObjectOnLOS2(&s, &d, &v, &StaticMesh);

		if (objectonlos2 == 999)
			coll->hit_static = 0;
		else
		{
			if (objectonlos2 >= 0)
			{
				if (objects[items[objectonlos2].object_number].collision != PickUpCollision)
				{
					coll->hit_static = 1;
					return;
				}
				else
					coll->hit_static = 0;
			}
			else
			{
				if (StaticMesh->Flags & 1)
				{
					stat = &static_objects[StaticMesh->static_number];

					if (!stat->x_minc && !stat->x_maxc && !stat->y_minc && !stat->y_maxc && !stat->z_minc && !stat->z_maxc)
						coll->hit_static = 0;
					else
					{
						coll->hit_static = 1;
						return;
					}
				}
				else
					coll->hit_static = 0;
			}
		}
	}
}

long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems, MESH_INFO** StoredStatics, long StoreLara)
{
	MESH_INFO* mesh;
	ROOM_INFO* r;
	ITEM_INFO* item2;
	short* doors;
	short* bounds;
	long j, sy, cy, dx, dy, dz, num;
	short rooms[22];
	short switch_bounds[6];
	short room_count, statics_count, items_count, item_number, next_item;

	rooms[0] = camera.pos.room_number;
	r = &room[rooms[0]];
	doors = r->door;
	room_count = 1;
	statics_count = 0;
	items_count = 0;

	if (doors)
	{
		for (int i = *doors++; i > 0; i--, doors += 16)
		{
			for (j = 0; j < room_count; j++)
				if (rooms[j] == *doors)
					break;

			if (j == room_count)
			{
				rooms[room_count] = *doors;
				room_count++;
			}
		}
	}

	if (StoredStatics)
	{
		for (int i = 0; i < room_count; i++)
		{
			r = &room[rooms[i]];
			mesh = r->mesh;

			for (j = r->num_meshes; j > 0; j--, mesh++)
			{
				if (mesh->Flags & 1)
				{
					bounds = &static_objects[mesh->static_number].x_minc;

					if (item->pos.y_pos + rad + 128 >= mesh->y + bounds[2] && item->pos.y_pos - rad - 128 <= mesh->y + bounds[3])
					{
						sy = phd_sin(mesh->y_rot);
						cy = phd_cos(mesh->y_rot);
						dx = item->pos.x_pos - mesh->x;
						dz = item->pos.z_pos - mesh->z;
						num = (dx * cy - sy * dz) >> W2V_SHIFT;

						if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1])
						{
							num = (dx * sy + cy * dz) >> W2V_SHIFT;

							if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5])
							{
								StoredStatics[statics_count] = mesh;
								statics_count++;

								if (!rad)
								{
									StoredItems[0] = 0;
									return 1;
								}
							}
						}
					}
				}
			}
		}

		StoredStatics[statics_count] = 0;
	}

	for (int i = 0; i < room_count; i++)
	{
		item_number = room[rooms[i]].item_number;

		while (item_number != NO_ITEM)
		{
			item2 = &items[item_number];
			next_item = item2->next_item;

			if (item2 == item)	//don't get the item we're checking for
			{
				item_number = next_item;
				continue;
			}

			if (!StoreLara && item2 == lara_item)	//don't get lara if we don't want her
			{
				item_number = next_item;
				continue;
			}

			if (item2->flags & IFL_CLEARBODY)	//don't get clearbodies
			{
				item_number = next_item;
				continue;
			}

			if (!objects[item2->object_number].collision && item2->object_number != LARA)	//don't get objects without collision
			{
				item_number = next_item;
				continue;
			}

			if (!objects[item2->object_number].draw_routine || !item2->mesh_bits)	//don't get objects that are not drawn
			{
				item_number = next_item;
				continue;
			}

			if (noInvisible && item2->status == ITEM_INVISIBLE)	//don't get invisibles if we don't want them
			{
				item_number = next_item;
				continue;
			}

			dx = item->pos.x_pos - item2->pos.x_pos;
			dy = item->pos.y_pos - item2->pos.y_pos;
			dz = item->pos.z_pos - item2->pos.z_pos;

			if (dx < -2048 || dx > 2048 || dy < -2048 || dy > 2048 || dz < -2048 || dz > 2048)	//further than 2 blocks? bye
			{
				item_number = next_item;
				continue;
			}

			bounds = GetBestFrame(item2);

			if (item->pos.y_pos + rad + 128 < item2->pos.y_pos + bounds[2] || item->pos.y_pos - rad - 128 > item2->pos.y_pos + bounds[3])
			{
				item_number = next_item;
				continue;
			}

			sy = phd_sin(item2->pos.y_rot);
			cy = phd_cos(item2->pos.y_rot);
			dx = item->pos.x_pos - item2->pos.x_pos;
			dz = item->pos.z_pos - item2->pos.z_pos;
			num = (dx * cy - sy * dz) >> W2V_SHIFT;

			if (item2->object_number == TURN_SWITCH)
			{
				switch_bounds[0] = -256;
				switch_bounds[1] = 256;
				switch_bounds[4] = -256;
				switch_bounds[5] = 256;
				bounds = switch_bounds;
			}

			if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1])
			{
				num = (dx * sy + cy * dz) >> W2V_SHIFT;

				if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5])
				{
					StoredItems[items_count] = item2;
					items_count++;

					if (!rad)
						return 1;
				}
			}

			item_number = next_item;
		}
	}

	StoredItems[items_count] = 0;
	return items_count | statics_count;
}

long MoveLaraPosition(PHD_VECTOR* v, ITEM_INFO* item, ITEM_INFO* laraitem)
{
	PHD_3DPOS pos;
	long height;
	short room_number;

	pos.x_rot = item->pos.x_rot;
	pos.y_rot = item->pos.y_rot;
	pos.z_rot = item->pos.z_rot;
	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	pos.x_pos = item->pos.x_pos + ((v->x * phd_mxptr[M00] + v->y * phd_mxptr[M01] + v->z * phd_mxptr[M02]) >> W2V_SHIFT);
	pos.y_pos = item->pos.y_pos + ((v->x * phd_mxptr[M10] + v->y * phd_mxptr[M11] + v->z * phd_mxptr[M12]) >> W2V_SHIFT);
	pos.z_pos = item->pos.z_pos + ((v->x * phd_mxptr[M20] + v->y * phd_mxptr[M21] + v->z * phd_mxptr[M22]) >> W2V_SHIFT);
	phd_PopMatrix();

	if (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM)
	{
		room_number = laraitem->room_number;
		height = GetHeight(GetFloor(pos.x_pos, pos.y_pos, pos.z_pos, &room_number), pos.x_pos, pos.y_pos, pos.z_pos);

		if (abs(height - laraitem->pos.y_pos) > 512)
		{
			if (lara.IsMoving)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			return 0;
		}

		if (phd_sqrt(SQUARE(pos.x_pos - laraitem->pos.x_pos) + SQUARE(pos.y_pos - laraitem->pos.y_pos) + SQUARE(pos.z_pos - laraitem->pos.z_pos)) < 128)
			return 1;
	}

	return Move3DPosTo3DPos(&laraitem->pos, &pos, 12, 364);
}

long Move3DPosTo3DPos(PHD_3DPOS* pos, PHD_3DPOS* dest, long speed, short rotation)
{
	ulong ang;
	long dx, dy, dz, distance, shift;
	ushort quad;
	short adiff;

	dx = dest->x_pos - pos->x_pos;
	dy = dest->y_pos - pos->y_pos;
	dz = dest->z_pos - pos->z_pos;
	distance = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

	if (speed >= distance)
	{
		pos->x_pos = dest->x_pos;
		pos->y_pos = dest->y_pos;
		pos->z_pos = dest->z_pos;
	}
	else
	{
		shift = 65536 * speed / distance;
		pos->x_pos += shift * dx >> 16;
		pos->y_pos += shift * dy >> 16;
		pos->z_pos += shift * dz >> 16;
	}

	if (!lara.IsMoving)
	{
		if (lara.water_status != LW_UNDERWATER)
		{
			ang = ulong(mGetAngle(dest->x_pos, dest->z_pos, pos->x_pos, pos->z_pos) + 0x2000) / 0x4000;
			quad = ushort(dest->y_rot + 0x2000) / 0x4000;
			ang -= quad;
			ang &= 3;

			switch (ang)
			{
			case 0:
				lara_item->anim_number = 65;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->goal_anim_state = AS_STEPLEFT;
				lara_item->current_anim_state = AS_STEPLEFT;
				break;

			case 1:
				lara_item->anim_number = 1;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->goal_anim_state = AS_WALK;
				lara_item->current_anim_state = AS_WALK;
				break;

			case 2:
				lara_item->anim_number = 67;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->goal_anim_state = AS_STEPRIGHT;
				lara_item->current_anim_state = AS_STEPRIGHT;
				break;

			default:
				lara_item->anim_number = 40;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->goal_anim_state = AS_BACK;
				lara_item->current_anim_state = AS_BACK;
				break;
			}

			lara.gun_status = LG_HANDS_BUSY;
		}

		lara.IsMoving = 1;
		lara.MoveCount = 0;
	}

	adiff = dest->x_rot - pos->x_rot;

	if (adiff > rotation)
		pos->x_rot += rotation;
	else if (adiff < -rotation)
		pos->x_rot -= rotation;
	else
		pos->x_rot = dest->x_rot;

	adiff = dest->y_rot - pos->y_rot;

	if (adiff > rotation)
		pos->y_rot += rotation;
	else if (adiff < -rotation)
		pos->y_rot -= rotation;
	else
		pos->y_rot = dest->y_rot;

	adiff = dest->z_rot - pos->z_rot;

	if (adiff > rotation)
		pos->z_rot += rotation;
	else if (adiff < -rotation)
		pos->z_rot -= rotation;
	else
		pos->z_rot = dest->z_rot;

	return pos->x_pos == dest->x_pos && pos->y_pos == dest->y_pos && pos->z_pos == dest->z_pos && pos->x_rot == dest->x_rot && pos->y_rot == dest->y_rot && pos->z_rot == dest->z_rot;
}

long CollideStaticObjects(COLL_INFO* coll, long x, long y, long z, short room_number, long hite)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	short* door;
	long lxmin, lxmax, lymin, lymax, lzmin, lzmax;
	long xmin, xmax, ymin, ymax, zmin, zmax;
	long i, j;
	short num_nearby_rooms;
	short nearby_rooms[22];

	coll->hit_static = 0;
	lxmin = x - coll->radius;
	lxmax = x + coll->radius;
	lymin = y - hite;
	lymax = y;
	lzmin = z - coll->radius;
	lzmax = z + coll->radius;
	num_nearby_rooms = 1;
	nearby_rooms[0] = room_number;
	door = room[room_number].door;

	if (door)
	{
		for (i = *door++; i > 0; i--)
		{
			for (j = 0; j < num_nearby_rooms; j++)
			{
				if (nearby_rooms[j] == *door)
					break;
			}

			if (j == num_nearby_rooms)
			{
				nearby_rooms[num_nearby_rooms] = *door;
				num_nearby_rooms++;
			}

			door += 16;
		}
	}

	for (i = 0; i < num_nearby_rooms; i++)
	{
		r = &room[nearby_rooms[i]];
		mesh = r->mesh;

		for (j = r->num_meshes; j > 0; j--, mesh++)
		{
			sinfo = &static_objects[mesh->static_number];

			if (!(mesh->Flags & 1))
				continue;

			ymin = mesh->y + sinfo->y_minc;
			ymax = mesh->y + sinfo->y_maxc;

			if (mesh->y_rot == -0x8000)
			{
				xmin = mesh->x - sinfo->x_maxc;
				xmax = mesh->x - sinfo->x_minc;
				zmin = mesh->z - sinfo->z_maxc;
				zmax = mesh->z - sinfo->z_minc;
			}
			else if (mesh->y_rot == -0x4000)
			{
				xmin = mesh->x - sinfo->z_maxc;
				xmax = mesh->x - sinfo->z_minc;
				zmin = mesh->z + sinfo->x_minc;
				zmax = mesh->z + sinfo->x_maxc;
			}
			else if (mesh->y_rot == 0x4000)
			{
				xmin = mesh->x + sinfo->z_minc;
				xmax = mesh->x + sinfo->z_maxc;
				zmin = mesh->z - sinfo->x_maxc;
				zmax = mesh->z - sinfo->x_minc;
			}
			else
			{
				xmin = mesh->x + sinfo->x_minc;
				xmax = mesh->x + sinfo->x_maxc;
				zmin = mesh->z + sinfo->z_minc;
				zmax = mesh->z + sinfo->z_maxc;
			}

			if (lxmax <= xmin || lxmin >= xmax || lymax <= ymin || lymin >= ymax || lzmax <= zmin || lzmin >= zmax)
				continue;

			coll->hit_static = 1;
			return 1;
		}
	}

	return 0;
}

void UpdateLaraRoom(ITEM_INFO* item, long height)
{
	FLOOR_INFO* floor;
	long x, y, z;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos + height;
	z = item->pos.z_pos;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	item->floor = GetHeight(floor, x, y, z);

	if (item->room_number != room_number)
		ItemNewRoom(lara.item_number, room_number);
}

void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll)
{
	ROOM_INFO* r;
	ITEM_INFO* item;
	MESH_INFO* mesh;
	PHD_3DPOS pos;
	short* door;
	short* bounds;
	long i, j, dx, dy, dz;
	short num_nearby_rooms, item_number, nex;
	short nearby_rooms[22];

	l->hit_status = 0;
	lara.hit_direction = -1;

	if (l->hit_points <= 0)
		return;

	num_nearby_rooms = 1;
	nearby_rooms[0] = l->room_number;
	door = room[nearby_rooms[0]].door;

	if (door)
	{
		for (i = *door++; i > 0; i--)
		{
			for (j = 0; j < num_nearby_rooms; j++)
			{
				if (nearby_rooms[j] == *door)
					break;
			}

			if (j == num_nearby_rooms)
			{
				nearby_rooms[num_nearby_rooms] = *door;
				num_nearby_rooms++;
			}

			door += 16;
		}
	}

	for (i = 0; i < num_nearby_rooms; i++)
	{
		r = &room[nearby_rooms[i]];
		item_number = r->item_number;

		while (item_number != NO_ITEM)
		{
			item = &items[item_number];
			nex = item->next_item;

			if (item->collidable && item->status != ITEM_INVISIBLE)
			{
				if (objects[item->object_number].collision)
				{
					dx = l->pos.x_pos - item->pos.x_pos;
					dy = l->pos.y_pos - item->pos.y_pos;
					dz = l->pos.z_pos - item->pos.z_pos;

					if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072)
						objects[item->object_number].collision(item_number, l, coll);
				}
			}

			item_number = nex;
		}

		if (coll->enable_baddie_push)
		{
			r = &room[nearby_rooms[i]];
			mesh = r->mesh;

			for (j = r->num_meshes; j > 0; j--, mesh++)
			{
				if (!(mesh->Flags & 1))
					continue;

				dx = l->pos.x_pos - mesh->x;
				dy = l->pos.y_pos - mesh->y;
				dz = l->pos.z_pos - mesh->z;

				if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072)
				{
					bounds = &static_objects[mesh->static_number].x_minc;
					pos.x_pos = mesh->x;
					pos.y_pos = mesh->y;
					pos.z_pos = mesh->z;
					pos.y_rot = mesh->y_rot;

					if (TestBoundsCollideStatic(bounds, &pos, coll->radius))
						ItemPushLaraStatic(l, bounds, &pos, coll);
				}
			}
		}
	}

	if (lara.hit_direction == -1)
		lara.hit_frame = 0;
}

long ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, long spaz, long BigPush)
{
	short* bounds;
	long dx, dz, s, c, x, z;
	long xmin, xmax, zmin, zmax, left, top, right, bottom;
	short facing;

	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;

	if (BigPush & 2)
		bounds = GlobalCollisionBounds;
	else
		bounds = GetBestFrame(item);

	xmin = bounds[0];
	xmax = bounds[1];
	zmin = bounds[4];
	zmax = bounds[5];

	if (BigPush & 1)
	{
		xmin -= coll->radius;
		xmax += coll->radius;
		zmin -= coll->radius;
		zmax += coll->radius;
	}

	if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
		return 0;

	left = x - xmin;
	top = zmax - z;
	right = xmax - x;
	bottom = z - zmin;

	if (left <= right && left <= top && left <= bottom)
		x -= left;
	else if (right <= left && right <= top && right <= bottom)
		x += right;
	else if (top <= left && top <= right && top <= bottom)
		z += top;
	else
		z -= bottom;

	l->pos.x_pos = item->pos.x_pos + ((c * x + s * z) >> W2V_SHIFT);
	l->pos.z_pos = item->pos.z_pos + ((c * z - s * x) >> W2V_SHIFT);

	if (spaz && bounds[3] - bounds[2] > 256)
	{
		x = (bounds[0] + bounds[1]) / 2;
		z = (bounds[4] + bounds[5]) / 2;
		dx -= (c * x + s * z) >> W2V_SHIFT;
		dz -= (c * z - s * x) >> W2V_SHIFT;
		lara.hit_direction = ushort(l->pos.y_rot - phd_atan(dz, dx) - 0x6000) >> W2V_SHIFT;

		if (!lara.hit_frame)
			SoundEffect(SFX_LARA_INJURY_RND, &l->pos, SFX_DEFAULT);

		lara.hit_frame++;

		if (lara.hit_frame > 34)
			lara.hit_frame = 34;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	facing = coll->facing;
	coll->facing = (short)phd_atan(l->pos.z_pos - coll->old.z, l->pos.x_pos - coll->old.x);
	GetCollisionInfo(coll, l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number, 762);
	coll->facing = facing;

	if (coll->coll_type == CT_NONE)
	{
		coll->old.x = l->pos.x_pos;
		coll->old.y = l->pos.y_pos;
		coll->old.z = l->pos.z_pos;
		UpdateLaraRoom(l, -10);
	}
	else
	{
		l->pos.x_pos = coll->old.x;
		l->pos.z_pos = coll->old.z;
	}

	if (lara.IsMoving && lara.MoveCount > 15)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

	return 1;
}

long ItemPushLaraStatic(ITEM_INFO* l, short* bounds, PHD_3DPOS* pos, COLL_INFO* coll)
{
	long dx, dz, s, c, x, z;
	long xmin, xmax, zmin, zmax, left, top, right, bottom;
	short facing;

	dx = l->pos.x_pos - pos->x_pos;
	dz = l->pos.z_pos - pos->z_pos;
	s = phd_sin(pos->y_rot);
	c = phd_cos(pos->y_rot);
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	xmin = bounds[0] - coll->radius;
	xmax = bounds[1] + coll->radius;
	zmin = bounds[4] - coll->radius;
	zmax = bounds[5] + coll->radius;

	if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
		return 0;

	left = x - xmin;
	top = zmax - z;
	right = xmax - x;
	bottom = z - zmin;

	if (left <= right && left <= top && left <= bottom)
		x -= left;
	else if (right <= left && right <= top && right <= bottom)
		x += right;
	else if (top <= left && top <= right && top <= bottom)
		z += top;
	else
		z -= bottom;

	l->pos.x_pos = pos->x_pos + ((c * x + s * z) >> W2V_SHIFT);
	l->pos.z_pos = pos->z_pos + ((c * z - s * x) >> W2V_SHIFT);
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	facing = coll->facing;
	coll->facing = (short)phd_atan(l->pos.z_pos - coll->old.z, l->pos.x_pos - coll->old.x);
	GetCollisionInfo(coll, l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number, 762);
	coll->facing = facing;

	if (coll->coll_type == CT_NONE)
	{
		coll->old.x = l->pos.x_pos;
		coll->old.y = l->pos.y_pos;
		coll->old.z = l->pos.z_pos;
		UpdateLaraRoom(l, -10);
	}
	else
	{
		l->pos.x_pos = coll->old.x;
		l->pos.z_pos = coll->old.z;
	}

	if (l == lara_item && lara.IsMoving && lara.MoveCount > 15)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

	return 1;
}

long TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, long rad)
{
	short* bounds;
	short* lbounds;
	long s, c, dx, dz, x, z;

	bounds = GetBestFrame(item);
	lbounds = GetBestFrame(l);

	if (item->pos.y_pos + bounds[3] <= l->pos.y_pos + lbounds[2] || item->pos.y_pos + bounds[2] >= l->pos.y_pos + lbounds[3])
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

long TestBoundsCollideStatic(short* bounds, PHD_3DPOS* pos, long rad)
{
	short* lbounds;
	long s, c, dx, dz, x, z;

	if (!(bounds[0] | bounds[1] | bounds[2] | bounds[3] | bounds[4] | bounds[5]))
		return 0;

	lbounds = GetBestFrame(lara_item);

	if (pos->y_pos + bounds[3] <= lara_item->pos.y_pos + lbounds[2] || pos->y_pos + bounds[2] >= lara_item->pos.y_pos + lbounds[3])
		return 0;

	s = phd_sin(pos->y_rot);
	c = phd_cos(pos->y_rot);
	dx = lara_item->pos.x_pos - pos->x_pos;
	dz = lara_item->pos.z_pos - pos->z_pos;
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

long TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l)
{
	PHD_VECTOR pos;
	long x, y, z;
	short xrot, yrot, zrot;

	xrot = l->pos.x_rot - item->pos.x_rot;
	yrot = l->pos.y_rot - item->pos.y_rot;
	zrot = l->pos.z_rot - item->pos.z_rot;

	if (xrot < bounds[6] || xrot > bounds[7] ||
		yrot < bounds[8] || yrot > bounds[9] ||
		zrot < bounds[10] || zrot > bounds[11])
		return 0;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	pos.x = l->pos.x_pos - item->pos.x_pos;
	pos.y = l->pos.y_pos - item->pos.y_pos;
	pos.z = l->pos.z_pos - item->pos.z_pos;
	x = (pos.x * phd_mxptr[M00] + pos.y * phd_mxptr[M10] + pos.z * phd_mxptr[M20]) >> W2V_SHIFT;
	y = (pos.x * phd_mxptr[M01] + pos.y * phd_mxptr[M11] + pos.z * phd_mxptr[M21]) >> W2V_SHIFT;
	z = (pos.x * phd_mxptr[M02] + pos.y * phd_mxptr[M12] + pos.z * phd_mxptr[M22]) >> W2V_SHIFT;
	phd_PopMatrix();

	return x >= bounds[0] && x <= bounds[1] && y >= bounds[2] && y <= bounds[3] && z >= bounds[4] && z <= bounds[5];
}

void AlignLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l)
{
	long x, y, z;

	l->pos.x_rot = item->pos.x_rot;
	l->pos.y_rot = item->pos.y_rot;
	l->pos.z_rot = item->pos.z_rot;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	x = item->pos.x_pos + ((pos->x * phd_mxptr[M00] + pos->y * phd_mxptr[M01] + pos->z * phd_mxptr[M02]) >> W2V_SHIFT);
	y = item->pos.y_pos + ((pos->x * phd_mxptr[M10] + pos->y * phd_mxptr[M11] + pos->z * phd_mxptr[M12]) >> W2V_SHIFT);
	z = item->pos.z_pos + ((pos->x * phd_mxptr[M20] + pos->y * phd_mxptr[M21] + pos->z * phd_mxptr[M22]) >> W2V_SHIFT);
	phd_PopMatrix();

	l->pos.x_pos = x;
	l->pos.y_pos = y;
	l->pos.z_pos = z;
}
