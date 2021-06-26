#pragma once
#include "../global/vars.h"

void inject_newinv2();

int S_CallInventory2();
void init_new_inventry();
void do_debounced_joystick_poo();
void DrawThreeDeeObject2D(int x, int y, int num, int shade, int xrot, int yrot, int zrot, int bright, int overlay);
void DrawInventoryItemMe(ITEM_INFO* item, long shade, int overlay, int shagflag);
int go_and_load_game();
int go_and_save_game();
void construct_combine_object_list();
void insert_object_into_list_v2(int num);
void construct_object_list();
void insert_object_into_list(int num);
void draw_current_object_list(int ringnum);
void handle_object_changeover(int ringnum);
void handle_inventry_menu();
void setup_ammo_selector();
void fade_ammo_selector();
void draw_ammo_selector();
void spinback(unsigned short* cock);
void update_laras_weapons_status();
int is_item_currently_combinable(short obj);
int have_i_got_item(short obj);
int do_these_objects_combine(int obj1, int obj2);
void combine_these_two_objects(short obj1, short obj2);
void seperate_object(short obj);
void combine_HK_SILENCER(int flag);
void combine_revolver_lasersight(int flag);
void combine_crossbow_lasersight(int flag);
void combine_PuzzleItem1(int flag);
void combine_PuzzleItem2(int flag);
void combine_PuzzleItem3(int flag);
void combine_PuzzleItem4(int flag);
void combine_PuzzleItem5(int flag);
void combine_PuzzleItem6(int flag);
void combine_PuzzleItem7(int flag);
void combine_PuzzleItem8(int flag);
void combine_KeyItem1(int flag);
void combine_KeyItem2(int flag);
void combine_KeyItem3(int flag);
void combine_KeyItem4(int flag);
void combine_KeyItem5(int flag);
void combine_KeyItem6(int flag);
void combine_KeyItem7(int flag);
void combine_KeyItem8(int flag);
void combine_PickupItem1(int flag);
void combine_PickupItem2(int flag);
void combine_PickupItem3(int flag);
void combine_PickupItem4(int flag);
void combine_clothbottle(int flag);
void setup_objectlist_startposition(short newobj);
void setup_objectlist_startposition2(short newobj);
void use_current_item();
void DEL_picked_up_object(short objnum);
void NailInvItem(short objnum);
int have_i_got_object(short object_number);
void remove_inventory_item(short object_number);
int convert_obj_to_invobj(short obj);
int convert_invobj_to_obj(int obj);
void init_keypad_mode();
void do_keypad_mode();
void do_examine_mode();
void do_stats_mode();
void dels_give_lara_items_cheat();
void dels_give_lara_guns_cheat();
int LoadGame();
int SaveGame();
void DelDrawSprite(int x, int y, int def, int z);

enum ring_types
{
	RING_INVENTORY,
	RING_AMMO
};

