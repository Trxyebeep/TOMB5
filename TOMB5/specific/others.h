#pragma once
#include "../global/vars.h"

void inject_others();

void aLoadRoomStream();

#define aFixUpRoom	( (void(__cdecl*)(ROOM_INFO*, char*)) 0x004917D0 )
#define PlayFmvNow	( (void(__cdecl*)(int)) 0x004A79A0 )
