#pragma once
#include "../global/vars.h"

void inject_drawroom(bool replace);

void DrawBoundsRectangle(float left, float top, float right, float bottom);
void DrawBoundsRectangleII(float left, float top, float right, float bottom, int rgba);
void DrawClipRectangle(ROOM_INFO* r);

#define DrawBuckets	( (void(__cdecl*)()) 0x0049D750 )

