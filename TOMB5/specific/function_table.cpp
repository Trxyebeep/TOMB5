#include "../tomb5/pch.h"
#include "function_table.h"
#include "polyinsert.h"
#include "function_stubs.h"
#include "dxshell.h"

static long CurrentFog;

void InitialiseFunctionTable()
{
	_BeginScene = HWBeginScene;
	_EndScene = HWEndScene;
	IsVisible = _NVisible;

	if (App.dx.lpZBuffer)
	{
		AddQuadZBuffer = AddQuadClippedZBuffer;
		AddTriZBuffer = AddTriClippedZBuffer;
		AddQuadSorted = AddQuadClippedSorted;
		AddTriSorted = AddTriClippedSorted;
	}
	else
	{
		AddQuadZBuffer = AddQuadSubdivide;
		AddTriZBuffer = AddTriSubdivide;
		AddQuadSorted = AddQuadSubdivide;
		AddTriSorted = AddTriSubdivide;		
	}

	AddLineSorted = AddLineClippedSorted;
}

void HWInitialise()
{
	Log(2, "HWIntialise");	//nice typo
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);	//disable all stages
	App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(7, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	if (App.Filtering)
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	}
	else
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);
	}

	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	//multiply
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	//texture color
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);	//diffuse color
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);	//texture factor(?)
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);	//diffuse color
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);	//disable alpha blending
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);	//enable specular
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);	//no culling

	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE));	//z buffering
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1));
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1));	//perspective correction

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);	//Modulate-alpha texture-blending
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);	//Fill solids.
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, 1);	//enable dithering
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);	//reference value for alpha testing
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);	//accept pixels with different alpha(?)
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);	//disables alpha tests

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORVERTEX, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORMODEL, D3DCOLOR_RGB));

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGSTART, (DWORD)FogStart));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGEND, (DWORD)FogEnd));
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0xFF000000);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
}

HRESULT HWBeginScene()
{
	if (App.dx.InScene)
		Log(1, "Already In Scene");

	App.dx.InScene = 1;
	App.dx.DoneBlit = 0;
	while (App.dx.WaitAtBeginScene) {};
	return App.dx.lpD3DDevice->BeginScene();
}

HRESULT HWEndScene()
{
	App.dx.InScene = 0;
	return App.dx.lpD3DDevice->EndScene();
}

bool _NVisible(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2)
{
	return (v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) < 0;
}

bool _Visible(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2)
{
	return (v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) > 0;
}

void SetCullCW()
{
	IsVisible = _Visible;
}

void SetCullCCW()
{
	IsVisible = _NVisible;
}

void SetFogColor(long r, long g, long b)
{
	r &= 0xFF;
	g &= 0xFF;
	b &= 0xFF;
	CurrentFog = RGBA(r, g, b, 0xFF);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, CurrentFog);
}

void inject_functbl(bool replace)
{
	INJECT(0x004A7EE0, InitialiseFunctionTable, replace);
	INJECT(0x004A8040, HWInitialise, replace);
	INJECT(0x004A7FA0, HWBeginScene, replace);
	INJECT(0x004A8010, HWEndScene, replace);
	INJECT(0x004A7E00, _NVisible, replace);
	INJECT(0x004A7E50, _Visible, replace);
	INJECT(0x004A7EA0, SetCullCW, replace);
	INJECT(0x004A7EC0, SetCullCCW, replace);
	INJECT(0x004A84A0, SetFogColor, replace);
}
