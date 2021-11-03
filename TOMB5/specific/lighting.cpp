#include "../tomb5/pch.h"
#include "lighting.h"

void InitObjectLighting(ITEM_INFO* item)
{
	PCLIGHT* light;
	long node_ambient;

	node_ambient = item->il.ambient;
	SetupLight_thing = 0;
	light = (PCLIGHT*)item->il.pCurrentLights;

	for (int i = 0; i < item->il.nCurrentLights; i++)
		if (light[i].Active)
			SuperSetupLight(&light[i], item, &node_ambient);

	light = (PCLIGHT*)item->il.pPrevLights;

	for (int i = 0; i < item->il.nPrevLights; i++)
		if (light[i].Active)
			SuperSetupLight(&light[i], item, &node_ambient);

	InitDynamicLighting(item);

	if (item == lara_item && bLaraInWater)
	{
#ifdef GENERAL_FIXES	//fixes lara's node ambient in water
		if (bLaraUnderWater < 0)
#else
		if (unused_bLaraUnderWater < 0)
#endif
		{
			node_ambient = LaraNodeAmbient[0];
			item->il.fcnt = 0;
			item->il.ambient = node_ambient;
		}
		else
			node_ambient = LaraNodeAmbient[1];
	}
	else
		node_ambient = item->il.ambient;

	aAmbientR = CLRR(node_ambient);
	aAmbientG = CLRG(node_ambient);
	aAmbientB = CLRB(node_ambient);

	if (aAmbientR < 16)
		aAmbientR = 16;

	if (aAmbientG < 16)
		aAmbientG = 16;

	if (aAmbientB < 16)
		aAmbientB = 16;
}

void inject_lighting(bool replace)
{
	INJECT(0x004AB7A0, InitObjectLighting, replace);
}
