#pragma once
#include "../global/types.h"

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, long angle, long room_no, long falloff);
void ControlStrobeLight(short item_number);
void ControlPulseLight(short item_number);
void ControlColouredLight(short item_number);
void ControlElectricalLight(short item_number);
void ControlBlinker(short item_number);
