#pragma once
#include "../global/vars.h"

void inject_camera();

void InitialiseCamera();
void CalculateCamera();
void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness);
void ScreenShake(ITEM_INFO* item, short MaxVal, short MaxDist);
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);
long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst);
void UpdateCameraElevation();
void ConfirmCameraTargetPos();

#define BinocularCamera	( (void(__cdecl*)(ITEM_INFO*)) 0x0040FC20 )
#define FixedCamera	( (void(__cdecl*)()) 0x0040E890 )
#define ChaseCamera	( (void(__cdecl*)(ITEM_INFO*)) 0x0040D150 )
#define	LookCamera	( (void(__cdecl*)(ITEM_INFO*)) 0x0040DC10 )
#define CombatCamera	( (void(__cdecl*)(ITEM_INFO*)) 0x0040D640 )
