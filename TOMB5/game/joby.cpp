#include "../tomb5/pch.h"
#include "joby.h"
#include "sound.h"
#include "control.h"
#include "effect2.h"
#include "laramisc.h"
#include "sphere.h"
#include "traps.h"
#include "delstuff.h"
#include "collide.h"
#include "tomb4fx.h"
#include "items.h"
#include "objlight.h"
#include "camera.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "objects.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"

void KlaxonTremor()
{
	static short timer;

	if (!(GlobalCounter & 0x1FF))
		SoundEffect(SFX_KLAXON, 0, 0x1000 | SFX_SETVOL);

	if (timer >= 0)
		timer++;

	if (timer > 450)
	{
		if (!(GetRandomControl() & 0x1FF))
		{
			InGameCnt = 0;
			timer = -32 - (GetRandomControl() & 0x1F);
			return;
		}
	}

	if (timer < 0)
	{
		if ((signed int)InGameCnt >= ABS(timer))
		{
			camera.bounce = -(GetRandomControl() % ABS(timer));
			++timer;
		}
		else
			camera.bounce = -(GetRandomControl() % ++InGameCnt);
	}
}

static int CheckCableBox(PHD_VECTOR* pos, short size)
{
	int ret;
	
	ret = 0;

	if (pos->x + size >= DeadlyBounds[0] && pos->x - size <= DeadlyBounds[1])
	{
		if (pos->y + size >= DeadlyBounds[2] && pos->y - size <= DeadlyBounds[3])
		{
			if (pos->z + size >= DeadlyBounds[4] && pos->z - size <= DeadlyBounds[5])
				ret = 1;
		}
	}

	return ret;
}

void ControlElectricalCables(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long rand, in_water, ripple;
	short room_num, room2_num, wr, flip;
	short ns, ffar;

	item = &items[item_number];
	ns = 0;
	ffar = 3;

	if (item->item_flags[0] > 2)
	{
		rand = ((GetRandomControl() & 0x1F) + 8) * item->item_flags[0];
		TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, item->item_flags[0], 0, rand, rand);
		item->item_flags[0] -= 2;
	}

	if (TriggerActive(item))
	{
		SoundEffect(SFX_ELECTRIC_WIRES, &item->pos, 0);
		ffar = ABS(lara_item->pos.x_pos - item->pos.x_pos) > 2048;
		ffar += ABS(lara_item->pos.y_pos - item->pos.y_pos) > 4096;
		ffar += ABS(lara_item->pos.z_pos - item->pos.z_pos) > 2048;
		rand = (GetRandomControl() & 0x1F) - 16;

		for (int i = 0; i < 3; i++)
		{
			if (GetRandomControl() & 1)
				TriggerCableSparks(rand, rand, item_number, i + 2, 0);
		}

		if (!(GlobalCounter & 3))
		{
			TriggerCableSparks(0, 0, item_number, 2, 1);
			TriggerCableSparks(0, 0, item_number, 3, 1);
			TriggerCableSparks(0, 0, item_number, 4, 1);
		}
	}
	else
		ns = 1;

	AnimateItem(item);

	if (!lara.burn && !ns && !ffar)
	{
		GetLaraDeadlyBounds();

		for (int i = 2; i < 27; i += 3)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, i);

			if (CheckCableBox(&pos, item->trigger_flags))
			{
				for (int j = 0; j < 48; j++)
					TriggerLaraSparks(0);

				item->item_flags[0] = 28;
				LaraBurn();
				lara.BurnCount = 48;
				lara.BurnBlue = 1;
				lara_item->hit_points = 0;
				return;
			}
		}
	}

	in_water = 0;
	wr = 0;
	ripple = GlobalCounter & 3;

	for (int i = 0, j = 8; j < 27; i++, j += 9)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 256;
		GetJointAbsPosition(item, &pos, j);

		if ((GetRandomControl() & 1) && !ns)
		{
			rand = (GetRandomControl() & 63) + 128;
			TriggerDynamic(pos.x, pos.y, pos.z, 12, 0, rand >> 1, rand);
		}

		room_num = item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);

		if (room[room_num].flags & ROOM_UNDERWATER)
		{
			if (ripple == i)
				SetupRipple(pos.x, room[room_num].maxceiling, pos.z, 32 + (GetRandomControl() & 7), 16);

			wr = room_num;
			in_water = 1;
		}
	}

	if (!ns && !lara.burn && in_water)
	{
		flip = room[wr].FlipNumber;

		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, 3);
		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);

		pos2.x = 0;
		pos2.y = 0;
		pos2.z = 0;
		GetLaraJointPos(&pos2, 6);
		room2_num = lara_item->room_number;
		GetFloor(pos2.x, pos2.y, pos2.z, &room2_num);

		if (room[room_num].FlipNumber == flip || room[room2_num].FlipNumber == flip)
		{
			if (lara_item->hit_points > 32)
			{
				SoundEffect(SFX_LARA_ELECTRIC_CRACKLES, &lara_item->pos, 0);
				TriggerLaraSparks(0);
				TriggerLaraSparks(1);
				TriggerDynamic(pos.x, pos.y, pos.z, 8, 0, GetRandomControl() & 127, (GetRandomControl() & 63) + 128);
				lara_item->hit_points -= 10;
				return;
			}

			item->item_flags[0] = 28;
			LaraBurn();
			lara.BurnCount = 48;
			lara.BurnBlue = 1;
			lara_item->hit_points = 0;
		}
	}
}

