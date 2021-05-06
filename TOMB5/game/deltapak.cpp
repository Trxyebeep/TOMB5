#include "../tomb5/pch.h"
#include "lara.h"
#include "lara_states.h"
#include "items.h"
#include "deltapak.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "effect2.h"
#include "control.h"
#include "lot.h"
#include "tomb4fx.h"
#include "../specific/specific.h"
#include "camera.h"
#include "gameflow.h"
#include "hair.h"
#include "delstuff.h"
#include "effects.h"
#include "../specific/display.h"
#include "sphere.h"
#include "health.h"
#include "objects.h"
#include "newinv2.h"
#include "xatracks.h"
#include "spotcam.h"
#include <d3dtypes.h>
#include "cutseq.h"
#include "../specific/light.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "draw.h"

short frig_shadow_bbox[6] =
{
	-165, 150, -777, 1, -87, 78
};

unsigned short larson_pistols_info1[2] =
{
	0x0390, 0xFFFF
};

short larson_chat_ranges1[12] =
{
	0x03AF, 0x03CB, 0x04AC, 0x04C9, 0x04DE, 0x0516, 0x05EC, 0x060C, 0x071A, 0x0762,
	-1, -1
};

short larson_chat_ranges2[8] =
{
	0x01C2, 0x0255, 0x04DE, 0x0533, 0x082A, 0x0863, -1, -1
};

short pierre_chat_ranges1[30] =
{
	0x02B8, 0x02C4, 0x02D5, 0x02E8, 0x03EB, 0x03FC, 0x0414, 0x0423, 0x0438, 0x0499,
	0x052A, 0x056C, 0x0578, 0x05C0, 0x061B, 0x0630, 0x063A, 0x064A, 0x0668, 0x0706,
	0x0783, 0x07A7, 0x07B6, 0x07D8, 0x07E2, 0x07ED, 0x07F5, 0x0814, -1, -1
};

short pierre_chat_ranges2[34] =
{
	0x00F2, 0x0121, 0x012E, 0x0155, 0x0348, 0x0352, 0x035F, 0x03B2, 0x041B, 0x0427,
	0x043A, 0x047F, 0x048F, 0x04C7, 0x0647, 0x065C, 0x066F, 0x067D, 0x0693, 0x06A6,
	0x06BD, 0x0738, 0x0960, 0x097D, 0x0A80, 0x0AC9, 0x0C60, 0x0C73, 0x0C85, 0x0C9E,
	0x0D46, 0x0D74, -1, -1
};

short lara_chat_ranges_andrea1[4] =
{
	0x0248, 0x0279, -1, -1
};

short lara_chat_ranges_andrea2[32] =
{
	0x03D6, 0x03E6, 0x03EF, 0x0401, 0x05B1, 0x05E6, 0x077B, 0x07AE, 0x07BA, 0x07F5,
	0x0803, 0x0817, 0x0881, 0x08D1, 0x08DE, 0x08FD, 0x090B, 0x092A, 0x0AEE, 0x0B07,
	0x0BFA, 0x0C20, 0x0CBA, 0x0D35, 0x0D8F, 0x0DED, 0x0E01, 0x0E5E, 0x0E9B, 0x0F47,
	-1, -1
};

unsigned short andrea3_pistols_info[5] =
{
	0x454B, 0x854E, 0x8557, 0x4557, 0xFFFF
};

short lara_chat_ranges_andrea3[14] =
{
	0x0028, 0x005E, 0x0104, 0x0141, 0x01AC, 0x01BF, 0x01E3, 0x020F, 0x02C2, 0x033D,
	0x035A, 0x03AC, -1, -1
};

short larson_chat_ranges3[14] =
{
	0x00A0, 0x00F1, 0x0152, 0x0199, 0x022F, 0x02B1, 0x03CB, 0x0462, 0x0472, 0x0512,
	0x05B4, 0x05F3, -1, -1
};

short lara_chat_ranges_andrea3b[12] =
{
	0x020A, 0x021B, 0x0223, 0x0238, 0x02BE, 0x02E0, 0x02E9, 0x0302, 0x0318, 0x03D2,
	-1, -1
};

short larson_chat_ranges3b[4] =
{
	0x0254, 0x02AD, -1, -1
};

short lara_chat_ranges_andrea4[22] =
{
	0x01C9, 0x021C, 0x03B1, 0x03FF, 0x064B, 0x0677, 0x0686, 0x0694, 0x069D, 0x06BE,
	0x0843, 0x0867, 0x0C02, 0x0C0D, 0x0DA3, 0x0DEB, 0x0DFE, 0x0E1F, 0x0E69, 0x0EAA,
	-1, -1
};

short pierre_chat_ranges4[48] =
{
	0x014E, 0x015B, 0x0168, 0x01A8, 0x022E, 0x0258, 0x0267, 0x0304, 0x0315, 0x038A,
	0x0424, 0x042D, 0x0439, 0x045C, 0x046E, 0x04DE, 0x04EC, 0x0504, 0x0511, 0x052B,
	0x053C, 0x056A, 0x057E, 0x05F5, 0x060F, 0x063C, 0x06E5, 0x0739, 0x074A, 0x0809,
	0x0A3F, 0x0A50, 0x0A66, 0x0A82, 0x0AC3, 0x0B23, 0x0B67, 0x0BC2, 0x0CE1, 0x0D22,
	0x0D3E, 0x0D62, 0x0E3A, 0x0E53, 0x0F16, 0x0F1C, -1, -1
};

unsigned short crane_pistols_info[11] =
{
	0x8035, 0x803D, 0x81C5, 0x81CF, 0x81EA, 0x4031, 0x403A, 0x41C9, 0x41DE, 0x41EE,
	0xFFFF
};

unsigned short craneguard_pistols_info[7] =
{
	0x006A, 0x0075, 0x00F0, 0x00F7, 0x0104, 0x0112, 0xFFFF
};

short voncroy_chat_ranges_richcut2[20] =
{
	0x01E1, 0x01F5, 0x0290, 0x02AC, 0x02B7, 0x02F7, 0x0300, 0x033E, 0x039F, 0x03CA,
	0x03D7, 0x0408, 0x0414, 0x0443, 0x05C6, 0x0628, 0x0638, 0x0663, -1, -1
};

short associate_chat_ranges_richcut2[12] =
{
	0x045D, 0x0515, 0x0681, 0x0723, 0x0731, 0x0741, 0x074A, 0x0764, 0x0772, 0x07AA,
	-1, -1
};

short guard_chat_ranges_richcut2[10] =
{
	0x0036, 0x00B0, 0x0205, 0x0287, 0x034F, 0x0392, 0x0529, 0x054C, -1, -1
};

short lara_chat_ranges_andy1[14] =
{
	0x0312, 0x0333, 0x054B, 0x0555, 0x056D, 0x0588, 0x0868, 0x087D, 0x0C25, 0x0C55,
	0x0C6A, 0x0C74, -1, -1
};

short hanged_man_chat_ranges_andy1[18] =
{
	0x01EE, 0x022D, 0x023E, 0x02F1, 0x0348, 0x052A, 0x0596, 0x084C, 0x0890, 0x0BFB,
	0x0C81, 0x0C8F, 0x0C9C, 0x0D4C, 0x0D5A, 0x0F16, -1, -1
};

short lara_chat_ranges_andy2[8] =
{
	0x05BA, 0x05D2, 0x05E5, 0x05FB, 0x07D9, 0x07FD, -1, -1
};

short priest_chat_ranges_andy2[24] =
{
	0x00EF, 0x014B, 0x02E0, 0x0321, 0x0332, 0x0367, 0x050D, 0x051B, 0x052C, 0x059D,
	0x069A, 0x06B4, 0x06C8, 0x074E, 0x0765, 0x07B8, 0x083D, 0x086B, 0x087D, 0x08EF,
	0x0916, 0x0980, -1, -1
};

short lara_chat_ranges_andy6[8] =
{
	0x02C8, 0x02D7, 0x02EF, 0x0303, 0x034C, 0x037A, -1, -1
};

short priest_chat_ranges_andy6[18] =
{
	0x0208, 0x02A0, 0x0319, 0x0338, 0x039C, 0x03AA, 0x03B4, 0x03D1, 0x03E0, 0x0471,
	0x047F, 0x04AD, 0x04C4, 0x0512, 0x051D, 0x0565, -1, -1
};

short lara_chat_ranges_andy7[14] =
{
	0x017D, 0x0189, 0x0195, 0x01BE, 0x023E, 0x026D, 0x02EA, 0x0307, 0x054B, 0x0564,
	0x056D, 0x0582, -1, -1
};

short priest_chat_ranges_andy7[32] =
{
	0x0003, 0x0018, 0x0023, 0x0074, 0x0088, 0x00A7, 0x00BB, 0x010F, 0x011A, 0x0159,
	0x01C9, 0x01FC, 0x0208, 0x0234, 0x0286, 0x02D2, 0x0310, 0x0343, 0x0356, 0x03DD,
	0x03F1, 0x0404, 0x041C, 0x045C, 0x0477, 0x047F, 0x0487, 0x04D7, 0x04E8, 0x0528,
	-1, -1
};

short lara_chat_ranges_andy9[6] =
{
	0x041A, 0x043A, 0x0F2C, 0x0F7F, -1, -1
};

short priest_chat_ranges_andy9[20] =
{
	0x024C, 0x0270, 0x0281, 0x02A7, 0x02BB, 0x0310, 0x0679, 0x06B4, 0x06C7, 0x0731,
	0x073F, 0x077A, 0x0AA3, 0x0ACE, 0x0AF0, 0x0B38, 0x0F8F, 0x0FE8, -1, -1
};

short knight_chat_ranges_andy9[14] =
{
	0x0130, 0x0228, 0x031E, 0x0389, 0x0449, 0x0634, 0x07A4, 0x0A83, 0x0B51, 0x0F0E,
	0x1012, 0x11F0, -1, -1
};

short lara_chat_ranges_andy11[28] =
{
	0x083F, 0x0851, 0x0CB0, 0x0CC6, 0x0CD5, 0x0CEA, 0x0CF9, 0x0D15, 0x0D1F, 0x0D35,
	0x0D3E, 0x0D50, 0x0D6B, 0x0DBE, 0x0E5B, 0x0E6C, 0x0E8B, 0x0EA2, 0x1021, 0x103A,
	0x12E3, 0x12F7, 0x1510, 0x152F, 0x16F2, 0x1715, -1, -1
};

short priest_chat_ranges_andy11[30] =
{
	0x0427, 0x044B, 0x045F, 0x0486, 0x06B6, 0x0734, 0x09DA, 0x0A15, 0x0B4F, 0x0B65,
	0x0B78, 0x0BB3, 0x0DD6, 0x0E22, 0x1164, 0x11D4, 0x11E5, 0x1271, 0x127C, 0x12AC,
	0x1311, 0x13C8, 0x1549, 0x15E4, 0x1633, 0x1620, 0x1640, 0x16C0, -1, -1
};
short knight_chat_ranges_andy11[14] =
{
	0x01B4, 0x03BE, 0x04C7, 0x0698, 0x0890, 0x08C4, 0x08D6, 0x0942, 0x1081, 0x1105,
	0x13E9, 0x150A, -1, -1
};

short admiral_chat_ranges_joby2[12] =
{
	0x0118, 0x0154, 0x015C, 0x0175, 0x017C, 0x019D, 0x02CF, 0x0376, 0x0457, 0x0488,
	-1, -1
};

short sergie_chat_ranges_joby2[8] =
{
	0x01B9, 0x02B8, 0x0386, 0x041E, 0x07A2, 0x0868, -1, -1
};

short lara_chat_ranges_joby3[6] =
{
	0x052B, 0x0530, 0x053D, 0x0556, -1, -1
};

short lara_chat_ranges_joby4[10] =
{
	0x0244, 0x0267, 0x0279, 0x0302, 0x074F, 0x0786, 0x0796, 0x07AB, -1, -1
};

short admiral_chat_ranges_joby4[20] =
{
	0x012D, 0x0187, 0x0193, 0x0203, 0x030F, 0x0330, 0x033C, 0x0404, 0x040E, 0x04E6,
	0x04F1, 0x0586, 0x06EB, 0x070C, 0x07EC, 0x084B, 0x0857, 0x087A, -1, -1
};

short admiral_chat_ranges_joby5[18] =
{
	0x01EB, 0x0242, 0x0369, 0x0459, 0x04DC, 0x0505, 0x0518, 0x0529, 0x0535, 0x05EE,
	0x0757, 0x07B9, 0x07C3, 0x07F5, 0x0839, 0x08C4, -1, -1
};

short sergie_chat_ranges_joby5[20] =
{
	0x009C, 0x00B9, 0x00C2, 0x018B, 0x019C, 0x01CD, 0x025A, 0x0349, 0x0469, 0x0480,
	0x0492, 0x04CC, 0x0605, 0x0616, 0x0627, 0x0673, 0x0681, 0x06F8, -1, -1
};

short lara_chat_ranges_joby7[10] =
{
	0x02D5, 0x02F7, 0x030D, 0x0339, 0x05CD, 0x05DD, 0x05EC, 0x0609, -1, -1
};

short lara_chat_ranges_joby8[20] =
{
	0x03BC, 0x042D, 0x04ED, 0x053A, 0x0550, 0x0597, 0x05AB, 0x05BE, 0x05CD, 0x05E0,
	0x05F6, 0x0620, 0x070C, 0x072F, 0x0897, 0x08CF, 0x0913, 0x0940, -1, -1
};

short actor_chat_ranges_joby8[14] =
{
	0x02B3, 0x03A8, 0x0447, 0x04E2, 0x064F, 0x068F, 0x069F, 0x06ED, 0x076D, 0x0799,
	0x07A6, 0x07F7, -1, -1
};

short lara_chat_ranges_joby9[12] =
{
	0x0282, 0x030C, 0x058A, 0x05A6, 0x05B1, 0x05DB, 0x05E6, 0x060A, 0x0619, 0x0691,
	-1, -1
};

short admiral_chat_ranges_joby9[36] =
{
	0x00B6, 0x015B, 0x0177, 0x0258, 0x033A, 0x04F5, 0x0509, 0x052D, 0x053C, 0x054D,
	0x055A, 0x057F, 0x06A3, 0x06D2, 0x06E0, 0x076E, 0x077E, 0x07A4, 0x07B6, 0x07F5,
	0x0807, 0x085C, 0x087A, 0x08AF, 0x08BC, 0x08CF, 0x08DE, 0x08F2, 0x08FE, 0x0977,
	0x0988, 0x09B4, 0x09C5, 0x09D0, -1, -1
};

