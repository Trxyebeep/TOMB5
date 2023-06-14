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
#include "camera.h"
#include "gameflow.h"
#include "hair.h"
#include "delstuff.h"
#include "effects.h"
#include "text.h"
#include "health.h"
#include "objects.h"
#include "newinv2.h"
#include "spotcam.h"
#include "cutseq.h"
#include "../specific/lighting.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "draw.h"
#include "subsuit.h"
#include "../specific/function_stubs.h"
#include "../specific/audio.h"
#include "../specific/alexstuff.h"
#include "lara.h"
#include "savegame.h"
#include "../tomb5/tomb5.h"

ushort old_lara_LHolster;

long nSpecialCut;

NEW_CUTSCENE* GLOBAL_cutme;
char* cutseq_resident_addresses[47];
ITEM_INFO duff_item[10];
long GLOBAL_cutseq_frame;
long GLOBAL_playing_cutseq = 0;
long GLOBAL_numcutseq_frames;
long cutrot = 0;
long cutseq_num = 0;
long cutseq_trig = 0;
long lastcamnum;
ushort dels_cutseq_player = 0;
char jobyfrigger = 0;
char Chris_Menu = 0;

static PACKNODE* camera_pnodes;
static PACKNODE* actor_pnodes[10];
static camera_type GLOBAL_oldcamtype;
static char* GLOBAL_resident_depack_buffers;
static ulong cutseq_meshswapbits[10];
static ulong cutseq_meshbits[10];
static long cut_seethrough = 128;
static short frig_shadow_bbox[6] = { -165, 150, -777, 1, -87, 78 };
static uchar disable_horizon = 0;
static char lara_chat_cnt = 0;
static char actor_chat_cnt = 0;
static char cutseq_busy_timeout = 0;
static long numnailed;
static char old_status_flags[32];
static ushort old_status_flags2[32];
static short temp_rotation_buffer[160];
static short old_lara_holster;

ushort larson_pistols_info1[2] = { 912, 0xFFFF };
ushort andrea3_pistols_info[5] = { 17739, 34126, 34135, 17751, 0xFFFF };
ushort crane_pistols_info[11] = { 32821, 32829, 33221, 33231, 33258, 16433, 16442, 16841, 16862, 16878, 0xFFFF };
ushort craneguard_pistols_info[7] = { 106, 117, 240, 247, 260, 274, 0xFFFF };

short larson_chat_ranges1[12] = { 943, 971, 1196, 1225, 1246, 1302, 1516, 1548, 1818, 1890, -1, -1 };
short larson_chat_ranges2[8] = { 450, 597, 1246, 1331, 2090, 2147, -1, -1 };
short lara_chat_ranges_andrea3[14] = { 40, 94, 260, 321, 428, 447, 483, 527, 706, 829, 858, 940, -1, -1 };
short larson_chat_ranges3[14] = { 160, 241, 338, 409, 559, 689, 871, 1122, 1138, 1298, 1460, 1523, -1, -1 };
short lara_chat_ranges_andrea3b[12] = { 522, 539, 547, 568, 702, 736, 745, 770, 792, 978, -1, -1 };
short larson_chat_ranges3b[4] = { 596, 685, -1, -1 };
short lara_chat_ranges_andrea4[22] = { 457, 540, 945, 1023, 1611, 1655, 1670, 1684, 1693, 1726, 2115, 2151, 3074, 3085, 3491, 3563, 3582, 3615, 3689, 3754, -1, -1 };
short lara_chat_ranges_andrea1[4] = { 584, 633, -1, -1 };
short voncroy_chat_ranges_richcut2[20] = { 481, 501, 656, 684, 695, 759, 768, 830, 927, 970, 983, 1032, 1044, 1091, 1478, 1576, 1592, 1635, -1, -1 };
short associate_chat_ranges_richcut2[12] = { 1117, 1301, 1665, 1827, 1841, 1857, 1866, 1892, 1906, 1962, -1, -1 };
short guard_chat_ranges_richcut2[10] = { 54, 176, 517, 647, 847, 914, 1321, 1356, -1, -1 };
short lara_chat_ranges_andy1[14] = { 786, 819, 1355, 1365, 1389, 1416, 2152, 2173, 3109, 3157,	3178, 3188, -1, -1 };
short hanged_man_chat_ranges_andy1[18] = { 464, 557, 574, 753, 840, 1322, 1430, 2124, 2192, 3067,3201, 3215, 3228, 3404, 3418, 3862, -1, -1 };
short lara_chat_ranges_andy2[8] = { 1466, 1490, 1509, 1531, 2009, 2045, -1, -1 };
short priest_chat_ranges_andy2[24] = { 239, 331, 736, 801, 818, 871, 1293, 1307, 1324, 1437, 1690, 1716, 1736, 1870, 1893, 1976, 2109, 2155, 2173, 2287, 2326, 2432, -1, -1 };
short lara_chat_ranges_andy6[8] = { 712, 727, 751, 771, 844, 890, -1, -1 };
short priest_chat_ranges_andy6[18] = { 520, 672, 793, 824, 924, 938, 948, 977, 992, 1137, 1151, 1197, 1220, 1298, 1309, 1381, -1, -1 };
short lara_chat_ranges_andy7[14] = { 381, 393, 405, 446, 574, 621, 746, 775, 1355, 1380, 1389, 1410, -1, -1 };
short lara_chat_ranges_andy9[6] = { 1050, 1082, 3884, 3967, -1, -1 };
short priest_chat_ranges_andy9[20] = { 588, 624, 641, 679, 699, 784, 1657, 1716, 1735, 1841, 1855, 1914, 2723, 2766, 2800, 2872, 3983, 4072, -1, -1 };
short knight_chat_ranges_andy9[14] = { 304, 552, 798, 905, 1097, 1588, 1956, 2691, 2897, 3854, 4114, 4592, -1, -1 };
short knight_chat_ranges_andy11[14] = { 436, 958, 1223, 1688, 2192, 2244, 2262, 2370, 4225, 4357, 5097, 5386, -1, -1 };
short admiral_chat_ranges_joby2[12] = { 280, 340, 348, 373, 380, 413, 719, 886, 1111, 1160, -1, -1 };
short sergie_chat_ranges_joby2[8] = { 441, 696, 902, 1054, 1954, 2152, -1, -1 };
short lara_chat_ranges_joby3[6] = { 1323, 1328, 1341, 1366, -1, -1 };
short lara_chat_ranges_joby4[10] = { 580, 615, 633, 770, 1871, 1926, 1942, 1963, -1, -1 };
short admiral_chat_ranges_joby4[20] = { 301, 391, 403, 515, 783, 816, 828, 1028, 1038, 1254, 1265, 1414, 1771, 1804, 2028, 2123, 2135, 2170, -1, -1 };
short admiral_chat_ranges_joby5[18] = { 491, 578, 873, 1113, 1244, 1285, 1304, 1321, 1333, 1518, 1879, 1977, 1987, 2037, 2105, 2244, -1, -1 };
short sergie_chat_ranges_joby5[20] = { 156, 185, 194, 395, 412, 461, 602, 841, 1129, 1152, 1170, 1228, 1541, 1558, 1575, 1651, 1665, 1784, -1, -1 };
short lara_chat_ranges_joby7[10] = { 725, 759, 781, 825, 1485, 1501, 1516, 1545, -1, -1 };
short lara_chat_ranges_joby8[20] = { 956, 1069, 1261, 1338, 1360, 1431, 1451, 1470, 1485, 1504, 1526, 1568, 1804, 1839, 2199, 2255, 2323, 2368, -1, -1 };
short actor_chat_ranges_joby8[14] = { 691, 936, 1095, 1250, 1615, 1679, 1695, 1773, 1901, 1945, 1958, 2039, -1, -1 };
short lara_chat_ranges_joby9[12] = { 642, 780, 1418, 1446, 1457, 1499, 1510, 1546, 1561, 1681, -1, -1 };
short lara_chat_ranges_joby10[12] = { 493, 524, 1300, 1327, 1897, 1933, 2521, 2555, 2980, 3008, -1, -1 };

short pierre_chat_ranges1[30] =
{
	696, 708, 725, 744, 1003, 1020, 1044, 1059, 1080, 1177, 1322, 1388, 1400, 1472, 1563,
	1584, 1594, 1610, 1640, 1798, 1923, 1959, 1974, 2008, 2018, 2029, 2037, 2068, -1, -1
};

short pierre_chat_ranges2[34] =
{
	242, 289, 302, 341, 840, 850, 863, 946, 1051, 1063, 1082, 1151, 1167, 1223, 1607, 1628, 1647,
	1661, 1683, 1702, 1725, 1848, 2400, 2429, 2688, 2761, 3168, 3187, 3205, 3230, 3398, 3444, -1, -1
};

short lara_chat_ranges_andrea2[32] =
{
	982, 998, 1007, 1025, 1457, 1510, 1915, 1966, 1978, 2037, 2051, 2071, 2177, 2257, 2270, 2301,
	2315, 2346, 2798, 2823, 3066, 3104, 3258, 3381, 3471, 3565, 3585, 3678, 3739, 3911, -1, -1
};

short pierre_chat_ranges4[48] =
{
	334, 347, 360, 424, 558, 600, 615, 772, 789, 906, 1060, 1069, 1081, 1116, 1134, 1246, 1260, 1284, 1297, 1323, 1340, 1386, 1406, 1525, 1551,
	1596, 1765, 1849, 1866, 2057, 2623, 2640, 2662, 2690, 2755, 2851, 2919, 3010, 2397, 3362, 3390, 3426, 3642, 3667, 3862, 3868, -1, -1
};

