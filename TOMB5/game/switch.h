#pragma once
#include "../global/vars.h"

void inject_switch(bool replace);

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

#define TestTriggersAtXYZ	( (void(__cdecl*)(long, long, long, short, long, long)) 0x0047D9D0 )
#define ProcessExplodingSwitchType8	( (void(__cdecl*)(ITEM_INFO*)) 0x0047FF20 )
#define GetSwitchTrigger	( (long(__cdecl*)(ITEM_INFO*, short*, long)) 0x0047D7B0 )
#define GetKeyTrigger	( (long(__cdecl*)(ITEM_INFO*)) 0x0047D8C0 )
#define SwitchTrigger	( (long(__cdecl*)(short, short)) 0x0047D670 )
