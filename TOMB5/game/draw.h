#pragma once
#include "../global/vars.h"

void inject_draw(bool replace);


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);
void InitInterpolate(int frac, int rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(int x, int y, int z);
void phd_TranslateRel_ID(int x, int y, int z, int x2, int y2, int z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, int skip);
void gar_RotYXZsuperpack(short** pprot, int skip);
void phd_PutPolygons_I(short* ptr, int clip);
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

extern short no_rotation[];

#define InterpolateMatrix	( (void(__cdecl*)()) 0x0042C8F0 )
#define	mRotBoundingBoxNoPersp	( (void(__cdecl*)(short*, short*)) 0x0042E240 )
#define GetRoomBounds	( (void(__cdecl*)()) 0x0042D4F0 )
#define InterpolateArmMatrix	( (void(__cdecl*)(long*)) 0x0042CC10 )
#define aInterpolateArmMatrix	( (void(__cdecl*)(float*)) 0x0042C790 )
#define calc_animating_item_clip_window	( (void(__cdecl*)(ITEM_INFO*, short*)) 0x0042B4C0 )
#define DrawStaticObjects	( (void(__cdecl*)(short)) 0x0042D060 )
#define DrawEffect	( (void(__cdecl*)(short)) 0x0042B340 )
