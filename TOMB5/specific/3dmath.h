#pragma once
#include "../global/vars.h"

void inject_3dmath(bool replace);

void AlterFOV(short fov);
void gte_sttr(PHD_VECTOR* vec);
void aInitMatrix();
void aSetViewMatrix();
void aSetTrans(long x, long y, long z);
void aTranslateAbs(long x, long y, long z);
void aUnitMatrixByMat(float* matrix);
void aPushUnitMatrix();
long aTranslateRel(long x, long y, long z);
void aRotX(short angle);
void aRotY(short angle);
void aRotZ(short angle);
void aRotYXZPack(long angles);
void aRotYXZ(short y, short x, short z);
void phd_PushMatrix();
void phd_SetTrans(long x, long y, long z);
void phd_PushUnitMatrix();
long phd_TranslateRel(long x, long y, long z);
void phd_RotX(short angle);
void phd_RotY(short angle);
void phd_RotZ(short angle);
void phd_RotYXZpack(long angles);
void phd_RotYXZ(short y, short x, short z);
void phd_TranslateAbs(long x, long y, long z);
void phd_GetVectorAngles(long x, long y, long z, short* angles);
void phd_TransposeMatrix();
void phd_LookAt(long xsrc, long ysrc, long zsec, long xtar, long ytar, long ztar, short roll);
void aLookAt(float xsrc, float ysrc, float zsrc, float xtar, float ytar, float ztar, long roll);
void aOuterProduct(FVECTOR* v1, FVECTOR* v2, FVECTOR* dest);
void aVectorNormal(FVECTOR* s, FVECTOR* v);
void aPerpVectors(FVECTOR* a, FVECTOR* b, FVECTOR* c);
void aPointCameraByVector(float* mx, FCAMERA* cam);
void aPointCamera(FCAMERA* cam);
void aScaleCurrentMatrix(PHD_VECTOR* vec);
void ScaleCurrentMatrix(PHD_VECTOR* vec);
void SetupZRange(long znear, long zfar);
void InitWindow(long x, long y, long w, long h, long znear, long zfar, long fov, long a, long b);
long phd_atan(long x, long y);
ulong phd_sqrt(ulong num);

#define mGetAngle	( (long(__cdecl*)(long, long, long, long)) 0x0048F290 )
#define phd_GenerateW2V	( (void(__cdecl*)(PHD_3DPOS*)) 0x0048F330 )
