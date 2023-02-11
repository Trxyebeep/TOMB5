#pragma once
#include "../global/vars.h"

void inject_control(bool replace);

long ControlPhase(long nframes, long demo_mode);
long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
long CheckGuardOnTrigger();
void InterpolateAngle(short dest, short* src, short* diff, short speed);
void TranslateItem(ITEM_INFO* item, short x, short y, short z);
void InitCutPlayed();
void SetCutPlayed(long num);
void SetCutNotPlayed(long num);
long CheckCutPlayed(long num);
void NeatAndTidyTriggerCutscene(long value, long timer);
long is_object_in_room(long roomnumber, long objnumber);
long check_xray_machine_trigger();
long GetHeight(FLOOR_INFO* floor, long x, long y, long z);
long GetCeiling(FLOOR_INFO* floor, long x, long y, long z);
FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number);
long ExplodeItemNode(ITEM_INFO* item, long Node, long NoXZVel, long bits);
short GetDoor(FLOOR_INFO* floor);
long LOS(GAME_VECTOR* start, GAME_VECTOR* target);
long xLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long zLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target);
long GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, long DrawTarget, long firing);
long ObjectOnLOS2(GAME_VECTOR* start, GAME_VECTOR* target, PHD_VECTOR* Coord, MESH_INFO** StaticMesh);
void TestTriggers(short* data, long heavy, long HeavyFlags);
void _TestTriggers(short* data, long heavy, long HeavyFlags);
void FlipMap(long FlipNumber);
void RemoveRoomFlipItems(ROOM_INFO* r);
void AddRoomFlipItems(ROOM_INFO* r);
void RefreshCamera(short type, short* data);
#ifdef GENERAL_FIXES
long S_Death();
#endif
void KillMoveItems();
void KillMoveEffects();
void UpdateSky();
void AlterFloorHeight(ITEM_INFO* item, long height);
long GetWaterHeight(long x, long y, long z, short room_number);
long TriggerActive(ITEM_INFO* item);
long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z);
long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z);
void FireCrossBowFromLaserSight(GAME_VECTOR* start, GAME_VECTOR* target);
void TriggerCDTrack(short value, short flags, short type);
void TriggerNormalCDTrack(short value, short flags, short type);
long IsRoomOutside(long x, long y, long z);
void AnimateItem(ITEM_INFO* item);
long RayBoxIntersect(PHD_VECTOR* min, PHD_VECTOR* max, PHD_VECTOR* origin, PHD_VECTOR* dir, PHD_VECTOR* Coord);
long DoRayBox(GAME_VECTOR* start, GAME_VECTOR* target, short* bounds, PHD_3DPOS* ItemPos, PHD_VECTOR* Coord, short item_number);
void ResetGuards();

extern uchar ShatterSounds[18][10];
