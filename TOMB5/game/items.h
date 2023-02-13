#pragma once
#include "../global/types.h"

void ItemNewRoom(short item_num, short room_number);
void InitialiseItemArray(short num);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);
void RemoveActiveItem(short item_num);
void RemoveDrawnItem(short item_num);
void AddActiveItem(short item_num);
short SpawnItem(ITEM_INFO* item, long obj_num);
long GlobalItemReplace(long in, long out);
void InitialiseFXArray(long allocmem);
short CreateEffect(short room_num);
void KillEffect(short fx_num);
void EffectNewRoom(short fx_num, short room_num);

extern short next_fx_active;
extern short next_item_active;
