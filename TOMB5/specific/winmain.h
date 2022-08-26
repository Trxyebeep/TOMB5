#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);

void ClearSurfaces();
long CheckMMXTechnology();
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x004D3150 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x004D30E0 )
