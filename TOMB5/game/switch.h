#pragma once
#include "../global/types.h"

void CrowDoveSwitchControl(short item_number);
void CrowDoveSwitchCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void SwitchControl(short item_number);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PulleyCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TurnSwitchControl(short item_number);
void TurnSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void RailSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void JumpSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CrowbarSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FullBlockSwitchControl(short item_number);
void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CogSwitchControl(short item_number);
void CogSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ProcessExplodingSwitchType8(ITEM_INFO* item);
void TestTriggersAtXYZ(long x, long y, long z, short room_number, short heavy, short flags);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);
long GetKeyTrigger(ITEM_INFO* item);
long SwitchTrigger(short item_number, short timer);

extern PHD_VECTOR OldPickupPos;
extern uchar CurrentSequence;
extern uchar Sequences[3];
extern uchar SequenceUsed[6];
extern uchar SequenceResults[3][3][3];
