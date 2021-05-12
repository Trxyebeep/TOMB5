#pragma once
#include "../global/vars.h"

void inject_laraflar();

void FlareControl(short item_number);
void undraw_flare_meshes();
void draw_flare_meshes();
void set_flare_arm(int frame);
void DoFlareInHand(int flare_age);
int DoFlareLight(PHD_VECTOR* pos, int flare_age);
void draw_flare();
void CreateFlare(short object, int thrown);
void ready_flare();
void undraw_flare();
void DrawFlareInAir(ITEM_INFO* item);