short lara_chat_ranges_joby10[12] =
{
	0x01ED, 0x020C, 0x0514, 0x052F, 0x0769, 0x078D, 0x09D9, 0x09FB, 0x0BA4, 0x0BC0,
	-1, -1
};

short admiral_chat_ranges_joby10[42] =
{
	0x0091, 0x00A3, 0x00D0, 0x00F0, 0x00FB, 0x0129, 0x0134, 0x0166, 0x0174, 0x01BD,
	0x01C8, 0x01D5, 0x0222, 0x0331, 0x0340, 0x0398, 0x03A7, 0x04FD, 0x0539, 0x0563,
	0x0570, 0x060F, 0x061E, 0x06E8, 0x06F3, 0x0742, 0x07A0, 0x07DC, 0x07EB, 0x08BB,
	0x08CC, 0x0983, 0x0A3E, 0x0A62, 0x0A74, 0x0A80, 0x0A8F, 0x0AB3, 0x0ABF, 0x0B0D,
	-1, -1
};

CUTSEQ_ROUTINES cutseq_control_routines[45] =
{
	{ 0, 0, 0 },
	{ stealth3_start, 0, stealth3_end },
	{ stealth3_start, 0, stealth3_end },
	{ stealth3_start, 0, stealth3_end },
	{ stealth3_start, 0, stealth3_end },
	{ joby6_init, joby6_control, joby6_end },
	{ andy5_init, andy5_control, andy5_end },
	{ andrea3b_init, andrea3b_control, andrea3b_end },
	{ andrea3_init, andrea3_control, andrea3_end },
	{ andy4b_init, andy4b_control, andy4b_end },
	{ andy4_init, andy4_control, andy4_end },
	{ richcut4_init, richcut4_control, richcut4_end },
	{ joby10_init, joby10_control, joby10_end },
	{ joby9_init, joby9_control, joby9_end },
	{ andy3_init, andy3_control, andy3_end },
	{ joby5_init, joby5_control, joby5_end },
	{ andrea2_init, andrea2_control, andrea2_end },
	{ andrea1_init, andrea1_control, andrea1_end },
	{ joby4_init, joby4_control, joby4_end },
	{ andy2_init, andy2_control, andy2_end },
	{ hamgate_init, hamgate_control, hamgate_end },
	{ andy1_init, andy1_control, andy1_end },
	{ joby3_init, joby3_control, joby3_end },
	{ richcut3_init, richcut3_control, richcut3_end },
	{ richcut1_init, richcut1_control, richcut1_end },
	{ joby2_init, joby2_control, joby2_end },
	{ richcut2_init, richcut2_control, richcut2_end },
	{ cranecut_init, cranecut_control, cranecut_end },
	{ special1_init, special1_control, special1_end },
	{ special2_init, special2_control, special2_end },
	{ special3_init, special3_control, special3_end },
	{ special4_init, special4_control, special4_end },
	{ joby8_init, joby8_control, joby8_end },
	{ andy6_init, andy6_control, andy6_end },
	{ andypew_init, andypew_control, andypew_end },
	{ andy7_init, andy7_control, andy7_end },
	{ cossack_init, cossack_control, cossack_end },
	{ andy9_init, andy9_control, andy9_end },
	{ andy8_init, andy8_control, andy8_end },
	{ andy10_init, andy10_control, andy10_end },
	{ joby7_init, joby7_control, joby7_end },
	{ andrea4_init, andrea4_control, andrea4_end },
	{ monk2_init, monk2_control, monk2_end },
	{ swampy_init, swampy_control, swampy_end },
	{ andy11_init, andy11_control, andy11_end },
};

void andrea1_init()
{
	return;
}

void andrea1_control()
{
	PHD_VECTOR pos;
	ITEM_INFO* item;

	switch (GLOBAL_cutseq_frame)
	{
	case 330:
		item = find_a_fucking_item(ANIMATING10);
		item->flags |= 0x20;
		item->mesh_bits = item->mesh_bits & 0xFFFFFFFD | 4;
		break;
	case 452:
		cutseq_givelara_pistols();
		break;
	case 580:
		undraw_pistol_mesh_left(1);
		break;
	case 603:
		undraw_pistol_mesh_right(1);
		lara.holster = old_lara_holster;
		break;
	case 705:
		lara_item->mesh_bits = 0;
		break;
	}

	pos.x = -50;
	pos.y = 200;
	pos.z = 0;
	deal_with_actor_shooting(larson_pistols_info1, 1, 14, &pos);
	handle_lara_chatting(lara_chat_ranges_andrea1);
	handle_actor_chatting(21, 8, 1, LARSON, larson_chat_ranges1);
	handle_actor_chatting(23, 8, 2, ANIMATING2_MIP, pierre_chat_ranges1);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea1_end()
{
	lara_item->mesh_bits = -1;
	lara.pickupitems &= 0xFFFE;
	
	return;
}

void andrea2_init()
{
	cutseq_meshbits[1] &= 0x7FFFFFFF;
	return;
}

void andrea2_control()
{
	int frame;
	ITEM_INFO* item;
	PHD_VECTOR pos;

	frame = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame == 410)
		cutseq_meshbits[1] |= 0x80000000;
	else if (GLOBAL_cutseq_frame == 178)
		cutseq_givelara_pistols();
	else if (GLOBAL_cutseq_frame == 667)
		undraw_pistol_mesh_left(1);
	else if (GLOBAL_cutseq_frame == 678)
	{
		undraw_pistol_mesh_right(1);
		lara.holster = old_lara_holster;
	}
	else if (GLOBAL_cutseq_frame == 2500)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 2797)
		lara_item->mesh_bits = -1;
	else if (GLOBAL_cutseq_frame == 2522)
	{
		item = find_a_fucking_item(ANIMATING10);
		item->flags |= 0x20;
		item->mesh_bits = item->mesh_bits & 0xFFFFFFF7 | 16;
	}
	else
	{
		if (GLOBAL_cutseq_frame > 2934)
		{
			pos.x = 0;
			pos.y = GetRandomControl() & 0x7F;
			pos.z = 0;
			GetActorJointAbsPosition(1, 2, &pos);
			DelTorchFlames(&pos);

			pos.x = 0;
			pos.y = GetRandomControl() & 0x7F;
			pos.z = 0;
			GetActorJointAbsPosition(1, 5, &pos);
			DelTorchFlames(&pos);

			pos.x = 0;
			pos.y = GetRandomControl() & 0x1F;
			pos.z = 0;
			GetActorJointAbsPosition(1, 3, &pos);
			DelTorchFlames(&pos);

			pos.x = 0;
			pos.y = GetRandomControl() & 0x1F;
			pos.z = 0;
			GetActorJointAbsPosition(1, 6, &pos);
			DelTorchFlames(&pos);

			if (frame > 3179)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetActorJointAbsPosition(1, 11, &pos);
				DelTorchFlames(&pos);
			}
		}

		if ((frame > 2933) && (frame < 2981))
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 1024;
			GetActorJointAbsPosition(1, 11, &pos);

			if (GetRandomControl() & 1)
				TriggerDynamic(pos.x, pos.y, pos.z, 24,
					((GetRandomControl() & 0x3F) + 192) >> (((frame - 2934) >> 4) & 0x1F),
					((GetRandomControl() & 0x1F) + 128) >> (((frame - 2934) >> 4) & 0x1F),
					(GetRandomControl() & 0x3F) >> (((frame - 2934) >> 4) & 0x1F));
			else
				TriggerDynamic(pos.x, pos.y, pos.z, 31,
					((GetRandomControl() & 0x3F) + 192) >> (((frame - 2934) >> 4) & 0x1F),
					((GetRandomControl() & 0x1F) + 128) >> (((frame - 2934) >> 4) & 0x1F),
					(GetRandomControl() & 0x3F) >> (((frame - 2934) >> 4) & 0x1F));
		}
	}

	handle_lara_chatting(lara_chat_ranges_andrea2);
	handle_actor_chatting(21, 8, 1, LARSON, larson_chat_ranges2);
	handle_actor_chatting(23, 8, 2, ANIMATING2_MIP, pierre_chat_ranges2);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
	return;
}

void andrea2_end()
{
	lara.pickupitems &= 0xFFFD;
	return;
}

void andrea3_init()
{
	cutseq_givelara_pistols();
	return;
}

void andrea3_control()
{
	if (GLOBAL_cutseq_frame == 332)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 421)
		lara_item->mesh_bits = -1;

	deal_with_pistols(andrea3_pistols_info);
	handle_lara_chatting(lara_chat_ranges_andrea3);
	handle_actor_chatting(21, 8, 1, LARSON, larson_chat_ranges3);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea3_end()
{
	ITEM_INFO* item;

	cutseq_removelara_pistols();

	item = find_a_fucking_item(LARSON);
	item->anim_number = objects[LARSON].anim_index;
	item->flags |= IFLAG_INVISIBLE;
	item->frame_number = anims[objects[LARSON].anim_index].frame_base;
	item->status = ITEM_ACTIVE;
	AddActiveItem(item - items);
	EnableBaddieAI(item - items, 1);
	DelsHandyTeleportLara(55808, 0, 30208, 32768);
}

void andrea3b_init()
{
	cutseq_givelara_pistols();
	cutseq_kill_item(ANIMATING10);
	cutseq_meshbits[2] = 0x80000F00;
	cutseq_meshbits[3] = 0x80000F00;
	cutseq_meshbits[4] = 0x80000F00;

	ITEM_INFO* item = find_a_fucking_item(LARSON);
	item->status = ITEM_INVISIBLE;
	RemoveActiveItem(item - items);
	DisableBaddieAI(item - items);
	item->flags |= IFLAG_INVISIBLE;
	disable_horizon = 1;
}

void andrea3b_control()
{
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame == 301 || GLOBAL_cutseq_frame == 639 || GLOBAL_cutseq_frame == 781)
		lara_item->mesh_bits = 0;

	if (f == 512 || f == 692 || f == 1084)
		lara_item->mesh_bits = -1;

	if (f == 1191)
	{
		cutseq_meshbits[1] &= 0x7FFFFFFF;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
	}
	else if (f == 550)
	{
		cutseq_meshbits[2] = 0xFFFFFFFF;
		cutseq_meshbits[3] = 0xFFFFFFFF;
		cutseq_meshbits[4] = 0xFFFFFFFF;
	}
	else if (f == 1150)
	{
		cutseq_meshbits[2] &= 0x7FFFFFFF;
		cutseq_meshbits[3] &= 0x7FFFFFFF;
		cutseq_meshbits[4] &= 0x7FFFFFFF;
	}

	handle_lara_chatting(lara_chat_ranges_andrea3b);
	handle_actor_chatting(21, 8, 1, LARSON, larson_chat_ranges3b);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea3b_end()
{
	disable_horizon = 0;
	cutseq_removelara_pistols();
	if (level_items > 0)
	{
		for (int i = 0; i < level_items; i++)
		{
			ITEM_INFO* item = &items[i];
			if (item->object_number == HYDRA)
			{
				item->status = ITEM_ACTIVE;
				item->flags |= IFLAG_INVISIBLE;
				AddActiveItem(item - items);
				EnableBaddieAI(item - items, 1);
			}
		}
	}
	DelsHandyTeleportLara(56846, 0, 26986, 50176);
}

void andrea4_init()
{
	return;
}

void andrea4_control()
{
	if (GLOBAL_cutseq_frame == 3134)
	{
		cutseq_givelara_pistols();
		undraw_pistol_mesh_left(1);
	}
	else if (GLOBAL_cutseq_frame == 3169)
		cutseq_givelara_pistols();
	else if (GLOBAL_cutseq_frame == 3173)
		do_pierre_gun_meshswap();

	handle_lara_chatting(lara_chat_ranges_andrea4);
	handle_actor_chatting(23, 8, 1, PIERRE, pierre_chat_ranges4);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea4_end()
{
	cutseq_removelara_pistols();
}

void cranecut_init()
{
	cutseq_kill_item(ANIMATING5);
	cutseq_kill_item(ANIMATING16);
	cutseq_kill_item(WRECKING_BALL);
	cutseq_kill_item(ANIMATING4);
	cutseq_givelara_pistols();
	meshes[objects[ANIMATING4].mesh_index + (13 * 2)] = meshes[objects[BLUE_GUARD].mesh_index + (13 * 2)];
	cutseq_meshbits[5] &= 0x7FFFFFFF;
}

void cranecut_control()
{
	PHD_VECTOR pos;
	int f;

	f = GLOBAL_cutseq_frame;

	switch (f)
	{
	case 74:
		lara_item->mesh_bits = 0;
		break;
	case 124:
		lara_item->mesh_bits = 0xFFFFFFFF;
		break;
	case 801:
		cutseq_meshbits[1] &= 0x7FFFFFFF;
		break;
	case 850:
		cutseq_removelara_pistols();
		break;
	case 1301:
		cutseq_meshbits[5] |= 0x80000000;
		FlipMap(1);
		break;
	}
	deal_with_pistols(crane_pistols_info);
	pos.x = 12;
	pos.y = 200;
	pos.z = 60;
	deal_with_actor_shooting(craneguard_pistols_info, 1, 13, &pos);
	if (f >= 456 && f <= 462)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		TriggerActorBlood(1, 2, &pos, 0, 1);
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		TriggerActorBlood(1, 5, &pos, 0, 1);
	}
	if (f >= 1455 && f <= 1495)
	{
		pos.x = 1400;
		pos.y = 900;
		pos.z = 512;
		GetActorJointAbsPosition(5, 0, &pos);
		for (int n = 0; n < 2800; n += 175)
			TriggerDelSmoke(pos.x, pos.y, n + pos.z, f - 1367);
	}
}

void cranecut_end()
{
	ITEM_INFO* item;
	
	item = cutseq_restore_item(ANIMATING5);
	RemoveActiveItem(item - items);
	item->flags &= ~IFLAG_ACTIVATION_MASK;
	cutseq_restore_item(ANIMATING16);
	cutseq_restore_item(WRECKING_BALL);
	DelsHandyTeleportLara(58543, -4096, 34972, 49152);
}

void richcut1_init()
{
	short item_num, nex;
	ITEM_INFO* item;

	item_num = room[lara_item->room_number].item_number;

	if (item_num != NO_ITEM)
	{
		do
		{
			item = &items[item_num];
			nex = item->next_item;

			if (item->object_number == SCIENTIST)
			{
				item->status = ITEM_INVISIBLE;
				RemoveActiveItem(item - items);
				DisableBaddieAI(item - items);
				item->flags |= IFLAG_INVISIBLE;
			}

			item_num = nex;
		} while (nex != NO_ITEM);
	}

	cutseq_givelara_hk();
	cutrot = 1;
}

