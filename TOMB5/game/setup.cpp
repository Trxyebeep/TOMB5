#include "../tomb5/pch.h"
#include "setup.h"
#include "laramisc.h"
#include "gameflow.h"
#include "newinv2.h"
#include "objects.h"
#include "init.h"
#include "collide.h"
#include "traps.h"
#include "switch.h"
#include "door.h"
#include "pickup.h"
#include "minisub.h"
#include "laraflar.h"
#include "andy.h"
#include "flmtorch.h"
#include "twogun.h"
#include "tomb4fx.h"
#include "joby.h"
#include "autogun.h"
#include "effects.h"
#include "objlight.h"
#include "lifts.h"
#include "deathsld.h"
#include "andrea.h"
#include "rope.h"
#include "tower.h"
#include "lasers.h"
#include "lion.h"
#include "dog.h"
#include "huskie.h"
#include "crow.h"
#include "mafia2.h"
#include "pierre.h"
#include "sniper.h"
#include "chef.h"
#include "hydra.h"
#include "imp.h"
#include "willwisp.h"
#include "swampy.h"
#include "skeleton.h"
#include "rat.h"
#include "bat.h"
#include "spider.h"
#include "guardian.h"
#include "effect2.h"
#include "lara1gun.h"
#include "hair.h"
#include "../specific/function_stubs.h"
#include "draw.h"
#include "moveblok.h"
#include "romangod.h"
#include "hitman.h"
#include "../specific/specificfx.h"
#include "missile.h"
#include "sas.h"
#include "gladiatr.h"
#include "items.h"
#include "control.h"
#include "deltapak.h"
#include "../specific/file.h"
#include "camera.h"
#include "footprnt.h"
#include "lara.h"
#include "larafire.h"
#include "savegame.h"
#include "lara2gun.h"

