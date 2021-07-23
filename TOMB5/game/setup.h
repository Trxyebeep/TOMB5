#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void InitialiseLara(int restore);

#define BuildOutsideTable	( (void(__cdecl*)()) 0x004774D0 )
