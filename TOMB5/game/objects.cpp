#include "../tomb5/pch.h"
#include "objects.h"
#include "gameflow.h"
#include "sphere.h"
#include "control.h"
#include "items.h"
#include "sound.h"
#include "collide.h"
#include "../specific/3dmath.h"
#include "switch.h"
#include "debris.h"
#include "lara_states.h"
#include "delstuff.h"
#include "tomb4fx.h"
#include "twogun.h"
#include "skeleton.h"
#include "tower.h"
#include "../specific/function_stubs.h"
#include "../specific/output.h"
#include "camera.h"
#include "../specific/input.h"
#include "../specific/file.h"
#include "lara.h"
#include "box.h"

OBJECT_INFO objects[NUMBER_OBJECTS];

static BITE_INFO EnemyBites[9] =
{
	{ 20, -95, 240, 13 },
	{ 48, 0, 180, -11 },
	{ -48, 0, 180, 14 },
	{ -55, 5, 225, 14 },
	{ 15, -60, 195, 13 },
	{ -30, -65, 250, 18 },
	{ 0, -110, 480, 13 },
	{ -20, -80, 190, -10 },
	{ 10, -60, 200, 13 }
};

static short ParallelBarsBounds[12] = { -640, 640, 704, 832, -96, 96, -1820, 1820, -5460, 5460, -1820, 1820 };
static short PoleBounds[12] = { -256, 256, 0, 0, -512, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short TightRopeBounds[12] = { -256, 256, 0, 0, -256, 256, -1820, 1820, -5460, 5460, -1820, 1820 };

static PHD_VECTOR PolePos = { 0, 0, -208 };
static PHD_VECTOR PolePosR = { 0, 0, 0 };
static PHD_VECTOR TightRopePos = { 0, 0, 0 };

void EarthQuake(short item_number)
{
	ITEM_INFO* item;
	long lp;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags == 888)
	{
		camera.bounce = -64 - (GetRandomControl() & 0x1F);
		SoundEffect(SFX_EARTHQUAKE_LOOP, 0, SFX_DEFAULT);
		item->item_flags[3]++;

		if (item->item_flags[3] > 150)
		{
			SoundEffect(SFX_J_GRAB_IMPACT, 0, SFX_DEFAULT);
			KillItem(item_number);
		}

		return;
	}

	if (item->trigger_flags == 333)
	{
		if (item->item_flags[0] < 495)
		{
			SoundEffect(SFX_EARTHQUAKE_LOOP, 0, SFX_DEFAULT);
			item->item_flags[0]++;
		}
		else
			KillItem(item_number);

		return;
	}

	if (!item->item_flags[1])
		item->item_flags[1] = 100;

	if (!item->item_flags[2] && abs(item->item_flags[0] - item->item_flags[1]) < 16)
	{

		if (item->item_flags[1] == 20)
		{
			item->item_flags[1] = 100;
			item->item_flags[2] = (GetRandomControl() & 0x7F) + 90;
		}
		else
		{
			item->item_flags[1] = 20;
			item->item_flags[2] = (GetRandomControl() & 0x7F) + 30;
		}
	}

	if (item->item_flags[2])
		item->item_flags[2]--;

	if (item->item_flags[0] <= item->item_flags[1])
		item->item_flags[0] += (GetRandomControl() & 7) + 2;
	else
		item->item_flags[0] -= (GetRandomControl() & 7) + 2;

	SoundEffect(SFX_EARTHQUAKE_LOOP, 0, (0x1000000 + (item->item_flags[0] << 16)) | SFX_SETPITCH);
	camera.bounce = -item->item_flags[0];

	if (GetRandomControl() < 1024)
	{
		for (lp = room[item->room_number].item_number; lp != NO_ITEM; lp = item->next_item)
		{
			item = &items[lp];

			if (item->object_number == FLAME_EMITTER && item->status != ITEM_ACTIVE && item->status != ITEM_DEACTIVATED)
			{
				AddActiveItem((short)lp);
				item->status = ITEM_ACTIVE;
				item->timer = 0;
				item->flags |= IFL_CODEBITS;
				break;
			}
		}
	}
}