void InitialiseLara(long restore)
{
	LARA_INFO backup;
	short item_num, gun;

	if (lara.item_number == NO_ITEM)
		return;

	item_num = lara.item_number;
	lara_item->data = &lara;
	lara_item->collidable = 0;

	if (!restore)
		memset(&lara, 0, sizeof(LARA_INFO));
	else
	{
		memcpy(&backup, &lara, sizeof(LARA_INFO));
		memset(&lara, 0, sizeof(LARA_INFO));
		memcpy(&lara.pistols_type_carried, &backup.pistols_type_carried, 59);	//restores inventory items
	}

	lara.look = 1;
	lara.item_number = item_num;
	lara.hit_direction = -1;
	lara.air = 1800;
	lara.weapon_item = NO_ITEM;
	PoisonFlag = 0;
	lara.dpoisoned = 0;
	lara.poisoned = 0;
	lara.water_surface_dist = 100;
	LHolster = LARA_HOLSTERS_PISTOLS;
	lara.holster = LARA_HOLSTERS_PISTOLS;
	lara.location = -1;
	lara.highest_location = -1;
	lara.RopePtr = NO_ITEM;
	lara_item->hit_points = 1000;

	for (int i = 0; i < gfNumPickups; i++)
		DEL_picked_up_object((short)convert_invobj_to_obj(gfPickups[i]));

	gfNumPickups = 0;

	if (!(gfLevelFlags & GF_YOUNGLARA) && objects[PISTOLS_ITEM].loaded)
		gun = WEAPON_PISTOLS;
	else
		gun = WEAPON_NONE;

	if (gfLevelFlags & GF_OFFICE && objects[HK_ITEM].loaded && lara.hk_type_carried & W_PRESENT)
		gun = WEAPON_HK;

	lara.gun_status = LG_NO_ARMS;
	lara.last_gun_type = gun;
	lara.gun_type = gun;
	lara.request_gun_type = gun;
	LaraInitialiseMeshes();
	lara.skelebob = 0;

	if (objects[PISTOLS_ITEM].loaded)
		lara.pistols_type_carried = W_PRESENT | W_AMMO1;

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

	for (int i = 0; i < gfNumTakeaways; i++)
		NailInvItem((short)convert_invobj_to_obj(gfTakeaways[i]));

	gfNumTakeaways = 0;
	weapons[WEAPON_REVOLVER].damage = gfCurrentLevel > LVL5_COLOSSEUM ? 15 : 6;

	if (gfCurrentLevel == LVL5_DEEPSEA_DIVE)
	{
		lara.puzzleitems[0] = 10;
		lara.pickupitems &= ~8;
	}
	else if (gfCurrentLevel == LVL5_SUBMARINE)
	{
		memset(&lara.puzzleitems, 0, 12);
		lara.puzzleitemscombo = 0;
		lara.pickupitems = 0;
		lara.pickupitemscombo = 0;
		lara.keyitems = 0;
		lara.keyitemscombo = 0;
	}
	else if (gfCurrentLevel == LVL5_SINKING_SUBMARINE)
	{
		lara.puzzleitems[0] = 0;
		lara.pickupitems = 0;
	}
	else if (gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
	{
		lara.pickupitems &= ~1;
		lara.puzzleitems[2] = 0;
		lara.puzzleitems[3] = 0;
		lara.pickupitems &= ~8;
	}
	else if (gfCurrentLevel == LVL5_RED_ALERT)
	{
		lara.pickupitems &= ~2;
		lara.pickupitems &= ~8;
	}

	if (gfCurrentLevel >= LVL5_THIRTEENTH_FLOOR && gfCurrentLevel <= LVL5_RED_ALERT)
	{
		lara.bottle = 0;
		lara.wetcloth = CLOTH_MISSING;
	}
}

void ObjectObjects()
{
	OBJECT_INFO* obj;
	long lp;

	obj = &objects[CAMERA_TARGET];
	obj->using_drawanimating_item = 0;
	obj->draw_routine = 0;

	for (lp = SMASH_OBJECT1; lp <= SMASH_OBJECT8; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseSmashObject;
		obj->control = SmashObjectControl;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	obj = &objects[BRIDGE_FLAT];
	obj->floor = BridgeFlatFloor;
	obj->ceiling = BridgeFlatCeiling;
	obj->object_mip = 3072;

	obj = &objects[BRIDGE_TILT1];
	obj->floor = BridgeTilt1Floor;
	obj->ceiling = BridgeTilt1Ceiling;
	obj->object_mip = 3072;

	obj = &objects[BRIDGE_TILT2];
	obj->floor = BridgeTilt2Floor;
	obj->ceiling = BridgeTilt2Ceiling;
	obj->object_mip = 3072;

	obj = &objects[CRUMBLING_FLOOR];
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_mesh = 1;
	obj->initialise = InitialiseFallingBlock2;
	obj->control = FallingBlock;
	obj->collision = FallingBlockCollision;
	obj->floor = FallingBlockFloor;
	obj->ceiling = FallingBlockCeiling;

	for (lp = SWITCH_TYPE1; lp <= SWITCH_TYPE6; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseSwitch;
		obj->control = SwitchControl;
		obj->collision = SwitchCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	obj = &objects[AIRLOCK_SWITCH];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (lp = SEQUENCE_SWITCH1; lp <= SEQUENCE_SWITCH3; lp++)
	{
		obj = &objects[lp];
		obj->control = FullBlockSwitchControl;
		obj->collision = FullBlockSwitchCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[UNDERWATER_SWITCH1];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision2;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[UNDERWATER_SWITCH2];
	obj->control = SwitchControl;
	obj->collision = UnderwaterSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TURN_SWITCH];
	obj->control = TurnSwitchControl;
	obj->collision = TurnSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[COG_SWITCH];
	obj->control = CogSwitchControl;
	obj->collision = CogSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[LEVER_SWITCH];
	obj->control = SwitchControl;
	obj->collision = RailSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[JUMP_SWITCH];
	obj->control = SwitchControl;
	obj->collision = JumpSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[CROWBAR_SWITCH];
	obj->control = SwitchControl;
	obj->collision = CrowbarSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PULLEY];
	obj->initialise = InitialisePulley;
	obj->control = SwitchControl;
	obj->collision = PulleyCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[CROWDOVE_SWITCH];
	obj->initialise = InitialiseCrowDoveSwitch;
	obj->control = CrowDoveSwitchControl;
	obj->collision = CrowDoveSwitchCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_mesh = 1;

	for (lp = DOOR_TYPE1; lp <= CLOSED_DOOR6; lp += 2)	//skips over MIPs
	{
		obj = &objects[lp];
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->collision = DoorCollision;
		obj->object_mip = 4096;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	for (lp = LIFT_DOORS1; lp <= LIFT_DOORS2; lp += 2)	//skips over MIPs
	{
		obj = &objects[lp];
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->draw_routine = DrawLiftDoors;
		obj->using_drawanimating_item = 0;
		obj->object_mip = 4096;
		obj->save_flags = 1;
	}

	obj = &objects[UNDERWATER_DOOR];
	obj->initialise = InitialiseDoor;
	obj->control = PushPullKickDoorControl;
	obj->collision = UnderwaterDoorCollision;
	obj->object_mip = 4096;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DOUBLE_DOORS];
	obj->initialise = InitialiseDoor;
	obj->control = PushPullKickDoorControl;
	obj->collision = DoubleDoorCollision;
	obj->object_mip = 4096;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SEQUENCE_DOOR1];
	obj->initialise = InitialiseDoor;
	obj->control = SequenceDoorControl;
	obj->collision = DoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (lp = PUSHPULL_DOOR1; lp <= KICK_DOOR2; lp += 2)	//skips over MIPs
	{
		obj = &objects[lp];
		obj->initialise = InitialiseDoor;
		obj->control = PushPullKickDoorControl;
		obj->collision = PushPullKickDoorCollision;
		obj->object_mip = 4096;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = FLOOR_TRAPDOOR1; lp <= FLOOR_TRAPDOOR2; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = FloorTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = CEILING_TRAPDOOR1; lp <= CEILING_TRAPDOOR2; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = CeilingTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = TRAPDOOR1; lp <= TRAPDOOR3; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = TrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = PUZZLE_ITEM1; lp <= BURNING_TORCH_ITEM; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialisePickup;
		obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (lp = PISTOLS_ITEM; lp <= FLARE_INV_ITEM; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialisePickup;
		obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (lp = SEARCH_OBJECT1; lp <= SEARCH_OBJECT4; lp += 2)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseSearchObject;
		obj->control = SearchObjectControl;
		obj->collision = SearchObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->object_mip = 2048;
	}

	obj = &objects[TORPEDO];
	obj->control = TorpedoControl;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[CHAFF];
	obj->control = ChaffControl;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[BURNING_TORCH_ITEM];
	obj->initialise = 0;
	obj->control = FlameTorchControl;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[CROSSBOW_BOLT];
	obj->initialise = 0;
	obj->control = ControlCrossbow;
	obj->collision = 0;
	obj->draw_routine = DrawWeaponMissile;
	obj->using_drawanimating_item = 0;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[FLARE_ITEM];
	obj->initialise = 0;
	obj->control = FlareControl;
	obj->collision = PickUpCollision;
	obj->draw_routine = DrawFlareInAir;
	obj->using_drawanimating_item = 0;
	obj->pivot_length = 256;
	obj->hit_points = 256;
	obj->save_position = 1;
	obj->save_flags = 1;

	for (lp = KEY_HOLE1; lp <= KEY_HOLE8; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseKeyhole;
		obj->collision = KeyHoleCollision;
		obj->save_flags = 1;
	}

	for (lp = PUZZLE_HOLE1; lp <= PUZZLE_HOLE8; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlAnimatingSlots;
		obj->collision = PuzzleHoleCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = PUZZLE_DONE1; lp <= PUZZLE_DONE8; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlAnimatingSlots;
		obj->collision = PuzzleDoneCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = ANIMATING1; lp <= ANIMATING12_MIP; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseAnimatingSlots;
		obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 4;
	}

	for (lp = ANIMATING13; lp <= ANIMATING16_MIP; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseAnimatingSlots;
		obj->control = ControlAnimatingSlots;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		obj->HitEffect = 0;
	}

	obj = &objects[BURNING_ROOTS];
	obj->initialise = InitialiseBurningRoots;
	obj->control = ControlBurningRoots;
	obj->collision = FireCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_mesh = 1;

	for (lp = 0; lp < 28; lp++)
		meshes[(objects[BURNING_ROOTS].mesh_index + (lp * 2)) + 1] = meshes[objects[ANIMATING16_MIP].mesh_index + (lp * 2)];

	obj = &objects[GRAPPLING_TARGET];
	obj->initialise = InitialiseGrapplingTarget;
	obj->collision = AIPickupCollision;
	obj->save_flags = 1;
	obj->save_mesh = 1;

	obj = &objects[EXPANDING_PLATFORM];
	obj->control = ControlZipController;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[TIGHT_ROPE];
	obj->initialise = InitialiseTightRope;
	obj->collision = TightRopeCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[PARALLEL_BARS];
	obj->collision = ParallelBarsCollision;

	obj = &objects[XRAY_CONTROLLER];
	obj->initialise = InitialiseXRayMachine;
	obj->control = ControlXRayMachine;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[STEEL_DOOR];
	obj->initialise = InitialiseSteelDoor;
	obj->control = ControlSteelDoor;
	obj->collision = SteelDoorCollision;
	obj->draw_routine_extra = DrawSteelDoorLensFlare;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_mesh = 1;

	for (lp = SWITCH_TYPE7; lp <= SWITCH_TYPE8; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseExplodingSwitch;
		obj->control = ControlAnimatingSlots;
		obj->collision = AIPickupCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	obj = &objects[TEETH_SPIKES];
	obj->initialise = InitialiseScaledSpike;
	obj->control = ControlScaledSpike;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	for (lp = RAISING_BLOCK1; lp <= RAISING_BLOCK2; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseRaisingBlock;
		obj->control = ControlRaisingBlock;
		obj->collision = 0;
		obj->draw_routine = DrawScaledSpike;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	for (lp = SMOKE_EMITTER_WHITE; lp <= STEAM_EMITTER; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseSmokeEmitter;
		obj->control = ControlSmokeEmitter;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	obj = &objects[ELECTRIC_FENCE];
	obj->control = ControlElectricFence;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[EXPLOSION];
	obj->initialise = InitialiseExplosion;
	obj->control = ControlExplosion;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[IRIS_LIGHTNING];
	obj->control = ControlIris;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[MONITOR_SCREEN];
	obj->collision = MonitorScreenCollision;
	obj->save_flags = 1;

	obj = &objects[SECURITY_SCREENS];
	obj->initialise = InitialiseSecurityScreens;
	obj->control = ControlSecurityScreens;
	obj->save_flags = 1;

	obj = &objects[MOTION_SENSORS];
	obj->initialise = InitialiseMotionSensors;
	obj->control = ControlMotionSensors;
	obj->collision = ObjectCollision;
	obj->explodable_meshbits = 1;
	obj->save_flags = 1;

	obj = &objects[BUBBLES];
	obj->control = ControlEnemyMissile;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;

	obj = &objects[WATERFALLMIST];
	obj->control = WaterFall;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[COLOURED_LIGHT];
	obj->control = ControlColouredLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BLINKING_LIGHT];
	obj->control = ControlBlinker;
	obj->save_flags = 1;

	obj = &objects[PULSE_LIGHT];
	obj->control = ControlPulseLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[STROBE_LIGHT];
	obj->control = ControlStrobeLight;
	obj->save_flags = 1;

	obj = &objects[ELECTRICAL_LIGHT];
	obj->control = ControlElectricalLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[LENS_FLARE];
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[TELEPORTER];
	obj->initialise = InitialiseTeleporter;
	obj->control = ControlTeleporter;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[LIFT_TELEPORTER];
	obj->control = ControlLiftTeleporter;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[LASERS];
	obj->initialise = InitialiseLasers;
	obj->control = ControlLasers;
	obj->draw_routine = DrawLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[STEAM_LASERS];
	obj->initialise = InitialiseSteamLasers;
	obj->control = ControlSteamLasers;
	obj->draw_routine = DrawSteamLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLOOR_LASERS];
	obj->initialise = InitialiseFloorLasers;
	obj->control = ControlFloorLasers;
	obj->draw_routine = DrawFloorLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FISHTANK];
	obj->initialise = InitialiseFishtank;
	obj->control = ControlFishtank;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	for (lp = WATERFALL1; lp <= WATERFALL3; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlWaterfall;
		obj->save_flags = 1;
	}

	for (lp = WATERFALLSS1; lp <= WATERFALLSS2; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlWaterfall;
		obj->save_flags = 1;
	}

	obj = &objects[EARTHQUAKE];
	obj->control = EarthQuake;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[HIGH_OBJECT2];
	obj->control = ControlLavaEffect;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BODY_PART];
	obj->control = ControlBodyPart;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;
}

void TrapObjects()
{
	OBJECT_INFO* obj;
	long lp;

	obj = &objects[ELECTRICAL_CABLES];
	obj->control = ControlElectricalCables;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ROME_HAMMER];
	obj->initialise = InitialiseRomeHammer;
	obj->control = ControlAnimatingSlots;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DEATH_SLIDE];
	obj->initialise = InitialiseDeathSlide;
	obj->control = ControlDeathSlide;
	obj->collision = DeathSlideCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ROLLINGBALL];
	obj->control = ControlRollingBall;
	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[TWOBLOCK_PLATFORM];
	obj->initialise = InitialiseTwoBlockPlatform;
	obj->control = ControlTwoBlockPlatform;
	obj->floor = TwoBlockPlatformFloor;
	obj->ceiling = TwoBlockPlatformCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KILL_ALL_TRIGGERS];
	obj->control = KillAllCurrentItems;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->hit_points = 0;
	obj->save_flags = 1;

	for (lp = FALLING_BLOCK; lp <= FALLING_BLOCK2; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseFallingBlock2;
		obj->control = FallingBlock;
		obj->collision = FallingBlockCollision;
		obj->floor = FallingBlockFloor;
		obj->ceiling = FallingBlockCeiling;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[FALLING_CEILING];
	obj->control = FallingCeiling;
	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (lp = PUSHABLE_OBJECT1; lp <= PUSHABLE_OBJECT5; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseMovingBlock;
		obj->control = MovableBlock;
		obj->collision = MovableBlockCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[DARTS];
	obj->control = DartsControl;
	obj->collision = ObjectCollision;
	obj->draw_routine = S_DrawDarts;
	obj->using_drawanimating_item = 0;
	obj->shadow_size = 128;

	obj = &objects[DART_EMITTER];
	obj->control = DartEmitterControl;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[HOMING_DART_EMITTER];
	obj->control = DartEmitterControl;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[FLAME];
	obj->control = FlameControl;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[FLAME_EMITTER];
	obj->initialise = InitialiseFlameEmitter;
	obj->control = FlameEmitterControl;
	obj->collision = FireCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER2];
	obj->initialise = InitialiseFlameEmitter2;
	obj->control = FlameEmitter2Control;
	obj->collision = FireCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER3];
	obj->control = FlameEmitter3Control;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[COOKER_FLAME];
	obj->initialise = InitialiseCookerFlame;
	obj->control = CookerFlameControl;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[CUTSCENE_ROPE];
	obj->initialise = InitialiseCutsceneRope;
	obj->control = CutsceneRopeControl;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;

	obj = &objects[PORTAL];
	obj->initialise = InitialisePortalDoor;
	obj->control = ControlPortalDoor;
	obj->draw_routine = DrawPortalDoor;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[GEN_SLOT1];
	obj->control = ControlGenSlot1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[GEN_SLOT2];
	obj->initialise = InitialiseGasCloud;
	obj->control = ControlGasCloud;
	obj->draw_routine = DrawGasCloud;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[GEN_SLOT3];
	obj->initialise = InitialiseGenSlot3;
	obj->control = ControlAnimatingSlots;
	obj->collision = HybridCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[GEN_SLOT4];
	obj->initialise = InitialiseArea51Laser;
	obj->control = ControlArea51Laser;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[HIGH_OBJECT1];
	obj->initialise = InitialiseRaisingPlinth;
	obj->control = ControlRaisingPlinth;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	init_all_ropes();
	obj = &objects[ROPE];
	obj->initialise = InitialiseRope;
	obj->control = RopeControl;
	obj->collision = RopeCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[POLEROPE];
	obj->collision = PoleCollision;
	obj->save_flags = 1;

	obj = &objects[WRECKING_BALL];
	obj->initialise = InitialiseWreckingBall;
	obj->control = ControlWreckingBall;
	obj->collision = WreckingBallCollision;
	obj->draw_routine_extra = DrawWreckingBall;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TRIGGER_TRIGGERER];
	obj->initialise = ControlTriggerTriggerer;
	obj->control = ControlTriggerTriggerer;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	for (lp = PROPELLER_H; lp <= PROPELLER_V; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialisePropeller;
		obj->control = ControlPropeller;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[RAISING_COG];
	obj->initialise = InitialiseRaisingCog;
	obj->control = ControlRaisingCog;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
}

