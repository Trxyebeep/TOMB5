#include "../tomb5/pch.h"
#include "tower.h"
#include "gameflow.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "camera.h"
#include "draw.h"
#include "sound.h"
#include "../specific/function_stubs.h"
#include "delstuff.h"
#include "effect2.h"
#include "objects.h"
#include "lara.h"
#include "effects.h"
#include "items.h"
#include "debris.h"
#include "switch.h"
#include "traps.h"
#include "tomb4fx.h"
#include "sphere.h"
#include "collide.h"
#include "../specific/file.h"
#include "../specific/gamemain.h"
#include "../specific/specificfx.h"
#include "../specific/function_table.h"

static PHD_VECTOR SteelDoorLensPos;

short SplashOffsets[18] = { 1072, 48, 1072, 48, 650, 280, 200, 320, -300, 320, -800, 320, -1200, 320, -1650, 280, -2112, 48 };

short SteelDoorPos[4][2] =
{
	{ 872, 512 },
	{ 872, -360 },
	{ -860, -360 },
	{ -860, 512 }
};

short SteelDoorMeshswaps[16] = { 37, 74, 111, 148, 185, 222, 259, 296, 340, 380, 420, 470, 481, 518, 555, 592 };

static char NotHitLaraCount;

long TestBoundsCollideCamera(short* bounds, PHD_3DPOS* pos, long radius)
{
	long x, z, dx, dz, sin, cos;

	if (pos->y_pos + bounds[3] > camera.pos.y - radius && pos->y_pos + bounds[2] < radius + camera.pos.y)
	{
		dx = camera.pos.x - pos->x_pos;
		dz = camera.pos.z - pos->z_pos;
		sin = phd_sin(pos->y_rot);
		cos = phd_cos(pos->y_rot);
		x = (cos * dx - sin * dz) >> W2V_SHIFT;
		z = (cos * dz + sin * dx) >> W2V_SHIFT;

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
	x = (dx * cos - dz * sin) >> W2V_SHIFT;
	z = (dx * sin + dz * cos) >> W2V_SHIFT;
	xmin = bounds[0] - 384;
	xmax = bounds[1] + 384;
	zmin = bounds[4] - 384;
	zmax = bounds[5] + 384;

	if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
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

	camera.pos.x = pos->x_pos + ((cos * x + sin * z) >> W2V_SHIFT);
	camera.pos.z = pos->z_pos + ((cos * z - sin * x) >> W2V_SHIFT);
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

void ControlGunship(short item_number)
{
	ITEM_INFO* item;
	MESH_INFO* StaticMesh;
	SPARKS* sptr;
	PHD_VECTOR v;
	GAME_VECTOR pos, pos1, pos2;
	long Target, ricochet, LaraOnLOS;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		SoundEffect(SFX_HELICOPTER_LOOP, &item->pos, 0);
		pos.x = (GetRandomControl() & 0x1FF) - 255;
		pos.y = (GetRandomControl() & 0x1FF) - 255;
		pos.z = (GetRandomControl() & 0x1FF) - 255;
		GetLaraJointPos((PHD_VECTOR*)&pos, LMX_TORSO);
		pos1.x = pos.x;
		pos1.y = pos.y;
		pos1.z = pos.z;

		if (!item->item_flags[0] && !item->item_flags[1] && !item->item_flags[2])
		{
			item->item_flags[0] = (short)(pos.x >> 4);
			item->item_flags[1] = (short)(pos.y >> 4);
			item->item_flags[2] = (short)(pos.z >> 4);
		}

		pos.x = (pos.x + 80 * item->item_flags[0]) / 6;
		pos.y = (pos.y + 80 * item->item_flags[1]) / 6;
		pos.z = (pos.z + 80 * item->item_flags[2]) / 6;
		item->item_flags[0] = (short)(pos.x >> 4);
		item->item_flags[1] = (short)(pos.y >> 4);
		item->item_flags[2] = (short)(pos.z >> 4);

		if (item->trigger_flags == 1)
			item->pos.z_pos += (pos1.z - item->pos.z_pos) >> 5;
		else
			item->pos.x_pos += (pos1.x - item->pos.x_pos) >> 5;

		item->pos.y_pos += (pos1.y - item->pos.y_pos - 256) >> 5;
		pos2.x = item->pos.x_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.y = item->pos.y_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.z = item->pos.z_pos + (GetRandomControl() & 0xFF) - 128;
		pos2.room_number = item->room_number;
		LaraOnLOS = LOS(&pos2, &pos1);
		pos1.x = 3 * pos.x - 2 * pos2.x;
		pos1.y = 3 * pos.y - 2 * pos2.y;
		pos1.z = 3 * pos.z - 2 * pos2.z;
		ricochet = !LOS(&pos2, &pos1);
		NotHitLaraCount = LaraOnLOS ? 1 : NotHitLaraCount + 1;

		if (NotHitLaraCount < 15)
			item->mesh_bits |= 0x100;
		else
			item->mesh_bits &= ~0x100;

		if (NotHitLaraCount < 15)
			SoundEffect(SFX_HK_FIRE, &item->pos, 0xC00000 | SFX_SETPITCH);

		if (GlobalCounter & 1)
		{
			GetLaraOnLOS = 1;
			Target = ObjectOnLOS2(&pos2, &pos1, &v, &StaticMesh);
			GetLaraOnLOS = 0;

			if (Target != 999 && Target >= 0)
			{
				if (items[Target].object_number == LARA)
				{
					TriggerDynamic(pos2.x, pos2.y, pos2.z, 16, (GetRandomControl() & 0x3F) + 96, (GetRandomControl() & 0x1F) + 64, 0);
					DoBloodSplat(v.x, v.y, v.z, short((GetRandomControl() & 1) + 2), short(GetRandomControl() << 1), lara_item->room_number);
					lara_item->hit_points -= 20;
				}
				else if (items[Target].object_number >= SMASH_OBJECT1 && items[Target].object_number <= SMASH_OBJECT8)
				{
					ExplodeItemNode(&items[Target], 0, 0, 0x80);
					SmashObject((short)Target);
					KillItem((short)Target);
				}
			}
			else if (NotHitLaraCount < 15)
			{
				TriggerDynamic(pos2.x, pos2.y, pos2.z, 16, (GetRandomControl() & 0x3F) + 96, (GetRandomControl() & 0x1F) + 64, 0);

				if (ricochet)
				{
					TriggerRicochetSpark(&pos1, 2 * GetRandomControl(), 3, 0);
					TriggerRicochetSpark(&pos1, 2 * GetRandomControl(), 3, 0);
				}

				if (Target < 0 && GetRandomControl() & 1)
				{
					if (StaticMesh->static_number >= 50 && StaticMesh->static_number < 59)
					{
						ShatterObject(0, StaticMesh, 64, pos1.room_number, 0);
						StaticMesh->Flags &= ~0x1;
						TestTriggersAtXYZ(StaticMesh->x, StaticMesh->y, StaticMesh->z, pos1.room_number, 1, 0);
						SoundEffect(ShatterSounds[gfCurrentLevel][StaticMesh->static_number - 50], (PHD_3DPOS*)StaticMesh, 0);
					}

					TriggerRicochetSpark((GAME_VECTOR*)&v, 2 * GetRandomControl(), 3, 0);
					TriggerRicochetSpark((GAME_VECTOR*)&v, 2 * GetRandomControl(), 3, 0);
				}
			}

			if (NotHitLaraCount < 15)
			{
				sptr = &spark[GetFreeSpark()];
				sptr->On = 1;
				sptr->sR = (GetRandomControl() & 0x7F) + 128;
				sptr->dR = sptr->sR;
				sptr->sG = (GetRandomControl() & 0x7F) + (sptr->sR >> 1);

				if (sptr->sG > sptr->sR)
					sptr->sG = sptr->sR;

				sptr->sB = 0;
				sptr->dR = 0;
				sptr->dG = 0;
				sptr->dB = 0;
				sptr->ColFadeSpeed = 12;
				sptr->TransType = 2;
				sptr->FadeToBlack = 0;
				sptr->Life = 12;
				sptr->sLife = 12;
				sptr->x = pos2.x;
				sptr->y = pos2.y;
				sptr->z = pos2.z;
				sptr->Xvel = (short)(4 * (pos1.x - pos2.x));
				sptr->Yvel = (short)(4 * (pos1.y - pos2.y));
				sptr->Zvel = (short)(4 * (pos1.z - pos2.z));
				sptr->Friction = 0;
				sptr->MaxYvel = 0;
				sptr->Gravity = 0;
				sptr->Flags = SF_NONE;
			}
		}

		AnimateItem(item);
	}
}

void ControlIris(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos, pos2;
	long r, g, b, rot;

	item = &items[item_number];

	if (item->timer)
	{
		item->timer--;

		if (!item->timer)
			item->flags |= IFL_CODEBITS;
	}
	else if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
	{
		SoundEffect(SFX_RICH_IRIS_ELEC, &item->pos, SFX_DEFAULT);

		if (!lara.burn)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, LMX_HIPS);

			if (abs(pos.y - item->pos.y_pos) >= 1024 || abs(pos.x - item->pos.x_pos) >= 2048 || abs(pos.z - item->pos.z_pos) >= 2048)
				item->item_flags[3] = 0;
			else
			{
				if (!item->item_flags[3])
					SoundEffect(SFX_LARA_INJURY_NONRND, &lara_item->pos, SFX_DEFAULT);

				if (lara_item->hit_points <= 72 || item->item_flags[3] >= 45)
				{
					LaraBurn();
					lara_item->hit_points = 0;
					lara.BurnCount = 24;
				}
				else
				{
					lara_item->hit_points -= 12;
					item->item_flags[3]++;
				}
			}
		}

		if (!(GlobalCounter & 1))
		{
			r = (GetRandomControl() & 0x3F) + 128;
			g = (GetRandomControl() & 0x7F) + 64;
			b = GetRandomControl() & 0x1F;
			pos.x = item->pos.x_pos;
			pos.y = item->pos.y_pos - 128;
			pos.z = item->pos.z_pos;

			if ((GlobalCounter & 3) == 2)
			{
				rot = (GetRandomControl() & 0xFF) + ((GlobalCounter & 0x7F) << 9) - 128;

				if (GlobalCounter & 4)
					rot += 32768;

				pos2.x = pos.x - ((2240 * phd_sin(rot)) >> W2V_SHIFT);
				pos2.y = pos.y;
				pos2.z = pos.z - ((2240 * phd_cos(rot)) >> W2V_SHIFT);
				TriggerLightning(&pos, &pos2, (GetRandomControl() & 0x3F) + 64, RGBA(r, g, b, 50), 21, 48, 5);
				TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(r >> 1, g >> 1, b >> 1, 32));
			}
			else
			{
				pos2.x = pos.x + ((GetRandomControl() & 0x1FF) << 1) - 512;
				pos2.y = pos.y + ((GetRandomControl() & 0x1FF) << 1) - 512;
				pos2.z = pos.z + ((GetRandomControl() & 0x1FF) << 1) - 512;
				TriggerLightning(&pos, &pos2, (GetRandomControl() & 0xF) + 16, RGBA(r, g >> 2, b >> 2, 24), 7, 48, 3);
			}

			TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(r >> 1, g >> 1, b >> 1, 96));
		}
	}
}

