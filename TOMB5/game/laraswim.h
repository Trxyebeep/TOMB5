#pragma once
#include "../global/types.h"


void inject_laraswim();


void LaraTestWaterDepth(ITEM_INFO* item, COLL_INFO* coll);
void LaraSwimCollision(ITEM_INFO* item, COLL_INFO* coll);
void SwimTurn(ITEM_INFO* item);
void SwimTurnSubsuit(ITEM_INFO* item);
void UpdateSubsuitAngles();
//LaraUnderWater
void lara_as_swimcheat(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll);
//GetWaterDepth
//LaraWaterCurrent

#define LaraUnderWater	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x004586F0 )
#define GetWaterDepth	( (long(__cdecl*)(long, long, long, short)) 0x004596D0 )
#define LaraWaterCurrent	( (void(__cdecl*)(COLL_INFO*)) 0x004598F0 )
