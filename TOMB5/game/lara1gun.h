#pragma once
#include "../global/vars.h"


void inject_lara1gun(bool replace);

void draw_shotgun_meshes(int weapon_type);
void undraw_shotgun_meshes(int weapon_type);
void ready_shotgun(int weapon_type);
void RifleHandler(int weapon_type);
void FireShotgun();
void FireHK(int running);
void FireCrossbow(PHD_3DPOS* Start);
void ControlCrossbow(short item_number);
void draw_shotgun(int weapon_type);
void undraw_shotgun(int weapon_type);
void AnimateShotgun(int weapon_type);
void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void TriggerGrapplingEffect(long x, long y, long z);
void CrossbowHitSwitchType78(ITEM_INFO* item, ITEM_INFO* target, int MustHitLastNode);
