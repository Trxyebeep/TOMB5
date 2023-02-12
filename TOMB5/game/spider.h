#pragma once
#include "../global/vars.h"

void inject_spider(bool replace);

long GetFreeSpider();
void ClearSpiders();
void TriggerSpider(short item_number);
void UpdateSpiders();
void DrawSpiders();

extern SPIDER_STRUCT* Spiders;
