#pragma once
#include "../global/types.h"

long GetFreeSpider();
void ClearSpiders();
void TriggerSpider(short item_number);
void UpdateSpiders();
void DrawSpiders();

extern SPIDER_STRUCT* Spiders;