void BaddyObjects()
{
	OBJECT_INFO* obj;
	long lp;

	obj = &objects[LARA];
	obj->initialise = InitialiseLaraLoad;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->shadow_size = 160;
	obj->hit_points = 1000;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	if (objects[SAS].loaded)
	{
		obj = &objects[SAS];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 40;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
	}

	if (objects[BLUE_GUARD].loaded)
	{
		obj = &objects[BLUE_GUARD];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 4;
		obj->explodable_meshbits = 0x4000;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	if (objects[SWAT].loaded)
	{
		obj = &objects[SWAT];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->explodable_meshbits = 0x4000;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	if (objects[SWAT_PLUS].loaded)
	{
		obj = &objects[SWAT_PLUS];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;
		else
			obj->anim_index = objects[BLUE_GUARD].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	if (objects[MAFIA].loaded)
	{
		obj = &objects[MAFIA];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;
		else
			obj->anim_index = objects[BLUE_GUARD].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->explodable_meshbits = 0x4000;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	if (objects[SCIENTIST].loaded)
	{
		obj = &objects[SCIENTIST];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;
		else
			obj->anim_index = objects[BLUE_GUARD].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	if (objects[CRANE_GUY].loaded)
	{
		obj = &objects[CRANE_GUY];
		obj->initialise = InitialiseSas;
		obj->control = SasControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;
		else
			obj->anim_index = objects[BLUE_GUARD].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 4;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (13 * 2)];
	}

	obj = &objects[CRANE_GUY_MIP];
	obj->control = ControlAnimatingSlots;
	obj->save_flags = 1;

	objects[SAS_MIP].save_flags = 1;
	objects[SWAT_MIP].save_flags = 1;
	objects[SWAT_PLUS_MIP].save_flags = 1;
	objects[BLUE_GUARD_MIP].save_flags = 1;
	objects[TWOGUN_MIP].save_flags = 1;
	objects[DOG_MIP].save_flags = 1;
	objects[CROW_MIP].save_flags = 1;
	objects[LARSON_MIP].save_flags = 1;
	objects[PIERRE_MIP].save_flags = 1;
	objects[MAFIA_MIP].save_flags = 1;
	objects[MAFIA2_MIP].save_flags = 1;
	objects[SAILOR_MIP].save_flags = 1;
	objects[LION_MIP].save_flags = 1;
	objects[GLADIATOR_MIP].save_flags = 1;
	objects[ROMAN_GOD_MIP].save_flags = 1;
	objects[HYDRA_MIP].save_flags = 1;
	objects[GUARDIAN_MIP].save_flags = 1;
	objects[HITMAN_MIP].save_flags = 1;
	objects[SCIENTIST_MIP].save_flags = 1;
	objects[WILLOWISP_MIP].save_flags = 1;
	objects[SKELETON_MIP].save_flags = 1;
	objects[REAPER_MIP].save_flags = 1;
	objects[MAZE_MONSTER_MIP].save_flags = 1;
	objects[GREEN_TEETH_MIP].save_flags = 1;
	objects[ATTACK_SUB_MIP].save_flags = 1;
	objects[SNIPER_MIP].save_flags = 1;
	objects[HUSKIE_MIP].save_flags = 1;
	objects[CHEF_MIP].save_flags = 1;
	objects[IMP_MIP].save_flags = 1;
	objects[GUNSHIP_MIP].save_flags = 1;

	if (objects[SAILOR].loaded)
	{
		obj = &objects[SAILOR];
		obj->initialise = InitialiseSas;
			obj->control = SasControl;
		obj->collision = CreatureCollision;

		if (objects[SWAT].loaded)
			obj->anim_index = objects[SWAT].anim_index;
		else
			obj->anim_index = objects[BLUE_GUARD].anim_index;

		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
	}

	if (objects[ATTACK_SUB].loaded)
	{
		obj = &objects[ATTACK_SUB];
		obj->initialise = InitialiseMinisub;
		obj->control = MinisubControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 100;
		obj->pivot_length = 200;
		obj->radius = 512;
		obj->bite_offset = 0;
		obj->object_mip = 7168;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 4] |= 4;
	}

	if (objects[LION].loaded)
	{
		obj = &objects[LION];
		obj->initialise = InitialiseLion;
		obj->control = LionControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 40;
		obj->pivot_length = 50;
		obj->radius = 341;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 76] |= 8;
	}

	if (objects[DOG].loaded)
	{
		obj = &objects[DOG];
		obj->initialise = InitialiseDog;
		obj->control = DogControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 18;
		obj->pivot_length = 50;
		obj->radius = 256;
		obj->object_mip = 5120;
		obj->bite_offset = 0;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 76] |= 8;
	}

	if (objects[HUSKIE].loaded)
	{
		obj = &objects[HUSKIE];
		obj->initialise = InitialiseHuskie;
		obj->control = HuskieControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 256;
		obj->object_mip = 5120;
		obj->bite_offset = 0;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 76] |= 8;
	}

	if (objects[CROW].loaded)
	{
		obj = &objects[CROW];
		obj->initialise = InitialiseCrow;
		obj->control = CrowControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 10;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 76] |= 8;
	}

	if (objects[REAPER].loaded)
	{
		obj = &objects[REAPER];
		obj->initialise = InitialiseJelly;
		obj->control = JellyControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 10;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}

	if (objects[MAFIA2].loaded)
	{
		obj = &objects[MAFIA2];
		obj->initialise = InitialiseMafia;
		obj->control = MafiaControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 26;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->explodable_meshbits = 0x4000;
		obj->bite_offset = 7;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
		meshes[obj->mesh_index + (7 * 2) + 1] = meshes[objects[MESHSWAP2].mesh_index + (7 * 2)];
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP2].mesh_index + (10 * 2)];
		meshes[obj->mesh_index + (13 * 2) + 1] = meshes[objects[MESHSWAP2].mesh_index + (13 * 2)];
	}

	if (objects[PIERRE].loaded)
	{
		obj = &objects[PIERRE];
		obj->initialise = InitialisePierre;
		obj->control = PierreControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 60;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 1;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 28] |= 4;
	}

	if (objects[LARSON].loaded)
	{
		obj = &objects[LARSON];
		obj->initialise = InitialisePierre;
		obj->control = PierreControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 60;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 3;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 28] |= 4;
	}

	if (objects[HITMAN].loaded)
	{
		obj = &objects[HITMAN];
		obj->initialise = InitialiseHitman;
		obj->control = HitmanControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 50;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 5;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->undead = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;

		for (lp = 0; lp < 20; lp++)
			meshes[obj->mesh_index + (lp * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (lp * 2)];
	}

	if (objects[SNIPER].loaded)
	{
		obj = &objects[SNIPER];
		obj->initialise = InitialiseSniper;
		obj->control = SniperControl;
		obj->collision = CreatureCollision;
		obj->draw_routine_extra = DrawBaddieGunFlash;
		obj->shadow_size = 128;
		obj->hit_points = 35;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->explodable_meshbits = 0x4000;
		obj->bite_offset = 6;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
	}

	if (objects[CHEF].loaded)
	{
		obj = &objects[CHEF];
		obj->initialise = InitialiseChef;
		obj->control = ChefControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 35;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
	}

	if (objects[TWOGUN].loaded)
	{
		obj = &objects[TWOGUN];
		obj->initialise = InitialiseTwogun;
		obj->control = TwogunControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 50;
		obj->radius = 128;
		obj->explodable_meshbits = 4;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 4] |= 8;
		bones[obj->bone_index + 4] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (2 * 2)];
	}

	if (objects[HYDRA].loaded)
	{
		obj = &objects[HYDRA];
		obj->initialise = InitialiseHydra;
		obj->control = HydraControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 1024;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 32] |= 8;
		bones[obj->bone_index + 32] |= 4;
		bones[obj->bone_index + 32] |= 16;
	}

	if (objects[IMP].loaded)
	{
		obj = &objects[IMP];
		obj->initialise = InitialiseImp;
		obj->control = ImpControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 12;
		obj->pivot_length = 20;
		obj->radius = 102;
		obj->bite_offset = 256;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 16] |= 16;
		bones[obj->bone_index + 16] |= 4;
		bones[obj->bone_index + 36] |= 16;
		bones[obj->bone_index + 36] |= 4;
		meshes[obj->mesh_index + (10 * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (10 * 2)];
	}

	if (objects[WILLOWISP].loaded)
	{
		obj = &objects[WILLOWISP];
		obj->initialise = InitialiseWillOWisp;
		obj->control = WillOWispControl;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
		obj->shadow_size = 128;
		obj->hit_points = 256;
		obj->pivot_length = 20;
		obj->radius = 256;
		obj->bite_offset = 256;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 16] |= 16;
		bones[obj->bone_index + 16] |= 4;
		bones[obj->bone_index + 36] |= 16;
		bones[obj->bone_index + 36] |= 4;
	}

	if (objects[MAZE_MONSTER].loaded)
	{
		obj = &objects[MAZE_MONSTER];
		obj->initialise = InitialiseMazeMonster;
		obj->control = MazeMonsterControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 4000;
		obj->pivot_length = 20;
		obj->radius = 341;
		obj->bite_offset = 256;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 16] |= 16u;
		bones[obj->bone_index + 16] |= 4u;
		bones[obj->bone_index + 36] |= 16u;
		bones[obj->bone_index + 36] |= 4u;
	}

	if (objects[GREEN_TEETH].loaded)
	{
		obj = &objects[GREEN_TEETH];
		obj->initialise = InitialiseSwampy;
		obj->control = SwampyControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 256;
		obj->hit_points = 100;
		obj->pivot_length = 20;
		obj->radius = 256;
		obj->bite_offset = 256;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		bones[obj->bone_index + 16] |= 16;
		bones[obj->bone_index + 16] |= 4;
		bones[obj->bone_index + 36] |= 16;
		bones[obj->bone_index + 36] |= 4;
	}

	if (objects[SKELETON].loaded)
	{
		obj = &objects[SKELETON];
		obj->initialise = InitialiseSkeleton;
		obj->control = SkeletonControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 100;
		obj->pivot_length = 20;
		obj->radius = 256;
		obj->bite_offset = 256;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 32] |= 8;
		bones[obj->bone_index + 32] |= 4;
	}

	if (objects[RAT].loaded)
	{
		obj = &objects[RAT];
		obj->initialise = InitialiseRatGenerator;
		obj->control = TriggerRat;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
	}

	if (objects[BAT].loaded)
	{
		obj = &objects[BAT];
		obj->initialise = InitialiseBatEmitter;
		obj->control = ControlBatEmitter;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
	}

	if (objects[SPIDER].loaded)
	{
		obj = &objects[SPIDER];
		obj->initialise = InitialiseSpiderGenerator;
		obj->control = TriggerSpider;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
	}

	if (objects[GLADIATOR].loaded)
	{
		obj = &objects[GLADIATOR];
		obj->initialise = InitialiseGladiator;
		obj->control = GladiatorControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 20;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;

		for (lp = 0; lp < 16; lp++)
			meshes[obj->mesh_index + (lp * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (lp * 2)];
	}

	if (objects[ROMAN_GOD].loaded)
	{
		obj = &objects[ROMAN_GOD];
		obj->initialise = InitialiseRomangod;
		obj->control = RomangodControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 300;
		obj->pivot_length = 50;
		obj->radius = 256;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 2;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;

		for (lp = 0; lp < 17; lp++)
			meshes[obj->mesh_index + (lp * 2) + 1] = meshes[objects[MESHSWAP1].mesh_index + (lp * 2)];
	}

	if (objects[GUARDIAN].loaded)
	{
		obj = &objects[GUARDIAN];
		obj->initialise = InitialiseGuardian;
		obj->control = GuardianControl;
		obj->collision = CreatureCollision;
		obj->explodable_meshbits = 6;
		obj->HitEffect = 3;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	if (objects[AUTOGUN].loaded)
	{
		obj = &objects[AUTOGUN];
		obj->initialise = InitialiseAutogun;
		obj->control = AutogunControl;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->HitEffect = 3;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 32] |= 8;
	}

	if (objects[GUNSHIP].loaded)
	{
		obj = &objects[GUNSHIP];
		obj->control = ControlGunship;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 4;
	}
}

