#pragma once
#include "../global/vars.h"

void inject_pickup(bool replace);

void PuzzleDone(ITEM_INFO* item, short item_num);
void RegeneratePickups();
long KeyTrigger(short item_num);
long PickupTrigger(short item_num);
void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleHoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void SearchObjectControl(short item_number);
void SearchObjectCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void MonitorScreenCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void AnimatingPickUp(short item_number);
void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
short* FindPlinth(ITEM_INFO* item);
void PickUpCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

#define	CollectCarriedItems	( (void(__cdecl*)(ITEM_INFO*)) 0x00469C90 )
