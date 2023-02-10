#pragma once
#include "../global/vars.h"

void inject_subsuit(bool replace);

void DoSubsuitStuff();
void FireChaff();
void TriggerAirBubbles();
void GetLaraJointPosRot(PHD_VECTOR* pos, long node, long rot, SVECTOR* sv);
void TriggerSubMist(PHD_VECTOR* pos, PHD_VECTOR* pos1, long size);
void TriggerEngineEffects();
void TriggerEngineEffects_CUT();
