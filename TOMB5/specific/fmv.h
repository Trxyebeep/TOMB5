#pragma once
#include "../global/types.h"

void inject_fmv(bool replace);

#ifdef GENERAL_FIXES
bool LoadBinkStuff();
#endif
void ShowBinkFrame();
long PlayFmv(long num);
long PlayFmvNow(long num, long u);
