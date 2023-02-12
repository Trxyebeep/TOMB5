#pragma once
#include "../global/vars.h"

void inject_debris(bool replace);

void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);
long GetFreeDebris();
void UpdateDebris();
void ShatterObject(SHATTER_ITEM* shatter_item, MESH_INFO* StaticMesh, short Num, short RoomNumber, long NoXZVel);

extern short DebrisFlags;
