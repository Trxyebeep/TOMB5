#include "../tomb5/pch.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "lara_states.h"
#include "control.h"
#include "objects.h"
#include "lara.h"
#include "../tomb5/tomb5.h"

short* GLaraShadowframe;
float lara_matrices[180];
float lara_joint_matrices[180];
long LaraNodeAmbient[2];
uchar LaraNodeUnderwater[15];
char SkinVertNums[40][12];
char ScratchVertNums[40][12];

void GetLaraJointPos(PHD_VECTOR* pos, long node)
{
	phd_PushMatrix();
	aMXPtr[M00] = lara_joint_matrices[node * indices_count + M00];
	aMXPtr[M01] = lara_joint_matrices[node * indices_count + M01];
	aMXPtr[M02] = lara_joint_matrices[node * indices_count + M02];
	aMXPtr[M03] = lara_joint_matrices[node * indices_count + M03];
	aMXPtr[M10] = lara_joint_matrices[node * indices_count + M10];
	aMXPtr[M11] = lara_joint_matrices[node * indices_count + M11];
	aMXPtr[M12] = lara_joint_matrices[node * indices_count + M12];
	aMXPtr[M13] = lara_joint_matrices[node * indices_count + M13];
	aMXPtr[M20] = lara_joint_matrices[node * indices_count + M20];
	aMXPtr[M21] = lara_joint_matrices[node * indices_count + M21];
	aMXPtr[M22] = lara_joint_matrices[node * indices_count + M22];
	aMXPtr[M23] = lara_joint_matrices[node * indices_count + M23];
	phd_TranslateRel(pos->x, pos->y, pos->z);
	pos->x = (long)aMXPtr[M03];
	pos->y = (long)aMXPtr[M13];
	pos->z = (long)aMXPtr[M23];
	pos->x += lara_item->pos.x_pos;
	pos->y += lara_item->pos.y_pos;
	pos->z += lara_item->pos.z_pos;
	phd_PopMatrix();
}

void CalcLaraMatrices(long flag)
{
	long* bone;
	short* frame;
	short* frmptr[2];
	long rate, frac;
	short spaz;
	
	bone = &bones[objects[lara_item->object_number].bone_index];
	frac = GetFrames(lara_item, frmptr, &rate);

	if (lara.hit_direction < 0)
	{
		if (frac)
		{
			GLaraShadowframe = GetBoundsAccurate(lara_item);
			Rich_CalcLaraMatrices_Interpolated(frmptr[0], frmptr[1], frac, rate, bone, flag);
			return;
		}
	}

	if (lara.hit_direction < 0)
		frame = *frmptr;
	else
	{
		if (!lara.hit_direction)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKF : ANIM_SPAZ_FORWARD;
		else if (lara.hit_direction == 1)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKR : ANIM_SPAZ_RIGHT;
		else if (lara.hit_direction == 2)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKB : ANIM_SPAZ_BACK;
		else
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKL : ANIM_SPAZ_LEFT;

		frame = &anims[spaz].frame_ptr[lara.hit_frame * (anims[spaz].interpolation >> 8)];
	}

	Rich_CalcLaraMatrices_Normal(frame, bone, flag);
}

void Rich_CalcLaraMatrices_Normal(short* frame, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* mx;
	short* rot;
	short* rot2;
	short gun;

	if (flag == 1)
		mx = lara_joint_matrices;
	else
		mx = lara_matrices;

	phd_PushMatrix();

	if (!flag || flag == 2)
		phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.z = -16384;
		vec.y = -16384;
		vec.x = -16384;
		ScaleCurrentMatrix(&vec);
	}

	phd_PushMatrix();
	rot = &frame[9];
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == WEAPON_HK &&
		(items[lara.weapon_item].current_anim_state == 0 || items[lara.weapon_item].current_anim_state == 1 ||
			items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 3 ||
			items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 7);
	}
	else
		gar_RotYXZsuperpack(&rot, 0);

	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	memcpy(mx, aMXPtr, 48);
	mx += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	rot2 = rot;
	gar_RotYXZsuperpack(&rot2, 6);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];

			gar_RotYXZsuperpack(&rot, 11);
		}
		else
			gar_RotYXZsuperpack(&rot, 0);

		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];	//going back 2 matrices
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) * 
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) * 
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		aMXPtr[M00] = aMXPtr[-2 * indices_count + M00];
		aMXPtr[M01] = aMXPtr[-2 * indices_count + M01];
		aMXPtr[M02] = aMXPtr[-2 * indices_count + M02];
		aMXPtr[M10] = aMXPtr[-2 * indices_count + M10];
		aMXPtr[M11] = aMXPtr[-2 * indices_count + M11];
		aMXPtr[M12] = aMXPtr[-2 * indices_count + M12];
		aMXPtr[M20] = aMXPtr[-2 * indices_count + M20];
		aMXPtr[M21] = aMXPtr[-2 * indices_count + M21];
		aMXPtr[M22] = aMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_HK:
	case WEAPON_CROSSBOW:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * 
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;
	}
	
	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
	GLaraShadowframe = frame;
}

void Rich_CalcLaraMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* mx;
	float* arms;
	short* rot;
	short* rot2;
	short* rotcopy;
	short* rot2copy;
	short gun;

	if (flag == 1)
		mx = lara_joint_matrices;
	else
		mx = lara_matrices;

	phd_PushMatrix();
	arms = aMXPtr;

	if (!flag || flag == 2)
		phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.z = -0x4000;
		vec.y = -0x4000;
		vec.x = -0x4000;
		ScaleCurrentMatrix(&vec);
	}

	phd_PushMatrix();
	rot = frame1 + 9;
	rot2 = frame2 + 9;
	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == WEAPON_HK &&
		(items[lara.weapon_item].current_anim_state == 0 || items[lara.weapon_item].current_anim_state == 1 ||
			items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 3 ||
			items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot2 = rot;
		gar_RotYXZsuperpack_I(&rot, &rot2, 7);
	}
	else
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	rotcopy = rot;
	rot2copy = rot2;
	gar_RotYXZsuperpack_I(&rotcopy, &rot2copy, 6);
	phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	memcpy(mx, aMXPtr, 48);
	mx += 12;
	phd_PopMatrix();

	phd_PopMatrix_I();
	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot2 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			rot = rot2;
			gar_RotYXZsuperpack_I(&rot, &rot2, 11);
		}
		else
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		aInterpolateArmMatrix(arms);
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) * 
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		aInterpolateArmMatrix(arms);
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		aInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) * 
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		aInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) * 
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_HK:
	case WEAPON_CROSSBOW:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		rot2 = &lara.right_arm.frame_base[lara.right_arm.frame_number * 
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot = rot2;
		gar_RotYXZsuperpack_I(&rot, &rot2, 8);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		memcpy(mx, aMXPtr, 48);
		mx += 12;
		phd_PopMatrix();
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}