void SmashObject(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	BOX_INFO* box;

	item = &items[item_number];
	r = &room[item->room_number];
	box = &boxes[r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].box];

	if (box->overlap_index & 0x8000)
		box->overlap_index &= 0xBF00;

	SoundEffect(SFX_SMASH_GLASS, &item->pos, SFX_DEFAULT);
	item->collidable = 0;
	item->mesh_bits = 0xFFFE;
	ExplodingDeath2(item_number, -1, 257);
	item->flags |= IFL_INVISIBLE;

	if (item->status == ITEM_ACTIVE)
		RemoveActiveItem(item_number);

	item->status = ITEM_DEACTIVATED;
}

void SmashObjectControl(short item_number)
{
	SmashObject(item_number);
}

void BridgeFlatFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (item->pos.y_pos >= y)
	{
		*height = item->pos.y_pos;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeFlatCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (item->pos.y_pos < y)
		*height = item->pos.y_pos + 256;
}

long GetOffset(ITEM_INFO* item, long x, long z)
{
	if (!item->pos.y_rot)
		return ~x & 0x3FF;
	
	if (item->pos.y_rot == -0x8000)
		return x & 0x3FF;
	
	if (item->pos.y_rot == 0x4000)
		return z & 0x3FF;

	return ~z & 0x3FF;
}

void BridgeTilt1Floor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;
	
	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (level >= y)
	{
		*height = level;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeTilt1Ceiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;
	
	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (level < y)
		*height = level + 256;
}

void BridgeTilt2Floor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;
	
	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (level >= y)
	{
		*height = level;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeTilt2Ceiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;
	
	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (level < y)
		*height = level + 256;
}

void ControlAnimatingSlots(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* c;
	PHD_VECTOR pos;
	long f, fe;
	short room_number;

	item = &items[item_number];

	if (gfCurrentLevel == LVL5_THIRTEENTH_FLOOR && item->object_number == ANIMATING8)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 0);
		room_number = item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_number);

		if (item->room_number != room_number)
			ItemNewRoom(item_number, room_number);
	}

	if (!TriggerActive(item))
	{
		switch (item->trigger_flags)
		{
		case 2:
			item->status = ITEM_INVISIBLE;
			return;

		case 333:
			item->flags |= IFL_CODEBITS;
			break;

		case 555:
			item->flags |= IFL_CODEBITS;

			if (item->anim_number == objects[item->object_number].anim_index + 1)
				return;

			item->anim_number = objects[item->object_number].anim_index + 1;
			break;
		}

		item->frame_number = anims[item->anim_number].frame_base;
		return;
	}

	item->status = ITEM_ACTIVE;
	AnimateItem(item);

	if (item->trigger_flags == 222)
		TriggerSkeletonFire(item);
	else if (item->trigger_flags == 111)
		ControlGunTestStation(item);
	else if (item->object_number == SWITCH_TYPE8 && item->trigger_flags != 444)
	{
		if (item->frame_number >= anims[item->anim_number].frame_end)
		{
			item->frame_number = anims[item->anim_number].frame_base;
			RemoveActiveItem(item_number);
			item->collidable = 0;
			item->looked_at = 0;
			item->flags &= ~IFL_CODEBITS;
		}
	}
	else
	{
		switch (item->trigger_flags)
		{
		case 666:
		case 667:
			f = item->frame_number - anims[item->anim_number].frame_base;
			fe = anims[item->anim_number].frame_end - item->frame_number;

			if (f < 16)
				SoundEffect(SFX_HELICOPTER_LOOP, &item->pos, ((2 * f) << 8) | SFX_SETVOL);
			else if (fe < 16)
				SoundEffect(SFX_HELICOPTER_LOOP, &item->pos, ((2 * fe) << 8) | SFX_SETVOL);
			else
				SoundEffect(SFX_HELICOPTER_LOOP, &item->pos, 0x1F00 | SFX_SETVOL);

			c = (CREATURE_INFO*)item->data;
			c->joint_rotation[0] += 0x1CAF;
			c->joint_rotation[1] += 0x1CAF;

			if (item->frame_number == anims[item->anim_number].frame_end)
				KillItem(item_number);

			break;

		case 668:
			SoundEffect(SFX_HELICOPTER_LOOP, 0, 0x800 | SFX_SETVOL | SFX_ALWAYS);
			break;

		case 777:

			if (item->frame_number >= anims[item->anim_number].frame_base + 27)
			{
				pos.x = 1668;
				pos.y= (GetRandomControl() & 0x3F) + 400;
				pos.z = 496;
				GetJointAbsPosition(item, &pos, 0);
				TriggerLiftBrakeSparks(&pos, item->pos.y_rot - 0x4000);

				pos.x = 1668;
				pos.y = (GetRandomControl() & 0x3F) + 400;
				pos.z = -512;
				GetJointAbsPosition(item, &pos, 0);
				TriggerLiftBrakeSparks(&pos, item->pos.y_rot - 0x4000);
			}

			break;

		case 888:
			f = item->frame_number - anims[item->anim_number].frame_base;

			if (f <= 13)
				item->mesh_bits = 127;
			else if (f <= 18)
				item->mesh_bits = 31;
			else if (f <= 24)
				item->mesh_bits = 31;
			else if (f <= 29)
				item->mesh_bits = 15;
			else if (f >= 35)
				item->mesh_bits = 3;
			else
				item->mesh_bits = 7;

			break;
		}
	}
}

void PoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short roty;
	
	item = &items[item_num];

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH ||
		lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		roty = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(PoleBounds, item, l))
		{
			if (MoveLaraPosition(&PolePos, item, l))
			{
				l->anim_number = ANIM_STAT2POLE;
				l->frame_number = anims[ANIM_STAT2POLE].frame_base;
				l->current_anim_state = AS_POLESTAT;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot = roty;
	}
	else if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->gravity_status != 0 && l->fallspeed > 0 &&
		(l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP))
	{
		if (TestBoundsCollide(item, l, 100) && TestCollision(item, l))
		{
			roty = item->pos.y_rot;
			item->pos.y_rot = l->pos.y_rot;

			if (l->current_anim_state == AS_REACH)
			{
				PolePosR.y = l->pos.y_pos - item->pos.y_pos + 10;
				AlignLaraPosition(&PolePosR, item, l);
				l->anim_number = ANIM_REACH2POLE;
				l->frame_number = anims[ANIM_REACH2POLE].frame_base;
			}
			else
			{
				PolePosR.y = l->pos.y_pos - item->pos.y_pos + 60;
				AlignLaraPosition(&PolePosR, item, l);
				l->anim_number = ANIM_JUMP2POLE;
				l->frame_number = anims[ANIM_JUMP2POLE].frame_base;
			}

			l->gravity_status = 0;
			l->fallspeed = 0;
			l->current_anim_state = AS_POLESTAT;
			lara.gun_status = LG_HANDS_BUSY;
			item->pos.y_rot = roty;
		}
	}
	else if ((l->current_anim_state < AS_POLESTAT || l->current_anim_state > AS_POLERIGHT) && l->current_anim_state != AS_BACKJUMP)
		ObjectCollision(item_num, l, coll);
}

void ControlTriggerTriggerer(short item_number)
{
	ITEM_INFO* item;
	short* data;

	item = &items[item_number];
	GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	data = trigger_index;

	if (!data)
		return;

	if ((*data & 0x1F) == LAVA_TYPE)
	{
		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == CLIMB_TYPE)
	{
		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MONKEY_TYPE)
	{
		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == TRIGTRIGGER_TYPE)
	{
		if (TriggerActive(item))
			*data |= IFL_TRIGGERED;
		else
			*data &= ~IFL_TRIGGERED;
	}
}

void AnimateWaterfalls()
{
	TEXTURESTRUCT* Twaterfall;
	OBJECT_INFO* obj;
	float offset, v;
	static long vOffset = 0;

	vOffset = (vOffset - 7) & 0x3F;
	offset = vOffset * (1.0F / 256.0F);

	for (int i = WATERFALL1; i <= WATERFALLSS2; i++)
	{
		obj = &objects[i];

		if (gfCurrentLevel == LVL5_RED_ALERT && i == WATERFALL2)
			continue;

		if (obj->loaded)
		{
			v = AnimatingWaterfallsV[i - WATERFALL1];
			Twaterfall = AnimatingWaterfalls[i - WATERFALL1];
			Twaterfall->v1 = offset + v;
			Twaterfall->v2 = offset + v;
			Twaterfall->v3 = offset + v + (63.0F / 256.0F);
			Twaterfall->v4 = offset + v + (63.0F / 256.0F);

			if (i < WATERFALLSS1)
			{
				Twaterfall++;
				Twaterfall->v1 = offset + v;
				Twaterfall->v2 = offset + v;
				Twaterfall->v3 = offset + v + (63.0F / 256.0F);
				Twaterfall->v4 = offset + v + (63.0F / 256.0F);
			}
		}
	}
}

