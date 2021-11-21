#pragma once
#include "../global/vars.h"

void inject_functbl(bool replace);

void InitialiseFunctionTable();
void HWInitialise();
HRESULT HWBeginScene();
HRESULT HWEndScene();

#define AddQuadSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C04 )
#define AddQuadZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C08 )
#define AddTriZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876C10 )
#define AddTriSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876BFC )
#define AddLineSorted	( *(void(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, int)) 0x00876C00 )
#define IsVisible	( *(bool(__cdecl**)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x00876C14 )
#define _BeginScene	( *(HRESULT(__cdecl**)()) 0x00876C18 )
#define _EndScene	( *(HRESULT(__cdecl**)()) 0x00876C0C )
#define _NVisible	( (bool(__cdecl*)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x004A7E00 )
#define	SetFogColor	( (void(__cdecl*)(char, char, char)) 0x004A84A0 )
