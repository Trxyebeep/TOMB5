#include "../tomb5/pch.h"
#include "flmtorch.h"
#include "delstuff.h"
#include "../specific/function_stubs.h"
#include "effect2.h"
#include "sound.h"
#include "lara_states.h"
#include "objects.h"
#include "laraflar.h"

void DoFlameTorch()
{
	PHD_VECTOR pos;

	switch (lara.left_arm.lock)
	{
	case 0:	//holding it

		if (lara.request_gun_type != lara.gun_type)
		{
			lara.left_arm.lock = 2;	//drop it
			lara.left_arm.frame_number = 31;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index + 2;
		}
		else if (input & IN_DRAW && !lara_item->gravity_status && !lara_item->fallspeed && lara_item->current_anim_state != AS_COMPRESS &&
			lara_item->current_anim_state != AS_UPJUMP && lara_item->current_anim_state != AS_FORWARDJUMP &&
			lara_item->current_anim_state != AS_BACKJUMP && lara_item->current_anim_state != AS_RIGHTJUMP &&
			lara_item->current_anim_state != AS_LEFTJUMP || lara.water_status == LW_UNDERWATER)
		{
			lara.left_arm.lock = 1;	//throw it
			lara.left_arm.frame_number = 1;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index + 1;

			if (lara.water_status == LW_UNDERWATER)
				lara.LitTorch = 0;
		}

		break;

	case 1:	//throwing it

		if (lara.left_arm.frame_number < 12 && lara_item->gravity_status)
		{
			lara.left_arm.lock = 0;	//keep holding it
			lara.left_arm.frame_number = 0;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
		}
		else
		{
			lara.left_arm.frame_number++;

#ifdef GENERAL_FIXES	//fixes the jumpy hand when throwing the torch
			if (lara.left_arm.frame_number == 13)
#else
			if (lara.left_arm.frame_number == 27)
#endif
			{
				lara.LitTorch = 0;
				lara.flare_control_left = 0;
				lara.left_arm.lock = 0;
				lara.gun_type = lara.last_gun_type;
				lara.request_gun_type = WEAPON_NONE;
				lara.gun_status = LG_NO_ARMS;
			}
			else if (lara.left_arm.frame_number == 12)
			{
				lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + (LM_LHAND * 2)];
				CreateFlare(BURNING_TORCH_ITEM, 1);
			}
		}

		break;

	case 2:	//dropping it (when pulling out a weapon)

		lara.left_arm.frame_number++;

		if (lara.left_arm.frame_number == 41)
		{
			lara.LitTorch = 0;
			lara.flare_control_left = 0;
			lara.left_arm.lock = 0;
			lara.last_gun_type = WEAPON_NONE;
			lara.gun_type = WEAPON_NONE;
			lara.gun_status = LG_NO_ARMS;
		}
		else if (lara.left_arm.frame_number == 36)
		{
			lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + (LM_LHAND * 2)];
			CreateFlare(BURNING_TORCH_ITEM, 0);
		}

		break;

	case 3:	//lighting it

		if (lara_item->current_anim_state != AS_CONTROLLED)
		{
			lara.LitTorch = lara_item->item_flags[3];
			lara.flare_control_left = 1;
			lara.left_arm.lock = 0;
			lara.left_arm.frame_number = 0;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
		}

		break;
	}

	if (lara.flare_control_left)
		lara.gun_status = LG_READY;

	lara.left_arm.frame_base = anims[lara.left_arm.anim_number].frame_ptr;

	if (lara.LitTorch)
	{
		pos.x = -32;
		pos.y = 64;
		pos.z = 256;
		GetLaraJointPos(&pos, LM_HEAD);
		TriggerDynamic(pos.x, pos.y, pos.z, 12 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);

		if (!(wibble & 7))
		   TriggerTorchFlame(lara_item - items, 0);

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_3DPOS*)&pos, 0);
		torchroom = lara_item->room_number;
	}
}

void TriggerTorchFlame(short item_number, long node)
{
	SPARKS* sptr;
	long size;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	sptr->Flags = 4762;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->NodeNumber = (uchar)node;
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->FxObj = (uchar)item_number;
	sptr->Scalar = 1;
	size = (GetRandomControl() & 0x1F) + 80;
	sptr->Size = (uchar)size;
	sptr->sSize = sptr->Size;
	sptr->dSize = (uchar)(size >> 3);
}

void inject_flmtorch(bool replace)
{
	INJECT(0x00433EA0, DoFlameTorch, replace);
	INJECT(0x00433990, TriggerTorchFlame, replace);
}
