#pragma once
#include "../global/types.h"

#define MMXSetPerspecLimit	( (void(__cdecl*)(long)) 0x004D3F30 )
#define MMXGetBackSurfWH	( (void(__cdecl*)(long&, long&)) 0x004D3D90 )
#define MMXGetDeviceViewPort	( (HRESULT(__cdecl*)(IDirect3DDevice3*)) 0x004D3D70 )	//todo make better sense of wtf this does
