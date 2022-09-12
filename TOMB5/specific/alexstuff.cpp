#include "../tomb5/pch.h"
#include "alexstuff.h"
#include "function_stubs.h"
#include "profiler.h"
#include "drawroom.h"
#include "winmain.h"
#include "file.h"
#include "../game/gameflow.h"
#include "../game/text.h"

const char* CreditNames[] =
{
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
#pragma warning(pop)

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
	offset = (long)(s + sizeof(ROOM_INFO));
	pR->fogbulb = (FOGBULB*)((char*)pR->fogbulb + offset);
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
		pFac += 5 * pRm[i].nTri + 2 * 3 * pRm[i].nQuad;;
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
	mAddProfilerEvent();
	mAddProfilerEvent();
	zero = 0;
	mAddProfilerEvent();

	if (aCamDir.y >= 0)
	{
		alphamaybe = (long)(95 - ((1 - aCamDir.y) * -144));
		alphamaybe <<= 24;
	}
	else
		alphamaybe = -0x11000000;

	mAddProfilerEvent();
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
	SEEK(file, offset, SEEK_SET);
	data = (char*)game_malloc(size, 0);
	READ(data, size, 1, file);
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

	READ(tsv_buffer, 1, 2048, file);	//whole header in tsv_buffer
	memset(cutseq_resident_addresses, 0, sizeof(cutseq_resident_addresses));
	lastcamnum = -1;
	GLOBAL_playing_cutseq = 0;
	cutseq_trig = 0;
	d1 = aReadCutData(n1, file);
	d2 = aReadCutData(n2, file);
	d3 = aReadCutData(n3, file);
	d4 = aReadCutData(n4, file);
	CLOSE(file);

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
			READ(tsv_buffer, 1, 2048, file);
			data = aReadCutData(n, file);
			CLOSE(file);
		}
	}

	return data;
}

#pragma warning(push)
#pragma warning(disable : 4244)

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
			PrintString(phd_winwidth >> 1, y + phd_centery, 6, SCRIPT_TEXT_bis(*c & 0x7FFF), FF_CENTER);
		else
			PrintString(phd_winwidth >> 1, y + phd_centery, 2, CreditNames[*c], FF_CENTER);

		if (*c & 0x8000)
			PrintBigString(phd_winwidth >> 1, y2 + phd_centery, 6, SCRIPT_TEXT_bis(*c & 0x7FFF), FF_CENTER);
		else
			PrintBigString(phd_winwidth >> 1, y2 + phd_centery, 2, CreditNames[*c], FF_CENTER);

		y += CREDIT_FONT_HEIGHT;
		y2 += CREDIT_FONT_HEIGHT * 2;
	}

	pos++;
	return 1;
}
#pragma warning(pop)

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
}
