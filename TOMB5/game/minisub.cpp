#include "../tomb5/pch.h"
#include "minisub.h"
#include "tomb4fx.h"
#include "control.h"
#include "objects.h"
#include "items.h"
#include "sound.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "lara1gun.h"
#include "effects.h"
#include "larafire.h"
#include "box.h"
#include "people.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "lara.h"

void TriggerTorpedoSteam(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 32;
	sptr->sG = 32;
	sptr->sB = 32;
	sptr->dR = 128;
	sptr->dG = 128;
	sptr->dB = 128;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 16;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + pos1->x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos1->y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos1->z - 16;
	sptr->Xvel = short(pos2->x + (GetRandomControl() & 0x7F) - pos1->x - 64);
	sptr->Yvel = short(pos2->y + (GetRandomControl() & 0x7F) - pos1->y - 64);
	sptr->Zvel = short(pos2->z + (GetRandomControl() & 0x7F) - pos1->z - 64);
	sptr->Friction = 51;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = 0;
	sptr->Scalar = uchar(2 - chaff);
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->Size = (GetRandomControl() & 0xF) + 32;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size << 1;
}

void TriggerMinisubLight(short item_number)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dG = (GetRandomControl() & 0x1F) + 224;
	sptr->dR = sptr->dG >> 1;
	sptr->dB = sptr->dG >> 1;
	sptr->ColFadeSpeed = 2;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->FadeToBlack = 0;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_NOKILL | SF_DEF | SF_SCALE;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = 7;
	sptr->x = 0;
	sptr->z = 0;
	sptr->y = 0;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Scalar = 1;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
	sptr->Size = (GetRandomControl() & 7) + 192;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size;
}

void FireTorpedo(ITEM_INFO* src)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short item_number;

	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	SoundEffect(SFX_UNDERWATER_TORPEDO, &item->pos, SFX_ALWAYS);
	item->object_number = TORPEDO;
	item->shade = -0x3DF0;

	for (int i = 0; i < 8; i++)
	{
		pos.x = (GetRandomControl() & 0x7F) - 414;
		pos.y = -320;
		pos.z = 352;
		GetJointAbsPosition(src, &pos, 4);

		pos2.x = (GetRandomControl() & 0x3FF) - 862;
		pos2.y = -320 - (GetRandomControl() & 0x3FF);
		pos2.z = (GetRandomControl() & 0x3FF) - 160;
		GetJointAbsPosition(src, &pos2, 4);

		TriggerTorpedoSteam(&pos, &pos2, 0);
	}

	item->room_number = src->room_number;
	GetFloor(pos.x, pos.y, pos.z, &item->room_number);
	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;
	InitialiseItem(item_number);
	item->pos.x_rot = 0;
	item->pos.y_rot = src->pos.y_rot;
	item->pos.z_rot = 0;
	item->speed = 0;
	item->fallspeed = 0;
	item->item_flags[0] = -1;
	AddActiveItem(item_number);
}

void TorpedoControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	FLOOR_INFO* floor;
	PHD_VECTOR target;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	long x, y, z, h, c;
	short angles[2];
	short found, room_number, speed;

	item = &items[item_number];
	SoundEffect(SFX_SWIMSUIT_METAL_CLASH, &item->pos, SFX_ALWAYS);

	if (item->item_flags[0] == NO_ITEM)
	{
		found = 0;

		for (int i = level_items; i < MAX_ITEMS; i++)
		{
			item2 = &items[i];

			if (item2->object_number == CHAFF && item2->active)
			{
				item->item_flags[0] = i;
				target.x = item2->pos.x_pos;
				target.y = item2->pos.y_pos;
				target.z = item2->pos.z_pos;
				found = 1;
				break;
			}
		}

		if (!found)
		{
			target.x = lara_item->pos.x_pos;
			target.y = lara_item->pos.y_pos;
			target.z = lara_item->pos.z_pos;
		}
	}
	else
	{
		item2 = &items[item->item_flags[0]];

		if (item2->active && item2->object_number == CHAFF)
		{
			target.x = item2->pos.x_pos;
			target.y = item2->pos.y_pos;
			target.z = item2->pos.z_pos;
			item->current_anim_state = short(target.x >> 2);
			item->goal_anim_state = short(target.y >> 2);
			item->required_anim_state = short(target.z >> 2);
		}
		else
		{
			target.x = item->current_anim_state << 2;
			target.y = item->goal_anim_state << 2;
			target.z = item->required_anim_state << 2;
		}
	}

	phd_GetVectorAngles(target.x - item->pos.x_pos, target.y - item->pos.y_pos, target.z - item->pos.z_pos, angles);

	if (item->speed < 48)
		item->speed += 4;
	else if (item->speed < 192)
		item->speed++;

	item->item_flags[1]++;

	if (item->item_flags[1] < 60)
	{
		y = (ushort)angles[0] - (ushort)item->pos.y_rot;

		if (abs(y) > 0x8000)
			y = (ushort)item->pos.y_rot - (ushort)angles[0];

		x = (ushort)angles[1] - (ushort)item->pos.x_rot;

		if (abs(x) > 0x8000)
			x = (ushort)item->pos.x_rot - (ushort)angles[0];

		x >>= 3;
		y >>= 3;

		if (x > 512)
			x = 512;
		else if (x < -512)
			x = -512;

		if (y > 512)
			y = 512;
		else if (y < -512)
			y = -512;

		item->pos.x_rot += (short)x;
		item->pos.y_rot += (short)y;
	}

	item->pos.z_rot += item->speed << 4;
	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	speed = item->speed * phd_cos(item->pos.x_rot) >> W2V_SHIFT;
	item->pos.x_pos += (speed * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
	item->pos.y_pos += (item->speed * phd_sin(-item->pos.x_rot) >> W2V_SHIFT);
	item->pos.z_pos += (speed * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos >= h || item->pos.y_pos <= c || !(room[room_number].flags & ROOM_UNDERWATER))
	{
		item->pos.x_pos = x;
		item->pos.y_pos = y;
		item->pos.z_pos = z;
		TriggerUnderwaterExplosion(item);
		SoundEffect(SFX_UNDERWATER_EXPLOSION, &item->pos, SFX_ALWAYS);
		KillItem(item_number);
		return;
	}

	if (ItemNearLara(&item->pos, 200))
	{
		TriggerUnderwaterExplosion(item);
		SoundEffect(SFX_UNDERWATER_EXPLOSION, &item->pos, SFX_ALWAYS);
		SoundEffect(SFX_LARA_UNDERWATER_HIT, &lara_item->pos, SFX_ALWAYS);
		lara_item->hit_points -= 200;
		lara_item->hit_status = 1;

		if (lara.Anxiety < 127)
			lara.Anxiety += 128;
		else
			lara.Anxiety--;

		KillItem(item_number);
		return;
	}

	if (ItemNearLara(&item->pos, 400) && lara.Anxiety < 224)
		lara.Anxiety += 32;

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	pos1.x = 0;
	pos1.y = 0;
	pos1.z = -64;
	GetJointAbsPosition(item, &pos1, 0);

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = -(64 << ((GlobalCounter & 1) + 2));
	GetJointAbsPosition(item, &pos2, 0);

	TriggerMiniSubMist(&pos1, &pos2, 1);
}

void ChaffControl(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos, pos2;
	long x, y, z, xv, yv, zv;

	item = &items[item_number];

	if (item->fallspeed)
	{
		item->pos.x_rot += 546;
		item->pos.z_rot += 910;
	}

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	xv = item->speed * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	zv = item->speed * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
	item->pos.x_pos += xv;
	item->pos.z_pos += zv;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->fallspeed += (5 - item->fallspeed) >> 1;
		item->speed += (5 - item->speed) >> 1;
	}
	else
		item->fallspeed += 6;

	yv = item->fallspeed;
	item->pos.y_pos += yv;
	DoProperDetection(item_number, x, y, z, xv, yv, zv);
	z = GlobalCounter & 1 ? 48 : -48;

	pos.x = 0;
	pos.y = 0;
	pos.z = z;
	GetJointAbsPosition(item, &pos, 0);

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = z << 3;
	GetJointAbsPosition(item, &pos2, 0);

	TriggerMiniSubMist(&pos, &pos2, 1);

	if (item->item_flags[0] < 300)
		item->item_flags[0]++;
	else if (!item->fallspeed && !item->speed)
	{
		if (item->item_flags[1] <= 90)
			item->item_flags[1]++;
		else
			KillItem(item_number);
	}
}

