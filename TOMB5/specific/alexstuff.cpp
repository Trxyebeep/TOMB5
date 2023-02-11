#include "../tomb5/pch.h"
#include "alexstuff.h"
#include "function_stubs.h"
#include "profiler.h"
#include "drawroom.h"
#include "winmain.h"
#include "file.h"
#include "../game/gameflow.h"
#include "../game/text.h"
#include "specificfx.h"
#include "function_table.h"
#include "lighting.h"
#include "d3dmatrix.h"
#include "../game/tomb4fx.h"
#include "../game/deltapak.h"
#include "texture.h"

const char* CreditNames[] =
{
#ifdef GENERAL_FIXES
	"nobody",
#endif
	"Derek Leigh-Gilchrist",
	"Martin Gibbins",
	"Tom Scutt",
	"Chris Coupe",
	"Alex Davis",
	"Richard Flower",
	"Martin Jensen",
	"Phil Chapman",
	"Jerr O'Carroll",
	"Joby Wood",
	"Andrea Cordella",
	"Richard Morton",
	"Andy Sandham",
	"Peter Connelly",
	"Martin Iveson",
	"Andy Watt",
	"Nick Connolly",
	"Hayos Fatunmbi",
	"Paul Field",
	"Steve Wakeman",
	"Dave Ward",
	"Jason Churchman",
	"Jeremy H. Smith",
	"Adrian Smith",
	"Benjamin Twose",
	"Ex Machina",
	"Ray Tran",
	"Stuart Abrahart",
	"Richard Apperley"
};

#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
#ifdef GENERAL_FIXES
short CreditGroups[15] =
{
	0,
	STR_PROGRAMMERS,
	STR_AI_PROGRAMMERS,
	STR_ADDITIONAL_PROGRAMMERS,
	STR_ANIMATORS,
	STR_LEVEL_DESIGNERS,
	STR_FMV_SEQUENCES,
	STR_MUSIC_AND_SOUND_FX,
	STR_ADDITIONAL_SOUND_FX,
	STR_ORIGINAL_STORY,
	STR_SCRIPT,
	STR_PRODUCER,
	STR_QA,
	STR_EXECUTIVE_PRODUCERS,
	0
};

const char* CreditsTable[] =
{
	"%01",
	"!05", "!02", "!01", "0",

	"%02",
	"!03", "0",

	"%03",
	"!04", "!06", "!07", "0",

	"%04",
	"!09", "!08", "0",

	"%05",
	"!11", "!12", "!13", "!10", "0",

	"%06",
	"!26", "0",

	"%07",
	"!14", "0",

	"%08",
	"!15", "0",

	"%09",
	"!12", "!13", "0",

	"%10",
	"!13", "0",

	"%11",
	"!16", "0",

	"%12",
	"!28", "!29", "!17", "!18", "!19", "!20", "!21", "!22", "!25", "0",

	"%13",
	"!23", "!24", "0",

	"Tomb Raider V Community Edition",
	"Troye", "ChocolateFan"
};
#else
//0x8000 = "category"/use string, otherwise use index from above names^
//-1 = empty space
//-2 = end of list
short CreditsTable[] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

	0x8000 | STR_PROGRAMMERS, -1,
	4, 1, 0,		//Alex, Martin (gibby), Del
	-1, -1, -1, -1,

	0x8000 | STR_AI_PROGRAMMERS, -1,
	2,				//Tom
	-1, -1, -1, -1,

	0x8000 | STR_ADDITIONAL_PROGRAMMERS, -1,
	3, 5, 6,		//Chris, Richard, Martin (jensen)
	-1, -1, -1, -1,

	0x8000 | STR_ANIMATORS, -1,
	8, 7,			//Jerr, Phil
	-1, -1, -1, -1,

	0x8000 | STR_LEVEL_DESIGNERS, -1,
	10, 11, 12, 9,	//Andrea, Richard, Andy, Joby
	-1, -1, -1, -1,

	0x8000 | STR_FMV_SEQUENCES, -1,
	25,				//"Ex Machina" (who?)
	-1, -1, -1, -1,

	0x8000 | STR_MUSIC_AND_SOUND_FX, -1,
	13,				//Peter
	-1, -1, -1, -1,

	0x8000 | STR_ADDITIONAL_SOUND_FX, -1,
	14,				//Martin (Iveson)
	-1, -1, -1, -1,

	0x8000 | STR_ORIGINAL_STORY, -1,
	11, 12,			//Richard, Andy
	-1, -1, -1, -1,

	0x8000 | STR_SCRIPT, -1,
	12,				//Andy
	-1, -1, -1, -1,

	0x8000 | STR_PRODUCER, -1,
	15,				//Andy Watt
	-1, -1, -1, -1,

	0x8000 | STR_QA, -1,
	27, 28, 16, 17, 18, 19, 20, 21, 24,	//buncha people
	-1, -1, -1, -1,

	0x8000 | STR_EXECUTIVE_PRODUCERS, -1,
	22, 23,			//Jeremy, Adrian
	-1, -1, -1, -1,

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-2
};
#endif
#pragma warning(pop)