void InitialiseObjects()
{
	OBJECT_INFO* obj;
	long lp;

	for (lp = 0; lp < NUMBER_OBJECTS; lp++)
	{
		obj = &objects[lp];
		obj->initialise = 0;
		obj->collision = 0;
		obj->control = 0;
		obj->intelligent = 0;
		obj->save_position = 0;
		obj->save_hitpoints = 0;
		obj->save_flags = 0;
		obj->save_anim = 0;
		obj->water_creature = 0;
		obj->using_drawanimating_item = 1;
		obj->save_mesh = 0;
		obj->draw_routine = DrawAnimatingItem;
		obj->ceiling = 0;
		obj->floor = 0;
		obj->pivot_length = 0;
		obj->radius = 10;
		obj->shadow_size = 0;
		obj->hit_points = -16384;
		obj->explodable_meshbits = 0;
		obj->draw_routine_extra = 0;
		obj->frame_base = (short*)((long)obj->frame_base + (char*)frames);
		obj->object_mip = 0;
	}

	BaddyObjects();
	ObjectObjects();
	TrapObjects();
	InitialiseHair();
	InitialiseEffects();

	for (lp = 0; lp < 6; lp++)
		SequenceUsed[lp] = 0;

	NumRPickups = 0;
	CurrentSequence = 0;
	SequenceResults[0][1][2] = 0;
	SequenceResults[0][2][1] = 1;
	SequenceResults[1][0][2] = 2;
	SequenceResults[1][2][0] = 3;
	SequenceResults[2][0][1] = 4;
	SequenceResults[2][1][0] = 5;

	for (lp = 0; lp < gfNumMips; lp++)
		objects[2 * (gfMips[lp] & 0xF) + ANIMATING1].object_mip = 64 * (gfMips[lp] & 0xF0);

	if (objects[RAT].loaded)
		Rats = (RAT_STRUCT*)game_malloc(832);

	if (objects[BAT].loaded)
		Bats = (BAT_STRUCT*)game_malloc(1920);

	if (objects[SPIDER].loaded)
		Spiders = (SPIDER_STRUCT*)game_malloc(1664);
}

