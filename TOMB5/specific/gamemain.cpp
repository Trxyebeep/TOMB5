#include "../tomb5/pch.h"
#include "gamemain.h"
#include "function_stubs.h"
#include "audio.h"
#include "file.h"
#include "function_table.h"
#include "3dmath.h"
#include "../game/text.h"
#include "time.h"
#include "winmain.h"
#include "../game/sound.h"
#include "../game/gameflow.h"
#include "dxshell.h"
#include "specificfx.h"
#ifdef GENERAL_FIXES
#include "../game/savegame.h"
#include "../tomb5/tomb5.h"
#endif

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

		wsprintf(buffer, "%s", SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]));
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
#ifdef GENERAL_FIXES
		WriteFile(file, &tomb5_save, sizeof(tomb5_save_info), &bytes, NULL);
#endif
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
#ifdef GENERAL_FIXES
		ReadFile(file, &tomb5_save, sizeof(tomb5_save_info), &tomb5_save_size, NULL);
#endif
		CloseHandle(file);
		return 1;
	}

	return 0;
}

void GameClose()
{
	Log(2, "GameClose");
	ACMClose();
	FreeLevel();

	if (DestVB)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Dest VB", DestVB, DestVB->Release());
		DestVB = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Dest VB");

	FREE(clipflags);

	if (wav_file_buffer)
		FREE(wav_file_buffer);

	if (ADPCMBuffer)
		FREE(ADPCMBuffer);

	FREE(malloc_buffer);
	FREE(gfScriptFile);
	FREE(gfLanguageFile);
}

unsigned int __stdcall GameMain(void* ptr)
{
	long fpcw;

	Log(2, "GameMain");

	if (GameInitialise())
	{
		InitialiseFunctionTable();
		HWInitialise();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		TIME_Init();
		App.SetupComplete = 1;
		fpcw = MungeFPCW(&FPCW);
		S_CDStop();
		ClearSurfaces();

		if (!App.SoundDisabled)
			SOUND_Init();

#ifdef GENERAL_FIXES	//Better here than in DoGameflow
		RPC_Init();
		init_tomb5_stuff();
#endif

		DoGameflow();
		GameClose();
		S_CDStop();

#ifdef GENERAL_FIXES
		RPC_close();
#endif

		if (fpcw)
			RestoreFPCW(FPCW);

		PostMessage(App.hWnd, WM_CLOSE, 0, 0);
		MainThread.active = 0;
		_endthreadex(1);
	}

	return 1;
}

bool GameInitialise()
{
	D3DVERTEXBUFFERDESC desc;

	Log(2, "GameInitialise");
	desc.dwCaps = 0;
	desc.dwSize = sizeof(desc);
	desc.dwFVF = D3DFVF_TLVERTEX;
	desc.dwNumVertices = 0x2000;
	DXAttempt(App.dx.lpD3D->CreateVertexBuffer(&desc, &DestVB, D3DDP_DONOTCLIP, 0));
	init_game_malloc();
	clipflags = (short*)MALLOC(0x8000);
	init_water_table();
	aInitFX();
	return 1;
}

void inject_gamemain(bool replace)
{
	INJECT(0x004A8B70, GetRandom, replace);
	INJECT(0x004A8880, init_water_table, replace);
	INJECT(0x004A8BC0, S_SaveGame, replace);
	INJECT(0x004A8E10, S_LoadGame, replace);
	INJECT(0x004A8790, GameClose, replace);
	INJECT(0x004A84F0, GameMain, replace);
	INJECT(0x004A8610, GameInitialise, replace);
}