static float water_buffer[8712];
static float water_plot_buffer[4356];
static long water_buffer_calced;

void aLoadRoomStream()
{
	ROOM_INFO* room_data;
	long length, num_rooms, size;
	char* data;

	length = *(long*)FileData;
	FileData += sizeof(long);
	num_rooms = *(long*)FileData;
	FileData += sizeof(long);
	room = (ROOM_INFO*)game_malloc(num_rooms * sizeof(ROOM_INFO), 0);
	room_data = room;

	for (int i = 0; i < num_rooms; i++)
	{
		FileData += sizeof(long);
		size = *(long*)FileData;
		FileData += sizeof(long);
		data = (char*)game_malloc(size, 0);
		memcpy(data, FileData, size);
		aFixUpRoom(room_data, data);
		FileData += size;
		room_data++;
	}

	number_rooms = (short)num_rooms;
}

void aFixUpRoom(ROOM_INFO* r, char* s)
{
	ROOM_INFO* pR;
	ROOMLET* pRm;
	float* pVtx;
	long* pPre;
	short* pFac;
	long offset;

	pR = (ROOM_INFO*)s;
	offset = long(s + sizeof(ROOM_INFO));

	pR->fogbulb = (FOGBULB_INFO*)((char*)pR->fogbulb + offset);
	pR->pclight = (PCLIGHT_INFO*)((char*)pR->pclight + offset);
	pR->floor = (FLOOR_INFO*)((char*)pR->floor + offset);
	pR->door = (short*)((char*)pR->door + offset);
	pR->mesh = (MESH_INFO*)((char*)pR->mesh + offset);
	pR->pRoomlets = (ROOMLET*)((char*)pR->pRoomlets + offset);
	pR->pRmFace = (short*)((char*)pR->pRmFace + offset);
	pR->pRmPrelight = (long*)((char*)pR->pRmPrelight + offset);
	pR->pRmVtx = (float*)((char*)pR->pRmVtx + offset);

	if ((long)pR->door & 1)
	{
		Log(0, "%X", pR->door);
		pR->door = 0;
	}

	pFac = pR->pRmFace;
	pPre = pR->pRmPrelight;
	pVtx = pR->pRmVtx;
	pRm = pR->pRoomlets;

	for (int i = 0; i < pR->nRoomlets; i++)
	{
		pRm[i].pFac = (short*)pFac;
		pRm[i].pPrelight = pPre;
		pRm[i].pSVtx = (float*)pVtx;

		pFac += (5 * pRm[i].nTri) + (6 * pRm[i].nQuad);
		pPre += pRm[i].nVtx;
		pVtx += 7 * pRm[i].nVtx;
	}

	memcpy(r, s, sizeof(ROOM_INFO));

	if (r->num_lights > MaxRoomLights)
		MaxRoomLights = r->num_lights;
}