void GetCarriedItems()
{
	ITEM_INFO* baddy;
	ITEM_INFO* pickup;
	ROOM_INFO* r;
	long lp;
	short item_num;

	for (lp = 0; lp < level_items; lp++)
	{
		baddy = &items[lp];
		baddy->carried_item = NO_ITEM;
	}

	for (lp = 0; lp < level_items; lp++)
	{
		baddy = &items[lp];

		if (objects[baddy->object_number].intelligent || (baddy->object_number >= SEARCH_OBJECT1 && baddy->object_number <= SEARCH_OBJECT3))
		{
			r = &room[baddy->room_number];
			item_num = r->item_number;

			while (item_num != NO_ITEM)
			{
				pickup = &items[item_num];

				if (abs(baddy->pos.x_pos - pickup->pos.x_pos) < 512 && abs(baddy->pos.y_pos - pickup->pos.y_pos) < 256 &&
					abs(baddy->pos.z_pos - pickup->pos.z_pos) < 512 && objects[pickup->object_number].collision == PickUpCollision)
				{
					pickup->carried_item = baddy->carried_item;
					baddy->carried_item = item_num;
					RemoveDrawnItem(item_num);
					pickup->room_number = NO_ROOM;
				}

				item_num = pickup->next_item;
			}
		}
	}
}

