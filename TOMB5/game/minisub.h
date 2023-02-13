#pragma once
#include "../global/types.h"

void TriggerTorpedoSteam(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff);
void TriggerMinisubLight(short item_number);
void FireTorpedo(ITEM_INFO* src);
void TorpedoControl(short item_number);
void ChaffControl(short item_number);
void TriggerMiniSubMist(PHD_VECTOR* pos1, PHD_VECTOR* pos2, long chaff);
void InitialiseMinisub(short item_number);
void MinisubControl(short item_number);
void InitialiseJelly(short item_number);
void JellyControl(short item_number);
