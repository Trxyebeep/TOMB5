#pragma once
#include "../global/vars.h"

void inject_objlight(bool replace);

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, long angle, int room_no, int falloff);
void ControlStrobeLight(short item_number);
void ControlPulseLight(short item_number);
void ControlColouredLight(short item_number);
void ControlElectricalLight(short item_number);
void ControlBlinker(short item_number);
