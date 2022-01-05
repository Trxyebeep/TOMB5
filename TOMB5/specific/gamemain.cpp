#include "../tomb5/pch.h"
#include "gamemain.h"

uchar water_abs[4] = { 4, 8, 12, 16 };
short water_shimmer[4] = { 31, 63, 95, 127 };
short water_choppy[4] = { 16, 53, 90, 127 };

ushort GetRandom(WATERTAB* wt, long lp)
{
	long loop;
	ushort ret;

	do
	{
		ret = rand() & 0xFC;
		
		for (loop = 0; loop < lp; loop++)
			if (wt[loop].random == ret)
				break;

	} while (loop != lp);

	return ret;
}

void init_water_table()
{
	float fSin;
	short sSin;

	srand(121197);

	for (int i = 0; i < 64; i++)
	{
		sSin = rcossin_tbl[i << 7];
		WaterTable[0][i].shimmer = (63 * sSin) >> 15;
		WaterTable[0][i].choppy = (16 * sSin) >> 12;
		WaterTable[0][i].random = (uchar)GetRandom(&WaterTable[0][0], i);
		WaterTable[0][i].abs = 0;
		WaterTable[1][i].shimmer = (32 * sSin) >> 15;
		WaterTable[1][i].choppy = 0;
		WaterTable[1][i].random = (uchar)GetRandom(&WaterTable[1][0], i);
		WaterTable[1][i].abs = -3;
		WaterTable[2][i].shimmer = (64 * sSin) >> 15;
		WaterTable[2][i].choppy = 0;
		WaterTable[2][i].random = (uchar)GetRandom(&WaterTable[2][0], i);
		WaterTable[2][i].abs = 0;
		WaterTable[3][i].shimmer = (96 * sSin) >> 15;
		WaterTable[3][i].choppy = 0;
		WaterTable[3][i].random = (uchar)GetRandom(&WaterTable[3][0], i);
		WaterTable[3][i].abs = 4;
		WaterTable[4][i].shimmer = (127 * sSin) >> 15;
		WaterTable[4][i].choppy = 0;
		WaterTable[4][i].random = (uchar)GetRandom(&WaterTable[4][0], i);
		WaterTable[4][i].abs = 8;

		for (int j = 0, k = 5; j < 4; j++, k += 4)
		{
			for (int m = 0; m < 4; m++)
			{
				WaterTable[k + m][i].shimmer = -((sSin * water_shimmer[m]) >> 15);
				WaterTable[k + m][i].choppy = sSin * water_choppy[j] >> 12;
				WaterTable[k + m][i].random = (uchar)GetRandom(&WaterTable[k + m][0], i);
				WaterTable[k + m][i].abs = water_abs[m];
			}
		}
	}

	for (int i = 0; i < 32; i++)
	{
		fSin = sin(i * 0.19634955F);
		vert_wibble_table[i] = fSin + fSin;
	}

	for (int i = 0; i < 128; i++)
	{
		fSin = sin(i * 0.024543693F);
		unused_vert_wibble_table[i] = fSin * 32;
	}
}

long S_SaveGame(long slot_num)
{
	HANDLE file;
	ulong bytes;
	long days, hours, minutes, seconds;
	char buffer[80], counter[16];

	for (int i = 0; i < 20; i++)
		buffer[i] = '\0';

	wsprintf(buffer, "savegame.%d", slot_num);
	file = CreateFile(buffer, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file != INVALID_HANDLE_VALUE)
	{
		for (int i = 0; i < 20; i++)
			buffer[i] = '\0';

		wsprintf(buffer, "%s", &gfStringWad[gfStringOffset_bis[gfLevelNames[gfCurrentLevel]]]);
		WriteFile(file, buffer, 75, &bytes, NULL);
		WriteFile(file, &save_counter, sizeof(long), &bytes, NULL);
		days = savegame.Game.Timer / 30 / 86400;
		hours = savegame.Game.Timer / 30 % 86400 / 3600;
		minutes = savegame.Game.Timer / 30 / 60 % 60;
		seconds = savegame.Game.Timer / 30 % 60;
		WriteFile(file, &days, 2, &bytes, NULL);
		WriteFile(file, &hours, 2, &bytes, NULL);
		WriteFile(file, &minutes, 2, &bytes, NULL);
		WriteFile(file, &seconds, 2, &bytes, NULL);
		WriteFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, NULL);
		CloseHandle(file);
		wsprintf(counter, "%d", save_counter);
		save_counter++;
		return 1;
	}

	return 0;
}

long S_LoadGame(long slot_num)
{
	HANDLE file;
	ulong bytes;
	long value;
	char buffer[80];

	wsprintf(buffer, "savegame.%d", slot_num);
	file = CreateFile(buffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file != INVALID_HANDLE_VALUE)
	{
		ReadFile(file, buffer, 75, &bytes, NULL);
		ReadFile(file, &value, sizeof(long), &bytes, NULL);
		ReadFile(file, &value, sizeof(long), &bytes, NULL);
		ReadFile(file, &value, sizeof(long), &bytes, NULL);
		ReadFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, NULL);
		CloseHandle(file);
		return 1;
	}

	return 0;
}

void inject_gamemain(bool replace)
{
	INJECT(0x004A8B70, GetRandom, replace);
	INJECT(0x004A8880, init_water_table, replace);
	INJECT(0x004A8BC0, S_SaveGame, replace);
	INJECT(0x004A8E10, S_LoadGame, replace);
}
