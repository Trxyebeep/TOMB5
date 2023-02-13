#pragma once
#include "../global/types.h"

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
void DrawBucket(TEXTUREBUCKET* bucket);
void DrawBuckets();
void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp);
long CheckBoundsClip(float* box);
void PrelightVertsMMXByRoomlet(D3DTLVERTEX* v, ROOMLET* r);
void PrelightVertsNonMMXByRoomlet(D3DTLVERTEX* v, ROOMLET* r);
void CalcTriFaceNormal(D3DVECTOR* p1, D3DVECTOR* p2, D3DVECTOR* p3, D3DVECTOR* n);
void CreateVertexNormals(MESH_DATA* mesh);

extern FOGBULB_STRUCT ActiveFogBulbs[64];
extern FOGBULB_STRUCT FogBulbs[16];
extern short CheckClipBox[8 * 3];
extern MESH_DATA** mesh_vtxbuf;
extern TEXTUREBUCKET Bucket[30];
extern float clip_left;
extern float clip_top;
extern float clip_right;
extern float clip_bottom;
extern long NumActiveFogBulbs;
extern long num_level_meshes;
