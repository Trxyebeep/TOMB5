#pragma once
#include "../global/vars.h"

void inject_lara2gun(bool replace);

void ready_pistols(long weapon_type);
void draw_pistol_meshes(long weapon_type);
void undraw_pistol_mesh_left(long weapon_type);
void undraw_pistol_mesh_right(long weapon_type);
void AnimatePistols(long weapon_type);
void PistolHandler(long weapon_type);
void draw_pistols(long weapon_type);

#define set_arm_info	( (void(__cdecl*)(LARA_ARM*, long)) 0x0044FA20 )
#define	undraw_pistols	( (void(__cdecl*)(int)) 0x0044FAC0 )
