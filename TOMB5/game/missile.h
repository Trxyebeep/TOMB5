#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

void ControlBodyPart(short fx_number);
void ShootAtLara(FX_INFO* fx);
void ControlMissile(short fx_number);

#define ExplodeFX	( (long(__cdecl*)(FX_INFO*, long, long)) 0x0045E010 )
