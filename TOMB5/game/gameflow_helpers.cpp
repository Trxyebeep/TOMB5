#include "../tomb5/pch.h"
#include "gameflow_helpers.h"
#include "gameflow.h"

int GetCampaignCheatValue()
{
	static int counter = 0;
	static int timer;
	int jump;

	if (timer)
		timer--;
	else
		counter = 0;

	jump = 0;

	switch (counter)
	{
	case 0:

		if (keymap[33])//F
		{
			timer = 450;
			counter = 1;
		}

		break;

	case 1:
		if (keymap[23])//I
			counter = 2;

		break;

	case 2:
		if (keymap[38])//L
			counter = 3;

		break;

	case 3:
		if (keymap[20])//T
			counter = 4;

		break;

	case 4:
		if (keymap[35])//H
			counter = 5;

		break;

	case 5:
		if (keymap[21])//Y
			counter = 6;

		break;

	case 6:
		if (keymap[2])//1, not the numpad
			jump = LVL5_STREETS_OF_ROME;

		if (keymap[3])//2, not the numpad
			jump = LVL5_BASE;

		if (keymap[4])//3, not the numpad
			jump = LVL5_GALLOWS_TREE;

		if (keymap[5])//4, not the numpad
			jump = LVL5_THIRTEENTH_FLOOR;

		if (jump)
		{
			counter = 0;
			timer = 0;
		}

		break;
	}

	return jump;
}

void inject_gf_helpers()
{
	INJECT(0x004B1F00, GetCampaignCheatValue);
}
