#pragma once

void inject_draw();


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);

#define	mRotBoundingBoxNoPersp	( (void(__cdecl*)(short*, short*)) 0x00402428 )
#define GetFrames	( (long(__cdecl*)(ITEM_INFO*, short*[], int*)) 0x0042CEB0 )

