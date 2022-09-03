#pragma once
#include "../global/vars.h"

void inject_tower2(bool replace);

void ControlGunship(short item_number);
void ControlIris(short item_number);
void ControlFishtank(short item_number);
void ControlArea51Laser(short item_number);
void ControlGasCloud(short item_number);
void SteelDoorCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void ControlSteelDoor(short item_number);
void DrawSprite2(long x, long y, long slot, long col, long size, long z);
void DrawSteelDoorLensFlare(ITEM_INFO* item);

#define TriggerLiftBrakeSparks	( (void(__cdecl*)(PHD_VECTOR*, short)) 0x00487B60 )
#define TriggerFishtankSpray	( (void(__cdecl*)(long, long, long, long)) 0x00486A70 )
#define TriggerWeldingEffects	( (void(__cdecl*)(PHD_VECTOR*, short, short)) 0x004870B0 )
#define TriggerSteelDoorSmoke	( (void(__cdecl*)(short, short, ITEM_INFO*)) 0x004877E0 )