void aUpdate()
{
	static float znear;
	static long zero;
	static long alphamaybe;

	spec_wibble++;
	znear = f_mznear;
	mAddProfilerEvent(0xFF00FF00);
	mAddProfilerEvent(0xFF0000FF);
	zero = 0;
	mAddProfilerEvent(0xFFFFFFFF);

	if (aCamDir.y >= 0)
		alphamaybe = long(95 - ((1 - aCamDir.y) * -144)) << 24;
	else
		alphamaybe = 0xEF000000;

	mAddProfilerEvent(0xFFFF00);
}

void aInitWater()
{
	memset(&water_buffer, 0, sizeof(water_buffer));
	water_buffer_calced = 0;
}

void aTransformClip_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx, long nClip)
{
	D3DMATRIX mx;
	short* clip;
	float x, y, z, zv;
	short c;

	mx = D3DMView;
	clip = &clipflags[nClip];

	for (int i = 0; i < nVtx; i++)
	{
		c = 0;
		x = mx._11 * vec->x + mx._21 * vec->y + mx._31 * vec->z + mx._41;
		y = mx._12 * vec->x + mx._22 * vec->y + mx._32 * vec->z + mx._42;
		z = mx._13 * vec->x + mx._23 * vec->y + mx._33 * vec->z + mx._43;

		if (z < f_mznear)
			c = -128;
		else
		{
			zv = f_mpersp / z;

			if (v->sz > FogEnd)
			{
				v->sz = f_zfar;
				c = 256;
			}

			v->sx = zv * x + f_centerx;
			v->sy = zv * y + f_centery;
			v->sz = z;
			v->rhw = zv * f_moneopersp;

			if (v->sx < phd_winxmin)
				c++;
			else if (v->sx > phd_winxmax)
				c += 2;

			if (v->sy < phd_winymin)
				c += 4;
			else if (v->sy > phd_winymax)
				c += 8;
		}

		*clip++ = c;
		vec++;
		v++;
	}
}

void aTransform_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx)
{
	D3DMATRIX mx;
	float x, y, z, zv;

	mx = D3DMView;

	for (int i = 0; i < nVtx; i++)
	{
		x = mx._11 * vec->x + mx._21 * vec->y + mx._31 * vec->z + mx._41;
		y = mx._12 * vec->x + mx._22 * vec->y + mx._32 * vec->z + mx._42;
		z = mx._13 * vec->x + mx._23 * vec->y + mx._33 * vec->z + mx._43;
		zv = f_mpersp / z;
		v->sx = zv * x + f_centerx;
		v->sy = zv * y + f_centery;
		v->sz = z;
		v->rhw = zv * f_moneopersp;
		vec++;
		v++;
	}
}

void aInit()
{
	aInitWater();
	aRoomInit();
}

void aWinString(long x, long y, char* string)
{
	WinDisplayString(x, y, string);
}

char* aReadCutData(long n, FILE* file)
{
	char* data;
	long offset, size;

	if (!n)
		return 0;

	//cutseq file header is put in tsv_buffer beforehand!
	offset = *(long*)&tsv_buffer[n * 2 * sizeof(long)];
	size = *(long*)&tsv_buffer[n * 2 * sizeof(long) + 4];
	fseek(file, offset, SEEK_SET);
	data = (char*)game_malloc(size, 0);
	fread(data, size, 1, file);
	return data;
}

long aCalcDepackBufferSz(char* data)
{
	long size;

	GLOBAL_cutme = (NEW_CUTSCENE*)data;
	size = 0;

	for (int i = 0; i < GLOBAL_cutme->numactors; i++)
		size += sizeof(PACKNODE) * (GLOBAL_cutme->actor_data[i].nodes + 1);

	return size + (sizeof(PACKNODE) * 2);
}

