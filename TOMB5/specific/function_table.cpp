#include "../tomb5/pch.h"
#include "function_table.h"

void InitialiseFunctionTable()
{
//	BeginScene = HWBeginScene;
//	EndScene = HWEndScene;
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
