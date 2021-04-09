#pragma once

void inject_spotcam();

#define InitialiseSpotCam	( (void(__cdecl*)(short)) 0x0047A9D0 )
#define CalculateSpotCams	( (void(__cdecl*)()) 0x0047B280 )
