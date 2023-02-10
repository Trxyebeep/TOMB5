#pragma once
#include "../global/vars.h"

void inject_joby(bool replace);

void KlaxonTremor();
void ControlElectricalCables(short item_number);
void WreckingBallCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void ControlWreckingBall(short item_number);
void DrawWreckingBall(ITEM_INFO* item);
void ControlSecurityScreens(short item_number);
void CookerFlameControl(short item_number);
void TriggerLaraSparks(long smoke);

#define TriggerCableSparks	( (void(__cdecl*)(long, long, short, long, long)) 0x00442480 )
