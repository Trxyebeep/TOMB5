#pragma once
#include "../global/vars.h"

void inject_spotcam();

long Spline(long x, long* knots, int nk);
#define InitSpotCamSequences	( (void(__cdecl*)()) 0x0047A800 )
#define InitialiseSpotCam	( (void(__cdecl*)(short)) 0x0047A9D0 )
#define CalculateSpotCams	( (void(__cdecl*)()) 0x0047B280 )
