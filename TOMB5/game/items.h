#pragma once
#include "../global/vars.h"

void inject_items(bool replace);

void ItemNewRoom(short item_num, short room_number);
void InitialiseItemArray(short num);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);

#define AddActiveItem	( (void(__cdecl*)(short)) 0x00440D10 )
#define	RemoveActiveItem	( (short(__cdecl*)(short)) 0x00440B60 )
#define CreateEffect	( (short(__cdecl*)(short)) 0x004410F0 )
#define InitialiseFXArray	( (void(__cdecl*)(long)) 0x00441080 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x004412F0 )
#define KillEffect	( (void(__cdecl*)(short)) 0x00441180 )
#define RemoveDrawnItem	( (void(__cdecl*)(short)) 0x00440C40 )
