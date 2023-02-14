#pragma once
#include "../global/types.h"

void aFixUpRoom(ROOM_INFO* r, char* s);
void aUpdate();
void aTransformClip_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx, long nClip);
void aTransform_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx);
void aInit();
char* aReadCutData(long n, FILE* file);
long aCalcDepackBufferSz(char* data);
void aMakeCutsceneResident(long n1, long n2, long n3, long n4);
char* aFetchCutData(long n);
long DoCredits();

extern long aWibble;
