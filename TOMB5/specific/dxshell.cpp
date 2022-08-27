#include "../tomb5/pch.h"
#include "dxshell.h"
#include "function_stubs.h"

long DDSCL_FLAGS[11] =	// for DXSetCooperativeLevel logging
{
	DDSCL_ALLOWMODEX,
	DDSCL_ALLOWREBOOT,
	DDSCL_CREATEDEVICEWINDOW,
	DDSCL_EXCLUSIVE,
	DDSCL_FPUSETUP,
	DDSCL_FULLSCREEN,
	DDSCL_MULTITHREADED,
	DDSCL_NORMAL,
	DDSCL_NOWINDOWCHANGES,
	DDSCL_SETDEVICEWINDOW,
	DDSCL_SETFOCUSWINDOW
};

const char* DDSCL_TEXT[11] =
{
	"allowmodex",
	"allowreboot",
	"createdevicewindow",
	"exclusive",
	"fpusetup",
	"fullscreen",
	"multithreaded",
	"normal",
	"nowindowchanges",
	"setdevicewindow",
	"setfocuswindow"
};

void DXReadKeyboard(char* KeyMap)
{
#ifndef GENERAL_FIXES	//Arsunt's fix for keyboard deadlock, reproduceable in TR5 when a debugger breakpoint is hit..
	HRESULT state;

	state = G_dxptr->Keyboard->GetDeviceState(256, KeyMap);

	if (FAILED(state))
	{
		if (state == DIERR_INPUTLOST)
			G_dxptr->Keyboard->Acquire();

		G_dxptr->Keyboard->GetDeviceState(256, KeyMap);
	}
#else
	while FAILED(G_dxptr->Keyboard->GetDeviceState(256, KeyMap))	//original only acquires keyboard at DIERR_INPUTLOST
	{
		if FAILED(G_dxptr->Keyboard->Acquire())
		{
			memset(KeyMap, 0, 256);
			break;
		}
	}
#endif
}

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count)
{
	uchar i;

	for (i = 0; !(mask & 1); i++)
		mask >>= 1;

	*shift = i;

	for (i = 0; mask & 1; i++)
		mask >>= 1;

	*count = i;
}

long DXAttempt(HRESULT r)
{
	if (SUCCEEDED(r))
		return DD_OK;

	Log(1, "ERROR : %s", DXGetErrorString(r));
	return DD_FALSE;
}

long DIAttempt(HRESULT r)
{
	if (SUCCEEDED(r))
		return DI_OK;

	Log(1, "ERROR : %s", DIGetErrorString(r));
	return r;
}

void* AddStruct(void* p, long num, long size)
{
	void* ptr;

	if (!num)
		ptr = MALLOC(size);
	else
		ptr = REALLOC(p, size * (num + 1));

	memset((char*)ptr + size * num, 0, size);
	return ptr;
}

long DXDDCreate(LPGUID pGuid, void** pDD4)
{
	LPDIRECTDRAW pDD;

	Log(2, "DXDDCreate");

	if (DXAttempt(DirectDrawCreate(pGuid, &pDD, 0)) != DD_OK)
	{
		Log(1, "DXDDCreate Failed");
		return 0;
	}

	DXAttempt(pDD->QueryInterface(IID_IDirectDraw4, pDD4));

	if (pDD)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", pDD, pDD->Release());
		pDD = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");

	Log(5, "DXDDCreate Successful");
	return 1;
}

long DXD3DCreate(LPDIRECTDRAW4 pDD4, void** pD3D)
{
	Log(2, "DXD3DCreate");

	if (DXAttempt(pDD4->QueryInterface(IID_IDirect3D3, pD3D)) != DD_OK)
	{
		Log(1, "DXD3DCreate Failed");
		return 0;
	}

	Log(5, "DXD3DCreate Successful");
	return 1;
}

long DXSetCooperativeLevel(LPDIRECTDRAW4 pDD4, HWND hwnd, long flags)
{
	char* ptr;
	char buf[1024];

	strcpy(buf, "DXSetCooperativeLevel - ");
	ptr = &buf[strlen(buf)];

	for (int i = 0; i < 11; i++)
	{
		if (DDSCL_FLAGS[i] & flags)
		{
			strcpy(ptr, DDSCL_TEXT[i]);
			ptr += strlen(DDSCL_TEXT[i]);
			*ptr++ = ' ';
			*ptr = '\0';
		}
	}

	*(ptr - 1) = '\0';
	Log(2, buf);

	if (DXAttempt(pDD4->SetCooperativeLevel(hwnd, flags)) != DD_OK)
		return 0;

	return 1;
}

HRESULT __stdcall DXEnumDisplayModes(DDSURFACEDESC2* lpDDSurfaceDesc2, LPVOID lpContext)
{
	DXDIRECTDRAWINFO* DDInfo;
	DXDISPLAYMODE* DM;
	long nDisplayModes;

	DDInfo = (DXDIRECTDRAWINFO*)lpContext;
	nDisplayModes = DDInfo->nDisplayModes;
	DDInfo->DisplayModes = (DXDISPLAYMODE*)AddStruct(DDInfo->DisplayModes, nDisplayModes, sizeof(DXDISPLAYMODE));
	DM = &DDInfo->DisplayModes[nDisplayModes];
	DM->w = lpDDSurfaceDesc2->dwWidth;
	DM->h = lpDDSurfaceDesc2->dwHeight;
	DM->bpp = lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount;
	DM->bPalette = (lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) >> 5;
	DM->RefreshRate = lpDDSurfaceDesc2->dwRefreshRate;
	memcpy(&DM->ddsd, lpDDSurfaceDesc2, sizeof(DM->ddsd));

	if (DM->bPalette)
		Log(3, "%d x %d - %d Bit - Palette", DM->w, DM->h, DM->bpp);
	else
	{
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask, &DM->rshift, &DM->rbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask, &DM->gshift, &DM->gbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask, &DM->bshift, &DM->bbpp);
		Log(3, "%d x %d - %d Bit - %d%d%d", DM->w, DM->h, DM->bpp, DM->rbpp, DM->gbpp, DM->bbpp);
	}

	DDInfo->nDisplayModes++;
	return DDENUMRET_OK;
}

