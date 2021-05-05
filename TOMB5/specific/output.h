#pragma once

void inject_output();

void S_DrawPickup(short object_number);

#define S_InitialisePolyList	( (void(__cdecl*)()) 0x004B2110 )
#define	StashSkinVertices	( (void(__cdecl*)(int)) 0x004B2270 )
#define SkinVerticesToScratch	( (void(__cdecl*)(int)) 0x004B2340 )
#define phd_PutPolygons	( (void(__cdecl*)(short*, int)) 0x004B3F00 )
#define phd_PutPolygons_seethrough	( (void(__cdecl*)(short*, int)) 0x004B4F10 )
#define phd_PutPolygonsPickup	( (void(__cdecl*)(short*, float, float, long)) 0x004B66B0 )
#define phd_PutPolygons_train	( (void(__cdecl*)(short*, int)) 0x004B74A0 )
#define phd_PutPolygonsSpcEnvmap	( (void(__cdecl*)(short*, int)) 0x004B5190 )
#define SkinNormalsToScratch	( (void(__cdecl*)(long)) 0x004B2480 )

#define S_OutputPolyList	( (void(__cdecl*)()) 0x004B79A0 )
#define S_DumpScreen	( (long(__cdecl*)()) 0x004B7DA0 )
