#pragma once
#include "../global/vars.h"

void inject_gf_helpers(bool replace);

int GetCampaignCheatValue();

#define DoCredits	( (int(__cdecl*)()) 0x004927C0 )
#define SuperShowLogo	( (void(__cdecl*)()) 0x004C9190 )
#define SetFade	( (void(__cdecl*)(int, int)) 0x004CA720 )
#define DoFrontEndOneShotStuff	( (void(__cdecl*)()) 0x004B2090 )
