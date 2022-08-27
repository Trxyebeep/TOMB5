#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXReadKeyboard(char* KeyMap);
void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
long DXAttempt(HRESULT r);
long DIAttempt(HRESULT r);

#define DXUpdateJoystick	( (void(__cdecl*)()) 0x004A2D00 )
#define DXCreateSurface	( (long(__cdecl*)(LPDIRECTDRAW4, LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4*)) 0x004A0520 )
#define DXGetErrorString	( (const char*(__cdecl*)(HRESULT)) 0x0049E050 )
#define DIGetErrorString	( (const char*(__cdecl*)(HRESULT)) 0x0049EFA0 )
