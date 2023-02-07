#include "../tomb5/pch.h"
#include "cmdline.h"
#include "function_stubs.h"
#include "../game/gameflow.h"

void CLSetup(char* cmd)
{
	Log(2, "CLSetup");

	if (cmd)
		start_setup = 0;
	else
		start_setup = 1;
}

void CLNoFMV(char* cmd)
{
	Log(2, "CLNoFMV");

	if (cmd)
		fmvs_disabled = 0;
	else
		fmvs_disabled = 1;
}

void InitDSDevice(HWND dlg, HWND hwnd)
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < App.DXInfo.nDSInfo; i++)
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)App.DXInfo.DSInfo[i].About);

	if (!App.DXInfo.nDSInfo)
	{
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)SCRIPT_TEXT(STR_NO_SOUND_CARD_INSTALLED));
		EnableWindow(GetDlgItem(dlg, 1018), 0);
		SendMessage(GetDlgItem(dlg, 1018), BM_SETCHECK, 1, 0);
		EnableWindow(hwnd, 0);
	}

	SendMessage(hwnd, CB_SETCURSEL, 0, 0);
}

void inject_cmdline(bool replace)
{
	INJECT(0x00495B70, CLSetup, replace);
	INJECT(0x00495BA0, CLNoFMV, replace);
	INJECT(0x00495C40, InitDSDevice, replace);
}
