#pragma once
#include "../global/vars.h"

void inject_skelly();

void InitialiseSkeleton(short item_number);
void SkeletonControl(short item_number);

#define TriggerSkeletonFire	( (void(__cdecl*)(ITEM_INFO*)) 0x00477D50 )
#define TriggerFontFire	( (void(__cdecl*)(ITEM_INFO*, int, int)) 0x00477ED0 )