void ControlFishtank(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	short x, z, dz;

	item = &items[item_number];

	for (int i = GlobalCounter & 1; i < 9; i += 2)
	{
		x = SplashOffsets[2 * i];
		z = SplashOffsets[2 * i + 1];

		if (i && i != 8)
		{
			x -= short(GetRandomControl() % abs(SplashOffsets[2 * i + 2] - x));
			dz = SplashOffsets[2 * i + 3] - z;

			if (dz < 0)
				z -= GetRandomControl() % dz;
			else if (dz > 0)
				z += GetRandomControl() % dz;
		}

		pos.x = x;
		pos.y = 0;
		pos.z = z;
		GetJointAbsPosition(item, &pos, 0);
		TriggerFishtankSpray(pos.x, pos.y, pos.z, (item->item_flags[1] - 800) >> 3);
	}

	if (item->item_flags[1] > 819)
		item->item_flags[1] -= item->item_flags[1] >> 6;
	else
		KillItem(item_number);
}

void ControlArea51Laser(short item_number)
{
	ITEM_INFO* item;
	long x, z, dx, dz;
	short num, room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->current_anim_state = 0;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos,
		(GetRandomControl() & 1) + 8, (GetRandomControl() & 3) + 24, GetRandomControl() & 3, GetRandomControl() & 1);
	item->mesh_bits = -1 - (GetRandomControl() & 0x14);
	dx = abs(((item->item_flags[1] & 0xFF) << 9) - item->pos.x_pos);

	if (dx < 768)
	{
		dz = abs(((item->item_flags[1] & 0xFF00) << 1) - item->pos.z_pos);

		if (dz < 768)
		{
			item->trigger_flags = 32;
			x = ((((item->item_flags[0] & 0xFF) << 9) + ((-2560 * (item->item_flags[2] * phd_sin(item->pos.y_rot))) >> W2V_SHIFT)) >> 9) & 0xFF;
			z = (((((-2560 * (item->item_flags[2] * phd_cos(item->pos.y_rot))) >> W2V_SHIFT) + ((item->item_flags[0] & 0xFF00) << 1)) >> 9) & 0xFF) << 8;
			item->item_flags[1] = (short)(x | z);
		}
	}

	if (item->item_flags[2] == 1)
	{
		if (item->trigger_flags)
		{
			if (item->item_flags[3])
			{
				if (item->item_flags[3] > 4)
					item->item_flags[3] -= item->item_flags[3] >> 2;
				else
					item->item_flags[3] = 0;
			}
			else
			{
				item->trigger_flags--;

				if (item->trigger_flags == 1)
					item->item_flags[2] = -1;
			}
		}
		else
		{
			item->item_flags[3] += 5;

			if (item->item_flags[3] > 512)
				item->item_flags[3] = 512;
		}
	}
	else
	{
		if (item->trigger_flags)
		{
			if (item->item_flags[3])
			{
				if (item->item_flags[3] < -4)
					item->item_flags[3] -= item->item_flags[3] >> 2;
				else
					item->item_flags[3] = 0;
			}
			else
			{
				item->trigger_flags--;

				if (item->trigger_flags == 1)
					item->item_flags[2] = -item->item_flags[2];
			}
		}
		else
		{
			item->item_flags[3] -= 5;

			if (item->item_flags[3] < -512)
				item->item_flags[3] = -512;
		}
	}

	if (item->item_flags[3])
	{
		num = abs(item->item_flags[3] >> 4);

		if (num > 31)
			num = 31;

		SoundEffect(SFX_ZOOM_VIEW_WHIRR, &item->pos, 0x800000 | SFX_ALWAYS | SFX_SETPITCH | SFX_SETVOL | (num << 8));
	}

	item->pos.z_pos += (item->item_flags[3] * phd_cos(item->pos.y_rot)) >> 16;
	item->pos.x_pos += (item->item_flags[3] * phd_sin(item->pos.y_rot)) >> 16;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	if (TestBoundsCollide(item, lara_item, 64))
	{
		if (!item->draw_room)
		{
			SoundEffect(SFX_LARA_INJURY_NONRND, &lara_item->pos, SFX_DEFAULT);
			item->draw_room = 1;
		}

		lara_item->hit_points -= 100;
		DoBloodSplat(lara_item->pos.x_pos, item->pos.y_pos - GetRandomControl() - 32, lara_item->pos.z_pos,
			(GetRandomControl() & 3) + 4, short(GetRandomControl() << 1), lara_item->room_number);
		AnimateItem(item);
	}
	else
	{
		item->draw_room = 0;
		AnimateItem(item);
	}
}

