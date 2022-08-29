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
void WinClose();
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd);
long MungeFPCW(short* fpcw);
void RestoreFPCW(short fpcw);
