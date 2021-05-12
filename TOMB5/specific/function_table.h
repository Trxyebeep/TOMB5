#pragma once
#include "../global/types.h"

void inject_functbl();

void InitialiseFunctionTable();

#define AddQuadSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C04 )
#define AddQuadZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C08 )
#define AddTriZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876C10 )
#define AddTriSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876BFC )
#define AddLineSorted	( *(void(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, int)) 0x00876C00 )
#define IsVisible	( *(bool(__cdecl**)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x00876C00 )
#define BeginScene	( *(HRESULT(__cdecl**)()) 0x00876C18 )
#define EndScene	( *(HRESULT(__cdecl**)()) 0x00876C0C )

//mess!
#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x004BC7F0 )
#define AddQuadSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x004BBFA0 )
#define AddTriSubdivide	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x004BBE40 )
#define AddTriClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x004BC120  )
#define AddTriClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x004BA960 )
#define AddQuadClippedZBuffer	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x004BA300 )
#define AddLineClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, D3DTLVERTEX*, int)) 0x004BCE20 )
#define _NVisible	( (bool(__cdecl*)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x004A7E00 )
#define HWBeginScene	( (HRESULT(__cdecl*)()) 0x004A7FA0 )
#define HWEndScene	( (HRESULT(__cdecl*)()) 0x004A8010 )
