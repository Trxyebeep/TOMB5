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
#include "../specific/function_stubs.h"
#include "deltapak.h"
#include "lara.h"

short WB_room;
static ITEM_INFO* WB_item;
static short wbx;
static short wbz;

void KlaxonTremor()
{
	static short timer = 0;
	static short bounce;

	if (!(GlobalCounter & 0x1FF))
		SoundEffect(SFX_KLAXON, 0, 0x1000 | SFX_SETVOL);

	if (timer >= 0)
		timer++;

	if (timer > 450 && !(GetRandomControl() & 0x1FF))
	{
		bounce = 0;
		timer = -32 - (GetRandomControl() & 0x1F);
		return;
	}

	if (timer < 0)
	{
		if (bounce < abs(timer))
		{
			bounce++;
			camera.bounce = -(GetRandomControl() % bounce);
		}
		else
		{
			camera.bounce = -(GetRandomControl() % abs(timer));
			timer++;
		}
	}
}

static long CheckCableBox(PHD_VECTOR* pos, short size)
{
	if (pos->x + size >= DeadlyBounds[0] && pos->x - size <= DeadlyBounds[1] &&
		pos->y + size >= DeadlyBounds[2] && pos->y - size <= DeadlyBounds[3] &&
		pos->z + size >= DeadlyBounds[4] && pos->z - size <= DeadlyBounds[5])
		return 1;

	return 0;
}

