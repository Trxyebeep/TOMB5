#include "../tomb5/pch.h"
#include "lighting.h"
#include "3dmath.h"

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

void SuperSetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient)
{
	SUNLIGHT_STRUCT* sun;
	POINTLIGHT_STRUCT* point;
	float x, y, z, num, num2;
	long aR, aG, aB, val, val2;

	if (light->Type == LIGHT_SUN)
	{
		sun = &SunLights[NumSunLights];
		x = (float)light->inx;
		y = (float)light->iny;
		z = (float)light->inz;
		num = 1.0F / (float)(SQUARE(z) + SQUARE(y) + SQUARE(x));
		sun->vec.x = (aLightMatrix._11 * x + aLightMatrix._12 * y + aLightMatrix._13 * z) * num;
		sun->vec.y = (aLightMatrix._21 * x + aLightMatrix._22 * y + aLightMatrix._23 * z) * num;
		sun->vec.z = (aLightMatrix._31 * x + aLightMatrix._32 * y + aLightMatrix._33 * z) * num;
		sun->r = light->r * 255.0F;
		sun->g = light->g * 255.0F;
		sun->b = light->b * 255.0F;
		NumSunLights++;
		TotalNumLights++;
	}
	else if (light->Type == LIGHT_POINT)
	{
		x = light->x - lGlobalMeshPos.x;
		y = light->y - lGlobalMeshPos.y;
		z = light->z - lGlobalMeshPos.z;
		point = &PointLights[NumPointLights];
		num2 = sqrt(SQUARE(z) + SQUARE(y) + SQUARE(x));
		num = 1.0F / num2;
		point->vec.x = (aLightMatrix._11 * x + aLightMatrix._12 * y + aLightMatrix._13 * z) * num;
		point->vec.y = (aLightMatrix._21 * x + aLightMatrix._22 * y + aLightMatrix._23 * z) * num;
		point->vec.z = (aLightMatrix._31 * x + aLightMatrix._32 * y + aLightMatrix._33 * z) * num;
		point->r = light->r * 255.0F;
		point->g = light->g * 255.0F;
		point->b = light->b * 255.0F;
		point->rad = (light->Outer - num2) / light->Outer;
		NumPointLights++;
		TotalNumLights++;
	}
	else if (light->Type == LIGHT_SHADOW)
	{
		aR = CLRR(*ambient);
		aG = CLRG(*ambient);
		aB = CLRB(*ambient);
		val = phd_sqrt(light->Range);
		val2 = light->shadow >> 3;

		if (val >= light->Inner)
			val2 = (long)((val - light->Outer) / ((light->Outer - light->Inner) / -val2));

		val = val2 >> 1;
		aR -= val;
		aG -= val;
		aB -= val;

		if (aR < 0)
			aR = 0;

		if (aG < 0)
			aG = 0;

		if (aB < 0)
			aB = 0;

		*ambient = RGBONLY(aR, aG, aB);
		NumShadowLights++;
	}
}

void inject_lighting(bool replace)
{
	INJECT(0x004AB7A0, InitObjectLighting, replace);
	INJECT(0x004AAFE0, SuperSetupLight, replace);
}
