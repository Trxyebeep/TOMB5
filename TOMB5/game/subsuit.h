#pragma once
#include "../global/vars.h"

void inject_subsuit(bool replace);

void DoSubsuitStuff();
void FireChaff();
void TriggerAirBubbles();
void GetLaraJointPosRot(PHD_VECTOR* pos, long node, long rot, SVECTOR* sv);

#define TriggerEngineEffects	( (void(__cdecl*)()) 0x0047CB70 )
#define TriggerEngineEffects_CUT	( (void(__cdecl*)()) 0x0047D140 )
