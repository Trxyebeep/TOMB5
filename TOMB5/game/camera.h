#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();
void CalculateCamera();
void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness);
void ScreenShake(ITEM_INFO* item, short MaxVal, short MaxDist);
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);
long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst);
void UpdateCameraElevation();
void ConfirmCameraTargetPos();
void FixedCamera();
void ChaseCamera(ITEM_INFO* item);
void CombatCamera(ITEM_INFO* item);
void MoveCamera(GAME_VECTOR* ideal, long speed);
void BinocularCamera(ITEM_INFO* item);

#define	LookCamera	( (void(__cdecl*)(ITEM_INFO*)) 0x0040DC10 )
