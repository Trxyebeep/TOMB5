#include "../tomb5/pch.h"
#include "tomb5.h"
#include "../specific/registry.h"

tomb5_options tomb5;

void init_tomb5_stuff()
{
	char buf[40];
	bool first;

	OpenRegistry("tomb5");
	first = REG_KeyWasCreated();

	if (first)	//key was created = no settings found, write defaults
	{
		sprintf(buf, "footprints");
		tomb5.footprints = 1;							//footprints on
		REG_WriteBool(buf, tomb5.footprints);

		sprintf(buf, "point_lights");
		tomb5.tr4_point_lights = 0;						//TR5 points
		REG_WriteBool(buf, tomb5.tr4_point_lights);		

		sprintf(buf, "shadow");
		tomb5.shadow_mode = 2;							//PSX like shadow
		REG_WriteLong(buf, tomb5.shadow_mode);

		sprintf(buf, "fog");
		tomb5.fog = 1;									//on
		REG_WriteBool(buf, tomb5.fog);
	}
	else	//Key already exists, settings already written, read them. also falls back to default if a smartass manually deletes a single value
	{
		sprintf(buf, "footprints");
		REG_ReadBool(buf, tomb5.footprints, 1);

		sprintf(buf, "point_lights");
		REG_ReadBool(buf, tomb5.tr4_point_lights, 0);

		sprintf(buf, "shadow");
		REG_ReadLong(buf, tomb5.shadow_mode, 2);

		sprintf(buf, "fog");
		REG_ReadBool(buf, tomb5.fog, 1);
	}

	App.Volumetric = tomb5.fog;

	CloseRegistry();
}

void save_new_tomb5_settings()
{
	char buf[40];
	OpenRegistry("tomb5");

	sprintf(buf, "footprints");
	REG_WriteBool(buf, tomb5.footprints);

	sprintf(buf, "point_lights");
	REG_WriteBool(buf, tomb5.tr4_point_lights);

	sprintf(buf, "shadow");
	REG_WriteLong(buf, tomb5.shadow_mode);

	sprintf(buf, "fog");
	REG_WriteBool(buf, tomb5.fog);
	App.Volumetric = tomb5.fog;

	CloseRegistry();
}
