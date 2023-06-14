#include "../tomb5/pch.h"
#include "people.h"
#include "draw.h"
#include "control.h"
#include "objects.h"
#include "sound.h"
#include "effect2.h"
#include "sphere.h"
#include "box.h"
#include "debris.h"
#include "effects.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "gameflow.h"
#include "lara.h"

long TargetVisible(ITEM_INFO* item, AI_INFO* info)
{
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if (!enemy || enemy->hit_points <= 0 || !enemy->data || (info->angle - creature->joint_rotation[2]) <= -0x2000 ||
		(info->angle - creature->joint_rotation[2]) >= 0x2000 || info->distance >= 0x4000000)
		return 0;

	bounds = GetBestFrame(enemy);
	start.x = item->pos.x_pos;
	start.y = item->pos.y_pos - 768;
	start.z = item->pos.z_pos;
	start.room_number = item->room_number;
	target.x = enemy->pos.x_pos;
	target.y = enemy->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.z = enemy->pos.z_pos;
	return LOS(&start, &target);
}

long Targetable(ITEM_INFO* item, AI_INFO* info)
{
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if (!enemy || enemy->hit_points <= 0 || !enemy->data || !info->ahead || info->distance >= 0x4000000)
		return 0;
 
	bounds = GetBestFrame(item);
	start.x = item->pos.x_pos;

	if (item->object_number == SNIPER)
		start.y = item->pos.y_pos - 768;
	else
		start.y = item->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);

	bounds = GetBestFrame(enemy);
	start.z = item->pos.z_pos;
	start.room_number = item->room_number;
	target.x = enemy->pos.x_pos;
	target.y = enemy->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.z = enemy->pos.z_pos;
	return LOS(&start, &target);
}

short GunShot(long x, long y, long z, short speed, short yrot, short room_number)
{
	return -1;
}

short GunHit(long x, long y, long z, short speed, short yrot, short room_number)
{
	PHD_VECTOR pos;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(lara_item, &pos, (25 * GetRandomControl()) / 0x7FFF);

	DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 3, lara_item->pos.y_rot, lara_item->room_number);
	SoundEffect(SFX_LARA_INJURY_RND, &lara_item->pos, SFX_DEFAULT);
	return GunShot(x, y, z, speed, yrot, room_number);
}

short GunMiss(long x, long y, long z, short speed, short yrot, short room_number)
{
	GAME_VECTOR pos;

	pos.x = lara_item->pos.x_pos + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.y = lara_item->floor;
	pos.z = lara_item->pos.z_pos + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.room_number = lara_item->room_number;
	Richochet(&pos);
	return GunShot(x, y, z, speed, yrot, room_number);
}

long ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* gun, short extra_rotation, long damage)
{
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	MESH_INFO* StaticMesh;
	GAME_VECTOR start;
	GAME_VECTOR target;
	PHD_VECTOR pos;
	PHD_VECTOR v;
	long hit, targetable, random, distance;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if (info->distance <= 0x4000000 && Targetable(item, info))
	{
		distance = phd_sin(info->enemy_facing) * enemy->speed * 0x4000000 / 300 >> W2V_SHIFT;
		distance = info->distance + SQUARE(distance);

		if (distance <= 0x4000000)
		{
			random = (0x4000000 - info->distance) / 3276 + 0x2000;
			hit = (GetRandomControl() < random);
		}
		else
			hit = 0;

		targetable = 1;
	}
	else
	{
		hit = 0;
		targetable = 0;
	}

	if (damage)
	{
		if (enemy == lara_item)
		{
			if (hit)
			{
				CreatureEffect(item, gun, GunHit);
				lara_item->hit_points -= (short)damage;
				lara_item->hit_status = 1;
			}
			else if (targetable)
				CreatureEffect(item, gun, GunMiss);
		}
		else
		{
			CreatureEffect(item, gun, GunShot);

			if (hit)
			{
				enemy->hit_points -= short(damage / 10);
				enemy->hit_status = 1;
				random = GetRandomControl() & 0xF;

				if (random > 14)
					random = 0;

				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(enemy, &pos, random);
				DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 4, enemy->pos.y_rot, enemy->room_number);
			}
		}
	}

	start.x = item->pos.x_pos;
	start.y = item->pos.y_pos - 768;
	start.z = item->pos.z_pos;
	start.room_number = item->room_number;
	target.x = enemy->pos.x_pos;
	target.y = enemy->pos.y_pos - 768;
	target.z = enemy->pos.z_pos;
	target.x += (target.x - start.x) >> 1;
	target.y += (target.y - start.y) >> 1;
	target.z += (target.z - start.z) >> 1;

	if (ObjectOnLOS2(&start, &target, &pos, &StaticMesh) < 0)
	{
		if (StaticMesh->static_number >= 50 && StaticMesh->static_number < 58)
		{
			ShatterObject(0, StaticMesh, 128, start.room_number, 0);
			SmashedMesh[SmashedMeshCount] = StaticMesh;
			SmashedMeshRoom[SmashedMeshCount] = start.room_number;
			SmashedMeshCount++;
			v.x = StaticMesh->x;
			v.y = StaticMesh->y;
			v.z = StaticMesh->z;
			SoundEffect(ShatterSounds[gfCurrentLevel][StaticMesh->static_number - 50], (PHD_3DPOS*)&v, SFX_DEFAULT);
			TriggerRicochetSpark(&target, item->pos.y_rot, 3, 0);
			StaticMesh->Flags &= ~1;
		}
	}

	return targetable;
}
