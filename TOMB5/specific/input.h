#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);
long S_UpdateInput();
long ReadJoystick(long& x, long& y);

#ifdef GENERAL_FIXES
extern short ammo_change_timer;
extern char ammo_change_buf[12];
#endif

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

