#pragma once
#include "../global/types.h"

void inject_deltaPak();

void andrea1_init();
void andrea1_control();
void andrea1_end();
void andrea2_init();
void andrea2_control();
void andrea2_end();
void andrea3_init();
void andrea3_control();
void andrea3_end();
void andrea3b_init();
void andrea3b_control();
void andrea3b_end();
void andrea4_init();
void andrea4_control();
void andrea4_end();
void cranecut_init();
void cranecut_control();
void cranecut_end();
void richcut1_init();
void richcut1_control();
void richcut1_end();
void richcut2_init();
void richcut2_control();
void richcut2_end();
void richcut3_init();
void richcut3_control();
void richcut3_end();
void richcut4_init();
void richcut4_control();
void richcut4_end();
void andy1_init();
void andy1_control();
void andy1_end();
void andy2_init();
void andy2_control();
void andy2_end();
void andy3_init();
void andy3_control();
void andy3_end();
void andy4_init();
void andy4_control();
void andy4_end();
void andy4b_init();
void andy4b_control();
void andy4b_end();
void andy5_init();
void andy5_control();
void andy6_init();
void andy6_control();
void andy6_end();
void andy7_init();
void andy7_control();
void andy7_end();
void andy8_init();
void andy8_control();
void andy8_end();
void andy9_init();
void andy9_control();
void andy9_end();
void andy10_init();
void andy10_control();
void andy10_end();
void andy11_init();
void andy11_control();
void andy11_end();
void andypew_init();
void andypew_control();
void andypew_end();
void cossack_init();
void cossack_control();
void cossack_end();
void monk2_init();
void monk2_control();
void monk2_end();
void swampy_init();
void swampy_control();
void swampy_end();
void hamgate_init();
void hamgate_control();
void hamgate_end();
void stealth3_start();
void stealth3_end();
void joby2_init();
void joby2_control();
void joby2_end();
void joby3_init();
void joby3_control();
void joby3_end();
void joby4_init();
void joby4_control();
void joby4_end();
void joby5_init();
void joby5_control();
void joby5_end();
void joby6_init();
void joby6_control();
void joby6_end();
void joby7_init();
void joby7_control();
void joby7_end();
void joby8_init();
void joby8_control();
void joby8_end();
void joby9_init();
void joby9_control();
void joby9_end();
void joby10_init();
void joby10_control();
void joby10_end();
void special1_init();
void special1_control();
void special1_end();
void special2_init();
void special2_control();
void special2_end();
void special3_init();
void special3_control();
void special3_end();
void special4_init();
void special4_control();
void special4_end();
void handle_cutseq_triggering(int name);
void cutseq_givelara_pistols();
void cutseq_removelara_pistols();
void do_pierre_gun_meshswap();
void do_catapult_meshswap();
void do_clanger_meshswap();
void do_hammer_meshswap();
void setup_preist_meshswap();
void do_chalk_meshswap();//optimized out
void cutseq_shoot_pistols(int left_or_right);
ITEM_INFO* ResetCutanimate(int objnum);
void Cutanimate(int objnum);
void cutseq_removelara_hk();
void cutseq_givelara_hk();
void TriggerActorBlood(int actornum, unsigned long nodenum, struct PHD_VECTOR* pos, int direction, int speed);
void finish_cutseq(int name);
void trigger_weapon_dynamics(int left_or_right);
ITEM_INFO* find_a_fucking_item(int object_number);
void TriggerDelSmoke(long x, long y, long z, int sizeme);
void TriggerDelBrownSmoke(long x, long y, long z);
void DelTorchFlames(PHD_VECTOR* pos);
void trigger_title_spotcam(int num);
void CutLaraBubbles();
void deal_with_pistols(unsigned short* shootdata);
void handle_lara_chatting(short* _ranges);
void handle_actor_chatting(int speechslot, int node, int slot, int objslot, short* _ranges);
void trigger_item_in_room(short room_number, int object);
void untrigger_item_in_room(short room_number, int object);
void deal_with_actor_shooting(unsigned short* shootdata, int actornum, int nodenum, PHD_VECTOR* pos);
void DelsHandyTeleportLara(int x, int y, int z, int yrot);
void InitPackNodes(NODELOADHEADER* lnode, PACKNODE* pnode, char* packed, int numnodes);
int GetTrackWord(unsigned long off, char* packed, unsigned char packmethod);
int DecodeTrack(char* packed, RTDECODE* decode);
void DecodeAnim(PACKNODE* node, int num_nodes, int frame, unsigned short flags);
void do_new_cutscene_camera();


#define GetActorJointAbsPosition ((void(__cdecl*)(int, unsigned long, PHD_VECTOR*)) 0x004243A0 )
#define	cutseq_kill_item	( (void(__cdecl*)(int)) 0x00422A20 )
#define Load_and_Init_Cutseq	( (int(__cdecl*)(int)) 0x00422B90 )
#define	cutseq_restore_item	( (ITEM_INFO*(__cdecl*)(int)) 0x00422AF0 )
#define GrabActorMatrix	( (void(__cdecl*)(int, int, D3DMATRIX*)) 0x00424080 )
#define trig_actor_gunflash	( (void(__cdecl*)(D3DMATRIX*, PHD_VECTOR*)) 0x00485EC0 )//somewhere in specific I think? only ever called in deal_with_actor_shooting anyway so keeping it here for now


enum cutscenes
{
	CUT_NULL,
	CUT_STEALTH3_1,
	CUT_STEALTH3_2,
	CUT_STEALTH3_3,
	CUT_STEALTH3_4,
	CUT_JOBY6,
	CUT_ANDY5,
	CUT_ANDREA3B,
	CUT_ANDREA3,
	CUT_ANDY4B,
	CUT_ANDY4,
	CUT_RICHCUT4,
	CUT_JOBY10,
	CUT_JOBY9,
	CUT_ANDY3,
	CUT_JOBY5,
	CUT_ANDREA2,
	CUT_ANDREA1,
	CUT_JOBY4,
	CUT_ANDY2,
	CUT_RICH1,
	CUT_ANDY1,
	CUT_JOBY_CUT_3,
	CUT_RICH_CUT_3,
	CUT_RICH_CUT_1,
	CUT_JOBY_CUT_2,
	CUT_RICH_CUT_2,
	CUT_JOBY_CRANE_CUT,
	CUT_SPECIAL1,
	CUT_SPECIAL2,
	CUT_SPECIAL3,
	CUT_SPECIAL4,
	CUT_JOBY8,
	CUT_ANDY6,
	CUT_ANDYPEW,
	CUT_ANDY7,
	CUT_COSSACK,
	CUT_ANDY9,
	CUT_ANDY8,
	CUT_ANDY10,
	CUT_JOBY7,
	CUT_ANDREA4,
	CUT_MONK2,
	CUT_SWAMPY,
	CUT_ANDY11,
};
