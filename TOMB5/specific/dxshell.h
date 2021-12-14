#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXReadKeyboard(char* KeyMap);

#define DXAttempt	( (void(__cdecl*)(HRESULT)) 0x0049F1C0 )
#define DXUpdateJoystick	( (void(__cdecl*)()) 0x004A2D00 )
