#pragma once
#include "../global/vars.h"

void inject_items(bool replace);

void ItemNewRoom(short item_num, short room_number);

#define KillItem	( (void(__cdecl*)(short)) 0x00440620 )
#define AddActiveItem	( (void(__cdecl*)(short)) 0x00440D10 )
#define	RemoveActiveItem	( (short(__cdecl*)(short)) 0x00440B60 )
#define CreateItem	( (short(__cdecl*)()) 0x00440840)
#define CreateEffect	( (short(__cdecl*)(short)) 0x004410F0 )
#define InitialiseItem	( (void(__cdecl*)(short)) 0x004408B0 )
#define InitialiseFXArray	( (void(__cdecl*)(int)) 0x00441080 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x004412F0 )
#define KillEffect	( (void(__cdecl*)(short)) 0x00441180 )