void ControlWaterfall(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		item->status = ITEM_ACTIVE;

		if (item->trigger_flags == 668)
			SoundEffect(SFX_D_METAL_KICKOPEN, &item->pos, SFX_DEFAULT);
		else if (item->trigger_flags == 777)
			SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
	}
	else if (item->trigger_flags == 2 || item->trigger_flags == 668)
		item->status = ITEM_INVISIBLE;
}

void TightRopeCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		item->pos.y_rot += 0x8000;

		if (TestLaraPosition(TightRopeBounds, item, l))
		{
			if (MoveLaraPosition(&TightRopePos, item, l))
			{
				l->current_anim_state = AS_TROPEGETON;
				l->anim_number = ANIM_TROPEGETON;
				l->frame_number = anims[ANIM_TROPEGETON].frame_base;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_z_rot = 0;
				lara.TightRopeOnCount = 60;
				lara.TightRopeOff = 0;
				lara.TightRopeFall = 0;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
			lara.IsMoving = 0;

		item->pos.y_rot += 0x8000;
	}
	else if (l->current_anim_state == AS_TROPEWALK && l->goal_anim_state != AS_TROPEGETOFF && !lara.TightRopeOff &&
		item->pos.y_rot == l->pos.y_rot && ((abs(item->pos.x_pos - l->pos.x_pos) + abs(item->pos.z_pos - l->pos.z_pos)) < 640))
		lara.TightRopeOff = 1;
}

void ParallelBarsCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short pass, pass1;

	item = &items[item_num];

	if (!(input & IN_ACTION) || l->current_anim_state != AS_REACH || l->anim_number != ANIM_GRABLOOP)
	{
		if (l->current_anim_state != AS_PBSPIN)
			ObjectCollision(item_num, l, coll);

		return;
	}

	pass = (short)TestLaraPosition(ParallelBarsBounds, item, l);

	if (!pass)
	{
		item->pos.y_rot += 0x8000;
		pass1 = (short)TestLaraPosition(ParallelBarsBounds, item, l);
		item->pos.y_rot += 0x8000;

		if (!pass1)
		{
			ObjectCollision(item_num, l, coll);
			return;
		}
	}

	l->current_anim_state = AS_CONTROLLED;
	l->anim_number = ANIM_PB_GRAB;
	l->frame_number = anims[ANIM_PB_GRAB].frame_base;
	l->fallspeed = 0;
	l->gravity_status = 0;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	
	if (pass)
		l->pos.y_rot = item->pos.y_rot;
	else
		l->pos.y_rot = item->pos.y_rot + 0x8000;

	pos.x = 0;
	pos.y = -128;
	pos.z = 512;
	GetLaraJointPos(&pos, LMX_HAND_L);

	pos2.x = 0;
	pos2.y = -128;
	pos2.z = 512;
	GetLaraJointPos(&pos2, LMX_HAND_R);

	if (l->pos.y_rot & 0x4000)
		l->pos.x_pos += item->pos.x_pos - ((pos.x + pos2.x) >> 1);
	else
		l->pos.z_pos += item->pos.z_pos - ((pos.z + pos2.z) >> 1);

	l->pos.y_pos += item->pos.y_pos - ((pos.y + pos2.y) >> 1);
	lara.GeneralPtr = item;
}

