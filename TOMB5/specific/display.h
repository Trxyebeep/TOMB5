#pragma once
#include "../global/vars.h"

#define IsHardware	( (int(__cdecl*)()) 0x004B1E90 )
#define IsSuperLowRes	( (int(__cdecl*)()) 0x004B1EB0 )
