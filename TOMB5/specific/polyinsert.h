#pragma once
#include "../global/types.h"

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CalcColorSplit(D3DCOLOR s, D3DCOLOR* d);
void InitialiseSortList();
void DoSort(long left, long right, SORTLIST** list);
void SortPolyList(long count, SORTLIST** list);
void AddClippedPoly(D3DTLBUMPVERTEX* dest, long nPoints, D3DTLBUMPVERTEX* v, TEXTURESTRUCT* pTex);
void SubdivideEdge(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v, short* c, float tu1, float tv1, float tu2, float tv2, float* tu, float* tv);
void SubdivideQuad(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, D3DTLVERTEX* v3, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void SubdivideTri(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void AddTriSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void AddQuadClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void AddTriClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddLineClippedSorted(D3DTLVERTEX* v0, D3DTLVERTEX* v1, short drawtype);
void AddQuadClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void AddTriClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);

extern SORTLIST* SortList[65536];
extern long SortCount;
extern D3DTLBUMPVERTEX XYUVClipperBuffer[20];
extern D3DTLBUMPVERTEX zClipperBuffer[20];
