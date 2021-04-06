#pragma once

void inject_draw();


short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);

#define GetFrames	( (long(__cdecl*)(ITEM_INFO*, short*[], int*)) 0x0042CEB0 )
