#pragma once
#include "../global/vars.h"

void inject_cmdline(bool replace);

void CLSetup(char* cmd);
void CLNoFMV(char* cmd);

#define DXSetupDialog	( (bool(__cdecl*)()) 0x00495BD0 )