void richcut1_control()
{
	ITEM_INFO* item;
	int f; 
	
	f = GLOBAL_cutseq_frame;

	switch (f)
	{
	case 101:
	case 493:
	case 673:
		lara_item->mesh_bits = 0;
		break;

	case 208:
	case 580:
	case 810:
		lara_item->mesh_bits = -1;
		break;

	case 840:
		item = find_a_fucking_item(CLOSED_DOOR2);
		AddActiveItem(item - items);
		item->status = ITEM_INVISIBLE;
		item->flags |= IFLAG_ACTIVATION_MASK;
		break;
	}
}

void richcut1_end()
{
	DelsHandyTeleportLara(34504, -1024, 54799, 36321);
	cutseq_removelara_hk();
	cutrot = 1;
}

void richcut2_init()
{
	cutrot = 1;
	lara_item->mesh_bits = 0;
	return;
}

void richcut2_control()
{
	if (GLOBAL_cutseq_frame == 300)
		cutseq_meshbits[5] &= 0x7FFFFFFF;

	handle_actor_chatting(23, 11, 1, ANIMATING6, voncroy_chat_ranges_richcut2);
	handle_actor_chatting(21, 18, 3, ANIMATING4, associate_chat_ranges_richcut2);
	handle_actor_chatting(438, 14, 2, ANIMATING1, guard_chat_ranges_richcut2);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void richcut2_end()
{
	lara_item->mesh_bits = -1;
}

void richcut3_init()
{
	cutseq_givelara_hk();
	cutrot = 1;
	cutseq_meshbits[1] &= 0x7FFFFFFF;
}

void richcut3_control()
{
	if (GLOBAL_cutseq_frame == 320)
	{
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_removelara_hk();
		lara.back_gun = WEAPON_NONE;
		lara.hk_type_carried = WTYPE_MISSING;
		lara.last_gun_type = WEAPON_NONE;
	}
}

void richcut3_end()
{
	ITEM_INFO* item; 
	
	item = find_a_fucking_item(HK_ITEM);

	if (item)
	{
		item->status = ITEM_ACTIVE;
		item->flags |= 0x20;
	}
}

void richcut4_init()
{
	cutrot = 3;
	meshes[objects[TWOGUN].mesh_index + 11] = meshes[objects[MAFIA2_MIP].mesh_index + 10];
	cutseq_meshswapbits[1] |= 0x20;
	meshes[objects[TWOGUN].mesh_index + 17] = meshes[objects[MAFIA2_MIP].mesh_index + 16];
	cutseq_meshbits[2] &= 0xFFFFFFFB;
	cutseq_meshswapbits[1] |= 0x100;
	lara_item->mesh_bits = 0;
}

void richcut4_control()
{
	ITEM_INFO* item;
	int f; 
	
	f = GLOBAL_cutseq_frame;

	if (f >= 300 && f <= 312)
	{
		item = find_a_fucking_item(SAILOR_MIP);

		if (!(f & 1))
			item->mesh_bits &= ~(1 << (8 - ((f - 300) >> 1)));
	}

	if (f != 308)
	{
		if (f == 534)
		{
			cutseq_meshswapbits[1] &= 0xFFFFFEFF;
			cutseq_meshbits[2] &= 0xFFFFFFDF;
		}
		else if (f == 557)
		{
			cutseq_meshswapbits[1] &= 0xFFFFFFDF;
			cutseq_meshbits[2] &= 0xFFFFFFFB;
		}
	}
	else
		cutseq_meshbits[2] |= 4;
}

void richcut4_end()
{
	find_a_fucking_item(SAILOR_MIP)->mesh_bits = -1;
	DelsHandyTeleportLara(35328, 0, 35328, 49152);
	lara_item->mesh_bits = -1;
}

void andy1_init()
{
	cutseq_kill_item(ANIMATING2);
}

void andy1_control()
{
	if (GLOBAL_cutseq_frame == 3750)
	{
		cutseq_meshbits[1] &= 0x7FFFFFFF;
		cutseq_restore_item(ANIMATING2);
		FlashFadeR = 255;
		FlashFadeG = 255;
		FlashFadeB = 255;
		FlashFader = 32;
	}

	handle_lara_chatting(lara_chat_ranges_andy1);
	handle_actor_chatting(23, 21, 1, SNIPER_MIP, hanged_man_chat_ranges_andy1);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy1_end()
{
	DelsHandyTeleportLara(63508, 9710, 65464, 949);
}

void andy2_init()
{
	setup_preist_meshswap();
}

void andy2_control()
{
	PHD_VECTOR pos;

	pos.x = 0;
	pos.y = 48;
	pos.z = 240;
	GetActorJointAbsPosition(1, 8, &pos);
	TriggerDynamic(pos.x, pos.y, pos.z, 12 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
	DelTorchFlames(&pos);
	handle_lara_chatting(lara_chat_ranges_andy2);
	handle_actor_chatting(21, 2, 1, SAILOR_MIP, priest_chat_ranges_andy2);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy2_end()
{
	DelsHandyTeleportLara(32289, 6271, 48635, 5661);
}

void andy3_init()
{
	cutseq_kill_item(SWAT_PLUS_MIP);
	cutseq_kill_item(BLUE_GUARD_MIP);
	cutseq_kill_item(TWOGUN_MIP);
	cutseq_kill_item(DOG_MIP);
	cutseq_kill_item(LARSON_MIP);
	cutseq_kill_item(PIERRE_MIP);
	cutseq_kill_item(MAFIA_MIP);
}

void andy3_control()
{
	if (GLOBAL_cutseq_frame == 240)
		do_catapult_meshswap();
}

void andy3_end()
{
	FlipMap(2);
	do_catapult_meshswap();
	lara.puzzleitems[1] = 0;
	DelsHandyTeleportLara(55902, 8448, 53349, 32768);
}

void andy4_init()
{
	cutseq_kill_item(ANIMATING5_MIP);
}

void andy4_control()
{
	PHD_VECTOR pos;
	int n;

	if (GLOBAL_cutseq_frame == 733)
	{
		for (n = 0; n < 8; n++)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetActorJointAbsPosition(1, 0, &pos);
			TriggerDelSmoke(pos.x, pos.y, pos.z, 88);
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetActorJointAbsPosition(2, 0, &pos);
			TriggerDelSmoke(pos.x, pos.y, pos.z, 88);
			GetActorJointAbsPosition(3, 0, &pos);
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			TriggerDelSmoke(0, 0, 0, 88);
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetActorJointAbsPosition(4, 0, &pos);
			TriggerDelSmoke(pos.x, pos.y, pos.z, 88);
		}
	}
}

void andy4_end()
{
	cutseq_restore_item(ANIMATING5_MIP);
	DelsHandyTeleportLara(43484, 5376, 50626, 0);
	SetCutNotPlayed(10);
}

void andy4b_init()
{
	cutseq_kill_item(ANIMATING5_MIP);
}

void andy4b_control()
{
	PHD_VECTOR pos;
	int actor;
	int f;

	f = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame == 510 || GLOBAL_cutseq_frame == 662)
		do_clanger_meshswap();

	if (f == 697 || f == 708 || f == 723 || f == 735)
	{
		if (f == 708) actor = 2;
		else if (f == 697) actor = 3;
		else if (f == 735) actor = 4;
		else actor = 1;

		for (int n = 0; n < 16; n++)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetActorJointAbsPosition(actor, 0, &pos);
			TriggerDelSmoke(pos.x, pos.y, pos.z, 88);
		}
	}
}

void andy4b_end()
{
	DelsHandyTeleportLara(43543, 5376, 47285, 32768);
	lara.puzzleitems[2] = 0;
}

void andy5_init()
{
	cutseq_kill_item(PUZZLE_HOLE2);
}

void andy5_control()
{
	PHD_VECTOR pos;
	int f; 
	
	if (GLOBAL_cutseq_frame >= 310)
	{
		if (GLOBAL_cutseq_frame < 350)
		{
			if (cut_seethrough > 32)
				cut_seethrough -= 4;
		}
		else if (GLOBAL_cutseq_frame <= 450)
		{
			cut_seethrough += 8;

			if (cut_seethrough > 128)
				cut_seethrough = 128;
		}
	}

	if (GLOBAL_cutseq_frame == 451)
		cut_seethrough = 128;
	else if (GLOBAL_cutseq_frame >= 1100)
	{
		cut_seethrough -= 4;
		if (cut_seethrough < 0)
			cut_seethrough = 0;
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetActorJointAbsPosition(1, 0, &pos);

	if (GLOBAL_cutseq_frame >= 1100)
	{
		if (!cut_seethrough)
			return;
		f = cut_seethrough + (GetRandomControl() & 0x1F);
	}
	else
		f = (GetRandomControl() & 0x3F) + 128;

	TriggerDynamic(pos.x, pos.y, pos.z, 10, GetRandomControl() & 0xF, (GetRandomControl() & 0x1F) + 16, f);
}

void andy5_end()
{
	cutseq_restore_item(PUZZLE_HOLE2);
}

void andy6_init()
{
	cutrot = 1;
	setup_preist_meshswap();
}

void andy6_control()
{
	PHD_VECTOR pos;

	if (GLOBAL_cutseq_frame == 900)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 1401)
		lara_item->mesh_bits = -1;

	pos.x = 0;
	pos.y = 48;
	pos.z = 240;
	GetActorJointAbsPosition(1, 8, &pos);
	TriggerDynamic(pos.x, pos.y, pos.z, 12 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
	DelTorchFlames(&pos);
	handle_lara_chatting(lara_chat_ranges_andy6);
	handle_actor_chatting(21, 2, 1, SAILOR_MIP, priest_chat_ranges_andy6);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy6_end()
{
	return;
}

void andy7_init()
{
	cutrot = 1;
}

void andy7_control()
{
	handle_lara_chatting(lara_chat_ranges_andy7);
	handle_actor_chatting(21, 2, 1, SAILOR_MIP, priest_chat_ranges_andy7);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy7_end()
{
	DelsHandyTeleportLara(78336, 5503, 41658, 0);
}

void andy8_init()
{
	return;
}

void andy8_control()
{
	ITEM_INFO* item = find_a_fucking_item(ANIMATING16);

	item->flags |= 0x20;

	switch (GLOBAL_cutseq_frame)
	{
	case 311:
		item->mesh_bits = 2;
		break;

	case 701:
		item->mesh_bits = 4;
		break;

	case 831:
		item->mesh_bits = 8;
		break;

	case 1244:
		lara_item->mesh_bits = 0;
		break;

	case 1476:
		lara_item->mesh_bits = -1;
		break;

	case 145:
		do_chalk_meshswap();
		break;
	}
}

void andy8_end()
{
	SetCutPlayed(36);
	do_chalk_meshswap();
	lara.puzzleitems[0] = 0;
}

void andy9_init()
{
	cutseq_kill_item(ANIMATING15);
	lara_item->mesh_bits = 0;
}

void andy9_control()
{
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame == 151 || GLOBAL_cutseq_frame == 1033 || GLOBAL_cutseq_frame == 3868)
		lara_item->mesh_bits = -1;

	if (f == 192 || f == 1099 || f == 3974)
		lara_item->mesh_bits = 0;

	handle_lara_chatting(lara_chat_ranges_andy9);
	handle_actor_chatting(21, 2, 4, SAILOR_MIP, priest_chat_ranges_andy9);
	handle_actor_chatting(23, 15, 1, SWAT_MIP, knight_chat_ranges_andy9);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy9_end()
{
	cutseq_restore_item(ANIMATING15);
	lara_item->mesh_bits = -1;
}

void andy10_init()// check the 2 item-items calls
{
	ITEM_INFO* item;

	cutseq_kill_item(ANIMATING4);
	cutseq_kill_item(ANIMATING5);
	cutseq_kill_item(ANIMATING14);
	cutseq_meshbits[1] &= 0x7FFFFFFF;
	cutseq_meshbits[2] &= 0x7FFFFFFF;
	cutseq_meshbits[3] &= 0x7FFFFFFF;
	cutseq_meshbits[4] &= 0x7FFFFFFF;
	cutseq_meshbits[5] &= 0x7FFFFFFF;
	cutseq_meshbits[6] &= 0x7FFFFFFF;
	item = find_a_fucking_item(GREEN_TEETH);
	item->status = ITEM_INVISIBLE;
	RemoveActiveItem(item - items);
	DisableBaddieAI(item - items);
	item->flags |= IFLAG_INVISIBLE;
	lara.water_status = LW_UNDERWATER;
	FlipMap(7);
	disable_horizon = 1;
}

void andy10_control()
{
	if (GLOBAL_cutseq_frame == 1250)
	{
		cutseq_meshbits[8] &= 0xFFFFFFE0;
		TriggerDynamic(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 31, 255, 255, 255);
		return;
	}

	if (GLOBAL_cutseq_frame == 902)
	{
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[5] |= 0x80000000;
		cutseq_meshbits[6] |= 0x80000000;
	}
	else if (GLOBAL_cutseq_frame == 338 || GLOBAL_cutseq_frame == 775)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 747)
	{
		lara_item->mesh_bits = -1;
		TriggerDynamic(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 31, 255, 255, 255);
		return;
	}
	else if (GLOBAL_cutseq_frame == 1574)
		cutseq_meshbits[4] &= 0x7FFFFFFF;

	if (GLOBAL_cutseq_frame == 338 || GLOBAL_cutseq_frame == 1030)
		cutseq_meshbits[4] |= 0x80000000;
	else if (GLOBAL_cutseq_frame == 1603)
		cutseq_meshbits[1] &= 0x7FFFFFFF;
	else if (GLOBAL_cutseq_frame == 1624)
		cutseq_meshbits[2] &= 0x7FFFFFFF;
	else if (GLOBAL_cutseq_frame == 854)
		cutseq_meshbits[7] &= 0x7FFFFFFF;

	TriggerDynamic(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 31, 255, 255, 255);
}

void andy10_end()
{
	lara_item->mesh_bits = -1;
	SetCutPlayed(43);
	Cutanimate(ANIMATING12);
	Cutanimate(ANIMATING13);
	FlipMap(7);
	DelsHandyTeleportLara(39424, 15360, 48640, 0);
	lara.water_status = LW_UNDERWATER;
	lara_item->pos.x_rot = -5278;
	lara_item->goal_anim_state = AS_TREAD;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->frame_number = anims[ANIMATION_LARA_UNDERWATER_IDLE].frame_base;
	lara_item->anim_number = ANIMATION_LARA_UNDERWATER_IDLE;
	NailInvItem(PUZZLE_ITEM2);
	disable_horizon = 0;
}

void andy11_init()
{
	cutseq_kill_item(ANIMATING15);
	lara_item->mesh_bits = 0;
	cutseq_meshbits[4] &= 0xFFF07FFF;
	cutseq_meshbits[5] &= 0x7FFFFFFF;
}

void andy11_control()
{
	if (GLOBAL_cutseq_frame == 2112 ||
		GLOBAL_cutseq_frame == 2660 ||
		GLOBAL_cutseq_frame == 3082 ||
		GLOBAL_cutseq_frame == 3626 ||
		GLOBAL_cutseq_frame == 4002 ||
		GLOBAL_cutseq_frame == 4064 ||
		GLOBAL_cutseq_frame == 4118 ||
		GLOBAL_cutseq_frame == 4366 ||
		GLOBAL_cutseq_frame == 4789 ||
		GLOBAL_cutseq_frame == 5390)
		lara_item->mesh_bits = -1;

	if (GLOBAL_cutseq_frame == 2164 ||
		GLOBAL_cutseq_frame == 2863 ||
		GLOBAL_cutseq_frame == 3534 ||
		GLOBAL_cutseq_frame == 3915 ||
		GLOBAL_cutseq_frame == 4064 ||
		GLOBAL_cutseq_frame == 4158 ||
		GLOBAL_cutseq_frame == 4569 ||
		GLOBAL_cutseq_frame == 5076)
		lara_item->mesh_bits = 0;

	if (GLOBAL_cutseq_frame == 3082)
		cutseq_meshbits[5] |= 0x80000000;
	else if (GLOBAL_cutseq_frame == 2660)
		cutseq_meshbits[2] &= 0x7FFFFFFF;

	handle_lara_chatting(lara_chat_ranges_andy11);
	handle_actor_chatting(21, 2, 4, SAILOR_MIP, priest_chat_ranges_andy11);
	handle_actor_chatting(23, 15, 1, SWAT_MIP, knight_chat_ranges_andy11);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy11_end()
{
	cutseq_restore_item(ANIMATING15);
}

void andypew_init()
{
	cutseq_kill_item(PUZZLE_HOLE2);
}

void andypew_control()
{
	return;
}

void andypew_end()
{
	cutseq_restore_item(PUZZLE_HOLE2);
	DelsHandyTeleportLara(45556, 7680, 23415, -32768);
}

void cossack_init()
{
	return;
}

void cossack_control()
{
	PHD_VECTOR pos;
	int n;
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 98
		|| f == 344
		|| f == 743
		|| f == 1263)
		lara_item->mesh_bits = 0;

	if (f == 281 || f == 468 || f == 901 || f == 1339)
		lara_item->mesh_bits = -1;

	if (f == 156)
	{
		pos.x = 0;
		pos.y = 120;
		pos.z = 0;
		GetActorJointAbsPosition(1, 17, &pos);
		for (n = 0; n < 7; n++)
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
	}
	else if (f == 157)
	{
		pos.x = 0;
		pos.y = 120;
		pos.z = 0;
		GetActorJointAbsPosition(1, 13, &pos);
		for (n = 0; n < 7; n++)
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
	}
	else
	{
		if (f >= 581 && f <= 620)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, 11);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);

			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, 14);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
		}
		if (f >= 1110 && f <= 1125)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, 11);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
		}
		if (f >= 1111 && f <= 1126)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, 4);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
		}
	}
}