short priest_chat_ranges_andy7[32] =
{
	3, 24, 35, 116, 136, 167, 187, 271, 282, 345, 457, 508, 520, 564, 646, 722, 784,
	835, 854, 989, 1009, 1028, 1052, 1116, 1143, 1151, 1159, 1239, 1256, 1320, -1, -1
};

short lara_chat_ranges_andy11[28] =
{
	2111, 2129, 3248, 3270, 3285, 3306, 3321, 3349, 3359, 3381, 3390, 3408, 3435, 3518,
	3675, 3692, 3723, 3746, 4129, 4154, 4835, 4855, 5392, 5423, 5874, 5909, -1, -1
};

short priest_chat_ranges_andy11[30] =
{
	1063, 1099, 1119, 1158, 1718, 1844, 2522, 2581, 2895, 2917, 2936, 2995, 3542, 3618, 4452,
	4564, 4581, 4721, 4732, 4780, 4881, 5064, 5449, 5604, 5683, 5664, 5696, 5824, -1, -1
};

short admiral_chat_ranges_joby9[36] =
{
	182, 347, 375, 600, 826, 1269, 1289, 1325, 1340, 1357, 1370, 1407, 1699, 1746, 1760, 1902, 1918, 1956,
	1974, 2037, 2055, 2140, 2170, 2223, 2236, 2255, 2270, 2290, 2302, 2423, 2440, 2484, 2501, 2512, -1, -1
};

short admiral_chat_ranges_joby10[42] =
{
	145, 163, 208, 240, 251, 297, 308, 358, 372, 445, 456, 469, 546, 817, 832, 920, 935, 1277, 1337, 1379, 1392, 1551,
	1566, 1768, 1779, 1858, 1952, 2012, 2027, 2235, 2252, 2435, 2622, 2658, 2676, 2688, 2703, 2739, 2751, 2829, -1, -1
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

}

void andrea1_control()
{
	ITEM_INFO* item;
	PHD_VECTOR pos;

	switch (GLOBAL_cutseq_frame)
	{
	case 330:
		item = find_a_fucking_item(ANIMATING10);
		item->flags |= IFL_TRIGGERED;
		item->mesh_bits = item->mesh_bits & ~6 | 4;
		break;

	case 452:
		cutseq_givelara_pistols();
		break;

	case 580:
		undraw_pistol_mesh_left(WEAPON_PISTOLS);
		break;

	case 603:
		undraw_pistol_mesh_right(WEAPON_PISTOLS);
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
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, LARSON, larson_chat_ranges1);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 8, 2, ANIMATING2_MIP, pierre_chat_ranges1);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea1_end()
{
	lara_item->mesh_bits = -1;
	lara.pickupitems &= ~1;
}

void andrea2_init()
{
	cutseq_meshbits[1] &= ~0x80000000;
}

void andrea2_control()
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	long frame, r, g, b, shift;

	frame = GLOBAL_cutseq_frame;

	if (frame == 410)
		cutseq_meshbits[1] |= 0x80000000;
	else if (frame == 178)
		cutseq_givelara_pistols();
	else if (frame == 667)
		undraw_pistol_mesh_left(WEAPON_PISTOLS);
	else if (frame == 678)
		undraw_pistol_mesh_right(WEAPON_PISTOLS);
	else if (frame == 2500)
		lara_item->mesh_bits = 0;
	else if (frame == 2797)
		lara_item->mesh_bits = -1;
	else if (frame == 2522)
	{
		item = find_a_fucking_item(ANIMATING10);
		item->flags |= IFL_TRIGGERED;
		item->mesh_bits = item->mesh_bits & ~0x18 | 0x10;
	}
	else
	{
		if (frame > 2934)
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

			if (frame >= 3180)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetActorJointAbsPosition(1, 11, &pos);
				DelTorchFlames(&pos);
			}
		}

		if (frame >= 2934 && frame <= 2980)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 1024;
			GetActorJointAbsPosition(1, 11, &pos);

			shift = (frame - 2934) >> 4;
			r = ((GetRandomControl() & 0x3F) + 192) >> shift;
			g = ((GetRandomControl() & 0x1F) + 128) >> shift;
			b = (GetRandomControl() & 0x3F) >> shift;

			if (GetRandomControl() & 1)
				TriggerDynamic(pos.x, pos.y, pos.z, 24, r, g, b);

			TriggerDynamic(pos.x, pos.y, pos.z, 31, r, g, b);
		}
	}

	handle_lara_chatting(lara_chat_ranges_andrea2);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, LARSON, larson_chat_ranges2);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 8, 2, ANIMATING2_MIP, pierre_chat_ranges2);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea2_end()
{
	lara.pickupitems &= ~2;
}

void andrea3_init()
{
	cutseq_givelara_pistols();
}

void andrea3_control()
{
	if (GLOBAL_cutseq_frame == 332)
		lara_item->mesh_bits = 0;
	else if (GLOBAL_cutseq_frame == 421)
		lara_item->mesh_bits = -1;

	deal_with_pistols(andrea3_pistols_info);
	handle_lara_chatting(lara_chat_ranges_andrea3);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, LARSON, larson_chat_ranges3);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea3_end()
{
	ITEM_INFO* item;

	cutseq_removelara_pistols();
	item = find_a_fucking_item(LARSON);
	item->anim_number = objects[LARSON].anim_index;
	item->frame_number = anims[objects[LARSON].anim_index].frame_base;
	item->flags |= IFL_INVISIBLE;
	item->status = ITEM_ACTIVE;
	AddActiveItem(item - items);
	EnableBaddieAI(item - items, 1);
	DelsHandyTeleportLara(55808, 0, 30208, 32768);
}

void andrea3b_init()
{
	ITEM_INFO* item;

	cutseq_givelara_pistols();
	cutseq_kill_item(ANIMATING10);
	cutseq_meshbits[2] = 0x80000F00;
	cutseq_meshbits[3] = 0x80000F00;
	cutseq_meshbits[4] = 0x80000F00;
	item = find_a_fucking_item(LARSON);
	item->status = ITEM_INVISIBLE;
	RemoveActiveItem(item - items);
	DisableBaddieAI(item - items);
	item->flags |= IFL_INVISIBLE;
	disable_horizon = 1;
}

void andrea3b_control()
{
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 301 || f == 639 || f == 781)
		lara_item->mesh_bits = 0;

	if (f == 512 || f == 692 || f == 1084)
		lara_item->mesh_bits = -1;

	if (f == 1191)
	{
		cutseq_meshbits[1] &= ~0x80000000;
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
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
	}

	handle_lara_chatting(lara_chat_ranges_andrea3b);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, LARSON, larson_chat_ranges3b);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andrea3b_end()
{
	ITEM_INFO* item;

	disable_horizon = 0;
	cutseq_removelara_pistols();

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == HYDRA)
		{
			item->status = ITEM_ACTIVE;
			item->flags |= IFL_INVISIBLE;
			AddActiveItem(item - items);
			EnableBaddieAI(item - items, 1);
		}
	}

	DelsHandyTeleportLara(56846, 0, 26986, 50176);
}

void andrea4_init()
{

}

void andrea4_control()
{
	if (GLOBAL_cutseq_frame == 3134)
	{
		cutseq_givelara_pistols();
		undraw_pistol_mesh_left(WEAPON_PISTOLS);
	}
	else if (GLOBAL_cutseq_frame == 3169)
		cutseq_givelara_pistols();
	else if (GLOBAL_cutseq_frame == 3173)
		do_pierre_gun_meshswap();

	handle_lara_chatting(lara_chat_ranges_andrea4);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 8, 1, PIERRE, pierre_chat_ranges4);
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
	meshes[objects[ANIMATING4].mesh_index + 26] = meshes[objects[BLUE_GUARD].mesh_index + 26];
	cutseq_meshbits[5] &= ~0x80000000;
}

void cranecut_control()
{
	PHD_VECTOR pos;
	long f;

	f = GLOBAL_cutseq_frame;

	switch (f)
	{
	case 74:
		lara_item->mesh_bits = 0;
		break;

	case 124:
		lara_item->mesh_bits = -1;
		break;

	case 801:
		cutseq_meshbits[1] &= ~0x80000000;
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

		for (int i = 0; i < 2800; i += 175)
			TriggerDelSmoke(pos.x, pos.y, pos.z + i, f - 1367);
	}
}

void cranecut_end()
{
	ITEM_INFO* item;
	
	item = cutseq_restore_item(ANIMATING5);
	RemoveActiveItem(item - items);
	item->flags &= ~IFL_CODEBITS;
	cutseq_restore_item(ANIMATING16);
	cutseq_restore_item(WRECKING_BALL);
	DelsHandyTeleportLara(58543, -4096, 34972, 49152);
}

void richcut1_init()
{
	ITEM_INFO* item;
	short item_num;

	item_num = room[lara_item->room_number].item_number;

	while (item_num != NO_ITEM)
	{
		item = &items[item_num];

		if (item->object_number == SCIENTIST)
		{
			item->status = ITEM_INVISIBLE;
			RemoveActiveItem(item - items);
			DisableBaddieAI(item - items);
			item->flags |= IFL_INVISIBLE;
		}

		item_num = item->next_item;
	}

	cutseq_givelara_hk();
	cutrot = 1;
}

