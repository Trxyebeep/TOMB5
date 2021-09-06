#include "../tomb5/pch.h"
#include "objlight.h"
#include "control.h"
#include "effect2.h"

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, long angle, int room_no, int falloff)
{
	GAME_VECTOR src, target;
	long sin, cos;

	src.x = x;
	src.y = y;
	src.z = z;
	GetFloor(x, y, z, (short*) &room_no);
	src.room_number = room_no;
	sin = phd_sin(16 * angle);
	cos = phd_cos(16 * angle);
	target.x = x + sin;
	target.y = y;
	target.z = z + cos;

	if (!LOS(&src, &target))
		TriggerDynamic(target.x, target.y, target.z, falloff, r, g, b);
}

void inject_objlight(bool replace)
{
	INJECT(0x00466B20, TriggerAlertLight, replace);
}