#pragma once
#include "../global/types.h"

void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
long TestBlockPush(ITEM_INFO* item, long height, ushort quadrant);
long TestBlockPull(ITEM_INFO* item, long height, ushort quadrant);
void ClearMovableBlockSplitters(long x, long y, long z, short room_number);
