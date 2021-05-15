#pragma once
#include "../global/vars.h"

void inject_joby();

void KlaxonTremor();
static int CheckCableBox(PHD_VECTOR* pos, short size);
void ControlElectricalCables(short item_number);

#define TriggerCableSparks	( (void(__cdecl*)(long, long, short, long, long)) 0x00442480 )
#define TriggerLaraSparks	( (void(__cdecl*)(long)) 0x00442320 )
