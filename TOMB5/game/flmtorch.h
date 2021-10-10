#pragma once
#include "../global/vars.h"

void inject_flmtorch(bool replace);

void FireCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);

#define DoFlameTorch	( (void(__cdecl*)()) 0x00433EA0 )
