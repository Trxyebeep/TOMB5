#pragma once
#include "../global/vars.h"

void inject_cutseq();

void _special1_control();
void _special1_end();
void _special2_control();
void _special2_end();
void _special3_control();
void _special3_end();
void _special4_control();
void _special4_end();
void ResetCutItem(int item_num);
void resetwindowsmash(int item_num);
void triggerwindowsmash(int item_num);
void FlamingHell(PHD_VECTOR* pos);
void FireTwoGunTitleWeapon(PHD_VECTOR* pos1, PHD_VECTOR* pos2);
