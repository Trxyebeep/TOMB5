#pragma once
#include "../global/types.h"

void inject_laraclmb();

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
static long LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbend(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll);
#define	LaraDoClimbLeftRight	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*, int, int)) 0x004515A0 )
#define LaraTestClimbPos	( (int(__cdecl*)(ITEM_INFO*, int, int, int, int, int*)) 0x00450970 )
#define	LaraTestClimbUpPos	( (int(__cdecl*)(ITEM_INFO*, int, int, int*, int*)) 0x00451200 )
