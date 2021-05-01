#pragma once

void inject_draw();


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);

#define	mRotBoundingBoxNoPersp	( (void(__cdecl*)(short*, short*)) 0x00402428 )
#define GetFrames	( (long(__cdecl*)(ITEM_INFO*, short*[], int*)) 0x0042CEB0 )
#define gar_RotYXZsuperpack	( (void(__cdecl*)(short**, long)) 0x0042C310 )
#define DrawPhaseGame	( (int(__cdecl*)()) 0x0042A400 )
#define SkyDrawPhase	( (void(__cdecl*)()) 0x0042A4A0 )
