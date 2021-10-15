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
#include "tower2.h"
#include "lasers.h"

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
	lara.holster = LARA_HOLSTERS_PISTOLS;
	lara.location = -1;
	lara.highest_location = -1;
	lara.RopePtr = NO_ITEM;
	lara_item->hit_points = 1000;

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
		lara.pistols_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

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
		NailInvItem(convert_invobj_to_obj(gfTakeaways[i]));

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

	obj = &objects[CAMERA_TARGET];
	obj->using_drawanimating_item = 0;
	obj->draw_routine = 0;

	for (int i = SMASH_OBJECT1; i < MESHSWAP1; i++)
	{
		obj = &objects[i];
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		obj->initialise = InitialiseSmashObject;
		obj->control = SmashObjectControl;
		obj->collision = ObjectCollision;
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

	for (int i = SWITCH_TYPE1; i < SWITCH_TYPE7; i++)
	{
		obj = &objects[i];
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		obj->initialise = InitialiseSwitch;
		obj->control = SwitchControl;
		obj->collision = SwitchCollision;
	}

	obj = &objects[AIRLOCK_SWITCH];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = SEQUENCE_SWITCH1; i < STEEL_DOOR; i++)
	{
		obj = &objects[i];
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->control = FullBlockSwitchControl;
		obj->collision = FullBlockSwitchCollision;
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

	for (int i = DOOR_TYPE1; i < CLOSED_DOOR6_MIP; i += 2)	//skips over MIPs
	{
		obj = &objects[i];
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->collision = DoorCollision;
		obj->object_mip = 4096;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
	}

	for (int i = LIFT_DOORS1; i < PUSHPULL_DOOR1; i += 2)	//skips over MIPs
	{
		obj = &objects[i];
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

	for (int i = PUSHPULL_DOOR1; i < KICK_DOOR2_MIP; i += 2)	//skips over MIPs
	{
		obj = &objects[i];
		obj->initialise = InitialiseDoor;
		obj->control = PushPullKickDoorControl;
		obj->collision = PushPullKickDoorCollision;
		obj->object_mip = 4096;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = FLOOR_TRAPDOOR1; i < CEILING_TRAPDOOR1; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = FloorTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = CEILING_TRAPDOOR1; i < SCALING_TRAPDOOR; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = CeilingTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = TRAPDOOR1; i < FLOOR_TRAPDOOR1; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
		obj->control = TrapDoorControl;
		obj->collision = TrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = PUZZLE_ITEM1; i < PUZZLE_HOLE1; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialisePickup;
		obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (int i = PISTOLS_ITEM; i < COMPASS_ITEM; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialisePickup;
		obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (int i = SEARCH_OBJECT1; i < SEARCH_OBJECT4_MIP; i += 2)
	{
		obj = &objects[i];
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
//	obj->control = ControlCrossbow;
	obj->collision = 0;
//	obj->draw_routine = DrawWeaponMissile;
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

	for (int i = KEY_HOLE1; i < SWITCH_TYPE1; i++)
	{
		obj = &objects[i];
		obj->collision = KeyHoleCollision;
		obj->save_flags = 1;
	}

	for (int i = PUZZLE_HOLE1; i < PUZZLE_DONE1; i++)
	{
		obj = &objects[i];
		obj->control = ControlAnimatingSlots;
		obj->collision = PuzzleHoleCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = PUZZLE_DONE1; i < KEY_HOLE1; i++)
	{
		obj = &objects[i];
		obj->control = ControlAnimatingSlots;
		obj->collision = PuzzleDoneCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = ANIMATING1; i < ANIMATING13; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseAnimatingSlots;
		obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 4;
	}

	for (int i = ANIMATING13; i < BRIDGE_FLAT; i++)
	{
		obj = &objects[i];
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

	for (int i = 0; i < 28; i++)
		meshes[((i * 2) + objects[BURNING_ROOTS].mesh_index) + 1] = meshes[((i * 2) + objects[ANIMATING16_MIP].mesh_index)];

	obj = &objects[GRAPPLING_TARGET];
	obj->initialise = InitialiseGrapplingTarget;
//	obj->collision = AIPickupCollision;
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
//	obj->control = ControlSteelDoor;
//	obj->collision = SteelDoorCollision;
	obj->draw_routine_extra = DrawSteelDoorLensFlare;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_mesh = 1;

	for (int i = SWITCH_TYPE7; i < AIRLOCK_SWITCH; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseExplodingSwitch;
		obj->control = ControlAnimatingSlots;
	//	obj->collision = AIPickupCollision;
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

	for (int i = RAISING_BLOCK1; i < EXPANDING_PLATFORM; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseRaisingBlock;
	//	obj->control = ControlRaisingBlock;
		obj->collision = 0;
		obj->draw_routine = DrawScaledSpike;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	for (int i = SMOKE_EMITTER_WHITE; i < EARTHQUAKE; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseSmokeEmitter;
	//	obj->control = ControlSmokeEmitter;
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
//	obj->control = ControlIris;
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
//	obj->control = ControlEnemyMissile;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;	//what the fuck
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
//	obj->draw_routine = DrawLensFlares;
	obj->using_drawanimating_item = 0;

	obj = &objects[TELEPORTER];
	obj->initialise = InitialiseTeleporter;
//	obj->control = ControlTeleporter;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[LIFT_TELEPORTER];
//	obj->control = ControlLiftTeleporter;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[LASERS];
	obj->initialise = InitialiseLasers;
//	obj->control = ControlLasers;
//	obj->draw_routine = DrawLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[STEAM_LASERS];
	obj->initialise = InitialiseSteamLasers;
//	obj->control = ControlSteamLasers;
//	obj->draw_routine = DrawSteamLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLOOR_LASERS];
	obj->initialise = InitialiseFloorLasers;
//	obj->control = ControlFloorLasers;
	obj->draw_routine = DrawFloorLasers;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FISHTANK];
	obj->initialise = InitialiseFishtank;
//	obj->control = ControlFishtank;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	for (int i = WATERFALL1; i < FISHTANK; i++)
	{
		obj = &objects[i];
		obj->control = ControlWaterfall;
		obj->save_flags = 1;
	}

	for (int i = WATERFALLSS1; i < ANIMATING1; i++)
	{
		obj = &objects[i];
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
//	obj->control = ControlBodyPart;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;	//what the fuck
	obj->nmeshes = 0;
	obj->loaded = 1;
}

void TrapObjects()
{
	OBJECT_INFO* obj;

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
//	obj->control = ControlTwoBlockPlatform;
//	obj->floor = TwoBlockPlatformFloor;
//	obj->ceiling = TwoBlockPlatformCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KILL_ALL_TRIGGERS];
//	obj->control = KillAllCurrentItems;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->hit_points = 0;
	obj->save_flags = 1;

	for (int i = FALLING_BLOCK; i < CRUMBLING_FLOOR; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseFallingBlock2;
		obj->control = FallingBlock;
		obj->collision = FallingBlockCollision;
		obj->floor = FallingBlockFloor;
		obj->ceiling = FallingBlockCeiling;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[FALLING_CEILING];
//	obj->control = FallingCeiling;
//	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = PUSHABLE_OBJECT1; i < WRECKING_BALL; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseMovingBlock;
	//	obj->control = MovableBlock;
	//	obj->collision = MovableBlockCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[DARTS];
	obj->control = DartsControl;
	obj->collision = ObjectCollision;
//	obj->draw_routine = S_DrawDarts;
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
//	obj->control = ControlGasCloud;
//	obj->draw_routine = DrawGasCloud;
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
//	obj->control = ControlArea51Laser;
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

	for (int i = PROPELLER_H; i < GRAPPLING_TARGET; i++)
	{
		obj = &objects[i];
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

void inject_setup(bool replace)
{
	INJECT(0x00473210, InitialiseLara, replace);
	INJECT(0x00476360, ObjectObjects, 0);
	INJECT(0x00475D40, TrapObjects, 0);
}
