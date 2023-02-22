#pragma once
#include "../global/types.h"

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
void mRotBoundingBoxNoPersp(short* bounds, short* rotatedBounds);
void PrintRooms(short room_number);
void DrawStaticObjects(short room_number);
void InterpolateMatrix();
void InterpolateArmMatrix(long* mx);
void aInterpolateArmMatrix(float* mx);
void DrawEffect(short fx_num);
void calc_animating_item_clip_window(ITEM_INFO* item, short* bounds);
void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom);
void GetRoomBounds();

extern STATIC_INFO static_objects[70];

extern float* aIMXPtr;
extern float aIFMStack[768];
extern long* IMptr;
extern long IMstack[768];
extern long IM_rate;
extern long IM_frac;

extern long CurrentRoom;
extern long outside;
extern short SkyPos;
extern short SkyPos2;
extern ushort LightningRGB[3];
extern  ushort LightningRGBs[3];
extern short no_rotation[12];
