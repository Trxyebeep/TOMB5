#pragma once
#include "../global/types.h"

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
void LookCamera(ITEM_INFO* item);

extern CAMERA_INFO camera;
extern SHATTER_ITEM ShatterItem;
extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;
extern PHD_VECTOR CamOldPos;
extern camera_type BinocularOldCamera;
extern long BinocularOn;
extern long BinocularRange;
extern long ExittingBinos;
extern long LaserSight;
extern long InfraRed;
extern char SniperOverlay;
extern char SniperCamActive;
extern uchar WeaponDelay;
