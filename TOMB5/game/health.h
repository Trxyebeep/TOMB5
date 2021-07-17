#pragma once
#include "../global/vars.h"


void inject_health(bool replace);


int FlashIt();
void DrawGameInfo(int timed);
void DrawHealthBar(int flash_state);
void DrawAirBar(int flash_state);
void MakeAmmoString(char* string);
void InitialisePickUpDisplay();
void DrawPickups(int timed);
void AddDisplayPickup(short object_number);
