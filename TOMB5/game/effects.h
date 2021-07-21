#pragma once
#include "../global/vars.h"

void inject_effects(bool replace);


extern void(*effect_routines[])(ITEM_INFO* item);

void SoundEffects();
void WaterFall(short item_number);
void void_effect(ITEM_INFO* item);
void turn180_effect(ITEM_INFO* item);
void floor_shake_effect(ITEM_INFO* item);
void PoseidonSFX(ITEM_INFO* item);
void finish_level_effect(ITEM_INFO* item);
void ActivateCamera(ITEM_INFO* item);
void ActivateKey(ITEM_INFO* item);
void RubbleFX(ITEM_INFO* item);
void SwapCrowbar(ITEM_INFO* item);
void SoundFlipEffect(ITEM_INFO* item);
void ExplosionFX(ITEM_INFO* item);
void lara_hands_free(ITEM_INFO* item);
void shoot_right_gun(ITEM_INFO* item);
void shoot_left_gun(ITEM_INFO* item);
void invisibility_on(ITEM_INFO* item);
void invisibility_off(ITEM_INFO* item);
void reset_hair(ITEM_INFO* item);
void SetFog(ITEM_INFO* item);
void LaraLocation(ITEM_INFO* item);
void ClearSpidersPatch(ITEM_INFO* item);
void ResetTest(ITEM_INFO* item);
void LaraLocationPad(ITEM_INFO* item);
void KillActiveBaddies(ITEM_INFO* item);
void BaddieBiteEffect(ITEM_INFO* item, BITE_INFO* bite);
void TL_1(ITEM_INFO* item);
void TL_2(ITEM_INFO* item);
void TL_3(ITEM_INFO* item);
void TL_4(ITEM_INFO* item);
void TL_5(ITEM_INFO* item);
void TL_6(ITEM_INFO* item);
void TL_7(ITEM_INFO* item);
void TL_8(ITEM_INFO* item);
void TL_9(ITEM_INFO* item);
void TL_10(ITEM_INFO* item);
void TL_11(ITEM_INFO* item);
void TL_12(ITEM_INFO* item);
short DoBloodSplat(long x, long y, long z, short random, short y_rot, short room_number);

#define WadeSplash	( (void(__cdecl*)(ITEM_INFO*, int, int)) 0x00432A30 )
#define Splash	( (void(__cdecl*)(ITEM_INFO*)) 0x00432900 )
#define ItemNearLara	( (int(__cdecl*)(PHD_3DPOS*, long)) 0x00432580 )
