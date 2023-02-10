#pragma once
#include "../global/vars.h"

void inject_box(bool replace);

void InitialiseCreature(short item_number);
long CreatureActive(short item_number);
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info);
long SearchLOT(LOT_INFO* LOT, long expansion);
long UpdateLOT(LOT_INFO* LOT, long expansion);
void TargetBox(LOT_INFO* LOT, short box_number);
long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
long ValidBox(ITEM_INFO* item, short zone_number, short box_number);
long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT);
void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
long CreatureCreature(short item_number);
long BadFloor(long x, long y, long z, long box_height, long next_height, short room_number, LOT_INFO* LOT);
void DropBaddyPickups(ITEM_INFO* item);
void CreatureDie(short item_number, long explode);
long CreatureAnimation(short item_number, short angle, short tilt);
short CreatureTurn(ITEM_INFO* item, short maximum_turn);
void CreatureTilt(ITEM_INFO* item, short angle);

#define AlertNearbyGuards	( (void(__cdecl*)(ITEM_INFO*)) 0x0040BB10 )
#define GetAITarget	( (void(__cdecl*)(CREATURE_INFO*)) 0x0040BCC0 )
#define CreatureJoint	( (void(__cdecl*)(ITEM_INFO*, short, short)) 0x0040B240 )
typedef short(__cdecl* generate)(long, long, long, short, short, short);
#define CreatureEffectT	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, short, short, generate)) 0x0040B550 )
#define CreatureEffect	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, generate)) 0x0040B4D0 )
#define	CreatureUnderwater	( (void(__cdecl*)(ITEM_INFO*, long)) 0x0040B400 )
#define AlertAllGuards	( (void(__cdecl*)(short)) 0x0040BA70 )
#define AIGuard	( (short(__cdecl*)(CREATURE_INFO*)) 0x0040BBE0 )
#define CreatureVault	( (long(__cdecl*)(short, short, long, long)) 0x0040B5D0 )
