#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);

void ClearSurfaces();
long CheckMMXTechnology();
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
void WinGetLastError();
void WinProcMsg();
void WinProcessCommands(long cmd);
void WinProcessCommandLine(LPSTR cmd);
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x004D3150 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x004D30E0 )
