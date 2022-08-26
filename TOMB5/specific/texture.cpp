#include "../tomb5/pch.h"
#include "texture.h"
#include "function_stubs.h"
#include "dxshell.h"

void AdjustTextInfo(PHDTEXTURESTRUCT* ptex, long num, TEXTURESTRUCT* tex)
{
	float w, h;
	ushort x, y;

	x = ushort(Textures[num].xoff << 8);
	y = ushort(Textures[num].yoff << 8);
	ptex->u1 -= x;
	ptex->u2 -= x;
	ptex->u3 -= x;
	ptex->u4 -= x;
	ptex->v1 -= y;
	ptex->v2 -= y;
	ptex->v3 -= y;
	ptex->v4 -= y;
	tex->drawtype = ptex->drawtype;
	tex->tpage = (ushort)num;
	tex->flag = ptex->tpage ^ (ptex->tpage ^ ptex->flag) & 0x7FFF;
	w = 1.0F / (65535.0F / float(256 / Textures[num].width));
	h = 1.0F / (65535.0F / float(256 / Textures[num].height));
	tex->u1 = (float)ptex->u1 * w;
	tex->v1 = (float)ptex->v1 * h;
	tex->u2 = (float)ptex->u2 * w;
	tex->v2 = (float)ptex->v2 * h;
	tex->u3 = (float)ptex->u3 * w;
	tex->v3 = (float)ptex->v3 * h;
	tex->u4 = (float)ptex->u4 * w;
	tex->v4 = (float)ptex->v4 * h;
}

long FindHighestBit(long n)
{
	long bit;

	bit = 0;

	for (int i = 0; i < 31; ++i)
	{
		if (n & 1)
			bit = i;

		n >>= 1;
	}

	return bit;
}

void RGBMDarkMap(uchar* r, uchar* g, uchar* b)
{
	*r >>= 1;
	*g >>= 1;
	*b >>= 1;
}

void RGBMLightMap(uchar* r, uchar* g, uchar* b)
{
	*r = (255 - *r) >> 1;
	*g = (255 - *g) >> 1;
	*b = (255 - *b) >> 1;
}

void aGenerateMipMaps(LPDDSURFACEDESC2 pDesc, LPDIRECTDRAWSURFACE4 pDest, long* pSrc, long count)
{
	LPDIRECTDRAWSURFACE4 dest;
	DDSCAPS2 caps;
	DDSURFACEDESC2 desc;
	char* pSurf;
	char* pcSrc;
	char* pcSrcT;
	long x, y, num;
	uchar r, g, b, a;

	Log(5, "Create MipMap");
	dest = pDest;
	x = pDesc->dwWidth >> 1;
	y = pDesc->dwHeight >> 1;
	num = 8;
	caps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

	for (int i = 0; i < count; i++)
	{
		memset(&desc, 0, sizeof(DDSURFACEDESC2));
		desc.dwSize = sizeof(DDSURFACEDESC2);
		DXAttempt(dest->GetAttachedSurface(&caps, &dest));
		DXAttempt(dest->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));
		pSurf = (char*)desc.lpSurface;
		pcSrc = (char*)pSrc;
		Log(5, "Level %d - %8.8x", i, pSurf);

		for (int j = 0; j < y; j++)
		{
			pcSrcT = pcSrc;

			for (int k = 0; k < x; k++)
			{
				//bgra!!!
				b = *pcSrcT++;
				g = *pcSrcT++;
				r = *pcSrcT++;
				a = *pcSrcT++;

				if (!r && !g && !b)
				{
					r = 255;
					g = 255;
				}

				*pSurf++ = b;
				*pSurf++ = g;
				*pSurf++ = r;
				*pSurf++ = a;
				pcSrcT += num;
			}

			pcSrc += num << 8;
		}

		DXAttempt(dest->Unlock(0));
		x >>= 1;
		y >>= 1;
		num >>= 1;
	}
}

LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format)
{
	DXTEXTUREINFO* tex;
	LPDIRECTDRAWSURFACE4 tSurf;
	DDSURFACEDESC2 desc;
	long* lS;
	long* lD;
	short* sS;
	short* sD;
	char* cD;
	ulong c, o, ro, go, bo, ao;
	uchar r, g, b, a;

	memset(&desc, 0, sizeof(DDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwWidth = w;
	desc.dwHeight = h;

	if (w < 32 || h < 32)
		MipMapCount = 0;

	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	if (App.dx.Flags & 0x80)
		desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	else
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	if (MipMapCount)
	{
		desc.dwFlags |= DDSD_MIPMAPCOUNT;
		desc.dwMipMapCount = MipMapCount + 1;
		desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}

	DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
	DXAttempt(tSurf->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));

	if (!format)
	{
		lS = pSrc;
		cD = (char*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
			{
				c = *(lS + x * 256 / w + y * 0x10000 / h);
				r = CLRR(c);
				g = CLRG(c);
				b = CLRB(c);
				a = CLRA(c);

				if (RGBM)
					RGBM(&r, &g, &b);

				tex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];
				ro = r >> (8 - tex->rbpp) << (tex->rshift);
				go = g >> (8 - tex->gbpp) << (tex->gshift);
				bo = b >> (8 - tex->bbpp) << (tex->bshift);
				ao = a >> (8 - tex->abpp) << (tex->ashift);
				o = ro | go | bo | ao;

				for (int i = tex->bpp; i > 0; i -= 8)
				{
					*cD++ = (char)o;
					o >>= 8;
				}
			}
		}
	}
	else if (format == 2)
	{
		sS = (short*)pSrc;
		sD = (short*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
				*sD++ = *(sS + x * 256 / w + y * 0x10000 / h);
		}
	}
	else if (format == 1)
	{
		lS = pSrc;
		lD = (long*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
				*lD++ = *(lS + x * 256 / w + y * 0x10000 / h);
		}
	}

	DXAttempt(tSurf->Unlock(0));

	if (MipMapCount)
		aGenerateMipMaps(&desc, tSurf, pSrc, MipMapCount);

	return tSurf;
}

void inject_texture(bool replace)
{
	INJECT(0x004D01D0, AdjustTextInfo, replace);
	INJECT(0x004D03A0, FindHighestBit, replace);
	INJECT(0x004D03D0, RGBMDarkMap, replace);
	INJECT(0x004D0400, RGBMLightMap, replace);
	INJECT(0x004D0980, aGenerateMipMaps, replace);
	INJECT(0x004D0450, CreateTexturePage, replace);
}
