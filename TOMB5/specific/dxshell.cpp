#include "../tomb5/pch.h"
#include "dxshell.h"

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

void inject_dxshell(bool replace)
{
	INJECT(0x004A2880, DXReadKeyboard, replace);
	INJECT(0x0049F9C0, DXBitMask2ShiftCnt, replace);
}