void TriggerMiniSubMist(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff)
{
	SPARKS* sptr;
	uchar size;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 32;
	sptr->sG = 32;
	sptr->sB = 32;
	sptr->dR = 80;
	sptr->dG = 80;
	sptr->dB = 80;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 16;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + pos1->x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos1->y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos1->z - 16;
	sptr->Xvel = short(pos2->x + (GetRandomControl() & 0x7F) - pos1->x - 64);
	sptr->Yvel = short(pos2->y + (GetRandomControl() & 0x7F) - pos1->y - 64);
	sptr->Zvel = short(pos2->z + (GetRandomControl() & 0x7F) - pos1->z - 64);
	sptr->Friction = 0;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 17;
	sptr->MaxYvel = 0;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->Scalar = 1;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	size = (GetRandomControl() & 0xF) + 32;
	sptr->dSize = size << 1;
	sptr->sSize = size >> chaff;
	sptr->Size = sptr->sSize;
}

void InitialiseMinisub(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[ATTACK_SUB].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 0;
	item->goal_anim_state = 0;

	if (!item->trigger_flags)
		item->trigger_flags = 120;
}

void MinisubControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* minisub;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	AI_INFO info;
	AI_INFO lara_info;
	GAME_VECTOR s;
	GAME_VECTOR d;
	long x, y, z;
	short angle, motor, tilt;
	
	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	minisub = (CREATURE_INFO*)item->data;

	if (item->ai_bits)
		GetAITarget(minisub);
	else
		minisub->enemy = lara_item;

	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, minisub->maximum_turn);

	if (minisub->enemy == lara_item)
	{
		lara_info.angle = info.angle;
		lara_info.distance = info.distance;
	}
	else
	{
		x = lara_item->pos.x_pos - item->pos.x_pos;
		z = lara_item->pos.z_pos - item->pos.z_pos;
		lara_info.angle = short(phd_atan(z, x) - item->pos.y_rot);
		lara_info.distance = SQUARE(x) + SQUARE(z);
	}

	lara_info.ahead = 1;
	tilt = item->item_flags[0] + (angle >> 1);

	if (tilt > 2048)
		tilt = 2048;
	else if (tilt < -2048)
		tilt = -2048;

	item->item_flags[0] = tilt;

	if (abs(item->item_flags[0]) < 64)
		item->item_flags[0] = 0;
	else if (item->item_flags[0] < 0)
		item->item_flags[0] += 64;
	else
		item->item_flags[0] -= 64;

	motor = info.x_angle - 0x2000;
	minisub->maximum_turn = 364;

	if (minisub->flags < item->trigger_flags)
		minisub->flags++;

	enemy = minisub->enemy;
	minisub->enemy = lara_item;

	if (!Targetable(item, &lara_info))
		item->goal_anim_state = 1;
	else
	{
		if (minisub->flags >= item->trigger_flags && lara_info.angle > -0x4000 && lara_info.angle < 0x4000)
		{
			FireTorpedo(item);
			minisub->flags = 0;
		}

		if (lara_info.distance < 0x900000)
			item->goal_anim_state = 0;
		else
		{
			item->goal_anim_state = 1;
			SoundEffect(SFX_MINI_SUB_LOOP, &item->pos, SFX_ALWAYS);
		}

		if (info.distance < 0x100000)
		{
			minisub->maximum_turn = 0;

			if (abs(lara_info.angle) < 364)
				item->pos.y_rot += lara_info.angle;
			else if (lara_info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;
		}
	}

	minisub->enemy = enemy;
	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, motor);
	CreatureJoint(item, 1, motor);

	if (GlobalCounter & 1)
	{
		pos1.x = 200;
		pos1.y = 320;
		pos1.z = 90;
		GetJointAbsPosition(item, &pos1, 1);

		pos2.x = 200;
		pos2.y = 1280;
		pos2.z = 90;
		GetJointAbsPosition(item, &pos2, 1);

		TriggerMiniSubMist(&pos1, &pos2, 0);

		pos1.x = 200;
		pos1.y = 320;
		pos1.z = -100;
		GetJointAbsPosition(item, &pos1, 1);

		pos2.x = 200;
		pos2.y = 1280;
		pos2.z = -100;
		GetJointAbsPosition(item, &pos2, 1);

		TriggerMiniSubMist(&pos1, &pos2, 0);
	}

	if (!(GlobalCounter & 1))
	{
		pos1.x = -200;
		pos1.y = 320;
		pos1.z = 90;
		GetJointAbsPosition(item, &pos1, 2);

		pos2.x = -200;
		pos2.y = 1280;
		pos2.z = 90;
		GetJointAbsPosition(item, &pos2, 2);

		TriggerMiniSubMist(&pos1, &pos2, 0);

		pos1.x = -200;
		pos1.y = 320;
		pos1.z = -100;
		GetJointAbsPosition(item, &pos1, 2);

		pos2.x = -200;
		pos2.y = 1280;
		pos2.z = -100;
		GetJointAbsPosition(item, &pos2, 2);

		TriggerMiniSubMist(&pos1, &pos2, 0);
	}
	
	TriggerMinisubLight(item_number);

	s.x = 0;
	s.y = -600;
	s.z = -40;
	s.room_number = item->room_number;
	GetJointAbsPosition(item, (PHD_VECTOR*)&s, 0);

	d.x = 0;
	d.y = -15784;
	d.z = -40;
	GetJointAbsPosition(item, (PHD_VECTOR*)&d, 0);

	if (!LOS(&s, &d))
	{
		x = d.x - s.x;
		y = d.y - s.y;
		z = d.z - s.z;
		z = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));

		if (z < 0x4000)
		{
			z = 0x4000 - z;
			x = (GetRandomControl() & 0xF) + (z >> 7) + 64;
			y = (GetRandomControl() & 1) + (z >> 11) + 12;
			TriggerDynamic(d.x, d.y, d.z, y, x >> 1, x, x >> 1);
		}
	}

	if (minisub->reached_goal && minisub->enemy)
	{
		if (minisub->enemy->flags & 2)
			item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

		item->item_flags[3]++;
		minisub->reached_goal = 0;
		minisub->enemy = 0;
	}

	CreatureAnimation(item_number, angle, tilt);
	CreatureUnderwater(item, -14080);
}

void InitialiseJelly(short item_number)
{
	ITEM_INFO* item;

	InitialiseCreature(item_number);
	item = &items[item_number];
	item->anim_number = objects[REAPER].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 2;
	item->goal_anim_state = 2;
}

void JellyControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* jelly;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	jelly = (CREATURE_INFO*)item->data;

	if (item->ai_bits)
		GetAITarget(jelly);
	else
		jelly->enemy = lara_item;

	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 0);
	CreatureMood(item, &info, 0);
	angle = CreatureTurn(item, 364);

	if (item->current_anim_state == 2 && !(GetRandomControl() & 0x3F))
		item->goal_anim_state = 1;

	if (jelly->reached_goal && jelly->enemy)
	{
		if (jelly->enemy->flags & 2)
			item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

		item->item_flags[3]++;
		jelly->reached_goal = 0;
		jelly->enemy = 0;
	}

	item->pos.x_rot = -0x3000;
	CreatureAnimation(item_number, angle, 0);
	CreatureUnderwater(item, 1024);
}
