#pragma once
#include "../global/vars.h"

void inject_texture(bool replace);

void AdjustTextInfo(PHDTEXTURESTRUCT* ptex, long num, TEXTURESTRUCT* tex);
long FindHighestBit(long n);
void RGBMDarkMap(uchar* r, uchar* g, uchar* b);
void RGBMLightMap(uchar* r, uchar* g, uchar* b);
void aGenerateMipMaps(LPDDSURFACEDESC2 pDesc, LPDIRECTDRAWSURFACE4 pDest, long* pSrc, long count);
typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();
void ShowTextures();
void aCheckBumpMappingSupport();
LPDIRECTDRAWSURFACE4 aCreateBumpPage(long w, long h, long* pSrc, long format);

extern TEXTURE Textures[64];
