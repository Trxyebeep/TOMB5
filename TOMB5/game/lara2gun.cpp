#include "../tomb5/pch.h"
#include "lara2gun.h"
#include "larafire.h"
#include "objects.h"

void ready_pistols(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
}

void draw_pistol_meshes(long weapon_type)
{
	long mesh_index;

	mesh_index = objects[WeaponObjectMesh(weapon_type)].mesh_index;
	lara.holster = LARA_HOLSTERS;
	lara.mesh_ptrs[LM_RHAND] = meshes[mesh_index + LM_RHAND * 2];

	if (weapon_type != WEAPON_REVOLVER)
		lara.mesh_ptrs[LM_LHAND] = meshes[mesh_index + LM_LHAND * 2];
}

void undraw_pistol_mesh_left(long weapon_type)
{
	WeaponObject(weapon_type);	//ok core
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];

	if (weapon_type == WEAPON_PISTOLS)
		lara.holster = LARA_HOLSTERS_PISTOLS;
	else if (weapon_type == WEAPON_UZI)
		lara.holster = LARA_HOLSTERS_UZIS;
}

void undraw_pistol_mesh_right(long weapon_type)
{
	WeaponObject(weapon_type);
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + LM_RHAND * 2];

	if (weapon_type == WEAPON_PISTOLS)
		lara.holster = LARA_HOLSTERS_PISTOLS;
	else if (weapon_type == WEAPON_UZI)
		lara.holster = LARA_HOLSTERS_UZIS;
	else if (weapon_type == WEAPON_REVOLVER)
		lara.holster = LARA_HOLSTERS_REVOLVER;
}

void inject_lara2gun(bool replace)
{
	INJECT(0x0044FDD0, ready_pistols, replace);
	INJECT(0x0044FE60, draw_pistol_meshes, replace);
	INJECT(0x0044FED0, undraw_pistol_mesh_left, replace);
	INJECT(0x0044FF40, undraw_pistol_mesh_right, replace);
}
