#pragma once
#include "../global/types.h"

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();

extern TEXTURE Textures[64];
extern long nTextures;