void WreckingBallCollision(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long dx, dy, dz;
	long middle, damage, lp;

	item = &items[item_num];

	if (TestBoundsCollide(item, laraitem, coll->radius))
	{
		dx = laraitem->pos.x_pos;
		dy = laraitem->pos.y_pos;
		dz = laraitem->pos.z_pos;
		middle = (dx & 1023) > 256 && (dx & 1023) < 768 && (dz & 1023) > 256 && (dz & 1023) < 768;
		damage = item->fallspeed > 0 ? 96 : 0;

		if (ItemPushLara(item, laraitem, coll, coll->enable_spaz, 1))
		{
			if (middle)
				laraitem->hit_points = 0;
			else
				laraitem->hit_points -= (short) damage;

			dx -= laraitem->pos.x_pos;
			dy -= laraitem->pos.y_pos;
			dz -= laraitem->pos.z_pos;

			if (damage)
			{
				for (lp = 14 + (GetRandomControl() & 3); lp > 0; --lp)
					TriggerBlood(laraitem->pos.x_pos + (GetRandomControl() & 63) - 32, laraitem->pos.y_pos - (GetRandomControl() & 511) - 256, laraitem->pos.z_pos + (GetRandomControl() & 63) - 32, -1, 1);
			}

			if (!coll->enable_baddie_push || middle)
			{
				laraitem->pos.x_pos += dx;
				laraitem->pos.y_pos += dy;
				laraitem->pos.z_pos += dz;
			}
		}
	}
}

