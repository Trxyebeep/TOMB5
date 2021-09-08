#include "../tomb5/pch.h"
#include "tower1.h"
#include "gameflow.h"
#include "control.h"

long TestBoundsCollideCamera(short* bounds, PHD_3DPOS* pos, long radius)
{
	long x, z, dx, dz, sin, cos;

	if (pos->y_pos + bounds[3] > camera.pos.y - radius && pos->y_pos + bounds[2] < radius + camera.pos.y)
	{
		dx = camera.pos.x - pos->x_pos;
		dz = camera.pos.z - pos->z_pos;
		sin = phd_sin(pos->y_rot);
		cos = phd_cos(pos->y_rot);
		x = (cos * (camera.pos.x - pos->x_pos) - sin * dz) >> 14;
		z = (cos * dz + sin * (camera.pos.x - pos->x_pos)) >> 14;

		if (x >= bounds[0] - radius && x <= radius + bounds[1] && z >= bounds[4] - radius && z <= radius + bounds[5])
			return 1;
	}

	return 0;
}

void CheckForRichesIllegalDiagonalWalls()
{
	MESH_INFO* mesh;
	ROOM_INFO* r;
	PHD_3DPOS pos;
	short* doors;
	short* bounds;
	long j, dx, dy, dz;
	short room_count;
	short rooms[22];

	rooms[0] = camera.pos.room_number;
	r = &room[rooms[0]];
	doors = r->door;
	room_count = 1;

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

	for (int i = 0; i < room_count; i++)
	{
		r = &room[rooms[i]];
		mesh = r->mesh;

		for (j = r->num_meshes; j > 0; j--, mesh++)
		{
			if (mesh->Flags & 1)
			{
				if (mesh->static_number == 36 || gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS && mesh->static_number == 27)
				{
					dx = camera.pos.x - mesh->x;
					dy = camera.pos.y - mesh->y;
					dz = camera.pos.z - mesh->z;

					if (dx > -4096 && dx < 4096 && dz > -4096 && dz < 4096 && dy > -4096 && dy < 4096)
					{
						bounds = &static_objects[mesh->static_number].x_minc;
						pos.x_pos = mesh->x;
						pos.y_pos = mesh->y;
						pos.z_pos = mesh->z;
						pos.y_rot = mesh->y_rot;

						if (TestBoundsCollideCamera(bounds, &pos, 512))
							ItemPushCamera(bounds, &pos);
					}
				}
			}
		}
	}
}

void ItemPushCamera(short* bounds, PHD_3DPOS* pos)
{
	FLOOR_INFO* floor;
	long x, z, dx, dz, sin, cos, left, right, top, bottom, h, c;
	short xmin, xmax, zmin, zmax;

	dx = camera.pos.x - pos->x_pos;
	dz = camera.pos.z - pos->z_pos;
	sin = phd_sin(pos->y_rot);
	cos = phd_cos(pos->y_rot);
	x = (dx * cos - dz * sin) >> 14;
	z = (dx * sin + dz * cos) >> 14;
	xmin = bounds[0] - 384;
	xmax = bounds[1] + 384;
	zmin = bounds[4] - 384;
	zmax = bounds[5] + 384;

	if (ABS(dx) > 4608 || ABS(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
		return;

	left = x - xmin;
	right = xmax - x;
	top = zmax - z;
	bottom = z - zmin;

	if (left <= right && left <= top && left <= bottom)//left is closest
		x -= left;
	else if (right <= left && right <= top && right <= bottom)//right is closest
		x += right;
	else if (top <= left && top <= right && top <= bottom)//top is closest
		z += top;
	else
		z -= bottom;//bottom

	camera.pos.x = pos->x_pos + ((cos * x + sin * z) >> 14);
	camera.pos.z = pos->z_pos + ((cos * z - sin * x) >> 14);
	floor = GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	h = GetHeight(floor, camera.pos.x, camera.pos.y, camera.pos.z);
	c = GetCeiling(floor, camera.pos.x, camera.pos.y, camera.pos.z);

	if (h == NO_HEIGHT || camera.pos.y > h || camera.pos.y < c)
	{
		camera.pos.x = CamOldPos.x;
		camera.pos.y = CamOldPos.y;
		camera.pos.z = CamOldPos.z;
		GetFloor(CamOldPos.x, CamOldPos.y, CamOldPos.z, &camera.pos.room_number);
	}
}

void inject_tower1(bool replace)
{
	INJECT(0x00480200, TestBoundsCollideCamera, replace);
	INJECT(0x0047FFE0, CheckForRichesIllegalDiagonalWalls, replace);
	INJECT(0x00480310, ItemPushCamera, replace);
}
