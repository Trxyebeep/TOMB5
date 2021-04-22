#pragma once

#define InitialiseCreature	( (void(__cdecl*)(short)) 0x00408550 )
#define CreatureActive	( (int(__cdecl*)(short)) 0x00408630 )
#define AlertNearbyGuards	( (void(__cdecl*)(ITEM_INFO*)) 0x0040BB10 )
#define CreatureDie	( (void(__cdecl*)(short, int)) 0x0040A090 )
#define CreatureAIInfo	( (void(__cdecl*)(ITEM_INFO*, AI_info*)) 0x004086C0 )
#define GetAITarget	( (void(__cdecl*)(creature_info*)) 0x0040BCC0 )
#define GetCreatureMood	( (void(__cdecl*)(ITEM_INFO*, AI_info*, int)) 0x004090A0 )
#define CreatureMood	( (void(__cdecl*)(ITEM_INFO*, AI_info*, int)) 0x00409370 )
#define CreatureTurn	( (short(__cdecl*)(ITEM_INFO*, short)) 0x0040AE90 )
#define CreatureTilt	( (void(__cdecl*)(ITEM_INFO*, short)) 0x0040B1B0 )
#define CreatureJoint	( (void(__cdecl*)(ITEM_INFO*, short, short)) 0x0040B240 )
#define CreatureAnimation	( (int(__cdecl*)(short, short, short)) 0x0040A1D0 )
typedef short(__cdecl* generate)(long, long, long, short, short, short);
typedef short(__cdecl* generate2)(long, long, long, short, int, int);
#define CreatureEffectT	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, short, short, generate)) 0x0040B550 )
#define CreatureEffect	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, generate2)) 0x0040B4D0 )
