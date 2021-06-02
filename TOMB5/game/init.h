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

#define SetupClosedDoorStuff	( (void(__cdecl*)(DOOR_DATA*, ITEM_INFO*, short, int, int)) 0x0043E3F0 )
