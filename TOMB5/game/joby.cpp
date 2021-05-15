#include "../tomb5/pch.h"
#include "joby.h"
#include "sound.h"
#include "control.h"
#include "effect2.h"
#include "laramisc.h"
#include "sphere.h"
#include "traps.h"
#include "delstuff.h"

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

		if (room[room_num].flags & RF_FILL_WATER)
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

void inject_joby()
{
	INJECT(0x00442C90, KlaxonTremor);
	INJECT(0x00442BE0, CheckCableBox);
	INJECT(0x00442610, ControlElectricalCables);
}
