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

#define S_InitialisePolyList	( (void(__cdecl*)()) 0x004B2110 )
#define	StashSkinVertices	( (void(__cdecl*)(long)) 0x004B2270 )
#define StashSkinNormals	( (void(__cdecl*)(long)) 0x004B2410 )
#define SkinVerticesToScratch	( (void(__cdecl*)(long)) 0x004B2340 )
#define phd_PutPolygons_seethrough	( (void(__cdecl*)(short*, long)) 0x004B4F10 )
#define phd_PutPolygonsSpcEnvmap	( (void(__cdecl*)(short*, long)) 0x004B5190 )
#define phd_PutPolygonsSpcXLU	( (void(__cdecl*)(short*, long)) 0x004B4CA0 )
#define SkinNormalsToScratch	( (void(__cdecl*)(long)) 0x004B2480 )
#define S_OutputPolyList	( (void(__cdecl*)()) 0x004B79A0 )
#define S_DumpScreen	( (long(__cdecl*)()) 0x004B7DA0 )
#define do_boot_screen	( (void(__cdecl*)(long)) 0x004B8A80 )
#define S_DumpScreenFrame	( (void(__cdecl*)()) 0x004B7E40 )
