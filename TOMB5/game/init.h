#pragma once
#include "../global/vars.h"

void inject_init();

void InitialiseTrapDoor(short item_number);
void InitialiseFallingBlock2(short item_number);
void InitialiseFlameEmitter(short item_number);
void InitialiseFlameEmitter2(short item_number);
void InitialiseTwoBlockPlatform(short item_number);
void InitialiseScaledSpike(short item_number);
void InitialiseRaisingBlock(short item_number);
void InitialiseSmashObject(short item_number);
void InitialiseEffects();
void InitialiseSmokeEmitter(short item_number);
void InitialiseDoor(short item_number);
void InitialisePulley(short item_number);
void InitialisePickup(short item_number);
void InitialiseClosedDoors();
void AddClosedDoor(ITEM_INFO* item);
void SetupClosedDoorStuff(DOOR_DATA* door, ITEM_INFO* item, short room2, int dx, int dy);
void SCDS(room_info* r, short** dptr, char* dn, long dx, long dy, long ox, long oz);
void InitialiseLasers(short item_number);
void InitialiseSteamLasers(short item_number);
void InitialiseFloorLasers(short item_number);
void InitialiseFishtank(short item_number);
void InitialiseWreckingBall(short item_number);
void InitialiseSearchObject(short item_number);
void InitialiseExplosion(short item_number);
