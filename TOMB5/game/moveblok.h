#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
long TestBlockPush(ITEM_INFO* item, long height, ushort quadrant);
long TestBlockPull(ITEM_INFO* item, long height, ushort quadrant);

#define ClearMovableBlockSplitters	( (void(__cdecl*)(long, long, long, short)) 0x0045E770 )
