#pragma once
#include "../global/vars.h"

extern short SPxzoffs[8];
extern short SPyoffs[8];

void inject_traps(bool replace);

void LaraBurn();
void LavaBurn(ITEM_INFO* item);
void ControlExplosion(short item_number);
void CloseTrapDoor(ITEM_INFO* item);
void DartEmitterControl(short item_number);
void DartsControl(short item_number);
void FlameEmitterControl(short item_number);
void FlameEmitter2Control(short item_number);
void FlameEmitter3Control(short item_number);
void FlameControl(short fx_number);
void RollingBallCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void ControlRollingBall(short item_number);
void TrapDoorControl(short item_number);
void FloorTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CeilingTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FallingBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DrawScaledSpike(ITEM_INFO* item);
void FallingBlock(short item_number);
void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* height);
void FallingBlockCeiling(ITEM_INFO* item, long x, long y, long z, long* height);
void ControlScaledSpike(short item_number);
void ControlRaisingBlock(short item_number);
void ControlTwoBlockPlatform(short item_number);
void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height);
void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height);
void FallingCeiling(short item_number);

#define OpenTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x004890C0 )
#define TestBoundsCollideTeethSpikes	( (long(__cdecl*)(ITEM_INFO*)) 0x0048BD90 )
#define OnTwoBlockPlatform	( (long(__cdecl*)(ITEM_INFO*, long, long)) 0x0048BAA0 )
