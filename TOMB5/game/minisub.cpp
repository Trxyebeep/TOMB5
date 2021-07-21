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

void TriggerTorpedoSteam(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff)
{
	SPARKS* sptr;
	uchar size;

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
	sptr->Life = (uchar)((GetRandomControl() & 7) + 16);
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + pos1->x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos1->y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos1->z - 16;
	sptr->Xvel = (short)(pos2->x + (GetRandomControl() & 0x7F) - pos1->x - 64);
	sptr->Yvel = (short)(pos2->y + (GetRandomControl() & 0x7F) - pos1->y - 64);
	sptr->Zvel = (short)(pos2->z + (GetRandomControl() & 0x7F) - pos1->z - 64);
	sptr->Friction = 51;
	sptr->Gravity = (short)(-4 - (GetRandomControl() & 3));
	sptr->MaxYvel = 0;
	sptr->Scalar = (uchar)(2 - chaff);
	sptr->Flags = 538;
	sptr->RotAng = (short)(GetRandomControl() & 0xFFF);
	sptr->RotAdd = (char)((GetRandomControl() & 0x3F) - 32);
	size = (GetRandomControl() & 0xF) + 32;
	sptr->sSize = size;
	sptr->Size = size;
	sptr->dSize = size << 1;
}

void TriggerMinisubLight(short item_number)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = -1;
	sptr->sG = -1;
	sptr->sB = -1;
	sptr->dG = (GetRandomControl() & 0x1F) - 32;
	sptr->dR = sptr->dG >> 1;
	sptr->dB = sptr->dG >> 1;
	sptr->ColFadeSpeed = 2;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->FadeToBlack = 0;
	sptr->Flags = 0x50AA;
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
	sptr->dSize = (GetRandomControl() & 7) - 64;
	sptr->sSize = sptr->dSize;
	sptr->Size = sptr->sSize;
}

