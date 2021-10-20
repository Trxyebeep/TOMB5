#pragma once
#include "../global/vars.h"

void inject_tower2(bool replace);

void ControlGunship(short item_number);
void DrawSteelDoorLensFlare(ITEM_INFO* item);
void ControlIris(short item_number);
void ControlFishtank(short item_number);

#define TriggerLiftBrakeSparks	( (void(__cdecl*)(PHD_VECTOR*, short)) 0x00487B60 )
#define TriggerFishtankSpray	( (void(__cdecl*)(long, long, long, long)) 0x00486A70 )
