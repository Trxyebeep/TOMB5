#include "../tomb5/pch.h"
#include "texture.h"

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

void inject_texture(bool replace)
{
	INJECT(0x004D01D0, AdjustTextInfo, replace);
	INJECT(0x004D03A0, FindHighestBit, replace);
	INJECT(0x004D03D0, RGBMDarkMap, replace);
	INJECT(0x004D0400, RGBMLightMap, replace);
}