enum invobj_types
{
	INV_UZI_ITEM = 0,
	INV_PISTOLS_ITEM = 1,
	INV_SHOTGUN_ITEM = 2,
	INV_REVOLVER_ITEM1 = 3,
	INV_REVOLVER_ITEM2 = 4,
	INV_CROSSBOW_AMMO2_ITEM1 = 5,
	INV_CROSSBOW_AMMO2_ITEM2 = 6,
	INV_HK_ITEM1 = 7,
	INV_HK_ITEM2 = 8,
	INV_SHOTGUN_AMMO1_ITEM = 9,
	INV_SHOTGUN_AMMO2_ITEM = 10,
	INV_HK_AMMO_ITEM1 = 11,
	INV_HK_AMMO_ITEM2 = 12,
	INV_HK_AMMO_ITEM3 = 13,
	INV_HK_AMMO_ITEM4 = 14,
	INV_CROSSBOW_AMMO2_ITEM3 = 15,
	INV_CROSSBOW_AMMO2_ITEM4 = 16,
	INV_REVOLVER_AMMO_ITEM = 17,
	INV_UZI_AMMO_ITEM = 18,
	INV_PISTOLS_AMMO_ITEM = 19,
	INV_LASERSIGHT_ITEM = 20,
	INV_SILENCER_ITEM = 21,
	INV_BIGMEDI_ITEM = 22,
	INV_SMALLMEDI_ITEM = 23,
	INV_BINOCULARS_ITEM = 24,
	INV_FLARE_INV_ITEM = 25,
	INV_COMPASS_ITEM = 26,
	INV_MEMCARD_LOAD_INV_ITEM = 27,
	INV_MEMCARD_SAVE_INV_ITEM = 28,
	INV_PUZZLE_ITEM1 = 29,
	INV_PUZZLE_ITEM2 = 30,
	INV_PUZZLE_ITEM3 = 31,
	INV_PUZZLE_ITEM4 = 32,
	INV_PUZZLE_ITEM5 = 33,
	INV_PUZZLE_ITEM6 = 34,
	INV_PUZZLE_ITEM7 = 35,
	INV_PUZZLE_ITEM8 = 36,
	INV_PUZZLE_ITEM1_COMBO1 = 37,
	INV_PUZZLE_ITEM1_COMBO2 = 38,
	INV_PUZZLE_ITEM2_COMBO1 = 39,
	INV_PUZZLE_ITEM2_COMBO2 = 40,
	INV_PUZZLE_ITEM3_COMBO1 = 41,
	INV_PUZZLE_ITEM3_COMBO2 = 42,
	INV_PUZZLE_ITEM4_COMBO1 = 43,
	INV_PUZZLE_ITEM4_COMBO2 = 44,
	INV_PUZZLE_ITEM5_COMBO1 = 45,
	INV_PUZZLE_ITEM5_COMBO2 = 46,
	INV_PUZZLE_ITEM6_COMBO1 = 47,
	INV_PUZZLE_ITEM6_COMBO2 = 48,
	INV_PUZZLE_ITEM7_COMBO1 = 49,
	INV_PUZZLE_ITEM7_COMBO2 = 50,
	INV_PUZZLE_ITEM8_COMBO1 = 51,
	INV_PUZZLE_ITEM8_COMBO2 = 52,
	INV_KEY_ITEM1 = 53,
	INV_KEY_ITEM2 = 54,
	INV_KEY_ITEM3 = 55,
	INV_KEY_ITEM4 = 56,
	INV_KEY_ITEM5 = 57,
	INV_KEY_ITEM6 = 58,
	INV_KEY_ITEM7 = 59,
	INV_KEY_ITEM8 = 60,
	INV_KEY_ITEM1_COMBO1 = 61,
	INV_KEY_ITEM1_COMBO2 = 62,
	INV_KEY_ITEM2_COMBO1 = 63,
	INV_KEY_ITEM2_COMBO2 = 64,
	INV_KEY_ITEM3_COMBO1 = 65,
	INV_KEY_ITEM3_COMBO2 = 66,
	INV_KEY_ITEM4_COMBO1 = 67,
	INV_KEY_ITEM4_COMBO2 = 68,
	INV_KEY_ITEM5_COMBO1 = 69,
	INV_KEY_ITEM5_COMBO2 = 70,
	INV_KEY_ITEM6_COMBO1 = 71,
	INV_KEY_ITEM6_COMBO2 = 72,
	INV_KEY_ITEM7_COMBO1 = 73,
	INV_KEY_ITEM7_COMBO2 = 74,
	INV_KEY_ITEM8_COMBO1 = 75,
	INV_KEY_ITEM8_COMBO2 = 76,
	INV_PICKUP_ITEM1 = 77,
	INV_PICKUP_ITEM2 = 78,
	INV_PICKUP_ITEM3 = 79,
	INV_PICKUP_ITEM4 = 80,
	INV_PICKUP_ITEM1_COMBO1 = 81,
	INV_PICKUP_ITEM1_COMBO2 = 82,
	INV_PICKUP_ITEM2_COMBO1 = 83,
	INV_PICKUP_ITEM2_COMBO2 = 84,
	INV_PICKUP_ITEM3_COMBO1 = 85,
	INV_PICKUP_ITEM3_COMBO2 = 86,
	INV_PICKUP_ITEM4_COMBO1 = 87,
	INV_PICKUP_ITEM4_COMBO2 = 88,
	INV_BURNING_TORCH_ITEM = 89,
	INV_CROWBAR_ITEM = 90,
	INV_EXAMINE1 = 91,
	INV_EXAMINE2 = 92,
	INV_EXAMINE3 = 93,
	INV_WET_CLOTH = 94,
	INV_CROSSBOW_ITEM = 95,
	INV_CROSSBOW_AMMO1_ITEM = 96,
	INV_CLOTH = 97,
	INV_BOTTLE = 98,
	INV_PUZZLE_HOLE8 = 99,
};

