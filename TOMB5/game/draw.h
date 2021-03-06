#pragma once
#include "../global/vars.h"

void inject_draw(bool replace);


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);
void InitInterpolate(long frac, long rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(long x, long y, long z);
void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip);
void gar_RotYXZsuperpack(short** pprot, long skip);
void phd_PutPolygons_I(short* ptr, long clip);
void aInterpolateMatrix();
long DrawPhaseGame();
void SkyDrawPhase();
void UpdateSkyLightning();
void CalculateObjectLighting(ITEM_INFO* item, short* frame);
void DrawAnimatingItem(ITEM_INFO* item);
void PrintObjects(short room_number);
void DrawRooms(short current_room);
void CalculateObjectLightingLara();
long GetFrames(ITEM_INFO* item, short* frm[], long* rate);
void SetupSkelebobMeshswaps();
void RestoreLaraMeshswaps();
void RenderIt(short current_room);

extern short no_rotation[];

#define InterpolateMatrix	( (void(__cdecl*)()) 0x0042C8F0 )
#define	mRotBoundingBoxNoPersp	( (void(__cdecl*)(short*, short*)) 0x0042E240 )
#define GetRoomBounds	( (void(__cdecl*)()) 0x0042D4F0 )
#define InterpolateArmMatrix	( (void(__cdecl*)(long*)) 0x0042CC10 )
#define aInterpolateArmMatrix	( (void(__cdecl*)(float*)) 0x0042C790 )
#define calc_animating_item_clip_window	( (void(__cdecl*)(ITEM_INFO*, short*)) 0x0042B4C0 )
#define DrawStaticObjects	( (void(__cdecl*)(short)) 0x0042D060 )
#define DrawEffect	( (void(__cdecl*)(short)) 0x0042B340 )
#define PrintRooms	( (void(__cdecl*)(short)) 0x0042E1C0 )
