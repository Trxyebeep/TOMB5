#pragma once
#include "../global/types.h"

void S_DrawPickup(short object_number);
void aTransformLightClipMesh(MESH_DATA* mesh);
void aTransformLightPrelightClipMesh(MESH_DATA* mesh);
void phd_PutPolygons_train(short* lol, long x);
void RenderLoadPic(long unused);
long S_GetObjectBounds(short* bounds);
void S_AnimateTextures(long n);
long aCheckMeshClip(MESH_DATA* mesh);
void ProjectTrainVerts(short nVerts, D3DTLVERTEX* v, short* clip, long x);
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, long x, long y, long dx, long dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name);
HRESULT aLoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name);
void do_boot_screen(long language);
void aCalcColorSplit(long col, long* pC, long* pS);
long S_DumpScreen();
long S_DumpScreenFrame();
void SetGlobalAmbient(long ambient);
void PrelightVerts(long nVerts, D3DTLVERTEX* v, MESH_DATA* mesh);
void CalcVertsColorSplitMMX(long nVerts, D3DTLVERTEX* v);
void StashSkinVertices(long node);
void SkinVerticesToScratch(long node);
void StashSkinNormals(long node);
void SkinNormalsToScratch(long node);
void S_InitialisePolyList();
void S_OutputPolyList();
void DebugString(char* txt, ...);
void S_InsertRoom(ROOM_INFO* r, long a);
void phd_PutPolygons(short* objptr, long clipstatus);
void phd_PutPolygonsSkyMesh(short* objptr, long clipstatus);
void phd_PutPolygonsPickup(short* objptr, float x, float y, long color);
void phd_PutPolygons_seethrough(short* objptr, long fade);
void phd_PutPolygonsSpcXLU(short* objptr, long clipstatus);
void phd_PutPolygonsSpcEnvmap(short* objptr, long clipstatus);

extern D3DTLVERTEX aVertexBuffer[1024];
extern long nPolys;
extern long nClippedPolys;
extern long DrawPrimitiveCnt;
extern long DrawSortedCnt;
extern long aGlobalSkinMesh;
extern long GlobalAlpha;
extern long GlobalAmbient;
extern float AnimatingTexturesV[16][8][3];
extern float aBoundingBox[24];
