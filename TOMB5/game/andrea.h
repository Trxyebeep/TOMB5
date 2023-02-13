#pragma once
#include "../global/types.h"

void ControlPropeller(short item_number);
void TriggerFanEffects(long* b, long y, short angle, long rate);
void ControlRaisingCog(short item_number);
void ControlPortalDoor(short item_number);
void ControlGenSlot1(short item_number);
void ControlRaisingPlinth(short item_number);
void DrawPortalDoor(ITEM_INFO* item);
