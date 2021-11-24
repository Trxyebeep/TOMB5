#include "../tomb5/pch.h"
#include "savegame.h"
#include "objects.h"
#include "traps.h"
#include "items.h"

void WriteSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
	{
		*SGpoint = *data;
		data++;
		SGpoint++;
	}
}

void ReadSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
	{
		*data = *SGpoint;
		data++;
		SGpoint++;
	}
}

long CheckSumValid(char* buffer)	//unused
{
	char* ptr;
	long checksum;

	ptr = buffer;
	checksum = 0;

	for (int i = 0; i < 3828; i++)
	{
		checksum += *ptr;
		ptr++;
	}

	return !checksum;
}

static void SaveLaraData()
{
	ITEM_INFO* item;

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] - (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base - (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base - (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr - (long)malloc_buffer);
	memcpy(&savegame.Lara, &lara, sizeof(savegame.Lara));

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr + (long)malloc_buffer);

	if (lara.weapon_item != NO_ITEM)
	{
		item = &items[lara.weapon_item];
		savegame.WeaponObject = item->object_number;
		savegame.WeaponAnim = item->anim_number;
		savegame.WeaponFrame = item->frame_number;
		savegame.WeaponCurrent = item->current_anim_state;
		savegame.WeaponGoal = item->goal_anim_state;
	}

	savegame.CutSceneTriggered1 = _CutSceneTriggered1;
	savegame.CutSceneTriggered2 = _CutSceneTriggered2;
}

static void RestoreLaraData(long FullSave)
{
	ITEM_INFO* item;
	char flag;

	if (!FullSave)
	{
		savegame.Lara.item_number = lara.item_number;

		if (savegame.Lara.IsMoving)
		{
			savegame.Lara.IsMoving = 0;
			savegame.Lara.gun_status = LG_NO_ARMS;
		}
	}

	memcpy(&lara, &savegame.Lara, sizeof(lara));
	lara.target = 0;
	lara.spaz_effect = 0;
	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr + (long)malloc_buffer);

	if (lara.burn)
	{
		flag = 0;
		lara.burn = 0;

		if (lara.BurnSmoke)
		{
			lara.BurnSmoke = 0;
			flag = 1;
		}

		LaraBurn();

		if (flag)
			lara.BurnSmoke = 1;
	}

	if (lara.weapon_item != -1)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		item->object_number = savegame.WeaponObject;
		item->anim_number = savegame.WeaponAnim;
		item->frame_number = savegame.WeaponFrame;
		item->current_anim_state = savegame.WeaponCurrent;
		item->goal_anim_state = savegame.WeaponGoal;
		item->status = ITEM_ACTIVE;
		item->room_number = NO_ROOM;
	}

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);

	_CutSceneTriggered1 = savegame.CutSceneTriggered1;
	_CutSceneTriggered2 = savegame.CutSceneTriggered2;
}

void inject_savegame(bool replace)
{
	INJECT(0x00470EC0, WriteSG, replace);
	INJECT(0x00470F10, ReadSG, replace);
	INJECT(0x00470F60, CheckSumValid, replace);
	INJECT(0x00471050, SaveLaraData, replace);
	INJECT(0x004720B0, RestoreLaraData, replace);
}