void InitialiseGameFlags()
{
	memset(flipmap, 0, 1020);
	memset(flip_stats, 0, 1020);
	flipeffect = -1;
	flip_status = 0;
	memset(cd_flags, 0, sizeof(cd_flags));
	IsAtmospherePlaying = 0;
	camera.underwater = 0;
}

void ClearFootPrints()
{
	for (int i = 0; i < 32; i++)
		FootPrint[i].Active = 0;

	FootPrintNum = 0;
}

void reset_cutseq_vars()
{
	cutseq_num = 0;
	cutseq_trig = 0;
	GLOBAL_playing_cutseq = 0;
	GLOBAL_cutseq_frame = 0;
	SetFadeClip(0, 1);
}

void GetAIPickups()
{
	ITEM_INFO* item;
	AIOBJECT* aiObj;
	short aiObjNum;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (!objects[item->object_number].intelligent)
			continue;

		item->ai_bits = 0;

		for (aiObjNum = 0; aiObjNum < nAIObjects; aiObjNum++)
		{
			aiObj = &AIObjects[aiObjNum];

			if (abs(aiObj->x - item->pos.x_pos) < 512 && abs(aiObj->z - item->pos.z_pos) < 512 &&
				aiObj->room_number == item->room_number && aiObj->object_number < AI_PATROL2)
			{
				item->ai_bits |= 1 << (aiObj->object_number - AI_GUARD);
				item->item_flags[3] = aiObj->trigger_flags;

				if (aiObj->object_number != AI_GUARD)
					aiObj->room_number = NO_ROOM;
			}
		}

		item->TOSSPAD |= item->ai_bits << 8 | (item->item_flags[3] & 0xFF);
	}
}

