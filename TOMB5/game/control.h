#pragma once
#include "../global/types.h"

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
int GetHeight(FLOOR_INFO* floor, int x, int y, int z);
FLOOR_INFO* GetFloor(int x, int y, int z, short* room_number);
int ExplodeItemNode(ITEM_INFO* item, int Node, int NoXZVel, long bits);
short GetDoor(FLOOR_INFO* floor);

#define	GetTargetOnLOS	( (int(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, int, int)) 0x0041A170 )
#define ObjectOnLOS2	( (int(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, PHD_VECTOR*, MESH_INFO**)) 0x00419110 )
#define GetCeiling ( (short(__cdecl*)(FLOOR_INFO*, int, int, int)) 0x00417640 )
#define UpdateSky	( (void(__cdecl*)()) 0x00414720 )
#define ClearDynamics	( (void(__cdecl*)()) 0x00431530 )
#define ClearFires	( (void(__cdecl*)()) 0x00481B10 )
#define KillMoveEffects	( (void(__cdecl*)()) 0x004146A0 )
#define KillMoveItems	( (void(__cdecl*)()) 0x00414620 )
#define TestTriggers	( (void(__cdecl*)(short*, int, int)) 0x00416760 )
#define UpdatePulseColour	( (void(__cdecl*)()) 0x00480830 )
#define	CheckNoColFloorTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x00418C80 )
#define	LOS	( (int(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x00417CF0 )
#define AnimateItem	( (void(__cdecl*)(ITEM_INFO*)) 0x00415300 )
#define	GetWaterHeight	( (long(__cdecl*)(long, long, long, short)) 0x00415DA0 )
#define TriggerActive	( (int(__cdecl*)(ITEM_INFO*)) 0x004175B0 )
#define	FlipMap	( (void(__cdecl*)(int)) 0x00418910 )
#define CheckNoColCeilingTriangle	( (long(__cdecl*)(FLOOR_INFO*, long, long)) 0x00418D60 )
