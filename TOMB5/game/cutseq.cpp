#include "../tomb5/pch.h"
#include "cutseq.h"
#include "control.h"
#include "deltapak.h"
#include "objects.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "delstuff.h"
#include "../specific/3dmath.h"

char iswappedit;//original one is on 0051CA84.

ushort special2_pistols_info[13] =
{
	0x00C4, 0x00CC, 0x00D4, 0x00DC, 0x00E4, 0x00EC, 0x00F4, 0x00FC, 0x0104, 0x010C,
	0x0114, 0x0121, 0xFFFF
};

ushort special3_pistols_info[] =
{
	0x0102, 0x010A, 0x0114, 0xFFFF, 0, 0, 0, 0, 0x00FF, 0x0100, 0x0114, 0xFF9C, 0x0064, 0x0100, 0x0200
};

void _special1_control()
{
	if (GLOBAL_cutseq_frame == 300)
	{
		FlipMap(0);
		iswappedit = 1;
	}

	dword_00E916F0 = 1;
}

void _special1_end()
{
	if (iswappedit)
	{
		FlipMap(0);
		iswappedit = 0;
	}

	if (!bDoCredits)
	{
		lara_item->mesh_bits = 0;
		trigger_title_spotcam(2);
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special2_control()
{
	PHD_VECTOR pos;
	ITEM_INFO* item;

	pos.x = 12;
	pos.y = 200;
	pos.z = 92;
	deal_with_actor_shooting(special2_pistols_info, 1, 13, &pos);

	switch (GLOBAL_cutseq_frame)
	{
	case 197:
		item = find_a_fucking_item(ANIMATING1_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 200:
		item = find_a_fucking_item(ANIMATING2_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 201:
		triggerwindowsmash(50);
		break;
	case 207:
		item = find_a_fucking_item(ANIMATING3_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 209:
		triggerwindowsmash(52);
		break;
	case 221:
		item = find_a_fucking_item(ANIMATING4_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 224:
		triggerwindowsmash(54);
		break;
	case 229:
		Cutanimate(446);
		break;
	case 245:
		triggerwindowsmash(56);
		break;
	case 261:
		item = find_a_fucking_item(ANIMATING5_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 269:
		triggerwindowsmash(58);
		break;
	case 280:
		item = find_a_fucking_item(ANIMATING6_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 282:
		item = find_a_fucking_item(ANIMATING7_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;
	case 284:
		Cutanimate(ANIMATING5);
		break;
	case 291:
		triggerwindowsmash(60);
		break;
	}
}

void _special2_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(3);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special3_control()
{
	PHD_VECTOR pos, pos2;
	SPARKS* sptr;

	pos.x = 12;
	pos.y = 200;
	pos.z = 92;
	deal_with_actor_shooting(special3_pistols_info, 1, 13, &pos);

	if (GLOBAL_cutseq_frame == 92 || GLOBAL_cutseq_frame == 93 || GLOBAL_cutseq_frame == 143 || GLOBAL_cutseq_frame == 144)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, 0);
		pos.z -= 256;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
	}

	if (GLOBAL_cutseq_frame == 472 || GLOBAL_cutseq_frame == 528)
	{
		pos.x = 8;
		pos.y = 230;
		pos.z = 40;
		GetActorJointAbsPosition(2, 5, &pos);

		pos2.x = 8;
		pos2.y = 4326;
		pos2.z = 40;
		GetActorJointAbsPosition(2, 5, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);
	}

	if (GLOBAL_cutseq_frame == 500)
	{
		pos.x = 0;
		pos.y = 230;
		pos.z = 40;
		GetActorJointAbsPosition(2, 8, &pos);

		pos2.x = 0;
		pos2.y = 4326;
		pos2.z = 40;
		GetActorJointAbsPosition(2, 8, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);

	}

	if (GLOBAL_cutseq_frame == 610)
	{
		pos.x = 0;
		pos.y = 2278;
		pos.z = 40;
		GetActorJointAbsPosition(2, 8, &pos);

		pos2.x = 0;
		pos2.y = 0;
		pos2.z = -1024;
		GetActorJointAbsPosition(1, 0, &pos2);

		FireTwoGunTitleWeapon(&pos, &pos2);
	}

	if (GLOBAL_cutseq_frame >= 610 && GLOBAL_cutseq_frame <= 642)
	{
		pos2.x = 0;
		pos2.y = 0;
		pos2.z = 0;
		GetActorJointAbsPosition(1, 0, &pos2);

		TriggerDynamic(pos2.x, pos2.y, pos2.z, (GetRandomControl() & 3) + 8, 0,
			(((GetRandomControl() & 0x3F) + 64) * (642 - GLOBAL_cutseq_frame)) >> 5,
			((642 - GLOBAL_cutseq_frame) * ((GetRandomControl() & 0x3F) + 180)) >> 5);

		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->dR = 0;
		sptr->sB = (16 * ((GetRandomControl() & 0x7F) + 128)) >> 4;
		sptr->sR = sptr->sB - (sptr->sB >> 2);
		sptr->sG = sptr->sB - (sptr->sB >> 2);
		sptr->dB = (16 * ((GetRandomControl() & 0x7F) + 32)) >> 4;
		sptr->dG = (uchar)sptr->dB >> 2;
		sptr->FadeToBlack = 8;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 24;
		sptr->sLife = (GetRandomControl() & 3) + 24;
		sptr->x = pos2.x;
		sptr->y = pos2.y;
		sptr->z = pos2.z;
		sptr->Xvel = ((phd_cos(2 * GetRandomControl()) >> 2) * phd_sin(GetRandomControl() * 2)) >> 14;
		sptr->Zvel = ((phd_cos(2 * GetRandomControl()) >> 2) * phd_cos(GetRandomControl() * 2)) >> 14;
		sptr->Yvel = phd_sin(-GetRandomControl() * 2) >> 4;
		sptr->Friction = 0;
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x7F) - 64;
		sptr->MaxYvel = 0;
		sptr->Scalar = 2;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
		sptr->dSize = 1;
		sptr->sSize = (GetRandomControl() & 0x3F) + 16;
		sptr->Size = (GetRandomControl() & 0x3F) + 16;
	}
}

void _special3_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(4);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void _special4_control()
{
	PHD_VECTOR pos;

	pos.x = 85834;
	pos.z = 72300;
	pos.y = -3138;

	TriggerFireFlame(85834, -3010, 72300, -1, 1);
	TriggerFireFlame(85834, -3010, 72044, -1, 1);
	TriggerFireFlame(85834, -3010, 72556, -1, 1);
	TriggerFireFlame(85578, -3010, 72300, -1, 1);
	TriggerFireFlame(86090, -3010, 72300, -1, 1);

	if (GLOBAL_cutseq_frame >= 460)
		FlamingHell(&pos);

	if (GLOBAL_cutseq_frame < 470)
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 31, (GetRandomControl() & 0xF) + 31, GetRandomControl() & 0x3F);
	else
		TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x7F) + 127, (GetRandomControl() & 0x7F) + 127, GetRandomControl() & 0x3F);

	if (GLOBAL_cutseq_frame == 390)
		Cutanimate(STROBE_LIGHT);
}

void _special4_end()
{
	if (!bDoCredits)
	{
		trigger_title_spotcam(1);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void ResetCutItem(int item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = -1;
}

void resetwindowsmash(int item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = 1;
}

void triggerwindowsmash(int item_num)
{
	ITEM_INFO* item = find_a_fucking_item(item_num);
	ExplodeItemNode(item, 0, 0, 64);
	item->mesh_bits = 2;
}

void FlamingHell(PHD_VECTOR* pos)
{
	SPARKS* sptr;
	int r, size;

	r = (GetRandomControl() & 0x1FF) - 128;

	if (r < 512)
		r = 512;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dB = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0x3F) + 90;
	sptr->sLife = (GetRandomControl() & 0x3F) + 90;
	sptr->x = (GetRandomControl() & 0xFF) + pos->x - 128;
	sptr->y = (GetRandomControl() & 0xFF) + (pos->y - 128) - 128;
	sptr->z = (GetRandomControl() & 0xFF) + pos->z - 128;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Flags = 538;
	sptr->Scalar = 2;
	size = (GetRandomControl() & 0xF) + (r >> 6) + 16;
	sptr->dSize = size;
	sptr->sSize = size >> 1;
	sptr->Size = size >> 1;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -(short)r;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->dSize += sptr->dSize >> 2;
}

void FireTwoGunTitleWeapon(PHD_VECTOR* pos1, PHD_VECTOR* pos2)
{
	TWOGUN_INFO* gun;
	short angles[2];

	phd_GetVectorAngles(pos2->x - pos1->x, pos2->y - pos1->y, pos2->z - pos1->z, &angles[0]);

	for (int i = 0; i < 4; i++)
	{
		gun = &twogun[i];

		if (!gun->life || i == 3)
			break;
	}

	gun->pos.x_pos = pos1->x;
	gun->pos.y_pos = pos1->y;
	gun->pos.z_pos = pos1->z;
	gun->pos.y_rot = angles[0];
	gun->pos.x_rot = angles[1];
	gun->pos.z_rot = 0;
	gun->life = 17;
	gun->spin = (short)(GetRandomControl() << 11);
	gun->dlength = 4096;
	gun->r = 0;
	gun->g = 96;
	gun->b = -1;
	gun->fadein = 8;
	TriggerLightningGlow(gun->pos.x_pos, gun->pos.y_pos, gun->pos.z_pos, ((GetRandomControl() & 3) + 64) << 24 | ((uchar)gun->g << 7) | ((uchar)gun->b >> 1));
	TriggerLightning(pos1, pos2, (GetRandomControl() & 7) + 8, ((uchar)gun->g | 0x160000) << 8 | (uchar)gun->b, 12, 80, 5);
}

void inject_cutseq(bool replace)
{
	INJECT(0x0041BCF0, _special1_control, replace);
	INJECT(0x0041BD40, _special1_end, replace);
	INJECT(0x0041BE90, _special2_control, replace);
	INJECT(0x0041C0F0, _special2_end, replace);
	INJECT(0x0041C160, _special3_control, replace);
	INJECT(0x0041C610, _special3_end, replace);
	INJECT(0x0041C690, _special4_control, replace);
	INJECT(0x0041C860, _special4_end, replace);
	INJECT(0x0041C8B0, ResetCutItem, replace);
	INJECT(0x0041C920, resetwindowsmash, replace);
	INJECT(0x0041C8E0, triggerwindowsmash, replace);
	INJECT(0x0041C950, FlamingHell, replace);
	INJECT(0x0041CB10, FireTwoGunTitleWeapon, replace);
}
