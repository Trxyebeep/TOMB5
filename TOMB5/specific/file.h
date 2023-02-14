#pragma once
#include "../global/types.h"

FILE* FileOpen(const char* Filename);
void FileClose(FILE* fp);
long FileSize(FILE* fp);
long LoadFile(const char* name, char** dest);
bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size);
void AdjustUV(long num);
void DoMonitorScreen();
void S_GetUVRotateTextures();
void FreeLevel();
bool LoadTextures(long RTPages, long OTPages, long BTPages);
bool LoadRooms();
bool LoadObjects();
bool LoadSprites();
bool LoadCameras();
bool LoadSoundEffects();
bool LoadBoxes();
bool LoadAnimatedTextures();
bool LoadTextureInfos();
bool LoadItems();
bool LoadAIInfo();
bool LoadCinematic();
bool LoadSamples();
long S_LoadLevelFile(long num);

extern TEXTURESTRUCT* textinfo;
extern SPRITESTRUCT* spriteinfo;
extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern AIOBJECT* AIObjects;
extern THREAD LevelLoadingThread;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long number_cameras;
extern long nAnimUVRanges;
extern short nAIObjects;

extern TEXTURESTRUCT* AnimatingWaterfalls[6];
extern float AnimatingWaterfallsV[6];