void ControlWreckingBall(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* baseitem;
	FLOOR_INFO* floor;
	short room_number, Xdiff, Zdiff, speed, xoff, zoff, c;
	long Tx, Tz, TLara, ox, oz;

	item = &items[item_number];
	TLara = 1;
	baseitem = &items[item->item_flags[3]];

	if (lara_item->pos.x_pos >= 45056 && lara_item->pos.x_pos <= 57344 && lara_item->pos.z_pos >= 26624 && lara_item->pos.z_pos <= 43008 || item->item_flags[2] < 900)
	{
		if (item->item_flags[2] < 900)
		{
			if (!item->item_flags[2] || !(GlobalCounter & 0x3F))
			{
				wbx = GetRandomControl() % 7 - 3;
				wbz = GetRandomControl() % 7 - 3;
			}

			Tx = 1024 * wbx + 51712;
			Tz = 1024 * wbz + 34304;
			TLara = 0;
		}
		else
		{
			Tx = lara_item->pos.x_pos;
			Tz = lara_item->pos.z_pos;
		}
	}
	else
	{
		Tx = 51200;
		Tz = 33792;
		TLara = 0;
	}

	if (item->item_flags[2] < 900)
		++item->item_flags[2];

	if (GLOBAL_playing_cutseq || CheckCutPlayed(27))
	{
		room_number = item->room_number;
		item->goal_anim_state = 0;
		item->pos.x_pos = 47616;
		item->pos.z_pos = 34816;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 1664;
	}
	else if (item->item_flags[1] <= 0)
	{
		ox = item->pos.x_pos;
		oz = item->pos.z_pos;
		Tx = Tx & 0xFFFFFE00 | 0x200;
		Tz = Tz & 0xFFFFFE00 | 0x200;
		Xdiff = (short) (Tx - item->pos.x_pos);
		Zdiff = (short) (Tz - item->pos.z_pos);
		xoff = 0;

		if (Xdiff < 0)
			xoff = -1024;
		else if (Xdiff > 0)
			xoff = 1024;

		zoff = 0;

		if (Zdiff < 0)
			zoff = -1024;
		else if (Zdiff > 0)
			zoff = 1024;

		c = (short) baseitem->pos.y_pos;
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos + xoff, c, item->pos.z_pos, &room_number);
		xoff = (short) GetCeiling(floor, item->pos.x_pos + xoff, c, item->pos.z_pos);
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, c, item->pos.z_pos + zoff, &room_number);
		zoff = (short) GetCeiling(floor, item->pos.x_pos, c, item->pos.z_pos + zoff);

		if (!item->item_flags[0])
		{
			if (xoff <= c && xoff != NO_HEIGHT && Xdiff && (ABS(Xdiff) > ABS(Zdiff) || !(zoff <= c && zoff != NO_HEIGHT) || GetRandomControl() & 1))
				item->item_flags[0] = 1;
			else if (zoff <= c && zoff != NO_HEIGHT && Zdiff)
				item->item_flags[0] = 2;
		}

		if (item->item_flags[0] == 1)
		{
			SoundEffect(SFX_J_GRAB_MOTOR_B_LP, &item->pos, 0);
			speed = ABS(Xdiff);

			if (speed >= 32)
				speed = 32;

			if (Xdiff > 0)
			{
				item->pos.x_pos += speed;
			}
			else if (Xdiff < 0)
			{
				item->pos.x_pos -= speed;
			}
			else
			{
				item->item_flags[0] = 0;
			}
		}

		if (item->item_flags[0] == 2)
		{
			SoundEffect(SFX_J_GRAB_MOTOR_B_LP, &item->pos, 0);
			speed = ABS(Zdiff);

			if (speed >= 32)
				speed = 32;

			if (Zdiff > 0)
			{
				item->pos.z_pos += speed;
			}
			else if (Zdiff < 0)
			{
				item->pos.z_pos -= speed;
			}
			else
			{
				item->item_flags[0] = 0;
			}
		}

		if (item->item_flags[1] == -1 && (ox != item->pos.x_pos || oz != item->pos.z_pos))
		{
			item->item_flags[1] = 0;
			SoundEffect(SFX_J_GRAB_MOTOR_A, &item->pos, 0);
		}

		if ((item->pos.x_pos & 0x3FF) == 512 && (item->pos.z_pos & 0x3FF) == 512)
			item->item_flags[0] = 0;

		if (Tx == item->pos.x_pos && Tz == item->pos.z_pos && TLara)
		{
			if (item->item_flags[1] != -1)
			{
				StopSoundEffect(SFX_J_GRAB_MOTOR_B_LP);
				SoundEffect(SFX_J_GRAB_MOTOR_C, &item->pos, 0);
			}

			item->item_flags[1] = 1;
			item->trigger_flags = 30;
		}
	}
	else if (item->item_flags[1] == 1)
	{
		if (!item->trigger_flags)
		{
			--item->trigger_flags;
		}
		else if (!item->current_anim_state)
		{
			item->goal_anim_state = 1;
		}
		else if (item->frame_number == anims[item->anim_number].frame_end)
		{
			SoundEffect(SFX_J_GRAB_DROP, &item->pos, 0);
			++item->item_flags[1];
			item->fallspeed = 6;
			item->pos.y_pos += item->fallspeed;
		}
	}
	else if (item->item_flags[1] == 2)
	{
		item->fallspeed += 24;
		item->pos.y_pos += item->fallspeed;
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		c = (short) GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (c < item->pos.y_pos)
		{
			item->pos.y_pos = c;

			if (item->fallspeed > 48)
			{
				ScreenShake(item, 64, 8192);
				item->fallspeed = -item->fallspeed >> 3;
			}
			else
			{
				++item->item_flags[1];
				item->fallspeed = 0;
			}
		}
		else if (c - item->pos.y_pos < 1536 && item->current_anim_state)
		{
			item->goal_anim_state = 0;
		}
	}
	else if (item->item_flags[1] == 3)
	{
		item->fallspeed -= 3;
		item->pos.y_pos += item->fallspeed;

		if (item->pos.y_pos < baseitem->pos.y_pos + 1644)
		{
			StopSoundEffect(SFX_J_GRAB_WINCH_UP_LP);
			item->item_flags[0] = 1;
			item->pos.y_pos = baseitem->pos.y_pos + 1644;

			if (item->fallspeed < -32)
			{
				SoundEffect(SFX_J_GRAB_IMPACT, &item->pos, 4104);
				item->fallspeed = -item->fallspeed >> 3;
				ScreenShake(item, 16, 8192);
			}
			else
			{
				item->item_flags[1] = -1;
				item->fallspeed = 0;
				item->item_flags[0] = 0;
			}
		}
		else if (!item->item_flags[0])
		{
			SoundEffect(SFX_J_GRAB_WINCH_UP_LP, &item->pos, 0);
		}
	}

	baseitem->pos.x_pos = item->pos.x_pos;
	baseitem->pos.z_pos = item->pos.z_pos;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	baseitem->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	GetFloor(baseitem->pos.x_pos, baseitem->pos.y_pos, baseitem->pos.z_pos, &room_number);

	if (room_number != baseitem->room_number)
		ItemNewRoom(item->item_flags[3], room_number);

	TriggerAlertLight(baseitem->pos.x_pos, baseitem->pos.y_pos + 64, baseitem->pos.z_pos, 255, 64, 0, 64 * (GlobalCounter & 0x3F), baseitem->room_number, 24);
	TriggerAlertLight(baseitem->pos.x_pos, baseitem->pos.y_pos + 64, baseitem->pos.z_pos, 255, 64, 0, 64 * (GlobalCounter - 32) & 0xFFF, baseitem->room_number, 24);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	AnimateItem(item);
	WB_item = item;
	WB_room = room_number;
}

