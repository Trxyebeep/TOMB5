#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

void ControlBodyPart(short fx_number);
void ShootAtLara(FX_INFO* fx);
void ControlMissile(short fx_number);
long ExplodeFX(FX_INFO* fx, long NoXZVel, short Num);
