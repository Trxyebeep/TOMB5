#pragma once

void inject_gf_helpers();

int GetCampaignCheatValue();

#define DoCredits	( (int(__cdecl*)()) 0x004927C0 )
#define SuperShowLogo	( (void(__cdecl*)()) 0x004C9190 )
#define SetFade	( (void(__cdecl*)(int, int)) 0x004CA720 )
#define DoFrontEndOneShotStuff	( (void(__cdecl*)()) 0x004B2090 )
#define ClearFXFogBulbs	( (void(__cdecl*)()) 0x004BA130 )//empty function
#define S_InitialiseScreen	( (void(__cdecl*)()) 0x004A7B10 )//empty
