#pragma once
#include "../global/vars.h"

void inject_subsuit();

void DoSubsuitStuff();
void FireChaff();
void TriggerAirBubbles();

#define TriggerEngineEffects	( (void(__cdecl*)()) 0x0047CB70 )
#define TriggerEngineEffects_CUT	( (void(__cdecl*)()) 0x0047D140 )
