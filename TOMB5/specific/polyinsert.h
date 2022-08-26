#pragma once
#include "../global/vars.h"

void inject_polyinsert(bool replace);

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CalcColorSplit(D3DCOLOR s, D3DCOLOR* d);
void InitialiseSortList();
void DoSort(long left, long right, SORTLIST** list);
void SortPolyList(long count, SORTLIST** list);
void ClearFXFogBulbs();
void ControlFXBulb();
void CreateFXBulbs();
void TriggerFXFogBulb();
long IsVolumetric();
void mD3DTransform(FVECTOR* vec, D3DMATRIX* mx);
void CreateFogPos();
long DistCompare();
void InitialiseFogBulbs();
void OmniEffect();
void OmniFog();

#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BC7F0 )
#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BC7F0 )
#define AddQuadSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BBFA0 )
#define AddTriSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BBE40 )
#define AddTriClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BC120  )
#define AddTriClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x004BA960 )
#define AddQuadClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004BA300 )
#define AddLineClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, D3DTLVERTEX*, long)) 0x004BCE20 )