void ControlElectricalCables(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long rand, in_water, ripple;
	short room_num, room2_num, wr, flip, ns, ffar;

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
		SoundEffect(SFX_ELECTRIC_WIRES, &item->pos, SFX_DEFAULT);
		ffar = abs(lara_item->pos.x_pos - item->pos.x_pos) > 2048;
		ffar += abs(lara_item->pos.y_pos - item->pos.y_pos) > 4096;
		ffar += abs(lara_item->pos.z_pos - item->pos.z_pos) > 2048;
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

		if (GetRandomControl() & 1 && !ns)
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
		GetLaraJointPos(&pos, LMX_FOOT_L);
		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);

		pos2.x = 0;
		pos2.y = 0;
		pos2.z = 0;
		GetLaraJointPos(&pos2, LMX_FOOT_R);
		room2_num = lara_item->room_number;
		GetFloor(pos2.x, pos2.y, pos2.z, &room2_num);

		if (room[room_num].FlipNumber == flip || room[room2_num].FlipNumber == flip)
		{
			if (lara_item->hit_points > 32)
			{
				SoundEffect(SFX_LARA_ELECTRIC_CRACKLES, &lara_item->pos, SFX_DEFAULT);
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

void WreckingBallCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long x, y, z, dx, dy, dz, middle, damage, lp;

	item = &items[item_num];

	if (!TestBoundsCollide(item, l, coll->radius))
		return;

	dx = l->pos.x_pos;
	dy = l->pos.y_pos;
	dz = l->pos.z_pos;
	middle = (dx & 1023) > 256 && (dx & 1023) < 768 && (dz & 1023) > 256 && (dz & 1023) < 768;
	damage = item->fallspeed > 0 ? 96 : 0;

	if (!ItemPushLara(item, l, coll, coll->enable_spaz, 1))
		return;

	if (middle)
		l->hit_points = 0;
	else
		l->hit_points -= (short)damage;

	dx -= l->pos.x_pos;
	dy -= l->pos.y_pos;
	dz -= l->pos.z_pos;

	if (damage)
	{
		damage = (GetRandomControl() & 3) + (damage >> 3) + 2;

		for (lp = 0; lp < damage; lp++)
		{
			x = l->pos.x_pos + (GetRandomControl() & 63) - 32;
			y = l->pos.y_pos - (GetRandomControl() & 511) - 256;
			z = l->pos.z_pos + (GetRandomControl() & 63) - 32;
			TriggerBlood(x, y, z, -1, 1);
		}
	}

	if (!coll->enable_baddie_push || middle)
	{
		l->pos.x_pos += dx;
		l->pos.y_pos += dy;
		l->pos.z_pos += dz;
	}
}

void ControlWreckingBall(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* baseitem;
	FLOOR_INFO* floor;
	long Tx, Tz, TLara, ox, oz;
	short room_number, Xdiff, Zdiff, speed, xoff, zoff, c;

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
		item->item_flags[2]++;

	if (GLOBAL_playing_cutseq || CheckCutPlayed(27))
	{
		item->goal_anim_state = 0;
		item->pos.x_pos = 47616;
		item->pos.z_pos = 34816;
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 1664;
	}
	else if (item->item_flags[1] <= 0)
	{
		ox = item->pos.x_pos;
		oz = item->pos.z_pos;
		Tx = Tx & ~0x3FF | 0x200;
		Tz = Tz & ~0x3FF | 0x200;
		Xdiff = short(Tx - item->pos.x_pos);
		Zdiff = short(Tz - item->pos.z_pos);
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

		c = (short)baseitem->pos.y_pos;

		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos + xoff, c, item->pos.z_pos, &room_number);
		xoff = (short)GetCeiling(floor, item->pos.x_pos + xoff, c, item->pos.z_pos);

		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, c, item->pos.z_pos + zoff, &room_number);
		zoff = (short)GetCeiling(floor, item->pos.x_pos, c, item->pos.z_pos + zoff);

		if (!item->item_flags[0])
		{
			if (xoff <= c && xoff != NO_HEIGHT && Xdiff && (abs(Xdiff) > abs(Zdiff) || !(zoff <= c && zoff != NO_HEIGHT) || GetRandomControl() & 1))
				item->item_flags[0] = 1;
			else if (zoff <= c && zoff != NO_HEIGHT && Zdiff)
				item->item_flags[0] = 2;
		}

		if (item->item_flags[0] == 1)
		{
			SoundEffect(SFX_J_GRAB_MOTOR_B_LP, &item->pos, SFX_DEFAULT);
			speed = abs(Xdiff);

			if (speed >= 32)
				speed = 32;

			if (Xdiff > 0)
				item->pos.x_pos += speed;
			else if (Xdiff < 0)
				item->pos.x_pos -= speed;
			else
				item->item_flags[0] = 0;
		}

		if (item->item_flags[0] == 2)
		{
			SoundEffect(SFX_J_GRAB_MOTOR_B_LP, &item->pos, SFX_DEFAULT);
			speed = abs(Zdiff);

			if (speed >= 32)
				speed = 32;

			if (Zdiff > 0)
				item->pos.z_pos += speed;
			else if (Zdiff < 0)
				item->pos.z_pos -= speed;
			else
				item->item_flags[0] = 0;
		}

		if (item->item_flags[1] == -1 && (ox != item->pos.x_pos || oz != item->pos.z_pos))
		{
			item->item_flags[1] = 0;
			SoundEffect(SFX_J_GRAB_MOTOR_A, &item->pos, SFX_DEFAULT);
		}

		if ((item->pos.x_pos & 0x3FF) == 512 && (item->pos.z_pos & 0x3FF) == 512)
			item->item_flags[0] = 0;

		if (Tx == item->pos.x_pos && Tz == item->pos.z_pos && TLara)
		{
			if (item->item_flags[1] != -1)
			{
				StopSoundEffect(SFX_J_GRAB_MOTOR_B_LP);
				SoundEffect(SFX_J_GRAB_MOTOR_C, &item->pos, SFX_DEFAULT);
			}

			item->item_flags[1] = 1;
			item->trigger_flags = 30;
		}
	}
	else if (item->item_flags[1] == 1)
	{
		if (item->trigger_flags)
			item->trigger_flags--;
		else if (!item->current_anim_state)
			item->goal_anim_state = 1;
		else if (item->frame_number == anims[item->anim_number].frame_end)
		{
			SoundEffect(SFX_J_GRAB_DROP, &item->pos, SFX_DEFAULT);
			item->item_flags[1]++;
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
		c = (short)GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

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
				item->item_flags[1]++;
				item->fallspeed = 0;
			}
		}
		else if (c - item->pos.y_pos < 1536 && item->current_anim_state)
			item->goal_anim_state = 0;
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
				SoundEffect(SFX_J_GRAB_IMPACT, &item->pos, 4096 | SFX_SETVOL);
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
			SoundEffect(SFX_J_GRAB_WINCH_UP_LP, &item->pos, SFX_DEFAULT);
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

