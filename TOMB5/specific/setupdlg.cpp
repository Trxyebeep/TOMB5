#include "../tomb5/pch.h"
#include "setupdlg.h"
#include "../../resource.h"
#include "registry.h"
#include "winmain.h"
#include "../game/gameflow.h"

bool start_setup = 0;
bool fmvs_disabled = 0;

static long nDDDevice;
static long nD3DDevice;
static bool Filter = 1;
static bool TextLow;

static void InitTFormats(HWND dlg, HWND hwnd)
{
	DXD3DDEVICE* device;
	DXTEXTUREINFO* tex;
	long bpp, r, g, b, a;
	char buffer[40];

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	EnableWindow(GetDlgItem(dlg, IDC_TFORMAT), 1);
	device = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice];

	for (int i = 0; i < device->nTextureInfos; i++)
	{
		tex = &device->TextureInfos[i];
		bpp = tex->bpp;
		r = tex->rbpp;
		g = tex->gbpp;
		b = tex->bbpp;
		a = tex->abpp;

		wsprintf(buffer, "%d %s RGBA %d%d%d%d", bpp, SCRIPT_TEXT(TXT_Bit), r, g, b, a);
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
	}

	SendMessage(hwnd, CB_SETCURSEL, 0, 0);
}

static void InitResolution(HWND dlg, HWND hwnd, bool resetvms)
{
	DXD3DDEVICE* device;
	DXDISPLAYMODE* dm;
	long w, h, n;
	char buffer[40];

	n = 0;

	if (resetvms)
	{
		SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
		device = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice];

		for (int i = 0; i < device->nDisplayModes; i++)
		{
			dm = &device->DisplayModes[i];
			w = dm->w;
			h = dm->h;

			wsprintf(buffer, "%dx%d", w, h);
			SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
			SendMessage(hwnd, CB_SETITEMDATA, n, i);

			if (w == 640 && h == 480)
				SendMessage(hwnd, CB_SETCURSEL, n, 0);

			n++;
		}
	}

	if (App.DXInfo.DDInfo[nDDDevice].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)
	{
		EnableWindow(GetDlgItem(dlg, IDC_WINDOWED), 1);
		SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_SETCHECK, 1, 0);
	}
	else
	{
		EnableWindow(GetDlgItem(dlg, IDC_WINDOWED), 0);
		SendMessage(GetDlgItem(dlg, IDC_WINDOWED), BM_SETCHECK, 0, 0);
	}

	SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_SETCHECK, Filter, 0);

	EnableWindow(GetDlgItem(dlg, IDC_LOW_QUALITY_TEX), 1);
	SendMessage(GetDlgItem(dlg, IDC_LOW_QUALITY_TEX), BM_SETCHECK, TextLow, 0);

	if (resetvms)
		InitTFormats(dlg, GetDlgItem(dlg, IDC_TFORMAT));
}

static void InitD3DDevice(HWND dlg, HWND hwnd)
{
	DXDIRECTDRAWINFO* ddraw;

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	ddraw = &App.DXInfo.DDInfo[nDDDevice];

	for (int i = 0; i < ddraw->nD3DDevices; i++)
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)ddraw->D3DDevices[i].About);

	nD3DDevice = 1;
	SendMessage(hwnd, CB_SETCURSEL, nD3DDevice, 0);
	InitResolution(dlg, GetDlgItem(dlg, IDC_RESOLUTION), 1);
}

static void InitDDDevice(HWND dlg, HWND hwnd)
{
	DDDEVICEIDENTIFIER* id;
	char buffer[256];

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < App.DXInfo.nDDInfo; i++)
	{
		id = &App.DXInfo.DDInfo[i].DDIdentifier;
		wsprintf(buffer, "%s - %s (%d.%d.%02d.%04d)", id->szDescription, id->szDriver,
			HIWORD(id->liDriverVersion.HighPart), LOWORD(id->liDriverVersion.HighPart),
			HIWORD(id->liDriverVersion.LowPart), LOWORD(id->liDriverVersion.LowPart));
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
	}

	nDDDevice = App.DXInfo.nDDInfo - 1;
	SendMessage(hwnd, CB_SETCURSEL, nDDDevice, 0);
	InitD3DDevice(dlg, GetDlgItem(dlg, IDC_D3D));
}

static void InitDSDevice(HWND dlg, HWND hwnd)
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < App.DXInfo.nDSInfo; i++)
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)App.DXInfo.DSInfo[i].About);

	if (!App.DXInfo.nDSInfo)
	{
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)SCRIPT_TEXT(TXT_No_Sound_Card_Installed));
		EnableWindow(GetDlgItem(dlg, IDC_DISABLE_SOUND), 0);
		SendMessage(GetDlgItem(dlg, IDC_DISABLE_SOUND), BM_SETCHECK, 1, 0);
		EnableWindow(hwnd, 0);
	}

	SendMessage(hwnd, CB_SETCURSEL, 0, 0);
}

static void InitDialogBox(HWND hwnd)
{
	InitDDDevice(hwnd, GetDlgItem(hwnd, IDC_GRAPHICS_ADAPTER));
	InitDSDevice(hwnd, GetDlgItem(hwnd, IDC_SOUND_ADAPTER));
}

static BOOL CALLBACK SetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		InitDialogBox(dlg);
		return 1;
	}

	if (message == WM_COMMAND)
	{
		switch ((ushort)wParam)
		{
		case IDOK:
			SaveSetup(dlg);
			EndDialog(dlg, 1);
			return 1;

		case IDCANCEL:
			EndDialog(dlg, 0);
			return 1;

		case IDC_GRAPHICS_ADAPTER:

			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				nDDDevice = SendMessage(GetDlgItem(dlg, IDC_GRAPHICS_ADAPTER), CB_GETCURSEL, 0, 0);
				InitD3DDevice(dlg, GetDlgItem(dlg, IDC_D3D));
			}

			break;

		case IDC_D3D:

			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				nD3DDevice = SendMessage(GetDlgItem(dlg, IDC_D3D), CB_GETCURSEL, 0, 0);
				InitResolution(dlg, GetDlgItem(dlg, IDC_RESOLUTION), 1);
			}

		case IDC_BILINEAR:

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, IDC_BILINEAR), BM_GETCHECK, 0, 0))
					Filter = 1;
				else
					Filter = 0;

				InitResolution(dlg, GetDlgItem(dlg, IDC_RESOLUTION), 0);
			}

			break;

		case IDC_LOW_QUALITY_TEX:

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, IDC_LOW_QUALITY_TEX), BM_GETCHECK, 0, 0))
					TextLow = 1;
				else
					TextLow = 0;

				InitResolution(dlg, GetDlgItem(dlg, IDC_RESOLUTION), 0);
			}

			break;

		case IDC_DISABLE_SOUND:

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, (ushort)wParam), BM_GETCHECK, 0, 0))
					EnableWindow(GetDlgItem(dlg, IDC_SOUND_ADAPTER), 0);
				else
					EnableWindow(GetDlgItem(dlg, IDC_SOUND_ADAPTER), 1);
			}

			break;
		}
	}

	return 0;
}

bool SetupDialog()
{
	long ret;

	ShowCursor(1);
	ret = DialogBox(App.hInstance, MAKEINTRESOURCE(IDD_SETUPDIALOG), 0, SetupDlgProc);
	ShowCursor(0);

	if (ret == -1)
	{
		MessageBox(0, "Unable To Initialise Dialog", "", 0);
		return 0;
	}

	if (ret)
		return 1;

	return 0;
}
