#pragma once
#include "../global/types.h"

void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);
void ready_shotgun(long weapon_type);
void RifleHandler(long weapon_type);
void FireShotgun();
void FireHK(long running);
void FireCrossbow(PHD_3DPOS* Start);
void ControlCrossbow(short item_number);
void draw_shotgun(long weapon_type);
void undraw_shotgun(long weapon_type);
void AnimateShotgun(long weapon_type);
void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void TriggerGrapplingEffect(long x, long y, long z);
void CrossbowHitSwitchType78(ITEM_INFO* item, ITEM_INFO* target, long MustHitLastNode);
void TriggerUnderwaterExplosion(ITEM_INFO* item);
