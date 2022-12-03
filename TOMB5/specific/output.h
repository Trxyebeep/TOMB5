#pragma once
#include "../global/vars.h"

void inject_output(bool replace);

void S_DrawPickup(short object_number);
void phd_PutPolygons(short* objptr, long clipstatus);
void phd_PutPolygonSkyMesh(short* objptr, long clipstatus);
void aTransformLightClipMesh(MESH_DATA* mesh);
void phd_PutPolygonsPickup(short* objptr, float x, float y, long color);
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

#define phd_PutPolygons_seethrough	( (void(__cdecl*)(short*, long)) 0x004B4F10 )
#define phd_PutPolygonsSpcEnvmap	( (void(__cdecl*)(short*, long)) 0x004B5190 )
#define phd_PutPolygonsSpcXLU	( (void(__cdecl*)(short*, long)) 0x004B4CA0 )
#define S_OutputPolyList	( (void(__cdecl*)()) 0x004B79A0 )