void BuildOutsideTable()
{
	ROOM_INFO* r;
	uchar* pTable;
	uchar* oTable;
	uchar* cTable;
	long max_slots, roomx, roomy, cont, offset, z, z2;
	long x, y, lp;
	char flipped[256];

	max_slots = 0;
	OutsideRoomOffsets = (short*)game_malloc(0x5B2);
	OutsideRoomTable = (char*)game_malloc(0xB640);
	memset(OutsideRoomTable, 0xFF, 0xB640);
	memset(flipped, 0, 255);

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (r->flipped_room != -1)
			flipped[r->flipped_room] = 1;
	}

	for (y = 0; y < 108; y += 4)
	{
		for (x = 0; x < 108; x += 4)
		{
			for (int i = 0; i < number_rooms; i++)
			{
				r = &room[i];

				if (flipped[i])
					continue;

				roomx = (r->z >> 10) + 1;
				roomy = (r->x >> 10) + 1;
				cont = 0;

				for (int ry = 0; ry < 4; ry++)
				{
					for (int rx = 0; rx < 4; rx++)
					{
						if (x + rx >= roomx && x + rx < roomx + r->x_size - 2 && y + ry >= roomy && y + ry < roomy + r->y_size - 2)
						{
							cont = 1;
							break;
						}
					}
				}

				if (!cont)
					continue;

				if (i == 255)
					printf("ERROR : Room 255 fuckeroony - go tell Chris\n");

				pTable = (uchar*)&OutsideRoomTable[64 * ((x >> 2) + 27 * (y >> 2))];

				for (lp = 0; lp < 64; lp++)
				{
					if (pTable[lp] == 255)
					{
						pTable[lp] = i;

						if (lp > max_slots)
							max_slots = lp;

						break;
					}
				}

				if (lp == 64)
					printf("ERROR : Buffer shittage - go tell Chris\n");
			}
		}
	}

	oTable = (uchar*)OutsideRoomTable;

	for (y = 0; y < 27; y++)
	{
		for (x = 0; x < 27; x++)
		{
			z = 0;
			offset = x + y * 27;
			pTable = (uchar*)&OutsideRoomTable[64 * (x + 27 * y)];
			while (pTable[z] != 255) z++;

			if (!z)
				OutsideRoomOffsets[offset] = -1;
			else if (z == 1)
				OutsideRoomOffsets[offset] = *pTable | 0x8000;
			else
			{
				cTable = (uchar*)OutsideRoomTable;

				while (cTable < oTable)
				{
					if (!memcmp(cTable, pTable, z))
					{
						OutsideRoomOffsets[offset] = short((long)cTable - (long)OutsideRoomTable);
						break;
					}

					z2 = 0;
					while (cTable[z2] != 255) z2++;
					cTable += z2 + 1;
				}

				if (cTable >= oTable)
				{
					OutsideRoomOffsets[offset] = short((long)oTable - (long)OutsideRoomTable);

					do
					{
						*oTable++ = *pTable++;
						z--;

					} while (z);

					*oTable++ = 255;
				}
			}
		}
	}
}

void SetupGame()
{
	SeedRandomDraw(0xD371F947);
	SeedRandomControl(0xD371F947);
	wibble = 0;
	torchroom = NO_ROOM;
	ClearFootPrints();
	InitBinoculars();
	InitTarget();
	InitialiseGameFlags();

	if (gfCurrentLevel == LVL5_THIRTEENTH_FLOOR || gfCurrentLevel == LVL5_BASE || gfCurrentLevel == LVL5_GALLOWS_TREE ||
		gfCurrentLevel == LVL5_STREETS_OF_ROME || gfInitialiseGame)
		InitialiseLara(0);
	else
		InitialiseLara(1);

	GetCarriedItems();
	GetAIPickups();
	SeedRandomDraw(0xD371F947);
	SeedRandomControl(0xD371F947);
}
