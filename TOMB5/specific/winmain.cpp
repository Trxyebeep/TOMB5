#include "../tomb5/pch.h"
#include "winmain.h"
#include "dxshell.h"
#include "function_stubs.h"
#include "output.h"

void ClearSurfaces()
{
	D3DRECT r;

	r.x1 = App.dx.rViewport.left;
	r.y1 = App.dx.rViewport.top;
	r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
	r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;

	if (App.dx.Flags & 0x80)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
	else
		ClearFakeDevice(App.dx.lpD3DDevice, 1, &r, D3DCLEAR_TARGET, 0, 1.0F, 0);

	S_DumpScreen();

	if (App.dx.Flags & 0x80)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
	else
		ClearFakeDevice(App.dx.lpD3DDevice, 1, &r, D3DCLEAR_TARGET, 0, 1.0F, 0);

	S_DumpScreen();
}

void inject_winmain(bool replace)
{
	INJECT(0x004D1AD0, ClearSurfaces, replace);
}