void cossack_end()
{
	DelsHandyTeleportLara(78080, 5376, 37888, 0);
	SetCutNotPlayed(36);
}

void monk2_init()
{
	cutrot = 1;
}

void monk2_control()
{
	PHD_VECTOR pos;
	int f;

	if (GLOBAL_cutseq_frame == 70)
		lara_item->mesh_bits = 0;
	else
	{

		if (GLOBAL_cutseq_frame < 630 || GLOBAL_cutseq_frame >= 720)
			f = cut_seethrough;
		else
		{
			f = cut_seethrough;
			if (cut_seethrough > 32)
			{
				f = cut_seethrough - 4;
				cut_seethrough -= 4;
			}
		}

		if (GLOBAL_cutseq_frame == 740)
			cut_seethrough = 128;
		else if (GLOBAL_cutseq_frame >= 940)
		{
			if (f > 0)
			{
				cut_seethrough = f - 4;
				if (f - 4 < 0)
					cut_seethrough = 0;
			}
			else
			{
				if (f < 0)
					cut_seethrough = 0;
			}
		}
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetActorJointAbsPosition(1, 0, &pos);

	TriggerDynamic(pos.x, pos.y, pos.z, 10,
		GetRandomControl() & 0xF, (GetRandomControl() & 0x1F) + 16, (GetRandomControl() & 0x3F) + 128);
}

void monk2_end()
{
	lara_item->mesh_bits = -1;
}

void swampy_init()
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(GREEN_TEETH);
	cutrot = 0;
	if (item)
	{
		item->status = ITEM_INVISIBLE;
		RemoveActiveItem(item - items);
		DisableBaddieAI(item - items);
		item->flags |= IFLAG_INVISIBLE;
	}
	lara.water_status = LW_UNDERWATER;
}

void swampy_control()
{
	if (GLOBAL_cutseq_frame >= 385 && GLOBAL_cutseq_frame <= 396)
		CutLaraBubbles();
}

void swampy_end()
{
	SetCutNotPlayed(43);
	AddActiveItem(find_a_fucking_item(GREEN_TEETH) - items);
	DelsHandyTeleportLara(42477, 12456, 55982, 28953);
	lara.water_status = LW_UNDERWATER;
	lara_item->pos.x_rot = -5278;
	lara_item->goal_anim_state = AS_TREAD;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->frame_number = anims[ANIMATION_LARA_UNDERWATER_IDLE].frame_base;
	lara_item->anim_number = ANIMATION_LARA_UNDERWATER_IDLE;
	if (lara.air > 200)
		lara.air = 200;
}

void hamgate_init()
{
	ITEM_INFO* item; 
	
	item = find_a_fucking_item(DOOR_TYPE1);
	AddActiveItem(item - items);
	item->flags |= IFLAG_ACTIVATION_MASK;
	item->mesh_bits = 3;
	item->status = ITEM_INVISIBLE;
	cutseq_meshbits[2] &= 0xFFFFFFFD;
	cutrot = 0;
}

void hamgate_control()
{
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame == 14 || GLOBAL_cutseq_frame == 269)
		do_hammer_meshswap();

	if (f == 442)
	{
		cutseq_meshbits[3] &= 0x7FFFFFFF;
		cutseq_meshbits[2] |= 2;
	}
}

void hamgate_end()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(DOOR_TYPE1);
	item->status = ITEM_ACTIVE;
	DelsHandyTeleportLara(31232, -3328, 48344, 0);
}

void stealth3_start()
{
	int i;
	ITEM_INFO* item;
	
	
	item = &items[0];

	for (i = 0; i < level_items; i++, item++)
	{
		if (item->object_number == CHEF ||
			item->object_number == SAS ||
			item->object_number == BLUE_GUARD ||
			item->object_number == SWAT_PLUS ||
			item->object_number == SWAT ||
			item->object_number == TWOGUN)
		{
			if (ABS(item->pos.x_pos - lara_item->pos.x_pos) < 1024 &&
				ABS(item->pos.z_pos - lara_item->pos.z_pos) < 1024 &&
				ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256)
			{
				GLOBAL_cutme->actor_data[1].objslot = item->object_number;
				item->status = ITEM_INVISIBLE;
				RemoveActiveItem(i);
				DisableBaddieAI(i);
				item->flags |= IFLAG_INVISIBLE;
			}
		}
	}

	if (cutseq_num == 2)
		SwapCrowbar(0);
}

void stealth3_end()
{
	ITEM_INFO* item;
	
	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (cutseq_num == 3)
			continue;

		if (item->object_number == CHEF ||
			item->object_number == SAS ||
			item->object_number == BLUE_GUARD ||
			item->object_number == SWAT_PLUS ||
			item->object_number == SWAT ||
			item->object_number == TWOGUN)
		{
			if (ABS(item->pos.x_pos - lara_item->pos.x_pos) < 1024 &&
				ABS(item->pos.z_pos - lara_item->pos.z_pos) < 1024 &&
				ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256)
			{
				item->hit_points = 0;

				if (item->object_number == TWOGUN)
				{
					item->anim_number = objects[TWOGUN].anim_index + 3;
					item->current_anim_state = 7;
				}
				else if (item->object_number == CHEF)
				{
					item->anim_number = objects[CHEF].anim_index + 11;
					item->current_anim_state = 6;
				}
				else if (objects[SWAT].loaded)
				{
					item->anim_number = objects[SWAT].anim_index + 11;
					item->current_anim_state = 6;
				}
				else
				{
					item->anim_number = objects[BLUE_GUARD].anim_index + 11;
					item->current_anim_state = 6;
				}

				item->frame_number = anims[item->anim_number].frame_end;
				AddActiveItem(i);
			}
		}
	}

	if (cutseq_num == 2)
		SwapCrowbar(0);
}

void joby2_init()
{
	disable_horizon = 1;
	cutseq_meshbits[1] &= 0x7FFFFFFF;
	cutseq_meshbits[2] &= 0x7FFFFFFF;
	cutseq_meshbits[3] &= 0x7FFFFFFF;
	cutseq_meshbits[4] &= 0x7FFFFFFF;
	cutseq_meshbits[6] &= 0x7FFFFFFF;
}

void joby2_control()
{
	switch (GLOBAL_cutseq_frame)
	{
	case 176:
		lara_item->mesh_bits = 0;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		break;
	case 1622:
		lara_item->mesh_bits = -1;
		cutseq_meshbits[1] &= 0x7FFFFFFF;
		cutseq_meshbits[2] &= 0x7FFFFFFF;
		cutseq_meshbits[3] &= 0x7FFFFFFF;
		cutseq_meshbits[4] &= 0x7FFFFFFF;
		cutseq_meshbits[5] &= 0x7FFFFFFF;
		cutseq_meshbits[6] &= 0x7FFFFFFF;
		disable_horizon = 1;
		break;
	case 1822:
		lara_item->mesh_bits = 0;
		disable_horizon = 0;
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		break;
	case 254:
		disable_horizon = 0;
		break;
	case 1485:
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[6] |= 0x80000000;
		break;
	}

	handle_actor_chatting(21, 2, 3, ANIMATING6_MIP, admiral_chat_ranges_joby2);
	handle_actor_chatting(23, 3, 4, ANIMATING9_MIP, sergie_chat_ranges_joby2);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby2_end()
{
	lara_item->mesh_bits = -1;
	DelsHandyTeleportLara(22905, -4608, 45415, 48806);
	cutseq_kill_item(ANIMATING6);
}

void joby3_init()
{
	return;
}

void joby3_control()
{
	if (GLOBAL_cutseq_frame == 583)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 765)
		lara_item->mesh_bits = -1;
	else if (GLOBAL_cutseq_frame == 1299)
		FlipMap(3);

	handle_lara_chatting(lara_chat_ranges_joby3);
}

void joby3_end()
{
	return;
}

void joby4_init()
{
	cutseq_kill_item(DOOR_TYPE1);
	cutseq_kill_item(DOOR_TYPE5);
	cutrot = 0;
}

