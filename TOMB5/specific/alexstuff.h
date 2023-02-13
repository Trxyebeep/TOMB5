#pragma once
#include "../global/types.h"

void inject_alexstuff(bool replace);

void aLoadRoomStream();
void aFixUpRoom(ROOM_INFO* r, char* s);
void aUpdate();
void aInitWater();
void aTransformClip_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx, long nClip);
void aTransform_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx);
void aInit();
void aWinString(long x, long y, char* string);
char* aReadCutData(long n, FILE* file);
long aCalcDepackBufferSz(char* data);
void aMakeCutsceneResident(long n1, long n2, long n3, long n4);
char* aFetchCutData(long n);
long DoCredits();
void DrawBigChar(short x, short y, ushort col, CHARDEF* c, long scale);
long GetBigStringLength(const char* string, short* top, short* bottom);
void PrintBigString(ushort x, ushort y, uchar col, const char* string, ushort flags);
void aProcessWater(long n);

extern long aWibble;