HRESULT __stdcall DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
	DXD3DDEVICE* d3d;
	DXZBUFFERINFO* zbuffer;
	long nZBufferInfos;

	d3d = (DXD3DDEVICE*)lpContext;
	nZBufferInfos = d3d->nZBufferInfos;
	d3d->ZBufferInfos = (DXZBUFFERINFO*)AddStruct(d3d->ZBufferInfos, nZBufferInfos, sizeof(DXZBUFFERINFO));
	zbuffer = &d3d->ZBufferInfos[nZBufferInfos];
	memcpy(&zbuffer->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));
	zbuffer->bpp = lpDDPixFmt->dwRGBBitCount;
	Log(3, "%d Bit", zbuffer->bpp);
	d3d->nZBufferInfos++;
	return D3DENUMRET_OK;
}

HRESULT __stdcall DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
	DXD3DDEVICE* d3d;
	DXTEXTUREINFO* tex;
	long nTextureInfos;

	if (!(lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) || !(lpDDPixFmt->dwFlags & DDPF_RGB))
		return DDENUMRET_OK;

	d3d = (DXD3DDEVICE*)lpContext;
	nTextureInfos = d3d->nTextureInfos;
	d3d->TextureInfos = (DXTEXTUREINFO*)AddStruct(d3d->TextureInfos, nTextureInfos, sizeof(DXTEXTUREINFO));
	tex = &d3d->TextureInfos[nTextureInfos];
	memcpy(&tex->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));

	if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED8)
	{
		tex->bPalette = 1;
		tex->bpp = 8;
		Log(3, "8 Bit");
	}
	else if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED4)
	{
		tex->bPalette = 1;
		tex->bpp = 4;
		Log(3, "4 Bit");
	}
	else
	{
		tex->bPalette = 0;
		tex->bpp = lpDDPixFmt->dwRGBBitCount;

		if (lpDDPixFmt->dwFlags & DDPF_RGB)
		{
			if (lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS)
			{
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRGBAlphaBitMask, &tex->ashift, &tex->abpp);
				Log(3, "%d Bit %d%d%d%d RGBA", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp, tex->abpp);
			}
			else
			{
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
				Log(3, "%d Bit %d%d%d RGB", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp);
			}
		}
	}

	d3d->nTextureInfos++;
	return D3DENUMRET_OK;
}

long BPPToDDBD(long BPP)
{
	switch (BPP)
	{
	case 1:
		return DDBD_1;

	case 2:
		return DDBD_2;

	case 4:
		return DDBD_4;

	case 8:
		return DDBD_8;

	case 16:
		return DDBD_16;

	case 24:
		return DDBD_24;

	case 32:
		return DDBD_32;

	default:
		return 0;
	}
}

long DXCreateD3DDevice(LPDIRECT3D3 d3d, GUID guid, LPDIRECTDRAWSURFACE4 surf, LPDIRECT3DDEVICE3* device)
{
	Log(2, "DXCreateD3DDevice");

	if (DXAttempt(d3d->CreateDevice(guid, surf, device, 0)) != DD_OK)
	{
		Log(1, "DXCreateD3DDevice Failed");
		return 0;
	}
	else
	{
		Log(2, "DXCreateD3DDevice Successful");
		return 1;
	}
}

long DXSetVideoMode(LPDIRECTDRAW4 dd, long dwWidth, long dwHeight, long dwBPP)
{
	Log(2, "DXSetVideoMode");
	Log(5, "SetDisplayMode - %dx%dx%d", dwWidth, dwHeight, dwBPP);

	if (DXAttempt(dd->SetDisplayMode(dwWidth, dwHeight, dwBPP, 0, 0)) != DD_OK)
		return 0;

	return 1;
}

long DXCreateSurface(LPDIRECTDRAW4 dd, LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4* surf)
{
	Log(2, "DXCreateSurface");

	if (DXAttempt(dd->CreateSurface(desc, surf, 0)) == DD_OK)
		return 1;

	Log(1, "DXCreateSurface Failed");
	return 0;
}

void inject_dxshell(bool replace)
{
	INJECT(0x004A2880, DXReadKeyboard, replace);
	INJECT(0x0049F9C0, DXBitMask2ShiftCnt, replace);
	INJECT(0x0049F1C0, DXAttempt, replace);
	INJECT(0x0049F200, DIAttempt, replace);
	INJECT(0x0049F4C0, AddStruct, replace);
	INJECT(0x0049F530, DXDDCreate, replace);
	INJECT(0x0049F620, DXD3DCreate, replace);
	INJECT(0x004A0600, DXSetCooperativeLevel, replace);
	INJECT(0x0049FA10, DXEnumDisplayModes, replace);
	INJECT(0x004A0490, DXEnumZBufferFormats, replace);
	INJECT(0x004A0270, DXEnumTextureFormats, replace);
	INJECT(0x0049FB80, BPPToDDBD, replace);
	INJECT(0x004A0C30, DXCreateD3DDevice, replace);
	INJECT(0x004A0590, DXSetVideoMode, replace);
	INJECT(0x004A0520, DXCreateSurface, replace);
}
