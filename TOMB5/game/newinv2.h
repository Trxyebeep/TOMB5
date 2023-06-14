#pragma once
#include "../global/types.h"

long S_CallInventory2();
void init_new_inventry();
void do_debounced_joystick_poo();
void DrawThreeDeeObject2D(long x, long y, long num, long shade, long xrot, long yrot, long zrot, long bright, long overlay);
void DrawInventoryItemMe(ITEM_INFO* item, long shade, long overlay, long shagflag);
long go_and_load_game();
long go_and_save_game();
void construct_combine_object_list();
void insert_object_into_list_v2(long num);
void construct_object_list();
void insert_object_into_list(long num);
void draw_current_object_list(long ringnum);
void handle_object_changeover(long ringnum);
void handle_inventry_menu();
void setup_ammo_selector();
void fade_ammo_selector();
void draw_ammo_selector();
void spinback(ushort* cock);
void update_laras_weapons_status();
long is_item_currently_combinable(short obj);
long have_i_got_item(short obj);
long do_these_objects_combine(long obj1, long obj2);
void combine_these_two_objects(short obj1, short obj2);
void seperate_object(short obj);
void combine_HK_SILENCER(long flag);
void combine_revolver_lasersight(long flag);
void combine_crossbow_lasersight(long flag);
void combine_PuzzleItem1(long flag);
void combine_PuzzleItem2(long flag);
void combine_PuzzleItem3(long flag);
void combine_PuzzleItem4(long flag);
void combine_PuzzleItem5(long flag);
void combine_PuzzleItem6(long flag);
void combine_PuzzleItem7(long flag);
void combine_PuzzleItem8(long flag);
void combine_KeyItem1(long flag);
void combine_KeyItem2(long flag);
void combine_KeyItem3(long flag);
void combine_KeyItem4(long flag);
void combine_KeyItem5(long flag);
void combine_KeyItem6(long flag);
void combine_KeyItem7(long flag);
void combine_KeyItem8(long flag);
void combine_PickupItem1(long flag);
void combine_PickupItem2(long flag);
void combine_PickupItem3(long flag);
void combine_PickupItem4(long flag);
void combine_clothbottle(long flag);
void setup_objectlist_startposition(short newobj);
void setup_objectlist_startposition2(short newobj);
void use_current_item();
void DEL_picked_up_object(short objnum);
void NailInvItem(short objnum);
long have_i_got_object(short object_number);
void remove_inventory_item(short object_number);
long convert_obj_to_invobj(short obj);
long convert_invobj_to_obj(long obj);
void init_keypad_mode();
void do_keypad_mode();
void do_examine_mode();
void do_stats_mode();
void dels_give_lara_items_cheat();
void dels_give_lara_guns_cheat();
long LoadGame();
long SaveGame();
void DelDrawSprite(long x, long y, long def, long z);

extern long GLOBAL_enterinventory;
extern long GLOBAL_inventoryitemchosen;
extern long GLOBAL_lastinvitem;
extern long GLOBAL_invkeypadmode;
extern long GLOBAL_invkeypadcombination;
extern long InventoryActive;

enum ring_types
{
	RING_INVENTORY,
	RING_AMMO
};

enum option_types
{
	OPT_UNUSED =		0x1,
	OPT_EQUIP =			0x2,
	OPT_USE =			0x4,
	OPT_COMBINE =		0x8,
	OPT_SEPARATE =		0x10,
	OPT_EXAMINE =		0x20,
	OPT_SHOTGUN =		0x40,
	OPT_CROSSBOW =		0x80,
	OPT_HK =			0x100,
	OPT_UZI =			0x200,
	OPT_PISTOLS =		0x400,
	OPT_REVOLVER =		0x800,
	OPT_LOAD =			0x1000,
	OPT_SAVE =			0x2000,
	OPT_GRAPPLE =		0x4000,
	OPT_STATS =			0x8000
};

enum MENU_TYPES
{
	ITYPE_EMPTY,
	ITYPE_USE,
	ITYPE_CHOOSEAMMO,
	ITYPE_COMBINE,
	ITYPE_SEPERATE,
	ITYPE_EQUIP,
	ITYPE_AMMO1,
	ITYPE_AMMO2,
	ITYPE_AMMO3,
	ITYPE_LOAD,
	ITYPE_SAVE,
	ITYPE_EXAMINE,
	ITYPE_STATS
};