void aMakeCutsceneResident(long n1, long n2, long n3, long n4)
{
	FILE* file;
	char* d1;
	char* d2;
	char* d3;
	char* d4;

	if (!n1 && !n2 && !n3 && !n4)
		return;

	file = FileOpen("DATA\\CUTSEQ.BIN");

	if (!file)
		return;

	fread(tsv_buffer, 1, 2048, file);	//whole header in tsv_buffer
	memset(cutseq_resident_addresses, 0, sizeof(cutseq_resident_addresses));
	lastcamnum = -1;
	GLOBAL_playing_cutseq = 0;
	cutseq_trig = 0;
	d1 = aReadCutData(n1, file);
	d2 = aReadCutData(n2, file);
	d3 = aReadCutData(n3, file);
	d4 = aReadCutData(n4, file);
	fclose(file);

	if (d1)
		aCalcDepackBufferSz(d1);

	if (d2)
		aCalcDepackBufferSz(d2);

	if (d3)
		aCalcDepackBufferSz(d3);

	if (d4)
		aCalcDepackBufferSz(d4);

	if (n1)
		cutseq_resident_addresses[n1] = d1;

	if (n2)
		cutseq_resident_addresses[n2] = d2;

	if (n3)
		cutseq_resident_addresses[n3] = d3;

	if (n4)
		cutseq_resident_addresses[n4] = d4;
}

char* aFetchCutData(long n)
{
	FILE* file;
	char* data;

	data = cutseq_resident_addresses[n];

	if (!data)
	{
		file = FileOpen("DATA\\CUTSEQ.BIN");

		if (file)
		{
			fread(tsv_buffer, 1, 2048, file);
			data = aReadCutData(n, file);
			fclose(file);
		}
	}

	return data;
}

#pragma warning(push)
#pragma warning(disable : 4244)
#ifdef GENERAL_FIXES

long DoCredits()
{
	const char* s;
	static ulong StartPos = 0;
	static long init = 0;
	long y, num_drawn;

	num_drawn = 0;

	if (!init)
	{
		StartPos = font_height + phd_winheight;
		init = 1;
	}

	y = StartPos;

	for (int i = 0; i < sizeof(CreditsTable) / 4; i++)
	{
		s = CreditsTable[i];

		if (y < font_height + phd_winheight + 1 && y > -font_height)
		{
			if (*s == '%')
			{
				PrintString(phd_winwidth >> 1, y, 6, SCRIPT_TEXT(CreditGroups[atoi(s + 1)]), FF_CENTER);
			//	PrintBigString(phd_winwidth >> 1, y << 1, 6, SCRIPT_TEXT(CreditGroups[atoi(s + 1)]), FF_CENTER);
			}
			else if (*s != '0')
			{
				if (i >= 57)
				{
					PrintString(phd_winwidth >> 1, y, 2 + (i == 57 ? 4 : 0), s, FF_CENTER);
				//	PrintBigString(phd_winwidth >> 1, y << 1, 2 + (i == 57 ? 4 : 0), s, FF_CENTER);
				}
				else
				{
					PrintString(phd_winwidth >> 1, y, 2, CreditNames[atoi(s + 1)], FF_CENTER);
				//	PrintBigString(phd_winwidth >> 1, y << 1, 2, CreditNames[atoi(s + 1)], FF_CENTER);
				}
			}

			num_drawn++;
		}

		y += font_height;
	}

	StartPos--;

	if (!num_drawn)
		init = 0;

	return num_drawn;
}

#else

#define CREDIT_FONT_HEIGHT	34
long DoCredits()
{
	short* c;
	long n, l, y, y2;
	static long pos = 1;

	n = pos / CREDIT_FONT_HEIGHT;
	l = n + 26;							//todo: what's 26?
	y = phd_winymax / -2 - pos % CREDIT_FONT_HEIGHT;
	y2 = y * 2;

	for (; n < l; n++)
	{
		c = &CreditsTable[n];

		if (*c == -2)
		{
			//done
			pos = 1;
			return 0;
		}

		if (*c == -1)	//empty line
		{
			y += CREDIT_FONT_HEIGHT;
			y2 += CREDIT_FONT_HEIGHT * 2;
			continue;
		}

		if (*c & 0x8000)
			PrintString(phd_winwidth >> 1, y + phd_centery, 6, SCRIPT_TEXT(*c & 0x7FFF), FF_CENTER);
		else
			PrintString(phd_winwidth >> 1, y + phd_centery, 2, CreditNames[*c], FF_CENTER);

		if (*c & 0x8000)
			PrintBigString(phd_winwidth >> 1, y2 + phd_centery, 6, SCRIPT_TEXT(*c & 0x7FFF), FF_CENTER);
		else
			PrintBigString(phd_winwidth >> 1, y2 + phd_centery, 2, CreditNames[*c], FF_CENTER);

		y += CREDIT_FONT_HEIGHT;
		y2 += CREDIT_FONT_HEIGHT * 2;
	}

	pos++;
	return 1;
}