void richcut1_control()
{
	ITEM_INFO* item;
	long f; 
	
	f = GLOBAL_cutseq_frame;

	if (f == 101 || f == 493 || f == 673)
		lara_item->mesh_bits = 0;

	if (f == 208 || f == 580 || f == 810)
		lara_item->mesh_bits = -1;

	if (f == 840)
	{
		item = find_a_fucking_item(CLOSED_DOOR2);
		AddActiveItem(item - items);
		item->status = ITEM_INVISIBLE;
		item->flags |= IFL_CODEBITS;
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
}

void richcut2_control()
{
	if (GLOBAL_cutseq_frame == 300)
		cutseq_meshbits[5] &= ~0x80000000;

	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 11, 1, ANIMATING6, voncroy_chat_ranges_richcut2);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 18, 3, ANIMATING4, associate_chat_ranges_richcut2);
	handle_actor_chatting(ANIMATING12, 14, 2, ANIMATING1, guard_chat_ranges_richcut2);
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
	cutseq_meshbits[1] &= ~0x80000000;
}

void richcut3_control()
{
	if (GLOBAL_cutseq_frame == 320)
	{
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_removelara_hk();
		lara.back_gun = 0;
		lara.hk_type_carried = W_NONE;
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
		item->flags |= IFL_TRIGGERED;
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
	long f; 
	
	f = GLOBAL_cutseq_frame;

	if (f >= 300 && f <= 312)
	{
		item = find_a_fucking_item(SAILOR_MIP);

		if (!(f & 1))
			item->mesh_bits &= ~(1 << (8 - ((f - 300) >> 1)));

		if (f == 308)
			cutseq_meshbits[2] |= 4;
	}

	
	if (f == 534)
	{
		cutseq_meshswapbits[1] &= ~0x100;
		cutseq_meshbits[2] &= ~0x20;
	}
	else if (f == 557)
	{
		cutseq_meshswapbits[1] &= ~0x20;
		cutseq_meshbits[2] &= ~4;
	}
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
		cutseq_meshbits[1] &= ~0x80000000;
		cutseq_restore_item(ANIMATING2);
		FlashFadeR = 255;
		FlashFadeG = 255;
		FlashFadeB = 255;
		FlashFader = 32;
	}

	handle_lara_chatting(lara_chat_ranges_andy1);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 21, 1, SNIPER_MIP, hanged_man_chat_ranges_andy1);
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
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 1, SAILOR_MIP, priest_chat_ranges_andy2);
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

	if (GLOBAL_cutseq_frame == 733)
	{
		for (int i = 0; i < 8; i++)
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

			GetActorJointAbsPosition(3, 0, &pos);	//fix me (original bug)
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
	long f, actor;

	f = GLOBAL_cutseq_frame;

	if (f == 510 || f == 662)
		do_clanger_meshswap();

	if (f == 697 || f == 708 || f == 723 || f == 735)
	{
		if (f == 708)
			actor = 2;
		else if (f == 697)
			actor = 3;
		else if (f == 735)
			actor = 4;
		else
			actor = 1;

		for (int i = 0; i < 16; i++)
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
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f >= 310)
	{
		if (f < 350)
		{
			if (cut_seethrough > 32)
				cut_seethrough -= 4;
		}
		else if (f <= 450)
		{
			cut_seethrough += 8;

			if (cut_seethrough > 128)
				cut_seethrough = 128;
		}
	}

	if (f == 451)
		cut_seethrough = 128;
	else if (f >= 1100)
	{
		cut_seethrough -= 4;

		if (cut_seethrough < 0)
			cut_seethrough = 0;
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetActorJointAbsPosition(1, 0, &pos);

	if (f >= 1100)
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
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 1, SAILOR_MIP, priest_chat_ranges_andy6);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy6_end()
{

}

void andy7_init()
{
	cutrot = 1;
}

void andy7_control()
{
	handle_lara_chatting(lara_chat_ranges_andy7);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 1, SAILOR_MIP, priest_chat_ranges_andy7);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy7_end()
{
	DelsHandyTeleportLara(78336, 5503, 41658, 0);
}

void andy8_init()
{

}

void andy8_control()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING16);
	item->flags |= IFL_TRIGGERED;

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
	lara.puzzleitems[0] = 0;
	do_chalk_meshswap();
}

void andy9_init()
{
	cutseq_kill_item(ANIMATING15);
	lara_item->mesh_bits = 0;
}

void andy9_control()
{
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 151 || f == 1033 || f == 3868)
		lara_item->mesh_bits = -1;

	if (f == 192 || f == 1099 || f == 3974)
		lara_item->mesh_bits = 0;

	handle_lara_chatting(lara_chat_ranges_andy9);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 4, SAILOR_MIP, priest_chat_ranges_andy9);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 15, 1, SWAT_MIP, knight_chat_ranges_andy9);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void andy9_end()
{
	cutseq_restore_item(ANIMATING15);
	lara_item->mesh_bits = -1;
}

void andy10_init()
{
	ITEM_INFO* item;

	cutseq_kill_item(ANIMATING4);
	cutseq_kill_item(ANIMATING5);
	cutseq_kill_item(ANIMATING14);
	cutseq_meshbits[1] &= ~0x80000000;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[5] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
	item = find_a_fucking_item(GREEN_TEETH);
	item->status = ITEM_INVISIBLE;
	RemoveActiveItem(item - items);
	DisableBaddieAI(item - items);
	item->flags |= IFL_INVISIBLE;
	lara.water_status = LW_UNDERWATER;
	FlipMap(7);
	disable_horizon = 1;
}

void andy10_control()
{
	long f;

	f = GLOBAL_cutseq_frame;

	if (f == 1250)
		cutseq_meshbits[8] &= ~0x1F;

	if (f == 902)
	{
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[5] |= 0x80000000;
		cutseq_meshbits[6] |= 0x80000000;
	}
	
	if (f == 338 || f == 775)
		lara_item->mesh_bits = 0;
	
	if (f == 747)
		lara_item->mesh_bits = -1;
	
	if (f == 902 || f == 1574)
		cutseq_meshbits[4] &= ~0x80000000;

	if (f == 338 || f == 1030)
		cutseq_meshbits[4] |= 0x80000000;
	
	if (f == 1603)
		cutseq_meshbits[1] &= ~0x80000000;
	
	if (f == 1624)
		cutseq_meshbits[2] &= ~0x80000000;
	
	if (f == 854)
		cutseq_meshbits[7] &= ~0x80000000;

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
	lara_item->anim_number = ANIM_TREAD;
	lara_item->frame_number = anims[ANIM_TREAD].frame_base;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->goal_anim_state = AS_TREAD;
	NailInvItem(PUZZLE_ITEM2);
	disable_horizon = 0;
}

void andy11_init()
{
	cutseq_kill_item(ANIMATING15);
	lara_item->mesh_bits = 0;
	cutseq_meshbits[4] &= ~0xF8000;
	cutseq_meshbits[5] &= ~0x80000000;
}

void andy11_control()
{
	long f;

	f = GLOBAL_cutseq_frame;

	if (f == 2112 || f == 2660 || f == 3082 || f == 3626 || f == 4002 || f == 4064 || f == 4118 || f == 4366 || f == 4789 || f == 5390)
		lara_item->mesh_bits = -1;

	if (f == 2164 || f == 2863 || f == 3534 || f == 3915 || f == 4064 || f == 4158 || f == 4569 || f == 5076)
		lara_item->mesh_bits = 0;

	if (f == 3082)
		cutseq_meshbits[5] |= 0x80000000;
	else if (f == 2660)
		cutseq_meshbits[2] &= ~0x80000000;

	handle_lara_chatting(lara_chat_ranges_andy11);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 4, SAILOR_MIP, priest_chat_ranges_andy11);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 15, 1, SWAT_MIP, knight_chat_ranges_andy11);
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

}

void andypew_end()
{
	cutseq_restore_item(PUZZLE_HOLE2);
	DelsHandyTeleportLara(45556, 7680, 23415, -32768);
}

void cossack_init()
{

}

