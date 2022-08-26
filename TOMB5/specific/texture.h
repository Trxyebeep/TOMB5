#pragma once
#include "../global/vars.h"

void inject_texture(bool replace);

void AdjustTextInfo(PHDTEXTURESTRUCT* ptex, long num, TEXTURESTRUCT* tex);
long FindHighestBit(long n);
void RGBMDarkMap(uchar* r, uchar* g, uchar* b);
void RGBMLightMap(uchar* r, uchar* g, uchar* b);

#define ShowTextures	( (void(__cdecl*)()) 0x004D0CC0 )
typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
#define CreateTexturePage	( (LPDIRECTDRAWSURFACE4(__cdecl*)(long, long, long, long*, rgbfunc, long)) 0x004D0450 )
