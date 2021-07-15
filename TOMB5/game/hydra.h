#pragma once
#include "../global/vars.h"

void inject_hydra();

void InitialiseHydra(short item_number);
void HydraControl(short item_number);

#define TriggerHydraPowerupFlames	( (void(__cdecl*)(short, short)) 0x0043C910 )
#define TriggerHydraMissile	( (void(__cdecl*)(PHD_3DPOS*, short, short)) 0x0043C6C0 )