void joby4_control()
{
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (GLOBAL_cutseq_frame <= 130)
		PrintString(phd_centerx, phd_winymax - 3 * font_height, 5, &gfStringWad[gfStringOffset[STR_SEVERAL_HOURS_LATER]], FF_CENTER);

	if (f == 575)
		cutseq_meshbits[5] &= 0x7FFFFFFF;
	else
	{
		if (f == 769 || f == 1966)
			lara_item->mesh_bits = 0;
		if (f == 1593)
			lara_item->mesh_bits = -1;
	}

	handle_lara_chatting(lara_chat_ranges_joby4);
	handle_actor_chatting(21, 2, 3, CRANE_GUY_MIP, admiral_chat_ranges_joby4);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby4_end()
{
	lara_item->mesh_bits = -1;
	cutseq_restore_item(DOOR_TYPE1);
	cutseq_restore_item(DOOR_TYPE5);
	DelsHandyTeleportLara(57950, 8960, 53760, 16384);
}

void joby5_init()
{
	cutseq_kill_item(ANIMATING3);
	cutseq_kill_item(ANIMATING4);
	cutseq_meshbits[2] &= 0x7FFFFFFF;
	cutseq_meshbits[4] &= 0x7FFFFFFF;
}

void joby5_control()
{
	int f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 144)
		lara_item->mesh_bits = 0;
	else if (f == 436)
		cutseq_meshbits[2] |= 0x80000000;
	else if (f == 1796)
		cutseq_meshbits[4] &= 0x7FFFFFFF;
	else if (f == 2175)
		cutseq_meshbits[1] &= 0x7FFFFFFF;
	else if (f == 151 || f == 2330)
		cutseq_meshbits[4] |= 0x80000000;

	handle_actor_chatting(21, 2, 3, CRANE_GUY_MIP, admiral_chat_ranges_joby5);
	handle_actor_chatting(23, 3, 4, LION_MIP, sergie_chat_ranges_joby5);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby5_end()
{
	cutseq_restore_item(ANIMATING3);
	cutseq_restore_item(ANIMATING4);
	DelsHandyTeleportLara(61952, -1792, 55808, 32768);
	lara_item->mesh_bits = -1;
}

void joby6_init()
{
	cutseq_kill_item(ANIMATING10);
	cutseq_kill_item(ANIMATING10_MIP);
	cutseq_kill_item(ANIMATING16);
	cutseq_kill_item(DOOR_TYPE4);
	cutseq_kill_item(PUZZLE_DONE3);
	cutseq_kill_item(PUZZLE_DONE4);
	cutseq_kill_item(PUZZLE_HOLE3);
	cutseq_kill_item(PUZZLE_HOLE4);
}

void joby6_control()
{
	if (GLOBAL_cutseq_frame == 542)
	{
		lara_item->mesh_bits = 0x81FF01FF;
		lara.mesh_ptrs[LM_HIPS] = meshes[objects[MESHSWAP3].mesh_index];
	}
}

void joby6_end()
{
	lara_item->mesh_bits = -1;
	lara.mesh_ptrs[LM_HIPS] = meshes[objects[LARA].mesh_index];
}

void joby7_init()
{
	lara_item->mesh_bits = 0;
	cutseq_kill_item(ANIMATING2);
	cutseq_meshbits[6] &= 0x7FFFFFFF;
}

void joby7_control()
{
	PHD_VECTOR s, d;
	int f, brightme, b;
	short room_no;

	f = GLOBAL_cutseq_frame;

	if (f == 750)
		cutseq_meshbits[6] |= 0x80000000;
	else if (f >= 650 && f <= 1050)
	{
		s.x = 0;
		s.y = -100;
		s.z = 0;
		GetActorJointAbsPosition(6, 0, &s);

		b = f - 650 > 220 ? 220 : f - 650;
		brightme = ((f - 650) >> 5) + 8;
		TriggerDynamic(s.x, s.y, s.z, brightme, 0, 0, b + 16);
	}
	if (f == 1410)
	{
		s.x = 0;
		s.y = -100;
		s.z = 0;
		GetActorJointAbsPosition(1, 0, &s);
		room_no = lara_item->room_number;
		IsRoomOutsideNo = -1;
		IsRoomOutside(s.x, s.y, s.z);

		if (IsRoomOutsideNo != -1)
			room_no = IsRoomOutsideNo;

		TriggerExplosionBubble(s.x, s.y, s.z, room_no);
		TriggerExplosionSparks(s.x, s.y, s.z, 2, -2, 1, room_no);
		TriggerExplosionSparks(s.x, s.y, s.z, 2, -1, 1, room_no);
	}

	s.x = 0;
	s.y = -1024;
	s.z = -128;
	GetActorJointAbsPosition(1, 7, &s);
	d.x = 0;
	d.y = -20480;
	d.z = -128;
	GetActorJointAbsPosition(1, 7, &d);
	LaraTorch(&s, &d, 0, 255);
	TriggerEngineEffects_CUT();
	handle_actor_chatting(17, 14, 1, CROW_MIP, lara_chat_ranges_joby7);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby7_end()
{
	lara_item->mesh_bits = -1;
	cutseq_restore_item(ANIMATING2);
	AddDisplayPickup(PICKUP_ITEM1);
	DelsHandyTeleportLara(30049, 17583, 69794, 51830);
	lara.water_status = LW_UNDERWATER;
	lara_item->pos.x_rot = 1090;
	lara_item->goal_anim_state = AS_TREAD;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->frame_number = anims[ANIMATION_LARA_UNDERWATER_IDLE].frame_base;
	lara_item->anim_number = ANIMATION_LARA_UNDERWATER_IDLE;
	lara.Anxiety = 80;
}

void joby8_init()
{
	short room_number;

	cutseq_kill_item(DOOR_TYPE4);
	cutseq_kill_item(ANIMATING16);
	lara_item->mesh_bits = 0x81FF01FF;

	room_number = lara_item->room_number;
	GetHeight(GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number), lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	TestTriggers(trigger_index, 1, 0);
	cutseq_meshswapbits[7] = 128;
	cutseq_meshbits[2] &= 0x7FFFFFFF;
	cutseq_meshbits[3] &= 0x7FFFFFFF;
	cutseq_meshbits[4] &= 0x7FFFFFFF;
	cutrot = 0;
}

void joby8_control()
{
	short room_num;

	switch (GLOBAL_cutseq_frame)
	{
	case 147:
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		break;
	case 2440:
		cutseq_meshbits[8] &= 0x7FFFFFFF;
		break;
	case 2681:
		cutseq_meshbits[1] &= 0x7FFFFFFF;
		cutseq_meshbits[2] &= 0x7FFFFFFF;
		cutseq_meshbits[3] &= 0x7FFFFFFF;
		cutseq_meshbits[4] &= 0x7FFFFFFF;
		cutseq_meshbits[5] &= 0x7FFFFFFF;
		break;
	case 440:
		lara_item->mesh_bits = -1;
		break;
	case 2103:
		cutseq_meshswapbits[7] = 9216;
		break;
	case 2724:
		room_num = lara_item->room_number;
		GetHeight(GetFloor(lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_num), lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos);
		TestTriggers(trigger_index, 1, 0);
		break;
	}

	if (GLOBAL_cutseq_frame >= 2440 && GLOBAL_cutseq_frame <= 2659)
	{
		int r, g, b;
		int i;
		SPARKS* sptr;
		PHD_VECTOR s;
		int scale;
		PHD_VECTOR d;

		r = (GetRandomControl() & 0x3F) + 96;
		g = GetRandomControl() % r;
		b = GetRandomControl() & 0xF;

		for (i = 0; i < 3; i++)
		{
			s.x = 0;
			s.y = 0;
			s.z = 0;

			GetActorJointAbsPosition(1, GetRandomControl() % 20, &s);

			sptr = &spark[GetFreeSpark()];

			sptr->On = 1;

			sptr->dR = r;
			sptr->dG = g;
			sptr->dB = b;

			scale = (GetRandomControl() & 0x3F) - 64;
			sptr->sR = scale;
			sptr->sB = scale;
			sptr->sG = scale;

			sptr->ColFadeSpeed = 8;
			sptr->TransType = 2;
			sptr->FadeToBlack = 4;

			sptr->Life = 12;
			sptr->sLife = 12;

			sptr->x = s.x;
			sptr->y = s.y;
			sptr->z = s.z;

			sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
			sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
			sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;

			sptr->Friction = 51;
			sptr->MaxYvel = 0;
			sptr->Gravity = 0;

			sptr->Flags = 0;

			TriggerFireFlame(s.x, s.y, s.z, -1, 254);
		}

		d.x = s.x + (phd_sin(2 * (GetRandomControl() & 0x7FFF)) >> 3);
		d.y = s.y;
		d.z = s.z + (phd_cos(2 * (GetRandomControl() & 0x7FFF)) >> 3);

		TriggerLightning(&s, &d, (GetRandomControl() & 0xF) + 30, RGBA(r, g, b, 16), 15, 40, 5);
		TriggerDynamic(s.x, s.y, s.z, 10, (GetRandomControl() & 0x7F) + 128, g, b);
	}
	else if (GLOBAL_cutseq_frame >= 2681 && GLOBAL_cutseq_frame <= 2724)
	{
		PHD_VECTOR s;
		int scale;
		int r;
		int g;
		int b;

		s.x = 512;
		s.y = 0;
		s.z = 0;
		GetLaraJointPos(&s, 0);
		scale = (GLOBAL_cutseq_frame - 2681) >> 4;
		r = ((GetRandomControl() & 0x3F) + 192) >> scale;
		g = ((GetRandomControl() & 0x1F) + 128) >> scale;
		b = (GetRandomControl() & 0x3F) >> scale;

		if (GetRandomControl() & 1)
			TriggerDynamic(s.x, s.y, s.z, 24, r, g, b);

		TriggerDynamic(s.x, s.y, s.z, 31, r, g, b);
	}

	handle_lara_chatting(lara_chat_ranges_joby8);
	handle_actor_chatting(23, 3, 1, LION_MIP, actor_chat_ranges_joby8);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby8_end()
{
	DelsHandyTeleportLara(58880, 5120, 76288, 0);
}

void joby9_init()
{
	cutseq_kill_item(CRANE_GUY_MIP);
}

void joby9_control()
{
	handle_lara_chatting(lara_chat_ranges_joby9);
	handle_actor_chatting(21, 2, 1, CRANE_GUY_MIP, admiral_chat_ranges_joby9);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby9_end()
{
	cutseq_restore_item(CRANE_GUY_MIP);
	AddDisplayPickup(KEY_ITEM7);
}

void joby10_init()
{
	cutseq_kill_item(CRANE_GUY_MIP);
}

void joby10_control()
{
	ITEM_INFO* item;

	if (GLOBAL_cutseq_frame == 3235)
	{
		item = find_a_fucking_item(HYDRA_MIP);
		item->status = ITEM_INACTIVE;
	}

	handle_lara_chatting(lara_chat_ranges_joby10);
	handle_actor_chatting(21, 2, 1, CRANE_GUY_MIP, admiral_chat_ranges_joby10);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void joby10_end()
{
	cutseq_restore_item(CRANE_GUY_MIP);
	DelsHandyTeleportLara(58267, -3072, 48676, 48958);
}

void special1_init()
{
	cutrot = 0;
	lara_item->mesh_bits = 1;
	Chris_Menu = 0;
}

void special1_control()
{
	_special1_control();
}

void special1_end()
{
	_special1_end();
}

void special2_init()
{
	cutrot = 0;
	lara_item->mesh_bits = -1;
	Chris_Menu = 0;
	ResetCutItem(ANIMATING1_MIP);
	ResetCutItem(ANIMATING2_MIP);
	ResetCutItem(ANIMATING3_MIP);
	ResetCutItem(ANIMATING4_MIP);
	ResetCutItem(ANIMATING5_MIP);
	ResetCutItem(ANIMATING6_MIP);
	ResetCutItem(ANIMATING7_MIP);
	resetwindowsmash(50);
	resetwindowsmash(52);
	resetwindowsmash(54);
	resetwindowsmash(56);
	resetwindowsmash(58);
	resetwindowsmash(60);
	ResetCutanimate(ANIMATING16);
	ResetCutanimate(ANIMATING5);
	dword_00E916F0 = 2;
}

void special2_control()
{
	_special2_control();
}

void special2_end()
{
	_special2_end();
}

void special3_init()
{
	cutrot = 0;
	lara_item->mesh_bits = -1;
	Chris_Menu = 0;
	dword_00E916F0 = 3;
}

void special3_control()
{
	_special3_control();
}

void special3_end()
{
	_special3_end();
}

void special4_init()
{
	lara_item->mesh_bits = -1;
	Chris_Menu = 0;
	cutrot = 1;
	dword_00E916F0 = 4;
}

void special4_control()
{
	_special4_control();
}

void special4_end()
{
	_special4_end();
}

void handle_cutseq_triggering(int name)
{
	int n, goin;// , fuck;//idk where to use it

	n = cutseq_num;

	if (!n)
		return;

	if (!cutseq_trig)
	{
		if (lara.gun_type == WEAPON_FLARE || lara.gun_status != LG_NO_ARMS && lara.gun_status != LG_HANDS_BUSY)
			lara.gun_status = LG_UNDRAW_GUNS;

		cutseq_trig = 1;
		memset(cutseq_meshswapbits, 0, sizeof(cutseq_meshswapbits));
		cutseq_busy_timeout = 50;
		memset(cutseq_meshbits, 0xFF, sizeof(cutseq_meshbits));

		if (gfCurrentLevel != LVL5_TITLE)
			SetFadeClip(28, 1);

		if (!ScreenFadedOut)
		{
			if (gfCurrentLevel != LVL5_TITLE)
				S_CDFade(0);

			SetScreenFadeOut(16, 0);
		}

		return;
	}

	if (cutseq_trig != 1)
	{
		if (cutseq_trig == 3)
		{
			SetScreenFadeOut(16, 1);

			if (gfCurrentLevel != LVL5_TITLE)
				S_CDFade(0);

			cutseq_trig = 4;
			return;
		}

		if (cutseq_trig != 4 || !ScreenFadedOut)
			return;

		if (gfCurrentLevel != LVL5_TITLE)
		{
			S_CDStop();
			n = cutseq_num;
		}

		ScreenFadedOut = 0;
		numnailed = 0;

		if (cutseq_control_routines[cutseq_num].end_func)
		{
			cutseq_control_routines[cutseq_num].end_func();
			n = cutseq_num;
		}

		if (n <= 4)
		{
			DelsHandyTeleportLara(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, cutrot << 14);
			n = cutseq_num;
		}

		cutseq_trig = 0;
		GLOBAL_playing_cutseq = 0;

		if (!bDoCredits)
		{
			if (dels_cutseq_player)
			{
				reset_flag = 1;
				dels_cutseq_player = 0;
			}
			else
			{
				if (n != 26 && n != 22 && n != 5 && n != 16 && n != 33 && n != 44 && n != 12)
				{
					finish_cutseq(name);
					cutseq_num = 0;
					camera.type = GLOBAL_oldcamtype;

					if (gfCurrentLevel != LVL5_TITLE)
						SetFadeClip(0, 1);

					AlterFOV(14560);

					if (gfCurrentLevel != LVL5_TITLE)
						S_CDPlay(CurrentAtmosphere, 1);

					IsAtmospherePlaying = 1;
					return;
				}

				gfLevelComplete = gfCurrentLevel + 1;
			}

			gfRequiredStartPos = 0;
			cutseq_num = 0;
			GLOBAL_playing_cutseq = 0;
			cutseq_trig = 0;
			AlterFOV(14560);
			ScreenFade = 0;
			dScreenFade = 0;
			ScreenFadeSpeed = 8;
			ScreenFadeBack = 0;
			ScreenFadedOut = 0;
			ScreenFading = 0;
			return;
		}

		switch (n)
		{
		case 28:
			goin = 29;
			break;

		case 29:
			goin = 30;
			break;

		case 30:
			goin = 28;
			break;

		default:
			Load_and_Init_Cutseq(n);
			cutseq_trig = 2;
			return;
		}

		cutseq_num = goin;
		Load_and_Init_Cutseq(goin);
		cutseq_trig = 2;
		return;
	}

	if (!ScreenFadedOut)
		return;

	if (--cutseq_busy_timeout > 0)
	{
		if (lara.gun_status != LG_HANDS_BUSY
			&& (lara.gun_status != LG_NO_ARMS || lara.flare_control_left)
			&& lara_item->current_anim_state != AS_ALL4S
			&& lara_item->current_anim_state != AS_CRAWL
			&& lara_item->current_anim_state != AS_ALL4TURNL
			&& lara_item->current_anim_state != AS_ALL4TURNR
			&& lara_item->current_anim_state != AS_CRAWLBACK)
			return;
	}
	else
		cutseq_busy_timeout = 0;

	lara.flare_control_left = 0;
	lara.flare_age = 0;

	if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA))
	{
		lara.gun_type = WEAPON_NONE;
		lara.request_gun_type = WEAPON_NONE;
		lara.gun_status = LG_NO_ARMS;
		lara.last_gun_type = WEAPON_PISTOLS;

		if (!objects[PISTOLS_ITEM].loaded || lara.pistols_type_carried == WTYPE_MISSING)
			lara.last_gun_type = WEAPON_NONE;

		if (gfLevelFlags & GF_LVOP_TRAIN && objects[HK_ITEM].loaded && lara.hk_type_carried & WTYPE_PRESENT)
			lara.last_gun_type = WEAPON_HK;

		lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + (2 * LM_LHAND)];
		lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + (2 * LM_RHAND)];
		lara.left_arm.frame_number = 0;
		lara.right_arm.frame_number = 0;
		lara.target = 0;
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		lara_item->goal_anim_state = AS_STOP;
		lara_item->current_anim_state = AS_STOP;
		lara_item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
		lara_item->anim_number = ANIMATION_LARA_STAY_SOLID;
		lara_item->speed = 0;
		lara_item->fallspeed = 0;
		lara_item->gravity_status = 0;
		lara.back_gun = 0;

		if (lara.weapon_item != NO_ITEM)
		{
			KillItem(lara.weapon_item);
			lara.weapon_item = NO_ITEM;
		}
	}

	lara.water_status = LW_ABOVE_WATER;

	if (gfCurrentLevel != LVL5_TITLE)
		S_CDStop();

	numnailed = 0;
	GLOBAL_oldcamtype = camera.type;
	ScreenFading = 0;
	SetScreenFadeIn(16);
	Load_and_Init_Cutseq(cutseq_num);
	cutseq_trig = 2;
	cut_seethrough = 128;

	if (cutseq_control_routines[cutseq_num].init_func)
		cutseq_control_routines[cutseq_num].init_func();

	AlterFOV(11488);

	if (GLOBAL_cutme->audio_track != -1 && !bDoCredits)
		S_StartSyncedAudio(GLOBAL_cutme->audio_track);
}

void cutseq_givelara_pistols()
{
	old_lara_holster = lara.holster;
	lara.holster = LARA_HOLSTERS;
	draw_pistol_meshes(WEAPON_PISTOLS);
}

void cutseq_removelara_pistols()
{
	undraw_pistol_mesh_left(WEAPON_PISTOLS);
	undraw_pistol_mesh_right(WEAPON_PISTOLS);
	lara.holster = old_lara_holster;
}

void do_pierre_gun_meshswap()
{
	meshes[objects[PIERRE].mesh_index + 23] = meshes[objects[MESHSWAP2].mesh_index + 22];
	cutseq_meshswapbits[1] |= 0x800;
}

void do_catapult_meshswap()
{
	short* temp = lara.mesh_ptrs[LM_LHAND];
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[CROW_MIP].mesh_index + (2 * LM_LHAND)];
	meshes[objects[CROW_MIP].mesh_index + (2 * LM_LHAND)] = temp;
}

