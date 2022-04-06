#pragma once
#include "../global/vars.h"

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x004D3150 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x004D30E0 )
#define ClearSurfaces	( (void(__cdecl*)()) 0x004D1AD0 )
