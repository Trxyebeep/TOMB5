#include "../tomb5/pch.h"
#include "setup.h"
#include "laramisc.h"
#include "gameflow.h"
#include "newinv2.h"
#include "objects.h"

void InitialiseLara(int restore)
{
	LARA_INFO backup;
	short item_num, gun;

	if (lara.item_number == NO_ITEM)
		return;

	item_num = lara.item_number;
	lara_item->data = &lara;
	lara_item->collidable = 0;

	if (!restore)
	{
		memset(&lara, 0, 0x154u);
		lara.TightRopeOff = 0;
		lara.TightRopeFall = 0;
		lara.ChaffTimer = 0;
	}
	else
	{
		memcpy(&backup, &lara, sizeof(lara));
		memset(&lara, 0, sizeof(lara));
		memcpy(&lara.pistols_type_carried, &backup.pistols_type_carried, 0x38u);
		lara.num_crossbow_ammo2 = backup.num_crossbow_ammo2;
	}

	lara.look = 1;
	lara.item_number = item_num;
	lara.hit_direction = 0;
	lara.air = 1800;
	lara.weapon_item = NO_ITEM;
	PoisonFlag = 0;
	lara.dpoisoned = 0;
	lara.poisoned = 0;
	lara.water_surface_dist = 100;
	lara.holster = 14;
	lara.location = -1;
	lara.highest_location = -1;
	lara.RopePtr = NO_ITEM;
	lara_item->hit_points = 1000;

	if (gfNumPickups > 0)
		for (int i = 0; i < gfNumPickups; i++)
			DEL_picked_up_object(convert_invobj_to_obj(gfPickups[i]));

	gfNumPickups = 0;

	if (!(gfLevelFlags & GF_YOUNGLARA) && objects[PISTOLS_ITEM].loaded)
		gun = WEAPON_PISTOLS;
	else
		gun = WEAPON_NONE;

	if (gfLevelFlags & GF_OFFICE && objects[HK_ITEM].loaded && lara.hk_type_carried & WTYPE_PRESENT)
		gun = WEAPON_HK;

	lara.gun_status = LG_NO_ARMS;
	lara.last_gun_type = gun;
	lara.gun_type = gun;
	lara.request_gun_type = gun;
	LaraInitialiseMeshes();
	lara.skelebob = 0;

	if (objects[PISTOLS_ITEM].loaded)
		lara.pistols_type_carried = 9;

	lara.binoculars = 1;

	if (!restore)
	{
		if (objects[FLARE_INV_ITEM].loaded)
			lara.num_flares = 3;

		lara.num_small_medipack = 3;
		lara.num_large_medipack = 1;
	}

	lara.num_pistols_ammo = -1;
	InitialiseLaraAnims(lara_item);
	DashTimer = 120;

	if (gfNumTakeaways > 0)
		for (int i = 0; i < gfNumTakeaways; i++)
			NailInvItem(convert_invobj_to_obj(gfTakeaways[i]));

	gfNumTakeaways = 0;
	weapons[WEAPON_REVOLVER].damage = gfCurrentLevel > LVL5_COLOSSEUM ? 15 : 6;

	switch (gfCurrentLevel)
	{
	case LVL5_DEEPSEA_DIVE:
		lara.pickupitems &= 0xFFF7;
		lara.puzzleitems[0] = 10;
		return;

	case LVL5_SUBMARINE:
		memset(&lara.puzzleitems, 0, 12);
		lara.puzzleitemscombo = 0;
		lara.pickupitems = 0;
		lara.pickupitemscombo = 0;
		lara.keyitems = 0;
		lara.keyitemscombo = 0;
		return;

	case LVL5_SINKING_SUBMARINE:
		lara.puzzleitems[0] = 0;
		lara.pickupitems = 0;
		return;

	case LVL5_ESCAPE_WITH_THE_IRIS:
		lara.pickupitems &= 0xFFFEu;
		lara.puzzleitems[2] = 0;
		lara.puzzleitems[3] = 0;
		break;

	case LVL5_RED_ALERT:
		lara.pickupitems &= 0xFFFD;
		break;

	default:

		if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT)
			lara.pickupitems &= 0xFFF7;

		break;
	}

	lara.bottle = 0;
	lara.wetcloth = CLOTH_MISSING;
}

void inject_setup(bool replace)
{
	INJECT(0x00473210, InitialiseLara, replace);
}
