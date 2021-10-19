#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);

#define ClearMovableBlockSplitters	( (void(__cdecl*)(long, long, long, short)) 0x0045E770 )
#define TestBlockPush	( (long(__cdecl*)(ITEM_INFO*, long, ushort)) 0x0045F010 )
#define TestBlockPull	( (long(__cdecl*)(ITEM_INFO*, long, ushort)) 0x0045F1E0 )
