#include "../tomb5/pch.h"
#include "cutseq.h"
#include "control.h"
#include "deltapak.h"
#include "objects.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "twogun.h"
#include "lara.h"
#include "gameflow.h"

char title_controls_locked_out;
static char iswappedit = 0;

static ushort special2_pistols_info[13] = { 196, 204, 212, 220, 228, 236, 244, 252, 260, 268, 276, 289, (ushort)-1 };
static ushort special3_pistols_info[4] = { 258, 266, 276, (ushort)-1 };

void _special1_control()
{
	if (GLOBAL_cutseq_frame == 300)
	{
		FlipMap(0);
		iswappedit = 1;
	}

	nSpecialCut = 1;
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
	ITEM_INFO* item;
	PHD_VECTOR pos;

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
		triggerwindowsmash(MAFIA_MIP);
		break;

	case 207:
		item = find_a_fucking_item(ANIMATING3_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;

	case 209:
		triggerwindowsmash(MAFIA2_MIP);
		break;

	case 221:
		item = find_a_fucking_item(ANIMATING4_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;

	case 224:
		triggerwindowsmash(SAILOR_MIP);
		break;

	case 229:
		Cutanimate(ANIMATING16);
		break;

	case 245:
		triggerwindowsmash(CRANE_GUY_MIP);
		break;

	case 261:
		item = find_a_fucking_item(ANIMATING5_MIP);
		ExplodeItemNode(item, 0, 0, 64);
		break;

	case 269:
		triggerwindowsmash(LION_MIP);
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
		triggerwindowsmash(GLADIATOR_MIP);
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
	SPARKS* sptr;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long g, b, f, a1, a2;

	pos.x = 12;
	pos.y = 200;
	pos.z = 92;
	deal_with_actor_shooting(special3_pistols_info, 1, 13, &pos);

	if (GLOBAL_cutseq_frame == 92 || GLOBAL_cutseq_frame == 93 || GLOBAL_cutseq_frame == 143 || GLOBAL_cutseq_frame == 144)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LMX_HIPS);
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
		f = 642 - GLOBAL_cutseq_frame;
		g = (((GetRandomControl() & 0x3F) + 64) * f) >> 5;
		b = (((GetRandomControl() & 0x3F) + 180) * f) >> 5;
		TriggerDynamic(pos2.x, pos2.y, pos2.z, (GetRandomControl() & 3) + 8, 0, g, b);

		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sB = (16 * ((GetRandomControl() & 0x7F) + 128)) >> 4;
		sptr->sR = sptr->sB - (sptr->sB >> 2);
		sptr->sG = sptr->sB - (sptr->sB >> 2);
		sptr->dR = 0;
		sptr->dB = (16 * ((GetRandomControl() & 0x7F) + 32)) >> 4;
		sptr->dG = sptr->dB >> 2;

		sptr->FadeToBlack = 8;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 24;
		sptr->sLife = sptr->Life;
		sptr->x = pos2.x;
		sptr->y = pos2.y;
		sptr->z = pos2.z;

		a1 = GetRandomControl() << 1;
		a2 = GetRandomControl() << 1;
		f = (1024 * phd_cos(a1)) >> W2V_SHIFT;
		sptr->Xvel = short(f * phd_sin(a2) >> W2V_SHIFT);
		sptr->Yvel = 256 * phd_sin(-a1) >> W2V_SHIFT;
		sptr->Zvel = short(f * phd_cos(a2) >> W2V_SHIFT);

		sptr->Friction = 0;
		sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x7F) - 64;
		sptr->MaxYvel = 0;
		sptr->Scalar = 2;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
		sptr->Size = (GetRandomControl() & 0x3F) + 16;
		sptr->sSize = sptr->Size;
		sptr->dSize = 1;
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
	long r, g, b;

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
	{
		r = (GetRandomControl() & 0x3F) + 31;
		g = (GetRandomControl() & 0xF) + 31;
	}
	else
	{
		r = (GetRandomControl() & 0x7F) + 127;
		g = (GetRandomControl() & 0x7F) + 127;
	}

	b = GetRandomControl() & 0x3F;
	TriggerDynamic(pos.x, pos.y, pos.z, 10, r, g, b);

	if (GLOBAL_cutseq_frame == 390)
		Cutanimate(STROBE_LIGHT);
}

void _special4_end()
{
	ResetCutanimate(STROBE_LIGHT);

	if (!bDoCredits)
	{
		trigger_title_spotcam(1);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void ResetCutItem(long item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = -1;
}

void resetwindowsmash(long item_num)
{
	find_a_fucking_item(item_num)->mesh_bits = 1;
}

void triggerwindowsmash(long item_num)
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(item_num);
	ExplodeItemNode(item, 0, 0, 64);
	item->mesh_bits = 2;
}

void FlamingHell(PHD_VECTOR* pos)
{
	SPARKS* sptr;
	long x, y, z, r;

	x = pos->x;
	y = pos->y - 128;
	z = pos->z;
	r = (GetRandomControl() & 0x1FF) - 128;

	if (r < 512)
		r = 512;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dB = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0x3F) + 90;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xFF) + x - 128;
	sptr->y = (GetRandomControl() & 0xFF) + y - 128;
	sptr->z = (GetRandomControl() & 0xFF) + z - 128;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = (short)-r;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->Scalar = 2;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->dSize = uchar((GetRandomControl() & 0xF) + (r >> 6) + 16);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;
	sptr->dSize += sptr->dSize >> 2;
}

void FireTwoGunTitleWeapon(PHD_VECTOR* pos1, PHD_VECTOR* pos2)
{
	TWOGUN_INFO* info;
	short angles[2];

	phd_GetVectorAngles(pos2->x - pos1->x, pos2->y - pos1->y, pos2->z - pos1->z, angles);

	for (int i = 0; i < 4; i++)
	{
		info = &twogun[i];

		if (!info->life || i == 3)
			break;
	}

	info->pos.x_pos = pos1->x;
	info->pos.y_pos = pos1->y;
	info->pos.z_pos = pos1->z;
	info->pos.x_rot = angles[1];
	info->pos.y_rot = angles[0];
	info->pos.z_rot = 0;
	info->life = 17;
	info->spin = short(GetRandomControl() << 11);
	info->dlength = 4096;
	info->r = 0;
	info->g = 96;
	info->b = -1;	//255
	info->fadein = 8;
	TriggerLightningGlow(info->pos.x_pos, info->pos.y_pos, info->pos.z_pos, RGBA(0, info->g >> 1, info->b >> 1, (GetRandomControl() & 3) + 64));
	TriggerLightning(pos1, pos2, (GetRandomControl() & 7) + 8, RGBA(0, info->g, info->b, 22), 12, 80, 5);
}
