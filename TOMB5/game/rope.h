#pragma once
#include "../global/vars.h"

void inject_rope(bool replace);

void InitialiseRope(short item_number);
void CreateRope(ROPE_STRUCT* rope, PHD_VECTOR* pos, PHD_VECTOR* dir, long slength, ITEM_INFO* item);
PHD_VECTOR* Normalise(PHD_VECTOR* v);
void GetRopePos(ROPE_STRUCT* rope, int pos, long* x, long* y, long* z);
long mDotProduct(PHD_VECTOR* a, PHD_VECTOR* b);
void vMul(PHD_VECTOR* v, long scale, PHD_VECTOR* d);
void mCrossProduct(PHD_VECTOR* a, PHD_VECTOR* b, PHD_VECTOR* n);
void phd_GetMatrixAngles(MATRIX3D* m, short* dest);
void RopeControl(short item_number);
void RopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CalculateRope(ROPE_STRUCT* Rope);
int RopeNodeCollision(ROPE_STRUCT* rope, long x, long y, long z, long rad);
void SetPendulumVelocity(int x, int y, int z);
void SetPendulumPoint(ROPE_STRUCT* Rope, int node);
void ModelRigidRope(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
void ModelRigid(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
void AlignLaraToRope(ITEM_INFO* l);
void LaraClimbRope(ITEM_INFO* item, COLL_INFO* coll);
