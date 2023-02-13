#pragma once
#include "../global/types.h"

void InitialiseRope(short item_number);
void CreateRope(ROPE_STRUCT* rope, PHD_VECTOR* pos, PHD_VECTOR* dir, long slength, ITEM_INFO* item);
PHD_VECTOR* Normalise(PHD_VECTOR* v);
void GetRopePos(ROPE_STRUCT* rope, long pos, long* x, long* y, long* z);
long mDotProduct(PHD_VECTOR* a, PHD_VECTOR* b);
void vMul(PHD_VECTOR* v, long scale, PHD_VECTOR* d);
void mCrossProduct(PHD_VECTOR* a, PHD_VECTOR* b, PHD_VECTOR* n);
void phd_GetMatrixAngles(long* m, short* dest);
void RopeControl(short item_number);
void RopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CalculateRope(ROPE_STRUCT* Rope);
long RopeNodeCollision(ROPE_STRUCT* rope, long x, long y, long z, long rad);
void SetPendulumVelocity(long x, long y, long z);
void SetPendulumPoint(ROPE_STRUCT* Rope, long node);
void ModelRigidRope(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
void ModelRigid(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
void AlignLaraToRope(ITEM_INFO* l);
void LaraClimbRope(ITEM_INFO* item, COLL_INFO* coll);
void DrawRopeList();
void ProjectRopePoints(ROPE_STRUCT* Rope);
void init_all_ropes();
void SaveRope();
void LoadRope();
void StraightenRope(ITEM_INFO* item);
void _Straighten(ROPE_STRUCT* rope, PHD_VECTOR* pos, PHD_VECTOR* dir, long slength);

extern ROPE_STRUCT RopeList[8];
extern PENDULUM CurrentPendulum;
extern long nRope;
