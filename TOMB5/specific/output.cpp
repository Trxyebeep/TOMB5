#include "../tomb5/pch.h"
#include "output.h"
#include "../specific/3dmath.h"
#include "../game/newinv2.h"
#include "../specific/matrix_shit.h"

void S_DrawPickup(short object_number)
{
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	SetD3DViewMatrix();
	aSetViewMatrix();
	DrawThreeDeeObject2D((phd_winxmax * 0.001953125 * 448.0 + PickupX), (phd_winymax * 0.00390625 * 216.0), convert_obj_to_invobj(object_number),
		128, 0, (GnFrameCounter & 0x7F) << 9, 0, 0, 1);
}

void inject_output()
{
	INJECT(0x004B78D0, S_DrawPickup);
}