void cossack_control()
{
	PHD_VECTOR pos;
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 98 || f == 344 || f == 743 || f == 1263)
		lara_item->mesh_bits = 0;

	if (f == 281 || f == 468 || f == 901 || f == 1339)
		lara_item->mesh_bits = -1;

	if (f == 156)
	{
		pos.x = 0;
		pos.y = 120;
		pos.z = 0;
		GetActorJointAbsPosition(1, 17, &pos);

		for (int i = 0; i < 7; i++)
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
	}
	else if (f == 157)
	{
		pos.x = 0;
		pos.y = 120;
		pos.z = 0;
		GetActorJointAbsPosition(1, 13, &pos);

		for (int i = 0; i < 7; i++)
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
	}
	else
	{
		if (f >= 581 && f <= 620)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, LMX_HAND_R);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);

			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, LMX_HAND_L);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
		}

		if (f >= 1110 && f <= 1125)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, LMX_HAND_R);
			TriggerDelBrownSmoke(pos.x, pos.y, pos.z);
		}

		if (f >= 1111 && f <= 1126)
		{
			pos.z = 0;
			pos.y = 0;
			pos.x = 0;
			GetLaraJointPos(&pos, LMX_THIGH_R);
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
	long f;

	f = GLOBAL_cutseq_frame;

	if (f == 70)
		lara_item->mesh_bits = 0;

	if (f >= 630 && f < 720)
	{
		if (cut_seethrough > 32)
			cut_seethrough -= 4;
	}

	if (f == 740)
		cut_seethrough = 128;

	if (f >= 940)
	{
		if (cut_seethrough > 0)
			cut_seethrough -= 4;

		if (cut_seethrough < 0)
			cut_seethrough = 0;
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetActorJointAbsPosition(1, 0, &pos);
	TriggerDynamic(pos.x, pos.y, pos.z, 10, GetRandomControl() & 0xF, (GetRandomControl() & 0x1F) + 16, (GetRandomControl() & 0x3F) + 128);
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
		item->flags |= IFL_INVISIBLE;
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
	lara_item->anim_number = ANIM_TREAD;
	lara_item->frame_number = anims[ANIM_TREAD].frame_base;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->goal_anim_state = AS_TREAD;

	if (lara.air > 200)
		lara.air = 200;
}

void hamgate_init()
{
	ITEM_INFO* item; 
	
	item = find_a_fucking_item(DOOR_TYPE1);
	AddActiveItem(item - items);
	item->flags |= IFL_CODEBITS;
	item->mesh_bits = 3;
	item->status = ITEM_INVISIBLE;
	cutseq_meshbits[2] &= ~2;
	cutrot = 0;
}

void hamgate_control()
{
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 14 || f == 269)
		do_hammer_meshswap();

	if (f == 442)
	{
		cutseq_meshbits[3] &= ~0x80000000;
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
	ITEM_INFO* item;
	
	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == CHEF || item->object_number == SAS || item->object_number == BLUE_GUARD ||
			item->object_number == SWAT_PLUS || item->object_number == SWAT || item->object_number == TWOGUN)
		{
			if (abs(item->pos.x_pos - lara_item->pos.x_pos) < 1024 &&
				abs(item->pos.z_pos - lara_item->pos.z_pos) < 1024 &&
				abs(item->pos.y_pos - lara_item->pos.y_pos) < 256)
			{
				GLOBAL_cutme->actor_data[1].objslot = item->object_number;
				item->status = ITEM_INVISIBLE;
				RemoveActiveItem(i);
				DisableBaddieAI(i);
				item->flags |= IFL_INVISIBLE;
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

		if (item->object_number == CHEF || item->object_number == SAS || item->object_number == BLUE_GUARD ||
			item->object_number == SWAT_PLUS || item->object_number == SWAT || item->object_number == TWOGUN)
		{
			if (abs(item->pos.x_pos - lara_item->pos.x_pos) < 1024 &&
				abs(item->pos.z_pos - lara_item->pos.z_pos) < 1024 &&
				abs(item->pos.y_pos - lara_item->pos.y_pos) < 256)
			{
				item->hit_points = 0;
				item->current_anim_state = 6;

				if (item->object_number == TWOGUN)
				{
					item->anim_number = objects[TWOGUN].anim_index + 3;
					item->current_anim_state = 7;
				}
				else if (item->object_number == CHEF)
					item->anim_number = objects[CHEF].anim_index + 11;
				else if (objects[SWAT].loaded)
					item->anim_number = objects[SWAT].anim_index + 11;
				else
					item->anim_number = objects[BLUE_GUARD].anim_index + 11;

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
	cutseq_meshbits[1] &= ~0x80000000;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
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
		cutseq_meshbits[1] &= ~0x80000000;
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
		cutseq_meshbits[5] &= ~0x80000000;
		cutseq_meshbits[6] &= ~0x80000000;
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

	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 3, ANIMATING6_MIP, admiral_chat_ranges_joby2);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 3, 4, ANIMATING9_MIP, sergie_chat_ranges_joby2);
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

}

void joby4_init()
{
	cutseq_kill_item(DOOR_TYPE1);
	cutseq_kill_item(DOOR_TYPE5);
	cutrot = 0;
}

void joby4_control()
{
	long f;

	f = GLOBAL_cutseq_frame;

	if (f == 575)
		cutseq_meshbits[5] &= ~0x80000000;

	if (f == 769 || f == 1966)
		lara_item->mesh_bits = 0;

	if (f == 1593)
		lara_item->mesh_bits = -1;

	handle_lara_chatting(lara_chat_ranges_joby4);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 3, CRANE_GUY_MIP, admiral_chat_ranges_joby4);
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
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
}

void joby5_control()
{
	long f;
	
	f = GLOBAL_cutseq_frame;

	if (f == 144)
		lara_item->mesh_bits = 0;
	else if (f == 436)
		cutseq_meshbits[2] |= 0x80000000;
	else if (f == 1796)
		cutseq_meshbits[4] &= ~0x80000000;
	else if (f == 2175)
		cutseq_meshbits[1] &= ~0x80000000;
	else if (f == 151 || f == 2330)
		cutseq_meshbits[4] |= 0x80000000;

	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 3, CRANE_GUY_MIP, admiral_chat_ranges_joby5);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 3, 4, LION_MIP, sergie_chat_ranges_joby5);
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
	cutseq_meshbits[6] &= ~0x80000000;
}

void joby7_control()
{
	PHD_VECTOR s;
	PHD_VECTOR d;
	long f, b;
	short room_no;

	f = GLOBAL_cutseq_frame;

	if (f == 750)
		cutseq_meshbits[6] |= 0x80000000;
	
	if (f >= 650 && f <= 1050)
	{
		s.x = 0;
		s.y = -100;
		s.z = 0;
		GetActorJointAbsPosition(6, 0, &s);

		b = f - 650;

		if (f - 650 > 220)
			b = 220;

		TriggerDynamic(s.x, s.y, s.z, ((f - 650) >> 5) + 8, 0, 0, b + 16);
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

	handle_actor_chatting(LARA_SPEECH_HEAD1, 14, 1, CROW_MIP, lara_chat_ranges_joby7);
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
	lara_item->anim_number = ANIM_TREAD;
	lara_item->frame_number = anims[ANIM_TREAD].frame_base;
	lara_item->current_anim_state = AS_TREAD;
	lara_item->goal_anim_state = AS_TREAD;
	lara.Anxiety = 80;
}

void joby8_init()
{
	FLOOR_INFO* floor;
	short room_number;

	cutseq_kill_item(DOOR_TYPE4);
	cutseq_kill_item(ANIMATING16);
	lara_item->mesh_bits = 0x81FF01FF;

	room_number = lara_item->room_number;
	floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
	GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	TestTriggers(trigger_index, 1, 0);

	cutseq_meshswapbits[7] = 128;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutrot = 0;
}

void joby8_control()
{
	SPARKS* sptr;
	FLOOR_INFO* floor;
	PHD_VECTOR s;
	PHD_VECTOR d;
	long r, g, b, scale;
	short room_num;

	switch (GLOBAL_cutseq_frame)
	{
	case 147:
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		break;

	case 2440:
		cutseq_meshbits[8] &= ~0x80000000;
		break;

	case 2681:
		cutseq_meshbits[1] &= ~0x80000000;
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
		cutseq_meshbits[5] &= ~0x80000000;
		break;

	case 440:
		lara_item->mesh_bits = -1;
		break;

	case 2103:
		cutseq_meshswapbits[7] = 9216;
		break;

	case 2724:
		room_num = lara_item->room_number;
		floor = GetFloor(lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_num);
		GetHeight(floor, lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos);
		TestTriggers(trigger_index, 1, 0);
		break;
	}

	if (GLOBAL_cutseq_frame >= 2440 && GLOBAL_cutseq_frame <= 2659)
	{
		r = (GetRandomControl() & 0x3F) + 96;
		g = GetRandomControl() % r;
		b = GetRandomControl() & 0xF;

		for (int i = 0; i < 3; i++)
		{
			s.x = 0;
			s.y = 0;
			s.z = 0;
			GetActorJointAbsPosition(1, GetRandomControl() % 20, &s);

			sptr = &spark[GetFreeSpark()];
			sptr->On = 1;
			sptr->sR = (GetRandomControl() & 0x3F) + 192;
			sptr->sG = sptr->sR;
			sptr->sB = sptr->sR;
			sptr->dR = (uchar)r;
			sptr->dG = (uchar)g;
			sptr->dB = (uchar)b;
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
			sptr->Flags = SF_NONE;
			TriggerFireFlame(s.x, s.y, s.z, -1, 254);
		}

		r = (GetRandomControl() & 0x7FFF) << 1;
		d.x = s.x + (phd_sin(r) >> 5);
		d.y = s.y;
		d.z = s.z + (phd_cos(r) >> 5);
		TriggerLightning(&s, &d, (GetRandomControl() & 0xF) + 30, RGBA(r, g, b, 16), 15, 40, 5);
		TriggerDynamic(s.x, s.y, s.z, 10, (GetRandomControl() & 0x7F) + 128, g, b);
	}
	
	if (GLOBAL_cutseq_frame >= 2681 && GLOBAL_cutseq_frame <= 2724)
	{
		s.x = 512;
		s.y = 0;
		s.z = 0;
		GetLaraJointPos(&s, LMX_HIPS);
		scale = (GLOBAL_cutseq_frame - 2681) >> 4;
		r = ((GetRandomControl() & 0x3F) + 192) >> scale;
		g = ((GetRandomControl() & 0x1F) + 128) >> scale;
		b = (GetRandomControl() & 0x3F) >> scale;

		if (GetRandomControl() & 1)
			TriggerDynamic(s.x, s.y, s.z, 24, r, g, b);

		TriggerDynamic(s.x, s.y, s.z, 31, r, g, b);
	}

	handle_lara_chatting(lara_chat_ranges_joby8);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 3, 1, LION_MIP, actor_chat_ranges_joby8);
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
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 1, CRANE_GUY_MIP, admiral_chat_ranges_joby9);
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
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 2, 1, CRANE_GUY_MIP, admiral_chat_ranges_joby10);
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
	lara_item->mesh_bits = -1;
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
	resetwindowsmash(MAFIA_MIP);
	resetwindowsmash(MAFIA2_MIP);
	resetwindowsmash(SAILOR_MIP);
	resetwindowsmash(CRANE_GUY_MIP);
	resetwindowsmash(LION_MIP);
	resetwindowsmash(GLADIATOR_MIP);
	ResetCutanimate(ANIMATING16);
	ResetCutanimate(ANIMATING5);
	nSpecialCut = 2;
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
	nSpecialCut = 3;
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
	nSpecialCut = 4;
}

