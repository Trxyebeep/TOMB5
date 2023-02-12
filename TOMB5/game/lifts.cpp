#include "../tomb5/pch.h"
#include "lifts.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "control.h"
#include "switch.h"
#include "objects.h"
#include "items.h"
#include "hair.h"
#include "sound.h"
#include "camera.h"
#include "spotcam.h"
#include "lara.h"

void DrawLiftDoors(ITEM_INFO* item)
{
	PHD_VECTOR v;
	short** meshpp;
	short* frmptr[2];
	long clip, rate;

	if (item->item_flags[0])
	{
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_bottom = phd_winheight;
		GetFrames(item, frmptr, &rate);
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		clip = S_GetObjectBounds(frmptr[0]);

		if (clip)
		{
			meshpp = &meshes[objects[item->object_number].mesh_index];
			phd_TranslateRel(frmptr[0][6], frmptr[0][7], frmptr[0][8]);
			v.x = item->item_flags[0] << 2;
			v.y = 16384;
			v.z = 16384;
			ScaleCurrentMatrix(&v);
			CalculateObjectLighting(item, frmptr[0]);
			phd_PutPolygons(*meshpp, clip);
		}

		phd_bottom = phd_winheight;
		phd_right = phd_winwidth;
		phd_left = 0;
		phd_top = 0;
		phd_PopMatrix();
	}
}

void ControlLiftTeleporter(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long num;
	short triggeredItems[8];
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (!item->item_flags[1])
	{
		if (!item->item_flags[0])
		{
			for (num = GetSwitchTrigger(lara_item, triggeredItems, 1); num > 0; num--)
			{
				item2 = &items[triggeredItems[num - 1]];

				if (item2->object_number >= SWITCH_TYPE1 && item2->object_number <= SWITCH_TYPE6 && item2->trigger_flags == 6)
				{
					item2->mesh_bits = (4 << (18 - 2 * (item2->item_flags[3] & 0xF))) |
						(4 << (18 - ((item2->item_flags[3] >> 3) & 0x1E))) |
						~(2 << (18 - 2 * (item2->item_flags[3] & 0xF))) &
						~(2 << (18 - ((item2->item_flags[3] >> 3) & 0x1E))) & 0xAAAAB;
					item->item_flags[3] = item2->item_flags[3];
					break;
				}
			}
		}

		if (item->item_flags[0] >= 30)
		{
			item->item_flags[0] = 0;
			item->item_flags[1] = 1;
		}
		else
			item->item_flags[0]++;
	}
	else if (item->item_flags[1] == 1)
	{
		camera.bounce = -32;
		camera.fixed_camera = 1;
		camera.old_type = FIXED_CAMERA;
		room_number = item->room_number;
		lara_item->pos.x_pos = (item->pos.x_pos & -1024) | (lara_item->pos.x_pos & 1023);
		lara_item->pos.y_pos = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
			item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		lara_item->pos.z_pos = (item->pos.z_pos & -1024) | (lara_item->pos.z_pos & 1023);
		lara_item->pos.y_rot = item->pos.y_rot + 0x8000;

		if (lara_item->room_number != room_number)
			ItemNewRoom(lara.item_number, room_number);

		InitialiseHair();

		for (num = GetSwitchTrigger(item, triggeredItems, 1); num > 0; num--)
		{
			item2 = &items[triggeredItems[num - 1]];

			if (item2->object_number >= SWITCH_TYPE1 && item2->object_number <= SWITCH_TYPE6 && item2->trigger_flags == 6)
			{
				item2->current_anim_state = 0;
				item2->goal_anim_state = 0;
				item2->anim_number = objects[item2->object_number].anim_index + 1;
				item2->frame_number = anims[item2->anim_number].frame_base;
				item2->mesh_bits = (4 << (18 - 2 * (item->item_flags[3] & 0xF))) |
					(4 << (18 - ((item->item_flags[3] >> 3) & 0x1E))) |
					~(2 << (18 - 2 * (item->item_flags[3] & 0xF))) &
					~(2 << (18 - ((item->item_flags[3] >> 3) & 0x1E))) & 0xAAAAB;
				break;
			}
		}

		item->item_flags[1] = 2;
		bDisableLaraControl = 0;
	}
	else if (item->item_flags[1] == 2)
	{
		if (item->item_flags[0] >= 120)
		{
			for (num = GetSwitchTrigger(item, triggeredItems, 1); num > 0; num--)
			{
				item2 = &items[triggeredItems[num - 1]];

				if (item2->object_number >= SWITCH_TYPE1 && item2->object_number <= SWITCH_TYPE6 && item2->trigger_flags == 6)
				{
					item2->current_anim_state = 1;
					item2->goal_anim_state = 1;
					item2->anim_number = objects[item2->object_number].anim_index;
					item2->frame_number = anims[item2->anim_number].frame_base;
					item2->status = ITEM_INACTIVE;
				}
				else if (item2->object_number >= LIFT_DOORS1 && item2->object_number <= LIFT_DOORS2)
				{
					AddActiveItem(triggeredItems[num - 1]);
					item2->flags |= IFL_CODEBITS;
					item2->status = ITEM_ACTIVE;
				}
			}

			camera.bounce = -32;
			RemoveActiveItem(item_number);
			item->flags &= ~IFL_CODEBITS;
			item->status = ITEM_INACTIVE;
			item->item_flags[1] = 0;
			item->item_flags[0] = 0;
		}
		else
		{
			if (!item->item_flags[0])
				SoundEffect(SFX_LIFT_MOVE, 0, SFX_ALWAYS);

			camera.bounce = -8;
			item->item_flags[0]++;
		}
	}
}

void inject_lifts(bool replace)
{
	INJECT(0x0045AAF0, DrawLiftDoors, replace);
	INJECT(0x0045A560, ControlLiftTeleporter, replace);
}
