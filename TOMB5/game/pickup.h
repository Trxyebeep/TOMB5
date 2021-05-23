#pragma once
#include "../global/vars.h"

void inject_pickup();

static void PuzzleDone(ITEM_INFO* item, short item_num);
void RegeneratePickups();
int KeyTrigger(short item_num);
int PickupTrigger(short item_num);
void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);

