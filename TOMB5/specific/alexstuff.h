#pragma once
#include "../global/vars.h"

void inject_alexstuff(bool replace);

void aLoadRoomStream();
void aFixUpRoom(ROOM_INFO* r, char* s);
void aUpdate();
void aInitWater();
void aTransformClip_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx, long nClip);
void aTransform_D3DV(D3DVECTOR* vec, D3DTLVERTEX* v, long nVtx);
void aInit();

#define aFetchCutData	( (char*(__cdecl*)(long)) 0x00491F60 )
#define DoCredits	( (long(__cdecl*)()) 0x004927C0 )
