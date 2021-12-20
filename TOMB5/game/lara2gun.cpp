#include "../tomb5/pch.h"
#include "lara2gun.h"
#include "larafire.h"

void ready_pistols(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
}

void inject_lara2gun(bool replace)
{
	INJECT(0x0044FDD0, ready_pistols, replace);
}
