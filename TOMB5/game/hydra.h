#pragma once
#include "../global/types.h"

void InitialiseHydra(short item_number);
void HydraControl(short item_number);
void TriggerHydraMissile(PHD_3DPOS* pos, short room_number, short num);
void TriggerHydraMissileFlame(PHD_VECTOR* pos, long xv, long yv, long zv);
void TriggerHydraPowerupFlames(short item_number, long shade);
