#pragma once
#include "../global/vars.h"

void inject_functbl(bool replace);

void InitialiseFunctionTable();
void HWInitialise();
HRESULT HWBeginScene();
HRESULT HWEndScene();
bool _NVisible(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2);
bool _Visible(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2);
void SetCullCW();
void SetCullCCW();
void SetFogColor(long r, long g, long b);

#define AddQuadSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x00876C04 )
#define AddQuadZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x00876C08 )
#define AddTriZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x00876C10 )
#define AddTriSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x00876BFC )
#define AddLineSorted	( *(void(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, long)) 0x00876C00 )
#define IsVisible	( *(bool(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, D3DTLVERTEX*)) 0x00876C14 )
#define _BeginScene	( *(HRESULT(__cdecl**)()) 0x00876C18 )
#define _EndScene	( *(HRESULT(__cdecl**)()) 0x00876C0C )
