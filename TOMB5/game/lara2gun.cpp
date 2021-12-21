#include "../tomb5/pch.h"
#include "lara2gun.h"
#include "larafire.h"
#include "objects.h"
#include "delstuff.h"
#include "tomb4fx.h"
#include "sound.h"

static PISTOL_DEF PistolTable[4] =
{
	{ LARA, 0, 0, 0, 0 },
	{ PISTOLS_ANIM, 4, 5, 13, 24 },
	{ REVOLVER_ANIM , 7, 8, 15, 29 },
	{ UZI_ANIM, 4, 5, 13, 24 }
};

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

void AnimatePistols(long weapon_type)
{
	PISTOL_DEF* p;
	WEAPON_INFO* winfo;
	PHD_VECTOR pos;
	static long uzi_left;
	static long uzi_right;
	short angles[2];
	short anil, anir, sound_already;

	sound_already = 0;

	if (lara_item->mesh_bits)
	{
		if (SmokeCountL)
		{
			switch (SmokeWeapon)
			{
			case WEAPON_PISTOLS:
				pos.x = 4;
				pos.y = 128;
				pos.z = 40;
				break;

			case WEAPON_REVOLVER:
				pos.x = 16;
				pos.y = 160;
				pos.z = 56;
				break;

			case WEAPON_UZI:
				pos.x = 8;
				pos.y = 140;
				pos.z = 48;
				break;
			}

			GetLaraJointPos(&pos, 14);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
		}

		if (SmokeCountR)
		{
			switch (SmokeWeapon)
			{
			case WEAPON_PISTOLS:
				pos.x = -16;
				pos.y = 128;
				pos.z = 40;
				break;

			case WEAPON_REVOLVER:
				pos.x = -32;
				pos.y = 160;
				pos.z = 56;
				break;

			case WEAPON_UZI:
				pos.x = -16;
				pos.y = 140;
				pos.z = 48;
				break;
			}

			GetLaraJointPos(&pos, 11);
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
		}
	}

	p = &PistolTable[lara.gun_type];
	winfo = &weapons[weapon_type];
	anir = lara.right_arm.frame_number;

	if (lara.right_arm.lock || input & IN_ACTION && !lara.target)
	{
		if (lara.right_arm.frame_number >= 0 && lara.right_arm.frame_number < p->Draw1Anim2)
			anir++;
		else if (lara.right_arm.frame_number == p->Draw1Anim2)
		{
			if (input & IN_ACTION)
			{
				if (weapon_type != WEAPON_REVOLVER)
				{
					angles[0] = lara.right_arm.y_rot + lara_item->pos.y_rot;
					angles[1] = lara.right_arm.x_rot;

					if (FireWeapon(weapon_type, lara.target, lara_item, angles))
					{
						SmokeCountR = 28;
						SmokeWeapon = weapon_type;
						TriggerGunShell(1, 369, weapon_type);
						lara.right_arm.flash_gun = winfo->flash_time;
						SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
						SoundEffect(winfo->sample_num, &lara_item->pos, SFX_DEFAULT);
						sound_already = 1;

						if (weapon_type == WEAPON_UZI)
							uzi_right = 1;

						savegame.Game.AmmoUsed++;
					}
				}

				anir = p->RecoilAnim;
			}
			else if (uzi_right)
			{
				SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
				uzi_right = 0;
			}
		}
		else if (lara.right_arm.frame_number >= p->RecoilAnim)
		{
			if (weapon_type == WEAPON_UZI)
			{
				SoundEffect(winfo->sample_num, &lara_item->pos, SFX_DEFAULT);
				uzi_right = 1;
			}

			anir++;

			if (anir == p->RecoilAnim + winfo->recoil_frame)
				anir = p->Draw1Anim2;
		}
	}
	else
	{
		if (lara.right_arm.frame_number >= p->RecoilAnim)
			anir = p->Draw1Anim2;
		else if (lara.right_arm.frame_number > 0 && lara.right_arm.frame_number <= p->Draw1Anim2)
			anir--;

		if (uzi_right)
		{
			SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
			uzi_right = 0;
		}
	}

	set_arm_info(&lara.right_arm, anir);
	anil = lara.left_arm.frame_number;

	if (lara.left_arm.lock || input & IN_ACTION && !lara.target)
	{
		if (lara.left_arm.frame_number >= 0 && lara.left_arm.frame_number < p->Draw1Anim2)
			anil++;
		else if (lara.left_arm.frame_number == p->Draw1Anim2)
		{
			if (input & IN_ACTION)
			{
				angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
				angles[1] = lara.left_arm.x_rot;

				if (FireWeapon(weapon_type, lara.target, lara_item, angles))
				{
					if (weapon_type == WEAPON_REVOLVER)
					{
						SmokeCountR = 28;
						SmokeWeapon = WEAPON_REVOLVER;
						lara.right_arm.flash_gun = winfo->flash_time;
					}
					else
					{
						SmokeCountL = 28;
						SmokeWeapon = weapon_type;
						TriggerGunShell(0, 369, weapon_type);
						lara.left_arm.flash_gun = winfo->flash_time;
					}

					if (!sound_already)
					{
						SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
						SoundEffect(winfo->sample_num, &lara_item->pos, 0);
					}

					if (weapon_type == WEAPON_UZI)
						uzi_left = 1;

					savegame.Game.AmmoUsed++;
				}

				anil = p->RecoilAnim;
			}
		}
		else if (lara.left_arm.frame_number >= p->RecoilAnim)
		{
			if (weapon_type == WEAPON_UZI)
			{
				SoundEffect(winfo->sample_num, &lara_item->pos, SFX_DEFAULT);
				uzi_left = 1;
			}

			anil++;

			if (anil == p->RecoilAnim + winfo->recoil_frame)
				anil = p->Draw1Anim2;
		}
	}
	else
	{
		if (lara.left_arm.frame_number >= p->RecoilAnim)
			anil = p->Draw1Anim2;
		else if (lara.left_arm.frame_number > 0 && lara.left_arm.frame_number <= p->Draw1Anim2)
			anil--;

		if (uzi_left)
		{
			SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
			uzi_left = 0;
		}
	}

	set_arm_info(&lara.left_arm, anil);
}

void inject_lara2gun(bool replace)
{
	INJECT(0x0044FDD0, ready_pistols, replace);
	INJECT(0x0044FE60, draw_pistol_meshes, replace);
	INJECT(0x0044FED0, undraw_pistol_mesh_left, replace);
	INJECT(0x0044FF40, undraw_pistol_mesh_right, replace);
	INJECT(0x004502B0, AnimatePistols, replace);
}