void DrawWreckingBall(ITEM_INFO* item)
{
	ITEM_INFO* baseitem;
	FLOOR_INFO* floor;
	PHD_VECTOR v;
	short* frmptr[2];
	long h, c, shade, y;
	short room_num;

	baseitem = &items[item->item_flags[3]];

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	shade = 192 - (abs(h - item->pos.y_pos) >> 5);

	if (shade < 64)
		shade = 64;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, h, item->pos.z_pos);
	aDrawWreckingBall(item, shade);							//Shadow

	if (item->pos.y_pos - baseitem->pos.y_pos != 1664)		//Chain
	{
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_bottom = phd_winheight;
		GetFrames(item, frmptr, &c);
		
		phd_PushMatrix();
		phd_TranslateAbs(baseitem->pos.x_pos, baseitem->pos.y_pos + 512, baseitem->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

		y = item->pos.y_pos - baseitem->pos.y_pos - 1664;
		y += (21846 * y) >> 16;
		v.z = 0x4000;
		v.y = y << 2;
		v.x = 0x4000;
		ScaleCurrentMatrix(&v);

		CalculateObjectLighting(item, *frmptr);
		phd_PutPolygons(meshes[objects[ANIMATING16_MIP].mesh_index], -1);

		phd_bottom = phd_winheight;
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_PopMatrix();
	}
}

void ControlSecurityScreens(short item_number)
{
	ITEM_INFO* item;
	short mb1;

	item = &items[item_number];

	if (TriggerActive(item) && item->trigger_flags != -1)
	{
		item->item_flags[0] = (item->item_flags[0] + 1) & 0xFF;

		if (!item->item_flags[0])
		{
			mb1 = (item->item_flags[1] + 1) & 3;
			item->item_flags[1] = mb1;
			item->mesh_bits = (32 << mb1) + (2 << mb1);
		}
	}
}

void CookerFlameControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (!lara.burn &&
			abs(lara_item->pos.x_pos - item->pos.x_pos) < 256 &&
			abs(lara_item->pos.z_pos - item->pos.z_pos) < 256 &&
			item->pos.y_pos - lara_item->pos.y_pos < 128)
			LaraBurn();

		item->item_flags[0] = (GetRandomControl() + item->item_flags[0]) & 0x1FF;
		item->item_flags[1] = item->item_flags[0] + 4096;
	}
}

void TriggerLaraSparks(long smoke)
{
	SPARKS* sptr;
	PHD_VECTOR pos;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetLaraJointPos(&pos, GetRandomControl() % 15);

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sR;
	sptr->dR = 0;
	sptr->dG = sptr->sR >> 1;
	sptr->dB = sptr->sR;
	sptr->TransType = 2;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->Life = 12;
	sptr->sLife = 12;
	sptr->x = pos.x;
	sptr->y = pos.y;
	sptr->z = pos.z;
	sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = SF_NONE;

	if (smoke)
		TriggerFireFlame(pos.x, pos.y, pos.z, -1, 254);
}

void TriggerCableSparks(long x, long z, short item_number, long node, long flare)
{
	SPARKS* sptr;
	uchar n;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = 0;
	sptr->dG = (GetRandomControl() & 0x7F) + 64;
	sptr->dB = 255;

	if (flare)
	{
		sptr->ColFadeSpeed = 1;
		sptr->FadeToBlack = 0;
		n = 4;
	}
	else
	{
		sptr->ColFadeSpeed = 3;
		sptr->FadeToBlack = 4;
		n = 16;
	}

	sptr->Life = n;
	sptr->sLife = n;
	sptr->FxObj = (uchar)item_number;
	sptr->TransType = 2;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_DEF | SF_SCALE;
	sptr->NodeNumber = (uchar)node;
	sptr->x = x;
	sptr->y = 0;
	sptr->z = z;

	if (flare)
	{
		sptr->Xvel = 0;
		sptr->Yvel = 0;
		sptr->Zvel = 0;
	}
	else
	{
		sptr->Xvel = (GetRandomControl() & 0x1FF) - 256;
		sptr->Yvel = (GetRandomControl() & 0xFF) - 64;
		sptr->Zvel = (GetRandomControl() & 0x1FF) - 256;
	}

	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	if (flare)
	{
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
		sptr->Scalar = 1;
		n = (GetRandomControl() & 0x1F) + 160;
	}
	else
	{
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
		sptr->Scalar = 0;
		n = (GetRandomControl() & 7) + 8;
	}

	sptr->Size = n;
	sptr->sSize = n;
	sptr->dSize = n >> 1;
}