void special4_control()
{
	_special4_control();
}

void special4_end()
{
	_special4_end();
}

void handle_cutseq_triggering(long name)
{
	long n, goin;

	if (!cutseq_num)
		return;

	if (!cutseq_trig)
	{
		if (lara.gun_type == WEAPON_FLARE || lara.gun_status != LG_NO_ARMS && lara.gun_status != LG_HANDS_BUSY)
			lara.gun_status = LG_UNDRAW_GUNS;

		memset(cutseq_meshswapbits, 0, sizeof(cutseq_meshswapbits));
		memset(cutseq_meshbits, -1, sizeof(cutseq_meshbits));
		cutseq_busy_timeout = 50;
		cutseq_trig = 1;

		if (gfCurrentLevel != LVL5_TITLE)
			SetFadeClip(28, 1);

		if (!ScreenFadedOut)
		{
			if (gfCurrentLevel != LVL5_TITLE)
				S_CDFade(0);

			SetScreenFadeOut(16, 0);
		}

	}
	else if (cutseq_trig == 1)
	{
		n = lara_item->current_anim_state;

		if (!ScreenFadedOut)
			return;

		goin = 0;
		cutseq_busy_timeout--;

		if (cutseq_busy_timeout <= 0)
		{
			cutseq_busy_timeout = 0;
			goin = 1;
		}

		if (lara.gun_status == LG_HANDS_BUSY || (lara.gun_status == LG_NO_ARMS && !lara.flare_control_left) ||
			n == AS_ALL4S || n == AS_CRAWL || n == AS_ALL4TURNL || n == AS_ALL4TURNR || n == AS_CRAWLBACK)
			goin = 1;

		if (goin)
		{
			lara.flare_control_left = 0;
			lara.flare_age = 0;

			if (!(gfLevelFlags & GF_YOUNGLARA))
			{
				lara.gun_type = WEAPON_NONE;
				lara.request_gun_type = WEAPON_NONE;
				lara.gun_status = LG_NO_ARMS;
				old_lara_LHolster = LHolster;
				LHolster = lara.pistols_type_carried == W_NONE ? LARA_HOLSTERS : LARA_HOLSTERS_PISTOLS;
				old_lara_holster = lara.holster;
				lara.holster = LHolster;
				lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + (2 * LM_LHAND)];
				lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + (2 * LM_RHAND)];
				lara.left_arm.frame_number = 0;
				lara.right_arm.frame_number = 0;
				lara.target = 0;
				lara.right_arm.lock = 0;
				lara.left_arm.lock = 0;
				lara_item->goal_anim_state = AS_STOP;
				lara_item->current_anim_state = AS_STOP;
				lara_item->frame_number = anims[ANIM_STOP].frame_base;
				lara_item->anim_number = ANIM_STOP;
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
	}
	else if (cutseq_trig == 3)
	{
		SetScreenFadeOut(16, 1);

		if (gfCurrentLevel != LVL5_TITLE)
			S_CDFade(0);

		cutseq_trig = 4;
	}
	else if (cutseq_trig == 4)
	{
		if (!ScreenFadedOut)
			return;

		if (gfCurrentLevel != LVL5_TITLE)
			S_CDStop();

		ScreenFadedOut = 0;
		numnailed = 0;

		if (cutseq_control_routines[cutseq_num].end_func)
			cutseq_control_routines[cutseq_num].end_func();

		LHolster = old_lara_LHolster;
		lara.holster = old_lara_holster;

		if (cutseq_num <= 4)
			DelsHandyTeleportLara(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, cutrot * 0x4000);

		cutseq_trig = 0;
		GLOBAL_playing_cutseq = 0;

		if (bDoCredits)
		{
			switch (cutseq_num)
			{
			case 28:
				cutseq_num = 29;
				break;

			case 29:
				cutseq_num = 30;
				break;

			case 30:
				cutseq_num = 28;
				break;
			}

			Load_and_Init_Cutseq(cutseq_num);
			cutseq_trig = 2;
			return;
		}

		if (dels_cutseq_player ||
			cutseq_num == 26 || cutseq_num == 22 || cutseq_num == 5 || cutseq_num == 16 || cutseq_num == 33 || cutseq_num == 44 || cutseq_num == 12)
		{
			if (dels_cutseq_player)
			{
				reset_flag = 1;
				dels_cutseq_player = 0;
			}
			else
				gfLevelComplete = gfCurrentLevel + 1;

			gfRequiredStartPos = 0;
			cutseq_num = 0;
			GLOBAL_playing_cutseq = 0;
			cutseq_trig = 0;
			AlterFOV(GAME_FOV);
			ScreenFade = 0;
			dScreenFade = 0;
			ScreenFadeSpeed = 8;
			ScreenFadeBack = 0;
			ScreenFadedOut = 0;
			ScreenFading = 0;
			return;
		}

		finish_cutseq(name);
		cutseq_num = 0;
		camera.type = GLOBAL_oldcamtype;

		if (gfCurrentLevel != LVL5_TITLE)
			SetFadeClip(0, 1);

		AlterFOV(GAME_FOV);

		if (gfCurrentLevel != LVL5_TITLE)
			S_CDPlay(CurrentAtmosphere, 1);

		IsAtmospherePlaying = 1;
	}
}

void cutseq_givelara_pistols()
{
	LHolster = LARA_HOLSTERS;
	lara.holster = LARA_HOLSTERS;
	draw_pistol_meshes(WEAPON_PISTOLS);
}

void cutseq_removelara_pistols()
{
	undraw_pistol_mesh_left(WEAPON_PISTOLS);
	undraw_pistol_mesh_right(WEAPON_PISTOLS);
}

void do_pierre_gun_meshswap()
{
	meshes[objects[PIERRE].mesh_index + 23] = meshes[objects[MESHSWAP2].mesh_index + 22];
	cutseq_meshswapbits[1] |= 0x800;
}

void do_catapult_meshswap()
{
	short* temp;
	
	temp = lara.mesh_ptrs[LM_LHAND];
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[CROW_MIP].mesh_index + (2 * LM_LHAND)];
	meshes[objects[CROW_MIP].mesh_index + (2 * LM_LHAND)] = temp;
}

void do_clanger_meshswap()
{
	short* temp;
	
	temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP2].mesh_index + (2 * LM_RHAND)];
	meshes[objects[MESHSWAP2].mesh_index + (2 * LM_RHAND)] = temp;
}

void do_hammer_meshswap()
{
	short* temp;
	
	temp = lara.mesh_ptrs[LM_RHAND];
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
	short* temp;
	
	temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MAFIA_MIP].mesh_index + 2 * LM_RHAND];
	meshes[objects[MAFIA_MIP].mesh_index + 2 * LM_RHAND] = temp;
}

void cutseq_shoot_pistols(long left_or_right)
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

ITEM_INFO* ResetCutanimate(long objnum)
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(objnum);
	item->anim_number = objects[objnum].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	RemoveActiveItem(item - items);
	item->status = ITEM_INACTIVE;
	item->flags &= ~IFL_CODEBITS;
	return item;
}

void Cutanimate(long objnum)
{
	ITEM_INFO* item;
	
	item = find_a_fucking_item(objnum);
	item->anim_number = objects[objnum].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	AddActiveItem(item - items);
	item->status = ITEM_ACTIVE;
	item->flags |= IFL_CODEBITS;
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

void TriggerActorBlood(long actornum, ulong nodenum, PHD_VECTOR* pos, long direction, long speed)
{
	GetActorJointAbsPosition(actornum, nodenum, pos);
	TriggerBlood(pos->x, pos->y, pos->z, direction >> 4, speed);
}

void finish_cutseq(long name)
{
	GLOBAL_playing_cutseq = 0;
	InitialiseHair();
}

void trigger_weapon_dynamics(long left_or_right)
{
	PHD_VECTOR pos;

	pos.x = (GetRandomControl() & 0xFF) - 128;
	pos.y = (GetRandomControl() & 0x7F) - 63;
	pos.z = (GetRandomControl() & 0xFF) - 128;
	GetLaraJointPos(&pos, left_or_right);
	TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, (GetRandomControl() & 0x3F));
}

ITEM_INFO* find_a_fucking_item(long object_number)
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

void TriggerDelSmoke(long x, long y, long z, long sizeme)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

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
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Friction = 2;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Scalar = 2;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = uchar(sizeme + (GetRandomControl() & 0x1F));
	sptr->Size = sptr->dSize >> 2;
	sptr->sSize = sptr->Size;
}

