#pragma once
#include "../global/types.h"

void FlareControl(short item_number);
void undraw_flare_meshes();
void draw_flare_meshes();
void set_flare_arm(long frame);
void DoFlareInHand(long flare_age);
long DoFlareLight(PHD_VECTOR* pos, long flare_age);
void draw_flare();
void CreateFlare(short object, long thrown);
void ready_flare();
void undraw_flare();
void DrawFlareInAir(ITEM_INFO* item);
