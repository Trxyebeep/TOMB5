#pragma once
#include "../global/vars.h"

void inject_alexstuff(bool replace);

void aLoadRoomStream();
void aFixUpRoom(ROOM_INFO* r, char* s);
void aUpdate();
void aInitWater();

#define aFetchCutData	( (char*(__cdecl*)(long)) 0x00491F60 )
#define DoCredits	( (long(__cdecl*)()) 0x004927C0 )
