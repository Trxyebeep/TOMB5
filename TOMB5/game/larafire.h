#pragma once
#include "../global/types.h"

void LaraGun();
void InitialiseNewWeapon();
void LaraTargetInfo(WEAPON_INFO* winfo);
void LaraGetNewTarget(WEAPON_INFO* winfo);
void find_target_point(ITEM_INFO* item, GAME_VECTOR* target);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles);
void SmashItem(short item_number, long weapon_type);
long WeaponObject(long weapon_type);
long WeaponObjectMesh(long weapon_type);
void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, long damage, long grenade);
short* get_current_ammo_pointer(long num);
void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv);

extern WEAPON_INFO weapons[9];
