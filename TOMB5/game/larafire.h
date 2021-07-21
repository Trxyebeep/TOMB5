#pragma once
#include "../global/vars.h"

void inject_larafire(bool replace);

void LaraGun();
static int CheckForHoldingState(int state);
void InitialiseNewWeapon();
void LaraTargetInfo(WEAPON_INFO* winfo);
void LaraGetNewTarget(WEAPON_INFO* winfo);
void find_target_point(ITEM_INFO* item, GAME_VECTOR* target);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
int FireWeapon(int weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles);
void SmashItem(short item_number, long weapon_type);
int WeaponObject(int weapon_type);
int WeaponObjectMesh(int weapon_type);
void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, int damage, int grenade);
short* get_current_ammo_pointer(int num);
void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv);
