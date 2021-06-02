#pragma once
#include "../global/vars.h"

extern short SPxzoffs[8];
extern short SPyoffs[8];

void inject_traps();

void LaraBurn();
void LavaBurn(ITEM_INFO* item);

#define CloseTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x00488E30 )