void FireTorpedo(ITEM_INFO* src)
{
	ITEM_INFO* item;
	PHD_VECTOR pos, pos2;
	short item_number;

	item_number = CreateItem();

	if (item_number != NO_ITEM)
	{
		item = &items[item_number];
		SoundEffect(SFX_UNDERWATER_TORPEDO, &item->pos, 2);
		item->object_number = TORPEDO;
		item->shade = -15856;

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
}

void TorpedoControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	ITEM_INFO* chaff;
	FLOOR_INFO* floor;
	PHD_VECTOR target;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	long x, y, z, height, ceiling;
	short angles[2];
	short counter, found, room_number, speed;

	item = &items[item_number];
	SoundEffect(SFX_SWIMSUIT_METAL_CLASH, &item->pos, 2);

	if (item->item_flags[0] == NO_ITEM)
	{
		for (counter = level_items, found = 0; counter < 256; counter++)
		{
			chaff = &items[counter];

			if (chaff->object_number == CHAFF && chaff->active)
			{
				item->item_flags[0] = counter;
				target.x = chaff->pos.x_pos;
				target.y = chaff->pos.y_pos;
				target.z = chaff->pos.z_pos;
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
			item->current_anim_state = (short)(target.x >> 2);
			item->goal_anim_state = (short)(target.y >> 2);
			item->required_anim_state = (short)(target.z >> 2);
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

	if (item->item_flags[1]++ < 60)
	{
		y = (ushort)angles[0] - (ushort)item->pos.y_rot;

		if (ABS(y) > 32768)
			y = (ushort)item->pos.y_rot - (ushort)angles[0];

		x = (ushort)angles[1] - (ushort)item->pos.x_rot;

		if (ABS(x) > 32768)
			x = (ushort)item->pos.x_rot - (ushort)angles[0];

		y >>= 3;
		x >>= 3;

		if (y > 512)
			y = 512;
		else if (y < -512)
			y = -512;

		if (x > 512)
			x = 512;
		else if (x < -512)
			x = -512;

		item->pos.y_rot += (short)y;
		item->pos.x_rot += (short)x;
	}

	speed = item->speed;
	item->pos.z_rot += speed << 4;
	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	speed = item->speed * phd_cos(item->pos.x_rot) >> 14;
	item->pos.x_pos += (speed * phd_sin(item->pos.y_rot) >> 14);
	item->pos.y_pos += (item->speed * phd_sin(-item->pos.x_rot) >> 14);
	item->pos.z_pos += (speed * phd_cos(item->pos.y_rot) >> 14);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < height && item->pos.y_pos > ceiling && room[room_number].flags & ROOM_UNDERWATER)
	{
		if (ItemNearLara(&item->pos, 200))
		{
			lara_item->hit_status = 1;
			KillItem(item_number);
			TriggerUnderwaterExplosion(item);
			SoundEffect(397, &item->pos, 2);
			SoundEffect(401, &lara_item->pos, 2);
			lara_item->hit_points -= 200;

			if (lara.Anxiety >= 127)
				lara.Anxiety--;//what
			else
				lara.Anxiety += 128;
		}
		else
		{
			if (ItemNearLara(&item->pos, 400) && lara.Anxiety < 224)
				lara.Anxiety += 32;

			if (room_number != item->room_number)
				ItemNewRoom(item_number, room_number);

			z = (GlobalCounter & 1) + 2;
			pos1.x = 0;
			pos1.y = 0;
			pos1.z = -64;
			GetJointAbsPosition(item, &pos1, 0);
			pos2.x = 0;
			pos2.y = 0;
			pos2.z = -64 << z;
			GetJointAbsPosition(item, &pos2, 0);
			TriggerMiniSubMist(&pos1, &pos2, 1);
		}
	}
	else
	{
		item->pos.x_pos = x;
		item->pos.y_pos = y;
		item->pos.z_pos = z;
		TriggerUnderwaterExplosion(item);
		SoundEffect(SFX_UNDERWATER_EXPLOSION, &item->pos, SFX_ALWAYS);
		KillItem(item_number);
	}
}

void ChaffControl(short item_number)
{
	ITEM_INFO* flare;
	PHD_VECTOR pos, pos2;
	long x, y, z, xv, yv, zv;

	flare = &items[item_number];

	if (flare->fallspeed)
	{
		flare->pos.x_rot += 546;
		flare->pos.z_rot += 910;
	}

	x = flare->pos.x_pos;
	y = flare->pos.y_pos;
	z = flare->pos.z_pos;
	xv = flare->speed * phd_sin(flare->pos.y_rot) >> 14;
	zv = flare->speed * phd_cos(flare->pos.y_rot) >> 14;
	flare->pos.x_pos += xv;
	flare->pos.z_pos += zv;

	if (room[flare->room_number].flags & ROOM_UNDERWATER)
	{
		flare->fallspeed += (5 - flare->fallspeed) >> 1;
		flare->speed += (5 - flare->speed) >> 1;
	}
	else
		flare->fallspeed += 6;

	yv = flare->fallspeed;
	flare->pos.y_pos += yv;
	DoProperDetection(item_number, x, y, z, xv, yv, zv);
	z = GlobalCounter & 1 ? 48 : -48;
	pos.x = 0;
	pos.y = 0;
	pos.z = z;
	GetJointAbsPosition(flare, &pos, 0);
	pos2.x = 0;
	pos2.y = 0;
	pos2.z = z << 3;
	GetJointAbsPosition(flare, &pos2, 0);
	TriggerMiniSubMist(&pos, &pos2, 1);

	if (flare->item_flags[0] >= 300)
	{
		if (!flare->fallspeed && !flare->speed)
		{
			if (flare->item_flags[1] <= 90)
				flare->item_flags[1]++;
			else
				KillItem(item_number);
		}
	}
	else
		flare->item_flags[0]++;
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
	sptr->Xvel = (short)(pos2->x + (GetRandomControl() & 0x7F) - pos1->x - 64);
	sptr->Yvel = (short)(pos2->y + (GetRandomControl() & 0x7F) - pos1->y - 64);
	sptr->Zvel = (short)(pos2->z + (GetRandomControl() & 0x7F) - pos1->z - 64);
	sptr->Friction = 0;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 17;
	sptr->MaxYvel = 0;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->Scalar = 1;
	sptr->Flags = 26;
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
	item->goal_anim_state = 0;
	item->current_anim_state = 0;

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
	long lara_dx, lara_dz, dx, dy, dz;
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
#ifdef GENERAL_FIXES//fixes dumb bug of the minisub not shooting that depends on luck and a little bit of idiocy from whoever wrote this function
		lara_info.ahead = 1;
#endif
	}
	else
	{
		lara_dz = lara_item->pos.z_pos - item->pos.z_pos;
		lara_dx = lara_item->pos.x_pos - item->pos.x_pos;
		lara_info.angle = (short)(phd_atan(lara_dz, lara_dx) - item->pos.y_rot);
		lara_info.distance = SQUARE(lara_dx) + SQUARE(lara_dz);
		lara_info.ahead = 1;
	}

	tilt = item->item_flags[0] + (angle >> 1);

	if (tilt > 2048)
		tilt = 2048;
	else if (tilt < -2048)
		tilt = -2048;

	item->item_flags[0] = tilt;

	if (ABS(item->item_flags[0]) < 64)
		item->item_flags[0] = 0;
	else
	{
		if (item->item_flags[0] < 0)
			item->item_flags[0] += 64;
		else
			item->item_flags[0] -= 64;
	}

	motor = info.x_angle - 8192;
	minisub->maximum_turn = 364;

	if (minisub->flags < item->trigger_flags)
		minisub->flags++;

	enemy = minisub->enemy;
	minisub->enemy = lara_item;

	if (Targetable(item, &lara_info))
	{
		if (minisub->flags >= item->trigger_flags && lara_info.angle > -16384 && lara_info.angle < 16384)
		{
			FireTorpedo(item);
			minisub->flags = 0;
		}

		if (lara_info.distance >= 9437184)
		{
			item->goal_anim_state = 1;
			SoundEffect(SFX_MINI_SUB_LOOP, &item->pos, SFX_ALWAYS);
		}
		else
			item->goal_anim_state = 0;

		if (info.distance < 1048576)
		{
			minisub->maximum_turn = 0;

			if (ABS(lara_info.angle) >= 364)
			{
				if (lara_info.angle >= 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;
			}
			else
				item->pos.y_rot += lara_info.angle;
		}
	}
	else
		item->goal_anim_state = 1;

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
		dx = d.x - s.x;
		dy = d.y - s.y;
		dz = d.z - s.z;
		dz = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

		if (dz < 16384)
		{
			dx = (GetRandomControl() & 0xF) + ((16384 - dz) >> 7) + 64;
			TriggerDynamic(d.x, d.y, d.z, (GetRandomControl() & 1) + ((16384 - dz) >> 11) + 12, dx >> 1, dx, dx >> 1);
		}
	}

	if (minisub->reached_goal)
	{
		if (minisub->enemy)
		{
			if (minisub->enemy->flags & 2)
				item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

			item->item_flags[3]++;
			minisub->reached_goal = 0;
			minisub->enemy = 0;
		}
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
	item->goal_anim_state = 2;
	item->current_anim_state = 2;
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

	if (jelly->reached_goal)
	{
		if (jelly->enemy)
		{
			if (jelly->enemy->flags & 2)
				item->item_flags[3] = (item->TOSSPAD & 0xFF) - 1;

			item->item_flags[3]++;
			jelly->reached_goal = 0;
			jelly->enemy = 0;
		}
	}

	item->pos.x_rot = -12288;
	CreatureAnimation(item_number, angle, 0);
	CreatureUnderwater(item, 1024);
}

void inject_minisub(bool replace)
{
	INJECT(0x0045C5E0, TriggerTorpedoSteam, replace);
	INJECT(0x0045C760, TriggerMinisubLight, replace);
	INJECT(0x0045C830, FireTorpedo, replace);
	INJECT(0x0045C9F0, TorpedoControl, replace);
	INJECT(0x0045CFB0, ChaffControl, replace);
	INJECT(0x0045D1D0, TriggerMiniSubMist, replace);
	INJECT(0x0045D360, InitialiseMinisub, replace);
	INJECT(0x0045D3F0, MinisubControl, replace);
	INJECT(0x0045DA70, InitialiseJelly, replace);
	INJECT(0x0045DAF0, JellyControl, replace);
}
