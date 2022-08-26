#include "../tomb5/pch.h"
#include "texture.h"

void AdjustTextInfo(PHDTEXTURESTRUCT* ptex, long num, TEXTURESTRUCT* tex)
{
	float w, h;
	long x, y;

	x = Textures[num].xoff << 8;
	y = Textures[num].yoff << 8;
	ptex->u1 -= x;
	ptex->u2 -= x;
	ptex->u3 -= x;
	ptex->u4 -= x;
	ptex->v1 -= y;
	ptex->v2 -= y;
	ptex->v3 -= y;
	ptex->v4 -= y;
	tex->drawtype = ptex->drawtype;
	tex->tpage = num;
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

void inject_texture(bool replace)
{
	INJECT(0x004D01D0, AdjustTextInfo, replace);
}