void do_clanger_meshswap()
{
	short* temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP2].mesh_index + (2 * LM_RHAND)];
	meshes[objects[MESHSWAP2].mesh_index + (2 * LM_RHAND)] = temp;
}

void do_hammer_meshswap()
{
	short* temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP3].mesh_index + (2 * LM_RHAND)];
	meshes[objects[MESHSWAP3].mesh_index + (2 * LM_RHAND)] = temp;
}

void setup_preist_meshswap()
{
	meshes[objects[SAILOR_MIP].mesh_index + 17] = meshes[objects[MESHSWAP3].mesh_index + 16];
	cutseq_meshswapbits[1] |= 0x100;
}

void do_chalk_meshswap()//optimized out
{
	short* temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MAFIA_MIP].mesh_index + 2 * LM_RHAND];
	meshes[objects[MAFIA_MIP].mesh_index + 2 * LM_RHAND] = temp;
}

void cutseq_shoot_pistols(int left_or_right)
{
	if (left_or_right == 14)
	{
		lara.left_arm.flash_gun = 4;
		SmokeCountL = 16;
	}
	else
	{
		lara.right_arm.flash_gun = 4;
		SmokeCountR = 16;
	}
}

ITEM_INFO* ResetCutanimate(int objnum)
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(objnum);
	item->anim_number = objects[objnum].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	RemoveActiveItem(item - items);
	item->status = ITEM_INACTIVE;
	item->flags &= ~IFLAG_ACTIVATION_MASK;
	return item;
}

void Cutanimate(int objnum)
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(objnum);
	item->anim_number = objects[objnum].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	AddActiveItem(item - items);
	item->status = ITEM_ACTIVE;
	item->flags |= IFLAG_ACTIVATION_MASK;
}

void cutseq_removelara_hk()
{
	undraw_shotgun_meshes(WEAPON_HK);
	lara.gun_type = WEAPON_NONE;
	lara.request_gun_type = WEAPON_NONE;
	lara.gun_status = LG_NO_ARMS;
	lara.last_gun_type = WEAPON_HK;
}

void cutseq_givelara_hk()
{
	draw_shotgun_meshes(WEAPON_HK);
}

void TriggerActorBlood(int actornum, unsigned long nodenum, PHD_VECTOR* pos, int direction, int speed)
{
	GetActorJointAbsPosition(actornum, nodenum, pos);
	TriggerBlood(pos->x, pos->y, pos->z, direction >> 4, speed);
}

void finish_cutseq(int name)
{
	GLOBAL_playing_cutseq = 0;
	InitialiseHair();
}

void trigger_weapon_dynamics(int left_or_right)
{
	PHD_VECTOR pos;

	pos.x = (GetRandomControl() & 0xFF) - 128;
	pos.y = (GetRandomControl() & 0x7F) - 63;
	pos.z = (GetRandomControl() & 0xFF) - 128;
	GetLaraJointPos(&pos, left_or_right);
	TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, (GetRandomControl() & 0x3F));
}

ITEM_INFO* find_a_fucking_item(int object_number)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == object_number)
			return item;
	}

	return 0;
}

void TriggerDelSmoke(long x, long y, long z, int sizeme)
{
	long size, dx, dz;
	SPARKS* sptr;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx >= -16384 && dx <= 16384 && dz >= -16384 && dz <= 16384)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 128;
		sptr->sG = 128;
		sptr->sB = 128;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 11;
		sptr->sLife = (GetRandomControl() & 3) + 11;
		sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
		sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
		sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
		sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
		sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
		sptr->Friction = 2;
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		sptr->Scalar = 2;
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
		size = sizeme + (GetRandomControl() & 0x1F);
		sptr->dSize = (unsigned char)size;
		sptr->sSize = (unsigned char)(size >> 2);
		sptr->Size = (unsigned char)(size >> 2);
	}
}

void TriggerDelBrownSmoke(long x, long y, long z)
{
	long size;
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 50;
	sptr->sG = 45;
	sptr->sB = 40;
	sptr->dR = 40;
	sptr->dG = 35;
	sptr->dB = 30;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 5;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 20;
	sptr->sLife = (GetRandomControl() & 7) + 20;
	sptr->x = (GetRandomControl() & 0x7F) + x - 63;
	sptr->y = (GetRandomControl() & 0x7F) + y - 63;
	sptr->z = (GetRandomControl() & 0x7F) + z - 63;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Friction = 2;
	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Scalar = 2;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	size = (GetRandomControl() & 0x1F) + 40;
	sptr->dSize = (unsigned char)size;
	sptr->sSize = (unsigned char)(size >> 2);
	sptr->Size = (unsigned char)(size >> 2);
}

void DelTorchFlames(PHD_VECTOR* pos)
{
	long x, y, z, size;
	SPARKS* sptr;

	x = pos->x;
	y = pos->y;
	z = pos->z;
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = -1;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 16;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = (GetRandomControl() & 7) + 32;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 51;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Flags = -32230;
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
	sptr->Scalar = 2;
	size = (GetRandomControl() & 0xF) + 16;
	sptr->sSize = (unsigned char)size;
	sptr->Size = (unsigned char)size;
	sptr->dSize = (unsigned char)(size >> 4);

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = (GetRandomControl() & 0x3F) - 64;
	sptr->sB = (GetRandomControl() & 0xF) + 16;
	sptr->dR >>= 2;
	sptr->dG >>= 2;
	sptr->dB >>= 2;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0xF) + 24; 
	sptr->sLife = (GetRandomControl() & 0xF) + 24;
	sptr->x = (GetRandomControl() & 0x3F) + x - 32;
	sptr->y = (GetRandomControl() & 0x3F) + y - 32;
	sptr->z = (GetRandomControl() & 0x3F) + z - 32;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Flags = -32230;
	sptr->Scalar = 2;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -22;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	size = (GetRandomControl() & 0xF) + 16;
	sptr->dSize = (unsigned char)size;
	sptr->sSize = (unsigned char)(size >> 1);
	sptr->Size = (unsigned char)(size >> 1);
	sptr->dSize += sptr->dSize >> 2;
}

void trigger_title_spotcam(int num)
{
	ITEM_INFO* item;

	jobyfrigger = 0;

	switch (num)
	{
	case 1:
		item = ResetCutanimate(ANIMATING10);
		item->pos.x_pos = 59904;
		item->pos.y_pos = 0;
		item->pos.z_pos = 42496;
		item->room_number = 0;
		item = ResetCutanimate(ANIMATING11);
		item->pos.x_pos = 59904;
		item->pos.y_pos = 0;
		item->pos.z_pos = 42496;
		item->room_number = 0;
		S_CDPlay(CDA_XA11_FLYBY1, 0);
		InitialiseSpotCam(1);
		return;

	case 4:
		jobyfrigger = 1;
		ResetCutanimate(ANIMATING4);
		item = ResetCutanimate(ANIMATING7);
		item->pos.x_pos = 32256;
		item->pos.y_pos = 0;
		item->pos.z_pos = 90624;
		item->room_number = 104;
		ResetCutanimate(ANIMATING8);
		item = ResetCutanimate(ANIMATING9);
		item->pos.x_pos = 31232;
		item->pos.y_pos = 0;
		item->pos.z_pos = 91648;
		item->room_number = 61;
		ResetCutanimate(ANIMATING12);
		ResetCutanimate(ANIMATING13);
		ResetCutanimate(ANIMATING14);
		ResetCutanimate(ANIMATING15);
		S_CDPlay(CDA_XA12_FLYBY4, 0);
		InitialiseSpotCam(num);
		return;

	case 2:
		S_CDPlay(CDA_XA11_FLYBY3, 0);
		InitialiseSpotCam(2);
		return;

	case 3:
		S_CDPlay(CDA_XA11_FLYBY2, 0);
		InitialiseSpotCam(3);
		return;

	default:
		InitialiseSpotCam(num);
		return;
	}	
}

void CutLaraBubbles()
{
	PHD_VECTOR offset;
	PHD_VECTOR pos;
	short roomnum;

	offset.x = 0;
	offset.y = -4;
	offset.z = 64;
	GetLaraJointPos(&offset, 8);
	roomnum = camera.pos.room_number;
	IsRoomOutsideNo = -1;
	IsRoomOutside(offset.x, offset.y, offset.z);

	if (IsRoomOutsideNo != -1)
		roomnum = IsRoomOutsideNo;

	for (int i = 2; i > 0; --i)
	{
		pos.x = (GetRandomControl() & 0x3F) + offset.x - 32;
		pos.y = (GetRandomControl() & 0x3F) + offset.y - 32;
		pos.z = (GetRandomControl() & 0x3F) + offset.z - 32;
		CreateBubble(&pos, roomnum, 7, 8, 0, 0, -96, 0);
	}
}

void deal_with_pistols(unsigned short* shootdata)
{
	PHD_VECTOR pos;
	int f;

	f = GLOBAL_cutseq_frame;

	while (1)
	{
		short dat;

		dat = *shootdata++;

		if (dat == -1)
			break;

		if (f == (dat & 0x3FFF))
		{
			if (dat & 0x8000)
				cutseq_shoot_pistols(14);

			if (dat & 0x4000)
				cutseq_shoot_pistols(11);
		}
	}

	if (SmokeCountL || SmokeCountR)
	{
		lara.mesh_ptrs[14] = meshes[objects[LARA_SCREAM].mesh_index + (14 * 2)];

		if (SmokeCountL)
		{
			pos.x = 4;
			pos.y = 128;
			pos.z = 40;
			GetLaraJointPos(&pos, 14);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
		}

		if (SmokeCountR)
		{
			pos.x = -16;
			pos.y = 128;
			pos.z = 40;
			GetLaraJointPos(&pos, 11);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
		}
	}
	else
		lara.mesh_ptrs[14] = meshes[objects[0].mesh_index + (14 * 2)];

	if (lara.left_arm.flash_gun)
	{
		lara.left_arm.flash_gun--;
		trigger_weapon_dynamics(14);
	}
	if (lara.right_arm.flash_gun)
	{
		lara.right_arm.flash_gun--;
		trigger_weapon_dynamics(11);
	}
}

void handle_lara_chatting(short* _ranges)//short* ranges//until the anim ranges array is moved into the dll.
{
	int r1, r2, f, rndme;
	short* poo;

	f = GLOBAL_cutseq_frame;
	lara_chat_cnt = (lara_chat_cnt - 1) & 1;
	poo = _ranges;

	while (1)
	{
		r1 = poo[0];
		r2 = poo[1];

		if (r1 == -1)
		{
			lara.mesh_ptrs[14] = meshes[objects[LARA_SKIN].mesh_index + 2 * 14];
			return;
		}

		if (f >= r1 && f <= r2)
			break;

		poo += 2;
	}

	if (!lara_chat_cnt)
	{
		rndme = objects[(GetRandomControl() & 3) + LARA_SPEECH_HEAD1].mesh_index;	
		lara.mesh_ptrs[14] = meshes[rndme + (14 * 2)];
	}
}

void handle_actor_chatting(int speechslot, int node, int slot, int objslot, short* _ranges)//short* ranges//until the anim ranges array is moved into the dll.
{
	int r1, r2, f, rnd;

	f = GLOBAL_cutseq_frame;
	rnd = GetRandomControl() & 1;

	while (1)
	{
		r1 = _ranges[0];
		r2 = _ranges[1];

		if (r1 == -1)
		{
			cutseq_meshswapbits[slot] &= ~(1 << node);
			return;
		}

		if (f >= r1 && f <= r2)
			break;

		_ranges += 2;
	}

	if (!actor_chat_cnt)
	{
		cutseq_meshswapbits[slot] |= 1 << node;
		*(&meshes[2 * node] + objects[objslot].mesh_index + 1) = (&meshes[2 * node])[objects[speechslot + rnd].mesh_index];

		if ((GetRandomControl() & 7) >= 6)
			cutseq_meshswapbits[slot] &= ~(1 << node);
	}

}

void trigger_item_in_room(short room_number, int object)
{
	short num, nex;
	ITEM_INFO* item;

	num = room[room_number].item_number;

	while (num != NO_ITEM)
	{
		item = &items[num];
		nex = item->next_item;

		if (item->object_number == object)
		{
			AddActiveItem(num);
			item->status = ITEM_ACTIVE;
			item->flags |= IFLAG_ACTIVATION_MASK;
		}

		num = nex;
	}
}

