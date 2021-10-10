#pragma once
#include "../global/vars.h"

extern short SPxzoffs[8];
extern short SPyoffs[8];

void inject_traps(bool replace);

void LaraBurn();
void LavaBurn(ITEM_INFO* item);
void ControlExplosion(short item_number);
void CloseTrapDoor(ITEM_INFO* item);
void DartEmitterControl(short item_number);
void DartsControl(short item_number);
void RollingBallCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void ControlRollingBall(short item_number);
