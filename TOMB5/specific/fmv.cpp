#include "../tomb5/pch.h"
#include "fmv.h"
#include "dxshell.h"
#include "audio.h"
#include "function_stubs.h"
#include "lighting.h"
#include "function_table.h"
#include "winmain.h"
#include "input.h"
#include "3dmath.h"
#include "../game/text.h"
#include "d3dmatrix.h"
#include "dxsound.h"
#include "gamemain.h"
#include "LoadSave.h"
#include "setupdlg.h"

#define GET_DLL_PROC(dll, proc, n) \
{ \
	*(FARPROC *)&(proc) = GetProcAddress((dll), n); \
	if(!proc) throw #proc; \
}

static void(__stdcall* BinkCopyToBuffer)(BINK_STRUCT*, LPVOID, LONG, long, long, long, long);
static void(__stdcall* BinkOpenDirectSound)(ulong);
static void(__stdcall* BinkSetSoundSystem)(LPVOID, LPDIRECTSOUND);
static LPVOID(__stdcall* BinkOpen)(char*, ulong);
static long(__stdcall* BinkDDSurfaceType)(LPDIRECTDRAWSURFACE4);
static long(__stdcall* BinkDoFrame)(BINK_STRUCT*);
static void(__stdcall* BinkNextFrame)(BINK_STRUCT*);
static long(__stdcall* BinkWait)(BINK_STRUCT*);
static void(__stdcall* BinkClose)(BINK_STRUCT*);
static HMODULE hBinkW32;

static long nFmvFrames[9] = { 880, 1826, 3869, 3112, 1903, 1973, 3200, 2799, 1725 };

static BINK_STRUCT* Bink;
static LPDIRECTDRAWSURFACE4 BinkSurface;
static long BinkSurfaceType;

bool LoadBinkStuff()
{
	hBinkW32 = LoadLibrary("binkw32.dll");

	if (!hBinkW32)
		return 0;

	try
	{
		GET_DLL_PROC(hBinkW32, BinkCopyToBuffer, "_BinkCopyToBuffer@28");
		GET_DLL_PROC(hBinkW32, BinkOpenDirectSound, "_BinkOpenDirectSound@4");
		GET_DLL_PROC(hBinkW32, BinkSetSoundSystem, "_BinkSetSoundSystem@8");
		GET_DLL_PROC(hBinkW32, BinkOpen, "_BinkOpen@8");
		GET_DLL_PROC(hBinkW32, BinkDDSurfaceType, "_BinkDDSurfaceType@4");
		GET_DLL_PROC(hBinkW32, BinkDoFrame, "_BinkDoFrame@4");
		GET_DLL_PROC(hBinkW32, BinkNextFrame, "_BinkNextFrame@4");
		GET_DLL_PROC(hBinkW32, BinkWait, "_BinkWait@4");
		GET_DLL_PROC(hBinkW32, BinkClose, "_BinkClose@4");
	}
	catch (LPCTSTR)
	{
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
		return 0;
	}

	return 1;
}

void FreeBinkStuff()
{
	if (hBinkW32)
	{
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
	}
}

void ShowBinkFrame()
{
	DDSURFACEDESC2 surf;

	memset(&surf, 0, sizeof(surf));
	surf.dwSize = sizeof(DDSURFACEDESC2);
	DXAttempt(BinkSurface->Lock(0, &surf, DDLOCK_NOSYSLOCK, 0));
	BinkCopyToBuffer(Bink, surf.lpSurface, surf.lPitch, Bink->num, 0, 0, BinkSurfaceType);
	DXAttempt(BinkSurface->Unlock(0));

	if (App.dx.Flags & DXF_WINDOWED)
		DXShowFrame();
}

long PlayFmv(long num)
{
	DXDISPLAYMODE* modes;
	DXDISPLAYMODE* current;
	long dm, rm, ndms;
	char name[80];
	char path[80];

	if (MainThread.ended)
		return 0;

	/*
	if ((1 << num) & FmvSceneTriggered)
		return 1;

	FmvSceneTriggered |= 1 << num;
	*/
	S_CDStop();

	if (fmvs_disabled)
		return 0;

	sprintf(name, "movie\\fmv%01d.bik", num);
	memset(path, 0, sizeof(path));
	strcat(path, name);
	Log("PlayFMV %s", path);
	App.fmv = 1;
	modes = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;
	rm = 0;
	dm = App.DXInfo.nDisplayMode;
	current = &modes[dm];

	if (current->bpp != 16 || current->w != 640 || current->h != 480)
	{
		ndms = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes;

		for (int i = 0; i < ndms; i++, modes++)
		{
			if (modes->bpp == 16 && modes->w == 640 && modes->h == 480)
			{
				App.DXInfo.nDisplayMode = i;
				break;
			}
		}

		DXChangeVideoMode();
		HWInitialise();
		ClearSurfaces();
		rm = 1;
	}

	Bink = 0;
	BinkSetSoundSystem(BinkOpenDirectSound, App.dx.lpDS);
	Bink = (BINK_STRUCT*)BinkOpen(path, 0);

	if (App.dx.Flags & DXF_WINDOWED)
		BinkSurface = App.dx.lpBackBuffer;
	else
		BinkSurface = App.dx.lpPrimaryBuffer;

	BinkSurfaceType = BinkDDSurfaceType(BinkSurface);

	if (Bink)
	{
		Log("Entering Bink Loop");
		BinkDoFrame(Bink);
		S_UpdateInput();

		for (int i = 0; i != nFmvFrames[num]; i++)
		{
			if (input & IN_OPTION || input & IN_DRAW || MainThread.ended)
				break;

			BinkNextFrame(Bink);

			while (BinkWait(Bink));

			ShowBinkFrame();
			BinkDoFrame(Bink);
			S_UpdateInput();
		}


		BinkClose(Bink);
		Bink = 0;
	}
	else
		Log("FAILED TO CREATE BINK OBJECT");

	if (rm)
	{
		App.DXInfo.nDisplayMode = dm;
		DXChangeVideoMode();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		S_InitD3DMatrix();
		aSetViewMatrix();
	}

	DXChangeOutputFormat(sfx_frequencies[SoundQuality], 1);
	HWInitialise();
	ClearSurfaces();
	App.fmv = 0;
	return 0;
}

long PlayFmvNow(long num, long u)
{
	switch (num)
	{
	case 0:
		PlayFmv(0);
		break;

	case 1:
		PlayFmv(1);
		break;

	case 2:
		PlayFmv(2);
		break;

	case 3:
		PlayFmv(3);
		PlayFmv(4);
		break;

	case 4:
		PlayFmv(5);
		PlayFmv(6);
		break;

	case 5:
		PlayFmv(7);
		break;

	case 6:
		PlayFmv(8);
		break;
	}

	return 0;
}
