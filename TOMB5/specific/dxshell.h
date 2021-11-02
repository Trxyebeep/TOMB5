#pragma once
#include "../global/vars.h"

#define DXAttempt	( (void(__cdecl*)(HRESULT)) 0x0049F1C0 )
#define DXUpdateJoystick	( (void(__cdecl*)()) 0x004A2D00 )
#define DXReadKeyboard	( (void(__cdecl*)(char*)) 0x004A2880 )
