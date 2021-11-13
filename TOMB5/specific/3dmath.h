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

#define ScaleCurrentMatrix	( (void(__cdecl*)(VECTOR*)) 0x0048EFF0 )
#define mGetAngle	( (long(__cdecl*)(long, long, long, long)) 0x0048F290 )
#define phd_GenerateW2V	( (void(__cdecl*)(PHD_3DPOS*)) 0x0048F330 )
#define phd_LookAt	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x0048F760 )
#define phd_atan	( (long(__cdecl*)(long, long)) 0x0048F8A0 )
#define phd_sqrt	( (long(__cdecl*)(long)) 0x0048F980 )
#define phd_PushMatrix	( (void(__cdecl*)()) 0x0048F9C0 )
#define	phd_SetTrans	( (void(__cdecl*)(long, long, long)) 0x0048FA40 )
#define phd_PushUnitMatrix	( (void(__cdecl*)()) 0x0048FA90 )
#define phd_TranslateRel	( (void(__cdecl*)(long, long, long)) 0x0048FB20 )
#define phd_RotX	( (void(__cdecl*)(short)) 0x0048FBE0 )
#define phd_RotY	( (void(__cdecl*)(short)) 0x0048FCD0 )
#define phd_RotZ	( (void(__cdecl*)(short)) 0x0048FDC0 )
#define	phd_RotYXZ	( (void(__cdecl*)(short, short, short)) 0x00490150 )
#define phd_TranslateAbs	( (void(__cdecl*)(long, long, long)) 0x004903F0 )
#define	phd_GetVectorAngles	( (void(__cdecl*)(long, long, long, short*)) 0x004904B0 )
#define aLookAt	( (void(__cdecl*)(float, float, float, float, float, float, int)) 0x00490C20 )
#define phd_RotYXZpack	( (void(__cdecl*)(long)) 0x0048FEB0 )