enum invobj_types
{
	INV_UZI_ITEM,
	INV_PISTOLS_ITEM,
	INV_SHOTGUN_ITEM,
	INV_REVOLVER_ITEM1,
	INV_REVOLVER_ITEM2,
	INV_CROSSBOW_AMMO2_ITEM1,
	INV_CROSSBOW_AMMO2_ITEM2,
	INV_HK_ITEM1,
	INV_HK_ITEM2,
	INV_SHOTGUN_AMMO1_ITEM,
	INV_SHOTGUN_AMMO2_ITEM,
	INV_HK_AMMO_ITEM1,
	INV_HK_AMMO_ITEM2,
	INV_HK_AMMO_ITEM3,
	INV_HK_AMMO_ITEM4,
	INV_CROSSBOW_AMMO2_ITEM3,
	INV_CROSSBOW_AMMO2_ITEM4,
	INV_REVOLVER_AMMO_ITEM,
	INV_UZI_AMMO_ITEM,
	INV_PISTOLS_AMMO_ITEM,
	INV_LASERSIGHT_ITEM,
	INV_SILENCER_ITEM,
	INV_BIGMEDI_ITEM,
	INV_SMALLMEDI_ITEM,
	INV_BINOCULARS_ITEM,
	INV_FLARE_INV_ITEM,
	INV_COMPASS_ITEM,
	INV_MEMCARD_LOAD_INV_ITEM,
	INV_MEMCARD_SAVE_INV_ITEM,
	INV_PUZZLE_ITEM1,
	INV_PUZZLE_ITEM2,
	INV_PUZZLE_ITEM3,
	INV_PUZZLE_ITEM4,
	INV_PUZZLE_ITEM5,
	INV_PUZZLE_ITEM6,
	INV_PUZZLE_ITEM7,
	INV_PUZZLE_ITEM8,
	INV_PUZZLE_ITEM1_COMBO1,
	INV_PUZZLE_ITEM1_COMBO2,
	INV_PUZZLE_ITEM2_COMBO1,
	INV_PUZZLE_ITEM2_COMBO2,
	INV_PUZZLE_ITEM3_COMBO1,
	INV_PUZZLE_ITEM3_COMBO2,
	INV_PUZZLE_ITEM4_COMBO1,
	INV_PUZZLE_ITEM4_COMBO2,
	INV_PUZZLE_ITEM5_COMBO1,
	INV_PUZZLE_ITEM5_COMBO2,
	INV_PUZZLE_ITEM6_COMBO1,
	INV_PUZZLE_ITEM6_COMBO2,
	INV_PUZZLE_ITEM7_COMBO1,
	INV_PUZZLE_ITEM7_COMBO2,
	INV_PUZZLE_ITEM8_COMBO1,
	INV_PUZZLE_ITEM8_COMBO2,
	INV_KEY_ITEM1,
	INV_KEY_ITEM2,
	INV_KEY_ITEM3,
	INV_KEY_ITEM4,
	INV_KEY_ITEM5,
	INV_KEY_ITEM6,
	INV_KEY_ITEM7,
	INV_KEY_ITEM8,
	INV_KEY_ITEM1_COMBO1,
	INV_KEY_ITEM1_COMBO2,
	INV_KEY_ITEM2_COMBO1,
	INV_KEY_ITEM2_COMBO2,
	INV_KEY_ITEM3_COMBO1,
	INV_KEY_ITEM3_COMBO2,
	INV_KEY_ITEM4_COMBO1,
	INV_KEY_ITEM4_COMBO2,
	INV_KEY_ITEM5_COMBO1,
	INV_KEY_ITEM5_COMBO2,
	INV_KEY_ITEM6_COMBO1,
	INV_KEY_ITEM6_COMBO2,
	INV_KEY_ITEM7_COMBO1,
	INV_KEY_ITEM7_COMBO2,
	INV_KEY_ITEM8_COMBO1,
	INV_KEY_ITEM8_COMBO2,
	INV_PICKUP_ITEM1,
	INV_PICKUP_ITEM2,
	INV_PICKUP_ITEM3,
	INV_PICKUP_ITEM4,
	INV_PICKUP_ITEM1_COMBO1,
	INV_PICKUP_ITEM1_COMBO2,
	INV_PICKUP_ITEM2_COMBO1,
	INV_PICKUP_ITEM2_COMBO2,
	INV_PICKUP_ITEM3_COMBO1,
	INV_PICKUP_ITEM3_COMBO2,
	INV_PICKUP_ITEM4_COMBO1,
	INV_PICKUP_ITEM4_COMBO2,
	INV_BURNING_TORCH_ITEM,
	INV_CROWBAR_ITEM,
	INV_EXAMINE1,
	INV_EXAMINE2,
	INV_EXAMINE3,
	INV_WET_CLOTH,
	INV_CROSSBOW_ITEM,
	INV_CROSSBOW_AMMO1_ITEM,
	INV_CLOTH,
	INV_BOTTLE,
	INV_PUZZLE_HOLE8,

	NUM_INVOBJ
};


extern INVOBJ inventry_objects_list[NUM_INVOBJ];
