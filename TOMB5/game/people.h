#pragma once
#include "../global/vars.h"

void inject_people();

int TargetVisible(ITEM_INFO* item, AI_info* info);
int Targetable(ITEM_INFO* item, AI_info* info);
short GunShot(long x, long y, long z, short speed, short yrot, short room_number);
short GunHit(long x, long y, long z, short speed, short yrot, short room_number);
short GunMiss(long x, long y, long z, short speed, short yrot, short room_number);
int ShotLara(ITEM_INFO* item, AI_info* info, BITE_INFO* gun, short extra_rotation, int damage);
