#include "../tomb5/pch.h"
#include "texture.h"
#include "function_stubs.h"
#include "dxshell.h"
#include "3dmath.h"

static long bBumpMapSupported = 0;
static DDPIXELFORMAT bumpPF;

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

void FreeTextures()
{
	TEXTURE* tex;

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
	DXAttempt(App.dx.lpD3D->EvictManagedTextures());

	for (int i = 0; i < nTextures; i++)
	{
		tex = &Textures[i];

		if (tex->tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Texture", tex->tex, tex->tex->Release());
			tex->tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Texture");

		if (!tex->staticTex)
		{
			if (tex->surface)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Surface", tex->surface, tex->surface->Release());
				tex->surface = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Surface");
		}
	}
}

void ShowTextures()
{
	D3DTLBUMPVERTEX v[4];
	static long page = 1;
	static long db = 0;
	static long bump = 0;
	static long x, y, n = 0;
	float n00, n10, n01, n11, o;
	long w, h;

	db++;

	if (db > 1)
	{
		db = 0;

		if (keymap[DIK_A])
			page--;

		if (keymap[DIK_S])
			page++;

		if (keymap[DIK_B])
			bump ^= 1;

		if (page < 0)
			page = 0;

		if (page >= nTextures)
			page = nTextures - 1;
	}

	x = 0;
	y = 32;
	w = 2 * (Textures[page].width + 1);
	h = 2 * (Textures[page].height + 1);

	v[0].sx = (float)x;
	v[0].sy = (float)y;
	v[0].sz = 0;
	v[0].rhw = 1;
	v[0].color = 0xFFFFFFFF;
	v[0].specular = 0xFF000000;
	v[0].tu = 0;
	v[0].tv = 0;
	v[0].tx = 0;
	v[0].ty = 0;

	v[1].sx = float(x + w);
	v[1].sy = (float)y;
	v[1].sz = 0;
	v[1].rhw = 1;
	v[1].color = 0xFFFFFFFF;
	v[1].specular = 0xFF000000;
	v[1].tu = 1;
	v[1].tv = 0;
	v[1].tx = 4;
	v[1].ty = 0;

	v[2].sx = float(x + w);
	v[2].sy = float(y + h);
	v[2].sz = 0;
	v[2].rhw = 1;
	v[2].color = 0xFFFFFFFF;
	v[2].specular = 0xFF000000;
	v[2].tu = 1;
	v[2].tv = 1;
	v[2].tx = 4;
	v[2].ty = 4;

	v[3].sx = (float)x;
	v[3].sy = float(y + h);
	v[3].sz = 0;
	v[3].rhw = 1;
	v[3].color = 0xFFFFFFFF;
	v[3].specular = 0xFF000000;
	v[3].tu = 0;
	v[3].tv = 1;
	v[3].tx = 0;
	v[3].ty = 4;

	if (Textures[page].realBump && bump & 1)
	{
		o = 1.0F;
		n00 = fSin(n) * 0.25F;
		n10 = fCos(n) * 0.25F;
		n01 = -fCos(n) * 0.25F;
		n11 = -fSin(n) * 0.25F;
		n = (n + 327) & 0xFFFF;

		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, *((ulong*)&n00));
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, *((ulong*)&n10));
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, *((ulong*)&n01));
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, *((ulong*)&n11));
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, *((ulong*)&o));
		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, 0);

		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 1);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_ADD);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[page].tex));
		DXAttempt(App.dx.lpD3DDevice->SetTexture(1, Textures[page].bumpTex));
		DXAttempt(App.dx.lpD3DDevice->SetTexture(2, Textures[nTextures - 3].tex));

		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, FVF, v, 4, 0);

		App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	}
	else
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, 4);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 2);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, 2);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, 2);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[page].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, FVF, v, 4, 0);
	}
}

HRESULT __stdcall aBumpTextureCallback(DDPIXELFORMAT* p, LPVOID pO)
{
	if (p->dwFlags == DDPF_BUMPDUDV)
	{
		memcpy(pO, p, sizeof(DDPIXELFORMAT));
		bBumpMapSupported = 1;
		return D3DENUMRET_CANCEL;
	}

	return D3DENUMRET_OK;
}

void aCheckBumpMappingSupport()
{
	DXD3DDEVICE* device;
	static long bump;

	bump = 0;
	device = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D];
	
	if (!(device->DeviceDesc.dwTextureOpCaps & (D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE)) ||
		device->DeviceDesc.wMaxTextureBlendStages < 3)
		return;

	bBumpMapSupported = 0;
	App.dx.lpD3DDevice->EnumTextureFormats(aBumpTextureCallback, (LPVOID)&bumpPF);

	if (bBumpMapSupported)
	{
		Log(5, "Bump bit count 0x%X", bumpPF.dwRGBBitCount);
		Log(5, "Bump DU %X", bumpPF.dwRBitMask);
		Log(5, "Bump DV %X", bumpPF.dwGBitMask);

		if (bumpPF.dwRGBBitCount == 16)
			bump = 1;
	}
}

LPDIRECTDRAWSURFACE4 aCreateBumpPage(long w, long h, long* pSrc, long format)
{
	LPDIRECTDRAWSURFACE4 tSurf;
	DDSURFACEDESC2 desc;
	short* pDest;
	ulong c;
	long d;
	ushort o;
	uchar r, g, b;

	memset(&desc, 0, sizeof(DDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwWidth = w;
	desc.dwHeight = h;
	memcpy(&desc.ddpfPixelFormat, &bumpPF, sizeof(desc.ddpfPixelFormat));
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	tSurf = 0;
	DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
	DXAttempt(tSurf->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));

	if (!format || format == 1)
	{
		pDest = (short*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
			{
				c = *(pSrc + x * 256 / w + y * 0x10000 / h);
				r = CLRR(c);
				g = CLRG(c);
				b = CLRB(c);
				d = ((r + g + b) / 3) - 127;
				
				if (d < -127)
					d = -127;
				else if (d > 128)
					d = 128;

				o = d & 0xFF;

				c = *(pSrc + x * 256 / w + y * 0x10000 / h);
				r = CLRR(c);
				g = CLRG(c);
				b = CLRB(c);
				d = ((r + g + b) / 3) - 127;

				if (d < -127)
					d = -127;
				else if (d > 128)
					d = 128;

				o |= (d & 0xFF) << 8;
				*pDest++ = o;
			}
		}
	}

	DXAttempt(tSurf->Unlock(0));
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
	INJECT(0x004D0B90, FreeTextures, replace);
	INJECT(0x004D0CC0, ShowTextures, replace);
	INJECT(0x004D15B0, aBumpTextureCallback, replace);
	INJECT(0x004D1600, aCheckBumpMappingSupport, replace);
	INJECT(0x004D1710, aCreateBumpPage, replace);
}