void untrigger_item_in_room(short room_number, int object)
{
	short num, nex;
	ITEM_INFO* item;

	num = room[room_number].item_number;

	while (num != NO_ITEM)
	{
		item = &items[num];
		nex = item->next_item;

		if (item->object_number == object)
		{
			RemoveActiveItem(num);
			item->status = ITEM_DEACTIVATED;
			item->flags |= ~IFLAG_ACTIVATION_MASK;
		}

		num = nex;
	}
}

void deal_with_actor_shooting(unsigned short* shootdata, int actornum, int nodenum, PHD_VECTOR* pos)
{
	int f;
	unsigned short dat;
	MATRIX3D arse;

	dat = *shootdata++;
	f = GLOBAL_cutseq_frame;

	if (dat != -1)
	{
		do
		{
			if (f == dat || f == dat + 1)
			{
				GrabActorMatrix(actornum, nodenum, &arse);
				trig_actor_gunflash(&arse, pos);
				GetActorJointAbsPosition(actornum, nodenum, pos);
				TriggerDynamic(pos->x, pos->y, pos->z, 16, (GetRandomControl() & 0x3F) + 0xC0, (GetRandomControl() & 0x1F) + 0x80, (GetRandomControl() & 0x3F));
				break;
			}

			dat = *shootdata++;

		} while (dat != 0xFFFF);
	}

}

void DelsHandyTeleportLara(int x, int y, int z, int yrot)
{
	lara_item->pos.x_pos = x;
	lara_item->pos.y_pos = y;
	lara_item->pos.z_pos = z;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara_item->pos.x_rot = 0;
	lara_item->pos.y_rot = yrot;
	lara_item->pos.z_rot = 0;
	IsRoomOutside(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	if (IsRoomOutsideNo != lara_item->room_number)
		ItemNewRoom(lara.item_number, IsRoomOutsideNo);

	lara_item->goal_anim_state = AS_STOP;
	lara_item->current_anim_state = AS_STOP;
	lara_item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
	lara_item->anim_number = ANIMATION_LARA_STAY_SOLID;
	lara_item->speed = 0;
	lara_item->fallspeed = 0;
	lara_item->gravity_status = 0;
	lara.gun_status = LG_NO_ARMS;
	camera.fixed_camera = 1;
}

void InitPackNodes(NODELOADHEADER* lnode, PACKNODE* pnode, char* packed, int numnodes)
{
	int offset;
	int xoff;
	int yoff;
	int zoff;

	offset = ((numnodes << 3) - numnodes) << 1;

	if (numnodes > 0)
	{
		for (int i = 0; i < numnodes; i++)
		{
			pnode->xkey = (unsigned short)lnode->xkey;
			pnode->ykey = (unsigned short)lnode->ykey;
			pnode->zkey = (unsigned short)lnode->zkey;
			pnode->decode_x.packmethod = (lnode->packmethod >> 10) & 0xF;
			pnode->decode_y.packmethod = (lnode->packmethod >> 5) & 0xF;
			pnode->decode_z.packmethod = (lnode->packmethod) & 0xF;
			pnode->xlength = lnode->xlength;
			pnode->ylength = lnode->ylength;
			pnode->zlength = lnode->zlength;
			xoff = ((lnode->xlength * pnode->decode_x.packmethod) >> 3) + 4;
			yoff = ((lnode->ylength * pnode->decode_y.packmethod) >> 3) + 4;
			zoff = ((lnode->zlength * pnode->decode_z.packmethod) >> 3) + 4;
			lnode++;
			pnode->xpacked = &packed[offset];
			pnode->ypacked = &packed[offset + xoff];
			pnode->zpacked = &packed[offset + xoff + yoff];
			pnode++;
			offset += xoff + yoff + zoff;
		}
	}

	return;
}

short GetTrackWord(int off, char* packed, int packmethod)
{
	int ret;
	long offset, offset2;

	offset = packmethod * off;
	offset2 = offset >> 3;

	ret = ((1 << packmethod) - 1) & ((unsigned int)(*(unsigned char*)(packed + offset2) |
		((*(unsigned char*)(packed + offset2 + 1) |
			(*(unsigned short*)(packed + offset2 + 2) << 8)) << 8)) >> (offset & 7));

	if (((1 << (packmethod - 1)) & ret) != 0)
		return (unsigned int)(ret | ~((1 << packmethod) - 1));

	return ret;
}

short DecodeTrack(char* packed, RTDECODE* decode)
{
	short word;

	if (!decode->decodetype)
	{
		word = GetTrackWord(decode->off, packed, decode->packmethod);

		if ((word & 0x20))
		{
			if (!(word & 0xF))
				decode->counter = 16;
			else
				decode->counter = (word & 0xF);

			decode->decodetype = 1;
			decode->off++;
			decode->length--;
		}
		else
		{
			decode->decodetype = 2;

			if ((word & 0x10))
			{
				GetTrackWord(decode->off + 1, packed, decode->packmethod);
				decode->counter = ((word & 7) << 5) | (GetTrackWord(decode->off + 1, packed, decode->packmethod) & 0x1F);
				decode->data = GetTrackWord(decode->off + 2, packed, decode->packmethod);
				decode->off += 3;
				decode->length -= 3;
			}
			else
			{
				decode->data = GetTrackWord(decode->off + 1, packed, decode->packmethod);
				decode->counter = word & 0x7;
				decode->off += 2;
				decode->length -= 2;
			}
		}
	}

	if (decode->decodetype == 2)
	{
		if (!--decode->counter)
			decode->decodetype = 0;

		return decode->data;
	}
	else
	{
		word = GetTrackWord(decode->off, packed, decode->packmethod);
		decode->off++;
		decode->length--;

		if (!--decode->counter)
			decode->decodetype = 0;

		return word;
	}
}

void DecodeAnim(PACKNODE* node, int num_nodes, int frame, int flags)
{
	if (!frame)
	{
		for (int i = num_nodes; i; i--, node++)
		{
			node->decode_x.off = 0;
			node->decode_x.counter = 0;
			node->decode_x.data = 0;
			node->decode_x.decodetype = 0;
			node->decode_y.off = 0;
			node->decode_y.counter = 0;
			node->decode_y.data = 0;
			node->decode_y.decodetype = 0;
			node->decode_z.off = 0;
			node->decode_z.counter = 0;
			node->decode_z.data = 0;
			node->decode_z.decodetype = 0;
			node->xrot_run = node->xkey;
			node->yrot_run = node->ykey;
			node->zrot_run = node->zkey;
			node->decode_x.length = node->xlength;
			node->decode_y.length = node->ylength;
			node->decode_z.length = node->zlength;
		}

		return;
	}

	node->xrot_run += DecodeTrack(node->xpacked, &node->decode_x);
	node->yrot_run += DecodeTrack(node->ypacked, &node->decode_y);
	node->zrot_run += DecodeTrack(node->zpacked, &node->decode_z);
	node++;

	for (int i = 1; i < num_nodes; i++, node++)
	{
		node->xrot_run += DecodeTrack(node->xpacked, &node->decode_x);
		node->yrot_run += DecodeTrack(node->ypacked, &node->decode_y);
		node->zrot_run += DecodeTrack(node->zpacked, &node->decode_z);
		node->xrot_run &= flags;
		node->yrot_run &= flags;
		node->zrot_run &= flags;
	}
}

void do_new_cutscene_camera()
{
	PACKNODE* nodes;
	tr5_vertex cam_pos;
	tr5_vertex cam_target;

	if (cutseq_control_routines[cutseq_num].control_func)
		cutseq_control_routines[cutseq_num].control_func();

	DecodeAnim(camera_pnodes, 2, GLOBAL_cutseq_frame, 65535);
	nodes = camera_pnodes;
	camera.target.y = nodes[0].yrot_run << 1;
	camera.pos.y = nodes[1].yrot_run << 1;

	switch (cutrot)
	{
	case 0:
		camera.target.x = nodes[0].xrot_run << 1;
		camera.target.z = nodes[0].zrot_run << 1;
		camera.pos.x = nodes[1].xrot_run << 1;
		camera.pos.z = nodes[1].zrot_run << 1;
		break;

	case 1:
		camera.target.x = nodes[0].zrot_run << 1;
		camera.target.z = -(nodes[0].xrot_run << 1);
		camera.pos.x = nodes[1].zrot_run << 1;
		camera.pos.z = -(nodes[1].xrot_run << 1);
		break;

	case 2:
		camera.target.x = -(nodes[0].xrot_run << 1);
		camera.target.z = -(nodes[0].zrot_run << 1);
		camera.pos.x = -(nodes[1].xrot_run << 1);
		camera.pos.z = -(nodes[1].zrot_run << 1);
		break;

	case 3:
		camera.target.x = -(nodes[0].zrot_run << 1);
		camera.target.z = nodes[0].xrot_run << 1;
		camera.pos.x = -(nodes[1].zrot_run << 1);
		camera.pos.z = nodes[1].xrot_run << 1;
		break;
	}

	camera.target.x += GLOBAL_cutme->orgx;
	camera.target.y += GLOBAL_cutme->orgy;
	camera.target.z += GLOBAL_cutme->orgz;
	camera.pos.x += GLOBAL_cutme->orgx;
	camera.pos.y += GLOBAL_cutme->orgy;
	camera.pos.z += GLOBAL_cutme->orgz;
	IsRoomOutsideNo = -1;
	IsRoomOutside(camera.pos.x, camera.pos.y, camera.pos.z);

	if (IsRoomOutsideNo != -1)
		camera.pos.room_number = IsRoomOutsideNo;

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	cam_pos.x = (float)camera.pos.x;
	cam_pos.y = (float)camera.pos.y;
	cam_pos.z = (float)camera.pos.z;
	cam_target.x = (float)camera.target.x;
	cam_target.y = (float)camera.target.y;
	cam_target.z = (float)camera.target.z;
	aLookAt(cam_pos, cam_target, 0);

	if (GLOBAL_cutme->actor_data[0].objslot != NO_ITEM)
		DecodeAnim(actor_pnodes[0], 16, GLOBAL_cutseq_frame, 1023);

	for (int n = 1; n < GLOBAL_cutme->numactors; n++)
		DecodeAnim(actor_pnodes[n], GLOBAL_cutme->actor_data[n].nodes + 1, GLOBAL_cutseq_frame, 1023);

	GLOBAL_cutseq_frame++;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames - 8 && cutseq_trig == 2)
		cutseq_trig = 3;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames)
		GLOBAL_cutseq_frame = GLOBAL_numcutseq_frames;
}

void updateAnimFrame(PACKNODE* node, int flags, short* frame)
{
	short y;
	short* nex;

	frame[7] = 3 * node->yrot_run;

	switch (cutrot)
	{
	case 0:
		frame[6] = 3 * node->xrot_run;
		frame[8] = 3 * node->zrot_run;
		break;

	case 1:
		frame[6] = 3 * node->zrot_run;
		frame[8] = -3 * node->xrot_run;
		break;

	case 2:
		frame[6] = -3 * node->xrot_run;
		frame[8] = -3 * node->zrot_run;
		break;

	case 3:
		frame[6] = -3 * node->zrot_run;
		frame[8] = 3 * node->xrot_run;
		break;
	}

	nex = frame + 9;

	for (int i = 1; i < flags; i++, nex += 2)
	{
		y = node[i].yrot_run;

		if (cutrot && i == 1)
			y = (y + (cutrot << 8)) & 0x3FF;

		nex[0] = (((node[i].xrot_run << 10 | y) << 10) | node[i].zrot_run) >> 16;
		nex[1] = ((node[i].xrot_run << 10 | y) << 10) | node[i].zrot_run;
	}
}

