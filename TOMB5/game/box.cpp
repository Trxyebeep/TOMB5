#include "../tomb5/pch.h"
#include "box.h"
#include "lot.h"
#include "../specific/3dmath.h"
#include "lara_states.h"

void InitialiseCreature(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->collidable = 1;
	item->data = 0;
	item->draw_room = ((item->pos.x_pos - room[item->room_number].x) >> 10 & 0xFF) << 8 | ((item->pos.z_pos - room[item->room_number].z) >> 10 & 0xFF);
	item->item_flags[2] = item->pos.y_pos - room[item->room_number].minfloor & 0xFF00 | item->room_number & 0xFF;
	item->TOSSPAD = item->pos.y_rot & 0xE000;
}

long CreatureActive(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->flags & IFL_CLEARBODY)
		return 0;

	if (item->status == 3)
	{
		if (!EnableBaddieAI(item_number, 0))
			return 0;

		item->status = 1;
	}

	return 1;
}

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info)
{
	CREATURE_INFO* creature;
	OBJECT_INFO* obj;
	ITEM_INFO* enemy;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	short* zone;
	long x, y, z;
	short pivot, ang, state;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	obj = &objects[item->object_number];
	enemy = creature->enemy;

	if (!enemy)
	{
		enemy = lara_item;
		creature->enemy = lara_item;
	}

	zone = ground_zone[creature->LOT.zone][flip_status];
	r = &room[item->room_number];
	floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];
	item->box_number = floor->box;
	info->zone_number = zone[item->box_number];

	r = &room[enemy->room_number];
	floor = &r->floor[((enemy->pos.z_pos - r->z) >> 10) + r->x_size * ((enemy->pos.x_pos - r->x) >> 10)];
	enemy->box_number = floor->box;
	info->enemy_zone = zone[enemy->box_number];

	if (boxes[enemy->box_number].overlap_index & creature->LOT.block_mask ||
		creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000))
		info->enemy_zone |= 0x4000;

	pivot = obj->pivot_length;

	if (enemy == lara_item)
		ang = lara.move_angle;
	else
		ang = enemy->pos.y_rot;

	x = enemy->pos.x_pos + (14 * enemy->speed * phd_sin(ang) >> 14) - (pivot * phd_sin(item->pos.y_rot) >> 14) - item->pos.x_pos;
	y = item->pos.y_pos - enemy->pos.y_pos;
	z = enemy->pos.z_pos + (14 * enemy->speed * phd_cos(ang) >> 14) - (pivot * phd_cos(item->pos.y_rot) >> 14) - item->pos.z_pos;

	ang = (short)phd_atan(z, x);

	if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
		info->distance = 0x7FFFFFFF;
	else if (creature->enemy)
		info->distance = SQUARE(x) + SQUARE(z);
	else
		info->distance = 0x7FFFFFFF;

	info->angle = ang - item->pos.y_rot;
	info->enemy_facing = ang - enemy->pos.y_rot + 0x8000;

	x = abs(x);
	z = abs(z);

	if (enemy == lara_item)
	{
		state = lara_item->current_anim_state;

		if (state == AS_DUCK || state == AS_DUCKROLL || (state > AS_MONKEY180 && state < AS_HANG2DUCK) || state == AS_DUCKROTL || state == AS_DUCKROTR )
			y -= 384;
	}

	if (x > z)
		info->x_angle = (short)phd_atan(x + (z >> 1), y);
	else
		info->x_angle = (short)phd_atan(z + (x >> 1), y);

	info->ahead = info->angle > -0x4000 && info->angle < 0x4000;
	info->bite = info->ahead && enemy->hit_points > 0 && abs(enemy->pos.y_pos - item->pos.y_pos) <= 512;
}

void inject_box(bool replace)
{
	INJECT(0x00408550, InitialiseCreature, replace);
	INJECT(0x00408630, CreatureActive, replace);
	INJECT(0x004086C0, CreatureAIInfo, replace);
}