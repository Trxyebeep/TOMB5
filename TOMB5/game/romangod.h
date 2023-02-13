#pragma once
#include "../global/types.h"

void TriggerRomanGodSparks(long x, long y, long z, long xv, long yv, long zv, long god);
void TriggerRomanGodMissileFlame(PHD_VECTOR* pos, long fxnum);
void TriggerRomanGodMissile(PHD_3DPOS* pos, short room_number, short num);
void TriggerRomanGodLightningGlow(long x, long y, long z, long rgb);
void TriggerRomanGodPowerupFlames(short item_number, long shade);
void TriggerRomangodGravel(ITEM_INFO* item, PHD_VECTOR* pos, long node);
void InitialiseRomangod(short item_number);
void RomangodControl(short item_number);
