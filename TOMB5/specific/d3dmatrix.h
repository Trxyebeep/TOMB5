#pragma once
#include "../global/vars.h"

#define SetD3DViewMatrix	( (void(__cdecl*)()) 0x00497320 )
#define SaveD3DCameraMatrix	( (void(__cdecl*)()) 0x00497280 )
#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00497650 )
#define D3DMultMatrix	( (LPD3DMATRIX(__cdecl*)(LPD3DMATRIX, LPD3DMATRIX, LPD3DMATRIX)) 0x00497CA0 )