void ControlXRayMachine(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	MESH_INFO* mesh;
	long num;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	switch (item->trigger_flags)
	{
	case 0:

		if (item->item_flags[0] == 666)
		{
			if (item->item_flags[1])
				item->item_flags[1]--;
			else
			{
				item->item_flags[1] = 30;
				SoundEffect(SFX_ALARM, &item->pos, SFX_DEFAULT);
			}
		}
		
		if (lara.skelebob && lara.hk_type_carried & W_PRESENT)
		{
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			item->item_flags[0] = 666;
		}

		break;

	case 111:

		if (item->item_flags[0])
		{
			item->item_flags[0]--;

			if (!item->item_flags[0])
			{
				TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
				RemoveActiveItem(item_number);
				item->flags |= IFL_INVISIBLE;
			}

			return;
		}

		if (lara.Fired)
			item->item_flags[0] = 15;

		break;

	case 222:

		if (item->item_flags[1] >= 144)
		{
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			RemoveActiveItem(item_number);
			item->flags |= IFL_INVISIBLE;
			return;
		}

		if (item->item_flags[1] < 128)
		{
			if (!item->item_flags[1])
				num = 16;
			else
				num = 1;

			SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
			TriggerFontFire(&items[item->item_flags[0]], item->item_flags[1], num);
		}

		item->item_flags[1]++;
		break;

	case 333:
	{
		r = &room[item->room_number];
		mesh = r->mesh;

		for (int i = 0; i < r->num_meshes; i++, mesh++)
		{
			if (mesh->Flags & 1 && item->pos.x_pos == mesh->x && item->pos.y_pos == mesh->y && item->pos.z_pos == mesh->z)
			{
				ShatterObject(0, mesh, 128, item->room_number, 0);
				mesh->Flags &= ~1;
				SoundEffect(ShatterSounds[gfCurrentLevel][mesh->static_number - 50], (PHD_3DPOS*)&mesh->x, SFX_DEFAULT);
			}
		}

		RemoveActiveItem(item_number);
		item->flags |= IFL_INVISIBLE;
		break;
	}

	default:
		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
		RemoveActiveItem(item_number);
		break;
	}
}

void CutsceneRopeControl(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	long dx, dy, dz;

	item = &items[item_number];
	pos1.x = -128;
	pos1.y = -72;
	pos1.z = -16;
	GetJointAbsPosition(&items[item->item_flags[2]], &pos1, 0);

	pos2.x = 830;
	pos2.z = -12;
	pos2.y = 0;
	GetJointAbsPosition(&items[item->item_flags[3]], &pos2, 0);

	item->pos.x_pos = pos2.x;
	item->pos.y_pos = pos2.y;
	item->pos.z_pos = pos2.z;
	dx = (pos2.x - pos1.x) * (pos2.x - pos1.x);
	dy = (pos2.y - pos1.y) * (pos2.y - pos1.y);
	dz = (pos2.z - pos1.z) * (pos2.z - pos1.z);
	item->item_flags[1] = (short)(((phd_sqrt(dx + dy + dz) << 1) + phd_sqrt(dx + dy + dz)) << 1);
	item->pos.x_rot = -4869;
}

void HybridCollision(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (gfCurrentLevel == LVL5_SINKING_SUBMARINE && item->frame_number < anims[item->anim_number].frame_end)
		ObjectCollision(item_num, laraitem, coll);
}

void DrawBaddieGunFlash(ITEM_INFO* item)
{
	float m[12];
	long node[2];
	ushort bite[2];
	short num;

	if (!item->fired_weapon)
		return;

	bite[0] = objects[item->object_number].bite_offset;
	bite[1] = objects[item->object_number].bite_offset + 1;
	node[0] = EnemyBites[bite[0]].mesh_num;
	node[1] = EnemyBites[bite[1]].mesh_num;
	
	num = node[0] < 0 ? 1 : 0;

	while (num >= 0)
	{
		GetJointAbsPositionMatrix(item, m, abs(node[num]));
		phd_PushMatrix();
		aMXPtr[M00] = m[M00];
		aMXPtr[M01] = m[M01];
		aMXPtr[M02] = m[M02];
		aMXPtr[M03] = m[M03];
		aMXPtr[M10] = m[M10];
		aMXPtr[M11] = m[M11];
		aMXPtr[M12] = m[M12];
		aMXPtr[M13] = m[M13];
		aMXPtr[M20] = m[M20];
		aMXPtr[M21] = m[M21];
		aMXPtr[M22] = m[M22];
		aMXPtr[M23] = m[M23];
		phd_RotX(-16384);
		phd_TranslateRel(EnemyBites[bite[num]].x, EnemyBites[bite[num]].y, EnemyBites[bite[num]].z);
		phd_RotZ(short(GetRandomControl() << 1));
		phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], -1);
		phd_PopMatrix();
		num--;
	}
}
