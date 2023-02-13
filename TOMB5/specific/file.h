#pragma once
#include "../global/types.h"

void inject_file(bool replace);

bool LoadTextureInfos();
bool LoadRooms();
FILE* FileOpen(const char* Filename);
bool FindCDDrive();
void FileClose(FILE* fp);
long FileSize(FILE* fp);
bool LoadItems();
bool LoadSprites();
bool LoadCameras();
bool LoadSoundEffects();
bool LoadAnimatedTextures();
bool LoadObjects();
bool LoadCinematic();
bool LoadSamples();
bool LoadAIInfo();
long S_LoadLevelFile(long num);
bool LoadTextures(long RTPages, long OTPages, long BTPages);
void S_GetUVRotateTextures();
void DoMonitorScreen();
void S_LoadLevel();
void LoadMapFile();
bool LoadBoxes();
void AdjustUV(long num);
long LoadFile(const char* name, char** dest);
void FreeLevel();
bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size);

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
extern char* FileData;
extern char* MapData;
extern long number_cameras;
extern long nAnimUVRanges;
extern short nAIObjects;

extern TEXTURESTRUCT* AnimatingWaterfalls[6];
extern float AnimatingWaterfallsV[6];
