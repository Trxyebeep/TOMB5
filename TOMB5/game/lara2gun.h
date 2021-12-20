#pragma once
#include "../global/vars.h"

void inject_lara2gun(bool replace);

void ready_pistols(long weapon_type);
void draw_pistol_meshes(long weapon_type);

#define PistolHandler	( (void(__cdecl*)(int)) 0x0044FFC0 )
#define	undraw_pistols	( (void(__cdecl*)(int)) 0x0044FAC0 )
#define	draw_pistols	( (void(__cdecl*)(int)) 0x0044F950 )
#define undraw_pistol_mesh_left	( (void(__cdecl*)(int)) 0x0044FED0 )
#define undraw_pistol_mesh_right	( (void(__cdecl*)(int)) 0x0044FF40 )
#define AnimatePistols	( (void(__cdecl*)(int)) 0x004502B0 )