void TriggerDelBrownSmoke(long x, long y, long z)
{
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
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x7F) + x - 63;
	sptr->y = (GetRandomControl() & 0x7F) + y - 63;
	sptr->z = (GetRandomControl() & 0x7F) + z - 63;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Friction = 2;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Scalar = 2;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x1F) + 40;
	sptr->Size = sptr->dSize >> 2;
	sptr->sSize = sptr->Size;
}

void DelTorchFlames(PHD_VECTOR* pos)
{
	SPARKS* sptr;

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
	sptr->sLife = sptr->Life;
	sptr->x = pos->x;
	sptr->y = pos->y;
	sptr->z = pos->z;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 51;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
	sptr->Scalar = 2;
	sptr->Size = (GetRandomControl() & 0xF) + 16;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;

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
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x3F) + pos->x - 32;
	sptr->y = (GetRandomControl() & 0x3F) + pos->y - 32;
	sptr->z = (GetRandomControl() & 0x3F) + pos->z - 32;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;
	sptr->Flags = SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->Scalar = 2;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -22;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->dSize = (GetRandomControl() & 0xF) + 16;
	sptr->Size = sptr->dSize >> 1;
	sptr->sSize = sptr->Size;
	sptr->dSize += sptr->dSize >> 2;
}

void trigger_title_spotcam(long num)
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

		S_CDPlay(83, 1);
		break;

	case 2:
		S_CDPlay(86, 1);
		break;

	case 3:
		S_CDPlay(87, 1);
		break;

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
		S_CDPlay(91, 1);
		break;
	}

	InitialiseSpotCam((short)num);
}

void CutLaraBubbles()
{
	PHD_VECTOR offset;
	PHD_VECTOR pos;
	short roomnum;

	offset.x = 0;
	offset.y = -4;
	offset.z = 64;
	GetLaraJointPos(&offset, LMX_HEAD);

	roomnum = camera.pos.room_number;
	IsRoomOutsideNo = -1;
	IsRoomOutside(offset.x, offset.y, offset.z);

	if (IsRoomOutsideNo != -1)
		roomnum = IsRoomOutsideNo;

	for (int i = 2; i > 0; i--)
	{
		pos.x = (GetRandomControl() & 0x3F) + offset.x - 32;
		pos.y = (GetRandomControl() & 0x3F) + offset.y - 32;
		pos.z = (GetRandomControl() & 0x3F) + offset.z - 32;
		CreateBubble((PHD_3DPOS*)&pos, roomnum, 7, 8, 0, 0, -96, 0);
	}
}

void deal_with_pistols(ushort* shootdata)
{
	PHD_VECTOR pos;
	long f;
	ushort dat;

	f = GLOBAL_cutseq_frame;

	while (1)
	{
		dat = *shootdata++;

		if (dat == 0xFFFF)
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
		lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA_SCREAM].mesh_index + LM_HEAD * 2];

		if (SmokeCountL)
		{
			pos.x = 4;
			pos.y = 128;
			pos.z = 40;
			GetLaraJointPos(&pos, LMX_HAND_L);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
		}

		if (SmokeCountR)
		{
			pos.x = -16;
			pos.y = 128;
			pos.z = 40;
			GetLaraJointPos(&pos, LMX_HAND_R);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
		}
	}
	else
		lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA].mesh_index + LM_HEAD * 2];

	if (lara.left_arm.flash_gun)
	{
		lara.left_arm.flash_gun--;
		trigger_weapon_dynamics(LMX_HAND_L);
	}

	if (lara.right_arm.flash_gun)
	{
		lara.right_arm.flash_gun--;
		trigger_weapon_dynamics(LMX_HAND_R);
	}
}

void handle_lara_chatting(short* _ranges)
{
	long r1, r2, f;

	f = GLOBAL_cutseq_frame;
	lara_chat_cnt = (lara_chat_cnt - 1) & 1;

	while (1)
	{
		r1 = _ranges[0];
		r2 = _ranges[1];

		if (r1 == -1)
		{
			lara.mesh_ptrs[14] = meshes[objects[LARA_SKIN].mesh_index + 2 * 14];
			return;
		}

		if (f >= r1 && f <= r2)
			break;

		_ranges += 2;
	}

	if (!lara_chat_cnt)
		lara.mesh_ptrs[LM_HEAD] = meshes[objects[(GetRandomControl() & 3) + LARA_SPEECH_HEAD1].mesh_index + LM_HEAD * 2];
}

void handle_actor_chatting(long speechslot, long node, long slot, long objslot, short* _ranges)
{
	long r1, r2, f, rnd;

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

void trigger_item_in_room(short room_number, long object)
{
	ITEM_INFO* item;
	short num;

	num = room[room_number].item_number;

	while (num != NO_ITEM)
	{
		item = &items[num];

		if (item->object_number == object)
		{
			AddActiveItem(num);
			item->status = ITEM_ACTIVE;
			item->flags |= IFL_CODEBITS;
		}

		num = item->next_item;
	}
}

void untrigger_item_in_room(short room_number, long object)
{
	ITEM_INFO* item;
	short num;

	num = room[room_number].item_number;

	while (num != NO_ITEM)
	{
		item = &items[num];

		if (item->object_number == object)
		{
			RemoveActiveItem(num);
			item->status = ITEM_DEACTIVATED;
			item->flags |= ~IFL_CODEBITS;
		}

		num = item->next_item;
	}
}

void deal_with_actor_shooting(ushort* shootdata, long actornum, long nodenum, PHD_VECTOR* pos)
{
	long arse[indices_count];
	long f;
	ushort dat;

	dat = *shootdata++;
	f = GLOBAL_cutseq_frame;

	while (dat != 0xFFFF)
	{
		if (f == dat || f == dat + 1)
		{
			GrabActorMatrix(actornum, nodenum, arse);
			trig_actor_gunflash(arse, pos);
			GetActorJointAbsPosition(actornum, nodenum, pos);
			TriggerDynamic(pos->x, pos->y, pos->z, 16, (GetRandomControl() & 0x3F) + 0xC0, (GetRandomControl() & 0x1F) + 0x80, (GetRandomControl() & 0x3F));
			return;
		}

		dat = *shootdata++;
	}
}

void DelsHandyTeleportLara(long x, long y, long z, long yrot)
{
	lara_item->pos.x_pos = x;
	lara_item->pos.y_pos = y;
	lara_item->pos.z_pos = z;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara_item->pos.x_rot = 0;
	lara_item->pos.y_rot = (short)yrot;
	lara_item->pos.z_rot = 0;
	IsRoomOutside(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	if (IsRoomOutsideNo != lara_item->room_number)
		ItemNewRoom(lara.item_number, IsRoomOutsideNo);

	lara_item->anim_number = ANIM_STOP;
	lara_item->frame_number = anims[ANIM_STOP].frame_base;
	lara_item->current_anim_state = AS_STOP;
	lara_item->goal_anim_state = AS_STOP;
	lara_item->speed = 0;
	lara_item->fallspeed = 0;
	lara_item->gravity_status = 0;
	lara.gun_status = LG_NO_ARMS;
	camera.fixed_camera = 1;
}

void InitPackNodes(NODELOADHEADER* lnode, PACKNODE* pnode, char* packed, long nNodes)
{
	long offset, xoff, yoff, zoff;

	offset = nNodes * sizeof(NODELOADHEADER);

	for (int i = 0; i < nNodes; i++)
	{
		pnode[i].xkey = lnode[i].xkey;
		pnode[i].ykey = lnode[i].ykey;
		pnode[i].zkey = lnode[i].zkey;
		pnode[i].decode_x.packmethod = (lnode[i].packmethod >> 10) & 0xF;
		pnode[i].decode_y.packmethod = (lnode[i].packmethod >> 5) & 0xF;
		pnode[i].decode_z.packmethod = lnode[i].packmethod & 0xF;
		pnode[i].xlength = lnode[i].xlength;
		pnode[i].ylength = lnode[i].ylength;
		pnode[i].zlength = lnode[i].zlength;
		xoff = ((lnode[i].xlength * pnode[i].decode_x.packmethod) >> 3) + 4;
		yoff = ((lnode[i].ylength * pnode[i].decode_y.packmethod) >> 3) + 4;
		zoff = ((lnode[i].zlength * pnode[i].decode_z.packmethod) >> 3) + 4;
		pnode[i].xpacked = &packed[offset];
		pnode[i].ypacked = &packed[offset + xoff];
		pnode[i].zpacked = &packed[offset + xoff + yoff];
		
		offset += xoff + yoff + zoff;
	}
}

short GetTrackWord(long off, char* packed, long packmethod)
{
	ulong data;
	long index;

	index = off * packmethod;
	data = *(ulong*)&packed[index >> 3];
	data >>= index & 7;
	data &= (1 << packmethod) - 1;

	if (data & (1 << (packmethod - 1)))
		data |= ~((1 << packmethod) - 1);
	
	return (short)data;
}

short DecodeTrack(char* packed, RTDECODE* code)
{
	short word, word2;

	if (!code->decodetype)
	{
		word = GetTrackWord(code->off, packed, code->packmethod);

		if (word & 0x20)
		{
			code->counter = word & 0xF;

			if (!code->counter)
				code->counter = 16;

			code->decodetype = 1;
			code->off++;
			code->length--;
		}
		else
		{
			code->decodetype = 2;

			if (word & 0x10)
			{
				word2 = GetTrackWord(code->off + 1, packed, code->packmethod);
				code->counter = (word & 7) << 5;
				code->counter |= word2 & 0x1F;
				code->counter &= 0xFF;
				code->data = GetTrackWord(code->off + 2, packed, code->packmethod);
				code->off += 3;
				code->length -= 3;
			}
			else
			{
				code->data = GetTrackWord(code->off + 1, packed, code->packmethod);
				code->counter = word & 0x7;
				code->off += 2;
				code->length -= 2;
			}
		}
	}

	if (code->decodetype == 2)
	{
		code->counter--;

		if (!code->counter)
			code->decodetype = 0;

		return code->data;
	}

	word = GetTrackWord(code->off, packed, code->packmethod);
	code->off++;
	code->length--;
	code->counter--;

	if (!code->counter)
		code->decodetype = 0;

	return word;
}

void DecodeAnim(PACKNODE* nodes, long nNodes, long frame, long limit)
{
	if (!frame)
	{
		for (int i = 0; i < nNodes; i++)
		{
			nodes[i].xrot_run = nodes[i].xkey;
			nodes[i].yrot_run = nodes[i].ykey;
			nodes[i].zrot_run = nodes[i].zkey;
			nodes[i].decode_x.length = nodes[i].xlength;
			nodes[i].decode_y.length = nodes[i].ylength;
			nodes[i].decode_z.length = nodes[i].zlength;
			nodes[i].decode_x.off = 0;
			nodes[i].decode_y.off = 0;
			nodes[i].decode_z.off = 0;
			nodes[i].decode_x.counter = 0;
			nodes[i].decode_y.counter = 0;
			nodes[i].decode_z.counter = 0;
			nodes[i].decode_x.data = 0;
			nodes[i].decode_y.data = 0;
			nodes[i].decode_z.data = 0;
			nodes[i].decode_x.decodetype = 0;
			nodes[i].decode_y.decodetype = 0;
			nodes[i].decode_z.decodetype = 0;
		}
	}
	else
	{
		nodes[0].xrot_run += DecodeTrack(nodes[0].xpacked, &nodes[0].decode_x);
		nodes[0].yrot_run += DecodeTrack(nodes[0].ypacked, &nodes[0].decode_y);
		nodes[0].zrot_run += DecodeTrack(nodes[0].zpacked, &nodes[0].decode_z);

		for (int i = 1; i < nNodes; i++)
		{
			nodes[i].xrot_run += DecodeTrack(nodes[i].xpacked, &nodes[i].decode_x);
			nodes[i].yrot_run += DecodeTrack(nodes[i].ypacked, &nodes[i].decode_y);
			nodes[i].zrot_run += DecodeTrack(nodes[i].zpacked, &nodes[i].decode_z);
			nodes[i].xrot_run &= limit;
			nodes[i].yrot_run &= limit;
			nodes[i].zrot_run &= limit;
		}
	}
}

void do_new_cutscene_camera()
{
	PACKNODE* nodes;

	if (cutseq_control_routines[cutseq_num].control_func)
		cutseq_control_routines[cutseq_num].control_func();

	DecodeAnim(camera_pnodes, 2, GLOBAL_cutseq_frame, 0xFFFF);
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

	if (GLOBAL_cutme->actor_data[0].objslot != NO_ITEM)
		DecodeAnim(actor_pnodes[0], 16, GLOBAL_cutseq_frame, 0x3FF);

	for (int i = 1; i < GLOBAL_cutme->numactors; i++)
		DecodeAnim(actor_pnodes[i], GLOBAL_cutme->actor_data[i].nodes + 1, GLOBAL_cutseq_frame, 0x3FF);

	GLOBAL_cutseq_frame++;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames - 8 && cutseq_trig == 2)
		cutseq_trig = 3;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames)
		GLOBAL_cutseq_frame = GLOBAL_numcutseq_frames;
}

