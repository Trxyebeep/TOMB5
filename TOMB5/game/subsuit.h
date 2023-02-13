#pragma once
#include "../global/types.h"

void DoSubsuitStuff();
void FireChaff();
void TriggerAirBubbles();
void GetLaraJointPosRot(PHD_VECTOR* pos, long node, long rot, SVECTOR* sv);
void TriggerSubMist(PHD_VECTOR* pos, PHD_VECTOR* pos1, long size);
void TriggerEngineEffects();
void TriggerEngineEffects_CUT();

extern SUBSUIT_INFO subsuit;
extern char SubHitCount;
