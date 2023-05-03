#include "../tomb5/pch.h"
#include "alexstuff.h"
#include "function_stubs.h"
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
#include "../game/control.h"
#include "3dmath.h"
#include "gamemain.h"

const char* CreditNames[] =
{
	"nobody",
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
short CreditGroups[15] =
{
	0,
	TXT_Programmers,
	TXT_AI_Programming,
	TXT_Additional_Programmers,
	TXT_Animators,
	TXT_Level_Designers,
	TXT_FMV_Sequences,
	TXT_Music_Sound_FX,
	TXT_Additional_Sound_FX,
	TXT_Original_Story,
	TXT_Script,
	TXT_Producer,
	TXT_QA,
	TXT_Executive_Producers,
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
#pragma warning(pop)

long aWibble;

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
		Log("%X", pR->door);
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
}

void aUpdate()
{
	aWibble++;
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

			if (v->sz > f_mzfar)
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
	aRoomInit();
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
	data = (char*)game_malloc(size);
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
				PrintString(phd_winwidth >> 1, y, 6, SCRIPT_TEXT(CreditGroups[atoi(s + 1)]), FF_CENTER);
			else if (*s != '0')
			{
				if (i >= 57)
					PrintString(phd_winwidth >> 1, y, 2 + (i == 57 ? 4 : 0), s, FF_CENTER);
				else
					PrintString(phd_winwidth >> 1, y, 2, CreditNames[atoi(s + 1)], FF_CENTER);
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
