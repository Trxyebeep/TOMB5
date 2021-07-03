#pragma once
#include "../global/vars.h"

void inject_control();

long ControlPhase(long _nframes, int demo_mode);
int GetRandomControl();
void SeedRandomControl(long seed);
int GetRandomDraw();
void SeedRandomDraw(long seed);
int GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
int CheckGuardOnTrigger();
void InterpolateAngle(short dest, short* src, short* diff, short speed);
void TranslateItem(ITEM_INFO* item, short x, short y, short z);
void InitCutPlayed();
void SetCutPlayed(int num);
void SetCutNotPlayed(int num);
int CheckCutPlayed(int num);
void NeatAndTidyTriggerCutscene(int value, int timer);
int is_object_in_room(int roomnumber, int objnumber);
int check_xray_machine_trigger();
long GetHeight(FLOOR_INFO* floor, long x, long y, long z);
long GetCeiling(FLOOR_INFO* floor, long x, long y, long z);
FLOOR_INFO* GetFloor(int x, int y, int z, short* room_number);
int ExplodeItemNode(ITEM_INFO* item, int Node, int NoXZVel, long bits);
short GetDoor(FLOOR_INFO* floor);
int LOS(GAME_VECTOR* start, GAME_VECTOR* target);
int xLOS(GAME_VECTOR* start, GAME_VECTOR* target);
int zLOS(GAME_VECTOR* start, GAME_VECTOR* target);
int ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target);
int GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, int DrawTarget, int firing);
int ObjectOnLOS2(GAME_VECTOR* start, GAME_VECTOR* target, PHD_VECTOR* Coord, MESH_INFO** StaticMesh);
void TestTriggers(short* data, int heavy, int HeavyFlags);
void _TestTriggers(short* data, int heavy, int HeavyFlags);

extern uchar ShatterSounds[18][10];

#define UpdateSky	( (void(__cdecl*)()) 0x00414720 )
#define ClearDynamics	( (void(__cdecl*)()) 0x00431530 )
#define ClearFires	( (void(__cdecl*)()) 0x00481B10 )
#define KillMoveEffects	( (void(__cdecl*)()) 0x004146A0 )
#define KillMoveItems	( (void(__cdecl*)()) 0x00414620 )
#define UpdatePulseColour	( (void(__cdecl*)()) 0x00480830 )
#define	CheckNoColFloorTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x00418C80 )
#define AnimateItem	( (void(__cdecl*)(ITEM_INFO*)) 0x00415300 )
#define	GetWaterHeight	( (long(__cdecl*)(long, long, long, short)) 0x00415DA0 )
#define TriggerActive	( (int(__cdecl*)(ITEM_INFO*)) 0x004175B0 )
#define	FlipMap	( (void(__cdecl*)(int)) 0x00418910 )
#define CheckNoColCeilingTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x00418D60 )
#define FireCrossBowFromLaserSight	( (void(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x0041A0B0 )
#define DoRayBox	( (int(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, short*, PHD_3DPOS*, PHD_VECTOR*, short)) 0x004193C0 )
#define RefreshCamera	( (void(__cdecl*)(short, short*)) 0x004165E0 )
#define TriggerCDTrack	( (void(__cdecl*)(short, short, short)) 0x00418B90 )
#define ResetGuards	( (void(__cdecl*)()) 0x0041AF10 )
#define IsRoomOutside	( (int(__cdecl*)(long, long, long)) 0x00418E90 )
