#include "../tomb5/pch.h"
#include "alexstuff.h"
#include "function_stubs.h"
#include "profiler.h"

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

void inject_alexstuff(bool replace)
{
	INJECT(0x004916C0, aLoadRoomStream, replace);
	INJECT(0x004917D0, aFixUpRoom, replace);
	INJECT(0x00491BE0, aUpdate, replace);
	INJECT(0x00491950, aInitWater, replace);
}
