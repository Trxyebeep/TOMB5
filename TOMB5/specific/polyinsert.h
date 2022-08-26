#pragma once
#include "../global/vars.h"

void inject_polyinsert(bool replace);

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CalcColorSplit(D3DCOLOR s, D3DCOLOR* d);
void InitialiseSortList();
void DoSort(long left, long right, SORTLIST** list);
void SortPolyList(long count, SORTLIST** list);

#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BC7F0 )
#define IsVolumetric	( (char(__cdecl*)()) 0x004BA1B0 )
#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BC7F0 )
#define AddQuadSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BBFA0 )
#define AddTriSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BBE40 )
#define AddTriClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BC120  )
#define AddTriClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BA960 )
#define AddQuadClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BA300 )
#define AddLineClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, D3DTLVERTEX*, long)) 0x004BCE20 )
#define InitialiseFogBulbs	( (void(__cdecl*)()) 0x004BA2A0 )
#define CreateFXBulbs	( (void(__cdecl*)()) 0x004BA170 )
#define ClearFXFogBulbs	( (void(__cdecl*)()) 0x004BA130 )
