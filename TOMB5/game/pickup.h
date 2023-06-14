#pragma once
#include "../global/types.h"

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
void PickUpCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CollectCarriedItems(ITEM_INFO* item);

extern uchar NumRPickups;
extern uchar RPickups[16];
extern char KeyTriggerActive;
