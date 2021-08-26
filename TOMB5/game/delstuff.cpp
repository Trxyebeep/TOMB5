#include "../tomb5/pch.h"
#include "delstuff.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "lara_states.h"

void GetLaraJointPos(PHD_VECTOR* pos, long node)
{
	phd_PushMatrix();
	phd_mxptr[0] = lara_joint_matrices[node].m00;
	phd_mxptr[1] = lara_joint_matrices[node].m01;
	phd_mxptr[2] = lara_joint_matrices[node].m02;
	phd_mxptr[3] = lara_joint_matrices[node].m03;
	phd_mxptr[4] = lara_joint_matrices[node].m10;
	phd_mxptr[5] = lara_joint_matrices[node].m11;
	phd_mxptr[6] = lara_joint_matrices[node].m12;
	phd_mxptr[7] = lara_joint_matrices[node].m13;
	phd_mxptr[8] = lara_joint_matrices[node].m20;
	phd_mxptr[9] = lara_joint_matrices[node].m21;
	phd_mxptr[10] = lara_joint_matrices[node].m22;
	phd_mxptr[11] = lara_joint_matrices[node].m23;
	phd_TranslateRel(pos->x, pos->y, pos->z);
	pos->x = phd_mxptr[3] >> 14;
	pos->y = phd_mxptr[7] >> 14;
	pos->z = phd_mxptr[11] >> 14;
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
	short jerk;

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

	if (lara.hit_direction < 0)//???????????????????????????????????????????
		frame = *frmptr;
	else
	{
		if (lara.hit_direction == 0)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKF : ANIM_JERK_FORWARD;
		else if (lara.hit_direction == 1)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKR : ANIM_JERK_RIGHT;
		else if (lara.hit_direction == 2)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKB : ANIM_JERK_BACK;
		else
			jerk = lara.IsDucked ? ANIM_JERK_DUCKL : ANIM_JERK_LEFT;

		frame = &anims[jerk].frame_ptr[lara.hit_frame * (anims[jerk].interpolation >> 8)];
	}

	Rich_CalcLaraMatrices_Normal(frame, bone, flag);
}

void inject_delshit(bool replace)
{
	INJECT(0x0041E2A0, GetLaraJointPos, replace);
	INJECT(0x0041E120, CalcLaraMatrices, replace);
}
