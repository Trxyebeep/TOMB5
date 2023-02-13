#pragma once
#include "../global/types.h"

void ClearSurfaces();
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
void WinProcMsg();
void WinProcessCommands(long cmd);
void WinProcessCommandLine(LPSTR cmd);
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void WinClose();
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd);

extern WINAPP App;
extern long resChangeCounter;
