#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
