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
void TriggerLiftBrakeSparks(PHD_VECTOR* pos, short yrot);
void TriggerSteelDoorSmoke(short angle, short nPos, ITEM_INFO* item);
void TriggerWeldingEffects(PHD_VECTOR* pos, short yrot, short flag);

#define TriggerFishtankSpray	( (void(__cdecl*)(long, long, long, long)) 0x00486A70 )
