#pragma once
#include "../global/vars.h"

void inject_minisub(bool replace);

void TriggerTorpedoSteam(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff);
void TriggerMinisubLight(short item_number);
void FireTorpedo(ITEM_INFO* src);
void TorpedoControl(short item_number);
void ChaffControl(short item_number);

#define TriggerMiniSubMist	( (void(__cdecl*)(PHD_VECTOR*, PHD_VECTOR*, long)) 0x0045D1D0 )
