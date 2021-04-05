#pragma once
#include "../tomb5/pch.h"
#include "gameflow.h"
#include "../global/types.h"
#include "../specific/game.h"

void DoGameflow()
{
	unsigned char* gf;
	unsigned char n;
	unsigned char* m;

	do_boot_screen(Gameflow->Language);
	num_fmvs = 0;
	fmv_to_play[1] = 0;
	fmv_to_play[0] = 0;
	gfCurrentLevel = Gameflow->TitleEnabled == 0;
	gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

	while (1)
	{
		switch (n = *gf++)
		{
		case GF_FMV:
			fmv_to_play[num_fmvs++] = *gf++;
			continue;

		case GF_LEVEL:
			gfLevelFlags = gf[1] | (gf[2] << 8);

			if (!(gfLevelFlags & GF_LVOP_NO_LEVEL))
				DoLevel(gf[3], gf[4]);
			else
			{
				gfStatus = 999;
				gfCurrentLevel++;
			}

			gfLegendTime = 0;
			LaserSight = 0;
			BinocularRange = 0;
			*(int*)gfResidentCut = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfNumPickups = 0;
			gfMirrorRoom = -1;
			gfFogColour.r = 0;
			gfFogColour.g = 0;
			gfFogColour.b = 0;
			gfFogColour.cd = 0;

			if (gfStatus != 2)
			{
				if (gfStatus < 3)
				{
					if (gfStatus == 1)
					{
						gfInitialiseGame = 1;
						gfCurrentLevel = Gameflow->TitleEnabled == 0;
					}
				}
				else
				{
					if (gfStatus == 3)
					{
						if (!Gameflow->DemoDisc && Gameflow->nLevels != 2 && gfLevelComplete <= Gameflow->nLevels)
							gfCurrentLevel = gfLevelComplete;
						else
							gfCurrentLevel = 0;
					}
					else if (gfStatus == 4)
						return;
				}
			}
			else
			{
				gfGameMode = 4;
				gfCurrentLevel = savegame.CurrentLevel;
			}

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

			break;
		case GF_TITLE_LEVEL:
			gfLevelFlags = gf[0] | (gf[1] << 8);
			DoTitle(gf[2], gf[3]);
			gfMirrorRoom = -1;
			*(int*)gfResidentCut = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfNumPickups = 0;

			if (gfStatus == 3)
			{
				gfGameMode = 0;
				gfCurrentLevel = gfLevelComplete;
				gfInitialiseGame = 1;
			}
			else if (gfStatus < 4)
			{
				if (gfStatus == 2)
				{
					gfGameMode = 4;
					gfCurrentLevel = savegame.CurrentLevel;
				}
			}
			else if (gfStatus == 4)
				return;

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

			break;
		case GF_LEVEL_DATA_END:
			continue;

		case GF_CUT:
			gfCutNumber = *gf++;
			continue;

		case GF_RESIDENTCUT1:
			gfResidentCut[0] = *gf++;
			continue;

		case GF_RESIDENTCUT2:
			gfResidentCut[1] = *gf++;
			continue;

		case GF_RESIDENTCUT3:
			gfResidentCut[2] = *gf++;
			continue;

		case GF_RESIDENTCUT4:
			gfResidentCut[3] = *gf++;
			continue;

		case GF_LAYER1:
			LightningRGB[0] = *gf;
			LightningRGBs[0] = *gf;
			gfLayer1Col.r = *gf++;
			LightningRGB[1] = *gf;
			LightningRGBs[1] = *gf;
			gfLayer1Col.g = *gf++;
			LightningRGB[2] = *gf;
			LightningRGBs[2] = *gf;
			gfLayer1Col.b = *gf++;
			gfLayer1Vel = *gf++;
			continue;

		case GF_LAYER2:
			LightningRGB[0] = *gf;
			LightningRGBs[0] = *gf;
			gfLayer2Col.r = *gf++;
			LightningRGB[1] = *gf;
			LightningRGBs[1] = *gf;
			gfLayer2Col.g = *gf++;
			LightningRGB[2] = *gf;
			LightningRGBs[2] = *gf;
			gfLayer2Col.b = *gf++;
			gfLayer2Vel = *gf++;
			continue;

		case GF_UV_ROTATE:
			gfUVRotate = *gf++;
			continue;

		case GF_LEGEND:
			gfLegend = *gf++;

			if (gfGameMode != 4)
				gfLegendTime = 150;

			continue;

		case GF_LENS_FLARE:
			gfLensFlare.x = ((gf[1] << 8) | gf[0]) << 8;
			gfLensFlare.y = ((gf[2] | (gf[3] << 8)) << 16) >> 8;
			gfLensFlare.z = ((gf[5] << 8) | gf[4]) << 8;
			gfLensFlareColour.r = gf[6];
			gfLensFlareColour.g = gf[7];
			gfLensFlareColour.b = gf[8];
			gf += 9;
			continue;

		case GF_MIRROR:
			gfMirrorRoom = *gf;
			gfMirrorZPlane = (gf[4] << 24) | (gf[3] << 16) | (gf[2] << 8) | gf[1];
			gf += 5;
			continue;

		case GF_FOG:
			m = gf++;
			gfFogColour.b = *gf;
			gfFogColour.g = *(char*)(m)++;
			gfFogColour.r = *(char*)(gf++);
			gf = (unsigned __int8*)((char*)(gf++) + 1);
			continue;

		case GF_ANIMATING_MIP:
			gfMips[gfNumMips++] = *gf++;
			continue;

		case GF_RESET_HUB:
			gfResetHubDest = *gf++;
			continue;

		case GF_GIVE_ITEM_AT_STARTUP:
			gfPickups[gfNumPickups++] = *gf;
			gf += 2;
			continue;

		case GF_LOSE_ITEM_AT_STARTUP:
			gfTakeaways[gfNumTakeaways++] = *gf;
			gf += 2;
			continue;

		default:

			if (n >= GF_KEY_ITEM1 && n <= GF_KEY_ITEM8)
				n -= 93;
			else if (n >= GF_PUZZLE_ITEM1 && n <= GF_PUZZLE_ITEM8)
				n += 127;
			else if (n >= GF_PICKUP_ITEM1 && n <= GF_PICKUP_ITEM4)
				n -= 93;
			else if (n >= GF_EXAMINE1 && n <= GF_EXAMINE3)
				n -= 83;
			else if (n >= GF_KEY_ITEM1 && n <= GF_KEY_ITEM8_COMBO2)
				n -= 116;
			else if (n >= GF_PUZZLE_ITEM1_COMBO1 && n <= GF_PUZZLE_ITEM8_COMBO2)
				n += 100;
			else if (n >= GF_PICKUP_ITEM1_COMBO1 && n <= GF_PICKUP_ITEM4_COMBO2)
				n -= 128;

			inventry_objects_list[n].objname = gf[0] | (gf[1] << 8);
			inventry_objects_list[n].yoff = gf[2] | (gf[3] << 8);
			inventry_objects_list[n].scale1 = gf[4] | (gf[5] << 8);
			inventry_objects_list[n].yrot = gf[6] | (gf[7] << 8);
			inventry_objects_list[n].xrot = gf[8] | (gf[9] << 8);
			inventry_objects_list[n].zrot = gf[10] | (gf[11] << 8);
			inventry_objects_list[n].flags = gf[12] | (gf[13] << 8);
			gf += 14;
			continue;
		}
	}
}

void inject_gameflow()
{
	INJECT(0x004354B0, DoGameflow);
}
