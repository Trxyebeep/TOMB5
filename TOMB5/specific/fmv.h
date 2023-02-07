#pragma once
#include "../global/vars.h"

void inject_fmv(bool replace);

#ifdef GENERAL_FIXES
bool LoadBinkStuff();
#endif
void ShowBinkFrame();
long PlayFmv(long num);
long PlayFmvNow(long num, long u);