#endif
#pragma warning(pop)

void DrawBigChar(short x, short y, ushort col, CHARDEF* c, long scale)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float u1, v1, u2, v2;
	long x1, y1, x2, y2, tc, bc;

	x += phd_winxmin;
	y += phd_winymin;

	x1 = x;
	y1 = y + scale * c->YOffset;

	x2 = x + scale * c->w;
	y2 = scale * (c->YOffset + c->h - big_char_height) + y;

	if (y2 < 0 || y1 > phd_winymax)
		return;

	setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, long(f_mznear + 1.0F), clipflags);

	tc = *(long*)&FontShades[col][2 * c->TopShade];
	bc = *(long*)&FontShades[col][2 * c->BottomShade];
	v[0].color = tc & 0xFFFFFF | 0x20000000;
	v[1].color = tc & 0xFFFFFF | 0x20000000;
	v[2].color = bc & 0xFFFFFF | 0x20000000;
	v[3].color = bc & 0xFFFFFF | 0x20000000;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;

	u1 = c->u + (1.0F / 512.0F);
	v1 = c->v + (1.0F / 512.0F);
	u2 = 512.0F / float(phd_winxmax + 1) * (float)c->w * (1.0F / 256.0F) + c->u - (1.0F / 512.0F);
	v2 = 240.0F / float(phd_winymax + 1) * (float)c->h * (1.0F / 256.0F) + c->v - (1.0F / 512.0F);
	tex.drawtype = 3;
	tex.flag = 0;
	tex.tpage = ushort(nTextures - 2);
	tex.u1 = u1;
	tex.v1 = v1;
	tex.u2 = u2;
	tex.v2 = v1;
	tex.u3 = u2;
	tex.v3 = v2;
	tex.u4 = u1;
	tex.v4 = v2;
	nPolyType = 4;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
}

long GetBigStringLength(const char* string, short* top, short* bottom)
{
	CHARDEF* def;
	long s, length;
	short lowest, highest, y;

	s = *string++;
	length = 0;
	lowest = -1024;
	highest = 1024;

	while (s)
	{
		if (s == '\n')
			break;

		if (s == ' ')
			length += long((float(phd_winxmax + 1) / 640.0F) * 8.0F) * 3;
		else if (s == '\t')
		{
			length += 40;

			if (top && highest > -12)
				highest = -12;

			if (bottom && lowest < 2)
				lowest = 2;
		}
		else if (s >= 20)
		{
			if (s < ' ')
				def = &CharDef[s + 74];
			else
				def = &CharDef[s - '!'];

			if (ScaleFlag)
				length += def->w - def->w / 4;
			else
				length += def->w * 3;

			y = def->YOffset;

			if (top && y < highest)
				highest = def->YOffset;

			if (bottom && def->h + y > lowest)
				lowest = def->h + y;
		}

		s = *string++;
	}

	if (top)
		*top = highest;

	if (bottom)
		*bottom = lowest;

	return length;
}

