#pragma once
#include "../global/types.h"

void FireCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void DoFlameTorch();
void TriggerTorchFlame(short item_number, long node);
void GetFlameTorch();
void FlameTorchControl(short item_number);

extern short torchroom;