void updateAnimFrame(PACKNODE* nodes, long nNodes, short* frame)
{
	short* nex;
	long rot;
	short x, y, z;

	frame[7] = 3 * nodes[0].yrot_run;

	switch (cutrot)
	{
	case 0:
		frame[6] = 3 * nodes[0].xrot_run;
		frame[8] = 3 * nodes[0].zrot_run;
		break;

	case 1:
		frame[6] = 3 * nodes[0].zrot_run;
		frame[8] = -3 * nodes[0].xrot_run;
		break;

	case 2:
		frame[6] = -3 * nodes[0].xrot_run;
		frame[8] = -3 * nodes[0].zrot_run;
		break;

	case 3:
		frame[6] = -3 * nodes[0].zrot_run;
		frame[8] = 3 * nodes[0].xrot_run;
		break;
	}

	nex = frame + 9;

	for (int i = 1; i < nNodes; i++)
	{
		x = nodes[i].xrot_run;
		y = nodes[i].yrot_run;
		z = nodes[i].zrot_run;

		if (cutrot && i == 1)
			y = (y + (cutrot << 8)) & 0x3FF;

		rot = z | y << 10 | x << 20;

		nex[0] = rot >> 16;
		nex[1] = rot & 0xFFFF;
		nex += 2;
	}
}

void DrawCutSeqActors()
{
	OBJECT_INFO* obj;
	short** mesh;
	long* bone;
	short* rot;
	long n;

	phd_PushMatrix();

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
						phd_PutPolygons(mesh[1], -1);
					else
						phd_PutPolygons_seethrough(mesh[1], cut_seethrough);
				}
				else if (i != 1 || cut_seethrough == 128)
					phd_PutPolygons(mesh[0], -1);
				else
					phd_PutPolygons_seethrough(mesh[0], cut_seethrough);
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
							phd_PutPolygons(mesh[1], -1);
						else
							phd_PutPolygons_seethrough(mesh[1], cut_seethrough);
					}
					else if (i != 1 || cut_seethrough == 128)
						phd_PutPolygons(mesh[0], -1);
					else
						phd_PutPolygons_seethrough(mesh[0], cut_seethrough);
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
	pos->x = phd_mxptr[M03] >> W2V_SHIFT;
	pos->y = phd_mxptr[M13] >> W2V_SHIFT;
	pos->z = phd_mxptr[M23] >> W2V_SHIFT;
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

	if (cutseq_num == 18 && GLOBAL_cutseq_frame <= 130)
		PrintString(phd_centerx, phd_winymax - 3 * font_height, 5, SCRIPT_TEXT(TXT_cack), FF_CENTER);

	lara_item->pos.x_pos = GLOBAL_cutme->orgx;
	lara_item->pos.y_pos = GLOBAL_cutme->orgy;
	lara_item->pos.z_pos = GLOBAL_cutme->orgz;

	if (GLOBAL_cutme->actor_data[0].objslot == NO_ITEM)
		return;

	frame = &temp_rotation_buffer[0];
	object = &objects[lara_item->object_number];
	bone = &bones[object->bone_index];
	updateAnimFrame(actor_pnodes[0], 16, frame);

	if (cutseq_num == 8 && (GLOBAL_cutseq_frame >= 696 && GLOBAL_cutseq_frame <= 841))
	{
		frame[6] = -69;
		frame[8] = 267;
	}

	Rich_CalcLaraMatrices_Normal(frame, bone, 0);
	phd_PushUnitMatrix();
	Rich_CalcLaraMatrices_Normal(frame, bone, 1);
	phd_PopMatrix();

	HairControl(0, 0, frame);

	if ((gfLevelFlags & GF_YOUNGLARA))
		HairControl(0, 1, frame);

	GLaraShadowframe = frig_shadow_bbox;
}

void CalculateObjectLightingLaraCutSeq()
{
	PHD_VECTOR pos;
	short room_num, room_num2;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetLaraJointPos(&pos, LMX_TORSO);
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

void GrabActorMatrix(long actornum, ulong nodenum, long* matrixstash)
{
	OBJECT_INFO* obj;
	long* bone;
	short* rot;
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
		*(float*)&matrixstash[M00] = aMXPtr[M00];
		*(float*)&matrixstash[M01] = aMXPtr[M01];
		*(float*)&matrixstash[M02] = aMXPtr[M02];
		*(float*)&matrixstash[M03] = aMXPtr[M03];
		*(float*)&matrixstash[M10] = aMXPtr[M10];
		*(float*)&matrixstash[M11] = aMXPtr[M11];
		*(float*)&matrixstash[M12] = aMXPtr[M12];
		*(float*)&matrixstash[M13] = aMXPtr[M13];
		*(float*)&matrixstash[M20] = aMXPtr[M20];
		*(float*)&matrixstash[M21] = aMXPtr[M21];
		*(float*)&matrixstash[M22] = aMXPtr[M22];
		*(float*)&matrixstash[M23] = aMXPtr[M23];
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
			*(float*)&matrixstash[M00] = aMXPtr[M00];
			*(float*)&matrixstash[M01] = aMXPtr[M01];
			*(float*)&matrixstash[M02] = aMXPtr[M02];
			*(float*)&matrixstash[M03] = aMXPtr[M03];
			*(float*)&matrixstash[M10] = aMXPtr[M10];
			*(float*)&matrixstash[M11] = aMXPtr[M11];
			*(float*)&matrixstash[M12] = aMXPtr[M12];
			*(float*)&matrixstash[M13] = aMXPtr[M13];
			*(float*)&matrixstash[M20] = aMXPtr[M20];
			*(float*)&matrixstash[M21] = aMXPtr[M21];
			*(float*)&matrixstash[M22] = aMXPtr[M22];
			*(float*)&matrixstash[M23] = aMXPtr[M23];
		}
	}

	phd_PopMatrix();
}