void PrintBigString(ushort x, ushort y, uchar col, const char* string, ushort flags)
{
	CHARDEF* def;
	short x2, bottom, l, top, bottom2;
	uchar s;

	ScaleFlag = (flags & FF_SMALL) != 0;
	x2 = (short)GetBigStringLength(string, 0, &bottom);

	if (flags & FF_CENTER)
		x2 = x - (x2 >> 1);
	else if (flags & FF_RJUSTIFY)
		x2 = x - x2;
	else
		x2 = x;

	s = *string++;

	while (s)
	{
		if (s == '\n')
		{
			if (*string == '\n')
			{
				bottom = 0;
				y += 16;
			}
			else
			{
				l = (short)GetBigStringLength(string, &top, &bottom2);

				if (flags & FF_CENTER)
					x2 = x - (l >> 1);
				else if (flags & FF_RJUSTIFY)
					x2 = x - l;
				else
					x2 = x;

				y += bottom - top + 2;
				bottom = bottom2;
			}

			s = *string++;
			continue;
		}

		if (s == ' ')
		{
			if (ScaleFlag)
				x2 += 6;
			else
				x2 += short(float(phd_winxmax + 1) / 640.0F * 8.0F) * 3;

			s = *string++;
			continue;
		}

		if (s == '\t')
		{
			x2 += 40;
			s = *string++;
			continue;
		}

		if (s < 20)
		{
			col = s - 1;
			s = *string++;
			continue;
		}

		if (s < ' ')
			def = &CharDef[s + 74];
		else
			def = &CharDef[s - '!'];

		DrawBigChar(x2, y, col, def, 3);

		if (ScaleFlag)
			x2 += def->w - def->w / 4;
		else
			x2 += def->w * 3;

		s = *string++;
	}

	ScaleFlag = 0;
}

void aProcessWater(long n)
{
	float* pPlot;
	float* pOld;
	float* pNew;
	float* pAbove;
	float* pBelow;
	float* stash;
	float num;
	long rnd, val, lp, lp2;

	pPlot = water_plot_buffer;

	if (water_buffer_calced)
	{
		pOld = water_buffer;
		pNew = &water_buffer[4356];
	}
	else
	{
		pOld = &water_buffer[4356];
		pNew = water_buffer;
	}

	pOld += 67;
	pNew += 67;
	pAbove = pOld - 66;
	pBelow = pOld + 66;
	stash = pNew;

	for (lp = 0; lp < 64; lp++)
	{
		for (lp2 = 0; lp2 < 64; lp2++)
		{
			num = (pOld[lp2 + 1] + pOld[lp2 - 1] + pAbove[lp2] + pAbove[lp2 + 1] + pAbove[lp2 - 1] + pBelow[lp2] + pBelow[lp2 + 1] + pBelow[lp2 - 1]) *
				0.25F - pNew[lp2];
			num -= num * 0.0125F;
			pNew[lp2] = num;
			pPlot[lp2] = -num;
		}

		pPlot += 66;
		pOld += 66;
		pNew += 66;
		pAbove += 66;
		pBelow += 66;
	}

	rnd = rand() & 3;

	for (lp = 0; lp < rnd; lp++)
	{
		val = 66 * (rand() % 56 + 4);
		val += rand() % 56 + 4;
		num = float(rand() & 7);
		stash[val] += num;

		num *= 0.5F;
		stash[val - 66] += num;
		stash[val + 66] += num;
		stash[val - 1] += num;
		stash[val + 1] += num;
	}

	water_buffer_calced ^= 1;
}

void inject_alexstuff(bool replace)
{
	INJECT(0x004916C0, aLoadRoomStream, replace);
	INJECT(0x004917D0, aFixUpRoom, replace);
	INJECT(0x00491BE0, aUpdate, replace);
	INJECT(0x00491950, aInitWater, replace);
	INJECT(0x004914C0, aTransformClip_D3DV, replace);
	INJECT(0x004913B0, aTransform_D3DV, replace);
	INJECT(0x00491360, aInit, replace);
	INJECT(0x00491380, aWinString, replace);
	INJECT(0x00491CC0, aReadCutData, replace);
	INJECT(0x00491D30, aCalcDepackBufferSz, replace);
	INJECT(0x00491DA0, aMakeCutsceneResident, replace);
	INJECT(0x00491F60, aFetchCutData, replace);
	INJECT(0x004927C0, DoCredits, replace);
	INJECT(0x00491FE0, DrawBigChar, replace);
	INJECT(0x004922E0, GetBigStringLength, replace);
	INJECT(0x004924B0, PrintBigString, replace);
	INJECT(0x00491980, aProcessWater, replace);
}
