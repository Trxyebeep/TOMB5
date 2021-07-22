#pragma once
#include "../global/vars.h"

void inject_others(bool replace);

void aLoadRoomStream();
void aFixUpRoom(ROOM_INFO* r, char* s);

#define PlayFmvNow	( (void(__cdecl*)(int)) 0x004A79A0 )
