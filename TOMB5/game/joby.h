#pragma once
#include "../global/types.h"

void KlaxonTremor();
void ControlElectricalCables(short item_number);
void WreckingBallCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void ControlWreckingBall(short item_number);
void DrawWreckingBall(ITEM_INFO* item);
void ControlSecurityScreens(short item_number);
void CookerFlameControl(short item_number);
void TriggerLaraSparks(long smoke);
void TriggerCableSparks(long x, long z, short item_number, long node, long flare);

extern short WB_room;
