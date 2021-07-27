#pragma once
#include "../global/vars.h"

void inject_functbl(bool replace);

void InitialiseFunctionTable();

#define AddQuadSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C04 )
#define AddQuadZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, int)) 0x00876C08 )
#define AddTriZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876C10 )
#define AddTriSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, int)) 0x00876BFC )
#define AddLineSorted	( *(void(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, int)) 0x00876C00 )
#define IsVisible	( *(bool(__cdecl**)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x00876C14 )
#define BeginScene	( *(HRESULT(__cdecl**)()) 0x00876C18 )
#define EndScene	( *(HRESULT(__cdecl**)()) 0x00876C0C )
#define _NVisible	( (bool(__cdecl*)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x004A7E00 )
#define HWBeginScene	( (HRESULT(__cdecl*)()) 0x004A7FA0 )
#define HWEndScene	( (HRESULT(__cdecl*)()) 0x004A8010 )
#define	SetFogColor	( (void(__cdecl*)(char, char, char)) 0x004A84A0 )
