#pragma once
#include "../global/vars.h"

void inject_input();

int Key(int number);

#define S_UpdateInput	( (int(__cdecl*)()) 0x004A92D0 )


enum IKEYS
{
	KEY_FORWARD = 0,
	KEY_BACK = 1,
	KEY_LEFT = 2,
	KEY_RIGHT = 3,
	KEY_DUCK = 4,
	KEY_SPRINT = 5,
	KEY_WALK = 6,
	KEY_JUMP = 7,
	KEY_ACTION = 8,
	KEY_DRAW = 9,
	KEY_FLARE = 10,
	KEY_LOOK = 11,
	KEY_ROLL = 12,
	KEY_OPTION = 13,
	KEY_STEPL = 14,
	KEY_STEPR = 15,
	KEY_PAUSE = 16,
	KEY_SELECT = 17,
};

