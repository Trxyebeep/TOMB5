#pragma once
#include "../global/vars.h"

void inject_coll(bool replace);

void TriggerLaraBlood();
void GetCollisionInfo(COLL_INFO* coll, long xpos, long ypos, long zpos, short room_number, long objheight);
long FindGridShift(long src, long dst);
void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
short GetTiltType(FLOOR_INFO* floor, long x, long y, long z);
void GenericSphereBoxCollision(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll);
void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CreatureCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void AIPickupCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void TrapCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void TestForObjectOnLedge(ITEM_INFO* item, COLL_INFO* coll);
long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems, MESH_INFO** StoredStatics, long StoreLara);
long MoveLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l);
long Move3DPosTo3DPos(PHD_3DPOS* pos, PHD_3DPOS* dest, long speed, short rotation);

#define CollideStaticObjects ( (long(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x00411DB0 )
#define	UpdateLaraRoom	( (void(__cdecl*)(ITEM_INFO*, long)) 0x004120F0 )
#define TestLaraPosition	( (long(__cdecl*)(short*, ITEM_INFO*, ITEM_INFO*)) 0x00413210 )
#define	TestBoundsCollide	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, long)) 0x00412CC0 )
#define	ItemPushLara	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, COLL_INFO*, long, long)) 0x00412860 )
#define LaraBaddieCollision	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00412170 )
#define AlignLaraPosition	( (void(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, ITEM_INFO*)) 0x004133C0 )
#define ItemPushLaraStatic	( (void(__cdecl*)(ITEM_INFO*, short*, PHD_3DPOS*, COLL_INFO*)) 0x00412F20 )
