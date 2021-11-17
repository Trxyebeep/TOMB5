#include "../tomb5/pch.h"
#include "tomb5.h"

tomb5_options tomb5;

void init_tomb5_stuff()	//currently just defaulting to my choice of settings but will read from registry someday
{
	tomb5.footprints = 1;					//footprints on
	tomb5.tr4_point_lights = 0;				//TR5 points
	tomb5.shadow_mode = 2;					//PSX like shadow
}
