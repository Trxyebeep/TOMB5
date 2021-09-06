#pragma once
#include "../global/vars.h"

void inject_joby(bool replace);

void KlaxonTremor();
static long CheckCableBox(PHD_VECTOR* pos, short size);
void ControlElectricalCables(short item_number);
void WreckingBallCollision(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll);
void ControlWreckingBall(short item_number);
void DrawWreckingBall(ITEM_INFO* item);
void ControlSecurityScreens(short item_number);

#define TriggerCableSparks	( (void(__cdecl*)(long, long, short, long, long)) 0x00442480 )
#define TriggerLaraSparks	( (void(__cdecl*)(long)) 0x00442320 )