void ControlGasCloud(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	PHD_VECTOR pos;
	long rad, num;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (lara.water_status != LW_FLYCHEAT)
	{
		if (!lara.Gassed)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraJointPos(&pos, LMX_HEAD);
			room_number = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_number);

			if (room[room_number].flags & ROOM_NO_LENSFLARE)
				lara.Gassed = 1;
		}
	}

	if (item->trigger_flags < 2)
	{
		if (item->item_flags[0] < 256)
			item->item_flags[0]++;

		return;
	}

	if (!item->item_flags[0])
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = 8;
	sptr->dG = 32;
	sptr->dB = 8;
	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + item->pos.x_pos - 16;
	sptr->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 16;
	sptr->z = (GetRandomControl() & 0x1F) + item->pos.z_pos - 16;
	rad = (GetRandomControl() & 0x7F) + 2048;

	if (item->trigger_flags == 2)
	{
		sptr->Xvel = short((rad * phd_sin(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
		sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Zvel = short((rad * phd_cos(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
	}
	else if (item->trigger_flags == 3)
	{
		sptr->Xvel = short((rad * phd_sin(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
		sptr->Yvel = (short)rad;
		sptr->Zvel = short((rad * phd_cos(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
	}
	else
	{
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = (short)rad;
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	}

	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->sB = 0;
	sptr->sG = 0;
	sptr->sR = 0;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 4;
	sptr->Scalar = 3;
	sptr->Friction = 68;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	sptr->dSize = (GetRandomControl() & 0x1F) + 64;
	sptr->Size = sptr->dSize >> 2;
	sptr->sSize = sptr->Size;

	if (GlobalCounter & 1)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->dR = 8;
		sptr->dG = 32;
		sptr->dB = 8;
		sptr->TransType = 2;
		rad = (GetRandomControl() & 0x3F) + 128;

		if (item->trigger_flags == 2)
		{
			num = (GetRandomControl() & 0x1FF) - 256;
			sptr->x = item->pos.x_pos + (GetRandomControl() & 0x1F) + ((num * phd_sin(item->pos.y_rot + 0x4000)) >> W2V_SHIFT) - 16;
			sptr->y = item->pos.y_pos + (GetRandomControl() & 0x1F) + ((GetRandomControl() & 0x1FF) - 256) - 16;
			sptr->z = item->pos.z_pos + (GetRandomControl() & 0x1F) + ((num * phd_cos(item->pos.y_rot + 0x4000)) >> W2V_SHIFT) - 16;
			sptr->Xvel = short((rad * phd_sin(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
			sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Zvel = short((rad * phd_cos(item->pos.y_rot - 0x8000)) >> W2V_SHIFT);
		}
		else
		{
			if (item->trigger_flags == 3)
			{
				sptr->x = item->pos.x_pos + (GetRandomControl() & 0x1F) + ((GetRandomControl() & 0x1FF) - 256) - 16;
				sptr->z = item->pos.z_pos + (GetRandomControl() & 0x1F) + ((GetRandomControl() & 0x1FF) - 256) - 16;
				sptr->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 16;
				room_number = item->room_number;
				sptr->y = GetCeiling(GetFloor(sptr->x, sptr->y, sptr->z, &room_number), sptr->x, sptr->y, sptr->z);
			}
			else
			{
				sptr->x = item->pos.x_pos + (GetRandomControl() & 0x1F) + ((GetRandomControl() & 0x1FF) - 256) - 16;
				sptr->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 16;
				sptr->z = item->pos.z_pos + (GetRandomControl() & 0x1F) + ((GetRandomControl() & 0x1FF) - 256) - 16;
			}

			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Yvel = (short)rad;
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		}

		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
		sptr->MaxYvel = 0;
		sptr->Gravity = -16 - (GetRandomControl() & 0xF);
		sptr->sB = 0;
		sptr->sG = 0;
		sptr->sR = 0;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->Scalar = 2;
		sptr->Friction = 4;
		sptr->Life = (GetRandomControl() & 3) + 16;
		sptr->sLife = sptr->Life;
		sptr->dSize = (GetRandomControl() & 0x1F) + 64;
		sptr->Size = sptr->dSize >> 2;
		sptr->sSize = sptr->Size;
	}

	item->item_flags[0] = 0;
}

void SteelDoorCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->item_flags[0] != 3 && TestBoundsCollide(&items[item_number], laraitem, coll->radius) &&
		TestCollision(item, laraitem) && coll->enable_baddie_push)
		ItemPushLara(item, laraitem, coll, 0, 1);
}

void ControlSteelDoor(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long h, div, rgb;
	short room_number, frame;

	item = &items[item_number];

	if (item->item_flags[0] != 3)
	{
		if (!item->item_flags[0])
		{
			item->pos.y_pos += item->fallspeed;
			item->fallspeed += 24;
			room_number = item->room_number;
			h = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
				item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (item->pos.y_pos > h)
			{
				item->pos.y_pos = h;

				if (item->fallspeed > 128)
				{
					ScreenShake(item, 64, 4096);
					item->fallspeed = -item->fallspeed >> 2;
				}
				else
				{
					item->item_flags[1]++;
					item->fallspeed = 0;

					if (item->item_flags[1] > 60)
					{
						item->item_flags[0]++;
						item->item_flags[1] = 0;
					}
				}
			}
		}
		else if (item->item_flags[0] == 1)
		{
			SoundEffect(SFX_WELD_THRU_DOOR_LOOP, &item->pos, SFX_DEFAULT);
			item->item_flags[3]++;

			if (item->item_flags[3] == SteelDoorMeshswaps[item->trigger_flags] * 3)
			{
				item->mesh_bits <<= 1;
				item->trigger_flags++;
			}

			pos.x = SteelDoorPos[item->item_flags[1]][0];
			pos.y = 0;
			pos.z = SteelDoorPos[item->item_flags[1]][1];
			GetJointAbsPosition(item, &pos, 0);
			pos2.x = SteelDoorPos[item->item_flags[1] + 1][0];
			pos2.y = 0;
			pos2.z = SteelDoorPos[item->item_flags[1] + 1][1];
			GetJointAbsPosition(item, &pos2, 0);
			div = item->item_flags[1] != 1 ? 384 : 768;
			pos2.x = item->item_flags[2] * (pos2.x - pos.x) / div;
			pos2.y = item->item_flags[2] * (pos2.y - pos.y) / div;
			pos2.z = item->item_flags[2] * (pos2.z - pos.z) / div;
			pos.x += pos2.x;
			pos.y += pos2.y;
			pos.z += pos2.z;
			SteelDoorLensPos = pos;
			TriggerWeldingEffects(&pos, item->pos.y_rot, item->item_flags[1]);
			item->item_flags[2]++;

			if (item->item_flags[2] == div)
			{
				item->item_flags[1]++;
				item->item_flags[2] = 0;

				if (item->item_flags[1] == 3)
				{
					item->item_flags[0]++;
					item->mesh_bits = 0x30000;
					item->item_flags[1] = 0;
				}
			}

			rgb = (GetRandomControl() & 0x3F) + 160;
			TriggerDynamic(pos.x, pos.y, pos.z, 10, rgb, rgb, rgb);
		}
		else
		{
			if (item->item_flags[1] >= 30)
			{
				if (item->current_anim_state == 2)
				{
					frame = item->frame_number;

					if (frame == anims[item->anim_number].frame_base + 13)
					{
						TriggerSteelDoorSmoke(item->pos.y_rot + 16384, 0, item);
						TriggerSteelDoorSmoke(item->pos.y_rot, 1, item);
						TriggerSteelDoorSmoke(item->pos.y_rot - 16384, 2, item);
						SoundEffect(SFX_RICH_VENT_IMPACT, &item->pos, SFX_DEFAULT);
						ScreenShake(item, 96, 4096);
					}
					else if (frame == anims[item->anim_number].frame_end)
					{
						TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
						item->item_flags[0]++;
					}
				}
			}
			else
			{
				item->item_flags[1]++;

				if (item->item_flags[1] == 30)
				{
					item->goal_anim_state = 2;
					TriggerSteelDoorSmoke(item->pos.y_rot, 0, item);
					TriggerSteelDoorSmoke(item->pos.y_rot, 1, item);
					TriggerSteelDoorSmoke(item->pos.y_rot, 2, item);
					ScreenShake(item, 32, 4096);
				}
			}
		}

		AnimateItem(item);
	}
}

void DrawSprite2(long x, long y, long slot, long col, long size, long z)
{
	D3DTLVERTEX v[4];
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT tex;
	long x1, y1, x2, y2;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + slot];
	x1 = x - size;
	y1 = y - size;
	x2 = x + size;
	y2 = y + size;
	setXY4(v, x1, y1, x2, y1, x1, y2, x2, y2, z, clipflags);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = col;
		v[i].specular = 0xFF000000;
	}

	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);
}

void DrawSteelDoorLensFlare(ITEM_INFO* item)
{
	FVECTOR pos;
	long dx, dy, dz;
	long x, y, z, r, g, b;
	float zv;

	if (item->item_flags[0] != 1 || !item->item_flags[3])
		return;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	dx = SteelDoorLensPos.x - item->pos.x_pos;
	dy = SteelDoorLensPos.y - item->pos.y_pos;
	dz = SteelDoorLensPos.z - item->pos.z_pos;
	pos.x = aMXPtr[M00] * dx + aMXPtr[M01] * dy + aMXPtr[M02] * dz + aMXPtr[M03];
	pos.y = aMXPtr[M10] * dx + aMXPtr[M11] * dy + aMXPtr[M12] * dz + aMXPtr[M13];
	pos.z = aMXPtr[M20] * dx + aMXPtr[M21] * dy + aMXPtr[M22] * dz + aMXPtr[M23];
	zv = f_persp / pos.z;
	x = long(pos.x * zv + f_centerx);
	y = long(pos.y * zv + f_centery);
	z = long(pos.z - 256);

	if (z < 0)
		z = 0;

	phd_PopMatrix();
	r = (GetRandomControl() & 0x3F) + 128;
	g = (GetRandomControl() & 0x3F) + 128;
	b = (GetRandomControl() & 0x3F) + 128;
	DrawSprite2(x, y, 32, RGBA(r, g, b, 128), (GetRandomControl() & 0xF) + 32, z);
}

void TriggerLiftBrakeSparks(PHD_VECTOR* pos, short yrot)
{
	SMOKE_SPARKS* smoke;
	SPARKS* sptr;
	long v;
	short yAdd;

	yrot += 0x8000;

	smoke = &smoke_spark[GetFreeSmokeSpark()];
	smoke->On = 1;
	smoke->sShade = 0;
	smoke->dShade = 64;
	smoke->ColFadeSpeed = 1;
	smoke->FadeToBlack = 8;
	smoke->TransType = 2;
	smoke->Life = (GetRandomControl() & 3) + 24;
	smoke->sLife = smoke->Life;

	v = 2 * (GetRandomControl() & 0x7F) + 128;
	yAdd = short((GetRandomControl() >> 2) - 4096);
	smoke->x = (GetRandomControl() & 0x1F) + pos->x - 16;
	smoke->y = pos->y;
	smoke->z = (GetRandomControl() & 0x1F) + pos->z - 16;
	smoke->Xvel = short((v * phd_sin(yrot + yAdd)) >> W2V_SHIFT);
	smoke->Yvel = (-128 - (GetRandomControl() & 0x7F)) << 3;
	smoke->Zvel = short((v * phd_cos(yrot + yAdd)) >> W2V_SHIFT);

	smoke->Friction = 84;
	smoke->Flags = SF_ROTATE;
	smoke->RotAng = GetRandomControl() & 0xFFF;
	smoke->RotAdd = (GetRandomControl() & 0x1F) - 16;
	smoke->MaxYvel = 0;
	smoke->Gravity = 0;
	smoke->Size = (GetRandomControl() & 0x1F) + 32;
	smoke->sSize = smoke->Size;
	smoke->dSize = smoke->Size + 16;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = sptr->dR - (GetRandomControl() & 0x7F);
	sptr->dB = 0;
	sptr->ColFadeSpeed = 4;
	sptr->TransType = 2;
	sptr->FadeToBlack = 4;
	sptr->Life = 12;
	sptr->sLife = 12;

	yAdd = short((GetRandomControl() >> 1) - 8192);
	v = GetRandomControl() & 0x1FF;
	sptr->Xvel = short((v * phd_sin(yrot + yAdd)) >> W2V_SHIFT);
	sptr->Yvel = (-512 - (GetRandomControl() & 0x7F)) << 2;
	sptr->Zvel = short((v * phd_cos(yrot + yAdd)) >> W2V_SHIFT);
	sptr->x = (sptr->Xvel >> 5) + (GetRandomControl() & 0x1F) + pos->x - 16;
	sptr->y = pos->y + (sptr->Yvel >> 5);
	sptr->z = (sptr->Zvel >> 5) + (GetRandomControl() & 0x1F) + pos->z - 16;

	sptr->Friction = 84;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = SF_NONE;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 16;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 0;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 24;
	sptr->sLife = sptr->Life;

	yAdd = short((GetRandomControl() >> 2) - 4096);
	v = 2 * (GetRandomControl() & 0x7F) + 128;
	sptr->x = (GetRandomControl() & 0xF) + pos->x - 8;
	sptr->y = pos->y;
	sptr->z = (GetRandomControl() & 0xF) + pos->z - 8;
	sptr->Xvel = short((v * phd_sin(yrot + yAdd)) >> W2V_SHIFT);
	sptr->Yvel = -512 - (GetRandomControl() & 0x3FF);
	sptr->Zvel = short((v * phd_cos(yrot + yAdd)) >> W2V_SHIFT);

	sptr->Friction = 6;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->MaxYvel = 0;
	sptr->Size = (GetRandomControl() & 0xF) + 12;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 1;
	sptr->Flags = SF_DEF | SF_SCALE;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + W2V_SHIFT;
	sptr->Gravity = (GetRandomControl() & 0xF) + 16;
	sptr->Scalar = 1;
}

void TriggerSteelDoorSmoke(short angle, short nPos, ITEM_INFO* item)
{
	SPARKS* sptr;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long dx, dy, dz, num, v, lp;

	pos.x = SteelDoorPos[nPos][0];
	pos.y = 0;
	pos.z = SteelDoorPos[nPos][1] - 512;
	GetJointAbsPosition(item, &pos, 17);

	pos2.x = SteelDoorPos[nPos + 1][0];
	pos2.y = 0;
	pos2.z = SteelDoorPos[nPos + 1][1] - 512;
	GetJointAbsPosition(item, &pos2, 17);

	dx = pos2.x - pos.x;
	dy = pos2.y - pos.y;
	dz = pos2.z - pos.z;

	if (nPos == 1)
		num = 5;
	else
		num = 4;

	for (lp = 0; lp < (1 << num); lp++)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = (GetRandomControl() & 0x1F) + 32;
		sptr->sG = sptr->sR;
		sptr->sB = sptr->sR;
		sptr->dR = (GetRandomControl() & 0x1F) + 64;
		sptr->dG = sptr->dR;
		sptr->dB = sptr->dR;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->Life = (GetRandomControl() & 7) + 24;
		sptr->sLife = sptr->Life;
		sptr->TransType = 2;

		v = GetRandomControl() & 0x3F;
		sptr->x = pos.x + (dx * lp >> num);
		sptr->y = pos.y + (dy * lp >> num);
		sptr->z = pos.z + (dz * lp >> num);
		sptr->Xvel = short((v * phd_sin(angle)) >> 10);
		sptr->Yvel = 0;
		sptr->Zvel = short((v * phd_cos(angle)) >> 10);

		sptr->Friction = 4;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
		sptr->Gravity = -8 - (GetRandomControl() & 7);
		sptr->MaxYvel = 0;
		sptr->Scalar = 3;
		sptr->dSize = (GetRandomControl() & 7) + 32;
		sptr->Size = sptr->dSize >> 1;
		sptr->sSize = sptr->Size;
	}
}

void TriggerWeldingEffects(PHD_VECTOR* pos, short yrot, short flag)
{
	SPARKS* sptr;
	SMOKE_SPARKS* smoke;
	long lp;
	short v, ang;

	//molten steel looking thing
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = 192;
	sptr->dG = (GetRandomControl() & 0x1F) + 96;
	sptr->dB = GetRandomControl() & 0x1F;
	sptr->ColFadeSpeed = 20;
	sptr->FadeToBlack = 32;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 56;
	sptr->sLife = sptr->Life;

	v = (GetRandomControl() & 0x1F) - 16;
	sptr->x = pos->x + ((v * phd_sin(yrot + 0x4000)) >> W2V_SHIFT);
	sptr->y = pos->y + (v >> 1);
	sptr->z = pos->z + ((v * phd_cos(yrot + 0x4000)) >> W2V_SHIFT);
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;

	sptr->Flags = SF_DEF | SF_SCALE;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
	sptr->Scalar = 0;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Size = (GetRandomControl() & 7) + 32;
	sptr->sSize = sptr->Size;
	sptr->dSize = (GetRandomControl() & 7) + 16;

	//the line sparks
	for (lp = 0; lp < 2; lp++)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 255;
		sptr->sG = 255;
		sptr->sB = 255;
		sptr->dR = 192;
		sptr->dG = GetRandomControl() & 0x7F;
		sptr->dB = GetRandomControl() & 0x7F;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 4;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 24;
		sptr->sLife = sptr->Life;
		v = short((GetRandomControl() & 0x7F) + 16);
		ang = (GetRandomControl() & 0x3FF) + yrot - 512;
		sptr->Xvel = (v * phd_sin(ang)) >> 10;
		sptr->Yvel = -(GetRandomControl() & 0x7F);
		sptr->Zvel = (v * phd_cos(ang)) >> 10;
		v = short((GetRandomControl() & 0x1F) - 16);
		sptr->x = pos->x + (sptr->Xvel >> 5) + ((v * phd_sin(yrot + 0x4000)) >> W2V_SHIFT);
		sptr->y = pos->y + (v >> 1) + (sptr->Yvel >> 5);
		sptr->z = pos->z + (sptr->Zvel >> 5) + ((v * phd_cos(yrot + 0x4000)) >> W2V_SHIFT);
		sptr->Friction = 5;
		sptr->Flags = SF_NONE;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
	}

	//smoke
	smoke = &smoke_spark[GetFreeSmokeSpark()];
	smoke->On = 1;
	smoke->sShade = 0;
	smoke->dShade = (GetRandomControl() & 0x1F) + 64;
	smoke->ColFadeSpeed = 4;
	smoke->FadeToBlack = 24 - (GetRandomControl() & 7);
	smoke->TransType = 2;
	smoke->Life = (GetRandomControl() & 7) + 48;
	smoke->sLife = smoke->Life;
	smoke->x = sptr->x;
	smoke->y = sptr->y;
	smoke->z = sptr->z;
	smoke->Xvel = sptr->Xvel >> 2;
	smoke->Yvel = 0;
	smoke->Zvel = sptr->Zvel >> 2;
	smoke->Friction = 2;
	smoke->Flags = SF_ROTATE;
	smoke->RotAng = GetRandomControl() & 0xFFF;
	smoke->RotAdd = (GetRandomControl() & 0x7F) - 64;
	smoke->Gravity = -16 - (GetRandomControl() & 0xF);
	smoke->MaxYvel = 0;
	smoke->dSize = (GetRandomControl() & 0x1F) + 32;
	smoke->Size = smoke->dSize >> 2;
	smoke->sSize = smoke->Size;

	//little left over molten steel dots 
	if (!(GetRandomControl() & 0x3F))
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 255;
		sptr->sG = 255;
		sptr->sB = 255;
		sptr->dR = 255;
		sptr->dG = (GetRandomControl() & 0x1F) + 0x80;
		sptr->dB = GetRandomControl() & 0x1F;
		sptr->ColFadeSpeed = 48;
		sptr->FadeToBlack = 32;
		sptr->Life = 120;
		sptr->sLife = 120;
		sptr->TransType = 2;

		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;
		v = (GetRandomControl() & 7) + 8;

		if (GetRandomControl() & 1)
			v = -v;

		if (flag == 1)
			sptr->y += v;
		else
		{
			sptr->x += (v * phd_sin(yrot + 0x4000)) >> W2V_SHIFT;
			sptr->z += (v * phd_cos(yrot + 0x4000)) >> W2V_SHIFT;
		}

		sptr->Xvel = 0;
		sptr->Yvel = 0;
		sptr->Zvel = 0;

		sptr->Flags = SF_DEF | SF_SCALE;
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
		sptr->Scalar = 1;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		sptr->Size = (GetRandomControl() & 3) + 4;
		sptr->sSize = sptr->Size;
		sptr->dSize = sptr->Size;
	}

	//little falling molten steel dots
	if (!(GetRandomControl() & 3))
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 255;
		sptr->sG = 255;
		sptr->sB = 255;
		sptr->dR = 255;
		sptr->dG = (GetRandomControl() & 0x1F) + 128;
		sptr->dB = GetRandomControl() & 0x1F;
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 32;
		sptr->sLife = sptr->Life;

		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;

		v = (GetRandomControl() & 7) + 8;

		if (GetRandomControl() & 1)
			v = -v;

		if (flag == 1)
		{
			sptr->y += v;
		}
		else
		{
			sptr->x += (v * phd_sin(yrot + 0x4000)) >> W2V_SHIFT;
			sptr->z += (v * phd_cos(yrot + 0x4000)) >> W2V_SHIFT;
		}

		v = short((GetRandomControl() & 0x7F) + 16);
		sptr->Xvel = (v * phd_sin(yrot)) >> W2V_SHIFT;
		sptr->Yvel = (GetRandomControl() & 0xF) + 16;
		sptr->Zvel = (v * phd_cos(yrot)) >> W2V_SHIFT;

		sptr->Friction = 3;
		sptr->Flags = SF_DEF | SF_SCALE;
		sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 14;
		sptr->Scalar = 1;
		sptr->Gravity = (GetRandomControl() & 0xF) + 8;
		sptr->MaxYvel = 0;
		sptr->Size = (GetRandomControl() & 3) + 8;
		sptr->sSize = sptr->Size;
		sptr->dSize = 0;
	}
}

void TriggerFishtankSpray(long x, long y, long z, long c)
{
	SPARKS* sptr;

	c >>= 1;

	if (c > 64)
		c = 64;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = (uchar)c;
	sptr->dG = sptr->dR;
	sptr->dB = sptr->dR;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 14;
	sptr->sLife = sptr->Life;

	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -(GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

	sptr->Friction = 4;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->Scalar = 3;
	sptr->Gravity = -16 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = (GetRandomControl() & 0x1F) + 64;
	sptr->Size = sptr->dSize >> 2;
	sptr->sSize = sptr->Size;
}