void DrawCutSeqActors()
{
	OBJECT_INFO* obj;
	short** mesh;
	long* bone;
	short* rot;
	int n;

	phd_PushMatrix();

	if (GLOBAL_cutme->numactors <= 1)
	{
		phd_PopMatrix();
		return;
	}

	for (int i = 1; i < GLOBAL_cutme->numactors; i++)
	{
		phd_PushMatrix();

		if (cutseq_meshbits[i] & 0x80000000)
		{
			n = 1;
			updateAnimFrame(actor_pnodes[i], GLOBAL_cutme->actor_data[i].nodes + 1, temp_rotation_buffer);
			phd_TranslateAbs(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
			obj = &objects[GLOBAL_cutme->actor_data[i].objslot];
			bone = &bones[obj->bone_index];
			mesh = &meshes[obj->mesh_index];
			CalcActorLighting(&duff_item[i - 1], obj, temp_rotation_buffer);
			phd_TranslateRel(temp_rotation_buffer[6], temp_rotation_buffer[7], temp_rotation_buffer[8]);
			rot = &temp_rotation_buffer[9];
			gar_RotYXZsuperpack(&rot, 0);

			if (cutseq_meshbits[i] & 1)
			{
				if (cutseq_meshswapbits[i] & 1)
				{
					if (i != 1 || cut_seethrough == 128)
						phd_PutPolygons(*(mesh + 1), -1);
					else
						phd_PutPolygons_seethrough(*(mesh + 1), cut_seethrough);
				}
				else if (i != 1 || cut_seethrough == 128)
					phd_PutPolygons(*mesh, -1);
				else
					phd_PutPolygons_seethrough(*mesh, cut_seethrough);
			}

			mesh += 2;

			for (int j = 0; j < obj->nmeshes - 1; j++, bone += 4, mesh += 2)
			{
				if (*bone & 1)
					phd_PopMatrix();

				if (*bone & 2)
					phd_PushMatrix();

				phd_TranslateRel(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack(&rot, 0);
				n <<= 1;

				if (cutseq_meshbits[i] & n)
				{
					if (n & cutseq_meshswapbits[i])
					{
						if (i != 1 || cut_seethrough == 128)
							phd_PutPolygons(*(mesh + 1), -1);
						else
							phd_PutPolygons_seethrough(*(mesh + 1), cut_seethrough);
					}
					else if (i != 1 || cut_seethrough == 128)
						phd_PutPolygons(*mesh, -1);
					else
						phd_PutPolygons_seethrough(*mesh, cut_seethrough);
				}
			}
		}

		phd_PopMatrix();
	}

	phd_PopMatrix();
}

void CalcActorLighting(ITEM_INFO* item, OBJECT_INFO* obj, short* rot)
{
	PHD_VECTOR pos;

	GetJointAbsPositionCutSeq(item, obj, rot, &pos);
	IsRoomOutsideNo = -1;
	IsRoomOutside(pos.x, pos.y, pos.z);

	if (IsRoomOutsideNo != -1)
		item->room_number = IsRoomOutsideNo;

	current_item = item;
	item->il.fcnt = -1;
	item->il.item_pos.x = pos.x;
	item->il.item_pos.y = pos.y;
	item->il.item_pos.z = pos.z;
	CalcAmbientLight(item);
	CreateLightList(item);
}

void GetJointAbsPositionCutSeq(ITEM_INFO* item, OBJECT_INFO* obj, short* rot, PHD_VECTOR* pos)
{
	short* rotation1;

	phd_PushUnitMatrix();
	phd_TranslateRel(rot[6], rot[7], rot[8]);
	rotation1 = rot + 9;
	gar_RotYXZsuperpack(&rotation1, 0);
	pos->x = phd_mxptr[3] >> 14;
	pos->y = phd_mxptr[7] >> 14;
	pos->z = phd_mxptr[11] >> 14;
	pos->x += item->pos.x_pos;
	pos->y += item->pos.y_pos;
	pos->z += item->pos.z_pos;
	phd_PopMatrix();
}

void frigup_lara()
{
	OBJECT_INFO* object;
	long* bone;
	short* frame;

	lara_item->pos.x_pos = GLOBAL_cutme->orgx;
	lara_item->pos.y_pos = GLOBAL_cutme->orgy;
	lara_item->pos.z_pos = GLOBAL_cutme->orgz;

	if (GLOBAL_cutme->actor_data[0].objslot == NO_ITEM)
		return;

	frame = &temp_rotation_buffer[0];
	object = &objects[lara_item->object_number];
	bone = &bones[object->bone_index];
	updateAnimFrame(actor_pnodes[0], 16, frame);
	Rich_CalcLaraMatrices_Normal(frame, bone, 0);
	phd_PushUnitMatrix();
	Rich_CalcLaraMatrices_Normal(frame, bone, 1);
	phd_PopMatrix();
	HairControl(0, 0, frame);

	if ((gfLevelFlags & GF_LVOP_YOUNG_LARA))
		HairControl(0, 1, frame);

	GLaraShadowframe = &frig_shadow_bbox[0];
}

void CalculateObjectLightingLaraCutSeq()
{
	PHD_VECTOR pos;
	short room_num, room_num2;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetLaraJointPos(&pos, 8);
	room_num = lara_item->room_number;
	IsRoomOutsideNo = -1;
	IsRoomOutside(pos.x, pos.y, pos.z);

	if (IsRoomOutsideNo != -1)
		room_num = IsRoomOutsideNo;

	room_num2 = lara_item->room_number;
	current_item = lara_item;
	lara_item->il.item_pos.x = pos.x;
	lara_item->il.item_pos.y = pos.y;
	lara_item->il.item_pos.z = pos.z;
	lara_item->room_number = room_num;
	CalcAmbientLight(lara_item);
	CreateLightList(lara_item);
	lara_item->room_number = room_num2;
}

void GrabActorMatrix(int actornum, unsigned long nodenum, MATRIX3D* matrixstash)
{
	OBJECT_INFO* obj;
	short* rot;
	long* bone;
	long bit;

	bit = 1;
	nodenum = 1 << nodenum;
	phd_PushMatrix();
	updateAnimFrame(actor_pnodes[actornum], GLOBAL_cutme->actor_data[actornum].nodes + 1, temp_rotation_buffer);
	obj = &objects[GLOBAL_cutme->actor_data[actornum].objslot];
	phd_TranslateAbs(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
	bone = &bones[obj->bone_index];
	phd_TranslateRel(temp_rotation_buffer[6], temp_rotation_buffer[7], temp_rotation_buffer[8]);
	rot = &temp_rotation_buffer[0] + 9;
	gar_RotYXZsuperpack(&rot, 0);

	if (nodenum == bit)
	{
		*((float*)matrixstash + 0) = *(aMXPtr + 0);
		*((float*)matrixstash + 1) = *(aMXPtr + 1);
		*((float*)matrixstash + 2) = *(aMXPtr + 2);
		*((float*)matrixstash + 3) = *(aMXPtr + 3);
		*((float*)matrixstash + 4) = *(aMXPtr + 4);
		*((float*)matrixstash + 5) = *(aMXPtr + 5);
		*((float*)matrixstash + 6) = *(aMXPtr + 6);
		*((float*)matrixstash + 7) = *(aMXPtr + 7);
		*((float*)matrixstash + 8) = *(aMXPtr + 8);
		*((float*)matrixstash + 9) = *(aMXPtr + 9);
		*((float*)matrixstash + 10) = *(aMXPtr + 10);
		*((float*)matrixstash + 11) = *(aMXPtr + 11);
	}

	for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4)
	{
		if (*bone & 1)
			phd_PopMatrix();

		if (*bone & 2)
			phd_PushMatrix();

		phd_TranslateRel(*(bone + 1), *(bone + 2), *(bone + 3));
		gar_RotYXZsuperpack(&rot, 0);
		bit <<= 1;

		if (nodenum == bit)
		{
			*((float*)matrixstash + 0) = *(aMXPtr + 0);
			*((float*)matrixstash + 1) = *(aMXPtr + 1);
			*((float*)matrixstash + 2) = *(aMXPtr + 2);
			*((float*)matrixstash + 3) = *(aMXPtr + 3);
			*((float*)matrixstash + 4) = *(aMXPtr + 4);
			*((float*)matrixstash + 5) = *(aMXPtr + 5);
			*((float*)matrixstash + 6) = *(aMXPtr + 6);
			*((float*)matrixstash + 7) = *(aMXPtr + 7);
			*((float*)matrixstash + 8) = *(aMXPtr + 8);
			*((float*)matrixstash + 9) = *(aMXPtr + 9);
			*((float*)matrixstash + 10) = *(aMXPtr + 10);
			*((float*)matrixstash + 11) = *(aMXPtr + 11);
		}
	}

	phd_PopMatrix();
}

void inject_deltaPak()
{
	INJECT(0x00425390, andrea1_init);
	INJECT(0x004253B0, andrea1_control);
	INJECT(0x004254F0, andrea1_end);
	INJECT(0x00425520, andrea2_init);
	INJECT(0x00425540, andrea2_control);
	INJECT(0x004258D0, andrea2_end);
	INJECT(0x004261C0, andrea3_init);
	INJECT(0x004261E0, andrea3_control);
	INJECT(0x00426270, andrea3_end);
	INJECT(0x00426350, andrea3b_init);
	INJECT(0x00426410, andrea3b_control);
	INJECT(0x00426570, andrea3b_end);
	INJECT(0x00427F70, andrea4_init);
	INJECT(0x00427F90, andrea4_control);
	INJECT(0x00428020, andrea4_end);
	INJECT(0x004234B0, cranecut_init);
	INJECT(0x004235C0, cranecut_control);
	INJECT(0x00423530, cranecut_end);
	INJECT(0x00423790, richcut1_init);
	INJECT(0x004238E0, richcut1_control);
	INJECT(0x004238A0, richcut1_end);
	INJECT(0x004239B0, richcut2_init);
	INJECT(0x00423A00, richcut2_control);
	INJECT(0x004239E0, richcut2_end);
	INJECT(0x00423A90, richcut3_init);
	INJECT(0x00423B00, richcut3_control);
	INJECT(0x00423AC0, richcut3_end);
	INJECT(0x00425CE0, richcut4_init);
	INJECT(0x00425D80, richcut4_control);
	INJECT(0x00425E60, richcut4_end);
	INJECT(0x00424B40, andy1_init);
	INJECT(0x00424B60, andy1_control);
	INJECT(0x00424C00, andy1_end);
	INJECT(0x00424DC0, andy2_init);
	INJECT(0x00424DE0, andy2_control);
	INJECT(0x00424EC0, andy2_end);
	INJECT(0x00425A70, andy3_init);
	INJECT(0x00425AC0, andy3_control);
	INJECT(0x00425AF0, andy3_end);
	INJECT(0x00425EB0, andy4_init);
	INJECT(0x00425ED0, andy4_control);
	INJECT(0x00425FF0, andy4_end);
	INJECT(0x00426030, andy4b_init);
	INJECT(0x00426050, andy4b_control);
	INJECT(0x00426140, andy4b_end);
	INJECT(0x00426660, andy5_init);
	INJECT(0x00426680, andy5_control);
	INJECT(0x004267F0, andy5_end);
	INJECT(0x00426F70, andy6_init);
	INJECT(0x00426F90, andy6_control);
	INJECT(0x004270B0, andy6_end);
	INJECT(0x00427150, andy7_init);
	INJECT(0x00427170, andy7_control);
	INJECT(0x004271B0, andy7_end);
	INJECT(0x00427750, andy8_init);
	INJECT(0x00427770, andy8_control);
	INJECT(0x00427810, andy8_end);
	INJECT(0x00427630, andy9_init);
	INJECT(0x00427660, andy9_control);
	INJECT(0x00427720, andy9_end);
	INJECT(0x00427880, andy10_init);
	INJECT(0x004279C0, andy10_control);
	INJECT(0x00427B30, andy10_end);
	INJECT(0x00428770, andy11_init);
	INJECT(0x004287C0, andy11_control);
	INJECT(0x00428920, andy11_end);
	INJECT(0x004270D0, andypew_init);
	INJECT(0x004270F0, andypew_control);
	INJECT(0x00427110, andypew_end);
	INJECT(0x004271E0, cossack_init);
	INJECT(0x00427200, cossack_control);
	INJECT(0x00427470, cossack_end);
	INJECT(0x00428080, monk2_init);
	INJECT(0x004280A0, monk2_control);
	INJECT(0x004281B0, monk2_end);
	INJECT(0x004281D0, swampy_init);
	INJECT(0x00428270, swampy_control);
	INJECT(0x004282A0, swampy_end);
	INJECT(0x00424C30, hamgate_init);
	INJECT(0x00424CC0, hamgate_control);
	INJECT(0x00424D30, hamgate_end);
	INJECT(0x00423CC0, stealth3_start);
	INJECT(0x00423DF0, stealth3_end);
	INJECT(0x004247A0, joby2_init);
	INJECT(0x00424820, joby2_control);
	INJECT(0x00424A20, joby2_end);
	INJECT(0x00424A70, joby3_init);
	INJECT(0x00424A90, joby3_control);
	INJECT(0x00424B20, joby3_end);
	INJECT(0x004251F0, joby4_init);
	INJECT(0x00425230, joby4_control);
	INJECT(0x00425340, joby4_end);
	INJECT(0x004258F0, joby5_init);
	INJECT(0x00425940, joby5_control);
	INJECT(0x00425A20, joby5_end);
	INJECT(0x00426810, joby6_init);
	INJECT(0x00426890, joby6_control);
	INJECT(0x004268D0, joby6_end);
	INJECT(0x00427C10, joby7_init);
	INJECT(0x00427C50, joby7_control);
	INJECT(0x00427EA0, joby7_end);
	INJECT(0x00426910, joby8_init);
	INJECT(0x00426A00, joby8_control);
	INJECT(0x00426F40, joby8_end);
	INJECT(0x00425B70, joby9_init);
	INJECT(0x00425B90, joby9_control);
	INJECT(0x00425BD0, joby9_end);
	INJECT(0x00425C00, joby10_init);
	INJECT(0x00425C20, joby10_control);
	INJECT(0x00425CA0, joby10_end);
	INJECT(0x00423B40, special1_init);
	INJECT(0x00423B60, special1_control);
	INJECT(0x00423B80, special1_end);
	INJECT(0x00423BA0, special2_init);
	INJECT(0x00423BC0, special2_control);
	INJECT(0x00423BE0, special2_end);
	INJECT(0x00423C00, special3_init);
	INJECT(0x00423C20, special3_control);
	INJECT(0x00423C40, special3_end);
	INJECT(0x00423C60, special4_init);
	INJECT(0x00423C80, special4_control);
	INJECT(0x00423CA0, special4_end);
	INJECT(0x00425B30, do_catapult_meshswap);
	INJECT(0x00426180, do_clanger_meshswap);
	INJECT(0x00424D80, do_hammer_meshswap);
	INJECT(0x00424EF0, setup_preist_meshswap);
	INJECT(0x00428040, do_pierre_gun_meshswap);
	//do_chalk_meshswap is optimized out BUT its existence is confirmed, it's in both the mac exe and the PSX symbols.
	INJECT(0x00420E10, handle_cutseq_triggering);
	INJECT(0x00422680, cutseq_givelara_pistols);
	INJECT(0x004226B0, cutseq_removelara_pistols);
	INJECT(0x00421480, do_new_cutscene_camera);	
	INJECT(0x00421880, InitPackNodes);
	INJECT(0x00421D60, frigup_lara);
	INJECT(0x00422010, finish_cutseq);
	INJECT(0x00422570, CalculateObjectLightingLaraCutSeq);
	INJECT(0x004226E0, cutseq_givelara_hk);
	INJECT(0x00422700, cutseq_removelara_hk);
	INJECT(0x00422740, cutseq_shoot_pistols);
	INJECT(0x00422780, trigger_weapon_dynamics);
	INJECT(0x00422840, deal_with_pistols);
//	INJECT(0x00422A20, cutseq_kill_item);
//	INJECT(0x00422AF0, cutseq_restore_item);
//	INJECT(0x00422B90, Load_and_Init_Cutseq);
//	INJECT(0x00422C30, init_cutseq_actors);
	INJECT(0x00422F80, DelsHandyTeleportLara);
	INJECT(0x00423170, handle_lara_chatting);
	INJECT(0x00423210, handle_actor_chatting);
	INJECT(0x00423330, trigger_item_in_room);
	INJECT(0x004233D0, untrigger_item_in_room);
	INJECT(0x00423FB0, deal_with_actor_shooting);
	INJECT(0x00424080, GrabActorMatrix);
//	INJECT(0x004243A0, GetActorJointAbsPosition);
	INJECT(0x00424570, TriggerActorBlood);
	INJECT(0x004245C0, TriggerDelSmoke);
	INJECT(0x004274B0, TriggerDelBrownSmoke);
	INJECT(0x00424F30, DelTorchFlames);
	INJECT(0x00428390, CutLaraBubbles);
	INJECT(0x004284A0, trigger_title_spotcam);
	INJECT(0x00428650, ResetCutanimate);
	INJECT(0x004286E0, Cutanimate);
	INJECT(0x00423470, find_a_fucking_item);
	INJECT(0x004219E0, DecodeAnim);
	INJECT(0x00421B50, DecodeTrack);
	INJECT(0x00421CD0, GetTrackWord);
	INJECT(0x00421E90, updateAnimFrame);
	INJECT(0x00422030, DrawCutSeqActors);
	INJECT(0x004223C0, CalcActorLighting);
	INJECT(0x00422490, GetJointAbsPositionCutSeq);
}
