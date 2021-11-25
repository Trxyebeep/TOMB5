#include "../tomb5/pch.h"
#include "savegame.h"
#include "objects.h"
#include "traps.h"
#include "items.h"
#include "rope.h"

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

static void SaveLevelData(long FullSave)	//write all the bs to the savegame buffer
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	OBJECT_INFO* obj;
	RAT_STRUCT* rat;
	BAT_STRUCT* bat;
	MESH_INFO* mesh;	//not in symbols
	CREATURE_INFO* creature;	//neither is this
	ulong flags;
	long k, flare_age;
	ushort packed;
	short pos, word, objnum, maxval;
	uchar byte;
	char lflags;

	WriteSG(&FmvSceneTriggered, sizeof(long));
	WriteSG(&GLOBAL_lastinvitem, sizeof(long));
	word = 0;//used to write 2 bytes

	for (int i = 0; i < 255; i++)
	{
		if (flip_stats[i])
			word |= (1 << i);
	}

	WriteSG(&word, sizeof(short));

	for (int i = 0; i < 255; i++)
	{
		word = (short)(flipmap[i] >> 8);
		WriteSG(&word, sizeof(short));
	}

	WriteSG(&flipeffect, sizeof(long));
	WriteSG(&fliptimer, sizeof(long));
	WriteSG(&flip_status, sizeof(long));
	WriteSG(cd_flags, 136);
	WriteSG(&CurrentAtmosphere, sizeof(uchar));
	word = 0;
	k = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (int j = 0; j < r->num_meshes; j++)
		{
			mesh = &r->mesh[j];

			if (mesh->static_number >= 50 && mesh->static_number <= 59)
			{
				word |= ((mesh->Flags & 1) << k);
				k++;

				if (k == 16)
				{
					WriteSG(&word, sizeof(short));
					k = 0;
					word = 0;
				}
			}
		}
	}

	if (k)
		WriteSG(&word, sizeof(short));

	WriteSG(&CurrentSequence, sizeof(uchar));
	byte = 0;

	for (int i = 0; i < 6; i++)
		byte |= SequenceUsed[i] << i;

	WriteSG(&byte, sizeof(uchar));
	WriteSG(Sequences, 3);

	for (int i = 0; i < number_cameras; i++)
		WriteSG(&camera.fixed[i].flags, sizeof(short));

	for (int i = 0; i < number_spotcams; i++)
		WriteSG(&SpotCam[i].flags, sizeof(short));

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];
		obj = &objects[item->object_number];
		packed = 0;

		if (item->flags & -0x8000)
		{
			packed = 0x2000;
			WriteSG(&packed, sizeof(ushort));
		}
		else
		{
			if (item->flags & (IFL_CODEBITS | IFL_INVISIBLE | IFL_TRIGGERED) || item->object_number == LARA && FullSave)
			{
				packed = 0x8000;

				if (item->pos.x_rot)
					packed |= 1;

				if (item->pos.z_rot)
					packed |= 2;

				if (item->pos.x_pos & 1)
					packed |= 4;

				if (item->pos.y_pos & 1)
					packed |= 8;

				if (item->pos.z_pos & 1)
					packed |= 0x10;

				if (item->speed)
					packed |= 0x20;

				if (item->fallspeed)
					packed |= 0x40;

				if (item->item_flags[0])
					packed |= 0x80;

				if (item->item_flags[1])
					packed |= 0x100;

				if (item->item_flags[2])
					packed |= 0x200;

				if (item->item_flags[3])
					packed |= 0x400;

				if (item->timer)
					packed |= 0x800;

				if (item->trigger_flags)
					packed |= 0x1000;

				if (obj->save_hitpoints)
					packed |= 0x4000;

				WriteSG(&packed, sizeof(ushort));

				if (obj->save_position)
				{
					pos = (short)(item->pos.x_pos >> 1);
					WriteSG(&pos, sizeof(short));
					pos = (short)(item->pos.y_pos >> 1);
					WriteSG(&pos, sizeof(short));
					pos = (short)(item->pos.z_pos >> 1);
					WriteSG(&pos, sizeof(short));
					byte = (uchar)item->room_number;
					WriteSG(&byte, sizeof(uchar));
					WriteSG(&item->pos.y_rot, sizeof(short));

					if (packed & 1)
						WriteSG(&item->pos.x_rot, sizeof(short));

					if (packed & 2)
						WriteSG(&item->pos.z_rot, sizeof(short));

					if (packed & 0x20)
						WriteSG(&item->speed, sizeof(short));

					if (packed & 0x40)
						WriteSG(&item->fallspeed, sizeof(short));
				}

				if (obj->save_anim)
				{
					byte = (uchar)(item->current_anim_state);	//save anim state etc.... but why in 1 byte!! THEY'RE SHORTS CORE
					WriteSG(&byte, sizeof(uchar));
					byte = (uchar)(item->goal_anim_state);
					WriteSG(&byte, sizeof(uchar));
					byte = (uchar)(item->required_anim_state);
					WriteSG(&byte, sizeof(uchar));

					if (item->object_number != LARA)
					{
						byte = item->anim_number - obj->anim_index;
						WriteSG(&byte, sizeof(uchar));
					}
					else
						WriteSG(&item->anim_number, sizeof(short));

					WriteSG(&item->frame_number, sizeof(short));
				}

				if (packed & 0x4000)
					WriteSG(&item->hit_points, sizeof(short));

				if (obj->save_flags)
				{
					//flags var has item->flags and the bitfield of the item. (flags | bitfield) EXCEPT FOR InDrawRoom, for some reason
					flags = item->flags;
					flags |= (*(ulong*)((char*)item + 0x15EA) & 0x7FFF) << 16;	//messy since we can't take bitfield address

					if (obj->intelligent && item->data)
						flags |= 0x80000000;

					WriteSG(&flags, sizeof(ulong));

					if (packed & 0x80)
						WriteSG(&item->item_flags[0], sizeof(short));

					if (packed & 0x100)
						WriteSG(&item->item_flags[1], sizeof(short));

					if (packed & 0x200)
						WriteSG(&item->item_flags[2], sizeof(short));

					if (packed & 0x400)
						WriteSG(&item->item_flags[3], sizeof(short));

					if (packed & 0x800)
						WriteSG(&item->timer, sizeof(short));

					if (packed & 0x1000)
						WriteSG(&item->trigger_flags, sizeof(short));

					if (obj->intelligent || (item->object_number >= SEARCH_OBJECT1 && item->object_number <= SEARCH_OBJECT4))
						WriteSG(&item->carried_item, sizeof(short));

					if (flags & 0x80000000)
					{
						creature = (CREATURE_INFO*)item->data;
						creature->enemy = (ITEM_INFO*)((long)creature->enemy - (long)malloc_buffer);
						WriteSG(item->data, 22);
						creature->enemy = (ITEM_INFO*)((long)creature->enemy + (long)malloc_buffer);
						WriteSG(&creature->ai_target.object_number, sizeof(short));
						WriteSG(&creature->ai_target.room_number, sizeof(short));
						WriteSG(&creature->ai_target.box_number, sizeof(ushort));
						WriteSG(&creature->ai_target.flags, sizeof(short));
						WriteSG(&creature->ai_target.trigger_flags, sizeof(short));
						WriteSG(&creature->ai_target.pos, sizeof(PHD_3DPOS));
						lflags = creature->LOT.can_jump;
						lflags |= creature->LOT.can_monkey << 1;
						lflags |= creature->LOT.is_amphibious << 2;
						lflags |= creature->LOT.is_jumping << 3;
						lflags |= creature->LOT.is_monkeying << 4;
						WriteSG(&lflags, sizeof(char));
					}
				}

				if (obj->save_mesh)
				{
					WriteSG(&item->mesh_bits, sizeof(ulong));
					WriteSG(&item->meshswap_meshbits, sizeof(ulong));
				}
			}
			else
				WriteSG(&packed, sizeof(ushort));
		}
	}

	if (FullSave)
	{
		byte = 0;
		item = &items[level_items];

		for (int i = level_items; i < 256; i++)
		{
			if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM ||
				item->object_number == CHAFF ||item->object_number == TORPEDO || item->object_number == CROSSBOW_BOLT))
				byte++;

			item++;
		}

		WriteSG(&byte, sizeof(uchar));
		item = &items[level_items];

		for (int i = level_items; i < 256; i++)
		{
			if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM ||
				item->object_number == CHAFF || item->object_number == TORPEDO || item->object_number == CROSSBOW_BOLT))
			{
				if (item->object_number == FLARE_ITEM)
					byte = 0;
				else if (item->object_number == BURNING_TORCH_ITEM)
					byte = 1;
				else if (item->object_number == CHAFF)
					byte = 2;
				else if (item->object_number == TORPEDO)
					byte = 3;
				else
					byte = 4;

				WriteSG(&byte, sizeof(char));
				WriteSG(&item->pos, sizeof(PHD_3DPOS));
				WriteSG(&item->room_number, sizeof(short));
				WriteSG(&item->speed, sizeof(short));
				WriteSG(&item->fallspeed, sizeof(short));

				if (item->object_number == FLARE_ITEM)
				{
					flare_age = (long)item->data;
					WriteSG(&flare_age, sizeof(long));
				}
				else if (item->object_number == BURNING_TORCH_ITEM)
					WriteSG(&item->item_flags[3], sizeof(short));
				else if (item->object_number == CHAFF || item->object_number == TORPEDO)
				{
					WriteSG(&item->item_flags[0], sizeof(short));
					WriteSG(&item->item_flags[1], sizeof(short));

					if (item->object_number == TORPEDO)
					{
						WriteSG(&item->current_anim_state, sizeof(short));
						WriteSG(&item->goal_anim_state, sizeof(short));
						WriteSG(&item->required_anim_state, sizeof(short));
					}
				}
			}

			item++;
		}

		for (int i = 0; i < 2; i++)
		{
			if (i)
			{
				rat = (RAT_STRUCT*)Spiders;
				objnum = SPIDER;
				maxval = 64;
			}
			else
			{
				rat = Rats;
				objnum = RAT;
				maxval = 32;
			}

			obj = &objects[objnum];

			if (obj->loaded)
			{
				byte = 0;

				for (int j = 0; i < maxval; i++)
				{
					if (rat[j].On)
						byte++;
				}

				WriteSG(&byte, sizeof(char));		//number of rats/spiders

				for (int j = 0; i < maxval; i++)
				{
					if (rat[j].On)
					{
						word = rat->room_number << 8;

						if (rat->pos.x_pos & 1)
							word |= 1;

						if (rat->pos.y_pos & 1)
							word |= 2;

						if (rat->pos.z_pos & 1)
							word |= 4;

						if (rat->pos.x_rot)
							word |= 8;

						WriteSG(&word, sizeof(short));
						pos = (short)(rat->pos.x_pos >> 1);
						WriteSG(&pos, sizeof(short));
						pos = (short)(rat->pos.y_pos >> 1);
						WriteSG(&pos, sizeof(short));
						pos = (short)(rat->pos.z_pos >> 1);
						WriteSG(&pos, sizeof(short));
						WriteSG(&rat->pos.y_rot, sizeof(short));

						if (word & 8)
							WriteSG(&rat->pos.x_rot, sizeof(short));

						WriteSG(&rat->flags, sizeof(uchar));
					}
				}
			}
		}

		obj = &objects[BAT];

		if (obj->loaded)
		{
			bat = Bats;
			byte = 0;

			for (int i = 0; i < 64; i++)
			{
				if (bat[i].On)
					byte++;
			}

			WriteSG(&byte, sizeof(char));	//number of bats

			for (int i = 0; i < 64; i++)
			{
				if (bat[i].On)
				{
					word = bat->room_number << 8;

					if (bat->pos.x_pos & 1)
						word |= 1;

					if (bat->pos.y_pos & 1)
						word |= 2;

					if (bat->pos.z_pos & 1)
						word |= 4;

					if (bat->pos.x_rot)
						word |= 8;

					WriteSG(&word, sizeof(short));	//what have we saved?
					pos = (short)(bat->pos.x_pos >> 1);
					WriteSG(&pos, sizeof(short));	//save xpos
					pos = (short)(bat->pos.y_pos >> 1);
					WriteSG(&pos, sizeof(short));	//ypos
					pos = (short)(bat->pos.z_pos >> 1);
					WriteSG(&pos, sizeof(short));	//zpos
					WriteSG(&bat->pos.y_rot, sizeof(short));	//yrot

					if (word & 8)
						WriteSG(&bat->pos.x_rot, sizeof(short));

					WriteSG(&bat->Counter, sizeof(short));
				}
			}
		}

		if (lara.RopePtr != -1)
			SaveRope();
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x00470EC0, WriteSG, replace);
	INJECT(0x00470F10, ReadSG, replace);
	INJECT(0x00470F60, CheckSumValid, replace);
	INJECT(0x00471050, SaveLaraData, replace);
	INJECT(0x004720B0, RestoreLaraData, replace);
	INJECT(0x004711E0, SaveLevelData, replace);
}