long Load_and_Init_Cutseq(long num)
{
	char* packed;

	SetCutPlayed(num);
	packed = aFetchCutData(num);
	GLOBAL_cutme = (NEW_CUTSCENE*)packed;

	if (cutseq_num <= 4)
	{
		GLOBAL_cutme->orgx = (lara_item->pos.x_pos & ~0x3FF) + 512;
		GLOBAL_cutme->orgy = lara_item->pos.y_pos;
		GLOBAL_cutme->orgz = (lara_item->pos.z_pos & ~0x3FF) + 512;
	}

	init_cutseq_actors(packed, 0);
	return 0;
}

void cutseq_kill_item(long num)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == num)
		{
			old_status_flags[numnailed] = item->status;	
			old_status_flags2[numnailed] = item->flags;
			item->status = ITEM_INVISIBLE;
			item->flags = (item->flags & 0xC1FF) | IFL_TRIGGERED;
			numnailed++;
		}
	}
}

ITEM_INFO* cutseq_restore_item(long num)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == num)
		{
			item->status = old_status_flags[numnailed];
			item->flags = old_status_flags2[numnailed];
			numnailed++;
			return item;
		}
	}

	return 0;
}

void GetActorJointAbsPosition(long actornum, ulong nodenum, PHD_VECTOR* vec)
{
	ITEM_INFO* item;
	long* bone;
	short* frame;
	short* rot;

	item = &duff_item[0];
	bone = &bones[objects[GLOBAL_cutme->actor_data[actornum].objslot].bone_index];
	frame = temp_rotation_buffer;
	phd_PushMatrix();
	updateAnimFrame(actor_pnodes[actornum], GLOBAL_cutme->actor_data[actornum].nodes + 1, frame);
	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);

	for (ulong i = 0; i < nodenum; i++, bone += 4)
	{
		if (*bone & 1)
			phd_PopMatrix();

		if (*bone & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);
	}

	phd_TranslateRel(vec->x, vec->y, vec->z);
	vec->x = phd_mxptr[M03] >> W2V_SHIFT;
	vec->y = phd_mxptr[M13] >> W2V_SHIFT;
	vec->z = phd_mxptr[M23] >> W2V_SHIFT;
	vec->x += item->pos.x_pos;
	vec->y += item->pos.y_pos;
	vec->z += item->pos.z_pos;
	phd_PopMatrix();
	phd_PopMatrix();
}

void init_cutseq_actors(char* data, long resident)
{
	NODELOADHEADER* nlheader;
	char* packed;
	char* resident_addr;
	long pda_nodes, offset;

	resident_addr = GLOBAL_resident_depack_buffers;
	lastcamnum = -1;
	GLOBAL_playing_cutseq = 0;

	if (cutseq_num == 26)				//restores the ending for the Security Breach cut
		GLOBAL_cutme->numframes = 1978;	//original is 1700 (ish)

	if (cutseq_num == 4)				//fixes the wrong audio for this cutscene (see 13th floor twogun chloroform cut)
		GLOBAL_cutme->audio_track = 19;	//original is 40

	GLOBAL_numcutseq_frames = GLOBAL_cutme->numframes;

	for (int i = 0; i < GLOBAL_cutme->numactors; i++)
	{
		offset = GLOBAL_cutme->actor_data[i].offset;
		packed = &data[offset];
		nlheader = (NODELOADHEADER*)packed;
		pda_nodes = GLOBAL_cutme->actor_data[i].nodes;

		if (resident)
		{
			actor_pnodes[i] = (PACKNODE*)resident_addr;
			resident_addr += sizeof(PACKNODE) * (pda_nodes + 1);
		}
		else
			actor_pnodes[i] = (PACKNODE*)game_malloc(sizeof(PACKNODE) * (pda_nodes + 1));

		if (!i)
		{
			if (GLOBAL_cutme->actor_data[i].objslot != NO_ITEM)
				InitPackNodes(nlheader, actor_pnodes[i], packed, pda_nodes + 1);
		}
		else
			InitPackNodes(nlheader, actor_pnodes[i], packed, pda_nodes + 1);

		memset(&duff_item[i], 0, sizeof(ITEM_INFO));
		duff_item[i].il.ambient = lara_item->il.ambient;
		duff_item[i].il.fcnt = -1;
		duff_item[i].il.room_number = -1;
		duff_item[i].il.RoomChange = 0;
		duff_item[i].il.nCurrentLights = 0;
		duff_item[i].il.nPrevLights = 0;
		duff_item[i].il.pCurrentLights = (void*)&duff_item[i].il.CurrentLights[0];
		duff_item[i].il.pPrevLights = (void*)&duff_item[i].il.PrevLights[0];
		duff_item[i].pos.x_pos = GLOBAL_cutme->orgx;
		duff_item[i].pos.y_pos = GLOBAL_cutme->orgy;
		duff_item[i].pos.z_pos = GLOBAL_cutme->orgz;
		duff_item[i].pos.x_rot = 0;
		duff_item[i].pos.y_rot = 0;
		duff_item[i].pos.z_rot = 0;
		duff_item[i].room_number = 0;
	}

	offset = GLOBAL_cutme->camera_offset;
	packed = data + offset;
	nlheader = (NODELOADHEADER*)packed;
	pda_nodes = 1; 		

	if (resident)
	{
		camera_pnodes = (PACKNODE*)resident_addr;
		resident_addr += sizeof(PACKNODE) * (pda_nodes + 1);
	}
	else
		camera_pnodes = (PACKNODE*)game_malloc(sizeof(PACKNODE) * (pda_nodes + 1));

	InitPackNodes(nlheader, camera_pnodes, packed, pda_nodes + 1);
	GLOBAL_playing_cutseq = 1;
	GLOBAL_cutseq_frame = 0;
	DelsHandyTeleportLara(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 0);
	camera.pos.x = lara_item->pos.x_pos;
	camera.pos.y = lara_item->pos.y_pos;
	camera.pos.z = lara_item->pos.z_pos;
	camera.pos.room_number = lara_item->room_number;
	InitialiseHair();
}

void init_resident_cutseq(long num)
{
	char* packed;

	packed = cutseq_resident_addresses[num];
	GLOBAL_cutme = (NEW_CUTSCENE*)packed;

	if (cutseq_num <= 4)
	{
		GLOBAL_cutme->orgx = (lara_item->pos.x_pos & -1024) + 512;
		GLOBAL_cutme->orgy = lara_item->pos.y_pos;
		GLOBAL_cutme->orgz = (lara_item->pos.z_pos & -1024) + 512;
	}

	init_cutseq_actors(packed, 1);
}

void nail_intelligent_object(short objnum)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == objnum)
		{
			item->status = ITEM_INVISIBLE;
			RemoveActiveItem(i);
			DisableBaddieAI(i);
		}
	}
}

void do_cutseq_skipper_stuff()
{
	ITEM_INFO* item;
	short room_num;

	if (gfCurrentLevel == LVL5_TITLE)
		return;

	//a bunch of hardcoded bullshit to avoid softlocking..

	switch (cutseq_num)
	{
	case 14:

		if (GLOBAL_cutseq_frame < 240)
			do_catapult_meshswap();

		break;

	case 17:
		
		if (GLOBAL_cutseq_frame < 330)
		{
			item = find_a_fucking_item(ANIMATING10);
			item->flags |= IFL_TRIGGERED;
			item->mesh_bits = item->mesh_bits & 0xFFFFFFFD | 4;
		}

		break;

	case 20:

		if (GLOBAL_cutseq_frame > 14 && GLOBAL_cutseq_frame < 269)
			do_hammer_meshswap();

		break;

	case 21:

		if (GLOBAL_cutseq_frame < 3750)
			cutseq_restore_item(ANIMATING2);

		break;

	case 23:

		if (GLOBAL_cutseq_frame < 320)
		{
			cutseq_removelara_hk();
			lara.back_gun = 0;
			lara.hk_type_carried = W_NONE;
			lara.last_gun_type = WEAPON_NONE;
		}

		break;

	case 24:

		if (GLOBAL_cutseq_frame < 840)
		{
			item = find_a_fucking_item(CLOSED_DOOR2);
			AddActiveItem(item - items);
			item->status = ITEM_INVISIBLE;
			item->flags |= IFL_CODEBITS;
		}

		break;

	case 27:

		if (GLOBAL_cutseq_frame < 850)
			cutseq_removelara_pistols();

		if (GLOBAL_cutseq_frame < 1301)
			FlipMap(1);

		break;

	case 32:

		if (GLOBAL_cutseq_frame < 2724)
		{
			room_num = lara_item->room_number;
			GetHeight(GetFloor(lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_num), lara_item->pos.x_pos + 1024, lara_item->pos.y_pos, lara_item->pos.z_pos);
			TestTriggers(trigger_index, 1, 0);
		}

		break;

	case 38:

		if (GLOBAL_cutseq_frame < 145)
			do_chalk_meshswap();

		if (GLOBAL_cutseq_frame < 831)
		{
			item = find_a_fucking_item(ANIMATING16);
			item->mesh_bits = 8;
		}

		break;

	default:
		break;
	}

	lara_item->mesh_bits = -1;	//for good fucking measure
	cutseq_trig = 3;	//end it
}
