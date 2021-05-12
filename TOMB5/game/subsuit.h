#pragma once

void inject_subsuit();

void DoSubsuitStuff();
void FireChaff();

#define TriggerAirBubbles	( (void(__cdecl*)()) 0x0047C4D0 )
#define TriggerEngineEffects	( (void(__cdecl*)()) 0x0047CB70 )
#define TriggerEngineEffects_CUT	( (void(__cdecl*)()) 0x0047D140 )