void DrawWreckingBall(ITEM_INFO* item)//actually only draws the shadow it seems?
{
	ITEM_INFO* baseitem;
	FLOOR_INFO* floor;
	VECTOR v;
	short** meshpp;
	short* frmptr[2];
	int height, ceiling, shade, y;
	short room_num;

	baseitem = &items[item->item_flags[3]];
	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	shade = 192 - (ABS(height - item->pos.y_pos) >> 5);

	if (shade < 64)
		shade = 64;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, height, item->pos.z_pos);
	aDrawWreckingBall(item, shade);
	if (item->pos.y_pos - baseitem->pos.y_pos != 1664)
	{
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_bottom = phd_winheight;
		GetFrames(item, frmptr, &ceiling);
		phd_PushMatrix();
		phd_TranslateAbs(baseitem->pos.x_pos, baseitem->pos.y_pos + 512, baseitem->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		meshpp = &meshes[objects[ANIMATING16_MIP].mesh_index];
		y = item->pos.y_pos - baseitem->pos.y_pos - 1664;
		v.vz = 16384;
		v.vy = 4 * (y + ((21846 * y) >> 16));
		v.vx = 16384;
		ScaleCurrentMatrix(&v);
		CalculateObjectLighting(item, *frmptr);
		phd_PutPolygons(*meshpp, -1);
		phd_bottom = phd_winheight;
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_PopMatrix();
	}
}

void inject_joby(bool replace)
{
	INJECT(0x00442C90, KlaxonTremor, replace);
	INJECT(0x00442BE0, CheckCableBox, replace);
	INJECT(0x00442610, ControlElectricalCables, replace);
	INJECT(0x00441D50, WreckingBallCollision, replace);
	INJECT(0x00441410, ControlWreckingBall, replace);
	INJECT(0x00441F50, DrawWreckingBall, replace);
}
