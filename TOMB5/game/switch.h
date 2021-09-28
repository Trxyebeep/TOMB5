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

#define TestTriggersAtXYZ	( (void(__cdecl*)(long, long, long, short, int, int)) 0x0047D9D0 )
#define ProcessExplodingSwitchType8	( (void(__cdecl*)(ITEM_INFO*)) 0x0047FF20 )
#define GetSwitchTrigger	( (int(__cdecl*)(ITEM_INFO*, short*, long)) 0x0047D7B0 )
#define GetKeyTrigger	( (int(__cdecl*)(ITEM_INFO*)) 0x0047D8C0 )
#define SwitchTrigger	( (int(__cdecl*)(short, short)) 0x0047D670 )
