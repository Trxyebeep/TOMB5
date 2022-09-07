#pragma once
#include "../global/vars.h"

void inject_drawroom(bool replace);

void DrawBoundsRectangle(float left, float top, float right, float bottom);
void DrawBoundsRectangleII(float left, float top, float right, float bottom, long rgba);
void DrawClipRectangle(ROOM_INFO* r);
void InsertRoom(ROOM_INFO* r);
void InsertRoomlet(ROOMLET* roomlet);
void RoomTestThing();
void aRoomletTransformLight(float* verts, long nVerts, long nLights, long nWaterVerts, long nShoreVerts);
void aBuildFogBulbList();
void ProcessMeshData(long num_meshes);
void DrawRoomletBounds(ROOMLET* r);
long aBuildRoomletLights(ROOMLET* r);
void aRoomInit();
void aResetFogBulbList();
void TriggerFogBulbFX(long r, long g, long b, long x, long y, long z, long rad, long den);
void aBuildFXFogBulbList();
void InitBuckets();
void aSetBumpComponent(TEXTUREBUCKET* bucket);
void aResetBumpComponent(TEXTUREBUCKET* bucket);

#define DrawBuckets	( (void(__cdecl*)()) 0x0049D750 )
#define CheckBoundsClip	( (long(__cdecl*)(float*)) 0x0049C6B0 )
